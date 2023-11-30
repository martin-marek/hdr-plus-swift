
import Foundation
import MetalPerformanceShaders

let add_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture")!)
let add_texture_exposure_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture_exposure")!)
let add_texture_highlights_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture_highlights")!)
let add_texture_uint16_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture_uint16")!)
let add_texture_weighted_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture_weighted")!)
let blur_mosaic_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "blur_mosaic_texture")!)
let calculate_weight_highlights_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_weight_highlights")!)
let convert_float_to_uint16_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_float_to_uint16")!)
let convert_to_bayer_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_to_bayer")!)
let convert_to_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_to_rgba")!)
let copy_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "copy_texture")!)
let crop_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "crop_texture")!)
let divide_buffer_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "divide_buffer")!)
let sum_divide_buffer_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "sum_divide_buffer")!)
let fill_with_zeros_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "fill_with_zeros")!)
let find_hotpixels_bayer_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "find_hotpixels_bayer")!)
let find_hotpixels_xtrans_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "find_hotpixels_xtrans")!)
let normalize_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "normalize_texture")!)
let prepare_texture_bayer_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "prepare_texture_bayer")!)
let sum_rect_columns_float_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "sum_rect_columns_float")!)
let sum_rect_columns_uint_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "sum_rect_columns_uint")!)
let sum_row_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "sum_row")!)
let upsample_bilinear_float_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "upsample_bilinear_float")!)
let upsample_nearest_int_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "upsample_nearest_int")!)

enum UpsampleType {
    case Bilinear
    case NearestNeighbour
}

