#include <metal_stdlib>
using namespace metal;
using namespace simd;

constant uint UINT16_MAX_VAL = 65535;
constant float PI = 3.14159265358979323846;


// ===========================================================================================================
// Helper functions
// ===========================================================================================================

kernel void fill_with_zeros(texture2d<float, access::write> texture [[texture(0)]],
                            uint2 gid [[thread_position_in_grid]]) {
    texture.write(0, gid);
}


kernel void texture_uint16_to_float(texture2d<uint, access::read> in_texture [[texture(0)]],
                                    texture2d<float, access::write> out_texture [[texture(1)]],
                                    uint2 gid [[thread_position_in_grid]]) {
    uint4 val_unint = in_texture.read(gid);
    float4 val_float = float4(val_unint); // removed scaling to the range 0 to 1
    out_texture.write(val_float, gid); 
}


kernel void convert_uint16(texture2d<float, access::read> in_texture [[texture(0)]],
                           texture2d<uint, access::write> out_texture [[texture(1)]],
                           uint2 gid [[thread_position_in_grid]]) {
      
    float color_value = in_texture.read(gid).r;
        
    color_value = clamp(color_value, 0.0f, float(UINT16_MAX_VAL));
    
    uint out = int(round(color_value));
    
    out_texture.write(out, gid);
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


kernel void avg_pool(texture2d<float, access::read> in_texture [[texture(0)]],
                     texture2d<float, access::write> out_texture [[texture(1)]],
                     constant int& scale [[buffer(0)]],
                     uint2 gid [[thread_position_in_grid]]) {
    float out_pixel = 0;
    int x0 = gid.x * scale;
    int y0 = gid.y * scale;
    for (int dx = 0; dx < scale; dx++) {
        for (int dy = 0; dy < scale; dy++) {
            int x = x0 + dx;
            int y = y0 + dy;
            out_pixel += in_texture.read(uint2(x, y)).r;
        }
    }
    out_pixel /= (scale*scale);
    out_texture.write(out_pixel, gid);
}


kernel void blur_mosaic_x(texture2d<float, access::read> in_texture [[texture(0)]],
                          texture2d<float, access::write> out_texture [[texture(1)]],
                          constant int& kernel_size [[buffer(0)]],
                          constant int& mosaic_pettern_width [[buffer(1)]],
                          uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_width = in_texture.get_width();
    
    // clamp kernel_size to a maximum of 8
    int const kernel_size_c = clamp(kernel_size, 0, 8);
    
    // set kernel weights of binomial filter
    float bw[9] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
    
    if(kernel_size_c == 1)      {bw[0]=    2; bw[1]=    1;}
    else if(kernel_size_c == 2) {bw[0]=    6; bw[1]=    4; bw[2]=   1;}
    else if(kernel_size_c == 3) {bw[0]=   20; bw[1]=   15; bw[2]=   6; bw[3]=   1;}
    else if(kernel_size_c == 4) {bw[0]=   70; bw[1]=   56; bw[2]=  28; bw[3]=   8; bw[4]=   1;}
    else if(kernel_size_c == 5) {bw[0]=  252; bw[1]=  210; bw[2]= 120; bw[3]=  45; bw[4]=  10; bw[5]=  1;}
    else if(kernel_size_c == 6) {bw[0]=  924; bw[1]=  792; bw[2]= 495; bw[3]= 220; bw[4]=  66; bw[5]= 12; bw[6]=  1;}
    else if(kernel_size_c == 7) {bw[0]= 3432; bw[1]= 3003; bw[2]=2002; bw[3]=1001; bw[4]= 364; bw[5]= 91; bw[6]= 14; bw[7]= 1;}
    else if(kernel_size_c == 8) {bw[0]=12870; bw[1]=11440; bw[2]=8008; bw[3]=4368; bw[4]=1820; bw[5]=560; bw[6]=120; bw[7]=16; bw[8]=1;}

    // compute a sigle output pixel
    float total_intensity = 0;
    float total_weight = 0;
    int y = gid.y;
    for (int dx = -kernel_size_c; dx <= kernel_size_c; dx++) {
        int x = gid.x + mosaic_pettern_width*dx;
        if (0 <= x && x < texture_width) {

            float const weight = bw[abs(dx)];
            total_intensity += weight * in_texture.read(uint2(x, y)).r;
            total_weight += weight;
        }
    }
    
    // write output pixel
    float out_intensity = total_intensity / total_weight;
    out_texture.write(out_intensity, gid);
}


kernel void blur_mosaic_y(texture2d<float, access::read> in_texture [[texture(0)]],
                          texture2d<float, access::write> out_texture [[texture(1)]],
                          constant int& kernel_size [[buffer(0)]],
                          constant int& mosaic_pettern_width [[buffer(1)]],
                          uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_height = in_texture.get_height();
    
    // set kernel weights of binomial filter and clamp kernel_size to a maximum of 8
    int const kernel_size_c = clamp(kernel_size, 0, 8);
   
    // set kernel weights of binomial filter
    float bw[9] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
    
    if(kernel_size_c == 1)      {bw[0]=    2; bw[1]=    1;}
    else if(kernel_size_c == 2) {bw[0]=    6; bw[1]=    4; bw[2]=   1;}
    else if(kernel_size_c == 3) {bw[0]=   20; bw[1]=   15; bw[2]=   6; bw[3]=   1;}
    else if(kernel_size_c == 4) {bw[0]=   70; bw[1]=   56; bw[2]=  28; bw[3]=   8; bw[4]=   1;}
    else if(kernel_size_c == 5) {bw[0]=  252; bw[1]=  210; bw[2]= 120; bw[3]=  45; bw[4]=  10; bw[5]=  1;}
    else if(kernel_size_c == 6) {bw[0]=  924; bw[1]=  792; bw[2]= 495; bw[3]= 220; bw[4]=  66; bw[5]= 12; bw[6]=  1;}
    else if(kernel_size_c == 7) {bw[0]= 3432; bw[1]= 3003; bw[2]=2002; bw[3]=1001; bw[4]= 364; bw[5]= 91; bw[6]= 14; bw[7]= 1;}
    else if(kernel_size_c == 8) {bw[0]=12870; bw[1]=11440; bw[2]=8008; bw[3]=4368; bw[4]=1820; bw[5]=560; bw[6]=120; bw[7]=16; bw[8]=1;}
    
    // compute a sigle output pixel
    float total_intensity = 0;
    float total_weight = 0;
    int x = gid.x;
    for (int dy = -kernel_size_c; dy <= kernel_size_c; dy++) {
        int y = gid.y + mosaic_pettern_width*dy;
        if (0 <= y && y < texture_height) {
           
            float const weight = bw[abs(dy)];
            total_intensity += weight * in_texture.read(uint2(x, y)).r;
            total_weight += weight;
        }
    }
    
    // write output pixel
    float out_intensity = total_intensity / total_weight;
    out_texture.write(out_intensity, gid);
}


kernel void extend_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                           texture2d<float, access::write> out_texture [[texture(1)]],
                           constant int& pad_left [[buffer(0)]],
                           constant int& pad_top [[buffer(1)]],
                           uint2 gid [[thread_position_in_grid]]) {
        
    int x = gid.x + pad_left;
    int y = gid.y + pad_top;
 
    float color_value = in_texture.read(gid).r;
    
    out_texture.write(color_value, uint2(x, y));
}


kernel void crop_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                         texture2d<float, access::write> out_texture [[texture(1)]],
                         constant int& pad_left [[buffer(0)]],
                         constant int& pad_top [[buffer(1)]],
                         uint2 gid [[thread_position_in_grid]]) {
      
    int x = gid.x + pad_left;
    int y = gid.y + pad_top;
  
    float color_value = in_texture.read(uint2(x, y)).r;
    
    out_texture.write(color_value, gid);
}




kernel void add_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                        texture2d<float, access::read_write> out_texture [[texture(1)]],
                        constant int& n_textures [[buffer(0)]],
                        uint2 gid [[thread_position_in_grid]]) {
    
    float color_value = out_texture.read(gid).r + in_texture.read(gid).r/float(n_textures);
    
    out_texture.write(color_value, gid);
}


kernel void copy_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                         texture2d<float, access::write> out_texture [[texture(1)]],
                         uint2 gid [[thread_position_in_grid]]) {

    out_texture.write(in_texture.read(gid), gid);
}


