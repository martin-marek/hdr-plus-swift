func load_images(_ image_urls: [URL], _ progress: ProcessingProgress) async throws -> [MTLTexture] {
    
    var textures_dict: [Int: MTLTexture] = [:]
    try await withThrowingTaskGroup(of: (Int, MTLTexture).self) { group in
        // start loading all images concurrently
        for i in 0..<image_urls.count {
            group.addTask {
                let texture = try image_url_to_bayer_texture(image_urls[i], device)
                DispatchQueue.main.async {
                    progress.int += 1
                }
                return (i, texture)
            }
        }
        
        // collect loaded texures
        for try await (i, texture) in group {
            textures_dict[i] = texture
        }
    }
    // convert dict to list
    var textures_list: [MTLTexture] = []
    for i in 0..<image_urls.count {
        textures_list.append(textures_dict[i]!)
    }

    return textures_list
}


class ImageLoader {
    private var images: [MTLTexture] = []
    private var queue = DispatchQueue(label: "loading.queue", attributes: .concurrent)

    func load_image(_ url: URL) throws {
        try queue.sync(flags: .barrier) {
            let texture = try image_url_to_bayer_texture(url, device)
            images.append(texture)
        }
    }

    func load_images(_ urls: [URL]) throws {
        for url in urls {
            try load_image(url)
        }
    }

    func all_images() -> [URL] {
        return queue.sync { images }
    }
}

func load_images(_ urls: [URL], _ progress: ProcessingProgress) throws -> [MTLTexture] {
    var images: [MTLTexture] = []
    var queue = DispatchQueue(label: "loading.queue", attributes: .concurrent)

    // concurrently load images
    for url in urls {
        try queue.async {
            
            // load image
            let texture = try image_url_to_bayer_texture(url, device)

            queue.sync(flags: .barrier) {
                images.append(texture)
            }

            // update UI progress
            DispatchQueue.main.async { progress.int += 1 }
        }
    }

    // wait until all the images are loaded
    while images.count < urls.count {
        usleep(1000)
    }

    return the loaded images
    return images
}


// start concurrenlty loading images
let n = urls.count
let temp = UnsafeMutablePointer<MTLTexture>.allocate(capacity: urls.count)
var images: [MTLTexture] = []

let dispatch_group = DispatchGroup()
let dispatch_queue = DispatchQueue.global()
for i in 0..<n {
    dispatch_group.enter()
    dispatch_queue.async(group: dispatch_group) {
        print(i, urls[i])
        let texture = try! image_url_to_bayer_texture(urls[i], device)
        temp[i] = texture
    }
    dispatch_group.leave()
}

DispatchQueue.concurrentPerform(iterations: n, execute: { (i) in
    print(i, urls[i])
    let texture = try! image_url_to_bayer_texture(urls[i], device)
    temp[i] = texture
})

var textures_dict: [Int: MTLTexture] = [:]
try await withThrowingTaskGroup(of: (Int, MTLTexture).self) { group in
    // start loading all images concurrently
    for i in 0..<image_urls.count {
        group.addTask {
            let texture = try image_url_to_bayer_texture(image_urls[i], device)
            DispatchQueue.main.async {
                progress.int += 1
            }
            return (i, texture)
        }
    }
    // collect loaded texures
    for try await (i, texture) in group {
        textures_dict[i] = texture
    }
}
// convert dict to list
var textures_list: [MTLTexture] = []
for i in 0..<image_urls.count {
    textures_list.append(textures_dict[i]!)
}
