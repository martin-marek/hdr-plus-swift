import Foundation
import SwiftUI
import MetalKit
import MetalPerformanceShaders


// possible error types during the alignment
enum AlignmentError: Error {
    case less_than_two_images
    case inconsistent_extensions
    case inconsistent_resolutions
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
    @Published var show_nonbayer_hq_alert = false
}

// set up Metal device
let device = MTLCreateSystemDefaultDevice()!
let command_queue = device.makeCommandQueue()!
let mfl = device.makeDefaultLibrary()!


// compile metal functions / pipelines
let fill_with_zeros_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "fill_with_zeros")!)
let texture_uint16_to_float_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "texture_uint16_to_float")!)
let convert_uint16_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_uint16")!)
let upsample_nearest_int_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "upsample_nearest_int")!)
let upsample_bilinear_float_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "upsample_bilinear_float")!)
let avg_pool_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "avg_pool")!)
let blur_mosaic_x_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "blur_mosaic_x")!)
let blur_mosaic_y_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "blur_mosaic_y")!)
let extend_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "extend_texture")!)
let crop_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "crop_texture")!)
let add_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_texture")!)
let copy_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "copy_texture")!)
let convert_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_rgba")!)
let convert_bayer_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "convert_bayer")!)
let average_x_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "average_x")!)
let average_y_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "average_y")!)
let average_x_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "average_x_rgba")!)
let average_y_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "average_y_rgba")!)
let compute_tile_differences_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_tile_differences")!)
let compute_tile_differences25_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_tile_differences25")!)
let compute_tile_alignments_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_tile_alignments")!)
let correct_upsampling_error_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "correct_upsampling_error")!)
let warp_texture_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "warp_texture")!)
let add_textures_weighted_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "add_textures_weighted")!)
let color_difference_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "color_difference")!)
let compute_merge_weight_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "compute_merge_weight")!)
let calculate_rms_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_rms_rgba")!)
let forward_dft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "forward_dft")!)
let forward_fft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "forward_fft")!)
let backward_dft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "backward_dft")!)
let backward_fft_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "backward_fft")!)
let merge_frequency_domain_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "merge_frequency_domain")!)
let calculate_mismatch_rgba_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "calculate_mismatch_rgba")!)
let normalize_mismatch_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "normalize_mismatch")!)
let correct_hotpixels_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "correct_hotpixels")!)
let equalize_exposure_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "equalize_exposure")!)
let correct_underexposure_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "correct_underexposure")!)
let deconvolute_frequency_domain_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "deconvolute_frequency_domain")!)
let reduce_artifacts_tile_border_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "reduce_artifacts_tile_border")!)



// ===========================================================================================================
// Main functions
// ===========================================================================================================


