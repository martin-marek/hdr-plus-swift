import Foundation
import MetalKit
import MetalPerformanceShaders


// possible error types during the alignment
enum AlignmentError: Error {
    case less_than_two_images
    case inconsistent_extensions
    case unsupported_image_type
    case inconsistent_resolutions
}

// set up Metal device
let device = MTLCreateSystemDefaultDevice()!
let command_queue = device.makeCommandQueue()!
let metal_function_library = device.makeDefaultLibrary()!


struct TileInfo {
    let tile_size: Int
    let search_dist: Int
    let n_tiles_x: Int
    let n_tiles_y: Int
    let n_pos_1d: Int
    let n_pos_2d: Int
}


func resize_texture(_ input_texture: MTLTexture, scale: Double = 0, width: Int = 0, height: Int = 0, signed: Bool) -> MTLTexture {
    // Metal has a built-in function for texture resizing but it desn't support uint pixels
    // hence I had to write this function, which support *only* uint pixels
    
    // convert args
    var out_width = 0
    var out_height = 0
    var scale_x = Double(0.0)
    var scale_y = Double(0.0)
    if scale == 0 {
        scale_x = Double(width) / Double(input_texture.width)
        scale_y = Double(height) / Double(input_texture.height)
        out_width = width
        out_height = height
    } else {
        scale_x = scale
        scale_y = scale
        out_width = Int(Double(input_texture.width) * scale_x)
        out_height = Int(Double(input_texture.height) * scale_y)
    }
    
    // create output texture
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: out_width, height: out_height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    
    // run the metal kernel
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let mtl_func: MTLFunction
    if signed {
        mtl_func = metal_function_library.makeFunction(name: "resize_nearest_int")!
    } else {
        mtl_func = metal_function_library.makeFunction(name: "resize_nearest_uint")!
    }
    let state = try! device.makeComputePipelineState(function: mtl_func)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let max_threads_per_thread_group = state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    command_encoder.setTexture(input_texture, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    let params_array = [Float32(scale)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Float32>.size, options: .storageModeShared)
    command_encoder.setBuffer(params_buffer, offset: 0, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    
    return output_texture
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
    let params_array = [Float32(n)]
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
            pyramid.append(resize_texture(input_texture, scale: 1/Double(downscale_factor), signed: false))
        } else {
            pyramid.append(resize_texture(pyramid.last!, scale: 1/Double(downscale_factor), signed: false))
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
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let max_threads_per_thread_group = compute_tile_alignments_state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
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
    // print("time to compute tile alignment: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
}


func warp_texture(_ texture_to_warp: MTLTexture, _ alignment: MTLTexture, _ downscale_factor: Int) -> MTLTexture {
    // let t = DispatchTime.now().uptimeNanoseconds
    
    // create the output texture
    let warped_texture = texture_like(texture_to_warp)
    
    // warp the texture
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let warp_texture = metal_function_library.makeFunction(name: "warp_texture")!
    let warp_texture_state = try! device.makeComputePipelineState(function: warp_texture)
    command_encoder.setComputePipelineState(warp_texture_state)
    let threads_per_grid = MTLSize(width: texture_to_warp.width, height: texture_to_warp.height, depth: 1)
    let max_threads_per_thread_group = warp_texture_state.maxTotalThreadsPerThreadgroup
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    let params_array = [Int32(downscale_factor)]
    let params_buffer = device.makeBuffer(bytes: params_array, length: params_array.count * MemoryLayout<Int32>.size, options: .storageModeShared)
    command_encoder.setTexture(texture_to_warp, index: 0)
    command_encoder.setTexture(alignment, index: 1)
    command_encoder.setTexture(warped_texture, index: 2)
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


func align_and_merge(image_urls: [URL], progress: ProcessingProgress, ref_idx: Int = 0, search_distance: String = "Medium", tile_size: Int = 32) throws -> MTLTexture {
    // check that 2+ images have been passed
    // DEBUG: this check is disabled
    // if image_urls.count < 2 {
    //     throw AlignmentError.less_than_two_images
    // }
    
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
    default:
        downscale_factor_array = [2, 2, 2, 4]
    }
    var tile_size_array: [Int]
    switch tile_size {
    case 16:
        tile_size_array = [16, 16, 16, 16]
    case 32:
        tile_size_array = [32, 16, 16, 16]
    case 64:
        tile_size_array = [64, 32, 16, 16]
    default:
        tile_size_array = [32, 16, 16, 16]
    }
    let search_dist_array = [1, 4, 4, 4]
    
    // load reference image
    // print("loading", image_dir+image_names[ref_idx])
    
    guard let ref_texture = image_url_to_bayer_texture(image_urls[ref_idx], device) else {
    // guard let ref_texture = image_url_to_rgb_texture(image_urls[ref_idx], device, command_queue) else {
        throw AlignmentError.unsupported_image_type
    }
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
            let tile_info = TileInfo(tile_size: tile_size, search_dist: search_dist, n_tiles_x: n_tiles_x, n_tiles_y: n_tiles_y, n_pos_1d: n_pos_1d, n_pos_2d: n_pos_2d)
            
            // resize previous alignment
            // - 'downscale_factor' has to be loaded from the *previous* layer since that is the layer that generated the current layer
            var downscale_factor: Int
            if (i < downscale_factor_array.count-1){
                downscale_factor = downscale_factor_array[i+1]
            } else {
                downscale_factor = 0
            }
            prev_alignment = resize_texture(current_alignment, width: n_tiles_x, height: n_tiles_y, signed: true)
            current_alignment = texture_like(prev_alignment)
            
            // compute tile differences
            let tile_diff = compute_tile_diff(ref_layer, comp_layer, prev_alignment, downscale_factor, tile_info)
            
            // compute tile alignment based on tile differnces
            compute_tile_alignment(tile_diff, prev_alignment, current_alignment, downscale_factor, tile_info)
        }

        // resize alignment to image shape
        current_alignment = resize_texture(current_alignment, width: ref_texture.width, height: ref_texture.height, signed: true)

        // warp the aligned layer
        let aligned_texture = warp_texture(comp_texture, current_alignment, downscale_factor_array[0])
        
        // add aligned texture to the output image
        add_textures(aligned_texture, output_texture)
        
        // set progress info to the GUI
        DispatchQueue.main.async {
            progress.int += 1
        }
    }
    
    // rescale output texture
    let output_texture_uint16 = average_texture_sums(output_texture, image_urls.count)
    
    // ensure that all metal processing is finished
    // TODO: is there a cleaner way to do this?
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.commit()
    command_buffer.waitUntilCompleted()
    
    return output_texture_uint16
}

