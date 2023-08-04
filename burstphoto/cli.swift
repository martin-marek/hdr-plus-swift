import Foundation
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
            
            // options: value range from 1.0 to 23.0
            let noise_reduction = 13.0
            // options: "Fast" or "Higher quality"
            let merging_algorithm = "Fast"
            // options: "Small", "Medium" or "Large"
            let tile_size = "Medium"
            // options: "Small", "Medium" or "Large"
            let search_distance = "Medium"
            // options: "Off", "LinearFullRange", "Linear1EV", "Curve0EV" or "Curve1EV"
            let exposure_control = "LinearFullRange"
            // options: "Native" or "16Bit"
            let output_bit_depth = "Native"
            
            // align+merge
            let out_url = try perform_denoising(image_urls: image_urls, progress: progress, merging_algorithm: merging_algorithm, tile_size: tile_size, search_distance: search_distance, noise_reduction: noise_reduction, exposure_control: exposure_control, output_bit_depth: output_bit_depth)
           
            print("Image saved in:", out_url.relativePath)            
        }
        
        // terminate Adobe XMP SDK
        terminate_xmp_sdk()
        
        // Delete the temporary DNG directory
        try FileManager.default.removeItem(atPath: NSHomeDirectory() + "/Pictures/Burst Photo/.dngs/")
    }
}
