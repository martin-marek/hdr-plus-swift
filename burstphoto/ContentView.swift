import SwiftUI


enum AppState {
    case main, processing, image_saved
}

class AppSettings: ObservableObject {
    @AppStorage("tile_size") static var tile_size: Int = 16
    @AppStorage("search_distance") static var search_distance: String = "Medium"
    @AppStorage("noise_reduction") static var noise_reduction: Double = 14.0
}

struct MyAlert {
    var show: Bool = false
    var title: String?
    var message: String?
    var image_url: URL?
}


struct ContentView: View {
    @State private var app_state = AppState.main // DEBUG
    @State private var image_urls: [URL] = []
    @StateObject var progress = ProcessingProgress()
    @State private var my_alert = MyAlert()
    @State private var out_url = URL(fileURLWithPath: "")
    @State var drop_active = false
    
    var body: some View {
        let drop_delegate = MyDropDelegate(app_state: $app_state, image_urls: $image_urls, progress: progress, active: $drop_active, my_alert: $my_alert, out_url: $out_url)
        
        Group {
            switch app_state {
            case .main:
                MainView(drop_active: $drop_active)
                    .onDrop(of: ["public.file-url"], delegate: drop_delegate)
                    .overlay(RoundedRectangle(cornerRadius: 10).stroke(drop_active ? Color.accentColor : Color.clear, lineWidth: 5).opacity(0.5))
                    .ignoresSafeArea()
            case .processing:
                ProcessingView(image_urls: $image_urls, progress: progress)
            case .image_saved:
                ImageSavedView(out_url: $out_url, drop_active: $drop_active)
                    .onDrop(of: ["public.file-url"], delegate: drop_delegate)
                    .overlay(RoundedRectangle(cornerRadius: 10).stroke(drop_active ? Color.accentColor : Color.clear, lineWidth: 5).opacity(0.5))
                    .ignoresSafeArea()
            }
        }
        .alert(isPresented: $my_alert.show, content: {
            return Alert(
                title: Text(my_alert.title!),
                message: Text(my_alert.message!),
                dismissButton: .cancel()
            )
        })
        .frame(width: 350, height: 400)
    }
}


struct DropIcon: View {
    @Binding var drop_active: Bool
    
    var body: some View {
        Image(nsImage: NSImage(named: NSImage.Name("drop_icon"))!)
            .resizable()
            .renderingMode(.template)
            .foregroundColor(.primary)
            .opacity(drop_active ? 0.5 : 0.4)
            .frame(width: 160, height: 160)
    }
}


struct MainView: View {
    @Binding var drop_active: Bool
    
    var body: some View {
        VStack{
            Spacer()
            
            Text("Drag & drop a burst of raw image files")
                .multilineTextAlignment(.center)
                .font(.system(size: 20, weight: .medium))
                .opacity(0.8)
                .frame(width: 200)
                .padding()
            
            Spacer()
            
            DropIcon(drop_active: $drop_active)
            
            Spacer()
            
            Text("*.DNG, *.ARW, *.NEF…")
                .font(.system(size: 14, weight: .light))
                .italic()
                .opacity(0.8)
                .frame(width: 200, height: 50)
            
            HStack {
                SettingsButton().padding(10)
                Spacer()
                HelpButton().padding(10)
            }
        }
    }
}


struct ImageSavedView: View {
    @Binding var out_url: URL
    @Binding var drop_active: Bool
    
    var body: some View {
        VStack{
            Spacer()
            
            VStack{
                Text("Image saved")
                    .multilineTextAlignment(.center)
                    .font(.system(size: 26, weight: .medium))
                    .opacity(0.8)
                
                Text("Open the image using the button below or drag & drop a new burst.")
                    .multilineTextAlignment(.center)
                    .opacity(0.8)
                    .frame(width: 250)
                    .padding(1)
                
                Button(action: {NSWorkspace.shared.activateFileViewerSelecting([out_url])},
                       label: {Text("Show in Finder")})
            }
            
            Spacer()
            
            DropIcon(drop_active: $drop_active)
            
            Spacer()
            
            HStack {
                SettingsButton().padding(10)
                Spacer()
                HelpButton().padding(10)
            }
        }
    }
}


struct ProcessingView: View {
    @Binding var image_urls: [URL]
    @ObservedObject var progress: ProcessingProgress
        
    func progress_int_to_str(_ int: Int) -> String {
                
        if progress.includes_conversion {
            if progress.int < 10000000 {
                return "Converting images to DNG (this might take a while)..."
            } else if progress.int < 20000000 {
                return "Loading images..."
            } else if progress.int < 100000000 {
                
                let percent = round(Double(progress.int-20000000)/800000*10)/10.0
                           
                return "Processing images (\(percent)%)..."
                
            } else {
                return "Saving processed image..."
            }
        } else {
            if progress.int < 20000000 {
                return "Loading images..."
            } else if progress.int < 100000000 {
                
                let percent = round(Double(progress.int-20000000)/800000*10)/10.0
           
                return "Processing images (\(percent)%)..."
            } else {
                return "Saving processed image..."
            }
        }
    }
    
