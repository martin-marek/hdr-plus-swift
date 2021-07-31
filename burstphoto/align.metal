#include <metal_stdlib>
using namespace metal;


constant float SINT16_MAX_VAL = 32767.0;


kernel void add_texture(texture2d<float, access::read> new_texture [[texture(0)]],
                        texture2d<float, access::read_write> out_texture [[texture(1)]],
                         constant float *params [[buffer(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
    float scale = params[0];
    float4 out_color = (out_texture.read(gid) + scale*new_texture.read(gid));
    out_texture.write(out_color, gid);
}


kernel void rgb_to_bw(texture2d<float, access::read> in_texture [[texture(0)]],
                      texture2d<float, access::write> out_texture [[texture(1)]],
                      uint2 gid [[thread_position_in_grid]]) {
    float4 rgb = in_texture.read(gid);
    float intensity = (rgb.r + rgb.g + rgb.b) / 3;
    float4 out = float4(intensity, 0, 0, 0);
    out_texture.write(out, gid);
}


kernel void compute_tile_differences(texture2d<float, access::read> ref_texture [[texture(0)]],
                                     texture2d<float, access::read> comp_texture [[texture(1)]],
                                     texture2d<float, access::read> prev_alignment [[texture(2)]],
                                     texture3d<float, access::write> tile_diff [[texture(3)]],
                                     constant int *params [[buffer(0)]],
                                     uint3 gid [[thread_position_in_grid]]) {
    
    // load args
    int downscale_factor = params[0];
    int tile_size = params[1];
    int search_dist = params[2];
    int n_tiles_x = params[3];
    int n_tiles_y = params[4];
    int texture_width = ref_texture.get_width();
    int texture_height = ref_texture.get_height();
    int tile_half_size = tile_size / 2;
    int n_pos_1d = 2*search_dist + 1;
    
    // compute tile position if previous alignment were 0
    int x0 = tile_half_size + int(floor((float(gid.x)/float(n_tiles_x)) * (texture_width - tile_half_size)));
    int y0 = tile_half_size + int(floor((float(gid.y)/float(n_tiles_y)) * (texture_height - tile_half_size)));
    
    // compute current tile displacement based on thread index
    int dy0 = gid.z / n_pos_1d - search_dist;
    int dx0 = gid.z % n_pos_1d - search_dist;
    
    // factor in previous alignmnet
    float4 prev_align = prev_alignment.read(uint2(gid.x, gid.y));
    int prev_align_x = int(prev_align.x * SINT16_MAX_VAL);
    int prev_align_y = int(prev_align.y * SINT16_MAX_VAL);
    dx0 += downscale_factor * prev_align_x;
    dy0 += downscale_factor * prev_align_y;
    
    // compute tile difference
    float diff = 0;
    for (int dx1 = -tile_half_size; dx1 < tile_half_size; dx1++){
        for (int dy1 = -tile_half_size; dy1 < tile_half_size; dy1++){
            // compute the indices of the pixels to compare
            int ref_tile_x = x0 + dx1;
            int ref_tile_y = y0 + dy1;
            int comp_tile_x = x0 + dx0 + dx1;
            int comp_tile_y = y0 + dy0 + dy1;
            // check if the comparison pixels are within frame
            // if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
            //     diff += 2;
            // } else {
            //     diff += abs(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
            // }
            diff += abs(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r - comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r);
        }
    }
    
    // store tile difference
    // - ints have to be converted to normalized floats
    float diff_nfloat = diff / float(tile_size * tile_size);
    float4 out = float4(diff_nfloat, 0, 0, 0);
    tile_diff.write(out, gid);
}


kernel void compute_tile_alignments(texture3d<float, access::read> tile_diff [[texture(0)]],
                                    texture2d<float, access::read> prev_alignment [[texture(1)]],
                                    texture2d<float, access::write> current_alignment [[texture(2)]],
                                    constant int *params [[buffer(0)]],
                                    uint2 gid [[thread_position_in_grid]]) {
    // load args
    int downscale_factor = params[0];
    int search_dist = params[1];
    int n_pos_1d = 2*search_dist + 1;
    int n_pos_2d = n_pos_1d * n_pos_1d;
    
    // find tile position with the lowest pixel difference
    float current_diff = 0;
    float min_diff_val = 1;
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
    float4 prev_align = prev_alignment.read(gid);
    int prev_align_x = int(prev_align.x * SINT16_MAX_VAL);
    int prev_align_y = int(prev_align.y * SINT16_MAX_VAL);
    dx += downscale_factor * prev_align_x;
    dy += downscale_factor * prev_align_y;
    
    // store alignment
    // - ints have to be converted to normalized floats
    float dx_nfloat = float(dx) / SINT16_MAX_VAL;
    float dy_nfloat = float(dy) / SINT16_MAX_VAL;
    float4 out = float4(dx_nfloat, dy_nfloat, 0, 0);
    current_alignment.write(out, gid);
}


kernel void warp_texture(texture2d<float, access::read> in_texture [[texture(0)]],
                         texture2d<float, access::read> alignment [[texture(1)]],
                         texture2d<float, access::write> out_texture [[texture(2)]],
                         constant int *params [[buffer(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int downscale_factor = params[0];
    
    // compute sampling index if the alignment were 0
    int x = gid.x;
    int y = gid.y;
    
    // factor in alignment
    // - normalized floats have to be converted to ints
    float4 prev_align = alignment.read(gid);
    int prev_align_x = int(prev_align.x * SINT16_MAX_VAL);
    int prev_align_y = int(prev_align.y * SINT16_MAX_VAL);
    x += downscale_factor * prev_align_x;
    y += downscale_factor * prev_align_y;
    
    // sample the aligned pixel
    float4 pixel = in_texture.read(uint2(x, y));
    out_texture.write(pixel, gid);
}
