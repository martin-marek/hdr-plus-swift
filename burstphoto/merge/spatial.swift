/**
 Functions related to spatial-domain merging of images.
 */
import Foundation
import MetalPerformanceShaders


let color_difference_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "color_difference")!)
let compute_merge_weight_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_merge_weight")!)


/// Convenience function for the spatial merging approach
///
/// Supports non-Bayer raw files
func align_merge_spatial_domain(progress: ProcessingProgress, ref_idx: Int, mosaic_pattern_width: Int, search_distance: Int, tile_size: Int, noise_reduction: Double, uniform_exposure: Bool, exposure_bias: [Int], black_level: [[Int]], color_factors: [[Double]], textures: [MTLTexture], hotpixel_weight_texture: MTLTexture, final_texture: MTLTexture) throws {
    print("Merging in the spatial domain...")
    
    let kernel_size = mosaic_pattern_width == 6 ? 8 : 16 // kernel size of binomial filtering used for blurring the image
    
    // derive normalized robustness value: four steps in noise_reduction (-4.0 in this case) yield an increase by a factor of two in the robustness norm with the idea that the sd of shot noise increases by a factor of sqrt(2) per iso level
    let robustness_rev = 0.5*(36.0-Double(Int(noise_reduction+0.5)))
    let robustness = 0.12*pow(1.3, robustness_rev) - 0.4529822
    
    // set original texture size
    let texture_width_orig = textures[ref_idx].width
    let texture_height_orig = textures[ref_idx].height
              
    // set alignment params
    let min_image_dim = min(texture_width_orig, texture_height_orig)
    var downscale_factor_array = [mosaic_pattern_width == 6 ? 3 : mosaic_pattern_width]
    var search_dist_array = [2]
    var tile_size_array = [tile_size]
    var res = min_image_dim / downscale_factor_array[0]
    
    // This loop generates lists, which include information on different parameters required for the alignment on different resolution levels. For each pyramid level, the downscale factor compared to the neighboring resolution, the search distance, tile size, resolution (only for lowest level) and total downscale factor compared to the original resolution (only for lowest level) are calculated.
    while (res > search_distance) {
        downscale_factor_array.append(2)
        search_dist_array.append(2)
        tile_size_array.append(max(tile_size_array.last!/2, 8))
        res /= 2
    }
     
    // calculate padding for extension of the image frame with zeros
    // For the alignment, the frame may be extended further by pad_align due to the following reason: the alignment is performed on different resolution levels and alignment vectors are upscaled by a simple multiplication by 2. As a consequence, the frame at all resolution levels has to be a multiple of the tile sizes of these resolution levels.
    let tile_factor = Int(tile_size_array.last!) * downscale_factor_array.reduce(1, *)
    
    var pad_align_x = Int(ceil(Float(texture_width_orig)/Float(tile_factor)))
    pad_align_x = (pad_align_x*Int(tile_factor) - texture_width_orig)/2
    
    var pad_align_y = Int(ceil(Float(texture_height_orig)/Float(tile_factor)))
    pad_align_y = (pad_align_y*Int(tile_factor) - texture_height_orig)/2
    
    // prepare reference texture by correcting hot pixels, equalizing exposure and extending the texture
    let ref_texture = prepare_texture(textures[ref_idx], hotpixel_weight_texture, pad_align_x, pad_align_x, pad_align_y, pad_align_y, 0, black_level, ref_idx)
    let ref_texture_cropped = crop_texture(ref_texture, pad_align_x, pad_align_x, pad_align_y, pad_align_y)
    
    var black_level_mean = Double(black_level[ref_idx].reduce(0, +)) / Double(black_level[ref_idx].count)
    
    // build reference pyramid
    let ref_pyramid = build_pyramid(ref_texture, downscale_factor_array, black_level_mean, color_factors[ref_idx])
    
    // blur reference texure and estimate noise standard deviation
    // -  the computation is done here to avoid repeating the same computation in 'robust_merge()'
    let ref_texture_blurred = blur(ref_texture_cropped, with_pattern_width: mosaic_pattern_width, using_kernel_size: kernel_size)
    let noise_sd = estimate_color_noise(ref_texture_cropped, ref_texture_blurred, mosaic_pattern_width)

    // iterate over comparison images
    for comp_idx in 0..<textures.count {
        
        // add the reference texture to the output
        if comp_idx == ref_idx {
            add_texture(ref_texture_cropped, final_texture, textures.count)
            DispatchQueue.main.async { progress.int += Int(80000000/Double(textures.count)) }
            continue
        }
            
        // prepare comparison texture by correcting hot pixels, equalizing exposure and extending the texture
        let comp_texture = prepare_texture(textures[comp_idx], hotpixel_weight_texture, pad_align_x, pad_align_x, pad_align_y, pad_align_y, (exposure_bias[ref_idx]-exposure_bias[comp_idx]), black_level, comp_idx)
         
        black_level_mean = Double(black_level[comp_idx].reduce(0, +)) / Double(black_level[comp_idx].count)
        
        // align comparison texture
        let aligned_texture = crop_texture(
            align_texture(ref_pyramid, comp_texture, downscale_factor_array, tile_size_array, search_dist_array, (exposure_bias[comp_idx]==exposure_bias[ref_idx]), black_level_mean, color_factors[comp_idx]),
            pad_align_x, pad_align_x,
            pad_align_y, pad_align_y
        )
        
        // robust-merge the texture
        let merged_texture = robust_merge(ref_texture_cropped, ref_texture_blurred, aligned_texture, kernel_size, robustness, noise_sd, mosaic_pattern_width)
        
        // add robust-merged texture to the output image
        add_texture(merged_texture, final_texture, textures.count)
        
        // sync GUI progress
        DispatchQueue.main.async { progress.int += Int(80000000/Double(textures.count)) }
    }
}


