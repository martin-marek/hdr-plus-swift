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
    
    float4 val_float = float4(val_unint);
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
                          constant int& mosaic_pattern_width [[buffer(1)]],
                          uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_width = in_texture.get_width();
    
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
    
    // compute a sigle output pixel
    float total_intensity = 0;
    float total_weight = 0;
    int y = gid.y;
    
    for (int dx = -kernel_size_trunc; dx <= kernel_size_trunc; dx++) {
        int x = gid.x + mosaic_pattern_width*dx;
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
                          constant int& mosaic_pattern_width [[buffer(1)]],
                          uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_height = in_texture.get_height();
    
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
    
    // compute a sigle output pixel
    float total_intensity = 0;
    float total_weight = 0;
    int x = gid.x;
    
    for (int dy = -kernel_size_trunc; dy <= kernel_size_trunc; dy++) {
        int y = gid.y + mosaic_pattern_width*dy;
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


// generic function for computation of tile differences that works for any search distance
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
                
                diff += abs(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r + 2*UINT16_MAX_VAL);
            } else {
                
                diff += abs(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
        }
    }
    
    // store tile difference
    tile_diff.write(diff, gid);
}


// highly-optimized function for computation of tile differences that works only for search distance 2 (in total 25 combinations)
// The aim of this function is to reduce the number of memory accesses required compared to the more simple function compute_tile_differences() while providing equal results. As the alignment always checks shifts on a 5x5 pixel grid, a simple implementation would read 25 pixels in the comparison texture for each pixel in the reference texture. This optimized function however uses a buffer vector covering 5 complete rows of the texture that slides line by line through the comparison texture and reduces the number of memory reads considerably.
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
     
    int ref_tile_x, ref_tile_y, comp_tile_x, comp_tile_y, tmp_index, dx_i, dy_i;
    
    // compute tile position if previous alignment were 0
    int x0 = int(floor( float(gid.x)/float(n_tiles_x-1) * (texture_width  - tile_size - 1) ));
    int y0 = int(floor( float(gid.y)/float(n_tiles_y-1) * (texture_height - tile_size - 1) ));
    
    // factor in previous alignmnet
    int4 const prev_align = prev_alignment.read(uint2(gid.x, gid.y));
    int const dx0 = downscale_factor * prev_align.x;
    int const dy0 = downscale_factor * prev_align.y;
    
    float diff[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float tmp_ref;
    float tmp_comp[5*68];
    
    // loop over first 4 rows of comp_texture
    for (int dy = -2; dy < +2; dy++) {
        
        // loop over columns of comp_texture to copy first 4 rows of comp_texture into tmp_comp
        for (int dx = -2; dx < tile_size+2; dx++) {
            
            comp_tile_x = x0 + dx0 + dx;
            comp_tile_y = y0 + dy0 + dy;
            
            // index of corresponding pixel value in tmp_comp
            tmp_index = (dy+2)*(tile_size+4) + dx+2;
            
            // if the comparison pixels are outside of the frame, attach a high loss to them
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                tmp_comp[tmp_index] = (-2*UINT16_MAX_VAL);
            } else {
                tmp_comp[tmp_index] = comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r;
            }
        }
    }
    
    // loop over rows of ref_texture
    for (int dy = 0; dy < tile_size; dy++) {
        
        // loop over columns of comp_texture to copy 1 additional row of comp_texture into tmp_comp
        for (int dx = -2; dx < tile_size+2; dx++) {
            
            comp_tile_x = x0 + dx0 + dx;
            comp_tile_y = y0 + dy0 + dy+2;
            
            // index of corresponding pixel value in tmp_comp
            tmp_index = ((dy+4)%5)*(tile_size+4) + dx+2;
            
            // if the comparison pixels are outside of the frame, attach a high loss to them
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                tmp_comp[tmp_index] = (-2*UINT16_MAX_VAL);
            } else {
                tmp_comp[tmp_index] = comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r;
            }
        }
        
        // loop over columns of ref_texture
        for (int dx = 0; dx < tile_size; dx++) {
            
            ref_tile_x = x0 + dx;
            ref_tile_y = y0 + dy;
            
            tmp_ref = ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r;
            
            // loop over 25 test displacements
            for (int i = 0; i < 25; i++) {
                
                dx_i = i % 5;
                dy_i = i / 5;
                
                // index of corresponding pixel value in tmp_comp
                tmp_index = ((dy+dy_i)%5)*(tile_size+4) + dx + dx_i;
                
                // add difference to corresponding combination
                diff[i] += abs(tmp_ref - tmp_comp[tmp_index]);
            }
        }
    }
    
    // store tile differences in texture
    for (int i = 0; i < 25; i++) {
        tile_diff.write(diff[i], uint3(gid.x, gid.y, i));
    }
}


kernel void compute_tile_alignments(texture3d<float, access::read> tile_diff [[texture(0)]],
                                    texture2d<int, access::read> prev_alignment [[texture(1)]],
                                    texture2d<int, access::write> current_alignment [[texture(2)]],
                                    constant int& downscale_factor [[buffer(0)]],
                                    constant int& search_dist [[buffer(1)]],
                                    uint2 gid [[thread_position_in_grid]]) {
    // load args
    int n_pos_1d = 2*search_dist + 1;
    int n_pos_2d = n_pos_1d * n_pos_1d;
    
    // find tile displacement with the lowest pixel difference
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
    
    // factor in previous alignment
    int4 prev_align = prev_alignment.read(gid);
    dx += downscale_factor * prev_align.x;
    dy += downscale_factor * prev_align.y;
    
    // store alignment
    int4 out = int4(dx, dy, 0, 0);
    current_alignment.write(out, gid);
}


