#include <metal_stdlib>
#include "../misc/constants.h"
using namespace metal;


kernel void add_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                        texture2d<float, access::read_write> out_texture [[texture(1)]],
                        constant float& n_textures [[buffer(0)]],
                        uint2 gid [[thread_position_in_grid]]) {
    
    float const color_value = out_texture.read(gid).r + in_texture.read(gid).r/n_textures;
    
    out_texture.write(color_value, gid);
}


kernel void add_texture_exposure(texture2d<float, access::read> in_texture [[texture(0)]],
                                 texture2d<float, access::read> in_texture_blurred [[texture(1)]],
                                 texture2d<float, access::read> weight_highlights_texture [[texture(2)]],
                                 texture2d<float, access::read_write> out_texture [[texture(3)]],
                                 texture2d<float, access::read_write> norm_texture [[texture(4)]],
                                 constant int& exposure_bias [[buffer(0)]],
                                 constant float& white_level [[buffer(1)]],
                                 constant float& black_level_mean [[buffer(2)]],
                                 constant float& color_factor_mean [[buffer(3)]],
                                 uint2 gid [[thread_position_in_grid]]) {
       
    // calculate weight based on exposure bias
    float weight_exposure = pow(2.0f, float(exposure_bias/100.0f));
    
    // extract pixel value
    float pixel_value = in_texture.read(gid).r;
    
    // adapt exposure weight based on the luminosity of each pixel relative to the white level
    float luminance = min(white_level, in_texture_blurred.read(gid).r/color_factor_mean);
    luminance = (luminance-black_level_mean)/white_level;
    
    // shadows get the exposure-dependent weight for optimal noise reduction while midtones and highlights have a reduced weight for better motion robustness
    // between 0.25 and 1.00 of the white level (based on pixel values after exposure correction), the weight becomes 1.0    
    weight_exposure = max(sqrt(weight_exposure), weight_exposure * pow(weight_exposure, -0.5f/(0.25f-black_level_mean/white_level)*luminance));
   
    // ensure smooth blending for pixel values between 0.25 and 0.99 of the white level (based on pixel values before exposure correction)
    float const weight_highlights = weight_highlights_texture.read(gid).r;
       
    // apply optimal weight based on exposure of pixel and take into account weight based on the pixel intensity
    pixel_value = weight_exposure*weight_highlights * pixel_value;
    
    out_texture.write(out_texture.read(gid).r + pixel_value, gid);
    
    norm_texture.write(norm_texture.read(gid).r + weight_exposure*weight_highlights, gid);
}


