import SwiftUI


enum AppState {
    case main, processing, settings, info
}

class ProcessingProgress: ObservableObject {
    @Published var int = 0
}

struct ContentView: View {
    @State private var app_state = AppState.main
    @State private var image_urls: [URL] = []
    @StateObject var progress = ProcessingProgress()
    
    // DEBUG
    // @State private var app_state = AppState.processing
    // @State private var image_urls: [URL] = [URL(string: "1")!, URL(string: "2")!, URL(string: "3")!]
    
    var body: some View {
        Group {
            if app_state == .main {
                MainView(app_state: $app_state, image_urls: $image_urls, progress: progress)
            } else if app_state == .processing {
                ProcessingView(app_state: $app_state, image_urls: $image_urls, progress: progress)
            }
        }
    }
}

struct MainView: View {
    @Binding var app_state: AppState
    @Binding var image_urls: [URL]
    @ObservedObject var progress: ProcessingProgress
    @State var drop_active = false
    
    var body: some View {
        let dropDelegate = MyDropDelegate(app_state: $app_state, image_urls: $image_urls, progress: progress, active: $drop_active)
        
        VStack{
            Text("Drag & drop a burst of RAW images")
                .multilineTextAlignment(.center)
                .font(.system(size: 18, weight: .medium))
                .opacity(0.8)
                .frame(width: 200, height: 100)
            
            Image(nsImage: NSImage(named: NSImage.Name("drop_icon"))!)
                .resizable()
                .renderingMode(.template)
                .foregroundColor(.primary)
                .opacity(drop_active ? 0.5 : 0.4)
                .frame(width: 160, height: 160)
            
            Text("*.DNG, *.ARW, *.CR3, *.NEF...")
                .font(.system(size: 14, weight: .light))
                .italic()
                .opacity(0.8)
                .frame(width: 200, height: 100)
        }
        .frame(minWidth: 0, maxWidth: .infinity, minHeight: 0, maxHeight: .infinity)
        .onDrop(of: ["public.file-url"], delegate: dropDelegate)
        // .background(TranslucentView())
        // .border(drop_active ? Color.accentColor : Color.clear, width: 2)
        .overlay(RoundedRectangle(cornerRadius: 10).stroke(drop_active ? Color.accentColor : Color.clear, lineWidth: 4))
        .ignoresSafeArea()
    }
}

struct ProcessingView: View {
    @Binding var app_state: AppState
    @Binding var image_urls: [URL]
    @ObservedObject var progress: ProcessingProgress
    @State private var text = ""
    let merge_as_num_of_images = 0
    
    var body: some View {
        
        ProgressView(progress.int < image_urls.count ? "Processing \(image_urls[progress.int].lastPathComponent)..." : "Merging images...",
                            value: Double(progress.int),
                            total: Double(image_urls.count + merge_as_num_of_images))
            .font(.system(size: 16, weight: .medium))
            .opacity(0.8)
            .padding(20)
    }
}

struct MyDropDelegate: DropDelegate {
    @Binding var app_state: AppState
    @Binding var image_urls: [URL]
    @ObservedObject var progress: ProcessingProgress
    @Binding var active: Bool
    
    func validateDrop(info: DropInfo) -> Bool {
        return info.hasItemsConforming(to: ["public.file-url"])
    }
    
    func dropEntered(info: DropInfo) {
        self.active = true
    }
    
    func dropExited(info: DropInfo) {
        self.active = false
    }
    
    func performDrop(info: DropInfo) -> Bool {
        // https://swiftwithmajid.com/2020/04/01/drag-and-drop-in-swiftui/
        guard info.hasItemsConforming(to: ["public.file-url"]) else {
            return false
        }

        let items = info.itemProviders(for: ["public.file-url"])
        var all_file_urls: [URL] = []
        for item in items {
            _ = item.loadObject(ofClass: URL.self) { url, _ in
                if let url = url {
                    all_file_urls.append(url)
                }
            }
        }
        
        print(all_file_urls)
        image_urls = all_file_urls
        app_state = .processing
        progress.int = 0
        DispatchQueue.global().async {
            align_and_merge(image_urls, progress)
        }

        return true
    }
}

struct TranslucentView: NSViewRepresentable {
    // makes the window translucent
    // https://stackoverflow.com/a/63669868/6495494
    func makeNSView(context: Context) -> NSVisualEffectView {
        let view = NSVisualEffectView()
        view.blendingMode = .behindWindow
        view.isEmphasized = true
        view.material = .sidebar
        return view
    }
    func updateNSView(_ nsView: NSVisualEffectView, context: Context) {
    }
}

