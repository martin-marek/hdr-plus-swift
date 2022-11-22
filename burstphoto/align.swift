import Foundation
import MetalKit
import MetalPerformanceShaders


// possible error types during the alignment
enum AlignmentError: Error {
    case less_than_two_images
    case inconsistent_extensions
    case inconsistent_resolutions
    case unsupported_mosaic_pattern
    case conversion_failed
    case missing_dng_converter
}


// all the relevant information about image tiles in a single struct
struct TileInfo {
    var tile_size: Int
    var tile_size_merge: Int
    var search_dist: Int
    var n_tiles_x: Int
    var n_tiles_y: Int
    var n_pos_1d: Int
    var n_pos_2d: Int
}


// class to store the progress of the align+merge
class ProcessingProgress: ObservableObject {
    @Published var int = 0
    @Published var includes_conversion = false
}


// set up Metal device
let device = MTLCreateSystemDefaultDevice()!
let command_queue = device.makeCommandQueue()!
let mfl = device.makeDefaultLibrary()!


// compile metal functions / pipelines
let fill_with_zeros_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "fill_with_zeros")!)
let texture_uint16_to_float_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "texture_uint16_to_float")!)
let upsample_nearest_int_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "upsample_nearest_int")!)
let upsample_bilinear_float_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "upsample_bilinear_float")!)
let avg_pool_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "avg_pool")!)
let compute_tile_differences_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_tile_differences")!)
let compute_tile_alignments_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_tile_alignments")!)
let warp_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "warp_texture")!)
let forward_dft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "forward_dft")!)
let backward_dft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "backward_dft")!)
let merge_frequency_domain_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "merge_frequency_domain")!)
let extend_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "extend_texture")!)
let crop_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "crop_texture")!)
let convert_uint16_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_uint16")!)
let add_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture")!)
let copy_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "copy_texture")!)
let convert_RGBA_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_RGBA")!)
let convert_2x2_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_2x2")!)
let fill_with_zeros_RGBA_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "fill_with_zeros_RGBA")!)
let calculate_rms_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_rms")!)
let calculate_diff_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_diff")!)



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


