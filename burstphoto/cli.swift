import Foundation
import CoreImage
import MetalKit
import MetalPerformanceShaders

// this is a command-line interface for Burst Photo, and is not part of the GUI application
// you can use this for development / automating jobs

@main
struct MyProgram {
    static func main() throws {
        
        // initialize Adobe XMP SDK
        initialize_xmp_sdk()
        
        // create a list of bursts to process
        let burst_dirs = [
            "/Users/martinmarek/Downloads/rafs mini/"
            // "/Users/martinmarek/My Drive/Coding/Burst/bursts/33TJ_20150614_232110_642_dng/",
            // "/Users/martinmarek/My Drive/Coding/Burst/bursts/Monika-HP-dng/",
            // "/Users/martinmarek/My Drive/Coding/Burst/bursts/Monika-stars-DNG/",
        ]
        
        // iterate over bursts
        for burst_dir in burst_dirs {
            
            // load image paths for the burst
            let fm = FileManager.default
            let burst_url = URL(fileURLWithPath: burst_dir)
            var image_urls = try fm.contentsOfDirectory(at: burst_url, includingPropertiesForKeys: [], options: [.skipsHiddenFiles, .skipsSubdirectoryDescendants])
            image_urls.sort(by: {$0.path < $1.path})
            
            // ProcessingProgress is only useful for a GUI, but we have to instantiate one anyway
            let progress = ProcessingProgress()

            // set alignment params
            let ref_idx = image_urls.count / 2
            let robustness = 0.5
            let search_distance = "Medium"
            let tile_size = 16
            let kernel_size = 5
            let dng_converter_path = "/Applications/Adobe DNG Converter.app"
            
            // align+merge
            let out_url = try align_and_merge(image_urls: image_urls, progress: progress, ref_idx: ref_idx, search_distance: search_distance, tile_size: tile_size, kernel_size: kernel_size, robustness: robustness, dng_converter_path: dng_converter_path)
            print("Image saved in:", out_url)
            
        }
        
        // terminate Adobe XMP SDK
        terminate_xmp_sdk()
    }
}