kernel void add_texture_highlights(texture2d<float, access::read> in_texture [[texture(0)]],
                                   texture2d<float, access::read_write> out_texture [[texture(1)]],
                                   constant float& white_level [[buffer(0)]],
                                   constant float& black_level_mean [[buffer(1)]],
                                   constant float& factor_red [[buffer(2)]],
                                   constant float& factor_blue [[buffer(3)]],
                                   uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_width  = in_texture.get_width();
    int texture_height = in_texture.get_height();
    
    int const x = gid.x*2;
    int const y = gid.y*2;
    
    float pixel_value4, pixel_value5, pixel_ratio4, pixel_ratio5, pixel_count, extrapolated_value, weight;
    
    // extract pixel values of 2x2 super pixel
    float pixel_value0 = in_texture.read(uint2(x  , y)).r;
    float pixel_value1 = in_texture.read(uint2(x+1, y)).r;
    float pixel_value2 = in_texture.read(uint2(x,   y+1)).r;
    float pixel_value3 = in_texture.read(uint2(x+1, y+1)).r;
    
    // calculate ratio of pixel value and white level
    float const pixel_ratio0 = (pixel_value0-black_level_mean)/(white_level-black_level_mean);
    float const pixel_ratio1 = (pixel_value1-black_level_mean)/(white_level-black_level_mean);
    float const pixel_ratio2 = (pixel_value2-black_level_mean)/(white_level-black_level_mean);
    float const pixel_ratio3 = (pixel_value3-black_level_mean)/(white_level-black_level_mean);
    
    // process first green channel if a bright pixel is detected
    if (pixel_ratio1 > 0.8f) {
                       
        pixel_value4 = pixel_value5 = 0.0f;
        pixel_ratio4 = pixel_ratio5 = 0.0f;
        pixel_count = 2.0f;
        
        // extract additional pixel close to the green pixel
        if (x+2 < texture_width) {
            pixel_value4 = in_texture.read(uint2(x+2, y)).r;
            pixel_ratio4 = (pixel_value4-black_level_mean)/(white_level-black_level_mean);
            pixel_count += 1.0f;
        }
        
        // extract additional pixel close to the green pixel
        if (y-1 >= 0) {
            pixel_value5 = in_texture.read(uint2(x+1, y-1)).r;
            pixel_ratio5 = (pixel_value5-black_level_mean)/(white_level-black_level_mean);
            pixel_count += 1.0f;
        }
        
        // if at least one surrounding pixel is above the normalized clipping threshold for the respective color channel
        if (pixel_ratio0 > 0.99f*factor_red || pixel_ratio3 > 0.99f*factor_blue || pixel_ratio4 > 0.99f*factor_red || pixel_ratio5 > 0.99f*factor_blue) {
            
            // extrapolate green pixel from surrounding red and blue pixels
            extrapolated_value = ((pixel_value0+pixel_value4)/factor_red + (pixel_value3+pixel_value5)/factor_blue)/pixel_count;
            
            // calculate weight for blending the extrapolated value and the original pixel value
            weight = 0.9f - 4.5f*clamp(1.0f-pixel_ratio1, 0.0f, 0.2f);
            
            pixel_value1 = weight*max(extrapolated_value, pixel_value1) + (1.0f-weight)*pixel_value1;
        }
    }
    
    // process second green channel if a bright pixel is detected
    if (pixel_ratio2 > 0.8f) {
           
        pixel_value4 = pixel_value5 = 0.0f;
        pixel_ratio4 = pixel_ratio5 = 0.0f;
        pixel_count = 2.0f;
        
        // extract additional pixel close to the green pixel
        if (x-1 >= 0) {
            pixel_value4 = in_texture.read(uint2(x-1, y+1)).r;
            pixel_ratio4 = (pixel_value4-black_level_mean)/(white_level-black_level_mean);
            pixel_count += 1.0f;
        }
        
        // extract additional pixel close to the green pixel
        if (y+2 < texture_height) {
            pixel_value5 = in_texture.read(uint2(x  , y+2)).r;
            pixel_ratio5 = (pixel_value5-black_level_mean)/(white_level-black_level_mean);
            pixel_count += 1.0f;
        }
        
        // if at least one surrounding pixel is above the normalized clipping threshold for the respective color channel
        if (pixel_ratio0 > 0.99f*factor_red || pixel_ratio3 > 0.99f*factor_blue || pixel_ratio5 > 0.99f*factor_red || pixel_ratio4 > 0.99f*factor_blue) {
            
            // extrapolate green pixel from surrounding red and blue pixels
            extrapolated_value = ((pixel_value0+pixel_value5)/factor_red + (pixel_value3+pixel_value4)/factor_blue)/pixel_count;
            
            // calculate weight for blending the extrapolated value and the original pixel value
            weight = 0.9f - 4.5f*clamp(1.0f-pixel_ratio2, 0.0f, 0.2f);
            
            pixel_value2 = weight*max(extrapolated_value, pixel_value2) + (1.0f-weight)*pixel_value2;
        }
    }
    
    pixel_value0 = out_texture.read(uint2(x  , y)).r   + pixel_value0;
    pixel_value1 = out_texture.read(uint2(x+1, y)).r   + pixel_value1;
    pixel_value2 = out_texture.read(uint2(x  , y+1)).r + pixel_value2;
    pixel_value3 = out_texture.read(uint2(x+1, y+1)).r + pixel_value3;
    
    out_texture.write(pixel_value0, uint2(x  , y));
    out_texture.write(pixel_value1, uint2(x+1, y));
    out_texture.write(pixel_value2, uint2(x  , y+1));
    out_texture.write(pixel_value3, uint2(x+1, y+1));
}