func fill_with_zeros_RGBA(_ texture: MTLTexture) {
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = fill_with_zeros_RGBA_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: texture.width, height: texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(texture, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func texture_uint16_to_float(_ in_texture: MTLTexture) -> MTLTexture {
    // create out texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Float, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // run kernel
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = texture_uint16_to_float_state
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


func upsample(_ input_texture: MTLTexture, width: Int, height: Int, mode: String) -> MTLTexture {
    
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
    let state = mode == "bilinear" ? upsample_bilinear_float_state : upsample_nearest_int_state
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


func avg_pool(_ input_texture: MTLTexture, _ scale: Int) -> MTLTexture {

    // create output texture
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: input_texture.width/scale, height: input_texture.height/scale, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    
    // run the metal kernel
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = avg_pool_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: output_texture.width, height: output_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(input_texture, index: 0)
    command_encoder.setTexture(output_texture, index: 1)
    command_encoder.setBytes([Int32(scale)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()

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
    texture_descriptor.pixelFormat = .r32Float
    texture_descriptor.width = tile_info.n_tiles_x
    texture_descriptor.height = tile_info.n_tiles_y
    texture_descriptor.depth = tile_info.n_pos_2d
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let tile_diff = device.makeTexture(descriptor: texture_descriptor)!
    
    // compute tile differences
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = compute_tile_differences_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: tile_info.n_pos_2d)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(ref_layer, index: 0)
    command_encoder.setTexture(comp_layer, index: 1)
    command_encoder.setTexture(prev_alignment, index: 2)
    command_encoder.setTexture(tile_diff, index: 3)
    command_encoder.setBytes([Int32(downscale_factor)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(tile_info.tile_size)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(tile_info.search_dist)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(tile_info.n_tiles_x)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.setBytes([Int32(tile_info.n_tiles_y)], length: MemoryLayout<Int32>.stride, index: 4)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return tile_diff
}


func compute_tile_alignment(_ tile_diff: MTLTexture, _ prev_alignment: MTLTexture, _ current_alignment: MTLTexture, _ downscale_factor: Int, _ tile_info: TileInfo) {
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = compute_tile_alignments_state
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
    
    // create the output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture_to_warp.pixelFormat, width: texture_to_warp.width, height: texture_to_warp.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let warped_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // warp the texture
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = warp_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: texture_to_warp.width, height: texture_to_warp.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(texture_to_warp, index: 0)
    command_encoder.setTexture(warped_texture, index: 1)
    command_encoder.setTexture(alignment, index: 2)
    command_encoder.setBytes([Int32(downscale_factor)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(tile_info.tile_size)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(tile_info.n_tiles_x)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(tile_info.n_tiles_y)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return warped_texture
}


func perform_denoising(image_urls: [URL], progress: ProcessingProgress, ref_idx: Int = 0, search_distance: String = "Medium", tile_size: Int = 16, kernel_size: Int = 5, robustness: Double = 1) throws -> URL {
    
    // check that all images are of the same extension
    let image_extension = image_urls[0].pathExtension
    let all_extensions_same = image_urls.allSatisfy{$0.pathExtension == image_extension}
    if !all_extensions_same {throw AlignmentError.inconsistent_extensions}
    
    // check that 2+ images were provided
    let n_images = image_urls.count
    if n_images < 2 {throw AlignmentError.less_than_two_images}
    
    // create output directory
    let out_dir = NSHomeDirectory() + "/Pictures/Burst Photo/"
    if !FileManager.default.fileExists(atPath: out_dir) {
        try FileManager.default.createDirectory(atPath: out_dir, withIntermediateDirectories: true, attributes: nil)
    }
    
    // create a directory for temporary dngs inside the output directory
    let tmp_dir = out_dir + ".dngs/"
    try FileManager.default.createDirectory(atPath: tmp_dir, withIntermediateDirectories: true)
    
    // measure execution time
    var t = DispatchTime.now().uptimeNanoseconds
    
    // ensure that all files are .dng, converting them if necessary
    var dng_urls = image_urls
    let convert_to_dng = image_extension != "dng"
    
    if convert_to_dng {
        // check if dng converter is installed
        let dng_converter_path = "/Applications/Adobe DNG Converter.app"
        if !FileManager.default.fileExists(atPath: dng_converter_path) {
            // if dng coverter is not installed, prompt user
            throw AlignmentError.missing_dng_converter
        } else {
            // the dng converter is installed -> use it
            dng_urls = try convert_images_to_dng(image_urls, dng_converter_path, tmp_dir)
            print("Time to convert images: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
            DispatchQueue.main.async { progress.int += n_images }
            t = DispatchTime.now().uptimeNanoseconds
        }
    }
    
    // set output location
    let in_url = dng_urls[ref_idx]
    let in_filename = in_url.deletingPathExtension().lastPathComponent
    let out_filename = in_filename + "_merged"
    let out_path = out_dir + out_filename + ".dng"
    let out_url = URL(fileURLWithPath: out_path)
    
    // load images
    t = DispatchTime.now().uptimeNanoseconds
    var (textures, mosaic_pettern_width) = try load_images(image_urls, progress)
    print("Time to load all images: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    let t0 = DispatchTime.now().uptimeNanoseconds
        
    // convert images from uint16 to float16
    textures = textures.map{texture_uint16_to_float($0)}
     
    // use a 32 bit float as final image
    let final_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: textures[ref_idx].width, height: textures[ref_idx].height, mipmapped: false)
    final_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let final_texture = device.makeTexture(descriptor: final_texture_descriptor)!
    fill_with_zeros(final_texture)
    
    // special mode: simple temporal averaging without alignment and robust merging
    if robustness == 0.0 {
        print("Special mode: temporal averaging only...")
          
        // iterate over comparison images
        for comp_idx in 0..<image_urls.count {
            
            add_texture(textures[comp_idx], final_texture, image_urls.count)
        }
        
        let output_texture_uint16 = convert_uint16(final_texture)
        
        print("Time to align+merge all images: ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
        
        // save the output image
        try texture_to_dng(output_texture_uint16, in_url, out_url)
            
        // delete the temporary dng directory
        try FileManager.default.removeItem(atPath: tmp_dir)
            
        return out_url
     
    // sophisticated approach with alignment of tiles and merging of tiles in the spatial domain (when pattern is not 2x2 Bayer)
    } else if mosaic_pettern_width != 2 {
        print("Merging in the spatial domain...")
        
        //let robustness_norm = 0.08838835*pow(sqrt(2), robustness*10) // use this value as a replacement of robustness for control of motion robustness / noise level
        
        throw AlignmentError.unsupported_mosaic_pattern
        
        
    // sophisticated approach with alignment of tiles and merging of tiles in frequency domain (only 2x2 Bayer pattern)
    } else {
        print("Merging in the frequency domain...")
        
        // tile size for merging in frequency domain is always 16x16
        let tile_size_merge = Int(16)
               
        // perform align and merge 4 times in a row with slight displacement of the frame to prevent artifacts in the merging process
        try align_merge_frequency_domain(progress: progress, shift_left: tile_size_merge, shift_right: 0, shift_top: tile_size_merge, shift_bottom: 0, ref_idx: ref_idx, mosaic_pettern_width: mosaic_pettern_width, search_distance: search_distance, tile_size: tile_size, tile_size_merge: tile_size_merge, robustness: robustness, textures: textures, final_texture: final_texture)
        
        try align_merge_frequency_domain(progress: progress, shift_left: 0, shift_right: tile_size_merge, shift_top: tile_size_merge, shift_bottom: 0, ref_idx: ref_idx, mosaic_pettern_width: mosaic_pettern_width, search_distance: search_distance, tile_size: tile_size, tile_size_merge: tile_size_merge, robustness: robustness, textures: textures, final_texture: final_texture)
         
        try align_merge_frequency_domain(progress: progress, shift_left: tile_size_merge, shift_right: 0, shift_top: 0, shift_bottom: tile_size_merge,ref_idx: ref_idx, mosaic_pettern_width: mosaic_pettern_width, search_distance: search_distance, tile_size: tile_size, tile_size_merge: tile_size_merge, robustness: robustness, textures: textures, final_texture: final_texture)
            
        try align_merge_frequency_domain(progress: progress, shift_left: 0, shift_right: tile_size_merge, shift_top: 0, shift_bottom: tile_size_merge, ref_idx: ref_idx, mosaic_pettern_width: mosaic_pettern_width, search_distance: search_distance, tile_size: tile_size, tile_size_merge: tile_size_merge, robustness: robustness, textures: textures, final_texture: final_texture)
                
        let output_texture_uint16 = convert_uint16(final_texture)
          
        print("Time to align+merge all images: ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
        
        // save the output image
        try texture_to_dng(output_texture_uint16, in_url, out_url)
            
        // delete the temporary dng directory
        try FileManager.default.removeItem(atPath: tmp_dir)
        
        return out_url
    }
}


func align_merge_frequency_domain(progress: ProcessingProgress, shift_left: Int, shift_right: Int, shift_top: Int, shift_bottom: Int, ref_idx: Int, mosaic_pettern_width: Int, search_distance: String, tile_size: Int, tile_size_merge: Int, robustness: Double, textures: [MTLTexture], final_texture: MTLTexture) throws {
    
    // set original texture size
    let texture_width_orig = textures[ref_idx].width
    let texture_height_orig = textures[ref_idx].height
            
    // set the maximum resolution of the smallest pyramid layer
    let search_distance_dict = [
        "Low": 128,
        "Medium": 64,
        "High": 32,
    ]
    let max_min_layer_res = search_distance_dict[search_distance]!
              
    // set alignment params
    let min_image_dim = min(texture_width_orig, texture_height_orig)
    var downscale_factor_array = [mosaic_pettern_width]
    var search_dist_array = [2]
    var tile_size_array = [tile_size]
    var res = min_image_dim / downscale_factor_array[0]
    var div = 2
    while (res > max_min_layer_res) {
        downscale_factor_array.append(2)
        search_dist_array.append(2)
        tile_size_array.append(max(tile_size_array.last!/2, 8))
        res /= 2
        div *= 2
    }
     
    // calculate shifts for extension of the image frame with zeros in such a way that each resolution level of alignment exactly fits within the frame and nearest neighbor upsampling of motion vector fields is sufficient
    let tile_factor = div*Int(tile_size_array.last!)
    
    var shift_align_x = Int(ceil(Float(texture_width_orig+tile_size_merge)/Float(tile_factor)))
    shift_align_x = (shift_align_x*Int(tile_factor) - texture_width_orig - tile_size_merge)/2
    
    var shift_align_y = Int(ceil(Float(texture_height_orig+tile_size_merge)/Float(tile_factor)))
    shift_align_y = (shift_align_y*Int(tile_factor) - texture_height_orig - tile_size_merge)/2
  
    // add padding for artifact suppression
    let pad_left   = shift_align_x + shift_left
    let pad_right  = shift_align_x + shift_right
    let pad_top    = shift_align_y + shift_top
    let pad_bottom = shift_align_y + shift_bottom

    // calculate shifts for smaller frame that merging in the frequency domain is only applied to the actual image
    var crop_merge_x = Int(floor(Float(shift_align_x)/Float(2*tile_size_merge)))
    crop_merge_x = crop_merge_x*2*tile_size_merge
    var crop_merge_y = Int(floor(Float(shift_align_y)/Float(2*tile_size_merge)))
    crop_merge_y = crop_merge_y*2*tile_size_merge

         
    // set reference texture
    let ref_texture = extend_texture(textures[ref_idx], pad_left, pad_right, pad_top, pad_bottom)
        
    // build reference pyramid
    let ref_pyramid = build_pyramid(ref_texture, downscale_factor_array)
    
    // initialize textures to store real and imaginary parts of the reference texture, a temp texture for the Fourier transform and the final texture
    let ref_texture_fft_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: (texture_width_orig+pad_left+pad_right-2*crop_merge_x), height: (texture_height_orig+pad_top+pad_bottom-2*crop_merge_y)/2, mipmapped: false)
    ref_texture_fft_descriptor.usage = [.shaderRead, .shaderWrite]
    let ref_texture_fft = device.makeTexture(descriptor: ref_texture_fft_descriptor)!
    
    let tmp_texture_fft_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: (texture_width_orig+pad_left+pad_right-2*crop_merge_x), height: (texture_height_orig+pad_top+pad_bottom-2*crop_merge_y)/2, mipmapped: false)
    tmp_texture_fft_descriptor.usage = [.shaderRead, .shaderWrite]
    let tmp_texture_fft = device.makeTexture(descriptor: tmp_texture_fft_descriptor)!
           
    // transform ref_texture to frequency domain
    let tile_info_merge = TileInfo(tile_size: tile_size, tile_size_merge: tile_size_merge, search_dist: 0, n_tiles_x: (texture_width_orig+pad_left+pad_right-2*crop_merge_x)/tile_size_merge, n_tiles_y: (texture_height_orig+pad_top+pad_bottom-2*crop_merge_y)/tile_size_merge, n_pos_1d: 0, n_pos_2d: 0)
    
    // estimate noise level of tiles
    let ref_texture_rgba = convert_RGBA(ref_texture, crop_merge_x, crop_merge_y)
    let rms_texture = calculate_rms(ref_texture_rgba, tile_info_merge)

    // transform reference texture into the Fourier space
    // to improve performance, convert textures into RGBA pixel format that SIMD instructions can be applied and crop image frame that unneccessary zeros are removed at the borders
    forward_dft(convert_RGBA(ref_texture, crop_merge_x, crop_merge_y), ref_texture_fft, tmp_texture_fft, mosaic_pettern_width, tile_info_merge)

    let final_texture_fft = copy_texture(ref_texture_fft)
    
    // iterate over comparison images
    for comp_idx in 0..<textures.count {
  
        let t0 = DispatchTime.now().uptimeNanoseconds
        
        if comp_idx == ref_idx {
            continue
        }
         
        let comp_texture = extend_texture(textures[comp_idx], pad_left, pad_right, pad_top, pad_bottom)
        
        // check that the comparison image has the same resolution as the reference image
        if (ref_texture.width != comp_texture.width) || (ref_texture.height != comp_texture.height) {
            throw AlignmentError.inconsistent_resolutions
        }
        
        // build comparison pyramid
        let comp_pyramid = build_pyramid(comp_texture, downscale_factor_array)
              
        // initiazite tile alignments
        let alignment_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rg16Sint, width: 1, height: 1, mipmapped: false)
        alignment_descriptor.usage = [.shaderRead, .shaderWrite]
        var prev_alignment = device.makeTexture(descriptor: alignment_descriptor)!
        var current_alignment = device.makeTexture(descriptor: alignment_descriptor)!
        var tile_info = TileInfo(tile_size: 0, tile_size_merge: 0, search_dist: 0, n_tiles_x: 0, n_tiles_y: 0, n_pos_1d: 0, n_pos_2d: 0)

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
            tile_info = TileInfo(tile_size: tile_size, tile_size_merge: 0, search_dist: search_dist, n_tiles_x: n_tiles_x, n_tiles_y: n_tiles_y, n_pos_1d: n_pos_1d, n_pos_2d: n_pos_2d)
            
            // resize previous alignment
            // - 'downscale_factor' has to be loaded from the *previous* layer since that is the layer that generated the current layer
            var downscale_factor: Int
            if (i < downscale_factor_array.count-1){
                downscale_factor = downscale_factor_array[i+1]
            } else {
                downscale_factor = 0
            }
          
            prev_alignment = upsample(current_alignment, width: n_tiles_x, height: n_tiles_y, mode: "nearest")
            current_alignment = texture_like(prev_alignment)
            
            // compute tile differences
            let tile_diff = compute_tile_diff(ref_layer, comp_layer, prev_alignment, downscale_factor, tile_info)
            
            // compute tile alignment based on tile differnces
            compute_tile_alignment(tile_diff, prev_alignment, current_alignment, downscale_factor, tile_info)
        }
        
        // warp the aligned layer
        tile_info.tile_size *= downscale_factor_array[0]
        let aligned_texture = warp_texture(comp_texture, current_alignment, tile_info, downscale_factor_array[0])
   
        // start debug capture
        //let capture_manager = MTLCaptureManager.shared()
        //let capture_descriptor = MTLCaptureDescriptor()
        //capture_descriptor.captureObject = device
        //try! capture_manager.startCapture(with: capture_descriptor)
        
        // merge aligned comparison texture with reference texture in the frequency domain
        // to improve performance, convert textures into RGBA pixel format that SIMD instructions can be applied and crop image frame that unneccessary zeros are removed at the borders
        merge_frequency_domain(convert_RGBA(aligned_texture, crop_merge_x, crop_merge_y), ref_texture_rgba, ref_texture_fft, tmp_texture_fft, final_texture_fft, rms_texture, robustness, mosaic_pettern_width, tile_info_merge);
               
        // stop debug capture
        //capture_manager.stopCapture()
      
        print("Align+merge: ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
        DispatchQueue.main.async { progress.int += 1 }
    }    

    // transform output texture back to image space and convert back to the 2x2 pixel structure
    let output_texture = crop_texture(convert_2x2(backward_dft(final_texture_fft, tmp_texture_fft, mosaic_pettern_width, tile_info_merge, textures.count)), pad_left-crop_merge_x, pad_right-crop_merge_x, pad_top-crop_merge_y, pad_bottom-crop_merge_y)

    // add output texture to the final texture to collect all textures of the four iterations
    add_texture(output_texture, final_texture, 1)
}


func forward_dft(_ in_texture: MTLTexture, _ out_texture_fft: MTLTexture, _ tmp_texture_fft: MTLTexture, _ mosaic_pettern_width: Int, _ tile_info: TileInfo) {
  
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = forward_dft_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(tmp_texture_fft, index: 1)
    command_encoder.setTexture(out_texture_fft, index: 2)
    command_encoder.setBytes([Int32(mosaic_pettern_width)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func backward_dft(_ in_texture_fft: MTLTexture, _ tmp_texture_fft: MTLTexture, _ mosaic_pettern_width: Int, _ tile_info: TileInfo, _ n_textures: Int) -> MTLTexture {
    
    // create output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba16Float, width: in_texture_fft.width/2, height: in_texture_fft.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = backward_dft_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture_fft, index: 0)
    command_encoder.setTexture(tmp_texture_fft, index: 1)
    command_encoder.setTexture(out_texture, index: 2)
    command_encoder.setBytes([Int32(mosaic_pettern_width)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(n_textures)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return out_texture
}


func merge_frequency_domain(_ aligned_texture: MTLTexture, _ ref_texture: MTLTexture, _ ref_texture_fft: MTLTexture, _ tmp_texture_fft: MTLTexture, _ out_texture_fft: MTLTexture, _ rms_texture: MTLTexture, _ robustness: Double, _ mosaic_pettern_width: Int, _ tile_info: TileInfo) {
  
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = merge_frequency_domain_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(aligned_texture, index: 0)
    command_encoder.setTexture(ref_texture, index: 1)
    command_encoder.setTexture(ref_texture_fft, index: 2)
    command_encoder.setTexture(tmp_texture_fft, index: 3)
    command_encoder.setTexture(out_texture_fft, index: 4)
    command_encoder.setTexture(rms_texture, index: 5)
    command_encoder.setBytes([Float32(robustness)], length: MemoryLayout<Float32>.stride, index: 0)
    command_encoder.setBytes([Int32(mosaic_pettern_width)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(tile_info.tile_size)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()   
}


func extend_texture(_ in_texture: MTLTexture, _ pad_left: Int, _ pad_right: Int, _ pad_top: Int, _ pad_bottom: Int) -> MTLTexture {
    
    // create output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Float, width: in_texture.width+pad_left+pad_right, height: in_texture.height+pad_top+pad_bottom, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    fill_with_zeros(out_texture)
        
    // run the metal kernel
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


func crop_texture(_ in_texture: MTLTexture, _ pad_left: Int, _ pad_right: Int, _ pad_top: Int, _ pad_bottom: Int) -> MTLTexture {
    
    // create output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Float, width: in_texture.width-pad_left-pad_right, height: in_texture.height-pad_top-pad_bottom, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // run the metal kernel
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


func convert_uint16(_ in_texture: MTLTexture) -> MTLTexture {
    
    // create output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Uint, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // run the metal kernel
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_uint16_state
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


func add_texture(_ in_texture: MTLTexture, _ out_texture: MTLTexture, _ n_textures: Int) {
    
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = add_texture_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.setBytes([Int32(n_textures)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
}


func copy_texture(_ in_texture: MTLTexture) -> MTLTexture {
    
    // create output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: in_texture.pixelFormat, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // copy texture
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
    
    return(out_texture)
}


func calculate_rms(_ in_texture: MTLTexture, _ tile_info: TileInfo) -> MTLTexture {
    
    // create output texture
    let rms_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba32Float, width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, mipmapped: false)
    rms_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let rms_texture = device.makeTexture(descriptor: rms_texture_descriptor)!
    
    // copy texture
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = calculate_rms_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: rms_texture.width, height: rms_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(rms_texture, index: 1)
    command_encoder.setBytes([Int32(tile_info.tile_size_merge)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return(rms_texture)
}


func calculate_diff(_ ref_texture: MTLTexture, _ aligned_texture: MTLTexture) -> MTLTexture {
    
    // create output texture
    let diff_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: ref_texture.width, height: ref_texture.height, mipmapped: false)
    diff_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let diff_texture = device.makeTexture(descriptor: diff_texture_descriptor)!
    
    // copy texture
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = calculate_diff_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: diff_texture.width, height: diff_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(ref_texture, index: 0)
    command_encoder.setTexture(aligned_texture, index: 1)
    command_encoder.setTexture(diff_texture, index: 2)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return(diff_texture)
}


func convert_RGBA(_ in_texture: MTLTexture, _ crop_merge_x: Int, _ crop_merge_y: Int) -> MTLTexture {
    
    // create output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba16Float, width: (in_texture.width-2*crop_merge_x)/2, height: (in_texture.height-2*crop_merge_y)/2, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
        
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_RGBA_state
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
    
    return(out_texture)
}


func convert_2x2(_ in_texture: MTLTexture) -> MTLTexture {
    
    // create output texture
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Float, width: in_texture.width*2, height: in_texture.height*2, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
        
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_2x2_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(out_texture, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return(out_texture)
}


