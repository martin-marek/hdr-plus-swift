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
            Spacer()
            
            Text("Drag & drop a burst of RAW images")
                .multilineTextAlignment(.center)
                .font(.system(size: 18, weight: .medium))
                .opacity(0.8)
                .frame(width: 200, height: 100)
            
            Spacer()
            
            Image(nsImage: NSImage(named: NSImage.Name("drop_icon"))!)
                .resizable()
                .renderingMode(.template)
                .foregroundColor(.primary)
                .opacity(drop_active ? 0.5 : 0.4)
                .frame(width: 160, height: 160)
            
            Spacer()
            
            Text("*.DNG, *.ARW, *.CR3, *.NEF...")
                .font(.system(size: 14, weight: .light))
                .italic()
                .opacity(0.8)
                .frame(width: 200, height: 50)
            
            Spacer()
            
            HStack {
                SettingsButton().padding(10)
                Spacer()
                HelpButton().padding(10)
            }
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
                            total: Double(image_urls.count - 1 + merge_as_num_of_images))
            .font(.system(size: 16, weight: .medium))
            .opacity(0.8)
            .padding(20)
    }
}


struct SettingsView: View {
    @AppStorage("tile_size") private var tile_size = 16
    @AppStorage("search_distance") private var search_distance = "Medium"
    let tile_sizes = [8, 16, 32, 64]
    let search_distances = ["Low", "Medium", "High"]

    var body: some View {
        VStack {
            Spacer()
            
            VStack(alignment: .leading) {
                Text("Tile size").font(.system(size: 14, weight: .medium))
                Picker(selection: $tile_size, label: EmptyView()) {
                    ForEach(tile_sizes, id: \.self) {
                        Text(String($0))
                    }
                }.pickerStyle(SegmentedPickerStyle())
            }.padding(20)
            
            Spacer()
            
            VStack(alignment: .leading) {
                Text("Search distance").font(.system(size: 14, weight: .medium))
                Picker(selection: $search_distance, label: EmptyView()) {
                    ForEach(search_distances, id: \.self) {
                        Text($0)
                    }
                }.pickerStyle(SegmentedPickerStyle())
            }.padding(20)
            
            Spacer()
            
            HStack {
                Spacer()
                HelpButton().padding(10)
            }
        }
        .navigationTitle("Preferences")
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

        var all_file_urls: [URL] = []
        let items = info.itemProviders(for: ["public.file-url"])
        for item in items {
            _ = item.loadObject(ofClass: URL.self) { url, _ in
                if let url = url {
                    all_file_urls.append(url)
                }
            }
        }
        
        progress.int = 0
        DispatchQueue.global().async {
            // wait until all the urls are loaded
            // - this a a dirty hack to avoid any sync/async handling
            while all_file_urls.count < items.count {
                print("sleeping")
                usleep(1000)
            }
            
            // if a directory was drag-and-dropped, convert it to a list of urls
            all_file_urls = optionally_convert_dir_to_urls(all_file_urls)
            
            print(all_file_urls)
            image_urls = all_file_urls
            app_state = .processing
            print("num. urls: ", all_file_urls.count)
            
            do {
                try align_and_merge(image_urls, progress)
            } catch AlignmentError.less_than_two_images {
              print("less_than_two_images")
            } catch AlignmentError.inconsistent_extensions {
              print("inconsistent_extensions")
            } catch AlignmentError.unsupported_image_type {
                print("unsupported_image_type")
            } catch AlignmentError.resolutions_dont_match {
                print("resolutions_dont_match")
            } catch {
                print("other error")
            }
            app_state = .main
        }

        return true
    }
}


struct HelpButton: View {
    let action: () -> Void = {NSApp.sendAction(Selector(("showHelpWindow:")), to: nil, from: nil)}

    var body: some View {
        Button(action: action, label: {
            ZStack {
                Circle()
                    .strokeBorder(Color(NSColor.separatorColor), lineWidth: 0.5)
                    .background(Circle().foregroundColor(Color(NSColor.controlColor)))
                    .shadow(color: Color(NSColor.separatorColor).opacity(0.3), radius: 1)
                    .frame(width: 20, height: 20)
                Text("?").font(.system(size: 15, weight: .medium)).opacity(0.8)
            }
        })
        .buttonStyle(PlainButtonStyle())
    }
}


struct SettingsButton: View {
    let action: () -> Void = {NSApp.sendAction(Selector(("showPreferencesWindow:")), to: nil, from: nil)}

    var body: some View {
        Button(action: action, label: {
            ZStack {
                Circle()
                    .strokeBorder(Color(NSColor.separatorColor), lineWidth: 0.5)
                    .background(Circle().foregroundColor(Color(NSColor.controlColor)))
                    .shadow(color: Color(NSColor.separatorColor).opacity(0.3), radius: 1)
                    .frame(width: 20, height: 20)
                Image(systemName: "gearshape").resizable().frame(width: 12, height: 12).opacity(0.8)
            }
        })
        .buttonStyle(PlainButtonStyle())
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