// main function of the burst photo app
func perform_denoising(image_urls: [URL], progress: ProcessingProgress, merging_algorithm: String = "Fast", tile_size: String = "Medium", search_distance: String = "Medium", kernel_size: Int = 5, noise_reduction: Double = 13.0, comp_underexposure: String = "Correction off") throws -> URL {
    
    // measure execution time
    let t0 = DispatchTime.now().uptimeNanoseconds
    var t = t0
    
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
       
    // ensure that all files are .dng, converting them if necessary
    var dng_urls = image_urls
    let convert_to_dng = image_extension.lowercased() != "dng"
    DispatchQueue.main.async { progress.includes_conversion = convert_to_dng }
    let dng_converter_path = "/Applications/Adobe DNG Converter.app"
    let final_dng_conversion = FileManager.default.fileExists(atPath: dng_converter_path)
    
    if convert_to_dng {
        // check if dng converter is installed
        if !FileManager.default.fileExists(atPath: dng_converter_path) {
            // if dng coverter is not installed, prompt user
            throw AlignmentError.missing_dng_converter
        } else {
            // the dng converter is installed -> use it
            dng_urls = try convert_images_to_dng(image_urls, dng_converter_path, tmp_dir)
            print("Time to convert images: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
            DispatchQueue.main.async { progress.int += 10000000 }
            t = DispatchTime.now().uptimeNanoseconds
        }
    }
       
    // load images
    t = DispatchTime.now().uptimeNanoseconds
    var (textures, mosaic_pattern_width, white_level, black_level, exposure_bias) = try load_images(dng_urls)
    print("Time to load all images: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    t = DispatchTime.now().uptimeNanoseconds
    DispatchQueue.main.async { progress.int += (convert_to_dng ? 10000000 : 20000000) }
    
    // if images have a uniform exposure: use central image as reference
    var ref_idx = image_urls.count / 2
       
    // if images have different exposures: use image with lowest exposure as reference to protect highlights
    // inspired by https://ai.googleblog.com/2021/04/hdr-with-bracketing-on-pixel-phones.html
    for comp_idx in 0..<image_urls.count {
     
        if exposure_bias[comp_idx] < exposure_bias[ref_idx] {
            ref_idx = comp_idx
        }
    }
    
     // if user has selected the "higher quality" algorithm but has a non-Bayer sensor, warn them the "Fast" algorithm will be used instead
    var merging_algorithm = merging_algorithm
    if merging_algorithm == "Higher quality" && mosaic_pattern_width != 2 {
        DispatchQueue.main.async { progress.show_nonbayer_hq_alert = true }
        merging_algorithm = "Fast"
    }
       
    // convert images from uint16 to float16
    textures = textures.map{texture_uint16_to_float($0)}
     
    // set the tile size for the alignment
    let tile_size_dict = [
        "Small": 16,
        "Medium": 32,
        "Large": 64,
    ]
    let tile_size_int = tile_size_dict[tile_size]!
    
    // set the maximum resolution of the smallest pyramid layer
    let search_distance_dict = [
        "Small": 128,
        "Medium": 64,
        "Large": 32,
    ]
    let search_distance_int = search_distance_dict[search_distance]!
    
    // use a 32 bit float as final image
    let final_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: textures[ref_idx].width, height: textures[ref_idx].height, mipmapped: false)
    final_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let final_texture = device.makeTexture(descriptor: final_texture_descriptor)!
    fill_with_zeros(final_texture)
    
    // correction of hot pixels
    if mosaic_pattern_width == 2 {
        // correction of hot pixels
        correct_hotpixels(textures, black_level)
        
        // correction of intensities for images with bracketed exposure
        // inspired by https://ai.googleblog.com/2021/04/hdr-with-bracketing-on-pixel-phones.html
        equalize_exposure(textures, black_level, exposure_bias, ref_idx)
    }
    
    // special mode: simple temporal averaging without alignment and robust merging
    if noise_reduction == 23.0 {
        print("Special mode: temporal averaging only...")
                  
        // iterate over all images
        for comp_idx in 0..<image_urls.count {
            
            add_texture(textures[comp_idx], final_texture, image_urls.count)
            DispatchQueue.main.async { progress.int += Int(80000000/Double(image_urls.count)) }
        }
        

    // alignment and merging of tiles in frequency domain (only 2x2 Bayer pattern)
    } else if merging_algorithm == "Higher quality" {
        print("Merging in the frequency domain...")
        
        // the tile size for merging in frequency domain is set to 8x8 for all tile sizes used for alignment. The smaller tile size leads to a reduction of artifacts at specular highlights at the expense of a slightly reduced suppression of low-frequency noise in the shadows
        // see https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf for more details
        let tile_size_merge = Int(8)

        // This correction accounts for the fact that bursts with exposure bracketing include images with longer exposure times, which exhibit a better signal-to-noise ratio. Thus the average noise level to be expected has to be reduced. As the same correction is applied to all images, images with longer exposures are slightly preferred by the merging algorithm compared to images with lower exposure. This simplified formula does not correctly include read noise (a square is ignored) and as a consequence, merging weights in the shadows will be very slightly overestimated. As the shadows are typically lifted heavily in HDR shots, this effect may be even preferable.
        var robustness_exposure_corr = 0.0

        for comp_idx in 0..<exposure_bias.count {
            let exposure_factor = pow(2.0, (Double(exposure_bias[comp_idx]-exposure_bias[ref_idx])/100.0))
            robustness_exposure_corr += (0.5 + 0.5/exposure_factor)
        }
        robustness_exposure_corr /= Double(exposure_bias.count)
                
        // derive normalized robustness value: two steps in noise_reduction (-2.0 in this case) yield an increase by a factor of two in the robustness norm with the idea that the variance of shot noise increases by a factor of two per iso level
        let robustness_rev = 0.5*(26.5-Double(Int(noise_reduction+0.5)))
        let robustness_norm = robustness_exposure_corr*pow(2.0, (-robustness_rev + 7.5));
        
        // derive estimate of read noise with the idea that read noise increases approx. by a factor of three (stronger than increase in shot noise) per iso level to increase noise reduction in darker regions relative to bright regions
        let read_noise = pow(pow(2.0, (-robustness_rev + 10.0)), 1.6);
        
        // derive a maximum value for the motion norm with the idea that denoising can be stronger in static regions with good alignment compared to regions with motion
        // factors from Google paper: daylight = 1, night = 6, darker night = 14, extreme low-light = 25. We use a continuous value derived from the robustness value to cover a similar range as proposed in the paper
        // see https://graphics.stanford.edu/papers/night-sight-sigasia19/night-sight-sigasia19.pdf for more details
        let max_motion_norm = max(1.0, pow(1.3, (11.0-robustness_rev)));
                
        // perform align and merge 4 times in a row with slight displacement of the frame to prevent artifacts in the merging process. The shift equals the tile size used in the merging process here, which later translates into tile_size_merge/2 when each color channel is processed independently
        try align_merge_frequency_domain(progress: progress, shift_left: tile_size_merge, shift_right: 0, shift_top: tile_size_merge, shift_bottom: 0, ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, tile_size_merge: tile_size_merge, robustness_norm: robustness_norm, read_noise: read_noise, max_motion_norm: max_motion_norm, exposure_bias: exposure_bias, black_level: black_level, textures: textures, final_texture: final_texture)
        
        try align_merge_frequency_domain(progress: progress, shift_left: 0, shift_right: tile_size_merge, shift_top: tile_size_merge, shift_bottom: 0, ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, tile_size_merge: tile_size_merge, robustness_norm: robustness_norm, read_noise: read_noise, max_motion_norm: max_motion_norm, exposure_bias: exposure_bias, black_level: black_level, textures: textures, final_texture: final_texture)
         
        try align_merge_frequency_domain(progress: progress, shift_left: tile_size_merge, shift_right: 0, shift_top: 0, shift_bottom: tile_size_merge,ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, tile_size_merge: tile_size_merge, robustness_norm: robustness_norm, read_noise: read_noise, max_motion_norm: max_motion_norm, exposure_bias: exposure_bias, black_level: black_level, textures: textures, final_texture: final_texture)
            
        try align_merge_frequency_domain(progress: progress, shift_left: 0, shift_right: tile_size_merge, shift_top: 0, shift_bottom: tile_size_merge, ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, tile_size_merge: tile_size_merge, robustness_norm: robustness_norm, read_noise: read_noise, max_motion_norm: max_motion_norm, exposure_bias: exposure_bias, black_level: black_level, textures: textures, final_texture: final_texture)
                
        
    // alignment and merging of tiles in the spatial domain (supports non-Bayer sensors)
    } else {
        print("Merging in the spatial domain...")
    
        let kernel_size = Int(16) // kernel size of binomial filtering used for blurring the image
        
        // derive normalized robustness value: four steps in noise_reduction (-4.0 in this case) yield an increase by a factor of two in the robustness norm with the idea that the sd of shot noise increases by a factor of sqrt(2) per iso level
        let robustness_rev = 0.5*(32.0-Double(Int(noise_reduction+0.5)))
        let robustness_norm = 0.12*pow(1.35, robustness_rev) - 0.1380840
    
        try align_merge_spatial_domain(progress: progress, ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, kernel_size: kernel_size, robustness: robustness_norm, textures: textures, final_texture: final_texture)
    }
    
    // apply tone mapping if the reference image is underexposed: by lifting the shadows, more shadow information is available while the tone mapping operator is constructed in such a way that highlights are protected
    // inspired by https://www-old.cs.utah.edu/docs/techreports/2002/pdf/UUCS-02-001.pdf
    if (mosaic_pattern_width == 2 && comp_underexposure == "Correction on") {
        correct_underexposure(final_texture, white_level[ref_idx], black_level, exposure_bias, ref_idx)
    }
    
    // convert final image to 16 bit integer
    let output_texture_uint16 = convert_uint16(final_texture)
      
    print("Time to align+merge all images: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    t = DispatchTime.now().uptimeNanoseconds
    
    DispatchQueue.main.async { progress.int += 10000000 }
    
    // set output location
    let in_url = dng_urls[ref_idx]
    let in_filename = in_url.deletingPathExtension().lastPathComponent
    // the value of the noise reduction strength is written into the filename
    let suffix_merging = merging_algorithm == "Higher quality" ? "q" : "f"
    let out_filename = in_filename + (noise_reduction==23.0 ? "_merged_avg.dng" : "_merged_" + suffix_merging + "\(Int(noise_reduction+0.5)).dng")
    let out_path = (final_dng_conversion ? tmp_dir : out_dir) + out_filename
    var out_url = URL(fileURLWithPath: out_path)
    
    // save the output image
    try texture_to_dng(output_texture_uint16, in_url, out_url)
    
    // check if dng converter is installed
    if final_dng_conversion {
        let path_delete = out_dir + out_filename
          
        // delete dng file if an old version exists
        if FileManager.default.fileExists(atPath: path_delete) {
            try FileManager.default.removeItem(atPath: path_delete)
        }
        
        // the dng converter is installed -> convert output DNG saved before with Adobe DNG Converter, which increases compatibility of the resulting DNG
        let final_url = try convert_images_to_dng([out_url], dng_converter_path, out_dir)
                   
        // update out URL to new file
        out_url = final_url[0]
    }
    
    // delete the temporary dng directory
    try FileManager.default.removeItem(atPath: tmp_dir)
    
    print("Time to save final image: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    print("")
    print("Total processing time for", textures.count, "images: ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
    print("")
    
    return out_url
}


// convenience function for the spatial merging approach
func align_merge_spatial_domain(progress: ProcessingProgress, ref_idx: Int, mosaic_pattern_width: Int, search_distance: Int, tile_size: Int, kernel_size: Int, robustness: Double, textures: [MTLTexture], final_texture: MTLTexture) throws {
    
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
    
    // build reference pyramid
    let ref_pyramid = build_pyramid(ref_texture, downscale_factor_array)
    
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
        
        // check that the comparison image has the same resolution as the reference image
        if (ref_texture.width != comp_texture.width) || (ref_texture.height != comp_texture.height) {
            throw AlignmentError.inconsistent_resolutions
        }
       
        // align comparison texture
        let aligned_texture = crop_texture(align_texture(ref_pyramid, comp_texture, downscale_factor_array, tile_size_array, search_dist_array), pad_align_x, pad_align_x, pad_align_y, pad_align_y)
        
        // robust-merge the texture
        let merged_texture = robust_merge(textures[ref_idx], ref_texture_blurred, aligned_texture, kernel_size, robustness, noise_sd, mosaic_pattern_width)
        
        // add robust-merged texture to the output image
        add_texture(merged_texture, final_texture, textures.count)
        
        // sync GUI progress
        DispatchQueue.main.async { progress.int += Int(80000000/Double(textures.count)) }
    }
}


// convenience function for the frequency-based merging approach
func align_merge_frequency_domain(progress: ProcessingProgress, shift_left: Int, shift_right: Int, shift_top: Int, shift_bottom: Int, ref_idx: Int, mosaic_pattern_width: Int, search_distance: Int, tile_size: Int, tile_size_merge: Int, robustness_norm: Double, read_noise: Double, max_motion_norm: Double, exposure_bias: [Int], black_level: [Int], textures: [MTLTexture], final_texture: MTLTexture) throws {
    
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
        
    // build reference pyramid
    let ref_pyramid = build_pyramid(ref_texture, downscale_factor_array)
    
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
    let ref_texture_rgba = convert_rgba(ref_texture, crop_merge_x, crop_merge_y)
    
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
        
        // align comparison texture
        let aligned_texture_rgba = convert_rgba(align_texture(ref_pyramid, comp_texture, downscale_factor_array, tile_size_array, search_dist_array), crop_merge_x, crop_merge_y)
                 
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
        
        // merge aligned comparison texture with reference texture in the frequency domain
        merge_frequency_domain(ref_texture_ft, aligned_texture_ft, final_texture_ft, rms_texture, mismatch_texture, robustness_norm, read_noise, max_motion_norm, tile_info_merge);
               
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
    output_texture = crop_texture(convert_bayer(output_texture), pad_left-crop_merge_x, pad_right-crop_merge_x, pad_top-crop_merge_y, pad_bottom-crop_merge_y)
    
    // add output texture to the final texture to collect all textures of the four iterations
    add_texture(output_texture, final_texture, 1)
}


// ===========================================================================================================
// Helper functions
// ===========================================================================================================

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


func texture_uint16_to_float(_ in_texture: MTLTexture) -> MTLTexture {

    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Float, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
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


func convert_uint16(_ in_texture: MTLTexture) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Uint, width: in_texture.width, height: in_texture.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
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


func upsample(_ input_texture: MTLTexture, width: Int, height: Int, mode: String) -> MTLTexture {
    
    // convert args
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


func avg_pool(_ input_texture: MTLTexture, _ scale: Int) -> MTLTexture {

    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: input_texture.width/scale, height: input_texture.height/scale, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    
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


func blur_mosaic_texture(_ in_texture: MTLTexture, _ kernel_size: Int, _ mosaic_pattern_width: Int) -> MTLTexture {
    
    // create a temp texture blurred along x-axis only and the output texture, blurred along both x- and y-axis
    let blur_x = texture_like(in_texture)
    let blur_xy = texture_like(in_texture)
    
    let kernel_size_mapped = (kernel_size == 16) ? 16 : max(0, min(8, kernel_size))
    
    // blur the texture along the x-axis
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = blur_mosaic_x_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: in_texture.height, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(blur_x, index: 1)
    command_encoder.setBytes([Int32(kernel_size_mapped)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(mosaic_pattern_width)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)

    // blur the texture along the y-axis
    let state2 = blur_mosaic_y_state
    command_encoder.setComputePipelineState(state2)
    command_encoder.setTexture(blur_x, index: 0)
    command_encoder.setTexture(blur_xy, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return blur_xy
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


func add_texture(_ in_texture: MTLTexture, _ out_texture: MTLTexture, _ n_textures: Int) {
    
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


func texture_like(_ input_texture: MTLTexture) -> MTLTexture {
    let output_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: input_texture.pixelFormat, width: input_texture.width, height: input_texture.height, mipmapped: false)
    output_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let output_texture = device.makeTexture(descriptor: output_texture_descriptor)!
    return output_texture
}


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


func convert_rgba(_ in_texture: MTLTexture, _ crop_merge_x: Int, _ crop_merge_y: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba16Float, width: (in_texture.width-2*crop_merge_x)/2, height: (in_texture.height-2*crop_merge_y)/2, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
        
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_rgba_state
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


func convert_bayer(_ in_texture: MTLTexture) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: in_texture.pixelFormat, width: in_texture.width*2, height: in_texture.height*2, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
        
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = convert_bayer_state
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


func texture_mean(_ in_texture: MTLTexture, _ pixelformat: String) -> MTLBuffer {
    
    // If the parameter pixelformat has the value "rgba", the texture mean is calculated for the four color channels independently. An input texture in pixelformat "rgba" is expected for that purpose. In all other cases, a single mean value of the texture is calculated.
    
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
    
    // return the average of all pixels in the input array
    return avg_buffer
}



// ===========================================================================================================
// Functions specific to image alignment
// ===========================================================================================================

func align_texture(_ ref_pyramid: [MTLTexture], _ comp_texture: MTLTexture, _ downscale_factor_array: Array<Int>, _ tile_size_array: Array<Int>, _ search_dist_array: Array<Int>) -> MTLTexture {
        
    // initialize tile alignments
    let alignment_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rg16Sint, width: 1, height: 1, mipmapped: false)
    alignment_descriptor.usage = [.shaderRead, .shaderWrite]
    var prev_alignment = device.makeTexture(descriptor: alignment_descriptor)!
    var current_alignment = device.makeTexture(descriptor: alignment_descriptor)!
    var tile_info = TileInfo(tile_size: 0, tile_size_merge: 0, search_dist: 0, n_tiles_x: 0, n_tiles_y: 0, n_pos_1d: 0, n_pos_2d: 0)
    
    // build comparison pyramid
    let comp_pyramid = build_pyramid(comp_texture, downscale_factor_array)
    
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
        
        // upsample alignment vectors by a factor of 2
        prev_alignment = upsample(current_alignment, width: n_tiles_x, height: n_tiles_y, mode: "nearest")
        
        // compare three alignment vector candidates, which improves alignment at borders of moving object
        // see https://graphics.stanford.edu/papers/hdrp/hasinoff-hdrplus-sigasia16.pdf for more details
        prev_alignment = correct_upsampling_error(ref_layer, comp_layer, prev_alignment, downscale_factor, tile_info)
          
        // compute tile differences
        let tile_diff = compute_tile_diff(ref_layer, comp_layer, prev_alignment, downscale_factor, tile_info)
      
        current_alignment = texture_like(prev_alignment)
        
        // compute tile alignment based on tile differences
        compute_tile_alignment(tile_diff, prev_alignment, current_alignment, downscale_factor, tile_info)
    }
    
    // warp the aligned layer
    tile_info.tile_size *= downscale_factor_array[0]
    let aligned_texture = warp_texture(comp_texture, current_alignment, tile_info, downscale_factor_array[0])
    
    return aligned_texture
}


func build_pyramid(_ input_texture: MTLTexture, _ downscale_factor_list: Array<Int>) -> Array<MTLTexture> {
    
    // iteratively resize the current layer in the pyramid
    var pyramid: Array<MTLTexture> = []
    for (i, downscale_factor) in downscale_factor_list.enumerated() {
        if i == 0 {
            pyramid.append(avg_pool(input_texture, downscale_factor))
         
        } else {
            pyramid.append(avg_pool(blur_mosaic_texture(pyramid.last!, 2, 1), downscale_factor))       
        }
    }
    return pyramid
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
    // either use generic function or highly-optimized function for testing a +/- 2 displacement in both image directions (in total 25 combinations)
    let state = (tile_info.n_pos_2d==25 ? compute_tile_differences25_state : compute_tile_differences_state)
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: (tile_info.n_pos_2d==25 ? 1 : tile_info.n_pos_2d))
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


func correct_upsampling_error(_ ref_layer: MTLTexture, _ comp_layer: MTLTexture, _ prev_alignment: MTLTexture, _ downscale_factor: Int, _ tile_info: TileInfo) -> MTLTexture {
    
    // create texture for corrected alignment
    let prev_alignment_corrected_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: prev_alignment.pixelFormat, width: prev_alignment.width, height: prev_alignment.height, mipmapped: false)
    prev_alignment_corrected_descriptor.usage = [.shaderRead, .shaderWrite]
    let prev_alignment_corrected = device.makeTexture(descriptor: prev_alignment_corrected_descriptor)!
        
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = correct_upsampling_error_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: tile_info.n_tiles_x, height: tile_info.n_tiles_y, depth: 1)
    let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
    command_encoder.setTexture(ref_layer, index: 0)
    command_encoder.setTexture(comp_layer, index: 1)
    command_encoder.setTexture(prev_alignment, index: 2)
    command_encoder.setTexture(prev_alignment_corrected, index: 3)
    command_encoder.setBytes([Int32(downscale_factor)], length: MemoryLayout<Int32>.stride, index: 0)
    command_encoder.setBytes([Int32(tile_info.tile_size)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.setBytes([Int32(tile_info.n_tiles_x)], length: MemoryLayout<Int32>.stride, index: 2)
    command_encoder.setBytes([Int32(tile_info.n_tiles_y)], length: MemoryLayout<Int32>.stride, index: 3)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    return prev_alignment_corrected
}


func warp_texture(_ texture_to_warp: MTLTexture, _ alignment: MTLTexture, _ tile_info: TileInfo, _ downscale_factor: Int) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture_to_warp.pixelFormat, width: texture_to_warp.width, height: texture_to_warp.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let warped_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
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


// ===========================================================================================================
// Functions specific to merging in the spatial domain
// ===========================================================================================================

func add_textures_weighted(_ texture1: MTLTexture, _ texture2: MTLTexture, _ weight_texture: MTLTexture) -> MTLTexture {
    
    let out_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: texture1.pixelFormat, width: texture1.width, height: texture1.height, mipmapped: false)
    out_texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let out_texture = device.makeTexture(descriptor: out_texture_descriptor)!
    
    // add textures
    let command_buffer = command_queue.makeCommandBuffer()!
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = add_textures_weighted_state
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
    let output_texture = add_textures_weighted(ref_texture, comp_texture, weight_texture_upsampled)
    
    return output_texture
}


// ===========================================================================================================
// Functions specific to merging in the frequency domain
// ===========================================================================================================

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


func merge_frequency_domain(_ ref_texture_ft: MTLTexture, _ aligned_texture_ft: MTLTexture, _ out_texture_ft: MTLTexture, _ rms_texture: MTLTexture, _ mismatch_texture: MTLTexture, _ robustness_norm: Double, _ read_noise: Double, _ max_motion_norm: Double, _ tile_info: TileInfo) {
        
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
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
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


func correct_hotpixels(_ textures: [MTLTexture], _ black_level: [Int]) {
    
    if (black_level[0] != -1) {
        
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
        let mean_texture_buffer = texture_mean(convert_rgba(average_texture, 0, 0), "rgba")
        
        // iterate over all images and correct hot pixels in each texture
        for comp_idx in 0..<textures.count {
            
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
            command_encoder.setBytes([Int32(black_level[comp_idx*4+0])], length: MemoryLayout<Int32>.stride, index: 1)
            command_encoder.setBytes([Int32(black_level[comp_idx*4+1])], length: MemoryLayout<Int32>.stride, index: 2)
            command_encoder.setBytes([Int32(black_level[comp_idx*4+2])], length: MemoryLayout<Int32>.stride, index: 3)
            command_encoder.setBytes([Int32(black_level[comp_idx*4+3])], length: MemoryLayout<Int32>.stride, index: 4)
            command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
            command_encoder.endEncoding()
            command_buffer.commit()
        }
    }
}


func equalize_exposure(_ textures: [MTLTexture], _ black_level: [Int], _ exposure_bias: [Int], _ ref_idx: Int) {

    // iterate over all images and correct exposure in each texture
    for comp_idx in 0..<textures.count {
        
        let exposure_diff = Int(exposure_bias[ref_idx] - exposure_bias[comp_idx])
        
        // only apply exposure correction if there is a different exposure and if a reasonable black level is available
        if (exposure_diff != 0 && black_level[0] != -1) {
            
            let command_buffer = command_queue.makeCommandBuffer()!
            let command_encoder = command_buffer.makeComputeCommandEncoder()!
            let state = equalize_exposure_state
            command_encoder.setComputePipelineState(state)
            let threads_per_grid = MTLSize(width: textures[comp_idx].width/2, height: textures[comp_idx].height/2, depth: 1)
            let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
            command_encoder.setTexture(textures[comp_idx], index: 0)
            command_encoder.setBytes([Int32(exposure_diff)], length: MemoryLayout<Int32>.stride, index: 0)
            command_encoder.setBytes([Int32(black_level[comp_idx*4+0])], length: MemoryLayout<Int32>.stride, index: 1)
            command_encoder.setBytes([Int32(black_level[comp_idx*4+1])], length: MemoryLayout<Int32>.stride, index: 2)
            command_encoder.setBytes([Int32(black_level[comp_idx*4+2])], length: MemoryLayout<Int32>.stride, index: 3)
            command_encoder.setBytes([Int32(black_level[comp_idx*4+3])], length: MemoryLayout<Int32>.stride, index: 4)
            command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
            command_encoder.endEncoding()
            command_buffer.commit()
        }
    }
}


func correct_underexposure(_ final_texture: MTLTexture, _ white_level: Int, _ black_level: [Int], _ exposure_bias: [Int], _ ref_idx: Int) {
          
    // only apply exposure correction if reference image is underexposed
    if (exposure_bias[ref_idx] < -1 && white_level != -1 && black_level[0] != -1) {
     
        var exp_idx = 0
        var uniform_exposure = true
        // find index of image with longest exposure to use the most robust black level value
        for comp_idx in 0..<exposure_bias.count {         
            if (exposure_bias[comp_idx] > exposure_bias[exp_idx]) {
                exp_idx = comp_idx
                uniform_exposure = false
            }
        }
        
        var black_level0 = 0.0
        var black_level1 = 0.0
        var black_level2 = 0.0
        var black_level3 = 0.0
        
        // if exposure levels are uniform, calculate mean value of all exposures
        if uniform_exposure {
            
            for comp_idx in 0..<exposure_bias.count {
                black_level0 += Double(black_level[comp_idx*4+0])
                black_level1 += Double(black_level[comp_idx*4+1])
                black_level2 += Double(black_level[comp_idx*4+2])
                black_level3 += Double(black_level[comp_idx*4+3])
            }
            
            black_level0 /= Double(exposure_bias.count)
            black_level1 /= Double(exposure_bias.count)
            black_level2 /= Double(exposure_bias.count)
            black_level3 /= Double(exposure_bias.count)
            
        } else {
            black_level0 = Double(black_level[exp_idx*4+0])
            black_level1 = Double(black_level[exp_idx*4+1])
            black_level2 = Double(black_level[exp_idx*4+2])
            black_level3 = Double(black_level[exp_idx*4+3])
        }              
    
        let command_buffer = command_queue.makeCommandBuffer()!
        let command_encoder = command_buffer.makeComputeCommandEncoder()!
        let state = correct_underexposure_state
        command_encoder.setComputePipelineState(state)
        let threads_per_grid = MTLSize(width: final_texture.width/2, height: final_texture.height/2, depth: 1)
        let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
        command_encoder.setTexture(final_texture, index: 0)
        command_encoder.setBytes([Int32(exposure_bias[ref_idx])], length: MemoryLayout<Int32>.stride, index: 0)
        command_encoder.setBytes([Int32(white_level)], length: MemoryLayout<Int32>.stride, index: 1)
        command_encoder.setBytes([Float32(black_level0)], length: MemoryLayout<Float32>.stride, index: 2)
        command_encoder.setBytes([Float32(black_level1)], length: MemoryLayout<Float32>.stride, index: 3)
        command_encoder.setBytes([Float32(black_level2)], length: MemoryLayout<Float32>.stride, index: 4)
        command_encoder.setBytes([Float32(black_level3)], length: MemoryLayout<Float32>.stride, index: 5)
        command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
        command_encoder.endEncoding()
        command_buffer.commit()
    }    
}

