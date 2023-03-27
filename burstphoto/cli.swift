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
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/DNG_Child Kopie/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/DNG_Klavier/Test/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/burst002_iso1000/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/Moon/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/Dark Sky/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/dinosaur order/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/highlights/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/33TJ_20150614_232110_642_dng order/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/bench-1/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/martin 7/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/Jozef-1-dng/",
            //"/Volumes/T7 2021/Workspace Photolab/Burst Photo Test/mountains-1/",
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
            let noise_reduction = 9.0 //9.0 for child, 15.0 for bench-1, 17.0 for martin 7, 13.0 for Google test, Dark Sky and dinosaur, 7.0 for Klavier
            let merging_algorithm = "Better quality"
            let tile_size = 32
            
            // align+merge
            let out_url = try perform_denoising(image_urls: image_urls, progress: progress, ref_idx: ref_idx, merging_algorithm: merging_algorithm, tile_size: tile_size, noise_reduction: noise_reduction)
            
            print("Image saved in:", out_url.relativePath)            
        }
        
        // terminate Adobe XMP SDK
        terminate_xmp_sdk()
    }
}
