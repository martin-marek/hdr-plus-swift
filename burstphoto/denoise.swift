import Foundation
import SwiftUI
import MetalPerformanceShaders


// possible error types during the alignment
enum AlignmentError: Error {
    case less_than_two_images
    case inconsistent_extensions
    case inconsistent_resolutions
    case conversion_failed
    case missing_dng_converter
    case non_bayer_exposure_bracketing
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
    @Published var show_nonbayer_exposure_alert = false
    @Published var show_nonbayer_bit_depth_alert = false
    @Published var show_exposure_bit_depth_alert = false
}

// set up Metal device
let device = MTLCreateSystemDefaultDevice()!
let command_queue = device.makeCommandQueue()!
let mfl = device.makeDefaultLibrary()!

// Must use NSString and not String since NSCache needs classes.
let textureCache = NSCache<NSString, ImageCacheWrapper>()

/**
 Main function of the burst photo app.
 */
func perform_denoising(image_urls: [URL], progress: ProcessingProgress, merging_algorithm: String = "Fast", tile_size: String = "Medium", search_distance: String = "Medium", noise_reduction: Double = 13.0, exposure_control: String = "LinearFullRange", output_bit_depth: String = "Native", out_dir: String, tmp_dir: String) throws -> URL {
    
    // Maximum size for the caches
    let textureCacheMaxSizeMB: Double = min(10_000.0,
                                            0.15 * Double(ProcessInfo.processInfo.physicalMemory) / 1000.0 / 1000.0)
    /// The initial value is set to be twice that of the in-memory cache.
    /// It is capped between 4–10 GB, but never allowed to be greater that 15% of the total free disk space.
    /// There is a hard cap of 15% of the total system free disk space.
    let maxDNGFolderSizeGB: Double = min(10.0,
                                         0.15 * systemFreeDiskSpace(),
                                         max(4.0,
                                             2 * textureCacheMaxSizeMB/1000))
    
    textureCache.totalCostLimit = Int(textureCacheMaxSizeMB)
    
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
       
    // ensure that all files are .dng, converting them if necessary
    var dng_urls = image_urls
    let convert_to_dng = image_extension.lowercased() != "dng"
    DispatchQueue.main.async { progress.includes_conversion = convert_to_dng }
    let dng_converter_path = "/Applications/Adobe DNG Converter.app"
    let dng_converter_present = FileManager.default.fileExists(atPath: dng_converter_path)
    
    if convert_to_dng {
        // check if dng converter is installed
        if !dng_converter_present {
            // if dng coverter is not installed, prompt user
            throw AlignmentError.missing_dng_converter
        } else {
            // the dng converter is installed -> use it
            print("Converting images...")
            dng_urls = try convert_raws_to_dngs(image_urls, dng_converter_path, tmp_dir, textureCache)
            print("Time to convert images: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
            DispatchQueue.main.async { progress.int += 10_000_000 }
            t = DispatchTime.now().uptimeNanoseconds
        }
    }
       
    // load images
    t = DispatchTime.now().uptimeNanoseconds
    print("Loading images...")
    var (textures, mosaic_pattern_width, white_level, black_level, exposure_bias, ISO_exposure_time, color_factors) = try load_images(dng_urls, textureCache: textureCache)
    print("Time to load all images: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    t = DispatchTime.now().uptimeNanoseconds
    DispatchQueue.main.async { progress.int += (convert_to_dng ? 10_000_000 : 20_000_000) }
    
    // if images have a uniform exposure: use central image as reference
    var ref_idx = image_urls.count / 2
    var uniform_exposure = true
    
    for comp_idx in 0..<image_urls.count {
        // if images have different exposures: use image with lowest exposure as reference to protect highlights
        // inspired by https://ai.googleblog.com/2021/04/hdr-with-bracketing-on-pixel-phones.html
        if exposure_bias[comp_idx] < exposure_bias[ref_idx] {
            ref_idx = comp_idx
        }
        // check if exposure is uniform or bracketed
        if (exposure_bias[comp_idx] != exposure_bias[0]) {
            uniform_exposure = false
        }
    }
    
    // check for non-Bayer sensors that the exposure of images is uniform
    if (!uniform_exposure && mosaic_pattern_width != 2) {throw AlignmentError.non_bayer_exposure_bracketing}
    
    // if user has selected a value different than "Off" for exposure control but has a non-Bayer sensor, warn them that exposure control = "Off" will be used instead
    var exposure_control = exposure_control
    if uniform_exposure && exposure_control != "Off" && mosaic_pattern_width != 2 {
        DispatchQueue.main.async { progress.show_nonbayer_exposure_alert = true }
        exposure_control = "Off"
    }
    
    // if exposure control = "Off" and uniform exposure, set black level and white level to -1
    if exposure_control == "Off" && uniform_exposure {
        for idx in 0..<white_level.count { white_level[idx] = -1 }
        for idx in 0..<black_level.count { black_level[idx] = -1 }
        for idx in 0..<color_factors.count { color_factors[idx] = -1.0 }
        for idx in 0..<ISO_exposure_time.count { ISO_exposure_time[idx] = -1.0 }
    }
    
    // if user has selected the "higher quality" algorithm but has a non-Bayer sensor, warn them the "Fast" algorithm will be used instead
    var merging_algorithm = merging_algorithm
    if merging_algorithm == "Higher quality" && mosaic_pattern_width != 2 {
        DispatchQueue.main.async { progress.show_nonbayer_hq_alert = true }
        merging_algorithm = "Fast"
    }


    // if user has selected the "16Bit" output bit depth but has a non-Bayer sensor, warn them the "Native" output bit depth will be used instead
    var output_bit_depth = output_bit_depth
    if output_bit_depth == "16Bit" && mosaic_pattern_width != 2 {
     DispatchQueue.main.async { progress.show_nonbayer_bit_depth_alert = true }
     output_bit_depth = "Native"
    }

    // if user has selected the "16Bit" output bit depth but has a non-Bayer sensor, warn them the "Native" output bit depth will be used instead
    if output_bit_depth == "16Bit" && exposure_control == "Off" {
     DispatchQueue.main.async { progress.show_exposure_bit_depth_alert = true }
     output_bit_depth = "Native"
    }
       
    // convert images from uint16 to float16
    textures = textures.map{convert_uint16_to_float($0)}
    
    // check that the comparison image has the same resolution as the reference image
    for comp_idx in 0..<textures.count {
        if (textures[ref_idx].width != textures[comp_idx].width) || (textures[ref_idx].height != textures[comp_idx].height) {
            throw AlignmentError.inconsistent_resolutions
        }
    }

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
    
    if mosaic_pattern_width == 2 {
        correct_hotpixels(textures, black_level, ISO_exposure_time, noise_reduction)
        equalize_exposure(textures, black_level, exposure_bias, ref_idx)
    }
    
    // special mode: simple temporal averaging without alignment and robust merging
    if noise_reduction == 23.0 {
        print("Special mode: temporal averaging only...")
        
        try calculate_temporal_average(progress: progress, mosaic_pattern_width: mosaic_pattern_width, exposure_bias: exposure_bias, white_level: white_level[ref_idx], black_level: black_level, uniform_exposure: uniform_exposure, color_factors: color_factors, textures: textures, final_texture: final_texture)
  

    // alignment and merging of tiles in frequency domain (only 2x2 Bayer pattern)
    } else if merging_algorithm == "Higher quality" {
        print("Merging in the frequency domain...")
        
        // The tile size for merging in frequency domain is set to 8x8 for all tile sizes used for alignment. The smaller tile size leads to a reduction of artifacts at specular highlights at the expense of a slightly reduced suppression of low-frequency noise in the shadows. The fixed value of 8 is supported by the highly-optimized fast Fourier transform (works up to value of <= 16). A slow, but easier to understand discrete Fourier transform is also provided for values larger than 16.
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
        let robustness_rev = 0.5*(26.5-Double(Int(noise_reduction+0.5)))
        let robustness_norm = exposure_corr1/exposure_corr2*pow(2.0, (-robustness_rev + 7.5))
        
        // derive estimate of read noise with the idea that read noise increases approx. by a factor of three (stronger than increase in shot noise) per iso level to increase noise reduction in darker regions relative to bright regions
        let read_noise = pow(pow(2.0, (-robustness_rev + 10.0)), 1.6)
        
        // derive a maximum value for the motion norm with the idea that denoising can be stronger in static regions with good alignment compared to regions with motion
        // factors from Google paper: daylight = 1, night = 6, darker night = 14, extreme low-light = 25. We use a continuous value derived from the robustness value to cover a similar range as proposed in the paper
        // see https://graphics.stanford.edu/papers/night-sight-sigasia19/night-sight-sigasia19.pdf for more details
        let max_motion_norm = max(1.0, pow(1.3, (11.0-robustness_rev)))
                
        // perform align and merge 4 times in a row with slight displacement of the frame to prevent artifacts in the merging process. The shift equals the tile size used in the merging process here, which later translates into tile_size_merge/2 when each color channel is processed independently
        try align_merge_frequency_domain(progress: progress, shift_left_not_right: true, shift_top_not_bottom: true, ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, tile_size_merge: tile_size_merge, robustness_norm: robustness_norm, read_noise: read_noise, max_motion_norm: max_motion_norm, uniform_exposure: uniform_exposure, exposure_bias: exposure_bias, black_level: black_level, color_factors: color_factors, textures: textures, final_texture: final_texture)
        
        try align_merge_frequency_domain(progress: progress, shift_left_not_right: false, shift_top_not_bottom: true, ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, tile_size_merge: tile_size_merge, robustness_norm: robustness_norm, read_noise: read_noise, max_motion_norm: max_motion_norm, uniform_exposure: uniform_exposure, exposure_bias: exposure_bias, black_level: black_level, color_factors: color_factors, textures: textures, final_texture: final_texture)
         
        try align_merge_frequency_domain(progress: progress, shift_left_not_right: true, shift_top_not_bottom: false, ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, tile_size_merge: tile_size_merge, robustness_norm: robustness_norm, read_noise: read_noise, max_motion_norm: max_motion_norm, uniform_exposure: uniform_exposure, exposure_bias: exposure_bias, black_level: black_level, color_factors: color_factors, textures: textures, final_texture: final_texture)
            
        try align_merge_frequency_domain(progress: progress, shift_left_not_right: false, shift_top_not_bottom: false, ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, tile_size_merge: tile_size_merge, robustness_norm: robustness_norm, read_noise: read_noise, max_motion_norm: max_motion_norm, uniform_exposure: uniform_exposure, exposure_bias: exposure_bias, black_level: black_level, color_factors: color_factors, textures: textures, final_texture: final_texture)
                
        
    // alignment and merging of tiles in the spatial domain (supports non-Bayer sensors)
    } else {
        print("Merging in the spatial domain...")
    
        let kernel_size = Int(16) // kernel size of binomial filtering used for blurring the image
        
        // derive normalized robustness value: four steps in noise_reduction (-4.0 in this case) yield an increase by a factor of two in the robustness norm with the idea that the sd of shot noise increases by a factor of sqrt(2) per iso level
        let robustness_rev = 0.5*(36.0-Double(Int(noise_reduction+0.5)))
        let robustness_norm = 0.12*pow(1.3, robustness_rev) - 0.4529822
    
        try align_merge_spatial_domain(progress: progress, ref_idx: ref_idx, mosaic_pattern_width: mosaic_pattern_width, search_distance: search_distance_int, tile_size: tile_size_int, kernel_size: kernel_size, robustness: robustness_norm, uniform_exposure: uniform_exposure, black_level: black_level, color_factors: color_factors, textures: textures, final_texture: final_texture)
    }
    
    if (mosaic_pattern_width == 2 && exposure_control != "Off") {
        correct_exposure(final_texture, white_level[ref_idx], black_level, exposure_control, exposure_bias, uniform_exposure, color_factors, ref_idx)
    }
    
    // apply scaling to 16 bit
    let scale_to_16bit = (output_bit_depth=="16Bit" && mosaic_pattern_width == 2 && exposure_control != "Off")
    let factor_16bit = (scale_to_16bit ? Int(pow(2.0, 16.0-ceil(log2(Double(white_level[ref_idx]))))+0.5) : 1)

    // convert final image to 16 bit integer
    let output_texture_uint16 = convert_float_to_uint16(final_texture, (white_level[ref_idx] == -1 ? 1000000 : factor_16bit*white_level[ref_idx]), black_level, ref_idx, factor_16bit, color_factors)
      
    print("Time to align+merge all images: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    t = DispatchTime.now().uptimeNanoseconds
    
    DispatchQueue.main.async { progress.int += 10_000_000 }
    
    // set output location
    let ref_dng_url = dng_urls[ref_idx]
    let in_filename = ref_dng_url.deletingPathExtension().lastPathComponent
    // adapt output filename with merging algorithm and noise reduction setting
    var suffix_merging = (merging_algorithm == "Higher quality" ? "q" : "f")
    suffix_merging = (noise_reduction==23.0 ? "_merged_avg" : "_merged_" + suffix_merging + "\(Int(noise_reduction+0.5))")

    // adapt output filename with exposure control setting
    let suffix_exposure_control_dict = [
        "Off"             : "",
        "LinearFullRange" : "_l0",
        "Linear1EV"       : "_l1",
        "Curve0EV"        : "_nl0",
        "Curve1EV"        : "_nl1",
    ]
    let suffix_exposure_control = suffix_exposure_control_dict[exposure_control]!

    let out_filename = in_filename + suffix_merging + suffix_exposure_control + ".dng"
    let out_path = (dng_converter_present ? tmp_dir : out_dir) + out_filename
    var out_url = URL(fileURLWithPath: out_path)
    
    // Ensure reference texture exists on disk (may not if it existed in memory cache)
    _ = try convert_raws_to_dngs([image_urls[ref_idx]], dng_converter_path, tmp_dir, NSCache<NSString, ImageCacheWrapper>())
    
    // save the output image
    try texture_to_dng(output_texture_uint16, ref_dng_url, out_url, (scale_to_16bit ? Int32(factor_16bit*white_level[ref_idx]) : -1))
    
    // check if dng converter is installed
    if dng_converter_present {
        let path_delete = out_dir + out_filename
          
        // delete dng file if an old version exists
        if FileManager.default.fileExists(atPath: path_delete) {
            try FileManager.default.removeItem(atPath: path_delete)
        }
        
        // the dng converter is installed -> convert output DNG saved before with Adobe DNG Converter, which increases compatibility of the resulting DNG
        let final_url = try convert_raws_to_dngs([out_url], dng_converter_path, out_dir, textureCache, override_cache: true)
        
        // Delete tmp version of final_url in the tmp dir
        if FileManager.default.fileExists(atPath: out_path) {
            try FileManager.default.removeItem(atPath: out_path)
        }
                   
        // update out URL to new file
        out_url = final_url[0]
    }
    print("Time to save final image: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)
    print("")
    print("Total processing time for", textures.count, "images: ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
    print("")
    
    // Ensure the disk dng cache does not go above the set limit
    try trim_disk_cache(cache_dir: tmp_dir, to_max_size: maxDNGFolderSizeGB)
    
    return out_url
}


/**
 Convenience function for temporal averaging
 */
func calculate_temporal_average(progress: ProcessingProgress, mosaic_pattern_width: Int, exposure_bias: [Int], white_level: Int, black_level: [Int], uniform_exposure: Bool, color_factors: [Double], textures: [MTLTexture], final_texture: MTLTexture) throws {
    
    // find index of image with shortest exposure
    var exp_idx = 0
    for comp_idx in 0..<exposure_bias.count {
         if (exposure_bias[comp_idx] < exposure_bias[exp_idx]) {
            exp_idx = comp_idx
        }
    }
    
    if (!uniform_exposure && white_level != -1 && black_level[0] != -1 && mosaic_pattern_width == 2) {
        
        // initialize weight texture used for normalization of the final image
        let norm_texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r32Float, width: final_texture.width, height: final_texture.height, mipmapped: false)
        norm_texture_descriptor.usage = [.shaderRead, .shaderWrite]
        let norm_texture = device.makeTexture(descriptor: norm_texture_descriptor)!
        fill_with_zeros(norm_texture)
        
        // temporal averaging with exposure weighting
        for comp_idx in 0..<textures.count {
            
            add_texture_exposure(textures[comp_idx], final_texture, norm_texture, exposure_bias[comp_idx]-exposure_bias[exp_idx], ((comp_idx==exp_idx) ? 1_000_000 : white_level), black_level, comp_idx)
            DispatchQueue.main.async { progress.int += Int(80_000_000/Double(textures.count)) }
        }
        
        // normalization of the final image
        normalize_texture(final_texture, norm_texture)
        
    } else if (white_level != -1 && black_level[0] != -1 && color_factors[0] > -0.9 && mosaic_pattern_width == 2) {
        
        // temporal averaging with extrapolation of green channels for very bright pixels
        for comp_idx in 0..<textures.count {
            add_texture_highlights(textures[comp_idx], final_texture, textures.count, white_level, black_level, color_factors, comp_idx)
            DispatchQueue.main.async { progress.int += Int(80_000_000/Double(textures.count)) }
        }
    } else {
        
        // simple temporal averaging
        for comp_idx in 0..<textures.count {
            add_texture(textures[comp_idx], final_texture, textures.count)
            DispatchQueue.main.async { progress.int += Int(80_000_000/Double(textures.count)) }
        }
    }
}
