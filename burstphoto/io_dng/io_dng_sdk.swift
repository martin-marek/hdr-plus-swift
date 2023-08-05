import Foundation
import MetalKit

// possible error types
enum ImageIOError: Error {
    case load_error
    case metal_error
    case save_error
}


func convert_raws_to_dngs(_ in_urls: [URL], _ dng_converter_path: String, _ tmp_dir: String) throws -> [URL] {

    // create command string
    let executable_path = dng_converter_path + "/Contents/MacOS/Adobe DNG Converter"
    // changed args to convert raw files to compressed DNGs
    let args = "--args -c -p0 -d \"\(tmp_dir)\""
    var command = "\"\(executable_path)\" \(args)"
    for url in in_urls {
        command += " \"\(url.relativePath)\""
    }

    // call adobe dng converter
    let output = try safeShell(command)

    // return urls of the newly created dngs
    var out_urls: [URL] = []
    for url in in_urls {
        let fine_name = url.deletingPathExtension().lastPathComponent + ".dng"
        let out_path = tmp_dir + fine_name
        let out_url = URL(fileURLWithPath: out_path)
        out_urls.append(out_url)
        if !FileManager.default.fileExists(atPath: out_path) {
            throw AlignmentError.conversion_failed
        }
    }

    return out_urls
}


func image_url_to_texture(_ url: URL, _ device: MTLDevice) throws -> (MTLTexture, Int, Int, [Int], Int, Double, [Double]) {
    
    // read image
    var error_code: Int32
    var pixel_bytes: UnsafeMutableRawPointer?
    var width: Int32 = 0;
    var height: Int32 = 0;
    var mosaic_pattern_width: Int32 = 0;
    var white_level: Int32 = 0;
    var black_level0: Int32 = 0;
    var black_level1: Int32 = 0;
    var black_level2: Int32 = 0;
    var black_level3: Int32 = 0;
    var black_level: [Int] = [0, 0, 0, 0];
    var exposure_bias: Int32 = 0;
    var ISO_exposure_time: Float32 = 0.0;
    var color_factor_r: Float32 = 0.0;
    var color_factor_g: Float32 = 0.0;
    var color_factor_b: Float32 = 0.0;
    var color_factors: [Double] = [0.0, 0.0, 0.0, 0.0];
    
    error_code = read_dng_from_disk(url.path, &pixel_bytes, &width, &height, &mosaic_pattern_width, &white_level, &black_level0, &black_level1, &black_level2, &black_level3, &exposure_bias, &ISO_exposure_time, &color_factor_r, &color_factor_g, &color_factor_b)
    if (error_code != 0) {throw ImageIOError.load_error}
    
    // convert image bitmap to MTLTexture
    let bytes_per_pixel = 2
    let bytes_per_row = bytes_per_pixel * Int(width)
    let texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Uint, width: Int(width), height: Int(height), mipmapped: false)
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    guard let texture = device.makeTexture(descriptor: texture_descriptor) else {throw ImageIOError.metal_error}
    let mtl_region = MTLRegionMake2D(0, 0, Int(width), Int(height))
    texture.replace(region: mtl_region, mipmapLevel: 0, withBytes: pixel_bytes!, bytesPerRow: bytes_per_row)
    
    // free memory
    free(pixel_bytes!)

    // convert four black levels to int16
    black_level[0] = Int(black_level0)
    black_level[1] = Int(black_level1)
    black_level[2] = Int(black_level2)
    black_level[3] = Int(black_level3)
    
    color_factors[0] = Double(color_factor_r)
    color_factors[1] = Double(color_factor_g)
    color_factors[2] = Double(color_factor_b)
    
    return (texture, Int(mosaic_pattern_width), Int(white_level), black_level, Int(exposure_bias), Double(ISO_exposure_time), color_factors)
}


func load_images(_ urls: [URL]) throws -> ([MTLTexture], Int, [Int], [Int], [Int], [Double], [Double]) {
    
    var textures_dict: [Int: MTLTexture] = [:]
    let compute_group = DispatchGroup()
    let compute_queue = DispatchQueue.global() // this is a concurrent queue to do compute
    let access_queue = DispatchQueue(label: "") // this is a serial queue to read/save data thread-safely
    var mosaic_pattern_width: Int?
    var white_level = Array(repeating: 0, count: urls.count)
    var black_level = Array(repeating: 0, count: 4*urls.count)
    var exposure_bias = Array(repeating: 0, count: urls.count)
    var ISO_exposure_time = Array(repeating: 0.0, count: urls.count)
    var color_factors = Array(repeating: 0.0, count: 3*urls.count)

    for i in 0..<urls.count {
        compute_queue.async(group: compute_group) {
    
            // asynchronously load texture
            if let (texture, _mosaic_pattern_width, _white_level, _black_level, _exposure_bias, _ISO_exposure_time, _color_factors) = try? image_url_to_texture(urls[i], device) {
        
                // thread-safely save the texture
                access_queue.sync {
                    textures_dict[i] = texture
                    mosaic_pattern_width = _mosaic_pattern_width
                    white_level[i] = _white_level
                    black_level[4*i+0] = _black_level[0]
                    black_level[4*i+1] = _black_level[1]
                    black_level[4*i+2] = _black_level[2]
                    black_level[4*i+3] = _black_level[3]
                    exposure_bias[i] = _exposure_bias
                    ISO_exposure_time[i] = _ISO_exposure_time
                    color_factors[3*i+0] = _color_factors[0]
                    color_factors[3*i+1] = _color_factors[1]
                    color_factors[3*i+2] = _color_factors[2]
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
    
    return (textures_list, mosaic_pattern_width!, white_level, black_level, exposure_bias, ISO_exposure_time, color_factors)
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
