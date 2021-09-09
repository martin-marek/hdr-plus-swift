import Foundation
import MetalKit
import MetalPerformanceShaders


// possible error types during the alignment
enum AlignmentError: Error {
    case less_than_two_images
    case inconsistent_extensions
    case unsupported_image_type
    case inconsistent_resolutions
    case search_distance_too_large
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


func upsample_nearest(_ input_texture: MTLTexture, width: Int, height: Int) -> MTLTexture {
    // Metal has a built-in function for texture resizing but it desn't support uint pixels
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
    let mtl_func = metal_function_library.makeFunction(name: "upsample_nearest")!
    let state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let max_threads_per_thread_group = state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    command_encoder.setTexture(input_texture, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    let params_array = [Float32(scale_x), Float32(scale_y)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Float32>.size, options: .storageModeShared)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return output_texture
}


func avg_pool(_ input_texture: MTLTexture, _ scale: Int) -> MTLTexture {
    // Metal has a built-in function for texture resizing but it desn't support uint pixels
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
    let max_threads_per_thread_group = state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    command_encoder.setTexture(input_texture, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
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
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    
    // run the metal kernel
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "average_texture_sums")!
    let state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let max_threads_per_thread_group = state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
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
    // let t = DispatchTime.now().uptimeNanoseconds
    let texture_descriptor = MTLTextureDescriptor()
    texture_descriptor.textureType = .type3D
    texture_descriptor.pixelFormat = .r32Uint
    texture_descriptor.width = tile_info.n_tiles_x
    texture_descriptor.height = tile_info.n_tiles_y
    texture_descriptor.depth = tile_info.n_pos_2d
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let tile_diff = device.makeTexture(descriptor: texture_descriptor)!
    
    // compute tile differences
    // let t = DispatchTime.now().uptimeNanoseconds
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let compute_tile_differences = metal_function_library.makeFunction(name: "compute_tile_differences")!
    let compute_tile_differences_state = try! device.makeComputePipelineState(function: compute_tile_differences)
    command_encoder.setComputePipelineState(compute_tile_differences_state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: tile_info.n_pos_2d)
    let max_threads_per_thread_group = compute_tile_differences_state.maxTotalThreadsPerThreadgroup
    let s = Int(floor((Float(max_threads_per_thread_group) / Float(tile_info.n_pos_2d)).squareRoot()))
    let threads_per_thread_group = MTLSize(width: s, height: s, depth: tile_info.n_pos_2d)
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
    // print("time to compute tile difference: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    
    return tile_diff
}


func compute_tile_alignment(_ tile_diff: MTLTexture, _ prev_alignment: MTLTexture, _ current_alignment: MTLTexture, _ downscale_factor: Int, _ tile_info: TileInfo) {

    // let t = DispatchTime.now().uptimeNanoseconds
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let compute_tile_alignments = metal_function_library.makeFunction(name: "compute_tile_alignments")!
    let compute_tile_alignments_state = try! device.makeComputePipelineState(function: compute_tile_alignments)
    command_encoder.setComputePipelineState(compute_tile_alignments_state)
    let threads_per_grid2 = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let max_threads_per_thread_group = compute_tile_alignments_state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    let params_array = [Int32(downscale_factor), Int32(tile_info.search_dist)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Int32>.size, options: .storageModeShared)
    command_encoder.setTexture(tile_diff, index: 0)
    command_encoder.setTexture(prev_alignment, index: 1)
    command_encoder.setTexture(current_alignment, index: 2)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid2, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    // print("time to compute tile alignment: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
}


func warp_texture(_ texture_to_warp: MTLTexture, _ alignment: MTLTexture, _ tile_info: TileInfo, _ downscale_factor: Int) -> MTLTexture {
    // let t = DispatchTime.now().uptimeNanoseconds
    
    // create the output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Uint, width: texture_to_warp.width, height: texture_to_warp.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let warped_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // warp the texture
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "warp_texture")!
    let mtl_state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: texture_to_warp.width, height: texture_to_warp.height, depth: 1)
    let max_threads_per_thread_group = mtl_state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
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
    // print("time to warp the aligned layer: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    
    return warped_texture
}


