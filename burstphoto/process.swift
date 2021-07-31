import Foundation

func _align_and_merge(_ image_urls: [URL], _ progress: ProcessingProgress, ref_idx: Int = 0) {
    // check that 2+ images have been passed
    if image_urls.count < 2 {
        print("<= IMAGES")
    }
    
    // check that all images are of the same extension
    let ref_ext = image_urls[0].pathExtension
    for i in 1..<image_urls.count {
        let comp_ext = image_urls[i].pathExtension
        if comp_ext != ref_ext {
            print("EXTENSIONS NOT EQUAL")
        }
    }
    
    // check that all images are loadable by libraw
    
    
    // check that all images are of the same resolution
    
    
    // align and merge images
    _align_and_merge(image_urls, progress)
    
    // return output
}
