/**
 Functions related to frequency-domain merging of images.
 */

import Foundation
import MetalPerformanceShaders

let merge_frequency_domain_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "merge_frequency_domain")!)

let calculate_abs_diff_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_abs_diff_rgba")!)
let calculate_highlights_norm_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_highlights_norm_rgba")!)
let calculate_mismatch_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_mismatch_rgba")!)
let calculate_rms_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_rms_rgba")!)
let deconvolute_frequency_domain_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "deconvolute_frequency_domain")!)
let normalize_mismatch_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "normalize_mismatch")!)
let reduce_artifacts_tile_border_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "reduce_artifacts_tile_border")!)

let backward_dft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "backward_dft")!)
let backward_fft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "backward_fft")!)
let forward_dft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "forward_dft")!)
let forward_fft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "forward_fft")!)


/// Convenience function for the frequency-based merging approach.
///
/// Perform the merging 4 times with a slight displacement between the frame to supress artifacts in the merging process.
/// The shift is equal to to the tile size used in the merging process, which later translates into tile\_size\_merge/2 when each color channel is processed independently.
///
/// Currently only supports Bayer raw files
func align_merge_frequency_domain(progress: ProcessingProgress, ref_idx: Int, mosaic_pattern_width: Int, search_distance: Int, tile_size: Int, noise_reduction: Double, uniform_exposure: Bool, exposure_bias: [Int], white_level: Int, black_level: [[Int]], color_factors: [[Double]], textures: [MTLTexture], hotpixel_weight_texture: MTLTexture, final_texture: MTLTexture) throws {
    print("Merging in the frequency domain...")
    
    // The tile size for merging in frequency domain is set to 8x8 for all tile sizes used for alignment. The smaller tile size leads to a reduction of artifacts at specular highlights at the expense of a slightly reduced suppression of low-frequency noise in the shadows. The fixed value of 8 is supported by the highly-optimized fast Fourier transform. A slow, but easier to understand discrete Fourier transform is also provided for values larger than 8.
    // see https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf for more details
    let tile_size_merge = Int(8)

    // These corrections account for the fact that bursts with exposure bracketing include images with longer exposure times, which exhibit a better signal-to-noise ratio. Thus the expected noise level n used in the merging equation d^2/(d^2 + n) has to be reduced to get a comparable noise reduction strength on average (exposure_corr1). Furthermore, a second correction (exposure_corr2) takes into account that images with longer exposure get slightly larger weights than images with shorter exposure (applied as an increased value for the parameter max_motion_norm => see call of function merge_frequency_domain). The simplified formulas do not correctly include read noise (a square is ignored) and as a consequence, merging weights in the shadows will be very slightly overestimated. As the shadows are typically lifted heavily in HDR shots, this effect may be even preferable.
    var exposure_corr1 = 0.0
    var exposure_corr2 = 0.0

    for comp_idx in 0..<exposure_bias.count {
        let exposure_factor = pow(2.0, (Double(exposure_bias[comp_idx]-exposure_bias[ref_idx])/100.0))
        exposure_corr1 += (0.5 + 0.5/exposure_factor)
        exposure_corr2 += min(4.0, exposure_factor)
    }
    exposure_corr1 /= Double(exposure_bias.count)
    exposure_corr2 /= Double(exposure_bias.count)
            
    // derive normalized robustness value: two steps in noise_reduction (-2.0 in this case) yield an increase by a factor of two in the robustness norm with the idea that the variance of shot noise increases by a factor of two per iso level
    let robustness_rev = 0.5*((uniform_exposure ? 26.5 : 28.5) - Double(Int(noise_reduction+0.5)))
    let robustness_norm = exposure_corr1/exposure_corr2*pow(2.0, (-robustness_rev + 7.5))
    
    // derive estimate of read noise with the idea that read noise increases approx. by a factor of three (stronger than increase in shot noise) per iso level to increase noise reduction in darker regions relative to bright regions
    let read_noise = pow(pow(2.0, (-robustness_rev + 10.0)), 1.6)
    
    // derive a maximum value for the motion norm with the idea that denoising can be stronger in static regions with good alignment compared to regions with motion
    // factors from Google paper: daylight = 1, night = 6, darker night = 14, extreme low-light = 25. We use a continuous value derived from the robustness value to cover a similar range as proposed in the paper
    // see https://graphics.stanford.edu/papers/night-sight-sigasia19/night-sight-sigasia19.pdf for more details
    let max_motion_norm = max(1.0, pow(1.3, (11.0-robustness_rev)))
    
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
    // The minimum size of the frame for the frequency merging has to be texture size + tile_size_merge as 4 frames shifted by tile_size_merge in x, y and x, y are processed in four consecutive runs.
    // For the alignment, the frame may be extended further by pad_align due to the following reason: the alignment is performed on different resolution levels and alignment vectors are upscaled by a simple multiplication by 2. As a consequence, the frame at all resolution levels has to be a multiple of the tile sizes of these resolution levels.
    let tile_factor = div*Int(tile_size_array.last!)
    
    var pad_align_x = Int(ceil(Float(texture_width_orig+tile_size_merge)/Float(tile_factor)))
    pad_align_x = (pad_align_x*Int(tile_factor) - texture_width_orig - tile_size_merge)/2
    
    var pad_align_y = Int(ceil(Float(texture_height_orig+tile_size_merge)/Float(tile_factor)))
    pad_align_y = (pad_align_y*Int(tile_factor) - texture_height_orig - tile_size_merge)/2
    
    // calculate padding for the merging in the frequency domain, which can be applied to the actual image frame + a smaller margin compared to the alignment
    var crop_merge_x = Int(floor(Float(pad_align_x)/Float(2*tile_size_merge)))
    crop_merge_x = crop_merge_x*2*tile_size_merge
    var crop_merge_y = Int(floor(Float(pad_align_y)/Float(2*tile_size_merge)))
    crop_merge_y = crop_merge_y*2*tile_size_merge
    let pad_merge_x = pad_align_x - crop_merge_x
    let pad_merge_y = pad_align_y - crop_merge_y

    // set tile information needed for the merging
    let tile_info_merge = TileInfo(tile_size: tile_size,
                                   tile_size_merge: tile_size_merge,
                                   search_dist: 0,
                                   n_tiles_x: (texture_width_orig+tile_size_merge+2*pad_merge_x)/(2*tile_size_merge),
                                   n_tiles_y: (texture_height_orig+tile_size_merge+2*pad_merge_y)/(2*tile_size_merge),
                                   n_pos_1d: 0,
                                   n_pos_2d: 0)
    
    for i in 1...4 {
        let t0 = DispatchTime.now().uptimeNanoseconds
        // set shift values
        let shift_left   = i % 2 == 0   ? tile_size_merge : 0;
        let shift_right  = i % 2 == 1   ? tile_size_merge : 0;
        let shift_top    = i < 3        ? tile_size_merge : 0;
        let shift_bottom = i >= 3       ? tile_size_merge : 0;
        
        // add shifts for artifact suppression
        let pad_left   = pad_align_x + shift_left
        let pad_right  = pad_align_x + shift_right
        let pad_top    = pad_align_y + shift_top
        let pad_bottom = pad_align_y + shift_bottom
        
        // prepare reference texture by correcting hot pixels, equalizing exposure and extending the texture
        let ref_texture = prepare_texture(textures[ref_idx], hotpixel_weight_texture, pad_left, pad_right, pad_top, pad_bottom, 0, black_level, ref_idx)
        // convert reference texture into RGBA pixel format that SIMD instructions can be applied
        let ref_texture_rgba = convert_to_rgba(ref_texture, crop_merge_x, crop_merge_y)
        
        var black_level_mean = Double(black_level[ref_idx].reduce(0, +)) / Double(black_level[ref_idx].count)
        
        // build reference pyramid
        let ref_pyramid = build_pyramid(ref_texture, downscale_factor_array, black_level_mean, color_factors[ref_idx])
              
        // estimate noise level of tiles
        let rms_texture = calculate_rms_rgba(ref_texture_rgba, tile_info_merge)
        
        // generate texture to accumulate the total mismatch
        let total_mismatch_texture = texture_like(rms_texture)
        fill_with_zeros(total_mismatch_texture)
        
        // transform reference texture into the frequency domain
        let ref_texture_ft = forward_ft(ref_texture_rgba, tile_info_merge)
        
        // add reference texture to the final texture
        let final_texture_ft = copy_texture(ref_texture_ft)
        
        // iterate over comparison images
        for comp_idx in 0..<textures.count {
            if comp_idx == ref_idx {
                continue
            }
            
            // prepare comparison texture by correcting hot pixels, equalizing exposure and extending the texture
            let comp_texture = prepare_texture(textures[comp_idx], hotpixel_weight_texture, pad_left, pad_right, pad_top, pad_bottom, (exposure_bias[ref_idx]-exposure_bias[comp_idx]), black_level, comp_idx)
            
            black_level_mean = Double(black_level[comp_idx].reduce(0, +)) / Double(black_level[comp_idx].count)
            
            // align comparison texture
            let aligned_texture_rgba = convert_to_rgba(
                align_texture(ref_pyramid, comp_texture, downscale_factor_array, tile_size_array, search_dist_array, (exposure_bias[comp_idx]==exposure_bias[ref_idx]), black_level_mean, color_factors[comp_idx]),
                crop_merge_x,
                crop_merge_y
            )
            
            // calculate exposure factor between reference texture and aligned texture
            let exposure_factor = pow(2.0, (Double(exposure_bias[comp_idx]-exposure_bias[ref_idx])/100.0))
            
            // calculate mismatch texture
            let mismatch_texture = calculate_mismatch_rgba(aligned_texture_rgba, ref_texture_rgba, rms_texture, exposure_factor, tile_info_merge)
            
            // normalize mismatch texture
            let mean_mismatch = texture_mean(crop_texture(mismatch_texture, shift_left/tile_size_merge, shift_right/tile_size_merge, shift_top/tile_size_merge, shift_bottom/tile_size_merge), .r)
            normalize_mismatch(mismatch_texture, mean_mismatch)
            
            // add mismatch texture to the total, accumulated mismatch texture
            add_texture(mismatch_texture, total_mismatch_texture, textures.count)
         
            let highlights_norm_texture = calculate_highlights_norm_rgba(aligned_texture_rgba, exposure_factor, tile_info_merge, (white_level == -1 ? 1000000 : white_level), black_level_mean)
            
            // transform aligned comparison texture into the frequency domain
            let aligned_texture_ft = forward_ft(aligned_texture_rgba, tile_info_merge)
            
            // adapt max motion norm for images with bracketed exposure
            let max_motion_norm_exposure = (uniform_exposure ? max_motion_norm : min(4.0, exposure_factor)*sqrt(max_motion_norm))
            
            // merge aligned comparison texture with reference texture in the frequency domain
            merge_frequency_domain(ref_texture_ft, aligned_texture_ft, final_texture_ft, rms_texture, mismatch_texture, highlights_norm_texture, robustness_norm, read_noise, max_motion_norm_exposure, uniform_exposure, tile_info_merge)
            
            // sync GUI progress
            DispatchQueue.main.async { progress.int += Int(80000000/Double(4*(textures.count-1))) }
        }
        // apply simple deconvolution to slightly correct potential blurring from misalignment of bursts
        deconvolute_frequency_domain(final_texture_ft, total_mismatch_texture, tile_info_merge)
        
        // transform output texture back to image domain
        var output_texture = backward_ft(final_texture_ft, tile_info_merge, textures.count)
        // reduce potential artifacts at tile borders
        reduce_artifacts_tile_border(output_texture, ref_texture_rgba, tile_info_merge, black_level[ref_idx])
        // convert back to the 2x2 pixel structure and crop to original size
        output_texture = convert_to_bayer(output_texture, pad_left-crop_merge_x, pad_right-crop_merge_x, pad_top-crop_merge_y, pad_bottom-crop_merge_y)
        
        // add output texture to the final texture to collect all textures of the four iterations
        add_texture(output_texture, final_texture, 1)
        
        print("Align+merge (\(i)/4): ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
    }
}


func calculate_abs_diff_rgba(_ texture1: MTLTexture, _ texture2: MTLTexture) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture1.pixelFormat, width: texture1.width, height: texture1.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_descriptor.storageMode = .private
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Calculate Abs Diff RGBA"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = calculate_abs_diff_rgba_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: texture1.width, height: texture1.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(texture1, index: 0)
    command_encoder.setTexture(texture2, index: 1)
    command_encoder.setTexture(out_texture, index: 2)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


func calculate_highlights_norm_rgba(_ aligned_texture: MTLTexture, _ exposure_factor: Double, _ tile_info: TileInfo, _ white_level: Int, _ black_level_mean: Double) -> MTLTexture {
  
    // create mismatch texture
    let highlights_norm_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, mipmapped: false)
    highlights_norm_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    highlights_norm_texture_descriptor.storageMode = .private
    let highlights_norm_texture = device.makeTexture(descriptor: highlights_norm_texture_descriptor)!
    highlights_norm_texture.label = "\(aligned_texture.label!.components(separatedBy: ":")[0]): Highlight Norm"
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Highlights Norm"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = calculate_highlights_norm_rgba_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(aligned_texture, index: 0)
    command_encoder.setTexture(highlights_norm_texture, index: 1)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Float32(exposure_factor)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.setBytes([Float32(white_level)], length: MemoryLayout<Float32>.stride, index: 2)
    command_encoder.setBytes([Float32(black_level_mean)], length: MemoryLayout<Float32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return highlights_norm_texture
}


func calculate_mismatch_rgba(_ aligned_texture: MTLTexture, _ ref_texture: MTLTexture, _ rms_texture: MTLTexture, _ exposure_factor: Double, _ tile_info: TileInfo) -> MTLTexture {
  
    // create mismatch texture
    let mismatch_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, mipmapped: false)
    mismatch_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    mismatch_texture_descriptor.storageMode = .private
    let mismatch_texture = device.makeTexture(descriptor: mismatch_texture_descriptor)!
    mismatch_texture.label = "\(aligned_texture.label!.components(separatedBy: ":")[0]): Mismatch"
    
    let abs_diff_texture = calculate_abs_diff_rgba(ref_texture, aligned_texture)
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Mismatch RGBA"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = calculate_mismatch_rgba_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(abs_diff_texture, index: 0)
    command_encoder.setTexture(rms_texture, index: 1)
    command_encoder.setTexture(mismatch_texture, index: 2)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Float32(exposure_factor)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return mismatch_texture
}