// At transitions between moving objects and non-moving background, the alignment vectors from downsampled images may be inaccurate. Therefore, after upsampling to the next resolution level, three candidate alignment vectors are evaluated for each tile. In addition to the vector obtained from upsampling, two vectors from neighboring tiles are checked. As a consequence, alignment at the transition regions described above is more accurate.
// see section on "Hierarchical alignment" in https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf and section "Multi-scale Pyramid Alignment" in https://www.ipol.im/pub/art/2021/336/
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
     
    // initialize some variables
    int comp_tile_x, comp_tile_y, tmp_tile_x, tmp_tile_y;
    float tmp_ref[64];
    
    // compute tile position if previous alignment were 0
    int const x0 = int(floor( float(gid.x)/float(n_tiles_x-1) * (texture_width  - tile_size - 1) ));
    int const y0 = int(floor( float(gid.y)/float(n_tiles_y-1) * (texture_height - tile_size - 1) ));

    // calculate shifts of gid index for 3 candidate alignments to evaluate
    int3 const x_shift = int3(0, ((gid.x%2 == 0) ? -1 : 1), 0);
    int3 const y_shift = int3(0, 0, ((gid.y%2 == 0) ? -1 : 1));
    
    int3 const x = clamp(int3(gid.x+x_shift), 0, n_tiles_x-1);
    int3 const y = clamp(int3(gid.y+y_shift), 0, n_tiles_y-1);
    
    // factor in previous alignment for 3 candidates
    int4 const prev_align0 = prev_alignment.read(uint2(x[0], y[0]));
    int4 const prev_align1 = prev_alignment.read(uint2(x[1], y[1]));
    int4 const prev_align2 = prev_alignment.read(uint2(x[2], y[2]));
    
    int3 const dx0 = downscale_factor * int3(prev_align0.x, prev_align1.x, prev_align2.x);
    int3 const dy0 = downscale_factor * int3(prev_align0.y, prev_align1.y, prev_align2.y);
     
    // compute tile differences for 3 candidates
    float diff[3] = {0.0f, 0.0f, 0.0f};
        
    // loop over all rows
    for (int dy = 0; dy < tile_size; dy += 64/tile_size) {
           
        // copy 64/tile_size rows into temp vector
        for (int i = 0; i < 64; i++) {            
            tmp_ref[i] = ref_texture.read(uint2(x0+(i % tile_size), y0+dy+int(i / tile_size))).r;
        }
        
        // loop over tmp vector: candidate 0 of alignment vector
        tmp_tile_x = x0 + dx0[0];
        tmp_tile_y = y0 + dy0[0] + dy;
        for (int i = 0; i < 64; i++) {
            
            // compute the indices of the pixels to compare
            comp_tile_x = tmp_tile_x + (i % tile_size);
            comp_tile_y = tmp_tile_y + int(i / tile_size);
            
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                diff[0] += tmp_ref[i] + 2*UINT16_MAX_VAL;
            } else {
                diff[0] += abs(tmp_ref[i] - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
        }
        
        // loop over tmp vector: candidate 0 of alignment vector
        tmp_tile_x = x0 + dx0[1];
        tmp_tile_y = y0 + dy0[1] + dy;
        for (int i = 0; i < 64; i++) {
            
            // compute the indices of the pixels to compare
            comp_tile_x = tmp_tile_x + (i % tile_size);
            comp_tile_y = tmp_tile_y + int(i / tile_size);
            
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                diff[1] += tmp_ref[i] + 2*UINT16_MAX_VAL;
            } else {
                diff[1] += abs(tmp_ref[i] - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
        }
        
        // loop over tmp vector: candidate 0 of alignment vector
        tmp_tile_x = x0 + dx0[2];
        tmp_tile_y = y0 + dy0[2] + dy;
        for (int i = 0; i < 64; i++) {
            
            // compute the indices of the pixels to compare
            comp_tile_x = tmp_tile_x + (i % tile_size);
            comp_tile_y = tmp_tile_y + int(i / tile_size);
            
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                diff[2] += tmp_ref[i] + 2*UINT16_MAX_VAL;
            } else {
                diff[2] += abs(tmp_ref[i] - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
        }
    }
     
    // store corrected (best) alignment
    if(diff[0] < diff[1] & diff[0] < diff[2]) {
        prev_alignment_corrected.write(prev_align0, gid);
        
    } else if(diff[1] < diff[2]) {
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
    
    // compute the two possible tile-grid indices that the given output pixel belongs to
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


kernel void color_difference(texture2d<float, access::read> texture1 [[texture(0)]],
                             texture2d<float, access::read> texture2 [[texture(1)]],
                             texture2d<float, access::write> out_texture [[texture(2)]],
                             constant int& mosaic_pattern_width [[buffer(0)]],
                             uint2 gid [[thread_position_in_grid]]) {
    
    float total_diff = 0;
    int x0 = gid.x * mosaic_pattern_width;
    int y0 = gid.y * mosaic_pattern_width;
    
    for (int dx = 0; dx < mosaic_pattern_width; dx++) {
        for (int dy = 0; dy < mosaic_pattern_width; dy++) {
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

// see section "Noise model and tiled approximation" in https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf or section "Noise Level Estimation" in https://www.ipol.im/pub/art/2021/336/
kernel void calculate_rms_rgba(texture2d<float, access::read> ref_texture [[texture(0)]],
                               texture2d<float, access::write> rms_texture [[texture(1)]],
                               constant int& tile_size [[buffer(0)]],
                               uint2 gid [[thread_position_in_grid]]) {
    
    // compute tile positions from gid
    int const x0 = gid.x*tile_size;
    int const y0 = gid.y*tile_size;
    
    // fill with zeros
    float4 noise_est = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // use tile size merge here
    for (int dy = 0; dy < tile_size; dy++) {
        for (int dx = 0; dx < tile_size; dx++) {
      
            float4 const data_noise = ref_texture.read(uint2(x0+dx, y0+dy));
            
            noise_est += (data_noise * data_noise);
        }
    }

    noise_est = 0.25f*sqrt(noise_est)/float(tile_size);
    
    rms_texture.write(noise_est, gid);
}
    

// simple and slow discrete Fourier transform applied to each color channel independently
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
    float4 Re, Im, dataRe, dataIm;
    
    // column-wise one-dimensional discrete Fourier transform along y-direction
    for (int dm = 0; dm < tile_size; dm++) {
        for (int dn = 0; dn < tile_size; dn++) {
             
            int const m = m0 + dm;
            int const n = n0 + dn;
            
            // fill with zeros
            Re = zeros;
            Im = zeros;
            
            for (int dy = 0; dy < tile_size; dy++) {
                                  
                int const y = n0 + dy;
                
                // see section "Overlapped tiles" in https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf or section "Overlapped Tiles and Raised Cosine Window" in https://www.ipol.im/pub/art/2021/336/
                // calculate modified raised cosine window weight for blending tiles to suppress artifacts
                norm_cosine = (0.5f-0.5f*cos(-angle*(dm+0.5f)))*(0.5f-0.5f*cos(-angle*(dy+0.5f)));
                                
                // calculate coefficients
                coefRe = cos(angle*dn*dy);
                coefIm = sin(angle*dn*dy);
                
                dataRe = norm_cosine*in_texture.read(uint2(m, y));
                
                Re += (coefRe * dataRe);
                Im += (coefIm * dataRe);
            }
            
            // write into temporary textures
            tmp_texture_ft.write(Re, uint2(2*m+0, n));
            tmp_texture_ft.write(Im, uint2(2*m+1, n));
        }
    }
    
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
                           
                dataRe = tmp_texture_ft.read(uint2(x+0, n));
                dataIm = tmp_texture_ft.read(uint2(x+1, n));
                             
                Re += (coefRe*dataRe - coefIm*dataIm);
                Im += (coefIm*dataRe + coefRe*dataIm);
            }
            
            out_texture_ft.write(Re, uint2(m+0, n));
            out_texture_ft.write(Im, uint2(m+1, n));
        }
    }
}


// simple and slow discrete Fourier transform applied to each color channel independently
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
            
            // normalize result
            Re = Re/norm_factor;
            out_texture.write(Re, uint2(m, n));
        }
    }
}


