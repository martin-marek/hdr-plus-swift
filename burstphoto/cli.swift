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
                     
            "/Volumes/My Burst Folder/Burst 01/",
            //"/Volumes/My Burst Folder/Burst 02/",
            //"/Volumes/My Burst Folder/Burst 03/",
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
            
            // set index of reference texture
            let ref_idx = image_urls.count / 2
            // options: value range from 1.0 to 23.0
            let noise_reduction = 13.0
            // options: "Better speed" or "Better quality"
            let merging_algorithm = "Better speed"
            // options: 16, 32, 64
            let tile_size = 32
            
            // align+merge
            let out_url = try perform_denoising(image_urls: image_urls, progress: progress, ref_idx: ref_idx, merging_algorithm: merging_algorithm, tile_size: tile_size, noise_reduction: noise_reduction)
            
            print("Image saved in:", out_url.relativePath)            
        }
        
        // terminate Adobe XMP SDK
        terminate_xmp_sdk()
    }
}