kernel void add_texture_uint16(texture2d<uint, access::read> in_texture [[texture(0)]],
                               texture2d<float, access::read_write> out_texture [[texture(1)]],
                               constant float& n_textures [[buffer(0)]],
                               uint2 gid [[thread_position_in_grid]]) {
    
    float const color_value = out_texture.read(gid).r + float(in_texture.read(gid).r)/n_textures;
    
    out_texture.write(color_value, gid);
}


kernel void add_texture_weighted(texture2d<float, access::read> texture1 [[texture(0)]],
                                 texture2d<float, access::read> texture2 [[texture(1)]],
                                 texture2d<float, access::read> weight_texture [[texture(2)]],
                                 texture2d<float, access::write> out_texture [[texture(3)]],
                                 uint2 gid [[thread_position_in_grid]]) {
    
    float const intensity1 = texture1.read(gid).r;
    float const intensity2 = texture2.read(gid).r;
    float const weight = weight_texture.read(gid).r;
    
    float const out_intensity = weight * intensity2 + (1 - weight) * intensity1;
    out_texture.write(out_intensity, gid);
}


kernel void blur_mosaic_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                                texture2d<float, access::write> out_texture [[texture(1)]],
                                constant int& kernel_size [[buffer(0)]],
                                constant int& mosaic_pattern_width [[buffer(1)]],
                                constant int& texture_size [[buffer(2)]],
                                constant int& direction [[buffer(3)]],
                                uint2 gid [[thread_position_in_grid]]) {
    
    // set kernel weights of binomial filter for identity operation
    float bw[9] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
    int kernel_size_trunc = kernel_size;
    
    // to speed up calculations, kernels are truncated in such a way that the total contribution of removed weights is smaller than 0.25%
    if (kernel_size== 1)      {bw[0]=    2; bw[1]=    1;}
    else if (kernel_size== 2) {bw[0]=    6; bw[1]=    4; bw[2]=   1;}
    else if (kernel_size== 3) {bw[0]=   20; bw[1]=   15; bw[2]=   6; bw[3]=   1;}
    else if (kernel_size== 4) {bw[0]=   70; bw[1]=   56; bw[2]=  28; bw[3]=   8; bw[4]=   1;}
    else if (kernel_size== 5) {bw[0]=  252; bw[1]=  210; bw[2]= 120; bw[3]=  45; bw[4]=  10; kernel_size_trunc=4;}
    else if (kernel_size== 6) {bw[0]=  924; bw[1]=  792; bw[2]= 495; bw[3]= 220; bw[4]=  66; bw[5]= 12; kernel_size_trunc=5;}
    else if (kernel_size== 7) {bw[0]= 3432; bw[1]= 3003; bw[2]=2002; bw[3]=1001; bw[4]= 364; bw[5]= 91; kernel_size_trunc=5;}
    else if (kernel_size== 8) {bw[0]=12870; bw[1]=11440; bw[2]=8008; bw[3]=4368; bw[4]=1820; bw[5]=560; bw[6]=120; kernel_size_trunc=6;}
    else if (kernel_size==16) {bw[0]=601080390; bw[1]=565722720; bw[2]=471435600; bw[3]=347373600; bw[4]=225792840; bw[5]=129024480; bw[6]=64512240; bw[7]=28048800; bw[8]=10518300; kernel_size_trunc=8;}
    
    // compute a single output pixel
    float total_intensity = 0.0f;
    float total_weight = 0.0f;
    float weight;
    
    // direction = 0: blurring in x-direction, direction = 1: blurring in y-direction
    uint2 xy;
    xy[1-direction] = gid[1-direction];
    int const i0 = gid[direction];
    
    for (int di = -kernel_size_trunc; di <= kernel_size_trunc; di++) {
        int i = i0 + mosaic_pattern_width*di;
        if (0 <= i && i < texture_size) {
           
            xy[direction] = i;
            weight = bw[abs(di)];
            total_intensity += weight * in_texture.read(xy).r;
            total_weight += weight;
        }
    }
    
    // write output pixel
    float const out_intensity = total_intensity / total_weight;
    out_texture.write(out_intensity, gid);
}


