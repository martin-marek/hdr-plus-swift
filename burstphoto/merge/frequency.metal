#include <metal_stdlib>
#include "../constants.h"

using namespace metal;

/**
 This is the most important function required for the frequency-based merging approach. It is based on ideas from several publications:
 - [Hasinoff 2016]: https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf
 - [Monod 2021]: https://www.ipol.im/pub/art/2021/336/
 - [Liba 2019]: https://graphics.stanford.edu/papers/night-sight-sigasia19/night-sight-sigasia19.pdf
 - [Delbracio 2015]: https://openaccess.thecvf.com/content_cvpr_2015/papers/Delbracio_Burst_Deblurring_Removing_2015_CVPR_paper.pdf
*/
kernel void merge_frequency_domain(texture2d<float, access::read> ref_texture_ft [[texture(0)]],
                                   texture2d<float, access::read> aligned_texture_ft [[texture(1)]],
                                   texture2d<float, access::read_write> out_texture_ft [[texture(2)]],
                                   texture2d<float, access::read> rms_texture [[texture(3)]],
                                   texture2d<float, access::read> mismatch_texture [[texture(4)]],
                                   constant float& robustness_norm [[buffer(0)]],
                                   constant float& read_noise [[buffer(1)]],
                                   constant float& max_motion_norm [[buffer(2)]],
                                   constant int& tile_size [[buffer(3)]],
                                   constant int& uniform_exposure [[buffer(4)]],
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
    
    // compute tile positions from gid
    int const m0 = gid.x*tile_size;
    int const n0 = gid.y*tile_size;
    
    // pre-calculate factors for sine and cosine calculation
    float const angle = -2*PI/float(tile_size);
    float const shift_step_size = 1.0f/6.0f;
    
    // pre-initalize some variables
    float weight, min_weight, max_weight, coefRe, coefIm, shift_x, shift_y, ratio_mag, magnitude_norm;
    float4 refRe, refIm, refMag, alignedRe, alignedIm, alignedRe2, alignedIm2, alignedMag2, mergedRe, mergedIm, weight4;
    float total_diff[49];
    
    // fill with zeros
    for(int i = 0; i < 49; i++) {
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
            
            // test 7x7 discrete steps
            for (int i = 0; i < 49; i++) {
                  
                // potential shift in pixels (specified on the pixel scale of each color channel)
                shift_x = -0.5f + int(i % 7) * shift_step_size;
                shift_y = -0.5f + int(i / 7) * shift_step_size;
                            
                // calculate coefficients for Fourier shift
                coefRe = cos(angle*(dm*shift_x+dn*shift_y));
                coefIm = sin(angle*(dm*shift_x+dn*shift_y));
                         
                // calculate complex frequency data of shifted tile
                alignedRe2 = refRe - (coefRe*alignedRe - coefIm*alignedIm);
                alignedIm2 = refIm - (coefIm*alignedRe + coefRe*alignedIm);
                
                weight4 = alignedRe2*alignedRe2 + alignedIm2*alignedIm2;
                
                // add magnitudes of differences
                total_diff[i] += (weight4[0]+weight4[1]+weight4[2]+weight4[3]);
            }
        }
    }
    
    // find best shift (which has the lowest total difference)
    float best_diff = 1e20f;
    int   best_i    = 0;
    
    for (int i = 0; i < 49; i++) {
        
        if(total_diff[i] < best_diff) {
            
            best_diff = total_diff[i];
            best_i    = i;
        }
    }
    
    // extract best shifts
    float const best_shift_x = -0.5f + int(best_i % 7) * shift_step_size;
    float const best_shift_y = -0.5f + int(best_i / 7) * shift_step_size;
    
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
            
            // if we are not at the central frequency bin (zero frequency), if the mismatch is low and if the burst has a uniform exposure
            if (dm+dn > 0 & mismatch < 0.3f & uniform_exposure == 1) {
                
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


/**
 See section "Noise model and tiled approximation" in https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf or section "Noise Level Estimation" in https://www.ipol.im/pub/art/2021/336/
 */
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

/**
 Simple and slow discrete Fourier transform applied to each color channel independently
 */
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


/**
 Highly-optimized fast Fourier transform applied to each color channel independently
 The aim of this function is to provide improved performance compared to the more simple function backward_dft() while providing equal results. It uses the following features for reduced calculation times:
 - the four color channels are stored as a float4 and all calculations employ SIMD instructions.
 - the one-dimensional transformation along y-direction employs the fast Fourier transform algorithm: At first, 4 small DFTs are calculated and then final results are obtained by two steps of cross-combination of values (based on a so-called butterfly diagram). This approach reduces the total number of memory reads and computational steps considerably.
 - the one-dimensional transformation along x-direction employs the fast Fourier transform algorithm: At first, 4 small DFTs are calculated and then final results are obtained by two steps of cross-combination of values (based on a so-called butterfly diagram). This approach reduces the total number of memory reads and computational steps considerably.
 */
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


/**
 Simple and slow discrete Fourier transform applied to each color channel independently
 */
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


/**
 Highly-optimized fast Fourier transform applied to each color channel independently
 The aim of this function is to provide improved performance compared to the more simple function forward_dft() while providing equal results. It uses the following features for reduced calculation times:
 - the four color channels are stored as a float4 and all calculations employ SIMD instructions.
 - the one-dimensional transformation along y-direction is a discrete Fourier transform. As the input image is real-valued, the frequency domain representation is symmetric and only values for N/2+1 rows have to be calculated.
 - the one-dimensional transformation along x-direction employs the fast Fourier transform algorithm: At first, 4 small DFTs are calculated and then final results are obtained by two steps of cross-combination of values (based on a so-called butterfly diagram). This approach reduces the total number of memory reads and computational steps considerably.
 - due to the symmetry mentioned earlier, only N/2+1 rows have to be transformed and the remaining N/2-1 rows can be directly inferred.
 */
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
