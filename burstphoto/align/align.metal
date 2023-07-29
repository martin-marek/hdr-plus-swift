#include <metal_stdlib>
#include "../constants.h"
using namespace metal;


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


kernel void avg_pool_normalization(texture2d<float, access::read> in_texture [[texture(0)]],
                                   texture2d<float, access::write> out_texture [[texture(1)]],
                                   constant int& scale [[buffer(0)]],
                                   constant float& factor_red [[buffer(1)]],
                                   constant float& factor_green [[buffer(2)]],
                                   constant float& factor_blue [[buffer(3)]],
                                   uint2 gid [[thread_position_in_grid]]) {
    
    float out_pixel = 0;
    int x0 = gid.x * scale;
    int y0 = gid.y * scale;
    
    float const norm_factors[4] = {factor_red, factor_green, factor_green, factor_blue};
    float const mean_factor = 0.25f*(norm_factors[0]+norm_factors[1]+norm_factors[2]+norm_factors[3]);
     
    for (int dx = 0; dx < scale; dx++) {
        for (int dy = 0; dy < scale; dy++) {
            int x = x0 + dx;
            int y = y0 + dy;
            out_pixel += (mean_factor/norm_factors[dy*scale+dx]*in_texture.read(uint2(x, y)).r);
        }
    }

    out_pixel /= (scale*scale);
    out_texture.write(out_pixel, gid);
}


/**
 Generic function for computation of tile differences that works for any search distance
 */
kernel void compute_tile_differences(texture2d<float, access::read> ref_texture [[texture(0)]],
                                     texture2d<float, access::read> comp_texture [[texture(1)]],
                                     texture2d<int, access::read> prev_alignment [[texture(2)]],
                                     texture3d<float, access::write> tile_diff [[texture(3)]],
                                     constant int& downscale_factor [[buffer(0)]],
                                     constant int& tile_size [[buffer(1)]],
                                     constant int& search_dist [[buffer(2)]],
                                     constant int& n_tiles_x [[buffer(3)]],
                                     constant int& n_tiles_y [[buffer(4)]],
                                     constant float& black_level [[buffer(5)]],
                                     constant int& weight_ssd [[buffer(6)]],
                                     uint3 gid [[thread_position_in_grid]]) {
    
    // load args
    int const texture_width = ref_texture.get_width();
    int const texture_height = ref_texture.get_height();
    int n_pos_1d = 2*search_dist + 1;
    
    float diff_abs;
    
    // compute tile position if previous alignment were 0
    int const x0 = gid.x*tile_size/2;
    int const y0 = gid.y*tile_size/2;
    
    // compute current tile displacement based on thread index
    int dy0 = gid.z / n_pos_1d - search_dist;
    int dx0 = gid.z % n_pos_1d - search_dist;
    
    // factor in previous alignment
    int4 prev_align = prev_alignment.read(uint2(gid.x, gid.y));
    dx0 += downscale_factor * prev_align.x;
    dy0 += downscale_factor * prev_align.y;
    
    // compute tile difference
    float diff = 0;
    for (int dx1 = 0; dx1 < tile_size; dx1++){
        for (int dy1 = 0; dy1 < tile_size; dy1++){
            // compute the indices of the pixels to compare
            int ref_tile_x = x0 + dx1;
            int ref_tile_y = y0 + dy1;
            int comp_tile_x = ref_tile_x + dx0;
            int comp_tile_y = ref_tile_y + dy0;
            
            // if the comparison pixels are outside of the frame, attach a high loss to them
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                diff_abs = abs(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r - 2*FLOAT16_MIN_VAL);
            } else {
                diff_abs = abs(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
            diff += (1-weight_ssd)*diff_abs + weight_ssd*diff_abs*diff_abs;
        }
    }
    
    // store tile difference
    tile_diff.write(diff, gid);
}


/**
 Highly-optimized function for computation of tile differences that works only for search_distance == 2 (25 total combinations).
 The aim of this function is to reduce the number of memory accesses required compared to the more simple function compute_tile_differences() while providing equal results. As the alignment always checks shifts on a 5x5 pixel grid, a simple implementation would read 25 pixels in the comparison texture for each pixel in the reference texture. This optimized function however uses a buffer vector covering 5 complete rows of the texture that slides line by line through the comparison texture and reduces the number of memory reads considerably.
 */
kernel void compute_tile_differences25(texture2d<half, access::read> ref_texture [[texture(0)]],
                                       texture2d<half, access::read> comp_texture [[texture(1)]],
                                       texture2d<int, access::read> prev_alignment [[texture(2)]],
                                       texture3d<float, access::write> tile_diff [[texture(3)]],
                                       constant int& downscale_factor [[buffer(0)]],
                                       constant int& tile_size [[buffer(1)]],
                                       constant int& search_dist [[buffer(2)]],
                                       constant int& n_tiles_x [[buffer(3)]],
                                       constant int& n_tiles_y [[buffer(4)]],
                                       constant float& black_level [[buffer(5)]],
                                       constant int& weight_ssd [[buffer(6)]],
                                       uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int const texture_width = ref_texture.get_width();
    int const texture_height = ref_texture.get_height();
     
    int ref_tile_x, ref_tile_y, comp_tile_x, comp_tile_y, tmp_index, dx_i, dy_i;
    
    // compute tile position if previous alignment were 0
    int const x0 = gid.x*tile_size/2;
    int const y0 = gid.y*tile_size/2;
    
    // factor in previous alignment
    int4 const prev_align = prev_alignment.read(uint2(gid.x, gid.y));
    int const dx0 = downscale_factor * prev_align.x;
    int const dy0 = downscale_factor * prev_align.y;
    
    float diff[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float diff_abs;
    half tmp_ref;
    half tmp_comp[5*68];
    
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
                tmp_comp[tmp_index] = FLOAT16_MIN_VAL;
            } else {
                tmp_comp[tmp_index] = FLOAT16_05_VAL*comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r;
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
                tmp_comp[tmp_index] = FLOAT16_MIN_VAL;
            } else {
                tmp_comp[tmp_index] = FLOAT16_05_VAL*comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r;
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
                
                diff_abs = abs(tmp_ref - 2.0f*tmp_comp[tmp_index]);
                
                // add difference to corresponding combination
                diff[i] += (1-weight_ssd)*diff_abs + weight_ssd*diff_abs*diff_abs;
            }
        }
    }
    
    // store tile differences in texture
    for (int i = 0; i < 25; i++) {
        tile_diff.write(diff[i], uint3(gid.x, gid.y, i));
    }
}

