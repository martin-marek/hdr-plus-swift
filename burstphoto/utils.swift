import Foundation
import MetalKit


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

func convert_images_to_dng(_ in_urls: [URL], _ dng_converter_path: String, _ tmp_dir: String) throws -> [URL] {

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
