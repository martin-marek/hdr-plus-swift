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
                     
            "/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/DNG_Child/Test/",
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
            let robustness = 0.9
            let search_distance = "Medium"
            let tile_size = 32
            
            // align+merge
            let out_url = try perform_denoising(image_urls: image_urls, progress: progress, ref_idx: ref_idx, search_distance: search_distance, tile_size: tile_size, robustness: robustness)
            
            print("Image saved in:", out_url.relativePath)
            
        }
        
        // terminate Adobe XMP SDK
        terminate_xmp_sdk()
    }
}
