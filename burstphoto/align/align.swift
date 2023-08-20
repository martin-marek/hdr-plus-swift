import Foundation
import MetalPerformanceShaders

let avg_pool_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "avg_pool")!)
let avg_pool_normalization_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "avg_pool_normalization")!)
let compute_tile_differences_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_tile_differences")!)
let compute_tile_differences25_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_tile_differences25")!)
let compute_tile_differences_exposure25_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_tile_differences_exposure25")!)
let correct_upsampling_error_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "correct_upsampling_error")!)
let find_best_tile_alignment_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "find_best_tile_alignment")!)
let warp_texture_bayer_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "warp_texture_bayer")!)
let warp_texture_xtrans_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "warp_texture_xtrans")!)

func align_texture(_ ref_pyramid: [MTLTexture], _ comp_texture: MTLTexture, _ downscale_factor_array: Array<Int>, _ tile_size_array: Array<Int>, _ search_dist_array: Array<Int>, _ uniform_exposure: Bool, _ black_level_mean: Double, _ color_factors3: Array<Double>) -> MTLTexture {
        
    // initialize tile alignments
    let alignment_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rg16Sint, width: 1, height: 1, mipmapped: false)
    alignment_descriptor.usage = [.shaderRead, .shaderWrite]
    var prev_alignment = device.makeTexture(descriptor: alignment_descriptor)!
    var current_alignment = device.makeTexture(descriptor: alignment_descriptor)!
    var tile_info = TileInfo(tile_size: 0, tile_size_merge: 0, search_dist: 0, n_tiles_x: 0, n_tiles_y: 0, n_pos_1d: 0, n_pos_2d: 0)
    
    // build comparison pyramid
    let comp_pyramid = build_pyramid(comp_texture, downscale_factor_array, color_factors3)
    
    // align tiles
    for i in (0 ... downscale_factor_array.count-1).reversed() {
        
        // load layer params
        let tile_size = tile_size_array[i]
        let search_dist = search_dist_array[i]
        let ref_layer = ref_pyramid[i]
        let comp_layer = comp_pyramid[i]
        
        // calculate the number of tiles
        let n_tiles_x = ref_layer.width / (tile_size / 2) - 1
        let n_tiles_y = ref_layer.height / (tile_size / 2) - 1
        let n_pos_1d = 2*search_dist + 1
        let n_pos_2d = n_pos_1d * n_pos_1d
        
        // store the values together in a struct to make it easier and more readable when passing between functions
        tile_info = TileInfo(tile_size: tile_size, tile_size_merge: 0, search_dist: search_dist, n_tiles_x: n_tiles_x, n_tiles_y: n_tiles_y, n_pos_1d: n_pos_1d, n_pos_2d: n_pos_2d)
        
        // resize previous alignment
        // - 'downscale_factor' has to be loaded from the *previous* layer since that is the layer that generated the current layer
        var downscale_factor: Int
        if (i < downscale_factor_array.count-1){
            downscale_factor = downscale_factor_array[i+1]
        } else {
            downscale_factor = 0
        }
        
        // upsample alignment vectors by a factor of 2
        prev_alignment = upsample(current_alignment, to_width: n_tiles_x, to_height: n_tiles_y, using: .NearestNeighbour)
        
        // compare three alignment vector candidates, which improves alignment at borders of moving object
        // see https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf for more details
        prev_alignment = correct_upsampling_error(ref_layer, comp_layer, prev_alignment, downscale_factor, uniform_exposure, black_level_mean, (i != 0), tile_info)
          
        // compute tile differences
        let tile_diff = compute_tile_diff(ref_layer, comp_layer, prev_alignment, downscale_factor, uniform_exposure, black_level_mean, (i != 0), tile_info)
      
        current_alignment = texture_like(prev_alignment)
        
        // find best tile alignment based on tile differences
        find_best_tile_alignment(tile_diff, prev_alignment, current_alignment, downscale_factor, tile_info)
    }

    // warp the aligned layer
    let aligned_texture = warp_texture(comp_texture, current_alignment, tile_info, downscale_factor_array[0])
    
    return aligned_texture
}


func avg_pool(_ input_texture: MTLTexture, _ scale: Int, _ normalization: Bool, _ color_factors3: Array<Double>) -> MTLTexture {

    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: input_texture.width/scale, height: input_texture.height/scale, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Avg Pool"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = (normalization ? avg_pool_normalization_state : avg_pool_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(input_texture, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    command_encoder.setBytes([Int32(scale)], length: MemoryLayout<Int32>.stride, index: 0)
    
    if normalization {
        command_encoder.setBytes([Float32(color_factors3[0])], length: MemoryLayout<Float32>.stride, index: 1)
        command_encoder.setBytes([Float32(color_factors3[1])], length: MemoryLayout<Float32>.stride, index: 2)
        command_encoder.setBytes([Float32(color_factors3[2])], length: MemoryLayout<Float32>.stride, index: 3)
    }
    
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()

    return output_texture
}


func build_pyramid(_ input_texture: MTLTexture, _ downscale_factor_list: Array<Int>, _ color_factors3: Array<Double>) -> Array<MTLTexture> {
    
    // iteratively resize the current layer in the pyramid
    var pyramid: Array<MTLTexture> = []
    for (i, downscale_factor) in downscale_factor_list.enumerated() {
        if i == 0 {
            // If color_factor is NOT available, a negative value will be set.
            pyramid.append(avg_pool(input_texture, downscale_factor, color_factors3[0] > 0, color_factors3))
        } else {
            pyramid.append(avg_pool(blur(pyramid.last!, with_pattern_width: 1, using_kernel_size: 2), downscale_factor, false, color_factors3))
        }
    }
    return pyramid
}