kernel void calculate_weight_highlights(texture2d<float, access::read> in_texture [[texture(0)]],
                                        texture2d<float, access::write> weight_highlights_texture [[texture(1)]],
                                        constant int& exposure_bias [[buffer(0)]],
                                        constant float& white_level [[buffer(1)]],
                                        constant float& black_level_mean [[buffer(2)]],
                                        constant int& kernel_size [[buffer(3)]],
                                        uint2 gid [[thread_position_in_grid]]) {
       
    // load args
    int texture_width  = in_texture.get_width();
    int texture_height = in_texture.get_height();
    
    // calculate weight based on exposure bias
    float const weight_exposure = pow(2.0f, float(exposure_bias/100.0f));
    
    // find the maximum intensity in a 5x5 window around the main pixel
    float pixel_value_max = 0.0f;
      
    for (int dy = -kernel_size; dy <= kernel_size; dy++) {
        int y = gid.y + dy;
        
        if (0 <= y && y < texture_height) {
            for (int dx = -kernel_size; dx <= kernel_size; dx++) {
                int x = gid.x + dx;
                
                if (0 <= x && x < texture_width) {
                    pixel_value_max = max(pixel_value_max, in_texture.read(uint2(x, y)).r);
                }
            }
        }
    }
    
    pixel_value_max = (pixel_value_max-black_level_mean)*weight_exposure + black_level_mean;

    // ensure smooth blending for pixel values between 0.25 and 0.99 of the white level (based on pixel values before exposure correction)
    float const weight_highlights = clamp(0.99f/0.74f-1.0f/0.74f*pixel_value_max/white_level, 0.0f, 1.0f);
      
    weight_highlights_texture.write(weight_highlights, gid);
}


kernel void convert_float_to_uint16(texture2d<float, access::read>  in_texture  [[texture(0)]],
                                    texture2d<uint,  access::write> out_texture [[texture(1)]],
                                    constant int& white_level           [[buffer(0)]],
                                    constant int& factor_16bit          [[buffer(1)]],
                                    constant int& mosaic_pattern_width  [[buffer(2)]],
                                    constant int* black_levels          [[buffer(3)]],
                                    uint2 gid [[thread_position_in_grid]]) {
    // load args
    float const black_level = black_levels[(gid.x % mosaic_pattern_width) + mosaic_pattern_width * (gid.y % mosaic_pattern_width)];

    // apply potential scaling to 16 bit and convert to integer
    int out_value = int(round(factor_16bit*(in_texture.read(gid).r - black_level) + black_level));
    out_value     = clamp(out_value, 0, min(white_level, int(UINT16_MAX_VAL)));
    
    // write back into texture
    out_texture.write(uint(out_value), gid);
}


kernel void convert_to_bayer(texture2d<float, access::read> in_texture [[texture(0)]],
                             texture2d<float, access::write> out_texture [[texture(1)]],
                             constant int& pad_left [[buffer(0)]],
                             constant int& pad_top [[buffer(1)]],
                             uint2 gid [[thread_position_in_grid]]) {
    
    int const x = gid.x*2;
    int const y = gid.y*2;
    
    float4 const color_value = in_texture.read(uint2(gid.x+pad_left, gid.y+pad_top));
     
    out_texture.write(color_value[0], uint2(x,   y));
    out_texture.write(color_value[1], uint2(x+1, y));
    out_texture.write(color_value[2], uint2(x,   y+1));
    out_texture.write(color_value[3], uint2(x+1, y+1));
}


kernel void convert_to_rgba(texture2d<float, access::read> in_texture [[texture(0)]],
                            texture2d<float, access::write> out_texture [[texture(1)]],
                            constant int& pad_left [[buffer(0)]],
                            constant int& pad_top [[buffer(1)]],
                            uint2 gid [[thread_position_in_grid]]) {
    
    int const x = gid.x*2 + pad_left;
    int const y = gid.y*2 + pad_top;
    
    float4 const color_value = float4(in_texture.read(uint2(x, y)).r,   in_texture.read(uint2(x+1, y)).r,
                                      in_texture.read(uint2(x, y+1)).r, in_texture.read(uint2(x+1, y+1)).r);
    
    out_texture.write(color_value, gid);
}


kernel void copy_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                         texture2d<float, access::write> out_texture [[texture(1)]],
                         uint2 gid [[thread_position_in_grid]]) {
    out_texture.write(in_texture.read(gid), gid);
}


