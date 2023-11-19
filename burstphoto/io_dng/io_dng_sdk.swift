import Foundation
import MetalKit

/**
 Must create a class to pass into NSCache.
 */
class ImageCacheWrapper : NSObject {
    let texture : MTLTexture
    let mosaic_pattern_width: Int
    let white_level: Int
    let black_levels: [Int]
    let exposure_bias: Int
    let ISO_exposure_time: Double
    let color_factors: [Double]
    
    init(texture: MTLTexture, mosaic_pattern_width: Int, white_level: Int, black_levels: [Int], exposure_bias: Int, ISO_exposure_time: Double, color_factors: [Double]) {
        self.texture = texture
        self.mosaic_pattern_width = mosaic_pattern_width
        self.white_level = white_level
        self.black_levels = black_levels
        self.exposure_bias = exposure_bias
        self.ISO_exposure_time = ISO_exposure_time
        self.color_factors = color_factors
    }
}

// possible error types
enum ImageIOError: Error {
    case load_error
    case metal_error
    case save_error
}

/// Take a list of urls representing non-DNG raw files and converts them using the Adobe DNG Converter.
/// If the output image already exists, it will not convert it again, unless `override_cache` is set to `true`.
///
/// - Parameter override_cache: Default of `false`. This value should always be set to `true` when outputting the final image.
func convert_raws_to_dngs(_ in_urls: [URL], _ dng_converter_path: String, _ tmp_dir: String, _ texture_cache: NSCache<NSString, ImageCacheWrapper>, override_cache: Bool = false) throws -> [URL] {

    // create command string
    let executable_path = dng_converter_path + "/Contents/MacOS/Adobe DNG Converter"
    // changed args to convert raw files to compressed DNGs
    let args = "--args -c -p0 -d \"\(tmp_dir)\""
    let command = "\"\(executable_path)\" \(args)"
    
    let compute_group = DispatchGroup()
    let compute_queue = DispatchQueue.global() // this is a concurrent queue to do compute
    
    var parallel_image_paths = [String](repeating: "",
                                        count: min(Int(0.75*Double(ProcessInfo.processInfo.processorCount)),
                                                   Int(0.6 + Double(in_urls.count)/2)
                                                  ))
    var urls_needing_conversion: Set<URL> = []
    
    // j here is used to keep the parallel queues of equal length in-case some, but not all, of the images are cached.
    var j = 0
    for i in 0..<in_urls.count {
        let out_path = tmp_dir + in_urls[i].deletingPathExtension().lastPathComponent + ".dng"
        let dng_exists = FileManager.default.fileExists(atPath: out_path)
        let in_memory_cache = texture_cache.object(forKey: NSString(string: URL(fileURLWithPath: out_path).absoluteString)) != nil
        
        if dng_exists {
            print(in_urls[i].lastPathComponent + " already exists as DNG, not converting to DNG.")
        }
        if in_memory_cache {
            print(in_urls[i].lastPathComponent + " already cached in memory, not converting to DNG.")
        }
        
        if override_cache || (!dng_exists && !in_memory_cache) {
            urls_needing_conversion.insert(in_urls[i])
            parallel_image_paths[Int(j/2) % parallel_image_paths.count] += " \"\(in_urls[i].relativePath)\""
            j += 2
            
            
        }
    }
    
    for parallel_command in parallel_image_paths {
        if !parallel_command.isEmpty {
            compute_queue.async(group: compute_group) {
                do {
                    try safeShell(command + parallel_command)
                } catch {}
            }
        }
    }
    
    // Wait until all images have been converted
    compute_group.wait()

    // return urls of the newly created dngs
    var out_urls: [URL] = []
    for url in in_urls {
        let fine_name = url.deletingPathExtension().lastPathComponent + ".dng"
        let out_path = tmp_dir + fine_name
        let out_url = URL(fileURLWithPath: out_path)
        out_urls.append(out_url)
        if urls_needing_conversion.contains(url) && !FileManager.default.fileExists(atPath: out_path) {
            throw AlignmentError.conversion_failed
        }
    }

    return out_urls
}