func calculate_rms_rgba(_ in_texture: MTLTexture, _ tile_info: TileInfo) -> MTLTexture {
    
    let rms_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, mipmapped: false)
    rms_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    rms_texture_descriptor.storageMode = .private
    let rms_texture = device.makeTexture(descriptor: rms_texture_descriptor)!
    rms_texture.label = "\(in_texture.label!.components(separatedBy: ":")[0]): RMS"
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "RMS RGBA"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = calculate_rms_rgba_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: rms_texture.width, height: rms_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(rms_texture, index: 1)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return rms_texture
}


func deconvolute_frequency_domain(_ final_texture_ft: MTLTexture, _ total_mismatch_texture: MTLTexture, _ tile_info: TileInfo) {
        
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Deconvolute"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = deconvolute_frequency_domain_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(final_texture_ft, index: 0)
    command_encoder.setTexture(total_mismatch_texture, index: 1)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func backward_ft(_ in_texture_ft: MTLTexture, _ tile_info: TileInfo, _ n_textures: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: in_texture_ft.width/2, height: in_texture_ft.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_descriptor.storageMode = .private
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    out_texture.label = "\(in_texture_ft.label!.components(separatedBy: ":")[0]): BT Image"
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Backward FT"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    // either use discrete Fourier transform or highly-optimized fast Fourier transform (only possible if tile_size <= 8)
    let state = (tile_info.tile_size_merge <= 8 ? backward_fft_state : backward_dft_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture_ft, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(n_textures)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


func forward_ft(_ in_texture: MTLTexture, _ tile_info: TileInfo) -> MTLTexture {
    
    let out_texture_ft_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: in_texture.width*2, height: in_texture.height, mipmapped: false)
    out_texture_ft_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_ft_descriptor.storageMode = .private
    let out_texture_ft = device.makeTexture(descriptor: out_texture_ft_descriptor)!
    out_texture_ft.label = "\(in_texture.label!.components(separatedBy: ":")[0]): FT Image"

    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Forward FT"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    // either use discrete Fourier transform or highly-optimized fast Fourier transform (only possible if tile_size <= 8)
    let state = (tile_info.tile_size_merge <= 8 ? forward_fft_state : forward_dft_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture_ft, index: 1)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture_ft
}


func merge_frequency_domain(_ ref_texture_ft: MTLTexture, _ aligned_texture_ft: MTLTexture, _ out_texture_ft: MTLTexture, _ rms_texture: MTLTexture, _ mismatch_texture: MTLTexture, _ highlights_norm_texture: MTLTexture, _ robustness_norm: Double, _ read_noise: Double, _ max_motion_norm: Double, _ uniform_exposure: Bool, _ tile_info: TileInfo) {
        
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Merge Frequency"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = merge_frequency_domain_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(ref_texture_ft, index: 0)
    command_encoder.setTexture(aligned_texture_ft, index: 1)
    command_encoder.setTexture(out_texture_ft, index: 2)
    command_encoder.setTexture(rms_texture, index: 3)
    command_encoder.setTexture(mismatch_texture, index: 4)
    command_encoder.setTexture(highlights_norm_texture, index: 5)
    command_encoder.setBytes([Float32(robustness_norm)], length: MemoryLayout<Float32>.stride, index: 0)
    command_encoder.setBytes([Float32(read_noise)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.setBytes([Float32(max_motion_norm)], length: MemoryLayout<Float32>.stride, index: 2)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.setBytes([Int32(uniform_exposure ? 1 : 0)], length: MemoryLayout<Int32>.stride, index: 4)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}

func normalize_mismatch(_ mismatch_texture: MTLTexture, _ mean_mismatch_buffer: MTLBuffer) {
   
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Normalize Mismatch"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = normalize_mismatch_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: mismatch_texture.width, height: mismatch_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(mismatch_texture, index: 0)
    command_encoder.setBuffer(mean_mismatch_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func reduce_artifacts_tile_border(_ out_texture: MTLTexture, _ ref_texture: MTLTexture, _ tile_info: TileInfo, _ black_level: [Int]) {
        
    if (black_level[0] != -1) {
        let command_buffer = command_queue.makeCommandBuffer()!
        command_buffer.label = "Reduce Artifacts at Tile Border"
        let command_encoder = command_buffer.makeComputeCommandEncoder()!
        let state = reduce_artifacts_tile_border_state
        command_encoder.setComputePipelineState(state)
        let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
        let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
        command_encoder.setTexture(out_texture, index: 0)
        command_encoder.setTexture(ref_texture, index: 1)
        command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
        command_encoder.setBytes([Int32(black_level[0])], length: MemoryLayout<Int32>.stride, index: 1)
        command_encoder.setBytes([Int32(black_level[1])], length: MemoryLayout<Int32>.stride, index: 2)
        command_encoder.setBytes([Int32(black_level[2])], length: MemoryLayout<Int32>.stride, index: 3)
        command_encoder.setBytes([Int32(black_level[3])], length: MemoryLayout<Int32>.stride, index: 4)
        command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
        command_encoder.endEncoding()
        command_buffer.commit()
    }
}