kernel void convert_rgba(texture2d<float, access::read> in_texture [[texture(0)]],
                         texture2d<float, access::write> out_texture [[texture(1)]],
                         constant int& crop_merge_x [[buffer(0)]],
                         constant int& crop_merge_y [[buffer(1)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    int const m = gid.x*2 + crop_merge_x;
    int const n = gid.y*2 + crop_merge_y;
    
    float4 color_value = float4(in_texture.read(uint2(m, n)).r,   in_texture.read(uint2(m+1, n)).r,
                                in_texture.read(uint2(m, n+1)).r, in_texture.read(uint2(m+1, n+1)).r);
    
    out_texture.write(color_value, gid);
}


kernel void convert_bayer(texture2d<float, access::read> in_texture [[texture(0)]],
                          texture2d<float, access::write> out_texture [[texture(1)]],
                          uint2 gid [[thread_position_in_grid]]) {
    
    int const m = gid.x*2;
    int const n = gid.y*2;
    
    float4 color_value = float4(in_texture.read(gid));
     
    out_texture.write(color_value[0], uint2(m,   n));
    out_texture.write(color_value[1], uint2(m+1, n));
    out_texture.write(color_value[2], uint2(m,   n+1));
    out_texture.write(color_value[3], uint2(m+1, n+1));
}



// ===========================================================================================================
// Functions specific to image alignment
// ===========================================================================================================

kernel void compute_tile_differences(texture2d<float, access::read> ref_texture [[texture(0)]],
                                     texture2d<float, access::read> comp_texture [[texture(1)]],
                                     texture2d<int, access::read> prev_alignment [[texture(2)]],
                                     texture3d<float, access::write> tile_diff [[texture(3)]],
                                     constant int& downscale_factor [[buffer(0)]],
                                     constant int& tile_size [[buffer(1)]],
                                     constant int& search_dist [[buffer(2)]],
                                     constant int& n_tiles_x [[buffer(3)]],
                                     constant int& n_tiles_y [[buffer(4)]],
                                     uint3 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_width = ref_texture.get_width();
    int texture_height = ref_texture.get_height();
    int tile_half_size = tile_size / 2;
    int n_pos_1d = 2*search_dist + 1;
    
    // compute tile position if previous alignment were 0
    int x0 = int(floor( tile_half_size + float(gid.x)/float(n_tiles_x-1) * (texture_width  - tile_size - 1) ));
    int y0 = int(floor( tile_half_size + float(gid.y)/float(n_tiles_y-1) * (texture_height - tile_size - 1) ));
    
    // compute current tile displacement based on thread index
    int dy0 = gid.z / n_pos_1d - search_dist;
    int dx0 = gid.z % n_pos_1d - search_dist;
    
    // factor in previous alignmnet
    int4 prev_align = prev_alignment.read(uint2(gid.x, gid.y));
    dx0 += downscale_factor * prev_align.x;
    dy0 += downscale_factor * prev_align.y;
    
    // compute tile difference
    float diff = 0;
    for (int dx1 = -tile_half_size; dx1 < tile_half_size; dx1++){
        for (int dy1 = -tile_half_size; dy1 < tile_half_size; dy1++){
            // compute the indices of the pixels to compare
            int ref_tile_x = x0 + dx1;
            int ref_tile_y = y0 + dy1;
            int comp_tile_x = ref_tile_x + dx0;
            int comp_tile_y = ref_tile_y + dy0;
            
            // if the comparison pixels are outside of the frame, attach a high loss to them
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                diff += (2*UINT16_MAX_VAL); // value has to be increased as scaling of values to the range of 0 to 1 was removed
            } else {
                diff += abs(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
        }
    }
    
    // store tile difference
    tile_diff.write(diff, gid);
}


kernel void compute_tile_differences25(texture2d<float, access::read> ref_texture [[texture(0)]],
                                       texture2d<float, access::read> comp_texture [[texture(1)]],
                                       texture2d<int, access::read> prev_alignment [[texture(2)]],
                                       texture3d<float, access::write> tile_diff [[texture(3)]],
                                       constant int& downscale_factor [[buffer(0)]],
                                       constant int& tile_size [[buffer(1)]],
                                       constant int& search_dist [[buffer(2)]],
                                       constant int& n_tiles_x [[buffer(3)]],
                                       constant int& n_tiles_y [[buffer(4)]],
                                       uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_width = ref_texture.get_width();
    int texture_height = ref_texture.get_height();
    int tile_half_size = tile_size / 2;
    int n_pos_1d = 2*search_dist + 1;
    
    int comp_tile_x, comp_tile_y;
    
    // compute tile position if previous alignment were 0
    int x0 = int(floor( tile_half_size + float(gid.x)/float(n_tiles_x-1) * (texture_width  - tile_size - 1) ));
    int y0 = int(floor( tile_half_size + float(gid.y)/float(n_tiles_y-1) * (texture_height - tile_size - 1) ));
    
    // factor in previous alignmnet
    int4 prev_align = prev_alignment.read(uint2(gid.x, gid.y));
    int dx0 = downscale_factor * prev_align.x;
    int dy0 = downscale_factor * prev_align.y;
    
    float diff[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
    for (int dy1 = -tile_half_size; dy1 < tile_half_size; dy1++) {
        for (int dx1 = -tile_half_size; dx1 < tile_half_size; dx1++) {
        
            // compute the indices of the pixels to compare
            int const ref_tile_x = x0 + dx1;
            int const ref_tile_y = y0 + dy1;
            float const ref_val = ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r;
            
            for (int i = 0; i < 25; i++) {
 
                comp_tile_x = ref_tile_x + dx0 + (i % n_pos_1d - search_dist);
                comp_tile_y = ref_tile_y + dy0 + (i / n_pos_1d - search_dist);
                if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                    diff[i] += (2*UINT16_MAX_VAL);
                } else {
                    diff[i] += abs(ref_val - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
                }
            }
        }
    }
    
    // store tile differences
    for (int i = 0; i < 25; i++)
    {
        tile_diff.write(diff[i], uint3(gid.x, gid.y, i));
    }
}

/*
kernel void compute_tile_differences25(texture2d<float, access::read> ref_texture [[texture(0)]],
                                       texture2d<float, access::read> comp_texture [[texture(1)]],
                                       texture2d<int, access::read> prev_alignment [[texture(2)]],
                                       texture3d<float, access::write> tile_diff [[texture(3)]],
                                       constant int& downscale_factor [[buffer(0)]],
                                       constant int& tile_size [[buffer(1)]],
                                       constant int& search_dist [[buffer(2)]],
                                       constant int& n_tiles_x [[buffer(3)]],
                                       constant int& n_tiles_y [[buffer(4)]],
                                       uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_width = ref_texture.get_width();
    int texture_height = ref_texture.get_height();
    int tile_half_size = tile_size / 2;
    
    int comp_tile_x, comp_tile_y;
    
    // compute tile position if previous alignment were 0
    int x0 = int(floor( tile_half_size + float(gid.x)/float(n_tiles_x-1) * (texture_width  - tile_size - 1) ));
    int y0 = int(floor( tile_half_size + float(gid.y)/float(n_tiles_y-1) * (texture_height - tile_size - 1) ));
    
    // factor in previous alignmnet
    int4 prev_align = prev_alignment.read(uint2(gid.x, gid.y));
    int dx0 = downscale_factor * prev_align.x;
    int dy0 = downscale_factor * prev_align.y;
    
    float diff[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float tmp_ref[64];
    float tmp_comp[68];
    
    for (int i = 0; i < 25; i++) {
        diff[i] = 2*UINT16_MAX_VAL*tile_size*tile_size;
    }
    
    for (int dy1 = -tile_half_size; dy1 < tile_half_size; dy1++) {
        
        for (int dx1 = -tile_half_size; dx1 < tile_half_size; dx1++) {
            
            int const ref_tile_x = x0 + dx1;
            int const ref_tile_y = y0 + dy1;
            
            tmp_ref[dx1+tile_half_size] = ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r;
        }
        
        for (int dy2 = -2; dy2 <= 2; dy2++) {
            
            comp_tile_y = y0 + dy0 + dy1 + dy2;
            
            if (comp_tile_y>=0 & comp_tile_y<texture_height) {
                
                for (int dx2 = -2; dx2 < tile_size+2; dx2++) {
                    
                    comp_tile_x = x0 + dx0 + dx2;
                    
                    if (comp_tile_x>=0 & comp_tile_x<texture_width) {
                        tmp_comp[dx2+2] = comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r;
                    }
                }
                
                for (int dx2 = -2; dx2 <= 2; dx2++) {
                    
                    int const index = (dy2+2)*5 + (dx2+2);
                    
                    for (int i = 0; i < tile_size; i++) {
                        
                        comp_tile_x = x0 + dx0 + dx2 + i;
                        
                        if (comp_tile_x>=0 & comp_tile_x<texture_width) {
                            diff[index] += (abs(tmp_ref[i] - tmp_comp[i+dx2+2]) - 2*UINT16_MAX_VAL);
                        }
                    }
                }
            }
        }
    }
    
    // store tile differences
    for (int i = 0; i < 25; i++)
    {
        tile_diff.write(diff[i], uint3(gid.x, gid.y, i));
    }
}
*/

kernel void compute_tile_alignments(texture3d<float, access::read> tile_diff [[texture(0)]],
                                    texture2d<int, access::read> prev_alignment [[texture(1)]],
                                    texture2d<int, access::write> current_alignment [[texture(2)]],
                                    constant int& downscale_factor [[buffer(0)]],
                                    constant int& search_dist [[buffer(1)]],
                                    uint2 gid [[thread_position_in_grid]]) {
    // load args
    int n_pos_1d = 2*search_dist + 1;
    int n_pos_2d = n_pos_1d * n_pos_1d;
    
    // find tile position with the lowest pixel difference
    float current_diff;
    float min_diff_val = tile_diff.read(uint3(gid.x, gid.y, 0)).r;
    int min_diff_idx = 0;
    for (int i = 0; i < n_pos_2d; i++) {
        current_diff = tile_diff.read(uint3(gid.x, gid.y, i)).r;
        if (current_diff < min_diff_val) {
            min_diff_val = current_diff;
            min_diff_idx = i;
        }
    }
    
    // compute tile displacement if previous alignment were 0
    int dy = min_diff_idx / n_pos_1d - search_dist;
    int dx = min_diff_idx % n_pos_1d - search_dist;
    
    // factor in previous alignmnet
    int4 prev_align = prev_alignment.read(gid);
    dx += downscale_factor * prev_align.x;
    dy += downscale_factor * prev_align.y;
    
    // store alignment
    int4 out = int4(dx, dy, 0, 0);
    current_alignment.write(out, gid);
}


kernel void correct_upsampling_error(texture2d<float, access::read> ref_texture [[texture(0)]],
                                     texture2d<float, access::read> comp_texture [[texture(1)]],
                                     texture2d<int, access::read> prev_alignment [[texture(2)]],
                                     texture2d<int, access::write> prev_alignment_corrected [[texture(3)]],
                                     constant int& downscale_factor [[buffer(0)]],
                                     constant int& tile_size [[buffer(1)]],
                                     constant int& n_tiles_x [[buffer(2)]],
                                     constant int& n_tiles_y [[buffer(3)]],
                                     uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_width = ref_texture.get_width();
    int texture_height = ref_texture.get_height();
    int tile_half_size = tile_size / 2;
    
    int comp_tile_x, comp_tile_y;
    
    // compute tile position if previous alignment were 0
    int x0 = int(floor( tile_half_size + float(gid.x)/float(n_tiles_x-1) * (texture_width  - tile_size - 1) ));
    int y0 = int(floor( tile_half_size + float(gid.y)/float(n_tiles_y-1) * (texture_height - tile_size - 1) ));

    // calculate shifts for 3 candidate alignments to evaluate
    int3 const x_shift = int3(0, ((gid.x%2 == 0) ? -1 : 1), 0);
    int3 const y_shift = int3(0, 0, ((gid.y%2 == 0) ? -1 : 1));
    
    int3 const x = clamp(int3(gid.x+x_shift), 0, n_tiles_x-1);
    int3 const y = clamp(int3(gid.y+y_shift), 0, n_tiles_y-1);
    
    // factor in previous alignmnet
    int4 prev_align0 = prev_alignment.read(uint2(x[0], y[0]));
    int dx00 = downscale_factor * prev_align0.x;
    int dy00 = downscale_factor * prev_align0.y;
    
    int4 prev_align1 = prev_alignment.read(uint2(x[1], y[1]));
    int dx01 = downscale_factor * prev_align1.x;
    int dy01 = downscale_factor * prev_align1.y;
    
    int4 prev_align2 = prev_alignment.read(uint2(x[2], y[2]));
    int dx02 = downscale_factor * prev_align2.x;
    int dy02 = downscale_factor * prev_align2.y;
    
    // compute tile difference
    float diff0 = 0;
    float diff1 = 0;
    float diff2 = 0;
    
    for (int dx1 = -tile_half_size; dx1 < tile_half_size; dx1++){
        for (int dy1 = -tile_half_size; dy1 < tile_half_size; dy1++){
            // compute the indices of the pixels to compare
            int const ref_tile_x = x0 + dx1;
            int const ref_tile_y = y0 + dy1;
            float const ref_val = ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r;
                      
            comp_tile_x = ref_tile_x + dx00;
            comp_tile_y = ref_tile_y + dy00;
            
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                diff0 += (2*UINT16_MAX_VAL);
            } else {
                diff0 += abs(ref_val - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
            
            comp_tile_x = ref_tile_x + dx01;
            comp_tile_y = ref_tile_y + dy01;

            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                diff1 += (2*UINT16_MAX_VAL);
            } else {
                diff1 += abs(ref_val - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
            
            comp_tile_x = ref_tile_x + dx02;
            comp_tile_y = ref_tile_y + dy02;
            
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                diff2 += (2*UINT16_MAX_VAL);
            } else {
                diff2 += abs(ref_val - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
        }
    }
    
    // store corrected (best) alignment
    if(diff0 < diff1 & diff0 < diff2) {
        prev_alignment_corrected.write(prev_align0, gid);
        
    } else if(diff1 < diff2) {
        prev_alignment_corrected.write(prev_align1, gid);
        
    } else {
        prev_alignment_corrected.write(prev_align2, gid);
    }
}


kernel void warp_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                         texture2d<float, access::read_write> out_texture [[texture(1)]],
                         texture2d<int, access::read> prev_alignment [[texture(2)]],
                         constant int& downscale_factor [[buffer(0)]],
                         constant int& tile_size [[buffer(1)]],
                         constant int& n_tiles_x [[buffer(2)]],
                         constant int& n_tiles_y [[buffer(3)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_width = in_texture.get_width();
    int texture_height = in_texture.get_height();
    int tile_half_size = tile_size / 2;
    
    // load coordinates of output pixel
    int x1_pix = gid.x;
    int y1_pix = gid.y;
    
    // compute the coordinates of output pixel in tile-grid units
    float x1_grid = float(x1_pix - tile_half_size) / float(texture_width  - tile_size - 1) * (n_tiles_x - 1);
    float y1_grid = float(y1_pix - tile_half_size) / float(texture_height - tile_size - 1) * (n_tiles_y - 1);
    
    // comput the two possible tile-grid indices that the given output pixel belongs to
    int x_grid_list[] = {int(floor(x1_grid)), int(floor(x1_grid)), int(ceil (x1_grid)), int(ceil(x1_grid))};
    int y_grid_list[] = {int(floor(y1_grid)), int(ceil (y1_grid)), int(floor(y1_grid)), int(ceil(y1_grid))};
    
    // loop over the two possible tile-grid indices that the given output pixel belongs to
    float total_intensity = 0;
    float total_weight = 0;
    for (int i = 0; i < 4; i++){
        
        // load the index of the tile
        int x_grid = x_grid_list[i];
        int y_grid = y_grid_list[i];
        
        // compute the pixel coordinates of the center of the reference tile
        int x0_pix = int(floor( tile_half_size + float(x_grid)/float(n_tiles_x-1) * (texture_width  - tile_size - 1) ));
        int y0_pix = int(floor( tile_half_size + float(y_grid)/float(n_tiles_y-1) * (texture_height - tile_size - 1) ));
        
        // check that the output pixel falls within the reference tile
        if ((abs(x1_pix - x0_pix) <= tile_half_size) && (abs(y1_pix - y0_pix) <= tile_half_size)) {
            
            // compute tile displacement
            int4 prev_align = prev_alignment.read(uint2(x_grid, y_grid));
            int dx = downscale_factor * prev_align.x;
            int dy = downscale_factor * prev_align.y;

            // load coordinates of the corresponding pixel from the comparison tile
            int x2_pix = x1_pix + dx;
            int y2_pix = y1_pix + dy;
            
            // compute the weight of the aligned pixel (based on distance from tile center)
            int dist_x = abs(x1_pix - x0_pix);
            int dist_y = abs(y1_pix - y0_pix);
            float weight_x = tile_size - dist_x - dist_y;
            float weight_y = tile_size - dist_x - dist_y;
            float curr_weight = weight_x * weight_y;
            total_weight += curr_weight;
            
            // add pixel value to the output
            total_intensity += curr_weight * in_texture.read(uint2(x2_pix, y2_pix)).r;
        }
    }
    
    // write output pixel
    float out_intensity = total_intensity / total_weight;
    out_texture.write(out_intensity, uint2(x1_pix, y1_pix));
}


// ===========================================================================================================
// Function specific to merging in the spatial domain
// ===========================================================================================================

kernel void add_textures_weighted(texture2d<float, access::read> texture1 [[texture(0)]],
                                  texture2d<float, access::read> texture2 [[texture(1)]],
                                  texture2d<float, access::read> weight_texture [[texture(2)]],
                                  texture2d<float, access::write> out_texture [[texture(3)]],
                                  uint2 gid [[thread_position_in_grid]]) {
    float intensity1 = texture1.read(gid).r;
    float intensity2 = texture2.read(gid).r;
    float weight = weight_texture.read(gid).r;
    float out_intensity = weight * intensity2 + (1 - weight) * intensity1;
    out_texture.write(out_intensity, gid);
}


kernel void average_x(texture1d<float, access::read> in_texture [[texture(0)]],
                      device float *out_buffer [[buffer(0)]],
                      constant int& width [[buffer(1)]],
                      uint gid [[thread_position_in_grid]]) {
    float total = 0;
    for (int x = 0; x < width; x++) {
        total += in_texture.read(uint(x)).r;
    }
    float avg = total / width;
    out_buffer[0] = avg;
}


kernel void average_y_rgba(texture2d<float, access::read> in_texture [[texture(0)]],
                           texture1d<float, access::write> out_texture [[texture(1)]],
                           uint gid [[thread_position_in_grid]]) {
    uint x = gid;
    int texture_height = in_texture.get_height();
    float4 total = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int y = 0; y < texture_height; y++) {
        total += in_texture.read(uint2(x, y));
    }
    float4 avg = total / texture_height;
    out_texture.write(avg, x);
}


kernel void average_x_rgba(texture1d<float, access::read> in_texture [[texture(0)]],
                           device float *out_buffer [[buffer(0)]],
                           constant int& width [[buffer(1)]],
                           uint gid [[thread_position_in_grid]]) {
    float4 total = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int x = 0; x < width; x++) {
        total += in_texture.read(uint(x));
    }
    float4 avg = total / width;
    out_buffer[0] = avg[0];
    out_buffer[1] = avg[1];
    out_buffer[2] = avg[2];
    out_buffer[3] = avg[3];
}


kernel void average_y(texture2d<float, access::read> in_texture [[texture(0)]],
                      texture1d<float, access::write> out_texture [[texture(1)]],
                      uint gid [[thread_position_in_grid]]) {
    uint x = gid;
    int texture_height = in_texture.get_height();
    float total = 0;
    for (int y = 0; y < texture_height; y++) {
        total += in_texture.read(uint2(x, y)).r;
    }
    float avg = total / texture_height;
    out_texture.write(avg, x);
}




kernel void color_difference(texture2d<float, access::read> texture1 [[texture(0)]],
                             texture2d<float, access::read> texture2 [[texture(1)]],
                             texture2d<float, access::write> out_texture [[texture(2)]],
                             constant int& mosaic_pettern_width [[buffer(0)]],
                             uint2 gid [[thread_position_in_grid]]) {
    float total_diff = 0;
    int x0 = gid.x * mosaic_pettern_width;
    int y0 = gid.y * mosaic_pettern_width;
    for (int dx = 0; dx < mosaic_pettern_width; dx++) {
        for (int dy = 0; dy < mosaic_pettern_width; dy++) {
            int x = x0 + dx;
            int y = y0 + dy;
            float i1 = texture1.read(uint2(x, y)).r;
            float i2 = texture2.read(uint2(x, y)).r;
            total_diff += abs(i1 - i2);
        }
    }
    out_texture.write(total_diff, gid);
}


kernel void compute_merge_weight(texture2d<float, access::read> texture_diff [[texture(0)]],
                                 texture2d<float, access::write> weight_texture [[texture(1)]],
                                 constant float* noise_sd_buffer [[buffer(0)]],
                                 constant float& robustness [[buffer(1)]],
                                 uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    float noise_sd = noise_sd_buffer[0];
    
    // load texture difference
    float diff = texture_diff.read(gid).r;
    
    // compute the weight to assign to the comparison frame
    // weight == 0 means that the aligned image is ignored
    // weight == 1 means that the aligned image has full weight
    float weight;
    if (robustness == 0) {
        // robustness == 0 means that robust merge is turned off
        weight = 1;
    } else {
        // compare the difference to image noise
        // as diff increases, the weight of the aligned image will continuously decrease from 1.0 to 0.0
        // the two extreme cases are:
        // diff == 0                   --> aligned image will have weight 1.0
        // diff >= noise_sd/robustness --> aligned image will have weight 0.0
        float max_diff = noise_sd / robustness;
        weight =  1 - diff / max_diff;
        weight = clamp(weight, 0.0, 1.0);
    }
    
    // write weight
    weight_texture.write(weight, gid);
}


// ===========================================================================================================
// Function specific to merging in the frequency domain
// ===========================================================================================================

kernel void calculate_rms_rgba(texture2d<float, access::read> ref_texture [[texture(0)]],
                               texture2d<float, access::write> rms_texture [[texture(1)]],
                               constant int& tile_size [[buffer(0)]],
                               uint2 gid [[thread_position_in_grid]]) {
    
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
    
    // fill with zeros
    float4 noise_est = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // use 16x16 tile size here
    for (int dy = 0; dy < tile_size; dy++) {
        for (int dx = 0; dx < tile_size; dx++) {
            
            int const x = m0 + dx;
            int const y = n0 + dy;
            
            float4 const data_noise = ref_texture.read(uint2(x, y));
            
            noise_est += (data_noise * data_noise);
        }
    }

    noise_est = 0.25f*sqrt(noise_est)/float(tile_size);
    
    rms_texture.write(noise_est, gid);
}
    

kernel void forward_dft(texture2d<float, access::read> in_texture [[texture(0)]],
                        texture2d<float, access::read_write> tmp_texture_ft [[texture(1)]],
                        texture2d<float, access::write> out_texture_ft [[texture(2)]],
                        constant int& tile_size [[buffer(0)]],
                        uint2 gid [[thread_position_in_grid]]) {
    
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
        
    // pre-calculate factors for sine and cosine calculation
    float const angle = -2*PI/float(tile_size);
    
    // pre-initalize some vectors
    float4 const zeros = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float coefRe, coefIm, norm_cosine;
    float4 Re0, Re1, Im0, Im1, dataRe, dataIm;
    
    // column-wise one-dimensional discrete Fourier transform along y-direction
    for (int dm = 0; dm < tile_size; dm++) {
        // exploit symmetry of real dft and calculate reduced number of rows
        for (int dn = 0; dn <= tile_size/2; dn++) {
             
            int const m = m0 + dm;
            int const n = n0 + dn;
            
            // fill with zeros
            Re0 = zeros;
            Im0 = zeros;
            
            for (int dy = 0; dy < tile_size; dy++) {
                                  
                int const y = n0 + dy;
                
                // calculate modified raised cosine window weight for blending tiles to suppress artifacts
                //norm_cosine = (0.5f-0.500f*cos(-angle*(dm+0.5f)))*(0.5f-0.500f*cos(-angle*(dy+0.5f)));
                // calculate modified raised cosine window weight for blending tiles to suppress artifacts (slightly adapted compared to original publication)
                norm_cosine = (0.5f-0.505f*cos(-angle*(dm+0.5f)))*(0.5f-0.505f*cos(-angle*(dy+0.5f)));
                
                // calculate coefficients
                coefRe = cos(angle*dn*dy);
                coefIm = sin(angle*dn*dy);
                
                dataRe = norm_cosine*in_texture.read(uint2(m, y));
                
                Re0 += (coefRe * dataRe);
                Im0 += (coefIm * dataRe);
            }
            
            // write into temporary textures
            tmp_texture_ft.write(Re0, uint2(2*m+0, n));
            tmp_texture_ft.write(Im0, uint2(2*m+1, n));
        }
    }
    
    // row-wise one-dimensional discrete Fourier transform along x-direction
    // exploit symmetry of real dft and calculate reduced number of rows
    for (int dn = 0; dn <= tile_size/2; dn++) {          
        for (int dm = 0; dm < tile_size; dm++) {
                       
            int const m = 2*(m0 + dm);
            int const n = n0 + dn;
            int const n2 = n0 + tile_size-dn;
             
            // fill with zeros
            Re0 = zeros;
            Im0 = zeros;
            Re1 = zeros;
            Im1 = zeros;
            
            for (int dx = 0; dx < tile_size; dx++) {
                                  
                int const x = 2*(m0 + dx);
                
                // calculate coefficients
                coefRe = cos(angle*dm*dx);
                coefIm = sin(angle*dm*dx);
                           
                dataRe = tmp_texture_ft.read(uint2(x+0, n));
                dataIm = tmp_texture_ft.read(uint2(x+1, n));
                             
                Re0 += (coefRe*dataRe - coefIm*dataIm);
                Im0 += (coefIm*dataRe + coefRe*dataIm);
                
                Re1 += (coefRe*dataRe + coefIm*dataIm);
                Im1 += (coefIm*dataRe - coefRe*dataIm);
            }
            
            out_texture_ft.write(Re0, uint2(m+0, n));
            out_texture_ft.write(Im0, uint2(m+1, n));
            
            // exploit symmetry of real dft and calculate values for remaining rows
            if(n2 < n0+tile_size & n2 != n0+tile_size/2)
            {
                out_texture_ft.write(Re1, uint2(m+0, n2));
                out_texture_ft.write(Im1, uint2(m+1, n2));
            }
        }
    }
}


kernel void backward_dft(texture2d<float, access::read> in_texture_ft [[texture(0)]],
                         texture2d<float, access::read_write> tmp_texture_ft [[texture(1)]],
                         texture2d<float, access::write> out_texture [[texture(2)]],
                         constant int& tile_size [[buffer(0)]],
                         constant int& n_textures [[buffer(1)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
    
    // pre-calculate factors for sine and cosine calculation
    float const angle = 2*PI/float(tile_size);
    
    // pre-initalize some vectors
    float4 const zeros       = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 const norm_factor = float4(float(n_textures*tile_size*tile_size), float(n_textures*tile_size*tile_size), float(n_textures*tile_size*tile_size), float(n_textures*tile_size*tile_size));
       
    float coefRe, coefIm;
    float4 Re, Im, dataRe, dataIm;
    
    // row-wise one-dimensional discrete Fourier transform along x-direction
    for (int dn = 0; dn < tile_size; dn++) {
        for (int dm = 0; dm < tile_size; dm++) {
             
            int const m = 2*(m0 + dm);
            int const n = n0 + dn;
            
            // fill with zeros
            Re = zeros;
            Im = zeros;
            
            for (int dx = 0; dx < tile_size; dx++) {
                                  
                int const x = 2*(m0 + dx);
              
                // calculate coefficients
                coefRe = cos(angle*dm*dx);
                coefIm = sin(angle*dm*dx);
                
                dataRe = in_texture_ft.read(uint2(x+0, n));
                dataIm = in_texture_ft.read(uint2(x+1, n));
                
                Re += (coefRe*dataRe - coefIm*dataIm);
                Im += (coefIm*dataRe + coefRe*dataIm);
            }
            
            // write into temporary textures
            tmp_texture_ft.write(Re, uint2(m+0, n));
            tmp_texture_ft.write(Im, uint2(m+1, n));
        }
    }
    
    // column-wise one-dimensional discrete Fourier transform along y-direction
    for (int dm = 0; dm < tile_size; dm++) {
        for (int dn = 0; dn < tile_size; dn++) {
              
            int const m = m0 + dm;
            int const n = n0 + dn;
             
            // fill with zeros
            Re = zeros;
              
            for (int dy = 0; dy < tile_size; dy++) {
                                  
                int const y = n0 + dy;
                
                // calculate coefficients
                coefRe = cos(angle*dn*dy);
                coefIm = sin(angle*dn*dy);
                           
                dataRe = tmp_texture_ft.read(uint2(2*m+0, y));
                dataIm = tmp_texture_ft.read(uint2(2*m+1, y));
                            
                Re += (coefRe*dataRe - coefIm*dataIm);
            }
            
            // normalize result and apply cosine window weight
            Re = Re/norm_factor;
                          
            out_texture.write(Re, uint2(m, n));
        }
    }
}


kernel void merge_frequency_domain(texture2d<float, access::read> ref_texture_ft [[texture(0)]],
                                   texture2d<float, access::read> aligned_texture_ft [[texture(1)]],
                                   texture2d<float, access::read_write> out_texture_ft [[texture(2)]],
                                   texture2d<float, access::read> rms_texture [[texture(3)]],
                                   texture2d<float, access::read> mismatch_texture [[texture(4)]],
                                   constant float& robustness_norm [[buffer(0)]],
                                   constant float& read_noise [[buffer(1)]],
                                   constant float& max_motion_norm [[buffer(2)]],
                                   constant int& tile_size [[buffer(3)]],
                                   uint2 gid [[thread_position_in_grid]]) {
    
    // combine estimated shot noise and read noise
    float4 const noise_est = rms_texture.read(gid) + read_noise;
    // normalize with tile size and robustness norm
    float4 const noise_norm = noise_est*tile_size*tile_size*robustness_norm;
          
    // increase of noise reduction for small values of mismatch
    // see https://graphics.stanford.edu/papers/night-sight-sigasia19/night-sight-sigasia19.pdf for more details
    float4 const mismatch = mismatch_texture.read(gid);
    float4 const motion_norm = clamp(max_motion_norm-(mismatch-0.02f)*(max_motion_norm-1.0f)/0.15f, 1.0f, max_motion_norm)/sqrt(2.0f);
    //float4 const motion_norm = float4(1.0f, 1.0f, 1.0f, 1.0f);
        
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
    
    float const angle = -2*PI/float(tile_size);

    float weight, coefRe, coefIm, shift_x, shift_y;
    float4 refRe, refIm, refMag, alignedRe, alignedIm, alignedRe2, alignedIm2, alignedMag2, mergedRe, mergedIm, weight4, diff4;
    
    float total_diff[81];
    // fill with zeros
    for(int i = 0; i < 81; i++) {
        total_diff[i] = 0.0f;
    }
    
    // subpixel alignment: test shifts between -1.0 and +1.0 pixels (specified on the original pixel scale)
    for (int dn = 0; dn < tile_size; dn++) {
        for (int dm = 0; dm < tile_size; dm++) {
            
            int const m = 2*(m0 + dm);
            int const n = n0 + dn;
            
            refRe = ref_texture_ft.read(uint2(m+0, n));
            refIm = ref_texture_ft.read(uint2(m+1, n));
            
            alignedRe = aligned_texture_ft.read(uint2(m+0, n));
            alignedIm = aligned_texture_ft.read(uint2(m+1, n));
    
            for(int i = 0; i < 81; i++) {
                  
                // potential shift in pixels (specified on the Bayer pixel scale)
                shift_x = -0.5f + int(i % 9) * 0.125f;
                shift_y = -0.5f + int(i / 9) * 0.125f;
                            
                // calculate coefficients for Fourier shift
                coefRe = cos(angle*(dm*shift_x+dn*shift_y));
                coefIm = sin(angle*(dm*shift_x+dn*shift_y));
                          
                alignedRe2 = (coefRe*alignedRe - coefIm*alignedIm);
                alignedIm2 = (coefIm*alignedRe + coefRe*alignedIm);
                
                diff4 = (refRe-alignedRe2)*(refRe-alignedRe2) + (refIm-alignedIm2)*(refIm-alignedIm2);
                // add magnitudes of differences
                total_diff[i] += (diff4[0]+diff4[1]+diff4[2]+diff4[3]);
            }
        }
    }
    
    // find best shifts with lowest differences
    float best_diff = 1e20;
    int   best_i    = 0;
    
    for(int i = 0; i < 81; i++) {
        
        if(total_diff[i] < best_diff) {
            
            best_diff = total_diff[i];
            best_i    = i;
        }
    }
        
    // extract best shifts
    float const best_shift_x = -0.5f + int(best_i % 9) * 0.125f;
    float const best_shift_y = -0.5f + int(best_i / 9) * 0.125f;
    
    // merging of reference and aligned texture
    for (int dn = 0; dn < tile_size; dn++) {
        for (int dm = 0; dm < tile_size; dm++) {
          
            int const m = 2*(m0 + dm);
            int const n = n0 + dn;
            
            refRe = ref_texture_ft.read(uint2(m+0, n));
            refIm = ref_texture_ft.read(uint2(m+1, n));
            
            alignedRe = aligned_texture_ft.read(uint2(m+0, n));
            alignedIm = aligned_texture_ft.read(uint2(m+1, n));
            
            // calculate coefficients for best Fourier shift
            coefRe = cos(angle*(dm*best_shift_x+dn*best_shift_y));
            coefIm = sin(angle*(dm*best_shift_x+dn*best_shift_y));
                      
            alignedRe2 = (coefRe*alignedRe - coefIm*alignedIm);
            alignedIm2 = (coefIm*alignedRe + coefRe*alignedIm);
                       
            // increase merging weights for images with larger frequency magnitudes and decrease weights for lower magnitudes
            // this approach is inspired by the publication in https://arxiv.org/pdf/1505.02731.pdf
            // calculate ratio of magnitudes of complex frequency data            
            float magnitude_norm = 1.0f;
            
            if(dm+dn > 0)
            {
                refMag      = sqrt(refRe*refRe + refIm*refIm);
                alignedMag2 = sqrt(alignedRe2*alignedRe2 + alignedIm2*alignedIm2);
                
                float const ratio_mag = (alignedMag2[0]+alignedMag2[1]+alignedMag2[2]+alignedMag2[3])/(refMag[0]+refMag[1]+refMag[2]+refMag[3]);
                
                // calculation of additional normalization factor that increases impact of sharper frames
                magnitude_norm = 1.0f*clamp(pow(ratio_mag, 4.0f), 0.5f, 3.0f);
                //magnitude_norm = 1.0f*clamp(pow(ratio_mag, 2.0f), 0.5f, 2.0f);
            }
            
            // calculation of merging weight by Wiener shrinkage
            weight4 = (refRe-alignedRe2)*(refRe-alignedRe2) + (refIm-alignedIm2)*(refIm-alignedIm2);
            weight4 = weight4/(weight4 + magnitude_norm*motion_norm*noise_norm);
            
            // use the same weight for all color channels to reduce color artifacts. Use maximum of weight4 as this increases motion robustness.
            // see https://graphics.stanford.edu/papers/night-sight-sigasia19/night-sight-sigasia19.pdf for more details
            weight = clamp(max(weight4[0], max(weight4[1], max(weight4[2], weight4[3]))), 0.0f, 1.0f);
            
            // merging of two textures
            mergedRe = out_texture_ft.read(uint2(m+0, n)) + (1.0f-weight)*alignedRe2 + weight*refRe;
            mergedIm = out_texture_ft.read(uint2(m+1, n)) + (1.0f-weight)*alignedIm2 + weight*refIm;
         
            out_texture_ft.write(mergedRe, uint2(m+0, n));
            out_texture_ft.write(mergedIm, uint2(m+1, n));
        }
    }   
}


kernel void calculate_mismatch_rgba(texture2d<float, access::read> ref_texture [[texture(0)]],
                                    texture2d<float, access::read> aligned_texture [[texture(1)]],
                                    texture2d<float, access::read> rms_texture [[texture(2)]],
                                    texture2d<float, access::write> mismatch_texture [[texture(3)]],
                                    constant int& tile_size [[buffer(0)]],
                                    constant int& tile_size_align [[buffer(1)]],
                                    uint2 gid [[thread_position_in_grid]]) {
        
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
    
    // use only estimated shot noise here
    float4 const noise_est = rms_texture.read(gid);
    
    float const angle = 2*PI/float(tile_size);
    
    // estimate motion mismatch as the absolute difference of reference tile and comparison tile
    // see https://graphics.stanford.edu/papers/night-sight-sigasia19/night-sight-sigasia19.pdf for more details
    // use 16x16, 32x32 or 64x64 tile size here as used for alignment
    
    int const x_start = max(0, m0 + tile_size/2 - tile_size_align/2);
    int const y_start = max(0, n0 + tile_size/2 - tile_size_align/2);
    
    int const x_end = min(int(ref_texture.get_width()-1),  m0 + tile_size/2 + tile_size_align/2);
    int const y_end = min(int(ref_texture.get_height()-1), n0 + tile_size/2 + tile_size_align/2);
    
    float tile_diff = 0.f;
    float n_total = 0.f;
     
    for (int dy = y_start; dy < y_end; dy++) {
        for (int dx = x_start; dx < x_end; dx++) {
        
            float4 const diff = float4(ref_texture.read(uint2(dx, dy))) - float4(aligned_texture.read(uint2(dx, dy)));
             
            // calculate modified raised cosine window weight for blending tiles to suppress artifacts
            float const weight = (0.5f-0.500f*cos(angle*(dx+0.5f)))*(0.5f-0.500f*cos(angle*(dy+0.5f)));
            // calculate modified raised cosine window weight for blending tiles to suppress artifacts (slightly adapted compared to original publication)
            //float const weight = (0.5f-0.505f*cos(angle*(dx+0.5f)))*(0.5f-0.505f*cos(angle*(dy+0.5f)));
                 
            tile_diff += weight*(0.25f * abs(diff[0]+diff[1]+diff[2]+diff[3]));
             
            n_total += weight;
        }
    }
     
    tile_diff /= float(n_total);
    
    // calculation of mismatch ratio
    //float4 const mismatch4 = (tile_diff*tile_diff) / (tile_diff*tile_diff + noise_est);
    float4 const mismatch4 = tile_diff / sqrt(noise_est+1e-12);
    float const mismatch = 0.25f*(mismatch4[0] + mismatch4[1] + mismatch4[2] + mismatch4[3]);
    
    mismatch_texture.write(mismatch, gid);
}


kernel void normalize_mismatch(texture2d<float, access::read_write> mismatch_texture [[texture(0)]],
                               constant float* mean_mismatch_buffer [[buffer(0)]],
                               uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    float const mean_mismatch = mean_mismatch_buffer[0];
    
    float mismatch_norm = mismatch_texture.read(gid).r;
    
    // normalize that mean is set to 0.2
    mismatch_norm /= (mean_mismatch*5.0f + 1e-12f);
    
    mismatch_norm = clamp(mismatch_norm, 0.0f, 1.0f);
    
    mismatch_texture.write(mismatch_norm, gid);
}


kernel void correct_hotpixels(texture2d<float, access::read> average_texture [[texture(0)]],
                              texture2d<float, access::read> in_texture [[texture(1)]],
                              texture2d<float, access::write> out_texture [[texture(2)]],
                              constant float* mean_texture_buffer [[buffer(0)]],
                              uint2 gid [[thread_position_in_grid]]) {
       
    int const x = gid.x+2;
    int const y = gid.y+2;
    
    // load args
    float mean_texture = 0.0f;
    
    if (x%2 == 0 & y%2 == 0) {
        mean_texture = mean_texture_buffer[0];
    }
    if (x%2 == 1 & y%2 == 0) {
        mean_texture = mean_texture_buffer[1];
    }
    if (x%2 == 0 & y%2 == 1) {
        mean_texture = mean_texture_buffer[2];
    }    
    if (x%2 == 1 & y%2 == 1) {
        mean_texture = mean_texture_buffer[3];
    }
        
    // calculate sum of 8 pixels surrounding the potential hot pixel
    float sum =   average_texture.read(uint2(x-2, y-2)).r;
    sum      +=   average_texture.read(uint2(x+2, y-2)).r;
    sum      +=   average_texture.read(uint2(x-2, y+2)).r;
    sum      +=   average_texture.read(uint2(x+2, y+2)).r;
    sum      += 2*average_texture.read(uint2(x-2, y+0)).r;
    sum      += 2*average_texture.read(uint2(x+2, y+0)).r;
    sum      += 2*average_texture.read(uint2(x+0, y-2)).r;
    sum      += 2*average_texture.read(uint2(x+0, y+2)).r;
    
    float const pixel_value = average_texture.read(uint2(x, y)).r;
    float const pixel_ratio = pixel_value/sum;
    
    // if hot pixel is detected
    if (pixel_ratio >= 0.15f & pixel_value >= 2.0f*mean_texture) {
        
        float sum2 = in_texture.read(uint2(x-2, y+0)).r;
        sum2      += in_texture.read(uint2(x+2, y+0)).r;
        sum2      += in_texture.read(uint2(x+0, y-2)).r;
        sum2      += in_texture.read(uint2(x+0, y+2)).r;
        
        float const weight = 10.0f*min(pixel_ratio-0.15f, 0.10f);
        
        out_texture.write(weight*0.25f*sum2 + (1.0f-weight)*pixel_value, uint2(x, y));
    }
}


kernel void forward_fft(texture2d<float, access::read> in_texture [[texture(0)]],
                        texture2d<float, access::read_write> tmp_texture_ft [[texture(1)]],
                        texture2d<float, access::write> out_texture_ft [[texture(2)]],
                        texture2d<float, access::write> out_texture_mag [[texture(3)]],
                        constant int& tile_size [[buffer(0)]],
                        uint2 gid [[thread_position_in_grid]]) {
    
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
    
    int const tile_size_14 = tile_size/4;
    int const tile_size_24 = tile_size/2;
    int const tile_size_34 = tile_size/4*3;
    
    // pre-calculate factors for sine and cosine calculation
    float const angle = -2*PI/float(tile_size);
    
    // pre-initalize some vectors
    float4 const zeros = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float coefRe, coefIm, norm_cosine;
    float4 Re0, Re1, Re2, Re3, Re00, Re11, Re22, Re33, Im0, Im1, Im2, Im3, Im00, Im11, Im22, Im33, dataRe, dataIm;
    float4 Re0_cs, Re1_cs, Re2_cs, Re3_cs, Re00_cs, Re11_cs, Re22_cs, Re33_cs, Im0_cs, Im1_cs, Im2_cs, Im3_cs, Im00_cs, Im11_cs, Im22_cs, Im33_cs;
    float4 tmp_data[32];
    
    // column-wise one-dimensional discrete Fourier transform along y-direction
    for (int dm = 0; dm < tile_size; dm++) {
        
        int const m = m0 + dm;
        
        // copy data to temp vector
        for (int dn = 0; dn < tile_size; dn++) {
            tmp_data[dn] = in_texture.read(uint2(m, n0+dn));
        }
        
        // exploit symmetry of real dft and calculate reduced number of rows
        for (int dn = 0; dn <= tile_size/2; dn++) {
                        
            int const n = n0 + dn;
            
            // fill with zeros
            Re0 = zeros;
            Im0 = zeros;
            
            for (int dy = 0; dy < tile_size; dy++) {
      
                // calculate modified raised cosine window weight for blending tiles to suppress artifacts
                //norm_cosine = (0.5f-0.500f*cos(-angle*(dm+0.5f)))*(0.5f-0.500f*cos(-angle*(dy+0.5f)));
                // calculate modified raised cosine window weight for blending tiles to suppress artifacts (slightly adapted compared to original publication)
                norm_cosine = (0.5f-0.505f*cos(-angle*(dm+0.5f)))*(0.5f-0.505f*cos(-angle*(dy+0.5f)));
                
                // calculate coefficients
                coefRe = cos(angle*dn*dy);
                coefIm = sin(angle*dn*dy);
      
                dataRe = norm_cosine*tmp_data[dy];
                
                Re0 += (coefRe * dataRe);
                Im0 += (coefIm * dataRe);
            }
            
            // write into temporary textures
            tmp_texture_ft.write(Re0, uint2(2*m+0, n));
            tmp_texture_ft.write(Im0, uint2(2*m+1, n));
        }
    }
        
    // row-wise one-dimensional fast Fourier transform along x-direction
    // exploit symmetry of real dft and calculate reduced number of rows
    for (int dn = 0; dn <= tile_size/2; dn++) {
        
        int const n = n0 + dn;
        int const n2 = n0 + tile_size-dn;
        
        // copy data to temp vector
        for (int dm = 0; dm < tile_size; dm++) {
            tmp_data[2*dm+0] = tmp_texture_ft.read(uint2(2*(m0+dm)+0, n));
            tmp_data[2*dm+1] = tmp_texture_ft.read(uint2(2*(m0+dm)+1, n));
        }
        
        // calculate 4 small discrete Fourier transforms
        for (int dm = 0; dm < tile_size/4; dm++) {
             
            int const m = 2*(m0 + dm);
            
            // fill with zeros
            Re0 = Im0 = Re1 = Im1 = Re2 = Im2 = Re3 = Im3 = zeros;
            Re0_cs = Im0_cs = Re1_cs = Im1_cs = Re2_cs = Im2_cs = Re3_cs = Im3_cs = zeros;
            
            for (int dx = 0; dx < tile_size; dx+=4) {
              
                // calculate coefficients
                coefRe = cos(angle*dm*dx);
                coefIm = sin(angle*dm*dx);
                
                // DFT0
                dataRe = tmp_data[2*dx+0];
                dataIm = tmp_data[2*dx+1];
                Re0   += (coefRe*dataRe - coefIm*dataIm);
                Im0   += (coefIm*dataRe + coefRe*dataIm);
                Re0_cs+= (coefRe*dataRe + coefIm*dataIm);
                Im0_cs+= (coefIm*dataRe - coefRe*dataIm);
                // DFT1
                dataRe = tmp_data[2*dx+2];
                dataIm = tmp_data[2*dx+3];
                Re2   += (coefRe*dataRe - coefIm*dataIm);
                Im2   += (coefIm*dataRe + coefRe*dataIm);
                Re2_cs+= (coefRe*dataRe + coefIm*dataIm);
                Im2_cs+= (coefIm*dataRe - coefRe*dataIm);
                // DFT2
                dataRe = tmp_data[2*dx+4];
                dataIm = tmp_data[2*dx+5];
                Re1   += (coefRe*dataRe - coefIm*dataIm);
                Im1   += (coefIm*dataRe + coefRe*dataIm);
                Re1_cs+= (coefRe*dataRe + coefIm*dataIm);
                Im1_cs+= (coefIm*dataRe - coefRe*dataIm);
                // DFT3
                dataRe = tmp_data[2*dx+6];
                dataIm = tmp_data[2*dx+7];
                Re3   += (coefRe*dataRe - coefIm*dataIm);
                Im3   += (coefIm*dataRe + coefRe*dataIm);
                Re3_cs+= (coefRe*dataRe + coefIm*dataIm);
                Im3_cs+= (coefIm*dataRe - coefRe*dataIm);
            }
            
            // first butterfly to combine results
            coefRe  = cos(angle*2*dm);
            coefIm  = sin(angle*2*dm);
            Re00    = Re0    + coefRe*Re1    - coefIm*Im1;
            Im00    = Im0    + coefIm*Re1    + coefRe*Im1;
            Re22    = Re2    + coefRe*Re3    - coefIm*Im3;
            Im22    = Im2    + coefIm*Re3    + coefRe*Im3;
            Re00_cs = Re0_cs + coefRe*Re1_cs - coefIm*Im1_cs;
            Im00_cs = Im0_cs + coefIm*Re1_cs + coefRe*Im1_cs;
            Re22_cs = Re2_cs + coefRe*Re3_cs - coefIm*Im3_cs;
            Im22_cs = Im2_cs + coefIm*Re3_cs + coefRe*Im3_cs;
                        
            coefRe  = cos(angle*2*(dm+tile_size_14));
            coefIm  = sin(angle*2*(dm+tile_size_14));
            Re11    = Re0    + coefRe*Re1    - coefIm*Im1;
            Im11    = Im0    + coefIm*Re1    + coefRe*Im1;
            Re33    = Re2    + coefRe*Re3    - coefIm*Im3;
            Im33    = Im2    + coefIm*Re3    + coefRe*Im3;
            Re11_cs = Re0_cs + coefRe*Re1_cs - coefIm*Im1_cs;
            Im11_cs = Im0_cs + coefIm*Re1_cs + coefRe*Im1_cs;
            Re33_cs = Re2_cs + coefRe*Re3_cs - coefIm*Im3_cs;
            Im33_cs = Im2_cs + coefIm*Re3_cs + coefRe*Im3_cs;
            
            // second butterfly to combine results
            coefRe = cos(angle*dm);
            coefIm = sin(angle*dm);
            Re0    = Re00    + coefRe*Re22    - coefIm*Im22;
            Im0    = Im00    + coefIm*Re22    + coefRe*Im22;
            Re0_cs = Re00_cs + coefRe*Re22_cs - coefIm*Im22_cs;
            Im0_cs = Im00_cs + coefIm*Re22_cs + coefRe*Im22_cs;
            
            coefRe = cos(angle*(dm+tile_size_24));
            coefIm = sin(angle*(dm+tile_size_24));
            Re2    = Re00    + coefRe*Re22    - coefIm*Im22;
            Im2    = Im00    + coefIm*Re22    + coefRe*Im22;
            Re2_cs = Re00_cs + coefRe*Re22_cs - coefIm*Im22_cs;
            Im2_cs = Im00_cs + coefIm*Re22_cs + coefRe*Im22_cs;
            
            coefRe = cos(angle*(dm+tile_size_14));
            coefIm = sin(angle*(dm+tile_size_14));
            Re1    = Re11    + coefRe*Re33    - coefIm*Im33;
            Im1    = Im11    + coefIm*Re33    + coefRe*Im33;
            Re1_cs = Re11_cs + coefRe*Re33_cs - coefIm*Im33_cs;
            Im1_cs = Im11_cs + coefIm*Re33_cs + coefRe*Im33_cs;
            
            coefRe = cos(angle*(dm+tile_size_34));
            coefIm = sin(angle*(dm+tile_size_34));
            Re3    = Re11    + coefRe*Re33    - coefIm*Im33;
            Im3    = Im11    + coefIm*Re33    + coefRe*Im33;
            Re3_cs = Re11_cs + coefRe*Re33_cs - coefIm*Im33_cs;
            Im3_cs = Im11_cs + coefIm*Re33_cs + coefRe*Im33_cs;
                           
            // write into output texture
            out_texture_ft.write(Re0, uint2(m+0, n));
            out_texture_ft.write(Im0, uint2(m+1, n));
            out_texture_ft.write(Re1, uint2(m+tile_size_24+0, n));
            out_texture_ft.write(Im1, uint2(m+tile_size_24+1, n));
            out_texture_ft.write(Re2, uint2(m+tile_size+0, n));
            out_texture_ft.write(Im2, uint2(m+tile_size+1, n));
            out_texture_ft.write(Re3, uint2(m+tile_size_24*3+0, n));
            out_texture_ft.write(Im3, uint2(m+tile_size_24*3+1, n));
              
            // exploit symmetry of real dft and calculate values for remaining rows
            if(n2 < n0+tile_size & n2 != n0+tile_size/2)
            {
                // write into output texture
                out_texture_ft.write(Re0_cs, uint2(m+0, n2));
                out_texture_ft.write(Im0_cs, uint2(m+1, n2));
                out_texture_ft.write(Re1_cs, uint2(m+tile_size_24+0, n2));
                out_texture_ft.write(Im1_cs, uint2(m+tile_size_24+1, n2));
                out_texture_ft.write(Re2_cs, uint2(m+tile_size+0, n2));
                out_texture_ft.write(Im2_cs, uint2(m+tile_size+1, n2));
                out_texture_ft.write(Re3_cs, uint2(m+tile_size_24*3+0, n2));
                out_texture_ft.write(Im3_cs, uint2(m+tile_size_24*3+1, n2));
            }
        }
    }
}


kernel void backward_fft(texture2d<float, access::read> in_texture_ft [[texture(0)]],
                         texture2d<float, access::read_write> tmp_texture_ft [[texture(1)]],
                         texture2d<float, access::write> out_texture [[texture(2)]],
                         constant int& tile_size [[buffer(0)]],
                         constant int& n_textures [[buffer(1)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
    
    int const tile_size_14 = tile_size/4;
    int const tile_size_24 = tile_size/2;
    int const tile_size_34 = tile_size/4*3;
    
    // pre-calculate factors for sine and cosine calculation
    float const angle = -2*PI/float(tile_size);
    
    // pre-initalize some vectors
    float4 const zeros       = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 const norm_factor = float4(float(n_textures*tile_size*tile_size), float(n_textures*tile_size*tile_size), float(n_textures*tile_size*tile_size), float(n_textures*tile_size*tile_size));
       
    float coefRe, coefIm;
    float4 Re0, Re1, Re2, Re3, Im0, Im1, Im2, Im3, Re00, Re11, Re22, Re33, Im00, Im11, Im22, Im33, dataRe, dataIm;
    float4 tmp_data[32];
    
    // row-wise one-dimensional fast Fourier transform along x-direction
    for (int dn = 0; dn < tile_size; dn++) {
        
        int const n = n0 + dn;
        
        // copy data to temp vector
        for (int dm = 0; dm < tile_size; dm++) {
            tmp_data[2*dm+0] = in_texture_ft.read(uint2(2*(m0+dm)+0, n));
            tmp_data[2*dm+1] = in_texture_ft.read(uint2(2*(m0+dm)+1, n));
        }
        
        // calculate 4 small discrete Fourier transforms
        for (int dm = 0; dm < tile_size/4; dm++) {
             
            int const m = 2*(m0 + dm);
            
            // fill with zeros
            Re0 = Im0 = Re1 = Im1 = Re2 = Im2 = Re3 = Im3 = zeros;
            
            for (int dx = 0; dx < tile_size; dx+=4) {
                
                // calculate coefficients
                coefRe = cos(angle*dm*dx);
                coefIm = sin(angle*dm*dx);
                
                // DFT0
                dataRe =  tmp_data[2*dx+0];
                dataIm = -tmp_data[2*dx+1];
                Re0   += (coefRe*dataRe - coefIm*dataIm);
                Im0   += (coefIm*dataRe + coefRe*dataIm);
                // DFT1
                dataRe =  tmp_data[2*dx+2];
                dataIm = -tmp_data[2*dx+3];
                Re2   += (coefRe*dataRe - coefIm*dataIm);
                Im2   += (coefIm*dataRe + coefRe*dataIm);
                // DFT2
                dataRe =  tmp_data[2*dx+4];
                dataIm = -tmp_data[2*dx+5];
                Re1   += (coefRe*dataRe - coefIm*dataIm);
                Im1   += (coefIm*dataRe + coefRe*dataIm);
                //DFT3
                dataRe =  tmp_data[2*dx+6];
                dataIm = -tmp_data[2*dx+7];
                Re3   += (coefRe*dataRe - coefIm*dataIm);
                Im3   += (coefIm*dataRe + coefRe*dataIm);
            }
            
            // first butterfly to combine results
            coefRe = cos(angle*2*dm);
            coefIm = sin(angle*2*dm);
            Re00 = Re0 + coefRe*Re1 - coefIm*Im1;
            Im00 = Im0 + coefIm*Re1 + coefRe*Im1;
            Re22 = Re2 + coefRe*Re3 - coefIm*Im3;
            Im22 = Im2 + coefIm*Re3 + coefRe*Im3;
                        
            coefRe = cos(angle*2*(dm+tile_size_14));
            coefIm = sin(angle*2*(dm+tile_size_14));
            Re11 = Re0 + coefRe*Re1 - coefIm*Im1;
            Im11 = Im0 + coefIm*Re1 + coefRe*Im1;
            Re33 = Re2 + coefRe*Re3 - coefIm*Im3;
            Im33 = Im2 + coefIm*Re3 + coefRe*Im3;
            
            // second butterfly to combine results
            Re0 = Re00 + cos(angle*dm)*Re22                - sin(angle*dm)*Im22;
            Im0 = Im00 + sin(angle*dm)*Re22                + cos(angle*dm)*Im22;
            Re2 = Re00 + cos(angle*(dm+tile_size_24))*Re22 - sin(angle*(dm+tile_size_24))*Im22;
            Im2 = Im00 + sin(angle*(dm+tile_size_24))*Re22 + cos(angle*(dm+tile_size_24))*Im22;
            Re1 = Re11 + cos(angle*(dm+tile_size_14))*Re33 - sin(angle*(dm+tile_size_14))*Im33;
            Im1 = Im11 + sin(angle*(dm+tile_size_14))*Re33 + cos(angle*(dm+tile_size_14))*Im33;
            Re3 = Re11 + cos(angle*(dm+tile_size_34))*Re33 - sin(angle*(dm+tile_size_34))*Im33;
            Im3 = Im11 + sin(angle*(dm+tile_size_34))*Re33 + cos(angle*(dm+tile_size_34))*Im33;
               
            // write into temporary textures
            tmp_texture_ft.write( Re0, uint2(m+0, n));
            tmp_texture_ft.write(-Im0, uint2(m+1, n));
            tmp_texture_ft.write( Re1, uint2(m+tile_size_24+0, n));
            tmp_texture_ft.write(-Im1, uint2(m+tile_size_24+1, n));
            tmp_texture_ft.write( Re2, uint2(m+tile_size+0, n));
            tmp_texture_ft.write(-Im2, uint2(m+tile_size+1, n));
            tmp_texture_ft.write( Re3, uint2(m+tile_size_24*3+0, n));
            tmp_texture_ft.write(-Im3, uint2(m+tile_size_24*3+1, n));
        }
    }
  
    // column-wise one-dimensional fast Fourier transform along y-direction
    for (int dm = 0; dm < tile_size; dm++) {
        
        int const m = m0 + dm;
        
        // copy data to temp vector
        for (int dn = 0; dn < tile_size; dn++) {
            tmp_data[2*dn+0] = tmp_texture_ft.read(uint2(2*m+0, n0+dn));
            tmp_data[2*dn+1] = tmp_texture_ft.read(uint2(2*m+1, n0+dn));
        }
        
        // calculate 4 small discrete Fourier transforms
        for (int dn = 0; dn < tile_size/4; dn++) {
                          
            int const n = n0 + dn;
            
            // fill with zeros
            Re0 = Im0 = Re1 = Im1 = Re2 = Im2 = Re3 = Im3 = zeros;
            
            for (int dy = 0; dy < tile_size; dy+=4) {
              
                // calculate coefficients
                coefRe = cos(angle*dn*dy);
                coefIm = sin(angle*dn*dy);
                
                // DFT0
                dataRe =  tmp_data[2*dy+0];
                dataIm = -tmp_data[2*dy+1];
                Re0   += (coefRe*dataRe - coefIm*dataIm);
                Im0   += (coefIm*dataRe + coefRe*dataIm);
                // DFT1
                dataRe =  tmp_data[2*dy+2];
                dataIm = -tmp_data[2*dy+3];
                Re2   += (coefRe*dataRe - coefIm*dataIm);
                Im2   += (coefIm*dataRe + coefRe*dataIm);
                // DFT2
                dataRe =  tmp_data[2*dy+4];
                dataIm = -tmp_data[2*dy+5];
                Re1   += (coefRe*dataRe - coefIm*dataIm);
                Im1   += (coefIm*dataRe + coefRe*dataIm);
                // DFT3
                dataRe =  tmp_data[2*dy+6];
                dataIm = -tmp_data[2*dy+7];
                Re3   += (coefRe*dataRe - coefIm*dataIm);
                Im3   += (coefIm*dataRe + coefRe*dataIm);
            }
            
            // first butterfly to combine results
            coefRe = cos(angle*2*dn);
            coefIm = sin(angle*2*dn);
            Re00 = Re0 + coefRe*Re1 - coefIm*Im1;
            Im00 = Im0 + coefIm*Re1 + coefRe*Im1;
            Re22 = Re2 + coefRe*Re3 - coefIm*Im3;
            Im22 = Im2 + coefIm*Re3 + coefRe*Im3;
                        
            coefRe = cos(angle*2*(dn+tile_size_14));
            coefIm = sin(angle*2*(dn+tile_size_14));
            Re11 = Re0 + coefRe*Re1 - coefIm*Im1;
            Im11 = Im0 + coefIm*Re1 + coefRe*Im1;
            Re33 = Re2 + coefRe*Re3 - coefIm*Im3;
            Im33 = Im2 + coefIm*Re3 + coefRe*Im3;
            
            // second butterfly to combine results
            Re0 = Re00 + cos(angle*dn)*Re22                - sin(angle*dn)*Im22;
            Re2 = Re00 + cos(angle*(dn+tile_size_24))*Re22 - sin(angle*(dn+tile_size_24))*Im22;
            Re1 = Re11 + cos(angle*(dn+tile_size_14))*Re33 - sin(angle*(dn+tile_size_14))*Im33;
            Re3 = Re11 + cos(angle*(dn+tile_size_34))*Re33 - sin(angle*(dn+tile_size_34))*Im33;
                          
            // write into temporary textures
            out_texture.write(Re0/norm_factor, uint2(m, n));
            out_texture.write(Re1/norm_factor, uint2(m, n+tile_size_14));
            out_texture.write(Re2/norm_factor, uint2(m, n+tile_size_24));
            out_texture.write(Re3/norm_factor, uint2(m, n+tile_size_34));
        }
    }
}
