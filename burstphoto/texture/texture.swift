
import Foundation
import MetalPerformanceShaders

let add_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture")!)
let add_texture_exposure_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture_exposure")!)
let add_texture_highlights_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture_highlights")!)
let add_texture_weighted_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture_weighted")!)
let average_x_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "average_x")!)
let average_y_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "average_y")!)
let average_x_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "average_x_rgba")!)
let average_y_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "average_y_rgba")!)
let blur_mosaic_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "blur_mosaic_texture")!)
let convert_float_to_uint16_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_float_to_uint16")!)
let convert_uint16_to_float_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_uint16_to_float")!)
let convert_to_bayer_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_to_bayer")!)
let convert_to_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_to_rgba")!)
let copy_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "copy_texture")!)
let correct_hotpixels_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "correct_hotpixels")!)
let crop_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "crop_texture")!)
let extend_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "extend_texture")!)
let fill_with_zeros_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "fill_with_zeros")!)
let normalize_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "normalize_texture")!)
let upsample_bilinear_float_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "upsample_bilinear_float")!)
let upsample_nearest_int_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "upsample_nearest_int")!)


func add_texture(_ in_texture: MTLTexture, _ out_texture: MTLTexture, _ n_textures: Int) {
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = add_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Float32(n_textures)], length: MemoryLayout<Float32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func add_texture_highlights(_ in_texture: MTLTexture, _ out_texture: MTLTexture, _ n_textures: Int, _ white_level: Int, _ black_level: [Int], _ color_factors: [Double], _ comp_idx: Int) {
    
    let black_level_mean = 0.25*Double(black_level[comp_idx*4+0] + black_level[comp_idx*4+1] + black_level[comp_idx*4+2] + black_level[comp_idx*4+3])

    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = add_texture_highlights_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width/2, height: in_texture.height/2, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Float32(n_textures)], length: MemoryLayout<Float32>.stride, index: 0)
    command_encoder.setBytes([Float32(white_level)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.setBytes([Float32(black_level_mean)], length: MemoryLayout<Float32>.stride, index: 2)
    command_encoder.setBytes([Float32(color_factors[comp_idx*3+0]/color_factors[comp_idx*3+1])], length: MemoryLayout<Float32>.stride, index: 3)
    command_encoder.setBytes([Float32(color_factors[comp_idx*3+2]/color_factors[comp_idx*3+1])], length: MemoryLayout<Float32>.stride, index: 4)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func add_texture_exposure(_ in_texture: MTLTexture, _ out_texture: MTLTexture, _ norm_texture: MTLTexture, _ exposure_bias: Int, _ white_level: Int, _ black_level: [Int], _ comp_idx: Int) {
    
    let black_level_mean = 0.25*Double(black_level[comp_idx*4+0] + black_level[comp_idx*4+1] + black_level[comp_idx*4+2] + black_level[comp_idx*4+3])
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = add_texture_exposure_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setTexture(norm_texture, index: 2)
    command_encoder.setBytes([Int32(exposure_bias)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Float32(white_level)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.setBytes([Float32(black_level_mean)], length: MemoryLayout<Float32>.stride, index: 2)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func add_texture_weighted(_ texture1: MTLTexture, _ texture2: MTLTexture, _ weight_texture: MTLTexture) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture1.pixelFormat, width: texture1.width, height: texture1.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = add_texture_weighted_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: texture1.width, height: texture1.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(texture1, index: 0)
    command_encoder.setTexture(texture2, index: 1)
    command_encoder.setTexture(weight_texture, index: 2)
    command_encoder.setTexture(out_texture, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


func blur_mosaic_texture(_ in_texture: MTLTexture, _ kernel_size: Int, _ mosaic_pattern_width: Int) -> MTLTexture {
    
    // create a temp texture blurred along x-axis only and the output texture, blurred along both x- and y-axis
    let blur_x = texture_like(in_texture)
    let blur_xy = texture_like(in_texture)
    
    let kernel_size_mapped = (kernel_size == 16) ? 16 : max(0, min(8, kernel_size))
    
    // blur the texture along the x-axis
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = blur_mosaic_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(blur_x, index: 1)
    command_encoder.setBytes([Int32(kernel_size_mapped)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(mosaic_pattern_width)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(in_texture.width)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(0)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    
    // blur the texture along the y-axis
    command_encoder.setTexture(blur_x, index: 0)
    command_encoder.setTexture(blur_xy, index: 1)
    command_encoder.setBytes([Int32(in_texture.height)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(1)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return blur_xy
}


/// Convert a texture of floats into 16 bit uints for storing in a DNG file.
func convert_float_to_uint16(_ in_texture: MTLTexture, _ white_level: Int, _ black_level: [Int], _ ref_idx: Int, _ factor_16bit: Int, _ color_factors: [Double]) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Uint, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_float_to_uint16_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: out_texture.width/2, height: out_texture.height/2, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Int32(white_level)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(black_level[ref_idx*4+0])], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(black_level[ref_idx*4+1])], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(black_level[ref_idx*4+2])], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.setBytes([Int32(black_level[ref_idx*4+3])], length: MemoryLayout<Int32>.stride, index: 4)
    command_encoder.setBytes([Float32(color_factors[0])], length: MemoryLayout<Float32>.stride, index: 5)
    command_encoder.setBytes([Float32(color_factors[1])], length: MemoryLayout<Float32>.stride, index: 6)
    command_encoder.setBytes([Float32(color_factors[2])], length: MemoryLayout<Float32>.stride, index: 7)    
    command_encoder.setBytes([Int32(factor_16bit)], length: MemoryLayout<Int32>.stride, index: 8)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()

    return out_texture
}


/// Convert a texture of 16-bit uints, from DNG, into 16 bit floats to avoid the quantazation errors that would come with doing the pipeline with integers.
func convert_uint16_to_float(_ in_texture: MTLTexture) -> MTLTexture {

    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Float, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_uint16_to_float_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


func convert_to_rgba(_ in_texture: MTLTexture, _ crop_merge_x: Int, _ crop_merge_y: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba16Float, width: (in_texture.width-2*crop_merge_x)/2, height: (in_texture.height-2*crop_merge_y)/2, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
        
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_to_rgba_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: out_texture.width, height: out_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Int32(crop_merge_x)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(crop_merge_y)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


func convert_to_bayer(_ in_texture: MTLTexture) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: in_texture.pixelFormat, width: in_texture.width*2, height: in_texture.height*2, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
        
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_to_bayer_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


/// Create a deep copy of the passed in texture.
func copy_texture(_ in_texture: MTLTexture) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: in_texture.pixelFormat, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = copy_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


func correct_hotpixels(_ textures: [MTLTexture], _ black_level: [Int], _ ISO_exposure_time: [Double], _ noise_reduction: Double) {
    
    var correction_strength = 1.0
    
    // calculate hot pixel correction strength based on ISO value, exposure time and number of frames in the burst
    if ISO_exposure_time[0] > 0.0 {
        
        correction_strength = 0.0
        for comp_idx in 0..<textures.count {
            correction_strength += ISO_exposure_time[comp_idx]
        }
        correction_strength = (min(max(correction_strength/sqrt(Double(textures.count)) * (noise_reduction==23.0 ? 0.25 : 1.00), 5.0), 80.0)-5.0)/75.0
    }
    
    // only apply hot pixel correction if correction strength is larger than 0.001
    if correction_strength > 0.001 {
    
        // generate simple average of all textures
        let average_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: textures[0].width, height: textures[0].height, mipmapped: false)
        average_texture_descriptor.usage = [.shaderRead, .shaderWrite]
        let average_texture = device.makeTexture(descriptor: average_texture_descriptor)!
        fill_with_zeros(average_texture)
        
        // iterate over all images
        for comp_idx in 0..<textures.count {
            
            add_texture(textures[comp_idx], average_texture, textures.count)
        }
        
        // calculate mean value specific for each color channel
        let mean_texture_buffer = texture_mean(convert_to_rgba(average_texture, 0, 0), "rgba")
        
        // standard parameters if black level is not available / available
        let hot_pixel_threshold     = (black_level[0] == -1) ? 1.0 : 2.0
        let hot_pixel_multiplicator = (black_level[0] == -1) ? 2.0 : 1.0
        
        // iterate over all images and correct hot pixels in each texture
        for comp_idx in 0..<textures.count {
            
            let black_level0 = (black_level[0] == -1) ? Int(0) : black_level[comp_idx*4+0]
            let black_level1 = (black_level[0] == -1) ? Int(0) : black_level[comp_idx*4+1]
            let black_level2 = (black_level[0] == -1) ? Int(0) : black_level[comp_idx*4+2]
            let black_level3 = (black_level[0] == -1) ? Int(0) : black_level[comp_idx*4+3]
            
            let tmp_texture = copy_texture(textures[comp_idx])
            
            let command_buffer = command_queue.makeCommandBuffer()!
            let command_encoder = command_buffer.makeComputeCommandEncoder()!
            let state = correct_hotpixels_state
            command_encoder.setComputePipelineState(state)
            let threads_per_grid = MTLSize(width: average_texture.width-4, height: average_texture.height-4, depth: 1)
            let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
            command_encoder.setTexture(average_texture, index: 0)
            command_encoder.setTexture(tmp_texture, index: 1)
            command_encoder.setTexture(textures[comp_idx], index: 2)
            command_encoder.setBuffer(mean_texture_buffer, offset: 0, index: 0)
            command_encoder.setBytes([Int32(black_level0)], length: MemoryLayout<Int32>.stride, index: 1)
            command_encoder.setBytes([Int32(black_level1)], length: MemoryLayout<Int32>.stride, index: 2)
            command_encoder.setBytes([Int32(black_level2)], length: MemoryLayout<Int32>.stride, index: 3)
            command_encoder.setBytes([Int32(black_level3)], length: MemoryLayout<Int32>.stride, index: 4)
            command_encoder.setBytes([Float32(hot_pixel_threshold)], length: MemoryLayout<Float32>.stride, index: 5)
            command_encoder.setBytes([Float32(hot_pixel_multiplicator)], length: MemoryLayout<Float32>.stride, index: 6)
            command_encoder.setBytes([Float32(correction_strength)], length: MemoryLayout<Float32>.stride, index: 7)
            command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
            command_encoder.endEncoding()
            command_buffer.commit()
        }
    }
}


func crop_texture(_ in_texture: MTLTexture, _ pad_left: Int, _ pad_right: Int, _ pad_top: Int, _ pad_bottom: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: in_texture.pixelFormat, width: in_texture.width-pad_left-pad_right, height: in_texture.height-pad_top-pad_bottom, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = crop_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: out_texture.width, height: out_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Int32(pad_left)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(pad_top)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()

    return out_texture
}


func extend_texture(_ in_texture: MTLTexture, _ pad_left: Int, _ pad_right: Int, _ pad_top: Int, _ pad_bottom: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Float, width: in_texture.width+pad_left+pad_right, height: in_texture.height+pad_top+pad_bottom, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    fill_with_zeros(out_texture)
        
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = extend_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Int32(pad_left)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(pad_top)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()

    return out_texture
}

/// Initialize the passed in texture with zeros.
func fill_with_zeros(_ texture: MTLTexture) {
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = fill_with_zeros_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: texture.width, height: texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(texture, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func get_threads_per_thread_group(_ state: MTLComputePipelineState, _ threads_per_grid: MTLSize) -> MTLSize {
    var available_threads = state.maxTotalThreadsPerThreadgroup
    if threads_per_grid.depth > available_threads {
        return MTLSize(width: 1, height: 1, depth: available_threads)
    } else {
        available_threads /= threads_per_grid.depth
        if threads_per_grid.height > available_threads {
            return MTLSize(width: 1, height: available_threads, depth: threads_per_grid.depth)
        } else {
            available_threads /= threads_per_grid.height
            return MTLSize(width: available_threads, height: threads_per_grid.height, depth: threads_per_grid.depth)
        }
    }
}


func normalize_texture(_ in_texture: MTLTexture, _ norm_texture: MTLTexture) {
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = normalize_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(norm_texture, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


/// Create and return a new texture that has the same properties as the one passed in.
func texture_like(_ input_texture: MTLTexture) -> MTLTexture {
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: input_texture.width, height: input_texture.height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    return output_texture
}


/// Function to calculate the average pixel value over the whole of a texture. If `pixelformat` is `rgba` then each channel will have an independent average calculated. Otherwise, a single value will be returned for the whole texture.
func texture_mean(_ in_texture: MTLTexture, _ pixelformat: String) -> MTLBuffer {
    // create a 1d texture that will contain the averages of the input texture along the x-axis
    let texture_descriptor = MTLTextureDescriptor()
    texture_descriptor.textureType = .type1D
    texture_descriptor.pixelFormat = in_texture.pixelFormat
    texture_descriptor.width = in_texture.width
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let avg_y = device.makeTexture(descriptor: texture_descriptor)!
    
    // average the input texture along the y-axis
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = (pixelformat == "rgba" ? average_y_rgba_state : average_y_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: 1, depth: 1)
    let max_threads_per_thread_group = state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(avg_y, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    
    // average the generated 1d texture along the x-axis
    let state2 = (pixelformat == "rgba" ? average_x_rgba_state : average_x_state)
    command_encoder.setComputePipelineState(state2)
    let avg_buffer = device.makeBuffer(length: (pixelformat=="rgba" ? 4 : 1)*MemoryLayout<Float32>.size, options: .storageModeShared)!
    command_encoder.setTexture(avg_y, index: 0)
    command_encoder.setBuffer(avg_buffer, offset: 0, index: 0)
    command_encoder.setBytes([Int32(in_texture.width)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return avg_buffer
}


func upsample(_ input_texture: MTLTexture, width: Int, height: Int, mode: String) -> MTLTexture {
    let scale_x = Double(width) / Double(input_texture.width)
    let scale_y = Double(height) / Double(input_texture.height)
    
    // create output texture
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: width, height: height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = (mode == "bilinear" ? upsample_bilinear_float_state : upsample_nearest_int_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(input_texture, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    command_encoder.setBytes([Float32(scale_x)], length: MemoryLayout<Float32>.stride, index: 0)
    command_encoder.setBytes([Float32(scale_y)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return output_texture
}