// This is the most important function required for the frequency-based merging approach. It is based on ideas from several publications:
// [Hasinoff 2016]: https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf
// [Monod 2021]: https://www.ipol.im/pub/art/2021/336/
// [Liba 2019]: https://graphics.stanford.edu/papers/night-sight-sigasia19/night-sight-sigasia19.pdf
// [Delbracio 2015]: https://openaccess.thecvf.com/content_cvpr_2015/papers/Delbracio_Burst_Deblurring_Removing_2015_CVPR_paper.pdf

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
          
    // derive motion norm from mismatch texture to increase the noise reduction for small values of mismatch using a similar linear relationship as shown in Figure 9f in [Liba 2019]
    float const mismatch = mismatch_texture.read(gid).r;
    // for a smooth transition, the magnitude norm is weighted based on the mismatch
    float const mismatch_weight = clamp(1.0f - 10.0f*(mismatch-0.2f), 0.0f, 1.0f);
    
    float const motion_norm = clamp(max_motion_norm-(mismatch-0.02f)*(max_motion_norm-1.0f)/0.15f, 1.0f, max_motion_norm);
    //float const motion_norm = 1.0f
    
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
    
    // pre-calculate factors for sine and cosine calculation
    float const angle = -2*PI/float(tile_size);

    // pre-initalize some variables
    float weight, min_weight, max_weight, coefRe, coefIm, shift_x, shift_y, ratio_mag, magnitude_norm;
    float4 refRe, refIm, refMag, alignedRe, alignedIm, alignedRe2, alignedIm2, alignedMag2, mergedRe, mergedIm, weight4, diff4;
    float total_diff[81];
    
    // fill with zeros
    for(int i = 0; i < 81; i++) {
        total_diff[i] = 0.0f;
    }
    
    // subpixel alignment based on the Fourier shift theorem: test shifts between -0.5 and +0.5 pixels specified on the pixel scale of each color channel, which corresponds to -1.0 and +1.0 pixels specified on the original pixel scale
    for (int dn = 0; dn < tile_size; dn++) {
        for (int dm = 0; dm < tile_size; dm++) {
            
            int const m = 2*(m0 + dm);
            int const n = n0 + dn;
            
            // extract complex frequency data of reference tile and aligned comparison tile
            refRe = ref_texture_ft.read(uint2(m+0, n));
            refIm = ref_texture_ft.read(uint2(m+1, n));
            
            alignedRe = aligned_texture_ft.read(uint2(m+0, n));
            alignedIm = aligned_texture_ft.read(uint2(m+1, n));
            
            // test 9x9 discrete steps
            for (int i = 0; i < 81; i++) {
                  
                // potential shift in pixels (specified on the pixel scale of each color channel)
                shift_x = -0.5f + int(i % 9) * 0.125f;
                shift_y = -0.5f + int(i / 9) * 0.125f;
                            
                // calculate coefficients for Fourier shift
                coefRe = cos(angle*(dm*shift_x+dn*shift_y));
                coefIm = sin(angle*(dm*shift_x+dn*shift_y));
                         
                // calculate complex frequency data of shifted tile
                alignedRe2 = (coefRe*alignedRe - coefIm*alignedIm);
                alignedIm2 = (coefIm*alignedRe + coefRe*alignedIm);
                
                diff4 = (refRe-alignedRe2)*(refRe-alignedRe2) + (refIm-alignedIm2)*(refIm-alignedIm2);
                
                // add magnitudes of differences
                total_diff[i] += (diff4[0]+diff4[1]+diff4[2]+diff4[3]);
            }
        }
    }
    
    // find best shift (which has the lowest total difference)
    float best_diff = 1e20f;
    int   best_i    = 0;
    
    for (int i = 0; i < 81; i++) {
        
        if(total_diff[i] < best_diff) {
            
            best_diff = total_diff[i];
            best_i    = i;
        }
    }
    
    // extract best shifts
    float const best_shift_x = -0.5f + int(best_i % 9) * 0.125f;
    float const best_shift_y = -0.5f + int(best_i / 9) * 0.125f;
    
    // perform the merging of the reference tile and the aligned comparison tile
    for (int dn = 0; dn < tile_size; dn++) {
        for (int dm = 0; dm < tile_size; dm++) {
          
            int const m = 2*(m0 + dm);
            int const n = n0 + dn;
            
            // extract complex frequency data of reference tile and aligned comparison tile
            refRe = ref_texture_ft.read(uint2(m+0, n));
            refIm = ref_texture_ft.read(uint2(m+1, n));
            
            alignedRe = aligned_texture_ft.read(uint2(m+0, n));
            alignedIm = aligned_texture_ft.read(uint2(m+1, n));
            
            // calculate coefficients for best Fourier shift
            coefRe = cos(angle*(dm*best_shift_x+dn*best_shift_y));
            coefIm = sin(angle*(dm*best_shift_x+dn*best_shift_y));
                
            // calculate complex frequency data of shifted tile
            alignedRe2 = (coefRe*alignedRe - coefIm*alignedIm);
            alignedIm2 = (coefIm*alignedRe + coefRe*alignedIm);
                       
            // increase merging weights for images with larger frequency magnitudes and decrease weights for lower magnitudes with the idea that larger magnitudes indicate images with higher sharpness
            // this approach is inspired by equation (3) in [Delbracio 2015]
            magnitude_norm = 1.0f;
            
            // if we are not at the central frequency bin (zero frequency) and if the mismatch is low
            if(dm+dn > 0 & mismatch < 0.3f) {
                
                // calculate magnitudes of complex frequency data
                refMag      = sqrt(refRe*refRe + refIm*refIm);
                alignedMag2 = sqrt(alignedRe2*alignedRe2 + alignedIm2*alignedIm2);
                
                // calculate ratio of magnitudes
                ratio_mag = (alignedMag2[0]+alignedMag2[1]+alignedMag2[2]+alignedMag2[3])/(refMag[0]+refMag[1]+refMag[2]+refMag[3]);
                     
                // calculate additional normalization factor that increases the merging weight larger magnitudes and decreases weight for lower magnitudes           
                magnitude_norm = mismatch_weight*clamp(ratio_mag*ratio_mag*ratio_mag*ratio_mag, 0.5f, 3.0f);
            }
            
            // calculation of merging weight by Wiener shrinkage as described in the section "Robust pairwise temporal merge" and equation (7) in [Hasinoff 2016] or in the section "Spatially varying temporal merging" and equation (7) and (9) in [Liba 2019] or in section "Pairwise Wiener Temporal Denoising" and equation (11) in [Monod 2021]
            // noise_norm corresponds to the original approach described in [Hasinoff 2016] and [Monod 2021]
            // motion_norm corresponds to the additional factor proposed in [Liba 2019]
            // magnitude_norm is based on ideas from [Delbracio 2015]
            
            weight4 = (refRe-alignedRe2)*(refRe-alignedRe2) + (refIm-alignedIm2)*(refIm-alignedIm2);
            weight4 = weight4/(weight4 + magnitude_norm*motion_norm*noise_norm);
            
            // use the same weight for all color channels to reduce color artifacts as described in [Liba 2019]
            //weight = clamp(max(weight4[0], max(weight4[1], max(weight4[2], weight4[3]))), 0.0f, 1.0f);
            min_weight = min(weight4[0], min(weight4[1], min(weight4[2], weight4[3])));
            max_weight = max(weight4[0], max(weight4[1], max(weight4[2], weight4[3])));
            // instead of the maximum weight as described in the publication, use the mean value of the two central weight values, which removes the two extremes and thus should slightly increase robustness of the approach
            weight = clamp(0.5f*(weight4[0]+weight4[1]+weight4[2]+weight4[3]-min_weight-max_weight), 0.0f, 1.0f);
            
            // apply pairwise merging of two tiles as described in equation (6) in [Hasinoff 2016] or equation (10) in [Monod 2021]
            mergedRe = out_texture_ft.read(uint2(m+0, n)) + (1.0f-weight)*alignedRe2 + weight*refRe;
            mergedIm = out_texture_ft.read(uint2(m+1, n)) + (1.0f-weight)*alignedIm2 + weight*refIm;
         
            out_texture_ft.write(mergedRe, uint2(m+0, n));
            out_texture_ft.write(mergedIm, uint2(m+1, n));
        }
    }   
}