/**
 Highly-optimized function for computation of tile differences that works only for search_distance == 2 (25 total combinations).
 The aim of this function is to reduce the number of memory accesses required compared to the more simple function compute_tile_differences() while extending it with a scaling of pixel intensities by the ratio of mean values of both tiles.
 */
kernel void compute_tile_differences_exposure25(texture2d<half, access::read> ref_texture [[texture(0)]],
                                                texture2d<half, access::read> comp_texture [[texture(1)]],
                                                texture2d<int, access::read> prev_alignment [[texture(2)]],
                                                texture3d<float, access::write> tile_diff [[texture(3)]],
                                                constant int& downscale_factor [[buffer(0)]],
                                                constant int& tile_size [[buffer(1)]],
                                                constant int& search_dist [[buffer(2)]],
                                                constant int& n_tiles_x [[buffer(3)]],
                                                constant int& n_tiles_y [[buffer(4)]],
                                                constant float& black_level [[buffer(5)]],
                                                constant int& weight_ssd [[buffer(6)]],
                                                uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int const texture_width = ref_texture.get_width();
    int const texture_height = ref_texture.get_height();
    
    half const black_level_float16 = half(black_level);
     
    int ref_tile_x, ref_tile_y, comp_tile_x, comp_tile_y, tmp_index, dx_i, dy_i;
    
    // compute tile position if previous alignment were 0
    int const x0 = gid.x*tile_size/2;
    int const y0 = gid.y*tile_size/2;
    
    // factor in previous alignment
    int4 const prev_align = prev_alignment.read(uint2(gid.x, gid.y));
    int const dx0 = downscale_factor * prev_align.x;
    int const dy0 = downscale_factor * prev_align.y;
    
    float sum_u[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float sum_v[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float diff[25]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float ratio[25];
    float diff_abs;
    half tmp_ref, tmp_comp1;
    half tmp_comp[5*68];
    
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
                tmp_comp[tmp_index] = FLOAT16_MAX_VAL;
            } else {
                tmp_comp[tmp_index] = max(FLOAT16_ZERO_VAL, FLOAT16_05_VAL * (comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r - black_level_float16));
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
                tmp_comp[tmp_index] = FLOAT16_MAX_VAL;
            } else {
                tmp_comp[tmp_index] = max(FLOAT16_ZERO_VAL, FLOAT16_05_VAL * (comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r - black_level_float16));
            }
        }
        
        // loop over columns of ref_texture
        for (int dx = 0; dx < tile_size; dx++) {
            
            ref_tile_x = x0 + dx;
            ref_tile_y = y0 + dy;
            
            tmp_ref = max(FLOAT16_ZERO_VAL, ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r - black_level_float16);
              
            // loop over 25 test displacements
            for (int i = 0; i < 25; i++) {
                
                dx_i = i % 5;
                dy_i = i / 5;
                
                // index of corresponding pixel value in tmp_comp
                tmp_index = ((dy+dy_i)%5)*(tile_size+4) + dx + dx_i;
                tmp_comp1 = tmp_comp[tmp_index];
         
                if (tmp_comp1 > -1)
                {
                    sum_u[i] += tmp_ref;
                    sum_v[i] += 2.0f*tmp_comp1;
                }
            }
        }
    }
       
    for (int i = 0; i < 25; i++) {
        // calculate ratio of mean values of the tiles, which is used for correction of slight differences in exposure
        ratio[i] = clamp(sum_u[i]/(sum_v[i]+1e-9), 0.9f, 1.1f);
    }
        
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
                tmp_comp[tmp_index] = FLOAT16_MIN_VAL;
            } else {
                tmp_comp[tmp_index] = max(FLOAT16_ZERO_VAL, FLOAT16_05_VAL * (comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r - black_level_float16));
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
                tmp_comp[tmp_index] = FLOAT16_MIN_VAL;
            } else {
                tmp_comp[tmp_index] = max(FLOAT16_ZERO_VAL, FLOAT16_05_VAL * (comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r - black_level_float16));
            }
        }
        
        // loop over columns of ref_texture
        for (int dx = 0; dx < tile_size; dx++) {
            
            ref_tile_x = x0 + dx;
            ref_tile_y = y0 + dy;
            
            tmp_ref = max(FLOAT16_ZERO_VAL, ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r - black_level_float16);
              
            // loop over 25 test displacements
            for (int i = 0; i < 25; i++) {
                
                dx_i = i % 5;
                dy_i = i / 5;
                
                // index of corresponding pixel value in tmp_comp
                tmp_index = ((dy+dy_i)%5)*(tile_size+4) + dx + dx_i;
                
                diff_abs = abs(tmp_ref - 2.0f*ratio[i]*tmp_comp[tmp_index]);
                
                // add difference to corresponding combination
                diff[i] += (1-weight_ssd)*diff_abs + weight_ssd*diff_abs*diff_abs;
            }
        }
    }
    
    // store tile differences in texture
    for (int i = 0; i < 25; i++) {
        tile_diff.write(diff[i], uint3(gid.x, gid.y, i));
    }
}


