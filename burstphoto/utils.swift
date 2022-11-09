import Foundation


func optionally_convert_dir_to_urls(_ urls: [URL]) -> [URL] {
    // if the argument is a list of one directory, return the urls withing that directory
    if urls.count == 1 {
        let url = urls[0]
        if let url_is_dir = (try? url.resourceValues(forKeys: [.isDirectoryKey]))?.isDirectory {
            if url_is_dir {
                let fm = FileManager.default
                if let updated_urls = try? fm.contentsOfDirectory(at: url, includingPropertiesForKeys: [], options: [.skipsHiddenFiles, .skipsSubdirectoryDescendants]) {
                    return updated_urls
                }
            }
        }
    }
    return urls
}


// https://stackoverflow.com/questions/26971240/how-do-i-run-a-terminal-command-in-a-swift-script-e-g-xcodebuild
@discardableResult // Add to suppress warnings when you don't want/need a result
func safeShell(_ command: String) throws -> String {
    let task = Process()
    let pipe = Pipe()
    
    task.standardOutput = pipe
    task.standardError = pipe
    task.arguments = ["-c", command]
    task.executableURL = URL(fileURLWithPath: "/bin/zsh") //<--updated
    task.standardInput = nil

    try task.run() //<--updated
    
    let data = pipe.fileHandleForReading.readDataToEndOfFile()
    let output = String(data: data, encoding: .utf8)!
    
    return output
}

func convert_to_dngs(_ in_urls: [URL], _ dng_converter_path: String, _ tmp_dir: String) throws -> [URL] {
    
    // creade command string
    let executable_path = dng_converter_path + "/Contents/MacOS/Adobe DNG Converter"
    let args = "--args -c -p0 -d \"\(tmp_dir)\""
    var command = "\"\(executable_path)\" \(args)"
    for url in in_urls {
        command += " \"\(url.relativePath)\""
    }
    print("command:", command)
    
    // call adobe dng converter
    let output = try safeShell(command)
    print("output:", output)
    let conversion_successful = !output.contains("Invalid")
    print("conversion_successful:", conversion_successful)
    
    // return urls of the newly created dngs
    var out_urls: [URL] = []
    for url in in_urls {
        let fine_name = url.deletingPathExtension().lastPathComponent + ".dng"
        let new_url = URL(fileURLWithPath: tmp_dir + fine_name)
        out_urls.append(new_url)
    }
    return out_urls
}