kernel void deconvolute_frequency_domain(texture2d<float, access::read_write> final_texture_ft [[texture(0)]],
                                         texture2d<float, access::read> total_mismatch_texture [[texture(1)]],
                                         constant int& tile_size [[buffer(0)]],
                                         uint2 gid [[thread_position_in_grid]]) {
    
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
 
    float4 convRe, convIm, convMag;
    float magnitude_zero, magnitude, weight;
    float cw[16];
    
    // tile size-dependent gains used for the different frequencies
    if (tile_size == 8) {
        cw[0] = 0.00f; cw[1] = 0.02f; cw[2] = 0.04f; cw[3] = 0.08f;
        cw[4] = 0.04f; cw[5] = 0.08f; cw[6] = 0.04f; cw[7] = 0.02f;
        
    } else if (tile_size == 16) {
        cw[ 0] = 0.00f; cw[ 1] = 0.01f; cw[ 2] = 0.02f; cw[ 3] = 0.03f;
        cw[ 4] = 0.04f; cw[ 5] = 0.06f; cw[ 6] = 0.08f; cw[ 7] = 0.06f;
        cw[ 8] = 0.04f; cw[ 9] = 0.06f; cw[10] = 0.08f; cw[11] = 0.06f;
        cw[12] = 0.04f; cw[13] = 0.03f; cw[14] = 0.02f; cw[15] = 0.01f;
    }
   
    float const mismatch = total_mismatch_texture.read(gid).r;
    // for a smooth transition, the deconvolution is weighted based on the mismatch
    float const mismatch_weight = clamp(1.0f - 10.0f*(mismatch-0.2f), 0.0f, 1.0f);
    
    convRe = final_texture_ft.read(uint2(2*m0+0, n0));
    convIm = final_texture_ft.read(uint2(2*m0+1, n0));
    
    convMag = sqrt(convRe*convRe + convIm*convIm);
    magnitude_zero = (convMag[0] + convMag[1] + convMag[2] + convMag[3]);
    
    for (int dn = 0; dn < tile_size; dn++) {
        for (int dm = 0; dm < tile_size; dm++) {
            
            if (dm+dn > 0 & mismatch < 0.3f) {
                
                int const m = 2*(m0 + dm);
                int const n = n0 + dn;
                
                convRe = final_texture_ft.read(uint2(m+0, n));
                convIm = final_texture_ft.read(uint2(m+1, n));
                
                convMag = sqrt(convRe*convRe + convIm*convIm);
                magnitude = (convMag[0] + convMag[1] + convMag[2] + convMag[3]);
                  
                // reduce the increase for frequencies with high magnitude
                // weight becomes 0 for ratio >= 0.05
                // weight becomes 1 for ratio <= 0.01
                weight = mismatch_weight*clamp(1.25f - 25.0f*magnitude/magnitude_zero, 0.0f, 1.0f);
                
                convRe = (1.0f+weight*cw[dm])*(1.0f+weight*cw[dn]) * convRe;
                convIm = (1.0f+weight*cw[dm])*(1.0f+weight*cw[dn]) * convIm;
                
                final_texture_ft.write(convRe, uint2(m+0, n));
                final_texture_ft.write(convIm, uint2(m+1, n));
            }
        }
    }
}


