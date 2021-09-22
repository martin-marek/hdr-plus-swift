import SwiftUI


enum AppState {
    case main, processing, AppSettings, info
}

class AppSettings: ObservableObject {
    @AppStorage("tile_size") static var tile_size: Int = 16
    @AppStorage("search_distance") static var search_distance: String = "Medium"
    @AppStorage("robustness") static var robustness: Double = 0.5
    @AppStorage("show_img_saved_alert") static var show_img_saved_alert: Bool = true
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
    
    var body: some View {
        Group {
            if app_state == .main {
                MainView(app_state: $app_state, image_urls: $image_urls, progress: progress, my_alert: $my_alert)
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
                    secondaryButton: .default(Text("Don't show again")) {AppSettings.show_img_saved_alert = false}
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
    @State var drop_active = false
    
    var body: some View {
        let dropDelegate = MyDropDelegate(app_state: $app_state, image_urls: $image_urls, progress: progress, active: $drop_active, my_alert: $my_alert)
        
        VStack{
            Spacer()
            
            Text("Drag & drop a burst of DNG images")
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
            
            Text("*.dng, *.DNG")
                .font(.system(size: 14, weight: .light))
                .italic()
                .opacity(0.8)
                .frame(width: 200, height: 50)
            
            Spacer()
            
            HStack {
                AppSettingsButton().padding(10)
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
    let saving_as_num_of_images = 0
    
    func progress_int_to_str(_ int: Int) -> String {
        if progress.int < image_urls.count {
            return "Loading \(image_urls[progress.int].lastPathComponent)..."
        } else if progress.int < 2*image_urls.count {
            return "Processing \(image_urls[progress.int - image_urls.count].lastPathComponent)..."
        } else {
            return "Saving processed image..."
        }
    }
    
    var body: some View {
        
        ProgressView(progress_int_to_str(progress.int), value: Double(progress.int), total: Double(2*image_urls.count + saving_as_num_of_images))
            .font(.system(size: 16, weight: .medium))
            .opacity(0.8)
            .padding(20)
        
    }
}


struct SettingsView: View {
    let tile_sizes = [8, 16, 32, 64]
    let search_distances = ["Low", "Medium", "High"]

    var body: some View {
        VStack {
            Spacer()
            
            VStack(alignment: .leading) {
                Text("Tile size").font(.system(size: 14, weight: .medium))
                Picker(selection: AppSettings.$tile_size, label: EmptyView()) {
                    ForEach(tile_sizes, id: \.self) {
                        Text(String($0))
                    }
                }.pickerStyle(SegmentedPickerStyle())
            }.padding(20)
            
            Spacer()
            
            VStack(alignment: .leading) {
                Text("Search distance").font(.system(size: 14, weight: .medium))
                Picker(selection: AppSettings.$search_distance, label: EmptyView()) {
                    ForEach(search_distances, id: \.self) {
                        Text($0)
                    }
                }.pickerStyle(SegmentedPickerStyle())
            }.padding(20)
            
            Spacer()
            
            VStack(alignment: .leading) {
                Text("Robustness").font(.system(size: 14, weight: .medium))
                HStack {
                    Text("Low")
                    Slider(value: AppSettings.$robustness, in: 0...1, step: 0.1)
                    Text("High")
                }
            }.padding(20)
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
        
        DispatchQueue.global().async {
            // wait until all the urls are loaded
            // - this a a dirty hack to avoid any sync/async handling
            while all_file_urls.count < items.count {
                usleep(1000)
            }
            
            // if a directory was drag-and-dropped, convert it to a list of urls
            all_file_urls = optionally_convert_dir_to_urls(all_file_urls)
            
            // sort the urls alphabetically
            all_file_urls.sort(by: {$0.path < $1.path})
            
            // update the app's list of urls
            image_urls = all_file_urls
            
            // sync GUI
            DispatchQueue.main.async {
                progress.int = 0
                app_state = .processing
            }
            
            do {
                // compute reference index (use the middle image)
                let ref_idx = image_urls.count / 2
                
                // align and merge the burst
                let output_texture = try align_and_merge(image_urls: image_urls, progress: progress, ref_idx: ref_idx, search_distance: AppSettings.search_distance, tile_size: AppSettings.tile_size, robustness: AppSettings.robustness)

                // set output image url
                let in_url = image_urls[ref_idx]
                let in_filename = in_url.deletingPathExtension().lastPathComponent
                let out_filename = in_filename + "_merged"
                let out_path = NSHomeDirectory() + "/Downloads/" + out_filename + ".dng"
                let out_url = URL(fileURLWithPath: out_path)
                
                // save the output image
                try bayer_texture_to_dng(output_texture, in_url, out_url)

                // inform the user about the saved image
                if AppSettings.show_img_saved_alert {
                    my_alert.type = .image_saved
                    my_alert.title = "Image saved"
                    my_alert.message = "Image saved to \"Downloads/\(out_filename).dng\". Processed images are automatically saved to Downloads."
                    my_alert.show = true
                }
            } catch ImageIOError.load_error {
                my_alert.type = .error
                my_alert.title = "Unsupported format"
                my_alert.message = "Image format not supported. Please use DNG images only, converted directly from RAW files using Adobe Lightroom or Adobe DNG Convert. Avoid using DNG files generated from edited images."
                my_alert.show = true
            } catch ImageIOError.save_error {
                my_alert.type = .error
                my_alert.title = "Image couldn't be saved"
                my_alert.message = "The processed image could not be saved for an uknown reason. Sorry."
                my_alert.show = true
            } catch AlignmentError.less_than_two_images {
                my_alert.type = .error
                my_alert.title = "Burst required"
                my_alert.message = "Please drag-and-drop at least 2 images."
                my_alert.show = true
            } catch AlignmentError.inconsistent_extensions {
                my_alert.type = .error
                my_alert.title = "Inconsistent formats"
                my_alert.message = "The dropped files heve inconsistent formats. Please make sure that all images are DNG files."
                my_alert.show = true
            } catch AlignmentError.inconsistent_resolutions {
                my_alert.type = .error
                my_alert.title = "Inconsistent resolution"
                my_alert.message = "The dropped files heve inconsistent resolutions. Please make sure that all images are DNG files generated directly from camera RAW files using Adobe Lightroom or Adobe DNG Convert."
                my_alert.show = true
            } catch {
                my_alert.type = .error
                my_alert.title = "Unknown error"
                my_alert.message = "Something went wrong. Sorry."
                my_alert.show = true
            }
            
            // sync GUI
            DispatchQueue.main.async {
                app_state = .main
            }
        }

        return true
    }
}


struct HelpButton: View {
    // https://blog.urtti.com/creating-a-macos-help-button-in-swiftui
    let action: () -> Void = {NSWorkspace.shared.open(URL(string: "https://burst.photo/help/")!)}

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


struct AppSettingsButton: View {
    // https://stackoverflow.com/a/65356627/6495494
    let action: () -> Void = {NSApp.sendAction(Selector(("showPreferencesWindow:")), to: nil, from: nil)}
    
    var body: some View {
        Button(action: action, label: {
            ZStack {
                Circle()
                    .strokeBorder(Color(NSColor.separatorColor), lineWidth: 0.5)
                    .background(Circle().foregroundColor(Color(NSColor.controlColor)))
                    .shadow(color: Color(NSColor.separatorColor).opacity(0.3), radius: 1)
                    .frame(width: 25, height: 25)
                Image(systemName: "gearshape").resizable().frame(width: 15, height: 15).opacity(0.8)
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