func image_url_to_texture(_ url: URL, _ device: MTLDevice) throws -> (MTLTexture, Int, Int, [Int], Int, Double, [Double]) {
    
    // read image
    var error_code: Int32
    var pixel_bytes: UnsafeMutableRawPointer?
    var width: Int32 = 0
    var height: Int32 = 0
    var _mosaic_pattern_width: Int32 = 0
    var white_level: Int32 = -1
    // Hardcoding mosaic width of 6, I don't think anything has a mosaic width above 6 (X-Trans sensor)
    var black_level_from_dng: [Int32]       = [Int32](repeating: -1, count: 6*6)
    let black_level_from_masked_area: [Int]
    var black_level: [Int]
    var exposure_bias: Int32 = -1
    var ISO_exposure_time: Float32 = 0.0;
    var color_factor_r: Float32 = -1.0
    var color_factor_g: Float32 = -1.0
    var color_factor_b: Float32 = -1.0
    var color_factors: [Double] = [0.0, 0.0, 0.0, 0.0]
    // There are at most 4 masked areas. Each one has, in order, the: top, left, bottom, and right edge of the masked area.
    var masked_areas: [Int32] = [
        -1, -1, -1, -1,
        -1, -1, -1, -1,
        -1, -1, -1, -1,
        -1, -1, -1, -1]
    
    error_code = read_dng_from_disk(url.path, &pixel_bytes, &width, &height, &_mosaic_pattern_width, &white_level, &black_level_from_dng, &masked_areas, &exposure_bias, &ISO_exposure_time, &color_factor_r, &color_factor_g, &color_factor_b)
    if (error_code != 0) {throw ImageIOError.load_error}
    
    let mosaic_pattern_width = Int(_mosaic_pattern_width)
    
    // convert image bitmap to MTLTexture
    let bytes_per_pixel = 2
    let bytes_per_row = bytes_per_pixel * Int(width)
    let texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Uint, width: Int(width), height: Int(height), mipmapped: false)
    texture_descriptor.usage = .shaderRead
    guard let texture = device.makeTexture(descriptor: texture_descriptor) else {throw ImageIOError.metal_error}
    texture.label = url.lastPathComponent
    
    texture.replace(region: MTLRegionMake2D(0, 0, Int(width), Int(height)), mipmapLevel: 0, withBytes: pixel_bytes!, bytesPerRow: bytes_per_row)
    
    
    free(pixel_bytes!)

    // If any masked areas exist, calculate the black levels from it
    black_level_from_masked_area = calculate_black_levels(for: texture, from_masked_areas: &masked_areas, mosaic_pattern_width: mosaic_pattern_width)
    
    // Load black levels either from the values the DNG reported or from the masked area
    black_level = [Int](repeating: 0, count: mosaic_pattern_width*mosaic_pattern_width)
    for i in 0..<black_level.count {
        // 0 is hardcoded as a suspicious black level value.
        if black_level_from_dng[i] > 0 {
            black_level[i] = Int(black_level_from_dng[i])
        } else {
            black_level[i] = black_level_from_masked_area[i]
        }
    }
    
    color_factors[0] = Double(color_factor_r)
    color_factors[1] = Double(color_factor_g)
    color_factors[2] = Double(color_factor_b)
    
    return (texture, mosaic_pattern_width, Int(white_level), black_level, Int(exposure_bias), Double(ISO_exposure_time), color_factors)
}