    var body: some View {
        ProgressView(progress_int_to_str(progress.int), value: Double(progress.int), total: 110000000.0)
            .font(.system(size: 16, weight: .medium))
            .opacity(0.8)
            .padding(20)
        
    }
}


struct SettingsView: View {
    let tile_sizes = [16, 32, 64]
    let search_distances = ["Low", "Medium", "High"]
    
    @State private var isEditing = false
     
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
                Text("Noise reduction:  \(Int(AppSettings.noise_reduction+0.5))")
                    .font(.system(size: 14, weight: .medium))
                    .foregroundColor(isEditing ? .secondary : .primary)
                HStack {
                    Slider(value: AppSettings.$noise_reduction, in: 1...25, step: 1.0,
                            onEditingChanged: { editing in                     isEditing = editing }
                    )
                    Stepper("", value: AppSettings.$noise_reduction, in: 1...25,
                            onEditingChanged: { editing in                     isEditing = editing }
                    )
                }
                Spacer()
                Text("Small values increase motion robustness and image sharpness.")
                    .font(.system(size: 12))
                    .foregroundColor(.secondary)
                Text("Large values increase the strength of noise reduction.")
                    .font(.system(size: 12))
                    .foregroundColor(.secondary)
            }.padding(20)
            
        }
        .frame(width: 400)
        .navigationTitle("Preferences")
    }
}


struct MyDropDelegate: DropDelegate {
    @Binding var app_state: AppState
    @Binding var image_urls: [URL]
    @ObservedObject var progress: ProcessingProgress
    @Binding var active: Bool
    @Binding var my_alert: MyAlert
    @Binding var out_url: URL
    
    
    func validateDrop(info: DropInfo) -> Bool {
        return info.hasItemsConforming(to: ["public.file-url"])
    }
    
    func dropEntered(info: DropInfo) {
        self.active = true
        if (app_state != .processing) {
            NSHapticFeedbackManager.defaultPerformer.perform(.alignment, performanceTime: .now)
        }
    }
    
    func dropExited(info: DropInfo) {
        self.active = false
        if (app_state != .processing) {
            NSHapticFeedbackManager.defaultPerformer.perform(.alignment, performanceTime: .now)
        }
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
            image_urls = optionally_convert_dir_to_urls(all_file_urls)
            
            // sort the urls alphabetically
            image_urls.sort(by: {$0.path < $1.path})
            
            // sync GUI
            DispatchQueue.main.async {
                progress.int = 0
                app_state = .processing
            }
            
            do {
                // compute reference index (use the middle image)
                let ref_idx = image_urls.count / 2

                // align and merge the burst
                out_url = try perform_denoising(image_urls: image_urls, progress: progress, ref_idx: ref_idx, search_distance: AppSettings.search_distance, tile_size: AppSettings.tile_size, noise_reduction: AppSettings.noise_reduction)
                
                // inform the user about the saved image
                app_state = .image_saved

            } catch ImageIOError.load_error {
                my_alert.title = "Unsupported format"
                my_alert.message = "Image format not supported. Please only use unprocessed RAW or DNG images. Using RAW images requires Adobe DNG Converter to be installed on your Mac."
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch ImageIOError.save_error {
                my_alert.title = "Image couldn't be saved"
                my_alert.message = "The processed image could not be saved for an uknown reason. Sorry."
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.less_than_two_images {
                my_alert.title = "Burst required"
                my_alert.message = "Please drag & drop at least 2 images."
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.inconsistent_extensions {
                my_alert.title = "Inconsistent formats"
                my_alert.message = "Please make sure that all images have the same format."
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.inconsistent_resolutions {
                my_alert.title = "Inconsistent resolution"
                my_alert.message = "Please make sure that all images have the same resolution."
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.missing_dng_converter {
                my_alert.title = "Missing Adobe DNG Converter"
                my_alert.message = "Only DNG files are supported natively. If you wish to use other RAW formats, please download and install Adobe DNG Converter. Burst Photo will then be able to process most RAW formats automatically."
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.conversion_failed {
                my_alert.title = "Conversion Failed"
                my_alert.message = "Image format not supported. Please only use unprocessed RAW or DNG images."
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch {
                my_alert.title = "Unknown error"
                my_alert.message = "Something went wrong. Sorry."
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
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


struct SettingsButton: View {
    // make the buttom open the app's settings / preferences window
    // https://stackoverflow.com/a/65356627/6495494
    let action: () -> Void = {
        // Version specific function call, as this changed in macOS 13 (Ventura)
        // https://developer.apple.com/forums/thread/711940
        if #available(macOS 13, *) {
          NSApp.sendAction(Selector(("showSettingsWindow:")), to: nil, from: nil)
        } else {
          NSApp.sendAction(Selector(("showPreferencesWindow:")), to: nil, from: nil)
        }
    }
    
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


struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
