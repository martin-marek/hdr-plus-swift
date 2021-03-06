import Foundation
import MetalKit
import MetalPerformanceShaders


// possible error types during the alignment
enum AlignmentError: Error {
    case less_than_two_images
    case inconsistent_extensions
    case inconsistent_resolutions
}


// all the relevant information about image tiles in a single struct
struct TileInfo {
    var tile_size: Int
    var search_dist: Int
    var n_tiles_x: Int
    var n_tiles_y: Int
    var n_pos_1d: Int
    var n_pos_2d: Int
}


// class to store the progress of the align+merge
class ProcessingProgress: ObservableObject {
    @Published var int = 0
}


// set up Metal device
let device = MTLCreateSystemDefaultDevice()!
let command_queue = device.makeCommandQueue()!
let metal_function_library = device.makeDefaultLibrary()!


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

func upsample(_ input_texture: MTLTexture, width: Int, height: Int, mode: String, type: String) -> MTLTexture {
    // Metal has a built-in function for texture resizing but it doesn't support uint pixels
    // hence I wrote this function, which support *only* uint pixels
    
    // convert args
    let scale_x = Double(width) / Double(input_texture.width)
    let scale_y = Double(height) / Double(input_texture.height)
    
    // create output texture
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: width, height: height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    
    // run the metal kernel
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    var mtl_func_name: String = ""
    if mode == "nearest" && type == "int" {mtl_func_name = "upsample_nearest_int"}
    if mode == "nearest" && type == "float" {mtl_func_name = "upsample_nearest_float"}
    if mode == "bilinear" && type == "int" {mtl_func_name = "upsample_bilinear_int"}
    let mtl_func = metal_function_library.makeFunction(name: mtl_func_name)!
    let state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(input_texture, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    let params_array = [Float32(scale_x), Float32(scale_y)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Float32>.size, options: .storageModeShared)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    
    return output_texture
}


func avg_pool(_ input_texture: MTLTexture, _ scale: Int) -> MTLTexture {
    // Metal has a built-in function for texture resizing but it doesn't support uint pixels
    // hence I wrote this function, which support *only* uint pixels
    
    // create output texture
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: input_texture.width/2, height: input_texture.height/2, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    
    // run the metal kernel
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "avg_pool")!
    let state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(input_texture, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    
    // if the input 'scale' was 2, we are done
    // otherwise, we need to keep average pooling the texture
    if scale == 2 {
        return output_texture
    } else {
        return avg_pool(output_texture, scale / 2)
    }
}


func average_texture_sums(_ input_texture: MTLTexture, _ n: Int) -> MTLTexture {
    
    // create output texture
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Uint, width: input_texture.width, height: input_texture.height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    print(output_texture_descriptor.storageMode)
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    
    // run the metal kernel
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "average_texture_sums")!
    let state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(input_texture, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    let params_array = [Int32(n)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Float32>.size, options: .storageModeShared)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()

    return output_texture
}


func build_pyramid(_ input_texture: MTLTexture, _ downscale_factor_list: Array<Int>) -> Array<MTLTexture> {
    
    // iteratively resize the current layer in the pyramid
    var pyramid: Array<MTLTexture> = []
    for (i, downscale_factor) in downscale_factor_list.enumerated() {
        if i == 0 {
            pyramid.append(avg_pool(input_texture, downscale_factor))
        } else {
            pyramid.append(avg_pool(pyramid.last!, downscale_factor))
        }
        // print("layer.shape: ", pyramid.last!.width, pyramid.last!.height)
    }
    return pyramid
}


func texture_like(_ input_texture: MTLTexture) -> MTLTexture {
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: input_texture.width, height: input_texture.height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    return output_texture
}


func compute_tile_diff(_ ref_layer: MTLTexture, _ comp_layer: MTLTexture, _ prev_alignment: MTLTexture, _ downscale_factor: Int, _ tile_info: TileInfo) -> MTLTexture {
    
    // create a 'tile difference' texture
    let texture_descriptor = MTLTextureDescriptor()
    texture_descriptor.textureType = .type3D
    texture_descriptor.pixelFormat = .r32Uint
    texture_descriptor.width = tile_info.n_tiles_x
    texture_descriptor.height = tile_info.n_tiles_y
    texture_descriptor.depth = tile_info.n_pos_2d
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let tile_diff = device.makeTexture(descriptor: texture_descriptor)!
    
    // compute tile differences
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let compute_tile_differences = metal_function_library.makeFunction(name: "compute_tile_differences")!
    let mtl_state = try! device.makeComputePipelineState(function: compute_tile_differences)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: tile_info.n_pos_2d)
    let threads_per_thread_group = get_threads_per_thread_group(mtl_state, threads_per_grid)
    let params_array = [Int32(downscale_factor), Int32(tile_info.tile_size), Int32(tile_info.search_dist), Int32(tile_info.n_tiles_x), Int32(tile_info.n_tiles_y)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Int32>.size, options: .storageModeShared)
    command_encoder.setTexture(ref_layer, index: 0)
    command_encoder.setTexture(comp_layer, index: 1)
    command_encoder.setTexture(prev_alignment, index: 2)
    command_encoder.setTexture(tile_diff, index: 3)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    
    return tile_diff
}


