import Foundation
import CoreImage
import MetalKit


func image_url_to_rgb_texture(_ url: URL, _ device: MTLDevice, _ command_queue: MTLCommandQueue) -> MTLTexture? {
    // works for raw images
    // - TODO: how do I do something like 'ci_filter.inputReturnDemosaiced = false'?
    // let t = DispatchTime.now().uptimeNanoseconds

    // create a CIFilter which holds the input image
    let ci_filter = CIFilter(imageURL: url)!
    
    // disable noise reduction / sharpening
    ci_filter.setValue(0, forKey: CIRAWFilterOption.noiseReductionAmount.rawValue)
    ci_filter.setValue(0, forKey: CIRAWFilterOption.colorNoiseReductionAmount.rawValue)
    ci_filter.setValue(0, forKey: CIRAWFilterOption.noiseReductionDetailAmount.rawValue)
    ci_filter.setValue(0, forKey: CIRAWFilterOption.luminanceNoiseReductionAmount.rawValue)
    ci_filter.setValue(0, forKey: CIRAWFilterOption.noiseReductionContrastAmount.rawValue)
    ci_filter.setValue(false, forKey: CIRAWFilterOption.enableSharpening.rawValue)

    // disable color processing
    ci_filter.setValue(0, forKey: CIRAWFilterOption.baselineExposure.rawValue)
    ci_filter.setValue(0, forKey: CIRAWFilterOption.boostAmount.rawValue)
    ci_filter.setValue(0, forKey: CIRAWFilterOption.boostShadowAmount.rawValue)
    ci_filter.setValue(true, forKey: CIRAWFilterOption.disableGamutMap.rawValue)
    if #available(macOS 11.1, *) {
        ci_filter.setValue(0, forKey: CIRAWFilterOption.ciInputLocalToneMapAmountKey.rawValue)
    }

    // create CIImage from the CIFilter
    let ci_image = ci_filter.outputImage!

    // render CIImage to a MTLTexture
    let command_buffer = command_queue.makeCommandBuffer()!
    let texture_descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: MTLPixelFormat.rgba16Unorm, width: Int(ci_image.extent.width), height: Int(ci_image.extent.height), mipmapped: false)
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    let texture = device.makeTexture(descriptor: texture_descriptor)!
    let ci_context = CIContext(options: nil)
    ci_context.render(ci_image, to: texture, commandBuffer: command_buffer, bounds: ci_image.extent, colorSpace: CGColorSpace(name: CGColorSpace.extendedSRGB)!)
    command_buffer.commit()
    // command_buffer.waitUntilCompleted()
    // print("time to load image: ", Float(DispatchTime.now().uptimeNanoseconds - t) / 1_000_000_000)

    return texture
}



func rgb_texture_to_tiff(_ texture: MTLTexture, _ url: URL) throws {
    // TODO: 16-bit TIFF output

    let ci_image = CIImage(mtlTexture: texture, options: nil)!
    // let ci_image = CIImage(mtlTexture: texture, options: [CIImageOption.colorSpace: kCVPixelFormatType_48RGB])!
    let ci_context = CIContext(options: nil)
    // let ci_context = CIContext(options: [CIContextOption.outputColorSpace: CGColorSpace(name: CGColorSpace.extendedLinearSRGB)!])
    // CIContextOption.workingColorSpace: CGColorSpace(name: CGColorSpace.extendedLinearSRGB)!
    // let ci_context = CIContext(options: [CIContextOption.outputColorSpace: CGColorSpace(name: CGColorSpace.extendedLinearSRGB)!])
    // try! ci_context.writeJPEGRepresentation(of: ci_image, to: url, colorSpace: CGColorSpace(name: CGColorSpace.linearSRGB)!)
    // try! ci_context.writeTIFFRepresentation(of: ci_image, to: url, format: .RGBA16, colorSpace: CGColorSpace(name: CGColorSpace.extendedLinearSRGB)!, options: [:])
    try! ci_context.writeTIFFRepresentation(of: ci_image, to: url, format: .RGBA16, colorSpace: CGColorSpace(name: CGColorSpace.extendedSRGB)!, options: [:])
}
