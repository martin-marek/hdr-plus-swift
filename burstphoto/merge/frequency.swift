/**
 Functions related to frequency-domain merging of images.
 */

import Foundation
import MetalPerformanceShaders

let merge_frequency_domain_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "merge_frequency_domain")!)

let calculate_mismatch_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_mismatch_rgba")!)
let calculate_rms_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_rms_rgba")!)
let deconvolute_frequency_domain_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "deconvolute_frequency_domain")!)
let normalize_mismatch_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "normalize_mismatch")!)
let reduce_artifacts_tile_border_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "reduce_artifacts_tile_border")!)

let backward_dft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "backward_dft")!)
let backward_fft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "backward_fft")!)
let forward_dft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "forward_dft")!)
let forward_fft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "forward_fft")!)


/// Convenience function for the frequency-based merging approach
func align_merge_frequency_domain(progress: ProcessingProgress, shift_left_not_right: Bool, shift_top_not_bottom: Bool, ref_idx: Int, mosaic_pattern_width: Int, search_distance: Int, tile_size: Int, tile_size_merge: Int, robustness_norm: Double, read_noise: Double, max_motion_norm: Double, uniform_exposure: Bool, exposure_bias: [Int], black_level: [Int], color_factors: [Double], textures: [MTLTexture], final_texture: MTLTexture) throws {
    
    // set original texture size
    let texture_width_orig = textures[ref_idx].width
    let texture_height_orig = textures[ref_idx].height
    
    // set shift values
    let shift_left   = shift_left_not_right ? tile_size_merge : 0;
    let shift_right  = shift_left_not_right ? 0 : tile_size_merge;
    let shift_top    = shift_top_not_bottom ? tile_size_merge : 0;
    let shift_bottom = shift_top_not_bottom ? 0 : tile_size_merge;
    
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
  
    // add shifts for artifact suppression
    let pad_left   = pad_align_x + shift_left
    let pad_right  = pad_align_x + shift_right
    let pad_top    = pad_align_y + shift_top
    let pad_bottom = pad_align_y + shift_bottom

    // calculate padding for the merging in the frequency domain, which can be applied to the actual image frame + a smaller margin compared to the alignment
    var crop_merge_x = Int(floor(Float(pad_align_x)/Float(2*tile_size_merge)))
    crop_merge_x = crop_merge_x*2*tile_size_merge
    var crop_merge_y = Int(floor(Float(pad_align_y)/Float(2*tile_size_merge)))
    crop_merge_y = crop_merge_y*2*tile_size_merge
    
    let pad_merge_x = pad_align_x - crop_merge_x
    let pad_merge_y = pad_align_y - crop_merge_y
             
    // set and extend reference texture
    let ref_texture = extend_texture(textures[ref_idx], pad_left, pad_right, pad_top, pad_bottom)
       
    var color_factors3 = [color_factors[ref_idx*3+0], color_factors[ref_idx*3+1], color_factors[ref_idx*3+2]]
    
    // build reference pyramid
    let ref_pyramid = build_pyramid(ref_texture, downscale_factor_array, color_factors3)
    
    // initialize textures to store real and imaginary parts of the reference texture, the aligned comparison texture and a temp texture used inside the Fourier transform functions
    let ref_texture_ft_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: (texture_width_orig+tile_size_merge+2*pad_merge_x), height: (texture_height_orig+tile_size_merge+2*pad_merge_y)/2, mipmapped: false)
    ref_texture_ft_descriptor.usage = [.shaderRead, .shaderWrite]
    let ref_texture_ft = device.makeTexture(descriptor: ref_texture_ft_descriptor)!
    
    let aligned_texture_ft_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: (texture_width_orig+tile_size_merge+2*pad_merge_x), height: (texture_height_orig+tile_size_merge+2*pad_merge_y)/2, mipmapped: false)
    aligned_texture_ft_descriptor.usage = [.shaderRead, .shaderWrite]
    let aligned_texture_ft = device.makeTexture(descriptor: aligned_texture_ft_descriptor)!
    
    let tmp_texture_ft_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: (texture_width_orig+tile_size_merge+2*pad_merge_x), height: (texture_height_orig+tile_size_merge+2*pad_merge_y)/2, mipmapped: false)
    tmp_texture_ft_descriptor.usage = [.shaderRead, .shaderWrite]
    let tmp_texture_ft = device.makeTexture(descriptor: tmp_texture_ft_descriptor)!

    // set tile information needed for the merging
    let tile_info_merge = TileInfo(tile_size: tile_size, tile_size_merge: tile_size_merge, search_dist: 0, n_tiles_x: (texture_width_orig+tile_size_merge+2*pad_merge_x)/(2*tile_size_merge), n_tiles_y: (texture_height_orig+tile_size_merge+2*pad_merge_y)/(2*tile_size_merge), n_pos_1d: 0, n_pos_2d: 0)
  
    // convert reference texture into RGBA pixel format that SIMD instructions can be applied
    let ref_texture_rgba = convert_to_rgba(ref_texture, crop_merge_x, crop_merge_y)
    
    // estimate noise level of tiles
    let rms_texture = calculate_rms_rgba(ref_texture_rgba, tile_info_merge)
    
    // generate texture to accumulate the total mismatch
    let total_mismatch_texture = texture_like(rms_texture)
    fill_with_zeros(total_mismatch_texture)
    
    // transform reference texture into the frequency domain
    forward_ft(ref_texture_rgba, ref_texture_ft, tmp_texture_ft, tile_info_merge)
    
    // add reference texture to the final texture
    let final_texture_ft = copy_texture(ref_texture_ft)
   
    // iterate over comparison images
    for comp_idx in 0..<textures.count {
  
        let t0 = DispatchTime.now().uptimeNanoseconds
        
        // if comparison image is equal to reference image, do nothing
        if comp_idx == ref_idx {
            continue
        }
         
        // set and extend comparison texture
        let comp_texture = extend_texture(textures[comp_idx], pad_left, pad_right, pad_top, pad_bottom)
        
        // check that the comparison image has the same resolution as the reference image
        if (ref_texture.width != comp_texture.width) || (ref_texture.height != comp_texture.height) {
            throw AlignmentError.inconsistent_resolutions
        }
        
        let black_level_mean = 0.25*Double(black_level[comp_idx*4+0] + black_level[comp_idx*4+1] + black_level[comp_idx*4+2] + black_level[comp_idx*4+3])
        color_factors3 = [color_factors[comp_idx*3+0], color_factors[comp_idx*3+1], color_factors[comp_idx*3+2]]
        
        // align comparison texture
        let aligned_texture_rgba = convert_to_rgba(align_texture(ref_pyramid, comp_texture, downscale_factor_array, tile_size_array, search_dist_array, uniform_exposure, black_level_mean, color_factors3), crop_merge_x, crop_merge_y)
                 
        // calculate exposure factor between reference texture and aligned texture
        let exposure_factor = pow(2.0, (Double(exposure_bias[comp_idx]-exposure_bias[ref_idx])/100.0))
        
        // calculate mismatch texture
        let mismatch_texture = calculate_mismatch_rgba(aligned_texture_rgba, ref_texture_rgba, rms_texture, exposure_factor, tile_info_merge)

        // normalize mismatch texture
        let mean_mismatch = texture_mean(crop_texture(mismatch_texture, shift_left/tile_size_merge, shift_right/tile_size_merge, shift_top/tile_size_merge, shift_bottom/tile_size_merge), "r")
        normalize_mismatch(mismatch_texture, mean_mismatch)
           
        // add mismatch texture to the total, accumulated mismatch texture
        add_texture(mismatch_texture, total_mismatch_texture, textures.count)
        
        // start debug capture
        //let capture_manager = MTLCaptureManager.shared()
        //let capture_descriptor = MTLCaptureDescriptor()
        //capture_descriptor.captureObject = device
        //try! capture_manager.startCapture(with: capture_descriptor)
          
        // transform aligned comparison texture into the frequency domain
        forward_ft(aligned_texture_rgba, aligned_texture_ft, tmp_texture_ft, tile_info_merge)
        
        // adapt max motion norm for images with bracketed exposure
        let max_motion_norm_exposure = (uniform_exposure ? max_motion_norm : min(4.0, exposure_factor)*sqrt(max_motion_norm))
               
        // merge aligned comparison texture with reference texture in the frequency domain
        merge_frequency_domain(ref_texture_ft, aligned_texture_ft, final_texture_ft, rms_texture, mismatch_texture, robustness_norm, read_noise, max_motion_norm_exposure, uniform_exposure, tile_info_merge)
               
        // stop debug capture
        //capture_manager.stopCapture()
      
        // sync GUI progress
        print("Align+merge: ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
        DispatchQueue.main.async { progress.int += Int(80000000/Double(4*(textures.count-1))) }
    }
    
    // apply simple deconvolution to slightly correct potential blurring from misalignment of bursts
    deconvolute_frequency_domain(final_texture_ft, total_mismatch_texture, tile_info_merge)
    
    // transform output texture back to image domain
    var output_texture = backward_ft(final_texture_ft, tmp_texture_ft, tile_info_merge, textures.count)
    // reduce potential artifacts at tile borders
    reduce_artifacts_tile_border(output_texture, ref_texture_rgba, tile_info_merge, black_level, ref_idx)
    // convert back to the 2x2 pixel structure and crop to original size
    output_texture = crop_texture(convert_to_bayer(output_texture), pad_left-crop_merge_x, pad_right-crop_merge_x, pad_top-crop_merge_y, pad_bottom-crop_merge_y)
    
    // add output texture to the final texture to collect all textures of the four iterations
    add_texture(output_texture, final_texture, 1)
}


