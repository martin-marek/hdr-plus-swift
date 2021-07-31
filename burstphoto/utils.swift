import Foundation


func optionally_convert_dir_to_urls(_ urls: [URL]) -> [URL] {
    if urls.count == 1 {
        let url = urls[0]
        guard let url_is_dir = (try? url.resourceValues(forKeys: [.isDirectoryKey]))?.isDirectory else {
            return urls
        }
        if url_is_dir {
            let fm = FileManager.default
            // guard var file_names = try? fm.contentsOfDirectory(atPath: url.path) else {
            //     return urls
            // }
            guard let updated_urls = try? fm.contentsOfDirectory(at: url, includingPropertiesForKeys: [], options: [.skipsHiddenFiles, .skipsSubdirectoryDescendants]) else {
                return urls
            }
            return updated_urls
        }
    }
    return urls
}