func load_images(_ urls: [URL], textureCache: NSCache<NSString, ImageCacheWrapper>) throws -> ([MTLTexture], Int, [Int], [[Int]], [Int], [Double], [[Double]]) {
    
    var textures_dict: [Int: MTLTexture] = [:]
    let compute_group = DispatchGroup()
    let compute_queue = DispatchQueue.global() // this is a concurrent queue to do compute
    let access_queue = DispatchQueue(label: "") // this is a serial queue to read/save data thread-safely
    var mosaic_pattern_width: Int?
    var white_level = Array(repeating: 0, count: urls.count)
    // Setting to
    var black_levels = Array(repeating: Array(repeating: 0, count: 6*6), count: urls.count)
    var exposure_bias = Array(repeating: 0, count: urls.count)
    var ISO_exposure_time = Array(repeating: 0.0, count: urls.count)
    var color_factors = Array(repeating: Array(repeating: 0.0, count: 3), count: urls.count)

    for i in 0..<urls.count {
        if let cachedValue = textureCache.object(forKey: NSString(string: urls[i].absoluteString)) {
            print("Loading image " + urls[i].lastPathComponent + " from in-memory cache.")
            access_queue.sync {
                textures_dict[i] = cachedValue.texture
                mosaic_pattern_width = cachedValue.mosaic_pattern_width
                white_level[i] = cachedValue.white_level
                for j in 0..<cachedValue.black_levels.count {
                    black_levels[i][j] = cachedValue.black_levels[j]
                }
                exposure_bias[i] = cachedValue.exposure_bias
                ISO_exposure_time[i] = cachedValue.ISO_exposure_time
                for j in 0..<3 {
                    color_factors[i][j] = cachedValue.color_factors[j]
                }
            }
        } else {
            print("Loading image " + urls[i].lastPathComponent + " from disk.")
            compute_queue.async(group: compute_group) {
                
                // asynchronously load texture
                if let (texture, _mosaic_pattern_width, _white_level, _black_levels, _exposure_bias, _ISO_exposure_time, _color_factors) = try? image_url_to_texture(urls[i], device) {
                    
                    // thread-safely save the texture
                    access_queue.sync {
                        textureCache.setObject(ImageCacheWrapper(texture: texture,
                                                                 mosaic_pattern_width: _mosaic_pattern_width,
                                                                 white_level: _white_level,
                                                                 black_levels: _black_levels,
                                                                 exposure_bias: _exposure_bias,
                                                                 ISO_exposure_time: _ISO_exposure_time,
                                                                 color_factors: _color_factors),
                                               forKey: NSString(string: urls[i].absoluteString),
                                               cost: Int(Float(texture.allocatedSize) / 1000 / 1000))
                        textures_dict[i] = texture
                        mosaic_pattern_width = _mosaic_pattern_width
                        white_level[i] = _white_level
                        for j in 0..<_black_levels.count {
                            black_levels[i][j] = _black_levels[j]
                        }
                        exposure_bias[i] = _exposure_bias
                        ISO_exposure_time[i] = _ISO_exposure_time
                        for j in 0..<3 {
                            color_factors[i][j] = _color_factors[j]
                        }
                    }
                }
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
    
    // Ensure mosaic pattern width is uniform between images.
    if mosaic_pattern_width != nil {
        let mosaic_squared = mosaic_pattern_width! * mosaic_pattern_width!
        for i in 0..<black_levels.count {
            for _ in mosaic_squared..<black_levels[i].count {
                _ = black_levels[i].popLast()
            }
        }
    }
    
    for i in 1..<textures_list.count {
        if (textures_list[0].width != textures_list[i].width) || (textures_list[0].height != textures_list[i].height) {
            throw AlignmentError.inconsistent_resolutions
        }
    }
    
    return (textures_list, mosaic_pattern_width!, white_level, black_levels, exposure_bias, ISO_exposure_time, color_factors)
}


func optionally_convert_dir_to_urls(_ urls: [URL]) -> [URL] {
    // if the argument is a list of one directory, return the urls withing that directory
    if urls.count == 1 {
        let url = urls[0]
        if let url_is_dir = (try? url.resourceValues(forKeys: [.isDirectoryKey]))?.isDirectory {
            if url_is_dir {
                let fm = FileManager.default
                if let updated_urls = try? fm.contentsOfDirectory(at: url, includingPropertiesForKeys: [], options: [.skipsHiddenFiles, .skipsSubdirectoryDescendants]) {
                    return updated_urls
                }
            }
        }
    }
    return urls
}


// https://stackoverflow.com/questions/26971240/how-do-i-run-a-terminal-command-in-a-swift-script-e-g-xcodebuild
@discardableResult // Add to suppress warnings when you don't want/need a result
func safeShell(_ command: String) throws -> String {
    let task = Process()
    let pipe = Pipe()
    
    task.standardOutput = pipe
    task.standardError = pipe
    task.arguments = ["-c", command]
    task.executableURL = URL(fileURLWithPath: "/bin/zsh") //<--updated
    task.standardInput = nil

    try task.run() //<--updated
    
    let data = pipe.fileHandleForReading.readDataToEndOfFile()
    let output = String(data: data, encoding: .utf8)!
    
    return output
}


func texture_to_dng(_ texture: MTLTexture, _ in_url: URL, _ out_url: URL, _ white_level: Int32) throws {
    // synchronize GPU and CPU memory
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Texture to DNG"
    let blit_encoder = command_buffer.makeBlitCommandEncoder()!
    blit_encoder.synchronize(resource: texture)
    blit_encoder.endEncoding()
    command_buffer.commit()
    command_buffer.waitUntilCompleted()
    
    // convert MTLTexture to a bitmap array
    let width = texture.width
    let height = texture.height
    let bytes_per_pixel = 2
    let bytes_per_row = bytes_per_pixel * width
    var bytes_pointer: UnsafeMutableRawPointer? = UnsafeMutableRawPointer.allocate(byteCount: width * height * bytes_per_pixel, alignment: 0)
    let mtl_region = MTLRegionMake2D(0, 0, width, height)
    texture.getBytes(bytes_pointer!, bytesPerRow: bytes_per_row, from: mtl_region, mipmapLevel: 0)

    // save image
    let error_code = write_dng_to_disk(in_url.path, out_url.path, &bytes_pointer, white_level)
    if (error_code != 0) {throw ImageIOError.save_error}
    
    // free memory
    free(bytes_pointer!)
}

/// Function to ensure that the specified cache directory does not become bigger than the specified size.
/// This folder will be deleted when the application starts and stops, but to ensure it does not become 10s of GBs while the application is running we run this function.
///
/// If the folder is larger than the specified maximum, files are deleted in chronological order (oldest ones first) under the asumption that those are the least likely to be needed again.
///
/// Based on information from:
///
/// - Parameter cache:      String representing the path to the to the dng cache folder on disk
/// - Parameter max_size:   The maximum size, in gigabytes, of the cache folder.
func trim_disk_cache(cache_dir: String, to_max_size max_size: Double) throws {
    let cache_url = URL(fileURLWithPath: cache_dir)
    
    let (contents, sizes) = size_of_and_chronological_contents_of(directory: cache_url)
    var excess_size = sizes.reduce(0, +) - max_size
    
    var i = 0
    while excess_size > 0 && i < contents.count {
        try FileManager.default.removeItem(at: contents[i])
        excess_size -= sizes[i]
        i += 1
    }
}
 

/// Get all files in the directory, calculate their size (in GB), sort them based on when they were added to the directory, and return the sorted URLs and dates as two arrays.
///
/// Assumes there are no sub-directories in the folder, which for our .dng folder this is true.
///
/// Based on:  https://stackoverflow.com/questions/32814535/how-to-get-directory-size-with-swift-on-os-x
func size_of_and_chronological_contents_of(directory: URL) -> ([URL], [Double]) {
    var contents: [URL] = []
    var dates: [Date] = []
    var sizes: [Double] = []
    do {
        contents = try FileManager.default.contentsOfDirectory(at: directory, includingPropertiesForKeys: [.fileSizeKey, .addedToDirectoryDateKey])
    } catch {
        return ([], [])
    }

    var i = 0
    while i < contents.count {
        let fileResourceValue: URLResourceValues
        do {
            fileResourceValue = try contents[i].resourceValues(forKeys: [.fileSizeKey, .addedToDirectoryDateKey])
        } catch {
            contents.remove(at: i)
            continue
        }
        i += 1
        
        dates.append(fileResourceValue.addedToDirectoryDate ?? Date(timeIntervalSince1970: 0)) // If it's missing default it to being the oldest file in the directory
        sizes.append(Double(fileResourceValue.fileSize ?? 0) / 1000 / 1000 / 1000)
    }
    // Sort the dates and sizes
    let sorted_indicies = dates.enumerated().sorted{$0.element < $1.element}.map{$0.offset}
    // Sort based on date and return only the sorted URLs.
    return (
        sorted_indicies.map{contents[$0]},
        sorted_indicies.map{sizes[$0]}
    )
}