/**
 At transitions between moving objects and non-moving background, the alignment vectors from downsampled images may be inaccurate. Therefore, after upsampling to the next resolution level, three candidate alignment vectors are evaluated for each tile. In addition to the vector obtained from upsampling, two vectors from neighboring tiles are checked. As a consequence, alignment at the transition regions described above is more accurate.
 See section on "Hierarchical alignment" in https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf and section "Multi-scale Pyramid Alignment" in https://www.ipol.im/pub/art/2021/336/
 */
kernel void correct_upsampling_error(texture2d<half, access::read> ref_texture [[texture(0)]],
                                     texture2d<half, access::read> comp_texture [[texture(1)]],
                                     texture2d<int, access::read> prev_alignment [[texture(2)]],
                                     texture2d<int, access::write> prev_alignment_corrected [[texture(3)]],
                                     constant int& downscale_factor [[buffer(0)]],
                                     constant int& tile_size [[buffer(1)]],
                                     constant int& n_tiles_x [[buffer(2)]],
                                     constant int& n_tiles_y [[buffer(3)]],
                                     constant float& black_level [[buffer(4)]],
                                     constant int& uniform_exposure [[buffer(5)]],
                                     constant int& weight_ssd [[buffer(6)]],
                                     uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int const texture_width = ref_texture.get_width();
    int const texture_height = ref_texture.get_height();
    
    half const black_level_float16 = half(black_level);
    
    // initialize some variables
    int comp_tile_x, comp_tile_y, tmp_tile_x, tmp_tile_y, weight_outside;
    float diff_abs;
    half tmp_ref[64];
    
    // compute tile position if previous alignment were 0
    int const x0 = gid.x*tile_size/2;
    int const y0 = gid.y*tile_size/2;
    
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
    float diff[3]  = {0.0f, 0.0f, 0.0f};
    float ratio[3] = {1.0f, 1.0f, 1.0f};
    
    // calculate exposure correction factors for slight scaling of pixel intensities
    if (uniform_exposure != 1) {
        
        float sum_u[3] = {0.0f, 0.0f, 0.0f};
        float sum_v[3] = {0.0f, 0.0f, 0.0f};
        
        // loop over all rows
        for (int dy = 0; dy < tile_size; dy += 64/tile_size) {
            
            // copy 64/tile_size rows into temp vector
            for (int i = 0; i < 64; i++) {
                tmp_ref[i] = max(FLOAT16_ZERO_VAL, ref_texture.read(uint2(x0+(i % tile_size), y0+dy+int(i / tile_size))).r - black_level_float16);
            }
            
            // loop over three candidates
            for (int c = 0; c < 3; c++) {
                
                // loop over tmp vector: candidate c of alignment vector
                tmp_tile_x = x0 + dx0[c];
                tmp_tile_y = y0 + dy0[c] + dy;
                for (int i = 0; i < 64; i++) {
                    
                    // compute the indices of the pixels to compare
                    comp_tile_x = tmp_tile_x + (i % tile_size);
                    comp_tile_y = tmp_tile_y + int(i / tile_size);
                    
                    if ((comp_tile_x >= 0) && (comp_tile_y >= 0) && (comp_tile_x < texture_width) && (comp_tile_y < texture_height)) {
                        
                        sum_u[c] += tmp_ref[i];
                        sum_v[c] += max(FLOAT16_ZERO_VAL, comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r - black_level_float16);
                    }
                }
            }
        }
        
        for (int c = 0; c < 3; c++) {
            // calculate ratio of mean values of the tiles, which is used for correction of slight differences in exposure
            ratio[c] = clamp(sum_u[c]/(sum_v[c]+1e-9), 0.9f, 1.1f);
        }
    }
    
    // loop over all rows
    for (int dy = 0; dy < tile_size; dy += 64/tile_size) {
        
        // copy 64/tile_size rows into temp vector
        for (int i = 0; i < 64; i++) {
            tmp_ref[i] = ref_texture.read(uint2(x0+(i % tile_size), y0+dy+int(i / tile_size))).r - black_level_float16;
        }
        
        // loop over three candidates
        for (int c = 0; c < 3; c++) {
            
            // loop over tmp vector: candidate c of alignment vector
            tmp_tile_x = x0 + dx0[c];
            tmp_tile_y = y0 + dy0[c] + dy;
            for (int i = 0; i < 64; i++) {
                
                // compute the indices of the pixels to compare
                comp_tile_x = tmp_tile_x + (i % tile_size);
                comp_tile_y = tmp_tile_y + int(i / tile_size);
                
                // if (comp_tile_x < 0 || comp_tile_y < 0 || comp_tile_x >= texture_width || comp_tile_y >= texture_height) => set weight_outside = 1, else set weight_outside = 0
                weight_outside = clamp(texture_width-comp_tile_x-1, -1, 0) + clamp(texture_height-comp_tile_y-1, -1, 0) + clamp(comp_tile_x, -1, 0) + clamp(comp_tile_y, -1, 0);
                weight_outside = -max(-1, weight_outside);
                
                diff_abs = abs(tmp_ref[i] - (1-weight_outside)*ratio[c]*(comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r - black_level_float16) - weight_outside*2*FLOAT16_MIN_VAL);
                
                // add difference to corresponding combination
                diff[c] += (1-weight_ssd)*diff_abs + weight_ssd*diff_abs*diff_abs;
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

kernel void find_best_tile_alignment(texture3d<float, access::read> tile_diff [[texture(0)]],
                                     texture2d<int, access::read> prev_alignment [[texture(1)]],
                                     texture2d<int, access::write> current_alignment [[texture(2)]],
                                     constant int& downscale_factor [[buffer(0)]],
                                     constant int& search_dist [[buffer(1)]],
                                     uint2 gid [[thread_position_in_grid]]) {
    // load args
    int const n_pos_1d = 2*search_dist + 1;
    int const n_pos_2d = n_pos_1d * n_pos_1d;
    
    // find tile displacement with the lowest pixel difference
    float current_diff;
    float min_diff_val = 1e20f;
    int min_diff_idx = 0;
    
    for (int i = 0; i < n_pos_2d; i++) {
        current_diff = tile_diff.read(uint3(gid.x, gid.y, i)).r;
        if (current_diff < min_diff_val) {
            min_diff_val = current_diff;
            min_diff_idx = i;
        }
    }
    
    // compute tile displacement if previous alignment were 0
    int const dx = min_diff_idx % n_pos_1d - search_dist;
    int const dy = min_diff_idx / n_pos_1d - search_dist;
    
    // factor in previous alignment
    int4 const prev_align = downscale_factor * prev_alignment.read(gid);
    
    // store alignment
    int4 const out = int4(prev_align.x+dx, prev_align.y+dy, 0, 0);
    current_alignment.write(out, gid);
}


kernel void warp_texture(texture2d<half, access::read> in_texture [[texture(0)]],
                         texture2d<half, access::write> out_texture [[texture(1)]],
                         texture2d<int, access::read> prev_alignment [[texture(2)]],
                         constant int& downscale_factor [[buffer(0)]],
                         constant int& half_tile_size [[buffer(1)]],
                         constant int& n_tiles_x [[buffer(2)]],
                         constant int& n_tiles_y [[buffer(3)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int const x = gid.x;
    int const y = gid.y;
    float const half_tile_size_float = float(half_tile_size);
    
    // compute the coordinates of output pixel in tile-grid units
    float const x_grid = (x+0.5f)/half_tile_size_float - 1.0f;
    float const y_grid = (y+0.5f)/half_tile_size_float - 1.0f;
    
    int const x_grid_floor = int(max(0.0f, floor(x_grid)) + 0.1f);
    int const y_grid_floor = int(max(0.0f, floor(y_grid)) + 0.1f);
    int const x_grid_ceil  = int(min(ceil(x_grid), n_tiles_x-1.0f) + 0.1f);
    int const y_grid_ceil  = int(min(ceil(y_grid), n_tiles_y-1.0f) + 0.1f);
    
    // weights calculated for the bilinear interpolation
    float const weight_x = ((x % half_tile_size) + 0.5f)/(2.0f*half_tile_size_float);
    float const weight_y = ((y % half_tile_size) + 0.5f)/(2.0f*half_tile_size_float);
    
    // factor in alignment
    int4 const prev_align0 = downscale_factor * prev_alignment.read(uint2(x_grid_floor, y_grid_floor));
    int4 const prev_align1 = downscale_factor * prev_alignment.read(uint2(x_grid_ceil,  y_grid_floor));
    int4 const prev_align2 = downscale_factor * prev_alignment.read(uint2(x_grid_floor, y_grid_ceil));
    int4 const prev_align3 = downscale_factor * prev_alignment.read(uint2(x_grid_ceil,  y_grid_ceil));
    
    // alignment vector from tile 0
    float pixel_value  = (1.0f-weight_x)*(1.0f-weight_y) * in_texture.read(uint2(x+prev_align0.x, y+prev_align0.y)).r;
    float total_weight = (1.0f-weight_x)*(1.0f-weight_y);
    
    // alignment vector from tile 1
    pixel_value  += weight_x*(1.0f-weight_y) * in_texture.read(uint2(x+prev_align1.x, y+prev_align1.y)).r;
    total_weight += weight_x*(1.0f-weight_y);
    
    // alignment vector from tile 2
    pixel_value  += (1.0f-weight_x)*weight_y * in_texture.read(uint2(x+prev_align2.x, y+prev_align2.y)).r;
    total_weight += (1.0f-weight_x)*weight_y;
    
    // alignment vector from tile 3
    pixel_value  += weight_x*weight_y * in_texture.read(uint2(x+prev_align3.x, y+prev_align3.y)).r;
    total_weight += weight_x*weight_y;
    
    // write output pixel
    out_texture.write(pixel_value/total_weight, gid);
}