func calculate_mismatch_rgba(_ aligned_texture: MTLTexture, _ ref_texture: MTLTexture, _ rms_texture: MTLTexture, _ exposure_factor: Double, _ tile_info: TileInfo) -> MTLTexture {
  
    // create mismatch texture
    let mismatch_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, mipmapped: false)
    mismatch_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let mismatch_texture = device.makeTexture(descriptor: mismatch_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = calculate_mismatch_rgba_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(aligned_texture, index: 0)
    command_encoder.setTexture(ref_texture, index: 1)
    command_encoder.setTexture(rms_texture, index: 2)
    command_encoder.setTexture(mismatch_texture, index: 3)
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
    let rms_texture = device.makeTexture(descriptor: rms_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
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


func backward_ft(_ in_texture_ft: MTLTexture, _ tmp_texture_ft: MTLTexture, _ tile_info: TileInfo, _ n_textures: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: in_texture_ft.width/2, height: in_texture_ft.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    // either use discrete Fourier transform or highly-optimized fast Fourier transform (only possible if tile_size <= 16)
    let state = (tile_info.tile_size_merge <= 16 ? backward_fft_state : backward_dft_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture_ft, index: 0)
    command_encoder.setTexture(tmp_texture_ft, index: 1)
    command_encoder.setTexture(out_texture, index: 2)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(n_textures)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


func forward_ft(_ in_texture: MTLTexture, _ out_texture_ft: MTLTexture, _ tmp_texture_ft: MTLTexture, _ tile_info: TileInfo) {
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    // either use discrete Fourier transform or highly-optimized fast Fourier transform (only possible if tile_size <= 16)
    let state = (tile_info.tile_size_merge <= 16 ? forward_fft_state : forward_dft_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(tmp_texture_ft, index: 1)
    command_encoder.setTexture(out_texture_ft, index: 2)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func merge_frequency_domain(_ ref_texture_ft: MTLTexture, _ aligned_texture_ft: MTLTexture, _ out_texture_ft: MTLTexture, _ rms_texture: MTLTexture, _ mismatch_texture: MTLTexture, _ robustness_norm: Double, _ read_noise: Double, _ max_motion_norm: Double, _ uniform_exposure: Bool, _ tile_info: TileInfo) {
        
    let command_buffer = command_queue.makeCommandBuffer()!
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


func reduce_artifacts_tile_border(_ output_texture: MTLTexture, _ ref_texture: MTLTexture, _ tile_info: TileInfo, _ black_level: [Int], _ ref_idx: Int) {
        
    if (black_level[0] != -1) {
        
        let command_buffer = command_queue.makeCommandBuffer()!
        let command_encoder = command_buffer.makeComputeCommandEncoder()!
        let state = reduce_artifacts_tile_border_state
        command_encoder.setComputePipelineState(state)
        let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
        let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
        command_encoder.setTexture(output_texture, index: 0)
        command_encoder.setTexture(ref_texture, index: 1)
        command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
        command_encoder.setBytes([Int32(black_level[ref_idx*4+0])], length: MemoryLayout<Int32>.stride, index: 1)
        command_encoder.setBytes([Int32(black_level[ref_idx*4+1])], length: MemoryLayout<Int32>.stride, index: 2)
        command_encoder.setBytes([Int32(black_level[ref_idx*4+2])], length: MemoryLayout<Int32>.stride, index: 3)
        command_encoder.setBytes([Int32(black_level[ref_idx*4+3])], length: MemoryLayout<Int32>.stride, index: 4)
        command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
        command_encoder.endEncoding()
        command_buffer.commit()
    }
}