kernel void crop_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                         texture2d<float, access::write> out_texture [[texture(1)]],
                         constant int& pad_left [[buffer(0)]],
                         constant int& pad_top [[buffer(1)]],
                         uint2 gid [[thread_position_in_grid]]) {
      
    int x = gid.x + pad_left;
    int y = gid.y + pad_top;
  
    float const color_value = in_texture.read(uint2(x, y)).r;
    out_texture.write(color_value, gid);
}


kernel void fill_with_zeros(texture2d<float, access::write> texture [[texture(0)]],
                            uint2 gid [[thread_position_in_grid]]) {
    texture.write(0, gid);
}


/**
 Hot pixel correction based on the idea that hot pixels appear at the same pixel location in all images
 */
kernel void find_hotpixels(texture2d<float, access::read> average_texture [[texture(0)]],
                           texture2d<float, access::write> hotpixel_weight_texture [[texture(1)]],
                           constant float* mean_texture_buffer [[buffer(0)]],
                           constant float& black_level0 [[buffer(1)]],
                           constant float& black_level1 [[buffer(2)]],
                           constant float& black_level2 [[buffer(3)]],
                           constant float& black_level3 [[buffer(4)]],
                           constant float& hot_pixel_threshold [[buffer(5)]],
                           constant float& hot_pixel_multiplicator [[buffer(6)]],
                           constant float& correction_strength [[buffer(7)]],
                           uint2 gid [[thread_position_in_grid]]) {
       
    int const x = gid.x+2;
    int const y = gid.y+2;
    
    // load args
    float mean_texture = 0.0f;
    float black_level = 0.0f;
    
    // extract color channel-dependent mean value of the average texture of all images and the black level
    if (x%2 == 0 & y%2 == 0) {
        mean_texture = mean_texture_buffer[0] - black_level0;
        black_level = float(black_level0);
    } else if (x%2 == 1 & y%2 == 0) {
        mean_texture = mean_texture_buffer[1] - black_level1;
        black_level = float(black_level1);
    } else if (x%2 == 0 & y%2 == 1) {
        mean_texture = mean_texture_buffer[2] - black_level2;
        black_level = float(black_level2);
    } else if (x%2 == 1 & y%2 == 1) {
        mean_texture = mean_texture_buffer[3] - black_level3;
        black_level = float(black_level3);
    }
        
    // calculate weighted sum of 8 pixels surrounding the potential hot pixel based on the average texture
    float sum =   average_texture.read(uint2(x-2, y-2)).r;
    sum      +=   average_texture.read(uint2(x+2, y-2)).r;
    sum      +=   average_texture.read(uint2(x-2, y+2)).r;
    sum      +=   average_texture.read(uint2(x+2, y+2)).r;
    sum      += 2*average_texture.read(uint2(x-2, y+0)).r;
    sum      += 2*average_texture.read(uint2(x+2, y+0)).r;
    sum      += 2*average_texture.read(uint2(x+0, y-2)).r;
    sum      += 2*average_texture.read(uint2(x+0, y+2)).r;
    
    // extract value of potential hot pixel from the average texture and divide by sum of surrounding pixels
    float const pixel_value = average_texture.read(uint2(x, y)).r;
    float const pixel_ratio = max(pixel_value-black_level, 1.0f)/max(sum/12.0f-black_level, 1.0f);
    
    // if hot pixel is detected
    if (pixel_ratio >= hot_pixel_threshold & pixel_value >= 2.0f*mean_texture) {
        
        // calculate weight for blending to have a smooth transition for not so obvious hot pixels
        float const weight = correction_strength*0.5f*min(hot_pixel_multiplicator*(pixel_ratio-hot_pixel_threshold), 2.0f);
        
        // save weight in texture
        hotpixel_weight_texture.write(weight, uint2(x, y));
    }
}

/**
 This function is intended to convert the source input texture from integer to 32 bit float while correcting hot pixels, equalizing exposure and extending the texture to the size needed for alignment
 */