kernel void reduce_artifacts_tile_border(texture2d<float, access::read_write> output_texture [[texture(0)]],
                                         texture2d<float, access::read> ref_texture [[texture(1)]],
                                         constant int& tile_size [[buffer(0)]],
                                         constant int& black_level0 [[buffer(1)]],
                                         constant int& black_level1 [[buffer(2)]],
                                         constant int& black_level2 [[buffer(3)]],
                                         constant int& black_level3 [[buffer(4)]],
                                         uint2 gid [[thread_position_in_grid]]) {
    
    // compute tile positions from gid
    int const x0 = gid.x*tile_size;
    int const y0 = gid.y*tile_size;
 
    // set min values and max values
    float4 const min_values = float4(black_level0-1.0f, black_level1-1.0f, black_level2-1.0f, black_level3-1.0f);
    float4 const max_values = float4(float(UINT16_MAX_VAL), float(UINT16_MAX_VAL), float(UINT16_MAX_VAL), float(UINT16_MAX_VAL));
        
    float4 pixel_value;
    float norm_cosine;
    
    // pre-calculate factors for sine and cosine calculation
    float const angle = -2*PI/float(tile_size);
    
    for (int dy = 0; dy < tile_size; dy++) {
        for (int dx = 0; dx < tile_size; dx++) {
            
            int const x = x0 + dx;
            int const y = y0 + dy;
            
            // see section "Overlapped tiles" in https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf or section "Overlapped Tiles and Raised Cosine Window" in https://www.ipol.im/pub/art/2021/336/
            // calculate modified raised cosine window weight for blending tiles to suppress artifacts
            norm_cosine = (0.5f-0.5f*cos(-angle*(dx+0.5f)))*(0.5f-0.5f*cos(-angle*(dy+0.5f)));
            
            // extract RGBA pixel values
            pixel_value = output_texture.read(uint2(x, y));
            // clamp values, which reduces potential artifacts (black lines) at tile borders by removing pixels with negative entries (negative when black level is subtracted)
            pixel_value = clamp(pixel_value, norm_cosine*min_values, max_values);
            
            // blend pixel values at tile borders with reference texture
            if (dx==0 | dx==tile_size-1 | dy==0 | dy==tile_size-1) {
                
                pixel_value = 0.5f*(norm_cosine*ref_texture.read(uint2(x, y)) + pixel_value);
            }
             
            output_texture.write(pixel_value, uint2(x, y));
        }
    }
}


kernel void calculate_mismatch_rgba(texture2d<float, access::read> ref_texture [[texture(0)]],
                                    texture2d<float, access::read> aligned_texture [[texture(1)]],
                                    texture2d<float, access::read> rms_texture [[texture(2)]],
                                    texture2d<float, access::write> mismatch_texture [[texture(3)]],
                                    constant int& tile_size [[buffer(0)]],
                                    constant float& exposure_factor [[buffer(1)]],
                                    uint2 gid [[thread_position_in_grid]]) {
        
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
    
    // use only estimated shot noise here
    float4 const noise_est = rms_texture.read(gid);
    
    // estimate motion mismatch as the absolute difference of reference tile and comparison tile
    // see section "Spatially varying temporal merging" in https://graphics.stanford.edu/papers/night-sight-sigasia19/night-sight-sigasia19.pdf for more details
    // use a spatial support twice of the tile size used for merging
    
    // clamp at top/left border of image frame
    int const x_start = max(0, m0 - tile_size/2);
    int const y_start = max(0, n0 - tile_size/2);
    
    // clamp at bottom/right border of image frame
    int const x_end = min(int(ref_texture.get_width()-1),  m0 + tile_size*3/2);
    int const y_end = min(int(ref_texture.get_height()-1), n0 + tile_size*3/2);
    
    // calculate shift for cosine window to shift to range 0 - (tile_size-1)
    int const x_shift = -(m0 - tile_size/2);
    int const y_shift = -(n0 - tile_size/2);
    
    // pre-calculate factors for sine and cosine calculation
    float const angle = -2*PI/float(tile_size);
    
    float4 tile_diff = 0.0f;
    float n_total = 0.0f;
    float norm_cosine;
     
    for (int dy = y_start; dy < y_end; dy++) {
        for (int dx = x_start; dx < x_end; dx++) {
          
            // use modified raised cosine window to apply lower weights at outer regions of the patch
            norm_cosine = (0.5f-0.17f*cos(-angle*((dx+x_shift)+0.5f)))*(0.5f-0.17f*cos(-angle*((dy+y_shift)+0.5f)));
            
            tile_diff += norm_cosine * abs(ref_texture.read(uint2(dx, dy)) - aligned_texture.read(uint2(dx, dy)));
             
            n_total += norm_cosine;
        }
    }
     
    tile_diff /= n_total;

    // calculation of mismatch ratio, which is different from the Wiener shrinkage proposed in the publication above (equation (8)). The quadratic terms of the Wiener shrinkage led to a strong separation of bright and dark pixels in the mismatch texture while mismatch should be (almost) independent of pixel brightness
    float4 const mismatch4 = tile_diff / sqrt(0.5f*noise_est + 0.5f*noise_est/exposure_factor + 1.0f);   
    float const mismatch = 0.25f*(mismatch4[0] + mismatch4[1] + mismatch4[2] + mismatch4[3]);
        
    mismatch_texture.write(mismatch, gid);
}


