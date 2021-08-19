#include <metal_stdlib>
using namespace metal;

// constant uint SINT16_MAX_VAL = 32767.0;
// constant uint UINT32_MAX_VAL = 4294967295;


kernel void add_texture(texture2d<uint, access::read> new_texture [[texture(0)]],
                        texture2d<uint, access::read_write> out_texture [[texture(1)]],
                        uint2 gid [[thread_position_in_grid]]) {
    uint4 out_color = out_texture.read(gid) + new_texture.read(gid);
    out_texture.write(out_color, gid);
}


kernel void upsample_nearest(texture2d<int, access::read> in_texture [[texture(0)]],
                             texture2d<int, access::write> out_texture [[texture(1)]],
                             constant float* params [[buffer(0)]],
                             uint2 gid [[thread_position_in_grid]]) {
    float scale_x = params[0];
    float scale_y = params[1];
    int x = int(round(float(gid.x) / scale_x));
    int y = int(round(float(gid.y) / scale_y));
    int4 out_color = in_texture.read(uint2(x, y));
    out_texture.write(out_color, gid);
}


kernel void avg_pool(texture2d<uint, access::read> in_texture [[texture(0)]],
                     texture2d<uint, access::write> out_texture [[texture(1)]],
                     uint2 gid [[thread_position_in_grid]]) {
    uint out_pixel = 0;
    int x0 = gid.x * 2;
    int y0 = gid.y * 2;
    for (int dx = 0; dx <= 1; dx++) {
        for (int dy = 0; dy <= 1; dy++) {
            int x = x0 + dx;
            int y = y0 + dy;
            out_pixel += in_texture.read(uint2(x, y)).r;
        }
    }
    out_pixel /= 4;
    out_texture.write(out_pixel, gid);
}


kernel void average_texture_sums(texture2d<uint, access::read> in_texture [[texture(0)]],
                                 texture2d<uint, access::write> out_texture [[texture(1)]],
                                 constant int* params [[buffer(0)]],
                                 uint2 gid [[thread_position_in_grid]]) {
    int divisor = params[0];
    uint4 pixel_color = in_texture.read(gid);
    pixel_color.r = pixel_color.r / divisor;
    out_texture.write(pixel_color, gid);
}


kernel void compute_tile_differences(texture2d<uint, access::read> ref_texture [[texture(0)]],
                                     texture2d<uint, access::read> comp_texture [[texture(1)]],
                                     texture2d<int, access::read> prev_alignment [[texture(2)]],
                                     texture3d<uint, access::write> tile_diff [[texture(3)]],
                                     constant int* params [[buffer(0)]],
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
    int x0 = tile_half_size + int(floor((float(gid.x)/float(n_tiles_x)) * (texture_width - tile_half_size - 1)));
    int y0 = tile_half_size + int(floor((float(gid.y)/float(n_tiles_y)) * (texture_height - tile_half_size - 1)));
    
    // compute current tile displacement based on thread index
    int dy0 = gid.z / n_pos_1d - search_dist;
    int dx0 = gid.z % n_pos_1d - search_dist;
    
    // factor in previous alignmnet
    int4 prev_align = prev_alignment.read(uint2(gid.x, gid.y));
    dx0 += downscale_factor * prev_align.x;
    dy0 += downscale_factor * prev_align.y;
    
    // compute tile difference
    uint diff = 0;
    for (int dx1 = -tile_half_size; dx1 < tile_half_size; dx1++){
        for (int dy1 = -tile_half_size; dy1 < tile_half_size; dy1++){
            // compute the indices of the pixels to compare
            int ref_tile_x = x0 + dx1;
            int ref_tile_y = y0 + dy1;
            int comp_tile_x = x0 + dx0 + dx1;
            int comp_tile_y = y0 + dy0 + dy1;
            // if the comparison pixels are outside of the frame, attach a very high loss to them
            // if ((comp_tile_x < 0) || (comp_tile_y < 0) || (comp_tile_x >= texture_width) || (comp_tile_y >= texture_height)) {
            //     diff += 2;
            // } else {
            //     diff += abs(int(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r) - int(comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r));
            // }
            diff += abs(int(ref_texture.read(uint2(ref_tile_x, ref_tile_y)).r) - int(comp_texture.read(uint2(comp_tile_x, comp_tile_y)).r));
        }
    }
    
    // store tile difference
    uint4 out = uint4(diff, 0, 0, 0);
    tile_diff.write(out, gid);
}


kernel void compute_tile_alignments(texture3d<uint, access::read> tile_diff [[texture(0)]],
                                    texture2d<int, access::read> prev_alignment [[texture(1)]],
                                    texture2d<int, access::write> current_alignment [[texture(2)]],
                                    constant int* params [[buffer(0)]],
                                    uint2 gid [[thread_position_in_grid]]) {
    // load args
    int downscale_factor = params[0];
    int search_dist = params[1];
    int n_pos_1d = 2*search_dist + 1;
    int n_pos_2d = n_pos_1d * n_pos_1d;
    
    // find tile position with the lowest pixel difference
    uint current_diff;
    uint min_diff_val = tile_diff.read(uint3(gid.x, gid.y, 0)).r;
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


kernel void warp_texture(texture2d<uint, access::read> in_texture [[texture(0)]],
                         texture2d<int, access::read> alignment [[texture(1)]],
                         texture2d<uint, access::write> out_texture [[texture(2)]],
                         constant int* params [[buffer(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    // load args
    int downscale_factor = params[0];
    
    // compute sampling index if the alignment were 0
    int x = gid.x;
    int y = gid.y;
    
    // factor in alignment
    int4 prev_align = alignment.read(gid);
    x += downscale_factor * prev_align.x;
    y += downscale_factor * prev_align.y;
    
    // sample the aligned pixel
    uint4 pixel = in_texture.read(uint2(x, y));
    out_texture.write(pixel, gid);
}