func compute_tile_alignment(_ tile_diff: MTLTexture, _ prev_alignment: MTLTexture, _ current_alignment: MTLTexture, _ downscale_factor: Int, _ tile_info: TileInfo) {
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let compute_tile_alignments = metal_function_library.makeFunction(name: "compute_tile_alignments")!
    let mtl_state = try! device.makeComputePipelineState(function: compute_tile_alignments)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(mtl_state, threads_per_grid)
    let params_array = [Int32(downscale_factor), Int32(tile_info.search_dist)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Int32>.size, options: .storageModeShared)
    command_encoder.setTexture(tile_diff, index: 0)
    command_encoder.setTexture(prev_alignment, index: 1)
    command_encoder.setTexture(current_alignment, index: 2)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
}


func warp_texture(_ texture_to_warp: MTLTexture, _ alignment: MTLTexture, _ tile_info: TileInfo, _ downscale_factor: Int) -> MTLTexture {
    
    // create the output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture_to_warp.pixelFormat, width: texture_to_warp.width, height: texture_to_warp.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let warped_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // warp the texture
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "warp_texture")!
    let mtl_state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: texture_to_warp.width, height: texture_to_warp.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(mtl_state, threads_per_grid)
    let params_array = [Int32(downscale_factor), Int32(tile_info.tile_size), Int32(tile_info.n_tiles_x), Int32(tile_info.n_tiles_y)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Int32>.size, options: .storageModeShared)
    command_encoder.setTexture(texture_to_warp, index: 0)
    command_encoder.setTexture(warped_texture, index: 1)
    command_encoder.setTexture(alignment, index: 2)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    
    return warped_texture
}


func add_textures(_ texture1: MTLTexture, _ texture2: MTLTexture, pixel_format: MTLPixelFormat = .r32Uint) -> MTLTexture {
    
    // create the output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: pixel_format, width: texture1.width, height: texture1.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "add_textures")!
    let state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: texture1.width, height: texture1.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(texture1, index: 0)
    command_encoder.setTexture(texture2, index: 1)
    command_encoder.setTexture(out_texture, index: 2)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    
    return out_texture
}


func add_textures_weighted(_ texture1: MTLTexture, _ texture2: MTLTexture, _ weight_texture: MTLTexture) -> MTLTexture {
    
    // create the output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture1.pixelFormat, width: texture1.width, height: texture1.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "add_textures_weighted")!
    let state = try! device.makeComputePipelineState(function: mtl_func)
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
    // command_buffer.waitUntilCompleted()
    
    return out_texture
}



