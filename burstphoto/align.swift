import Foundation
import CoreImage
import MetalKit
import MetalPerformanceShaders


// alignment params
let downscale_factor_array = [2, 2, 4, 4]
let tile_size_array = [16, 16, 16, 16]
let search_dist_array = [1, 4, 4, 4]
let ref_idx = 0

// set up Metal device
let device = MTLCreateSystemDefaultDevice()!
let command_queue = device.makeCommandQueue()!
let metal_function_library = device.makeDefaultLibrary()!

func align_and_merge(_ image_urls: [URL], _ progress: ProcessingProgress) { // -> MTLTexture
    for i in 0 ..< image_urls.count {
        sleep(1)
        DispatchQueue.main.async {
            progress.int = i
        }
    }
    DispatchQueue.main.async {
        progress.int += 1
    }
}