func add_textures(_ texture_to_add: MTLTexture, _ output_texture: MTLTexture) {
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "add_texture")!
    let state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let max_threads_per_thread_group = state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    command_encoder.setTexture(texture_to_add, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func blur_texture(_ in_texture: MTLTexture, _ kernel_size: Int) -> MTLTexture {
    
    // create a temp texture blurred along x-axis only and the output texture, blurred along both x- and y-axis
    let blur_x = texture_like(in_texture)
    let blur_xy = texture_like(in_texture)
    
    // blur the texture along the x-axis
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "blur_x")!
    let mtl_state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let max_threads_per_thread_group = mtl_state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    let params_array = [Int32(kernel_size)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Int32>.size, options: .storageModeShared)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(blur_x, index: 1)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()

    // blur the texture along the y-axis
    let command_buffer2 = command_queue.makeCommandBuffer()!
    let command_encoder2 = command_buffer2.makeComputeCommandEncoder()!
    let mtl_func2 = metal_function_library.makeFunction(name: "blur_y")!
    let mtl_state2 = try! device.makeComputePipelineState(function: mtl_func2)
    command_encoder2.setComputePipelineState(mtl_state2)
    command_encoder2.setTexture(blur_x, index: 0)
    command_encoder2.setTexture(blur_xy, index: 1)
    command_encoder2.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder2.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder2.endEncoding()
    command_buffer2.commit()
    
    return blur_xy
}


func absolute_difference(_ texture1: MTLTexture, _ texture2: MTLTexture) -> MTLTexture {
    
    // create output texture
    let output_texture = texture_like(texture1)
    
    // compute pixel pairwise differences
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "absolute_difference")!
    let mtl_state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: texture1.width, height: texture1.height, depth: 1)
    let max_threads_per_thread_group = mtl_state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    command_encoder.setTexture(texture1, index: 0)
    command_encoder.setTexture(texture2, index: 1)
    command_encoder.setTexture(output_texture, index: 2)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
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


func estimate_image_noise(_ in_texure: MTLTexture, _ kernel_size: Int) -> Float {
    // blur the input texture
    let texture_blurred = blur_texture(in_texure, kernel_size)
    
    // compute the absolute difference between the original and the blurred texture
    let texture_diff = absolute_difference(in_texure, texture_blurred)
    
    // compute the average of the difference between the original and the blurred texture
    let mean_diff = texture_mean(texture_diff)
    
    return mean_diff
}


func robust_merge(_ ref_texture: MTLTexture, _ comp_texture: MTLTexture, _ kernel_size: Int, _ robustness: Double) -> MTLTexture {
    // create output texture
    let output_texture = texture_like(ref_texture)
    
    // blur ref and comp texures
    let ref_texture_blurred = blur_texture(ref_texture, kernel_size)
    let comp_texture_blurred = blur_texture(comp_texture, kernel_size)
    
    // estimate noise standard deviation
    let noise_sd = estimate_image_noise(ref_texture, kernel_size)
    
    // robust merge
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func = metal_function_library.makeFunction(name: "robust_merge")!
    let mtl_state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(mtl_state)
    let threads_per_grid = MTLSize(width: ref_texture.width, height: ref_texture.height, depth: 1)
    let max_threads_per_thread_group = mtl_state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    let params_array = [Float32(noise_sd), Float32(robustness)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Float32>.size, options: .storageModeShared)
    command_encoder.setTexture(ref_texture, index: 0)
    command_encoder.setTexture(comp_texture, index: 1)
    command_encoder.setTexture(ref_texture_blurred, index: 2)
    command_encoder.setTexture(comp_texture_blurred, index: 3)
    command_encoder.setTexture(output_texture, index: 4)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return output_texture
}


