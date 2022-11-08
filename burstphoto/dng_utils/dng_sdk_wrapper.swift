import Foundation
import MetalKit

// possible error types
enum ImageIOError: Error {
    case load_error
    case metal_error
    case save_error
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

func optionally_convert_to_dngs(raw_ruls urls: [URL], using_tmp_dir tmp_dir_path: String, dng_converter_path: String) throws -> [URL] {
    var urls_new = urls
    
    for i in 0..<urls_new.capacity {
        let url_original = urls_new[i]
        let file_extension = url_original.absoluteString.suffix(3).lowercased()
        if file_extension != "dng" {
            // Call adobe dng converter
            let file_path_original = url_original.path
            let command = "." + dng_converter_path + " " + file_path_original + " -d " + tmp_dir_path
            let output = try safeShell(command)
            
            // Save the new URL
            let new_file_path = tmp_dir_path + url_original.deletingPathExtension().lastPathComponent + ".dng"
            let url_new = URL(fileURLWithPath: new_file_path)
            urls_new[i] = url_new
        }
    }
    return urls_new
}

func image_url_to_texture(_ url: URL, _ device: MTLDevice) throws -> (MTLTexture, Int) {
    
    // read image
    var error_code: Int32
    var pixel_bytes: UnsafeMutableRawPointer?
    var width: Int32 = 0;
    var height: Int32 = 0;
    var mosaic_pettern_width: Int32 = 0;
    error_code = read_image(url.path, &pixel_bytes, &width, &height, &mosaic_pettern_width)
    if (error_code != 0) {throw ImageIOError.load_error}
    
    // conver image bitmap to MTLTexture
    let bytes_per_pixel = 2
    let bytes_per_row = bytes_per_pixel * Int(width)
    let texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Uint, width: Int(width), height: Int(height), mipmapped: false)
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    guard let texture = device.makeTexture(descriptor: texture_descriptor) else {throw ImageIOError.metal_error}
    let mtl_region = MTLRegionMake2D(0, 0, Int(width), Int(height))
    texture.replace(region: mtl_region, mipmapLevel: 0, withBytes: pixel_bytes!, bytesPerRow: bytes_per_row)
    
    // free memory
    free(pixel_bytes!)

    return (texture, Int(mosaic_pettern_width))
}


func texture_to_dng(_ texture: MTLTexture, _ in_url: URL, _ out_url: URL) throws {
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
    let error_code = write_image(in_url.path, out_url.path, &bytes_pointer)
    if (error_code != 0) {throw ImageIOError.save_error}
    
    // free memory
    free(bytes_pointer!)
}
