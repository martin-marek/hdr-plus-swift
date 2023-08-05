/**
 Functions related to spatial-domain merging of images.
 */
import Foundation
import MetalPerformanceShaders


let color_difference_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "color_difference")!)
let compute_merge_weight_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_merge_weight")!)


/// Convenience function for the spatial merging approach
func align_merge_spatial_domain(progress: ProcessingProgress, ref_idx: Int, mosaic_pattern_width: Int, search_distance: Int, tile_size: Int, kernel_size: Int, robustness: Double, uniform_exposure: Bool, black_level: [Int], color_factors: [Double], textures: [MTLTexture], final_texture: MTLTexture) throws {
    // set original texture size
    let texture_width_orig = textures[ref_idx].width
    let texture_height_orig = textures[ref_idx].height
              
    // set alignment params
    let min_image_dim = min(texture_width_orig, texture_height_orig)
    var downscale_factor_array = [mosaic_pattern_width]
    var search_dist_array = [2]
    var tile_size_array = [tile_size]
    var res = min_image_dim / downscale_factor_array[0]
    var div = mosaic_pattern_width
    
    // This loop generates lists, which include information on different parameters required for the alignment on different resolution levels. For each pyramid level, the downscale factor compared to the neighboring resolution, the search distance, tile size, resolution (only for lowest level) and total downscale factor compared to the original resolution (only for lowest level) are calculated.
    while (res > search_distance) {
        downscale_factor_array.append(2)
        search_dist_array.append(2)
        tile_size_array.append(max(tile_size_array.last!/2, 8))
        res /= 2
        div *= 2
    }
     
    // calculate padding for extension of the image frame with zeros
    // For the alignment, the frame may be extended further by pad_align due to the following reason: the alignment is performed on different resolution levels and alignment vectors are upscaled by a simple multiplication by 2. As a consequence, the frame at all resolution levels has to be a multiple of the tile sizes of these resolution levels.
    let tile_factor = div*Int(tile_size_array.last!)
    
    var pad_align_x = Int(ceil(Float(texture_width_orig)/Float(tile_factor)))
    pad_align_x = (pad_align_x*Int(tile_factor) - texture_width_orig)/2
    
    var pad_align_y = Int(ceil(Float(texture_height_orig)/Float(tile_factor)))
    pad_align_y = (pad_align_y*Int(tile_factor) - texture_height_orig)/2
      
    // set reference texture
    let ref_texture = extend_texture(textures[ref_idx], pad_align_x, pad_align_x, pad_align_y, pad_align_y)
            
    var color_factors3 = [color_factors[ref_idx*3+0], color_factors[ref_idx*3+1], color_factors[ref_idx*3+2]]
    
    // build reference pyramid
    let ref_pyramid = build_pyramid(ref_texture, downscale_factor_array, color_factors3)
    
    // blur reference texure and estimate noise standard deviation
    // -  the computation is done here to avoid repeating the same computation in 'robust_merge()'
    let ref_texture_blurred = blur_mosaic_texture(textures[ref_idx], kernel_size, mosaic_pattern_width)
    let noise_sd = estimate_color_noise(textures[ref_idx], ref_texture_blurred, mosaic_pattern_width)

    // iterate over comparison images
    for comp_idx in 0..<textures.count {
        
        // add the reference texture to the output
        if comp_idx == ref_idx {
            add_texture(textures[comp_idx], final_texture, textures.count)
            DispatchQueue.main.async { progress.int += Int(80000000/Double(textures.count)) }
            continue
        }
            
        // set comparison texture
        let comp_texture = extend_texture(textures[comp_idx], pad_align_x, pad_align_x, pad_align_y, pad_align_y)
     
        let black_level_mean = 0.25*Double(black_level[comp_idx*4+0] + black_level[comp_idx*4+1] + black_level[comp_idx*4+2] + black_level[comp_idx*4+3])
        color_factors3 = [color_factors[comp_idx*3+0], color_factors[comp_idx*3+1], color_factors[comp_idx*3+2]]
        
        // align comparison texture
        let aligned_texture = crop_texture(align_texture(ref_pyramid, comp_texture, downscale_factor_array, tile_size_array, search_dist_array, uniform_exposure, black_level_mean, color_factors3), pad_align_x, pad_align_x, pad_align_y, pad_align_y)
        
        // robust-merge the texture
        let merged_texture = robust_merge(textures[ref_idx], ref_texture_blurred, aligned_texture, kernel_size, robustness, noise_sd, mosaic_pattern_width)
        
        // add robust-merged texture to the output image
        add_texture(merged_texture, final_texture, textures.count)
        
        // sync GUI progress
        DispatchQueue.main.async { progress.int += Int(80000000/Double(textures.count)) }
    }
}


func color_difference(_ texture1: MTLTexture, _ texture2: MTLTexture, _ mosaic_pattern_width: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture1.pixelFormat, width: texture1.width/mosaic_pattern_width, height: texture1.height/mosaic_pattern_width, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // compute pixel pairwise differences
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = color_difference_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: texture1.width/2, height: texture1.height/2, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(texture1, index: 0)
    command_encoder.setTexture(texture2, index: 1)
    command_encoder.setTexture(output_texture, index: 2)
    command_encoder.setBytes([Int32(mosaic_pattern_width)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return output_texture
}


func estimate_color_noise(_ texture: MTLTexture, _ texture_blurred: MTLTexture, _ mosaic_pattern_width: Int) -> MTLBuffer {
    
    // compute the color difference of each mosaic superpixel between the original and the blurred texture
    let texture_diff = color_difference(texture, texture_blurred, mosaic_pattern_width)
    
    // compute the average of the difference between the original and the blurred texture
    let mean_diff = texture_mean(texture_diff, "r")
    
    return mean_diff
}


func robust_merge(_ ref_texture: MTLTexture, _ ref_texture_blurred: MTLTexture, _ comp_texture: MTLTexture, _ kernel_size: Int, _ robustness: Double, _ noise_sd: MTLBuffer, _ mosaic_pattern_width: Int) -> MTLTexture {
    
    // blur comparison texture
    let comp_texture_blurred = blur_mosaic_texture(comp_texture, kernel_size, mosaic_pattern_width)
    
    // compute the color difference of each superpixel between the blurred reference and the comparison textures
    let texture_diff = color_difference(ref_texture_blurred, comp_texture_blurred, mosaic_pattern_width)
    
    // create a weight texture
    let weight_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: texture_diff.width, height: texture_diff.height, mipmapped: false)
    weight_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let weight_texture = device.makeTexture(descriptor: weight_texture_descriptor)!
    
    // compute merge weight
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = compute_merge_weight_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: texture_diff.width, height: texture_diff.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(texture_diff, index: 0)
    command_encoder.setTexture(weight_texture, index: 1)
    command_encoder.setBuffer(noise_sd, offset: 0, index: 0)
    command_encoder.setBytes([Float32(robustness)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    // upsample merge weight to full image resolution
    let weight_texture_upsampled = upsample(weight_texture, width: ref_texture.width, height: ref_texture.height, mode: "bilinear")
    
    // average the input textures based on the weight
    let output_texture = add_texture_weighted(ref_texture, comp_texture, weight_texture_upsampled)
    
    return output_texture
}