kernel void normalize_mismatch(texture2d<float, access::read_write> mismatch_texture [[texture(0)]],
                               constant float* mean_mismatch_buffer [[buffer(0)]],
                               uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    float const mean_mismatch = mean_mismatch_buffer[0];
    
    float mismatch_norm = mismatch_texture.read(gid).r;
    
    // normalize that mean value of mismatch texture is set to 0.12, which is close to the threshold value of 0.17. For values larger than the threshold, the strength of temporal denoising is not increased anymore
    mismatch_norm *= (0.12f/(mean_mismatch + 1e-12f));
    
    // clamp to range of 0 to 1 to remove very large values
    mismatch_norm = clamp(mismatch_norm, 0.0f, 1.0f);
    
    mismatch_texture.write(mismatch_norm, gid);
}


// hot pixel correction based on the idea that hot pixels appear at the same pixel location in all images
kernel void correct_hotpixels(texture2d<float, access::read> average_texture [[texture(0)]],
                              texture2d<float, access::read> in_texture [[texture(1)]],
                              texture2d<float, access::write> out_texture [[texture(2)]],
                              constant float* mean_texture_buffer [[buffer(0)]],
                              constant int& black_level0 [[buffer(1)]],
                              constant int& black_level1 [[buffer(2)]],
                              constant int& black_level2 [[buffer(3)]],
                              constant int& black_level3 [[buffer(4)]],
                              uint2 gid [[thread_position_in_grid]]) {
       
    int const x = gid.x+2;
    int const y = gid.y+2;
    
    // load args
    float mean_texture = 0.0f;
    float black_level = 0.0f;
    
    // extract color channel-dependent mean value of the average texture of all images
    if (x%2 == 0 & y%2 == 0) {
        mean_texture = mean_texture_buffer[0] - black_level0;
        black_level = float(black_level0);
    }
    if (x%2 == 1 & y%2 == 0) {
        mean_texture = mean_texture_buffer[1] - black_level1;
        black_level = float(black_level1);
    }
    if (x%2 == 0 & y%2 == 1) {
        mean_texture = mean_texture_buffer[2] - black_level2;
        black_level = float(black_level2);
    }
    if (x%2 == 1 & y%2 == 1) {
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
    if (pixel_ratio >= 2.0f & pixel_value >= 2.0f*mean_texture) {
        
        // calculate mean value of 4 surrounding values
        float sum2 = in_texture.read(uint2(x-2, y+0)).r;
        sum2      += in_texture.read(uint2(x+2, y+0)).r;
        sum2      += in_texture.read(uint2(x+0, y-2)).r;
        sum2      += in_texture.read(uint2(x+0, y+2)).r;
        
        // calculate weight for blending to have a smooth transition for not so obvious hot pixels
        float const weight = 0.5f*min(pixel_ratio-2.0f, 2.0f);
        
        // blend values and replace hot pixel value
        out_texture.write(weight*0.25f*sum2 + (1.0f-weight)*in_texture.read(uint2(x, y)).r, uint2(x, y));
    }
}


// exposure correction in case of a burst with exposure bracketing
kernel void equalize_exposure(texture2d<float, access::read_write> comp_texture [[texture(0)]],
                             constant int& exposure_diff [[buffer(0)]],
                             constant int& black_level0 [[buffer(1)]],
                             constant int& black_level1 [[buffer(2)]],
                             constant int& black_level2 [[buffer(3)]],
                             constant int& black_level3 [[buffer(4)]],
                             uint2 gid [[thread_position_in_grid]]) {
       
    int const x = gid.x*2;
    int const y = gid.y*2;
    
    // load args
    float4 const black_level = float4(black_level0, black_level1, black_level2, black_level3);
    
    // extract pixel values of 2x2 super pixel
    float4 pixel_value = float4(comp_texture.read(uint2(x,   y)).r,
                                comp_texture.read(uint2(x+1, y)).r,
                                comp_texture.read(uint2(x,   y+1)).r,
                                comp_texture.read(uint2(x+1, y+1)).r);
    
    // calculate exposure correction factor from exposure difference
    float const corr_factor = pow(2.0f, float(exposure_diff/100.0f));
    
    // correct exposure
    pixel_value = (pixel_value - black_level)*corr_factor + black_level;
    pixel_value = clamp(pixel_value, 0.0f, float(UINT16_MAX_VAL));

    // write back into texture
    comp_texture.write(pixel_value[0], uint2(x,   y));
    comp_texture.write(pixel_value[1], uint2(x+1, y));
    comp_texture.write(pixel_value[2], uint2(x,   y+1));
    comp_texture.write(pixel_value[3], uint2(x+1, y+1));
}
 

// correction of underexposure with reinhard tone mapping operator
// inspired by https://www-old.cs.utah.edu/docs/techreports/2002/pdf/UUCS-02-001.pdf
kernel void correct_underexposure(texture2d<float, access::read_write> final_texture [[texture(0)]],
                                  constant int& exposure_bias [[buffer(0)]],
                                  constant int& white_level [[buffer(1)]],
                                  constant float& black_level0 [[buffer(2)]],
                                  constant float& black_level1 [[buffer(3)]],
                                  constant float& black_level2 [[buffer(4)]],
                                  constant float& black_level3 [[buffer(5)]],
                                  uint2 gid [[thread_position_in_grid]]) {
      
    int const x = gid.x*2;
    int const y = gid.y*2;
   
    // load args
    float4 const black_level = float4(black_level0, black_level1, black_level2, black_level3);
   
    // extract pixel values of 2x2 super pixel
    float4 pixel_value = float4(final_texture.read(uint2(x,   y)).r,
                                final_texture.read(uint2(x+1, y)).r,
                                final_texture.read(uint2(x,   y+1)).r,
                                final_texture.read(uint2(x+1, y+1)).r);
    
    // calculate gain for intensity correction
    float const correction_stops = float(-exposure_bias/100.0f);
    
    // the gain is limited to 3.5 stops and slightly damped for values > 1.5 stops
    float gain = min(correction_stops, 3.5f);
    gain = pow(2.0f, gain-0.1f*max(0.0f, gain-1.5f));
    
    // subtract black level and rescale intensity to range from 0 to 1
    pixel_value = (pixel_value-black_level)/(float(white_level)-black_level);
    
    // use luminance to ensure that the four color channels are treated equally
    float const luminance_before = 0.25f*(pixel_value[0]+pixel_value[1]+pixel_value[2]+pixel_value[3]);
    // apply gain
    float luminance_after = gain * luminance_before;
        
    // apply tone mappting operator specified in equation (4) in https://www-old.cs.utah.edu/docs/techreports/2002/pdf/UUCS-02-001.pdf
    // the operator is linear in the shadows and midtones while protecting the highlights
    luminance_after = luminance_after * (1.0f+luminance_after/(gain*gain)) / (1.0f+luminance_after);
    
    // apply scaling derived from luminance values and return to original intensity scale
    pixel_value = pixel_value * luminance_after/luminance_before * (float(white_level)-black_level) + black_level;
    pixel_value = clamp(pixel_value, 0.0f, float(UINT16_MAX_VAL));

    final_texture.write(pixel_value[0], uint2(x,   y));
    final_texture.write(pixel_value[1], uint2(x+1, y));
    final_texture.write(pixel_value[2], uint2(x,   y+1));
    final_texture.write(pixel_value[3], uint2(x+1, y+1));
}


// highly-optimized fast Fourier transform applied to each color channel independently
// The aim of this function is to provide improved performance compared to the more simple function forward_dft() while providing equal results. It uses the following features for reduced calculation times:
// - the four color channels are stored as a float4 and all calculations employ SIMD instructions.
// - the one-dimensional transformation along y-direction is a discrete Fourier transform. As the input image is real-valued, the frequency domain representation is symmetric and only values for N/2+1 rows have to be calculated.
// - the one-dimensional transformation along x-direction employs the fast Fourier transform algorithm: At first, 4 small DFTs are calculated and then final results are obtained by two steps of cross-combination of values (based on a so-called butterfly diagram). This approach reduces the total number of memory reads and computational steps considerably.
// - due to the symmetry mentioned earlier, only N/2+1 rows have to be transformed and the remaining N/2-1 rows can be directly inferred.
kernel void forward_fft(texture2d<float, access::read> in_texture [[texture(0)]],
                        texture2d<float, access::read_write> tmp_texture_ft [[texture(1)]],
                        texture2d<float, access::write> out_texture_ft [[texture(2)]],
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
    
    float coefRe, coefIm, norm_cosine0, norm_cosine1;
    float4 Re0, Re1, Re2, Re3, Re00, Re11, Re22, Re33, Im0, Im1, Im2, Im3, Im00, Im11, Im22, Im33, dataRe, dataIm;
    float4 tmp_data[32];
    
    // column-wise one-dimensional discrete Fourier transform along y-direction
    for (int dm = 0; dm < tile_size; dm+=2) {
        
        int const m = m0 + dm;
        
        // copy data to temp vector
        for (int dn = 0; dn < tile_size; dn++) {
            tmp_data[2*dn+0] = in_texture.read(uint2(m+0, n0+dn));
            tmp_data[2*dn+1] = in_texture.read(uint2(m+1, n0+dn));
        }
        
        // exploit symmetry of real dft and calculate reduced number of rows
        for (int dn = 0; dn <= tile_size/2; dn++) {
                        
            int const n = n0 + dn;
            
            // fill with zeros
            Re0 = zeros;
            Im0 = zeros;            
            Re1 = zeros;
            Im1 = zeros;
            
            for (int dy = 0; dy < tile_size; dy++) {
      
                // see section "Overlapped tiles" in https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf or section "Overlapped Tiles and Raised Cosine Window" in https://www.ipol.im/pub/art/2021/336/
                // calculate modified raised cosine window weight for blending tiles to suppress artifacts
                norm_cosine0 = (0.5f-0.5f*cos(-angle*(dm+0.5f)))*(0.5f-0.5f*cos(-angle*(dy+0.5f)));
                norm_cosine1 = (0.5f-0.5f*cos(-angle*(dm+1.5f)))*(0.5f-0.5f*cos(-angle*(dy+0.5f)));
                         
                // calculate coefficients
                coefRe = cos(angle*dn*dy);
                coefIm = sin(angle*dn*dy);
      
                dataRe = norm_cosine0*tmp_data[2*dy+0];
                Re0 += (coefRe * dataRe);
                Im0 += (coefIm * dataRe);
                
                dataRe = norm_cosine1*tmp_data[2*dy+1];
                Re1 += (coefRe * dataRe);
                Im1 += (coefIm * dataRe);
            }
            
            // write into temporary textures
            tmp_texture_ft.write(Re0, uint2(2*m+0, n));
            tmp_texture_ft.write(Im0, uint2(2*m+1, n));
            tmp_texture_ft.write(Re1, uint2(2*m+2, n));
            tmp_texture_ft.write(Im1, uint2(2*m+3, n));
        }
    }
        
    // row-wise one-dimensional fast Fourier transform along x-direction
    // exploit symmetry of real dft and calculate reduced number of rows
    for (int dn = 0; dn <= tile_size/2; dn++) {
        
        int const n = n0 + dn;        
        
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
            
            for (int dx = 0; dx < tile_size; dx+=4) {
              
                // calculate coefficients
                coefRe = cos(angle*dm*dx);
                coefIm = sin(angle*dm*dx);
                                
                // DFT0
                dataRe = tmp_data[2*dx+0];
                dataIm = tmp_data[2*dx+1];
                Re0   += (coefRe*dataRe - coefIm*dataIm);
                Im0   += (coefIm*dataRe + coefRe*dataIm);
                // DFT1
                dataRe = tmp_data[2*dx+2];
                dataIm = tmp_data[2*dx+3];
                Re2   += (coefRe*dataRe - coefIm*dataIm);
                Im2   += (coefIm*dataRe + coefRe*dataIm);
                // DFT2
                dataRe = tmp_data[2*dx+4];
                dataIm = tmp_data[2*dx+5];
                Re1   += (coefRe*dataRe - coefIm*dataIm);
                Im1   += (coefIm*dataRe + coefRe*dataIm);
                // DFT3
                dataRe = tmp_data[2*dx+6];
                dataIm = tmp_data[2*dx+7];
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
                           
            // write into output texture
            out_texture_ft.write(Re0, uint2(m+0, n));
            out_texture_ft.write(Im0, uint2(m+1, n));
            out_texture_ft.write(Re1, uint2(m+tile_size_24+0, n));
            out_texture_ft.write(Im1, uint2(m+tile_size_24+1, n));
            out_texture_ft.write(Re2, uint2(m+tile_size+0, n));
            out_texture_ft.write(Im2, uint2(m+tile_size+1, n));
            out_texture_ft.write(Re3, uint2(m+tile_size_24*3+0, n));
            out_texture_ft.write(Im3, uint2(m+tile_size_24*3+1, n));
              
            // exploit symmetry of real dft and set values for remaining rows
            if(dn > 0 & dn != tile_size/2)
            {
                int const n2 = n0 + tile_size-dn;
                //int const m20 = 2*(m0 + (dm==0 ? 0 : tile_size-dm));
                int const m20 = 2*(m0 + min(dm, 1)*(tile_size-dm));
                int const m21 = 2*(m0 + tile_size-dm-tile_size_14);
                int const m22 = 2*(m0 + tile_size-dm-tile_size_24);
                int const m23 = 2*(m0 + tile_size-dm-tile_size_14*3);
                
                // write into output texture
                out_texture_ft.write( Re0, uint2(m20+0, n2));
                out_texture_ft.write(-Im0, uint2(m20+1, n2));
                out_texture_ft.write( Re1, uint2(m21+0, n2));
                out_texture_ft.write(-Im1, uint2(m21+1, n2));
                out_texture_ft.write( Re2, uint2(m22+0, n2));
                out_texture_ft.write(-Im2, uint2(m22+1, n2));
                out_texture_ft.write( Re3, uint2(m23+0, n2));
                out_texture_ft.write(-Im3, uint2(m23+1, n2));
            }
        }
    }
}


// highly-optimized fast Fourier transform applied to each color channel independently
// The aim of this function is to provide improved performance compared to the more simple function backward_dft() while providing equal results. It uses the following features for reduced calculation times:
// - the four color channels are stored as a float4 and all calculations employ SIMD instructions.
// - the one-dimensional transformation along y-direction employs the fast Fourier transform algorithm: At first, 4 small DFTs are calculated and then final results are obtained by two steps of cross-combination of values (based on a so-called butterfly diagram). This approach reduces the total number of memory reads and computational steps considerably.
// - the one-dimensional transformation along x-direction employs the fast Fourier transform algorithm: At first, 4 small DFTs are calculated and then final results are obtained by two steps of cross-combination of values (based on a so-called butterfly diagram). This approach reduces the total number of memory reads and computational steps considerably.
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
                dataRe = tmp_data[2*dx+0];
                dataIm = tmp_data[2*dx+1];
                Re0   += (coefRe*dataRe + coefIm*dataIm);
                Im0   += (coefIm*dataRe - coefRe*dataIm);
                // DFT1
                dataRe = tmp_data[2*dx+2];
                dataIm = tmp_data[2*dx+3];
                Re2   += (coefRe*dataRe + coefIm*dataIm);
                Im2   += (coefIm*dataRe - coefRe*dataIm);
                // DFT2
                dataRe = tmp_data[2*dx+4];
                dataIm = tmp_data[2*dx+5];
                Re1   += (coefRe*dataRe + coefIm*dataIm);
                Im1   += (coefIm*dataRe - coefRe*dataIm);
                //DFT3
                dataRe = tmp_data[2*dx+6];
                dataIm = tmp_data[2*dx+7];
                Re3   += (coefRe*dataRe + coefIm*dataIm);
                Im3   += (coefIm*dataRe - coefRe*dataIm);
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
                dataRe = tmp_data[2*dy+0];
                dataIm = tmp_data[2*dy+1];
                Re0   += (coefRe*dataRe + coefIm*dataIm);
                Im0   += (coefIm*dataRe - coefRe*dataIm);
                // DFT1
                dataRe = tmp_data[2*dy+2];
                dataIm = tmp_data[2*dy+3];
                Re2   += (coefRe*dataRe + coefIm*dataIm);
                Im2   += (coefIm*dataRe - coefRe*dataIm);
                // DFT2
                dataRe = tmp_data[2*dy+4];
                dataIm = tmp_data[2*dy+5];
                Re1   += (coefRe*dataRe + coefIm*dataIm);
                Im1   += (coefIm*dataRe - coefRe*dataIm);
                // DFT3
                dataRe = tmp_data[2*dy+6];
                dataIm = tmp_data[2*dy+7];
                Re3   += (coefRe*dataRe + coefIm*dataIm);
                Im3   += (coefIm*dataRe - coefRe*dataIm);
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
                      
            // write into output textures
            out_texture.write(Re0/norm_factor, uint2(m, n));
            out_texture.write(Re1/norm_factor, uint2(m, n+tile_size_14));
            out_texture.write(Re2/norm_factor, uint2(m, n+tile_size_24));
            out_texture.write(Re3/norm_factor, uint2(m, n+tile_size_34));
        }
    }
}
