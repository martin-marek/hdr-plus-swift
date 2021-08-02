import SwiftUI


enum AppState {
    case main, processing, settings, info
}

class ProcessingProgress: ObservableObject {
    @Published var int = 0
}

struct MyAlert {
    enum ErrorType {
        case error, image_saved
    }
    
    var show: Bool = false
    var type: ErrorType?
    var title: String?
    var message: String?
}


struct ContentView: View {
    @State private var app_state = AppState.main
    @State private var image_urls: [URL] = []
    @StateObject var progress = ProcessingProgress()
    @State private var my_alert = MyAlert()
    @AppStorage("show_img_saved_alert") private var show_img_saved_alert = true
    
    var body: some View {
        Group {
            if app_state == .main {
                MainView(app_state: $app_state, image_urls: $image_urls, progress: progress, my_alert: $my_alert, show_img_saved_alert: $show_img_saved_alert)
            } else if app_state == .processing {
                ProcessingView(app_state: $app_state, image_urls: $image_urls, progress: progress)
            }
        }
        .alert(isPresented: $my_alert.show, content: {
            switch my_alert.type! {
            case .error:
                return Alert(
                    title: Text(my_alert.title!),
                    message: Text(my_alert.message!),
                    dismissButton: .cancel()
                )
            case .image_saved:
                return Alert(
                    title: Text(my_alert.title!),
                    message: Text(my_alert.message!),
                    primaryButton: .default(Text("OK")) {},
                    secondaryButton: .default(Text("Don't show again")) {show_img_saved_alert = false}
                )
            }
        })
    }
}


struct MainView: View {
    @Binding var app_state: AppState
    @Binding var image_urls: [URL]
    @ObservedObject var progress: ProcessingProgress
    @Binding var my_alert: MyAlert
    @Binding var show_img_saved_alert: Bool
    @State var drop_active = false
    
    var body: some View {
        let dropDelegate = MyDropDelegate(app_state: $app_state, image_urls: $image_urls, progress: progress, active: $drop_active, my_alert: $my_alert, show_img_saved_alert: $show_img_saved_alert)
        
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
                // SettingsButton().padding(10)
                Spacer()
                HelpButton().padding(10)
            }
        }
        .frame(minWidth: 0, maxWidth: .infinity, minHeight: 0, maxHeight: .infinity)
        .onDrop(of: ["public.file-url"], delegate: dropDelegate)
        // .background(TranslucentView())
        // .border(drop_active ? Color.accentColor : Color.clear, width: 2)
        .overlay(RoundedRectangle(cornerRadius: 10).stroke(drop_active ? Color.accentColor : Color.clear, lineWidth: 5).opacity(0.5))
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
    @Binding var my_alert: MyAlert
    @Binding var show_img_saved_alert: Bool
    
    
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
                usleep(1000)
            }
            
            // if a directory was drag-and-dropped, convert it to a list of urls
            all_file_urls = optionally_convert_dir_to_urls(all_file_urls)
            
            // update the app's list of urls
            image_urls = all_file_urls
            
            // aligna and merge all image
            app_state = .processing
            var merge_success = false
            do {
                // align and merge the burst
                let output_texture = try align_and_merge(image_urls, progress)
                merge_success = true
                
                // set output image url
                let in_url = image_urls[0]
                let in_dir = in_url.deletingLastPathComponent().path
                let in_filename = in_url.deletingPathExtension().lastPathComponent
                let out_filename = in_filename + "_merged"
                let out_path = in_dir + "/" + out_filename + ".TIFF"
                let out_url = URL(fileURLWithPath: out_path)
                
                // save the output image
                var image_save_success = false
                do {
                    try bayer_texture_to_tiff(output_texture, in_url, out_url)
                    image_save_success = true
                } catch {
                    my_alert.type = .error
                    my_alert.title = "Image couldn't be saved"
                    my_alert.message = "The processed image could not be saved for an uknown reason. Sorry."
                    my_alert.show = true
                }
                
                // inform the user about the saved image
                if image_save_success {
                    if show_img_saved_alert {
                        my_alert.type = .image_saved
                        my_alert.title = "Image saved"
                        my_alert.message = "Image saved to \"\(out_url.path)\". Processed images are automatically saved to the same location as the imported images."
                        my_alert.show = true
                    }
                }
            } catch AlignmentError.less_than_two_images {
                my_alert.type = .error
                my_alert.title = "Burst required"
                my_alert.message = "Please drag-and-drop at least 2 images."
                my_alert.show = true
            } catch AlignmentError.inconsistent_extensions {
                my_alert.type = .error
                my_alert.title = "Inconsistent formats"
                my_alert.message = "The dropped files heve inconsistent formats. Please make sure that all images are straight-out-of-camera RAW files."
                my_alert.show = true
            } catch AlignmentError.unsupported_image_type {
                my_alert.type = .error
                my_alert.title = "Unsupported format"
                my_alert.message = "Image format not supported. Please use RAW images only. If you are using straight-out-of-camera RAW images, your camera model is not supported yet."
                my_alert.show = true
            } catch AlignmentError.inconsistent_resolutions {
                my_alert.type = .error
                my_alert.title = "Inconsistent resolution"
                my_alert.message = "The dropped files heve inconsistent resolutions. Please make sure that all images are straight-out-of-camera RAW files from a single camera."
                my_alert.show = true
            } catch {
                my_alert.type = .error
                my_alert.title = "Unknown error"
                my_alert.message = "Something went wrong. Sorry."
                my_alert.show = true
            }
            app_state = .main
        }

        return true
    }
}


struct HelpButton: View {
    // https://blog.urtti.com/creating-a-macos-help-button-in-swiftui
    let action: () -> Void = {NSApp.sendAction(Selector(("showHelpWindow:")), to: nil, from: nil)}

    var body: some View {
        Button(action: action, label: {
            ZStack {
                Circle()
                    .strokeBorder(Color(NSColor.separatorColor), lineWidth: 0.5)
                    .background(Circle().foregroundColor(Color(NSColor.controlColor)))
                    .shadow(color: Color(NSColor.separatorColor).opacity(0.3), radius: 1)
                    .frame(width: 25, height: 25)
                Text("?").font(.system(size: 18, weight: .medium)).opacity(0.8)
            }
        })
        .buttonStyle(PlainButtonStyle())
    }
}


// struct SettingsButton: View {
//     // https://stackoverflow.com/a/65356627/6495494
//     let action: () -> Void = {NSApp.sendAction(Selector(("showPreferencesWindow:")), to: nil, from: nil)}
//
//     var body: some View {
//         Button(action: action, label: {
//             ZStack {
//                 Circle()
//                     .strokeBorder(Color(NSColor.separatorColor), lineWidth: 0.5)
//                     .background(Circle().foregroundColor(Color(NSColor.controlColor)))
//                     .shadow(color: Color(NSColor.separatorColor).opacity(0.3), radius: 1)
//                     .frame(width: 25, height: 25)
//                 Image(systemName: "gearshape").resizable().frame(width: 15, height: 15).opacity(0.8)
//             }
//         })
//         .buttonStyle(PlainButtonStyle())
//     }
// }


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

