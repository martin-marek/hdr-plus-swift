import Foundation
import MetalKit

// possible error types
enum ImageIOError: Error {
    case load_error
    case metal_error
    case save_error
}



func image_url_to_texture(_ url: URL, _ device: MTLDevice) throws -> (MTLTexture, Int, Int, [Int], Int, [Double]) {
    
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
    var color_factor_r: Float32 = 0.0;
    var color_factor_g: Float32 = 0.0;
    var color_factor_b: Float32 = 0.0;
    var color_factors: [Double] = [0.0, 0.0, 0.0, 0.0];
    
    error_code = read_image(url.path, &pixel_bytes, &width, &height, &mosaic_pattern_width, &white_level, &black_level0, &black_level1, &black_level2, &black_level3, &exposure_bias, &color_factor_r, &color_factor_g, &color_factor_b)
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
    
    return (texture, Int(mosaic_pattern_width), Int(white_level), black_level, Int(exposure_bias), color_factors)
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
    let error_code = write_image(in_url.path, out_url.path, &bytes_pointer, white_level)
    if (error_code != 0) {throw ImageIOError.save_error}
    
    // free memory
    free(bytes_pointer!)
}
