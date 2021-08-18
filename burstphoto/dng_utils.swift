import Foundation
import MetalKit

// possible error types
enum ImageIOError: Error {
    case save_error
}

func image_url_to_bayer_texture(_ url: URL, _ device: MTLDevice) -> MTLTexture? {
    // keep track of execution time
    let t0 = DispatchTime.now().uptimeNanoseconds
    
    // error_code = read_image(url.path, pixel_bytes_pointer, width, height)
    // error_code = withUnsafePointer(to: &pixel_bytes, {read_image(url.path, $0, width, height)})
    
    // read image
    // var error_code: Int32
    // let pixel_bytes_pointer: UnsafeMutablePointer<UnsafeMutableRawPointer?>
    // let width_pointer: UnsafeMutablePointer<Int32>
    // let height_pointer: UnsafeMutablePointer<Int32>
    // error_code = read_image(url.path, pixel_bytes_pointer, width_pointer, height_pointer)
    // if (error_code != 0) {return nil}
    // let width = Int(width_pointer.pointee)
    // let height = Int(height_pointer.pointee)
    // let pixel_bytes = pixel_bytes_pointer.pointee!
    
    var error_code: Int32
    var pixel_bytes: UnsafeMutableRawPointer?
    var width: Int32 = 0;
    var height: Int32 = 0;
    error_code = read_image(url.path, &pixel_bytes, &width, &height)
    if (error_code != 0) {return nil}
    
    // conver image bitmap to MTLTexture
    let bytes_per_pixel = 2
    let bytes_per_row = bytes_per_pixel * Int(width)
    let texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Unorm, width: Int(width), height: Int(height), mipmapped: false)
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    guard let texture = device.makeTexture(descriptor: texture_descriptor) else {return nil}
    let mtl_region = MTLRegionMake2D(0, 0, Int(width), Int(height))
    texture.replace(region: mtl_region, mipmapLevel: 0, withBytes: pixel_bytes!, bytesPerRow: bytes_per_row)
    
// }

print("Time to load bayer matrix: ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
    return texture
}


func bayer_texture_to_dng(_ texture: MTLTexture, _ in_url: URL, _ out_url: URL) throws {
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
}
