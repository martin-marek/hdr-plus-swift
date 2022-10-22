#include <metal_stdlib>
using namespace metal;

constant uint UINT16_MAX_VAL = 65535;


kernel void fill_with_zeros(texture2d<float, access::write> texture [[texture(0)]],
                            uint2 gid [[thread_position_in_grid]]) {
    texture.write(0, gid);
}


kernel void texture_uint16_to_float(texture2d<uint, access::read> in_texture [[texture(0)]],
                                    texture2d<float, access::write> out_texture [[texture(1)]],
                                    uint2 gid [[thread_position_in_grid]]) {
    uint4 val_unint = in_texture.read(gid);
    float4 val_float = float4(val_unint) / UINT16_MAX_VAL;
    out_texture.write(val_float, gid);
}


kernel void add_texture(texture2d<float, access::read> texture1 [[texture(0)]],
                        texture2d<float, access::read_write> texture2 [[texture(1)]],
                        uint2 gid [[thread_position_in_grid]]) {
    float4 out_color = texture1.read(gid) + texture2.read(gid);
    texture2.write(out_color, gid);
}


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


kernel void average_texture_sums(texture2d<float, access::read> in_texture [[texture(0)]],
                                 texture2d<uint, access::write> out_texture [[texture(1)]],
                                 constant int& divisor [[buffer(0)]],
                                 uint2 gid [[thread_position_in_grid]]) {
    float in_color = in_texture.read(gid).r;
    uint out = int(round(in_color * UINT16_MAX_VAL / divisor));
    out_texture.write(out, gid);
}


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
            int comp_tile_x = x0 + dx0 + dx1;
            int comp_tile_y = y0 + dy0 + dy1;
            // if the comparison pixels are outside of the frame, attach a high loss to them
            if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
                diff += 2;
            } else {
                diff += abs(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            }
        }
    }
    
    // store tile difference
    tile_diff.write(diff, gid);
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


kernel void blur_bayer_x(texture2d<float, access::read> in_texture [[texture(0)]],
                         texture2d<float, access::write> out_texture [[texture(1)]],
                         constant int& kernel_size [[buffer(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_width = in_texture.get_width();
    
    // compute a sigle output pixel
    float total_intensity = 0;
    float total_weight = 0;
    int y = gid.y;
    for (int dx = -kernel_size; dx <= kernel_size; dx++) {
        int x = gid.x + 2*dx;
        if (0 <= x && x < texture_width) {
            float weight = kernel_size - dx + 1;
            total_intensity += weight * in_texture.read(uint2(x, y)).r;
            total_weight += weight;
        }
    }
    
    // write output pixel
    float out_intensity = total_intensity / total_weight;
    out_texture.write(out_intensity, gid);
}


kernel void blur_bayer_y(texture2d<float, access::read> in_texture [[texture(0)]],
                         texture2d<float, access::write> out_texture [[texture(1)]],
                         constant int& kernel_size [[buffer(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int texture_height = in_texture.get_height();
    
    // compute a sigle output pixel
    float total_intensity = 0;
    float total_weight = 0;
    int x = gid.x;
    for (int dy = -kernel_size; dy <= kernel_size; dy++) {
        int y = gid.y + 2*dy;
        if (0 <= y && y < texture_height) {
            float weight = kernel_size - dy + 1;
            total_intensity += weight * in_texture.read(uint2(x, y)).r;
            total_weight += weight;
        }
    }
    
    // write output pixel
    float out_intensity = total_intensity / total_weight;
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
                             uint2 gid [[thread_position_in_grid]]) {
    float total_diff = 0;
    int x0 = gid.x * 2;
    int y0 = gid.y * 2;
    for (int dx = 0; dx <= 1; dx++) {
        for (int dy = 0; dy <= 1; dy++) {
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
    float weight;
    if (robustness == 0) {
        weight = 1;
    } else {
        float max_diff = noise_sd / robustness;
        weight =  1 - float(diff) / float(max_diff);
        weight = clamp(weight, 0.0, 1.0);
    }
    
    // write weight
    weight_texture.write(weight, gid);
}
