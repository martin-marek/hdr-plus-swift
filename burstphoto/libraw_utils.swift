import Foundation
import MetalKit


func image_url_to_bayer_texture(_ url: URL, _ device: MTLDevice) -> MTLTexture? {
    // keep track of execution time
    // let t0 = DispatchTime.now().uptimeNanoseconds

    // create a pointer to the instance of a libraw_data_t structure
    let raw_data = libraw_init(0)

    // load image
    var msg = libraw_open_file(raw_data, url.path)
    if (msg != LIBRAW_SUCCESS.rawValue) {
        libraw_close(raw_data)
        return nil
    }
    
    // unpack image
    msg = libraw_unpack(raw_data)
    if (msg != LIBRAW_SUCCESS.rawValue) {
        libraw_close(raw_data)
        return nil
    }

    // get image pixels as bytes
    guard let bayer_matrix_bytes = raw_data!.pointee.rawdata.raw_image else {
        libraw_close(raw_data)
        return nil
    }
    
    // conver image bitmap to MTLTexture
    let width = Int(libraw_get_raw_width(raw_data))
    let height = Int(libraw_get_raw_height(raw_data))
    let bytes_per_pixel = 2
    let bytes_per_row = bytes_per_pixel * width
    let texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .r16Unorm, width: width, height: height, mipmapped: false)
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    guard let texture = device.makeTexture(descriptor: texture_descriptor) else {
        libraw_close(raw_data)
        return nil
    }
    let mtl_region = MTLRegionMake2D(0, 0, width, height)
    texture.replace(region: mtl_region, mipmapLevel: 0, withBytes: bayer_matrix_bytes, bytesPerRow: bytes_per_row)
    
    // release the raw data buffer
    // libraw_free_image(raw_data)
    // libraw_recycle(raw_data)
    libraw_close(raw_data)
    
    // print("Time to load bayer matrix: ", Float(DispatchTime.now().uptimeNanoseconds - t0) / 1_000_000_000)
    return texture
}


func bayer_texture_to_tiff(_ texture: MTLTexture, _ in_url: URL, _ out_url: URL){
    // create a pointer to the instance of a libraw_data_t structure
    let raw_data = libraw_init(0)

    // load image
    var msg = libraw_open_file(raw_data, in_url.path)
    msg = libraw_unpack(raw_data)
    
    // convert MTLTexture to a bitmap array
    let width = Int(libraw_get_raw_width(raw_data))
    let height = Int(libraw_get_raw_height(raw_data))
    let bytes_per_pixel = 2
    let bytes_per_row = bytes_per_pixel * width
    let bytes_pointer = UnsafeMutableRawPointer.allocate(byteCount: width * height * bytes_per_pixel, alignment: 0)
    let mtl_region = MTLRegionMake2D(0, 0, width, height)
    texture.getBytes(bytes_pointer, bytesPerRow: bytes_per_row, from: mtl_region, mipmapLevel: 0)
    
    // replace original bayer pixels by the new values
    UnsafeMutableRawPointer(raw_data!.pointee.rawdata.raw_image!).copyMemory(from: bytes_pointer, byteCount: width*height*bytes_per_pixel)
    
    // process image
    // raw_data!.pointee.params.no_auto_bright = Int32(1)
    msg = libraw_dcraw_process(raw_data)
    
    // save processing image
    // TODO: add settings for this
    // - output format: 8-bit tiff, 16-bit tiff, jpeg, png
    // - auto_exposure: true / false
    raw_data!.pointee.params.output_tiff = Int32(1) // required for Adobe app support
    raw_data!.pointee.params.output_bps = Int32(16)
    libraw_dcraw_ppm_tiff_writer(raw_data, out_url.path)
    
    // release the raw data buffer
    libraw_free_image(raw_data)
    libraw_recycle(raw_data)
    libraw_close(raw_data)
}