kernel void prepare_texture(texture2d<uint, access::read> in_texture [[texture(0)]],
                            texture2d<float, access::read> hotpixel_weight_texture [[texture(1)]],
                            texture2d<float, access::write> out_texture [[texture(2)]],
                            constant int& pad_left [[buffer(0)]],
                            constant int& pad_top [[buffer(1)]],
                            constant int& exposure_diff [[buffer(2)]],
                            constant int& black_level0 [[buffer(3)]],
                            constant int& black_level1 [[buffer(4)]],
                            constant int& black_level2 [[buffer(5)]],
                            constant int& black_level3 [[buffer(6)]],
                            uint2 gid [[thread_position_in_grid]]) {
        
    // load args
    int x = gid.x;
    int y = gid.y;
    
    int const texture_width = in_texture.get_width();
    int const texture_height = in_texture.get_height();
 
    float pixel_value = float(in_texture.read(gid).r);
    
    float const hotpixel_weight = hotpixel_weight_texture.read(gid).r;
    
    if (hotpixel_weight > 0.001f && x>=2 && x<texture_width-2 && y>=2 && y<texture_height-2) {
        
        // calculate mean value of 4 surrounding values
        float sum = in_texture.read(uint2(x-2, y+0)).r;
        sum      += in_texture.read(uint2(x+2, y+0)).r;
        sum      += in_texture.read(uint2(x+0, y-2)).r;
        sum      += in_texture.read(uint2(x+0, y+2)).r;
        
        // blend values and replace hot pixel value
        pixel_value = hotpixel_weight*0.25f*sum + (1.0f-hotpixel_weight)*pixel_value;
    }
    
    float4 const black_level4 = float4(black_level0, black_level1, black_level2, black_level3);
    
    // calculate exposure correction factor from exposure difference
    float const corr_factor = pow(2.0f, float(exposure_diff/100.0f));        
    float const black_level = black_level4[(gid.y%2)*2 + (gid.x%2)];
    
    // correct exposure
    pixel_value = (pixel_value - black_level)*corr_factor + black_level;
    pixel_value = max(pixel_value, 0.0f);
    
    out_texture.write(pixel_value, uint2(gid.x+pad_left, gid.y+pad_top));
}

/**
 Divide value from in_buffer by divisor and set to out_buffer. Using an out_buffer instead of doing it in-place in order to keep usage consistent with sum_divide_buffer
 Input and output buffer are the same size.
 */
kernel void divide_buffer(device   float  *in_buffer   [[buffer(0)]],
                          device   float  *out_buffer  [[buffer(1)]],
                          constant float& divisor      [[buffer(2)]],
                          constant int&   buffer_size  [[buffer(3)]],  // Unused. Here to keep signature consistent with sum_divide_buffer
                          uint2 gid [[thread_position_in_grid]]) {
    out_buffer[gid.x] = in_buffer[gid.x] / divisor;
}

/**
 Calculate the sum of in_buffer, divide by divisor, and set to out_buffer.
 */
kernel void sum_divide_buffer(device   float  *in_buffer   [[buffer(0)]],
                              device   float  *out_buffer  [[buffer(1)]],
                              constant float& divisor      [[buffer(2)]],
                              constant int&   buffer_size  [[buffer(3)]],
                              uint2 gid [[thread_position_in_grid]]) {
    for (int i = 0; i < buffer_size; i++) {
        out_buffer[0] += in_buffer[i];
    }
    out_buffer[0] /= divisor;
}


kernel void normalize_texture(texture2d<float, access::read_write> in_texture [[texture(0)]],
                              texture2d<float, access::read> norm_texture [[texture(1)]],
                              constant float& norm_scalar [[buffer(0)]],
                              uint2 gid [[thread_position_in_grid]]) {
    
    in_texture.write(in_texture.read(gid).r/(norm_texture.read(gid).r + norm_scalar), gid);
}

/**
 Used for calculating texture_mean. Can't use the UInt version since the images at this point are stored as floats.
 */
kernel void sum_rect_columns_float(texture2d<float, access::read> in_texture [[texture(0)]],
                                   texture2d<float, access::write> out_texture [[texture(1)]],
                                   constant int& top [[buffer(1)]],
                                   constant int& left [[buffer(2)]],
                                   constant int& bottom [[buffer(3)]],
                                   constant int& mosaic_pattern_width [[buffer(4)]],
                                   uint2 gid [[thread_position_in_grid]]) {
    uint x = left + gid.x;
    uint dy = gid.y;
    
    float total = 0;
    for (int y = top+dy; y < bottom; y += mosaic_pattern_width) {
        total += in_texture.read(uint2(x, y)).r;
    }

    out_texture.write(total, gid);
}