func blur_bayer_texture(_ in_texture: MTLTexture, _ kernel_size: Int) -> MTLTexture {
    
    // create a temp texture blurred along x-axis only and the output texture, blurred along both x- and y-axis
    let blur_x = texture_like(in_texture)
    let blur_xy = texture_like(in_texture)
    
    // blur the texture along the x-axis
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "blur_bayer_x")!
    let mtl_state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(mtl_state, threads_per_grid)
    let params_array = [Int32(kernel_size)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Int32>.size, options: .storageModeShared)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(blur_x, index: 1)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()

    // blur the texture along the y-axis
    let command_buffer2 = command_queue.makeCommandBuffer()!
    let command_encoder2 = command_buffer2.makeComputeCommandEncoder()!
    let mtl_func2 = metal_function_library.makeFunction(name: "blur_bayer_y")!
    let mtl_state2 = try! device.makeComputePipelineState(function: mtl_func2)
    command_encoder2.setComputePipelineState(mtl_state2)
    command_encoder2.setTexture(blur_x, index: 0)
    command_encoder2.setTexture(blur_xy, index: 1)
    command_encoder2.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder2.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder2.endEncoding()
    command_buffer2.commit()
    command_buffer2.waitUntilCompleted()
    
    return blur_xy
}


func color_difference(_ texture1: MTLTexture, _ texture2: MTLTexture) -> MTLTexture {
    
    // create output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture1.pixelFormat, width: texture1.width/2, height: texture1.height/2, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // compute pixel pairwise differences
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "color_difference")!
    let mtl_state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: texture1.width/2, height: texture1.height/2, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(mtl_state, threads_per_grid)
    command_encoder.setTexture(texture1, index: 0)
    command_encoder.setTexture(texture2, index: 1)
    command_encoder.setTexture(output_texture, index: 2)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    
    return output_texture
}


func texture_mean(_ in_texture: MTLTexture) -> Float {
    
    // create a 1d texture that will contain the averages of the input texture along the x-axis
    let texture_descriptor = MTLTextureDescriptor()
    texture_descriptor.textureType = .type1D
    texture_descriptor.pixelFormat = in_texture.pixelFormat
    texture_descriptor.width = in_texture.width
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let avg_y = device.makeTexture(descriptor: texture_descriptor)!
    
    // average the input texture along the x-axis
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "average_y")!
    let state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: 1, depth: 1)
    let max_threads_per_thread_group = state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(avg_y, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    
    // sum up the generated 1d texture along the y-axis
    var sum: UInt32 = 0
    let command_buffer2 = command_queue.makeCommandBuffer()!
    let command_encoder2 = command_buffer2.makeComputeCommandEncoder()!
    let mtl_func2 = metal_function_library.makeFunction(name: "sum_1d")!
    let state2 = try! device.makeComputePipelineState(function: mtl_func2)
    command_encoder2.setComputePipelineState(state2)
    let params_buffer = device.makeBuffer(bytes: &sum, length: MemoryLayout<UInt32>.size, options: .storageModeShared)
    command_encoder2.setTexture(avg_y, index: 0)
    command_encoder2.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder2.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder2.endEncoding()
    command_buffer2.commit()
    command_buffer2.waitUntilCompleted()
    
    // copy data from MTLBuffer back to the local variable 'sum'
    let nsData = NSData(bytesNoCopy: params_buffer!.contents(), length: params_buffer!.length, freeWhenDone: false)
    nsData.getBytes(&sum, length:params_buffer!.length)
    
    // average the sum along y-axis
    let avg: Float = Float(sum) / Float(in_texture.width)
    
    // return the average of all pixels in the input array
    return avg
}


func estimate_color_noise(_ texure: MTLTexture, _ texture_blurred: MTLTexture) -> Float {
    
    // compute the color difference of each rggb superpixel between the original and the blurred texture
    let texture_diff = color_difference(texure, texture_blurred)
    
    // compute the average of the difference between the original and the blurred texture
    let mean_diff = texture_mean(texture_diff)
    
    return mean_diff
}