func add_texture(_ in_texture: MTLTexture, _ out_texture: MTLTexture, _ n_textures: Int) {
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Add Texture"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = (in_texture.pixelFormat == .r16Uint ? add_texture_uint16_state : add_texture_state)
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


/// This function is intended for averaging of frames with uniform exposure or for adding the darkest frame in an exposure bracketed burst: add frame and apply extrapolation of green channels for very bright pixels. All pixels in the frame get the same global weight of 1. Therefore a scalar value for normalization storing the sum of accumulated frames is sufficient.
func add_texture_highlights(_ in_texture: MTLTexture, _ out_texture: MTLTexture, _ white_level: Int, _ black_level: [Int], _ color_factors: [Double]) {
    
    let black_level_mean = Double(black_level.reduce(0, +)) / Double(black_level.count)

    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Add Texture (Highlights)"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = add_texture_highlights_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width/2, height: in_texture.height/2, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Float32(white_level)], length: MemoryLayout<Float32>.stride, index: 0)
    command_encoder.setBytes([Float32(black_level_mean)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.setBytes([Float32(color_factors[0]/color_factors[1])], length: MemoryLayout<Float32>.stride, index: 2)
    command_encoder.setBytes([Float32(color_factors[2]/color_factors[1])], length: MemoryLayout<Float32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


/// This function is intended for adding up all frames of a bracketed expsoure besides the darkest frame: add frame with exposure-weighting and exclude regions with clipped highlights. Due to the exposure weighting, frames typically have weights > 1. Inside the function, pixel weights are further adapted based on their brightness: in the shadows, weights are linear with exposure. In the midtones/highlights, this converges towards weights being linear with the square-root of exposure. For clipped highlight pixels, the weight becomes zero. As the weights are pixel-specific, a texture for normalization is employed storing the sum of pixel-specific weights.
func add_texture_exposure(_ in_texture: MTLTexture, _ out_texture: MTLTexture, _ norm_texture: MTLTexture, _ exposure_bias: Int, _ white_level: Int, _ black_level: [Int], _ color_factors: [Double], _ mosaic_pattern_width: Int) {
    
    let black_level_mean = Double(black_level.reduce(0, +)) / Double(black_level.count)
    
    let color_factor_mean: Double
    let kernel_size: Int
    if (mosaic_pattern_width == 6) {
        color_factor_mean = (8.0*color_factors[0] + 20.0*color_factors[1] + 8.0*color_factors[2]) / 36.0
        kernel_size       = 2
    } else if (mosaic_pattern_width == 2) {
        color_factor_mean = (    color_factors[0] +  2.0*color_factors[1] +     color_factors[2]) /  4.0
        kernel_size       = 1
    } else {
        color_factor_mean = (    color_factors[0] +      color_factors[1] +     color_factors[2]) /  3.0
        kernel_size       = 1
    }
    
    // the blurred texture serves as an approximation of local luminance
    let in_texture_blurred = blur(in_texture, with_pattern_width: 1, using_kernel_size: kernel_size)
    // blurring of the weight texture ensures a smooth blending of frames, especially at regions where clipped highlight pixels are excluded
    let weight_highlights_texture_blurred = calculate_weight_highlights(in_texture, exposure_bias, white_level, black_level_mean)
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Add Texture (Exposure)"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = add_texture_exposure_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(in_texture_blurred, index: 1)
    command_encoder.setTexture(weight_highlights_texture_blurred, index: 2)
    command_encoder.setTexture(out_texture, index: 3)
    command_encoder.setTexture(norm_texture, index: 4)
    command_encoder.setBytes([Int32(exposure_bias)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Float32(white_level)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.setBytes([Float32(black_level_mean)], length: MemoryLayout<Float32>.stride, index: 2)
    command_encoder.setBytes([Float32(color_factor_mean)], length: MemoryLayout<Float32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


/// Calculate the weighted average of `texture1` and `texture2` using the spatially varying weights specified in `weight_texture`.
/// Larger weights bias towards `texture1`.
func add_texture_weighted(_ texture1: MTLTexture, _ texture2: MTLTexture, _ weight_texture: MTLTexture) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture1.pixelFormat, width: texture1.width, height: texture1.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_descriptor.storageMode = .private
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    out_texture.label = texture1.label
    
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Add Texture (Weighted)"
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


func blur(_ in_texture: MTLTexture, with_pattern_width mosaic_pattern_width: Int, using_kernel_size kernel_size: Int) -> MTLTexture {
    let blurred_in_x_texture  = texture_like(in_texture)
    let blurred_in_xy_texture = texture_like(in_texture)
    blurred_in_x_texture.label  = "\(in_texture.label!.components(separatedBy: ":")[0]): blurred in x by \(kernel_size)"
    blurred_in_xy_texture.label = "\(in_texture.label!.components(separatedBy: ":")[0]): blurred by \(kernel_size)"
    
    let kernel_size_mapped = (kernel_size == 16) ? 16 : max(0, min(8, kernel_size))
    
    // Blur the texture along the x-axis
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Blur"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = blur_mosaic_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(blurred_in_x_texture, index: 1)
    command_encoder.setBytes([Int32(kernel_size_mapped)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(mosaic_pattern_width)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(in_texture.width)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(0)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    
    // Blur along the y-axis
    command_encoder.setTexture(blurred_in_x_texture, index: 0)
    command_encoder.setTexture(blurred_in_xy_texture, index: 1)
    command_encoder.setBytes([Int32(in_texture.height)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(1)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return blurred_in_xy_texture
}


/// Calculate the sum of each subpixel in the mosaic pattern within the specified region.
/// Returns and array of length mosaic\_pattern\_width^2 which is to be treated as a flattened 2D array: 2D(x, y) == 1D(x + width\*y)
func calculate_black_levels(for texture: MTLTexture, from_masked_areas masked_areas: UnsafeMutablePointer<Int32>, mosaic_pattern_width: Int) -> [Int] {
    var num_pixels: Float = 0.0
    var command_buffers: [MTLCommandBuffer] = []
    var black_level_buffers: [MTLBuffer] = []
    var black_level_from_masked_area = [Float](repeating: 0.0, count: Int(mosaic_pattern_width*mosaic_pattern_width))
    
    for i in 0..<4 {
        // Up to 4 masked areas exist, as soon as we reach -1 we know there are no more masked areas after.
        if masked_areas[4*i] == -1 { break }
        let top     = masked_areas[4*i + 0]
        let left    = masked_areas[4*i + 1]
        let bottom  = masked_areas[4*i + 2]
        let right   = masked_areas[4*i + 3]
        
        num_pixels += Float((bottom - top) * (right - left))
        
        // Create output texture from the y-axis blurring
        let texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: Int(right - left), height: mosaic_pattern_width, mipmapped: false)
        texture_descriptor.usage = [.shaderRead, .shaderWrite]
        texture_descriptor.storageMode = .private
        let summed_y = device.makeTexture(descriptor: texture_descriptor)!
        summed_y.label = "\(texture.label!.components(separatedBy: ":")[0]): Summed in y for black level"
        
        // Sum along columns
        let command_buffer = command_queue.makeCommandBuffer()!
        let command_encoder = command_buffer.makeComputeCommandEncoder()!
        command_buffer.label = "Black Levels \(i) for \(String(describing: texture.label))"
        command_encoder.setComputePipelineState(sum_rect_columns_uint_state)
        let thread_groups_per_grid = MTLSize(width: summed_y.width, height: summed_y.height, depth: 1)
        let threads_per_thread_group = get_threads_per_thread_group(sum_rect_columns_uint_state, thread_groups_per_grid)
        
        command_encoder.setTexture(texture, index: 0)
        command_encoder.setTexture(summed_y, index: 1)
        command_encoder.setBytes([top], length: MemoryLayout<Int32>.stride, index: 1)
        command_encoder.setBytes([left], length: MemoryLayout<Int32>.stride, index: 2)
        command_encoder.setBytes([bottom], length: MemoryLayout<Int32>.stride, index: 3)
        command_encoder.setBytes([Int32(mosaic_pattern_width)], length: MemoryLayout<Int32>.stride, index: 4)
        command_encoder.dispatchThreads(thread_groups_per_grid, threadsPerThreadgroup: threads_per_thread_group)
        command_encoder.popDebugGroup()
        
        // Sum along the row
        let sum_buffer = device.makeBuffer(length: (mosaic_pattern_width*mosaic_pattern_width)*MemoryLayout<Float32>.size,
                                           options: .storageModeShared)!
        sum_buffer.label = "\(texture.label!.components(separatedBy: ":")[0]): Black Levels from masked area \(i)"
        command_encoder.setComputePipelineState(sum_row_state)
        command_encoder.setTexture(summed_y, index: 0)
        command_encoder.setBuffer(sum_buffer, offset: 0, index: 0)
        command_encoder.setBytes([Int32(summed_y.width)], length: MemoryLayout<Int32>.stride, index: 1)
        command_encoder.setBytes([Int32(mosaic_pattern_width)], length: MemoryLayout<Int32>.stride, index: 2)
        let threads_per_grid_x = MTLSize(width: mosaic_pattern_width, height: mosaic_pattern_width, depth: 1)
        command_encoder.dispatchThreads(threads_per_grid_x, threadsPerThreadgroup: threads_per_thread_group)
        command_encoder.endEncoding()
        command_buffer.commit()
        
        command_buffers.append(command_buffer)
        black_level_buffers.append(sum_buffer)
    }
    
    if num_pixels > 0 {
        // E.g. if a masked area is 2x2 and we're using a bayer image (2x2 RGGB mosaic pattern), while there are 4 pixels in the masked area, each subpixel (R, G, G, or B) actually only has 1/4 (1 / mosaic_width^2) that number of pixels.
         num_pixels /= Float(mosaic_pattern_width*mosaic_pattern_width)
        
        for i in 0..<command_buffers.count {
            command_buffers[i].waitUntilCompleted()
            let _this_black_levels = black_level_buffers[i].contents().bindMemory(to: Float32.self, capacity: 1)
            for j in 0..<black_level_from_masked_area.count {
                black_level_from_masked_area[j] += _this_black_levels[j] / num_pixels
            }
        }
    }
    
    return black_level_from_masked_area.map { Int(round($0)) }
}


func calculate_weight_highlights(_ in_texture: MTLTexture, _ exposure_bias: Int, _ white_level: Int, _ black_level_mean: Double) -> MTLTexture {
    
    let weight_highlights_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: in_texture.width, height: in_texture.height, mipmapped: false)
    weight_highlights_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    weight_highlights_texture_descriptor.storageMode = .private
    let weight_highlights_texture = device.makeTexture(descriptor: weight_highlights_texture_descriptor)!
    weight_highlights_texture.label = "\(in_texture.label!.components(separatedBy: ":")[0]): Weight Highlights"
  
    let kernel_size = 4
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Calculate highlights weight"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = calculate_weight_highlights_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(weight_highlights_texture, index: 1)
    command_encoder.setBytes([Int32(exposure_bias)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Float32(white_level)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.setBytes([Float32(black_level_mean)], length: MemoryLayout<Float32>.stride, index: 2)
    command_encoder.setBytes([Int32(kernel_size)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    let weight_highlights_texture_blurred = blur(weight_highlights_texture, with_pattern_width: 1, using_kernel_size: 5)
    
    return weight_highlights_texture_blurred
}


/// Convert a texture of floats into 16 bit uints for storing in a DNG file.
func convert_float_to_uint16(_ in_texture: MTLTexture, _ white_level: Int, _ black_level: [Int], _ factor_16bit: Int, _ mosaic_pattern_width: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Uint, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    out_texture.label = "\(in_texture.label!.components(separatedBy: ":")[0]): UInt16"
    
    let black_levels_buffer = device.makeBuffer(bytes: black_level.map{Int32($0)},
                                                length: MemoryLayout<Int32>.size * black_level.count)!
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Float to UInt"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_float_to_uint16_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: out_texture.width, height: out_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Int32(white_level)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(factor_16bit)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(mosaic_pattern_width)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBuffer(black_levels_buffer, offset: 0, index: 3)
    
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()

    return out_texture
}


func convert_to_rgba(_ in_texture: MTLTexture, _ crop_x: Int, _ crop_y: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: (in_texture.pixelFormat == .r16Float ? .rgba16Float : .rgba32Float), width: (in_texture.width-2*crop_x)/2, height: (in_texture.height-2*crop_y)/2, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_descriptor.storageMode = .private
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    out_texture.label = "\(in_texture.label!.components(separatedBy: ":")[0]): Bayer to RGBA"
        
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "To RGBA"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_to_rgba_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: out_texture.width, height: out_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Int32(crop_x)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(crop_y)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


func convert_to_bayer(_ in_texture: MTLTexture) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: (in_texture.pixelFormat == .rgba16Float ? .r16Float : .r32Float), width: in_texture.width*2, height: in_texture.height*2, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_descriptor.storageMode = .private
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    out_texture.label = "\(in_texture.label!.components(separatedBy: ":")[0]): RGBA to Bayer"
        
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "To Bayer"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_to_bayer_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: out_texture.width, height: out_texture.height, depth: 1)
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
    out_texture_descriptor.storageMode = .private
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    out_texture.label = in_texture.label
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Copy Texture"
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


func crop_texture(_ in_texture: MTLTexture, _ pad_left: Int, _ pad_right: Int, _ pad_top: Int, _ pad_bottom: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: in_texture.pixelFormat, width: in_texture.width-pad_left-pad_right, height: in_texture.height-pad_top-pad_bottom, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_descriptor.storageMode = .private
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    out_texture.label = in_texture.label
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Crop Texture"
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


/// Initialize the passed in texture with zeros.
func fill_with_zeros(_ texture: MTLTexture) {
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Fill with Zeros"
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


/// Find hotpixels based on the idea that they will be the same pixels in all frames of the burst.
func find_hotpixels(_ textures: [MTLTexture], _ hotpixel_weight_texture: MTLTexture, _ black_level: [[Int]], _ ISO_exposure_time: [Double], _ noise_reduction: Double, _ mosaic_pattern_width: Int) {
    
    if mosaic_pattern_width != 2 || mosaic_pattern_width != 6 {
        return
    }
    
    // calculate hot pixel correction strength based on ISO value, exposure time and number of frames in the burst
    var correction_strength: Double
    if ISO_exposure_time[0] > 0.0 {
        correction_strength = ISO_exposure_time.reduce(0, +)
        
        correction_strength = ( // TODO: This needs an explanation
            min(80,
                max(5.0,
                    correction_strength/sqrt(Double(textures.count)) * (noise_reduction==23.0 ? 0.25 : 1.00)
                )
            ) - 5.0
        ) / 75.0
    } else {
        correction_strength = 1.0
    }
    
    // only apply hot pixel correction if correction strength is larger than 0.001
    if correction_strength > 0.001 {
    
        // generate simple average of all textures
        let average_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: textures[0].width, height: textures[0].height, mipmapped: false)
        average_texture_descriptor.usage = [.shaderRead, .shaderWrite]
        average_texture_descriptor.storageMode = .private
        let average_texture = device.makeTexture(descriptor: average_texture_descriptor)!
        average_texture.label = "Average of all texture"
        fill_with_zeros(average_texture)
        
        // iterate over all images
        for comp_idx in 0..<textures.count {
            add_texture(textures[comp_idx], average_texture, textures.count)
        }
        
        // calculate mean value specific for each color channel
        let mean_texture_buffer = texture_mean(average_texture,
                                               per_sub_pixel: true,
                                               mosaic_pattern_width: mosaic_pattern_width)
        
        // standard parameters if black level is not available / available
        let hot_pixel_multiplicator = (black_level[0][0] == -1) ? 2.0 : 1.0
        var hot_pixel_threshold     = (black_level[0][0] == -1) ? 1.0 : 2.0
        // X-Trans sensor has more spacing between nearest pixels of same color, need a more relaxed threshold.
        if mosaic_pattern_width == 6 {
            hot_pixel_threshold *= 1.4
        }
        
        // Calculate mean black level for each color channel
        var black_levels_mean = Array(repeating: Float32(0), count: mosaic_pattern_width*mosaic_pattern_width)
        if black_level[0][0] != 1 {
            for channel_idx in 0..<mosaic_pattern_width*mosaic_pattern_width {
                for img_idx in 0..<textures.count {
                    black_levels_mean[channel_idx] += Float32(black_level[img_idx][channel_idx])
                }
                black_levels_mean[channel_idx] /= Float32(textures.count)
            }
        }
        let black_levels_buffer = device.makeBuffer(bytes: black_levels_mean, length: MemoryLayout<Float32>.size * black_levels_mean.count)!
             
        let command_buffer = command_queue.makeCommandBuffer()!
        command_buffer.label = "Finding hotpixels"
        let command_encoder = command_buffer.makeComputeCommandEncoder()!
        let state: MTLComputePipelineState
        switch mosaic_pattern_width {
            case 2:
                state = find_hotpixels_bayer_state
            case 6:
                state = find_hotpixels_xtrans_state
            default:
                return
        }
        command_encoder.setComputePipelineState(state)
        // -4 in width and height represent that hotpixel correction is not applied on a 2-pixel wide border around the image.
        // This is done so that the algorithm is simpler and comparing neighbours don't have to handle the edge cases.
        let threads_per_grid = MTLSize(width: average_texture.width-4, height: average_texture.height-4, depth: 1)
        let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
        command_encoder.setTexture(average_texture, index: 0)
        command_encoder.setTexture(hotpixel_weight_texture, index: 1)
        command_encoder.setBuffer(mean_texture_buffer, offset: 0, index: 0)
        command_encoder.setBuffer(black_levels_buffer, offset: 0, index: 1)
        command_encoder.setBytes([Float32(hot_pixel_threshold)],     length: MemoryLayout<Float32>.stride, index: 2)
        command_encoder.setBytes([Float32(hot_pixel_multiplicator)], length: MemoryLayout<Float32>.stride, index: 3)
        command_encoder.setBytes([Float32(correction_strength)],     length: MemoryLayout<Float32>.stride, index: 4)
        command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
        command_encoder.endEncoding()
        command_buffer.commit()
    }
}


func get_threads_per_thread_group(_ state: MTLComputePipelineState, _ threads_per_grid: MTLSize) -> MTLSize {
    var thread_execution_width = state.threadExecutionWidth
    if threads_per_grid.depth >= thread_execution_width {
        return MTLSize(width: 1, height: 1, depth: thread_execution_width)
    } else {
        thread_execution_width /= threads_per_grid.depth
        // set initial values that always work, but may not be optimal
        var best_dim_x = 1
        var best_dim_y = thread_execution_width
        let best_dim_z = threads_per_grid.depth
         
        if threads_per_grid.height <= thread_execution_width {
            thread_execution_width /= threads_per_grid.height
            best_dim_x = thread_execution_width
            best_dim_y = threads_per_grid.height
        }
         
        thread_execution_width = state.threadExecutionWidth        
        var best_runs = Int(1e12)
        // perform additional optimization for 2D grids and try to find a pattern that has the lowest possible overhead (ideally thread grid is exactly a multiple of grid specified by grid_x and grid_y)
        // the divisor is varied from 2 to thread_execution_width/2 and for each combination the total number of runs is calculated
        // the combination with the lowest number of runs is selected, which in addition has a ratio of dim_x/dim_y that is similar to the ratio of the thread grid (e.g. for a thread grid with a ratio of 3:2, dim_x = 8 and dim_y = 4 may be selected assuming thread_execution_width = 32
        for divisor in 1..<thread_execution_width/4+1 {
            let dim_x = thread_execution_width/(2*divisor)
            let dim_y = 2*divisor
             
            if dim_x*dim_y == thread_execution_width && dim_x<=threads_per_grid.width && dim_y<=threads_per_grid.height {
                let runs = Int(ceil(Double(threads_per_grid.width)/Double(dim_x))*ceil(Double(threads_per_grid.height)/Double(dim_y))+0.1)
                 
                if runs < best_runs || (runs==best_runs && dim_x>=dim_y) {
                    best_runs = runs
                    best_dim_x = dim_x
                    best_dim_y = dim_y
                }
            }
        }
        
        return MTLSize(width: best_dim_x, height: best_dim_y, depth: best_dim_z)
    }
}


func normalize_texture(_ in_texture: MTLTexture, _ norm_texture: MTLTexture, _ norm_scalar: Int) {
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Normalize Texture"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = normalize_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(norm_texture, index: 1)
    command_encoder.setBytes([Float32(norm_scalar)], length: MemoryLayout<Float32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}

/// This function is intended to convert the source input texture from integer to 32 bit float while correcting hot pixels, equalizing exposure and extending the texture to the size needed for alignment
/// Images with different exposures are all still mapped to the same bit range by the camera. This means that the raw pixel value is not directly comparable between images with different exposures and must be transformed before they can be merged.
///
/// For example, if the reference image is taken at 0 EV and has a pixel value of 45 and image 2 is taken at 2 EV and has a pixel value of 200, the two values represent vastly different things. The pixel value of image 2 must be decreased by 2^-2 (200 x 2^-2 = 50) in order for the pixel values to be comparable.
///
/// Inspired by https://ai.googleblog.com/2021/04/hdr-with-bracketing-on-pixel-phones.html
func prepare_texture(_ in_texture: MTLTexture, _ hotpixel_weight_texture: MTLTexture, _ pad_left: Int, _ pad_right: Int, _ pad_top: Int, _ pad_bottom: Int, _ exposure_diff: Int, _ black_level: [Int], _ mosaic_pattern_width: Int) -> MTLTexture {

    // always use pixel format float32 with increased precision that merging is performed with best possible precision    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: in_texture.width+pad_left+pad_right, height: in_texture.height+pad_top+pad_bottom, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_descriptor.storageMode = .private
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    out_texture.label = "\(in_texture.label!.components(separatedBy: ":")[0]): Prepared"
    
    fill_with_zeros(out_texture)
    
    let black_levels_buffer = device.makeBuffer(bytes: black_level.map{ $0 == -1 ? Float32(0) : Float32($0)},
                                                length: black_level.count * MemoryLayout<Float32>.size)!
        
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Prepare Texture"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = prepare_texture_bayer_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(hotpixel_weight_texture, index: 1)
    command_encoder.setTexture(out_texture, index: 2)
    command_encoder.setBuffer(black_levels_buffer, offset: 0, index: 0)
    command_encoder.setBytes([Int32(pad_left)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(pad_top)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(exposure_diff)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


/// Create and return a new texture that has the same properties as the one passed in.
func texture_like(_ in_texture: MTLTexture) -> MTLTexture {
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: in_texture.pixelFormat, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    out_texture_descriptor.storageMode = .private
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    out_texture.label = in_texture.label
    
    return out_texture
}


/// Function to calculate the average pixel value over the whole of a texture.
/// If `per_sub_pixel` is `true`, then each subpixel in the mosaic pattern will have an independent average calculated, producing a `pattern_width * pattern_width` buffer.
/// If it's `false`, then a single value will be calculated for the whole texture.
func texture_mean(_ in_texture: MTLTexture, per_sub_pixel: Bool, mosaic_pattern_width: Int) -> MTLBuffer {
    
    // Create output texture from the y-axis blurring
    let texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: in_texture.width, height: mosaic_pattern_width, mipmapped: false)
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    texture_descriptor.storageMode = .private
    let summed_y = device.makeTexture(descriptor: texture_descriptor)!

    // Sum each subpixel of the mosaic vertically along columns, creating a (width, mosaic_pattern_width) sized image
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    command_buffer.label = "Mean for \(String(describing: in_texture.label))\(per_sub_pixel ? " per_sub_pixel" : "")"
    command_encoder.setComputePipelineState(sum_rect_columns_float_state)
    let thread_per_grid = MTLSize(width: summed_y.width, height: summed_y.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(sum_rect_columns_float_state, thread_per_grid)
    
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(summed_y, index: 1)
    command_encoder.setBytes([0], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([0], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([in_texture.height], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(mosaic_pattern_width)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(thread_per_grid, threadsPerThreadgroup: threads_per_thread_group)

    // Sum along the row
    // If `per_sub_pixel` is true, then the result is per sub pixel, otherwise a single value is calculated
    let sum_buffer = device.makeBuffer(length: (mosaic_pattern_width*mosaic_pattern_width)*MemoryLayout<Float32>.size,
                                       options: .storageModeShared)!
    command_encoder.setComputePipelineState(sum_row_state)
    let threads_per_grid_x = MTLSize(width: mosaic_pattern_width, height: mosaic_pattern_width, depth: 1)
    let threads_per_thread_group_x = get_threads_per_thread_group(sum_row_state, threads_per_grid_x)
    
    command_encoder.setTexture(summed_y, index: 0)
    command_encoder.setBuffer(sum_buffer, offset: 0, index: 0)
    command_encoder.setBytes([Int32(summed_y.width)],       length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(mosaic_pattern_width)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.dispatchThreads(threads_per_grid_x, threadsPerThreadgroup: threads_per_thread_group_x)
    
    // Calculate the average from the sum
    let state       = per_sub_pixel ? divide_buffer_state                       : sum_divide_buffer_state
    let buffer_size = per_sub_pixel ? mosaic_pattern_width*mosaic_pattern_width : 1
    let avg_buffer  = device.makeBuffer(length: buffer_size*MemoryLayout<Float32>.size, options: .storageModeShared)!
    command_encoder.setComputePipelineState(state)
    // If doing per-subpixel, the total number of pixels of each subpixel is 1/(mosaic_pattern_withh)^2 times the total
    let num_pixels_per_value = Float(in_texture.width * in_texture.height) / (per_sub_pixel ? Float(mosaic_pattern_width*mosaic_pattern_width) : 1.0)
    let threads_per_grid_divisor = MTLSize(width: buffer_size, height: 1, depth: 1)
    let threads_per_thread_group_divisor = get_threads_per_thread_group(state, threads_per_grid_divisor)
    
    command_encoder.setBuffer(sum_buffer,                                   offset: 0,                            index: 0)
    command_encoder.setBuffer(avg_buffer,                                   offset: 0,                            index: 1)
    command_encoder.setBytes([num_pixels_per_value],                        length: MemoryLayout<Float32>.stride, index: 2)
    command_encoder.setBytes([mosaic_pattern_width*mosaic_pattern_width],   length: MemoryLayout<Int>.stride,     index: 3)
    command_encoder.dispatchThreads(threads_per_grid_divisor, threadsPerThreadgroup: threads_per_thread_group_divisor)
    
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return avg_buffer
}


/// Upsample the provided texture to the specified widths using either a nearest neighbour approach or using bilinear interpolation.
func upsample(_ input_texture: MTLTexture, to_width width: Int, to_height height: Int, using mode: UpsampleType) -> MTLTexture {
    let scale_x = Double(width)  / Double(input_texture.width)
    let scale_y = Double(height) / Double(input_texture.height)
    
    // create output texture
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: width, height: height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    output_texture_descriptor.storageMode = .private
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    output_texture.label = input_texture.label
    
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Upsample"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = (mode == .Bilinear ? upsample_bilinear_float_state : upsample_nearest_int_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(input_texture,  index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    command_encoder.setBytes([Float32(scale_x)], length: MemoryLayout<Float32>.stride, index: 0)
    command_encoder.setBytes([Float32(scale_y)], length: MemoryLayout<Float32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return output_texture
}