func compute_tile_diff(_ ref_layer: MTLTexture, _ comp_layer: MTLTexture, _ prev_alignment: MTLTexture, _ downscale_factor: Int, _ uniform_exposure: Bool, _ black_level_mean: Double, _ use_ssd: Bool, _ tile_info: TileInfo) -> MTLTexture {
    
    // create a 'tile difference' texture
    let texture_descriptor = MTLTextureDescriptor()
    texture_descriptor.textureType = .type3D
    texture_descriptor.pixelFormat = .r32Float
    texture_descriptor.width = tile_info.n_tiles_x
    texture_descriptor.height = tile_info.n_tiles_y
    texture_descriptor.depth = tile_info.n_pos_2d
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let tile_diff = device.makeTexture(descriptor: texture_descriptor)!
    
    // compute tile differences
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Compute Tile Diff"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    // either use generic function or highly-optimized function for testing a +/- 2 displacement in both image directions (in total 25 combinations)
    let state = (tile_info.n_pos_2d==25 ? (uniform_exposure ? compute_tile_differences25_state : compute_tile_differences_exposure25_state) : compute_tile_differences_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: (tile_info.n_pos_2d==25 ? 1 : tile_info.n_pos_2d))
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(ref_layer, index: 0)
    command_encoder.setTexture(comp_layer, index: 1)
    command_encoder.setTexture(prev_alignment, index: 2)
    command_encoder.setTexture(tile_diff, index: 3)
    command_encoder.setBytes([Int32(downscale_factor)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(tile_info.tile_size)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(tile_info.search_dist)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Float32(uniform_exposure ? 0.0 : black_level_mean)], length: MemoryLayout<Float32>.stride, index: 3)
    command_encoder.setBytes([Int32(use_ssd ? 1 : 0)], length: MemoryLayout<Int32>.stride, index: 4)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return tile_diff
}


func correct_upsampling_error(_ ref_layer: MTLTexture, _ comp_layer: MTLTexture, _ prev_alignment: MTLTexture, _ downscale_factor: Int, _ uniform_exposure: Bool, _ black_level_mean: Double, _ use_ssd: Bool, _ tile_info: TileInfo) -> MTLTexture {
    
    // create texture for corrected alignment
    let prev_alignment_corrected_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: prev_alignment.pixelFormat, width: prev_alignment.width, height: prev_alignment.height, mipmapped: false)
    prev_alignment_corrected_descriptor.usage = [.shaderRead, .shaderWrite]
    let prev_alignment_corrected = device.makeTexture(descriptor: prev_alignment_corrected_descriptor)!
        
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Correct Upsampling Error"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = correct_upsampling_error_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(ref_layer, index: 0)
    command_encoder.setTexture(comp_layer, index: 1)
    command_encoder.setTexture(prev_alignment, index: 2)
    command_encoder.setTexture(prev_alignment_corrected, index: 3)
    command_encoder.setBytes([Int32(downscale_factor)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(tile_info.tile_size)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(tile_info.n_tiles_x)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(tile_info.n_tiles_y)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.setBytes([Float32(uniform_exposure ? 0.0 : black_level_mean)], length: MemoryLayout<Float32>.stride, index: 4)
    command_encoder.setBytes([Int32(uniform_exposure ? 1 : 0)], length: MemoryLayout<Float32>.stride, index: 5)
    command_encoder.setBytes([Int32(use_ssd ? 1 : 0)], length: MemoryLayout<Int32>.stride, index: 6)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return prev_alignment_corrected
}


func find_best_tile_alignment(_ tile_diff: MTLTexture, _ prev_alignment: MTLTexture, _ current_alignment: MTLTexture, _ downscale_factor: Int, _ tile_info: TileInfo) {
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Find Best Tile Alignment"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = find_best_tile_alignment_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(tile_diff, index: 0)
    command_encoder.setTexture(prev_alignment, index: 1)
    command_encoder.setTexture(current_alignment, index: 2)
    command_encoder.setBytes([Int32(downscale_factor)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(tile_info.search_dist)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func warp_texture(_ texture_to_warp: MTLTexture, _ alignment: MTLTexture, _ tile_info: TileInfo, _ downscale_factor: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture_to_warp.pixelFormat,
                                                                          width: texture_to_warp.width,
                                                                          height: texture_to_warp.height,
                                                                          mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let warped_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Warp Texture"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    // The function warp_texture_xtrans corresponds to an old version of the warp function and would also work with images with Bayer pattern
    let state = (downscale_factor==2 ? warp_texture_bayer_state : warp_texture_xtrans_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: texture_to_warp.width, height: texture_to_warp.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(texture_to_warp, index: 0)
    command_encoder.setTexture(warped_texture, index: 1)
    command_encoder.setTexture(alignment, index: 2)
    command_encoder.setBytes([Int32(downscale_factor)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32((downscale_factor==2 ? 1 : downscale_factor)*tile_info.tile_size)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(tile_info.n_tiles_x)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(tile_info.n_tiles_y)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return warped_texture
}
