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
    var width: Int32 = 0
    var height: Int32 = 0
    var _mosaic_pattern_width: Int32 = 0
    var white_level: Int32 = -1
    // Hardcoding 9, I don't think anything has a mosaic width above 3
    var black_level_from_dng: [Int32]       = [Int32](repeating: -1, count: 3*3)
    let black_level_from_masked_area: [Int]
    // TODO: How to deal with unknown length here? Hardcode 9 and only use the ones we need?
    var black_level: [Int]
    var exposure_bias: Int32 = -1
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
    
    error_code = read_image(url.path, &pixel_bytes, &width, &height, &_mosaic_pattern_width, &white_level, &black_level_from_dng, &masked_areas, &exposure_bias, &color_factor_r, &color_factor_g, &color_factor_b)
    if (error_code != 0) {throw ImageIOError.load_error}
    
    let mosaic_pattern_width = Int(_mosaic_pattern_width)
    
    // convert image bitmap to MTLTexture
    let bytes_per_pixel = 2
    let bytes_per_row = bytes_per_pixel * Int(width)
    let texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Uint, width: Int(width), height: Int(height), mipmapped: false)
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    guard let texture = device.makeTexture(descriptor: texture_descriptor) else {throw ImageIOError.metal_error}
    let mtl_region = MTLRegionMake2D(0, 0, Int(width), Int(height))
    texture.replace(region: mtl_region, mipmapLevel: 0, withBytes: pixel_bytes!, bytesPerRow: bytes_per_row)
    
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
    
    return (texture, mosaic_pattern_width, Int(white_level), black_level, Int(exposure_bias), color_factors)
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