func align_and_merge(image_urls: [URL], progress: ProcessingProgress, ref_idx: Int = 0, search_distance: String = "Medium", tile_size: Int = 32, kernel_size: Int = 10, robustness: Double = 1) throws -> MTLTexture {
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
    
    // set alignment params
    var downscale_factor_array: [Int]
    switch search_distance {
    case "Low":
        downscale_factor_array = [2, 2, 2, 2]
    case "Medium":
        downscale_factor_array = [2, 2, 2, 4]
    case "High":
        downscale_factor_array = [2, 2, 4, 4]
    case "Highest":
        downscale_factor_array = [2, 4, 4, 4]
    default:
        downscale_factor_array = []
    }
    var tile_size_array: [Int]
    switch tile_size {
    case 8:
        tile_size_array = [8, 8, 8, 8]
    case 16:
        tile_size_array = [16, 16, 16, 16]
    case 32:
        tile_size_array = [32, 16, 16, 16]
    case 64:
        tile_size_array = [64, 32, 16, 16]
    default:
        tile_size_array = []
    }
    let search_dist_array = [1, 4, 4, 4]
    
    // load reference image
    // print("loading", image_dir+image_names[ref_idx])
    guard let ref_texture = image_url_to_bayer_texture(image_urls[ref_idx], device) else {
    // guard let ref_texture = image_url_to_rgb_texture(image_urls[ref_idx], device, command_queue) else {
        throw AlignmentError.unsupported_image_type
    }
    
    // check image resolution
    let min_image_dim = min(ref_texture.width, ref_texture.height)
    var total_downscale_factor = 1
    for d in downscale_factor_array {
        total_downscale_factor *= d
    }
    let min_image_resolution = total_downscale_factor * tile_size_array[0]
    if min_image_dim < min_image_resolution {
        throw AlignmentError.search_distance_too_large
    }
    
    // build reference pyramid
    let ref_pyramid = build_pyramid(ref_texture, downscale_factor_array)
    
    // create an empty output texture
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Uint, width: ref_texture.width, height: ref_texture.height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!

    // iterate over comparison images
    for comp_idx in 0..<image_urls.count {
        // add the reference texture to the output
        if comp_idx == ref_idx {
            add_textures(ref_texture, output_texture)
            continue
        }
        
        // DEBUGGING
        // print("comp_idx: ", comp_idx)
        
        // load comparison image
        guard let comp_texture = image_url_to_bayer_texture(image_urls[comp_idx], device) else {
        // guard let comp_texture = image_url_to_rgb_texture(image_urls[comp_idx], device, command_queue) else {
            throw AlignmentError.unsupported_image_type
        }
        
        // check that the comparison image has the same resolution as the reference image
        if !(ref_texture.width == comp_texture.width) && (ref_texture.height == comp_texture.height) {
            throw AlignmentError.inconsistent_resolutions
        }
        
        let comp_pyramid = build_pyramid(comp_texture, downscale_factor_array)

        // initiazite tile alignments
        let alignment_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rg16Sint, width: 1, height: 1, mipmapped: false)
        alignment_descriptor.usage = [.shaderRead, .shaderWrite]
        var prev_alignment = device.makeTexture(descriptor: alignment_descriptor)!
        var current_alignment = device.makeTexture(descriptor: alignment_descriptor)!
        var tile_info = TileInfo(tile_size: 0, search_dist: 0, n_tiles_x: 0, n_tiles_y: 0, n_pos_1d: 0, n_pos_2d: 0)

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
            prev_alignment = upsample_nearest(current_alignment, width: n_tiles_x, height: n_tiles_y)
            current_alignment = texture_like(prev_alignment)
            
            // compute tile differences
            let tile_diff = compute_tile_diff(ref_layer, comp_layer, prev_alignment, downscale_factor, tile_info)
            
            // compute tile alignment based on tile differnces
            compute_tile_alignment(tile_diff, prev_alignment, current_alignment, downscale_factor, tile_info)
        }

        // warp the aligned layer
        tile_info.tile_size *= downscale_factor_array[0]
        let aligned_texture = warp_texture(comp_texture, current_alignment, tile_info, downscale_factor_array[0])
        
        // robust-merge the texture
        let merged_texture = robust_merge(ref_texture, aligned_texture, kernel_size, robustness)
        
        // add robust-merged texture to the output image
        add_textures(merged_texture, output_texture)
        
        // set progress info to the GUI
        DispatchQueue.main.async {
            progress.int += 1
        }
    }
    
    // rescale output texture
    let output_texture_uint16 = average_texture_sums(output_texture, image_urls.count)
    
    // average texture
    print("orig avg: ", texture_mean(ref_texture))
    print("blur avg: ", texture_mean(blur_texture(ref_texture, 5)))
    print("noise: ", estimate_image_noise(ref_texture, 5))
    
    // ensure that all metal processing is finished
    // TODO: is there a cleaner way to do this?
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.commit()
    command_buffer.waitUntilCompleted()
    
    return output_texture_uint16
}