func robust_merge(_ ref_texture: MTLTexture, _ ref_texture_blurred: MTLTexture, _ comp_texture: MTLTexture, _ kernel_size: Int, _ robustness: Double, _ noise_sd: Float) -> MTLTexture {
    
    // if robustness is 0, return simply the comparison texture
    if robustness == 0 {return comp_texture}
    
    // blur comp texure
    let comp_texture_blurred = blur_bayer_texture(comp_texture, kernel_size)
    
    // compute the color difference of each rggb superpixel between the blurred reference and the comparison textures
    let texture_diff = color_difference(ref_texture_blurred, comp_texture_blurred)
    
    // create a weight texture
    let weight_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Float, width: ref_texture.width/2, height: ref_texture.height/2, mipmapped: false)
    weight_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let weight_texture = device.makeTexture(descriptor: weight_texture_descriptor)!
    
    // compute merge weight
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "compute_merge_weight")!
    let mtl_state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: ref_texture.width/2, height: ref_texture.height/2, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(mtl_state, threads_per_grid)
    let params_array = [Float32(noise_sd), Float32(robustness)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Float32>.size, options: .storageModeShared)
    command_encoder.setTexture(texture_diff, index: 0)
    command_encoder.setTexture(weight_texture, index: 1)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    
    // upsample merge weight to full image resolution
    let weight_texture_upsampled = upsample(weight_texture, width: ref_texture.width, height: ref_texture.height, mode: "nearest", type: "float")
    
    // average the input textures based on the weight
    let output_texture = add_textures_weighted(ref_texture, comp_texture, weight_texture_upsampled)
    
    return output_texture
}


func fill_with_zeros(_ texture: MTLTexture) {
    // a new MTLTexture *should* be initialized with zeros...
    // but testing build 1.0.5 on Majka's macbook revelead that this
    // may fail when repeatedly drag-and-dropping the same burst
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "fill_with_zeros")!
    let mtl_state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: texture.width, height: texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(mtl_state, threads_per_grid)
    command_encoder.setTexture(texture, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
}


func load_images(_ urls: [URL], _ progress: ProcessingProgress) throws -> [MTLTexture] {
    
    var textures_dict: [Int: MTLTexture] = [:]
    let compute_group = DispatchGroup()
    let compute_queue = DispatchQueue.global() // this is a concurrent queue to do compute
    let access_queue = DispatchQueue(label: "") // this is a serial queue to read/save data thread-safely

    for i in 0..<urls.count {
        compute_queue.async(group: compute_group) {
    
            // asynchronously load texture
            if let texture = try? image_url_to_bayer_texture(urls[i], device) {
    
                // sync GUI progress
                DispatchQueue.main.async { progress.int += 1 }
    
                // thread-safely save the texture
                access_queue.sync { textures_dict[i] = texture }
            }
        }
    }
    
    // wait until all the images are loaded
    compute_group.wait()
    
    // convert dict to list
    var textures_list: [MTLTexture] = []
    for i in 0..<urls.count {
        
        // ensure thread-safety
        try access_queue.sync {
            
            // check whether the images have been loaded successfully
            if let texture = textures_dict[i] {
                textures_list.append(texture)
            } else {
                throw ImageIOError.load_error
            }
        }
    }
    
    return textures_list
}


