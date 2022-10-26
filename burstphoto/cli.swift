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
            "/Users/martinmarek/My Drive/Coding/Burst/bursts/33TJ_20150614_232110_642_dng/",
            "/Users/martinmarek/My Drive/Coding/Burst/bursts/Monika-HP-dng/",
            "/Users/martinmarek/My Drive/Coding/Burst/bursts/Monika-stars-DNG/",
        ]
        
        // iterate over bursts
        for burst_dir in burst_dirs {
            
            // load image paths for the burst
            let fm = FileManager.default
            let image_names = try fm.contentsOfDirectory(atPath: burst_dir).filter{$0.hasSuffix(".dng") || $0.hasSuffix(".DNG")}
            var image_urls = image_names.map {URL(fileURLWithPath: burst_dir+$0)}
            image_urls.sort(by: {$0.path < $1.path})
            
            // ProcessingProgress is only useful for a GUI, but we have to instantiate one anyway
            let progress = ProcessingProgress()

            // set alignment params
            let ref_idx = image_urls.count / 2
            let robustness = 0.5
            let search_distance = "Medium"
            let tile_size = 16
            let kernel_size = 5
            
            // align+merge
            let output_texture = try align_and_merge(image_urls: image_urls, progress: progress, ref_idx: ref_idx, search_distance: search_distance, tile_size: tile_size, kernel_size: kernel_size, robustness: robustness)

            // set output image url
            let in_url = image_urls[ref_idx]
            let in_filename = in_url.deletingPathExtension().lastPathComponent
            let out_filename = in_filename + "_merged"
            let out_dir = NSHomeDirectory() + "/Pictures/Burst Photo/"
            let out_path = out_dir + out_filename + ".dng"
            let out_url = URL(fileURLWithPath: out_path)
            
            // create output directory
            if !FileManager.default.fileExists(atPath: out_dir) {
                try FileManager.default.createDirectory(atPath: out_dir, withIntermediateDirectories: true, attributes: nil)
            }
            
            // save the output image
            try texture_to_dng(output_texture, in_url, out_url)
        }
        
        // terminate Adobe XMP SDK
        terminate_xmp_sdk()
    }
}