/**
 Used for calculating a black level from masked areas of the DNG.
 DNG data is storred as UInt, thus need a seperate version from the float one above.
 */
kernel void sum_rect_columns_uint(texture2d<uint, access::read> in_texture [[texture(0)]],
                                  texture2d<float, access::write> out_texture [[texture(1)]],
                                  constant int& top [[buffer(1)]],
                                  constant int& left [[buffer(2)]],
                                  constant int& bottom [[buffer(3)]],
                                  constant int& mosaic_pattern_width [[buffer(4)]],
                                  uint2 gid [[thread_position_in_grid]]) {
    uint x = left + gid.x;
    uint dy = gid.y;
    
    float total = 0;
    for (int y = top+dy; y < bottom; y += mosaic_pattern_width) {
        total += in_texture.read(uint2(x, y)).r;
    }

    out_texture.write(total, gid);
}


kernel void sum_row(texture2d<float, access::read> in_texture [[texture(0)]],
                    device float *out_buffer [[buffer(0)]],
                    constant int& width [[buffer(1)]],
                    constant int& mosaic_pattern_width [[buffer(2)]],
                    uint2 gid [[thread_position_in_grid]]) {
    float total = 0.0;
    
    for (int x = 0; x < width; x+= mosaic_pattern_width) {
        total += in_texture.read(uint2(x, gid.y)).r;
    }
    
    out_buffer[gid.x + mosaic_pattern_width*gid.y] = total;
}


/**
  Naming based on https://en.wikipedia.org/wiki/Bilinear_interpolation#/media/File:BilinearInterpolation.svg
  */
kernel void upsample_bilinear_float(texture2d<float, access::read> in_texture [[texture(0)]],
                                    texture2d<float, access::write> out_texture [[texture(1)]],
                                    constant float& scale_x [[buffer(0)]],
                                    constant float& scale_y [[buffer(1)]],
                                    uint2 gid [[thread_position_in_grid]]) {
    
    // naming based on https://en.wikipedia.org/wiki/Bilinear_interpolation#/media/File:BilinearInterpolation.svg
    float x = float(gid.x) / scale_x;
    float y = float(gid.y) / scale_y;
    float epsilon = 1e-5;
    
    // interpolate over the x-axis
    float4 i1, i2;
    if (abs(x - round(x)) < epsilon) {
        i1 = float4(in_texture.read(uint2(round(x), floor(y))));
        i2 = float4(in_texture.read(uint2(round(x), ceil(y) )));
    } else {
        float4 i11 = float4(in_texture.read(uint2(floor(x), floor(y))));
        float4 i12 = float4(in_texture.read(uint2(floor(x), ceil(y) )));
        float4 i21 = float4(in_texture.read(uint2(ceil(x),  floor(y))));
        float4 i22 = float4(in_texture.read(uint2(ceil(x),  ceil(y) )));
        i1 = (ceil(x) - x) * i11 + (x - floor(x)) * i21;
        i2 = (ceil(x) - x) * i12 + (x - floor(x)) * i22;
    }
    
    // interpolate over the y-axis
    float4 i;
    if (abs(y - round(y)) < epsilon) {
        i = i1;
    } else {
        i = (ceil(y) - y) * i1 + (y - floor(y)) * i2;
    }
    
    out_texture.write(i, gid);
}


kernel void upsample_nearest_int(texture2d<int, access::read> in_texture [[texture(0)]],
                                 texture2d<int, access::write> out_texture [[texture(1)]],
                                 constant float& scale_x [[buffer(0)]],
                                 constant float& scale_y [[buffer(1)]],
                                 uint2 gid [[thread_position_in_grid]]) {

    int x = int(round(float(gid.x) / scale_x));
    int y = int(round(float(gid.y) / scale_y));
    
    int4 out_color = in_texture.read(uint2(x, y));
    out_texture.write(out_color, gid);
}
