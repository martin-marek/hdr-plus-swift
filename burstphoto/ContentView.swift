import SwiftUI


enum AppState {
    case main, processing, image_saved
}

class AppSettings: ObservableObject {
    @AppStorage("tile_size") static var tile_size: Int = 16
    @AppStorage("search_distance") static var search_distance: String = "Medium"
    @AppStorage("robustness") static var robustness: Double = 0.5
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
            
            Text("Drag & drop a burst of DNG images")
                .multilineTextAlignment(.center)
                .font(.system(size: 20, weight: .medium))
                .opacity(0.8)
                .frame(width: 200)
                .padding()
            
            Spacer()
            
            DropIcon(drop_active: $drop_active)
            
            Spacer()
            
            Text("*.dng, *.DNG")
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
                .foregroundColor(.accentColor)
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
        .frame(width: 350, height: 300)
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
                let out_dir = NSHomeDirectory() + "/Pictures/Burst Photo/"
                let out_path = out_dir + out_filename + ".dng"
                out_url = URL(fileURLWithPath: out_path)
                
                // create output directory
                if !FileManager.default.fileExists(atPath: out_dir) {
                    try FileManager.default.createDirectory(atPath: out_dir, withIntermediateDirectories: true, attributes: nil)
                }
                
                // save the output image
                try texture_to_dng(output_texture, in_url, out_url)

                // inform the user about the saved image
                app_state = .image_saved

            } catch ImageIOError.load_error {
                my_alert.title = "Unsupported format"
                my_alert.message = "Image format not supported. Please use RAW DNG images only, converted directly from RAW files using Adobe Lightroom or Adobe DNG Converter. Avoid using processed (demosaiced) DNG images."
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
                my_alert.message = "The dropped files heve inconsistent formats. Please make sure that all images are DNG files."
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.inconsistent_resolutions {
                my_alert.title = "Inconsistent resolution"
                my_alert.message = "The dropped files heve inconsistent resolutions. Please make sure that all images are DNG files generated directly from camera RAW files using Adobe Lightroom or Adobe DNG Converter."
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


struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