func align_and_merge(image_urls: [URL], progress: ProcessingProgress, ref_idx: Int = 0, search_distance: String = "Medium", tile_size: Int = 16, kernel_size: Int = 5, robustness: Double = 1) throws -> MTLTexture {
    // DEBUGGING
    // check that 2+ images have been passed
    if image_urls.count < 2 {
        throw AlignmentError.less_than_two_images
    }
    
    // check that all images are of the same extension
    let ref_ext = image_urls[0].pathExtension
    for i in 1..<image_urls.count {
        let comp_ext = image_urls[i].pathExtension
        if comp_ext != ref_ext {
            throw AlignmentError.inconsistent_extensions
        }
    }
    
    // load images
    var t = DispatchTime.now().uptimeNanoseconds
    let textures = try load_images(image_urls, progress)
    let ref_texture = textures[ref_idx]
    print("Time to load all images: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    let t0 = DispatchTime.now().uptimeNanoseconds
    
    // set the maximum resolution of the smallest pyramid layer
    let search_distance_dict = [
        "Low": 128,
        "Medium": 64,
        "High": 32,
    ]
    let max_min_layer_res = search_distance_dict[search_distance]!
    
    // set alignment params
    let min_image_dim = min(ref_texture.width, ref_texture.height)
    var downscale_factor_array = [2]
    var search_dist_array = [2]
    var tile_size_array = [tile_size]
    var res = min_image_dim / downscale_factor_array[0]
    while (res > max_min_layer_res) {
        downscale_factor_array.append(2)
        search_dist_array.append(2)
        tile_size_array.append(max(tile_size_array.last!/2, 8))
        res /= 2
    }
    
    // build reference pyramid
    let ref_pyramid = build_pyramid(ref_texture, downscale_factor_array)
    
    // blur reference texure and estimate noise standard deviation
    // -  the computation is done here to avoid repeating the same computation in 'robust_merge()'
    let ref_texture_blurred = blur_bayer_texture(ref_texture, kernel_size)
    let noise_sd = estimate_color_noise(ref_texture, ref_texture_blurred)
    
    // create an empty output texture
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Uint, width: ref_texture.width, height: ref_texture.height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    var output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    fill_with_zeros(output_texture)

    // iterate over comparison images
    for comp_idx in 0..<image_urls.count {
        // add the reference texture to the output
        if comp_idx == ref_idx {
            output_texture = add_textures(ref_texture, output_texture)
            DispatchQueue.main.async { progress.int += 1 }
            continue
        }
        
        // check that the comparison image has the same resolution as the reference image
        if (ref_texture.width != textures[comp_idx].width) || (ref_texture.height != textures[comp_idx].height) {
            throw AlignmentError.inconsistent_resolutions
        }
        
        // build comparison pyramid
        let comp_pyramid = build_pyramid(textures[comp_idx], downscale_factor_array)

        // initiazite tile alignments
        let alignment_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rg16Sint, width: 1, height: 1, mipmapped: false)
        alignment_descriptor.usage = [.shaderRead, .shaderWrite]
        var prev_alignment = device.makeTexture(descriptor: alignment_descriptor)!
        var current_alignment = device.makeTexture(descriptor: alignment_descriptor)!
        var tile_info = TileInfo(tile_size: 0, search_dist: 0, n_tiles_x: 0, n_tiles_y: 0, n_pos_1d: 0, n_pos_2d: 0)

        // align tiles
        for i in (0 ... downscale_factor_array.count-1).reversed() {
            t = DispatchTime.now().uptimeNanoseconds
            
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
            
            // store tile info in a single dict
            tile_info = TileInfo(tile_size: tile_size, search_dist: search_dist, n_tiles_x: n_tiles_x, n_tiles_y: n_tiles_y, n_pos_1d: n_pos_1d, n_pos_2d: n_pos_2d)
            
            // resize previous alignment
            // - 'downscale_factor' has to be loaded from the *previous* layer since that is the layer that generated the current layer
            var downscale_factor: Int
            if (i < downscale_factor_array.count-1){
                downscale_factor = downscale_factor_array[i+1]
            } else {
                downscale_factor = 0
            }
            prev_alignment = upsample(current_alignment, width: n_tiles_x, height: n_tiles_y, mode: "bilinear", type: "int")
            current_alignment = texture_like(prev_alignment)
            
            // compute tile differences
            let tile_diff = compute_tile_diff(ref_layer, comp_layer, prev_alignment, downscale_factor, tile_info)
            
            // compute tile alignment based on tile differnces
            compute_tile_alignment(tile_diff, prev_alignment, current_alignment, downscale_factor, tile_info)
            
        }

        // warp the aligned layer
        tile_info.tile_size *= downscale_factor_array[0]
        let aligned_texture = warp_texture(textures[comp_idx], current_alignment, tile_info, downscale_factor_array[0])
        
        // robust-merge the texture
        let merged_texture = robust_merge(ref_texture, ref_texture_blurred, aligned_texture, kernel_size, robustness, noise_sd)
        
        // add robust-merged texture to the output image
        output_texture = add_textures(merged_texture, output_texture)
        
        // sync GUI progress
        DispatchQueue.main.async { progress.int += 1 }
    }
    
    // rescale output texture
    let output_texture_uint16 = average_texture_sums(output_texture, image_urls.count)
    
    print("Time to align+merge all images: ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
    
    return output_texture_uint16
}