/// For each super-pixel, calculate the sum of absolute differences between each color channel.
/// E.g. for a Bayer RGG'B super pixel this calculates, for each superpixel, `abs(R1 - R2) + abs(G1 - G2) + abs(G'1 - G'2) + abs(B1 - B2)`.
func color_difference(between texture1: MTLTexture, and texture2: MTLTexture, mosaic_pattern_width: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture1.pixelFormat, width: texture1.width/mosaic_pattern_width, height: texture1.height/mosaic_pattern_width, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_descriptor.storageMode = .private
    let output_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    output_texture.label = "\(texture1.label!.components(separatedBy: ":")[0]): Color Difference"
    
    // compute pixel pairwise differences
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Color Difference"
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
    let texture_diff = color_difference(between: texture, and: texture_blurred, mosaic_pattern_width: mosaic_pattern_width)
    
    // compute the average of the difference between the original and the blurred texture
    let mean_diff = texture_mean(texture_diff, per_sub_pixel: false, mosaic_pattern_width: mosaic_pattern_width)
    
    return mean_diff
}


func robust_merge(_ ref_texture: MTLTexture, _ ref_texture_blurred: MTLTexture, _ comp_texture: MTLTexture, _ kernel_size: Int, _ robustness: Double, _ noise_sd: MTLBuffer, _ mosaic_pattern_width: Int) -> MTLTexture {
    
    // blur comparison texture
    let comp_texture_blurred = blur(comp_texture, with_pattern_width: mosaic_pattern_width, using_kernel_size: kernel_size)
    
    // compute the color difference of each superpixel between the blurred reference and the comparison textures
    let texture_diff = color_difference(between: ref_texture_blurred, and: comp_texture_blurred, mosaic_pattern_width: mosaic_pattern_width)
    
    // create a weight texture
    let weight_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: texture_diff.width, height: texture_diff.height, mipmapped: false)
    weight_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    weight_texture_descriptor.storageMode = .private
    let weight_texture = device.makeTexture(descriptor: weight_texture_descriptor)!
    weight_texture.label = "\(comp_texture.label!.components(separatedBy: ":")[0]): Weight Texture"
    
    // compute merge weight
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Spatial Merge"
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
    let weight_texture_upsampled = upsample(weight_texture, to_width: ref_texture.width, to_height: ref_texture.height, using: .Bilinear)
    
    // average the input textures based on the weight
    let merged_texture = add_texture_weighted(ref_texture, comp_texture, weight_texture_upsampled)
    
    return merged_texture
}

