import SwiftUI


enum AppState {
    case main, processing, image_saved
}

struct MyAlert {
    var show: Bool = false
    var title: String?
    var message: String?
    var dismiss_button: Alert.Button?
}


struct ContentView: View {
    @ObservedObject var settings: AppSettings
    @State private var app_state = AppState.main
    @State private var image_urls: [URL] = []
    @StateObject var progress = ProcessingProgress()
    @State var my_alert = MyAlert()
    @State private var out_url = URL(fileURLWithPath: "")
    @State var drop_active = false
    
    var body: some View {
        let drop_delegate = MyDropDelegate(settings: settings, app_state: $app_state, image_urls: $image_urls, progress: progress, active: $drop_active, my_alert: $my_alert, out_url: $out_url)
        
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
                dismissButton: my_alert.dismiss_button!
            )
        })
        .onReceive(progress.$show_nonbayer_hq_alert, perform: { val in
            if val {
                my_alert.title = "Higher quality not supported"
                my_alert.message = "You have selected the \"Higher quality\" merging algorithm in Preferences but your camera only supports the \"Fast\" algorithm. Press OK to use the \"Fast\" algorithm."
                my_alert.dismiss_button = .default(Text("OK"))
                my_alert.show = true
                settings.merging_algorithm = "Fast"
            }
        })
        .onReceive(progress.$show_nonbayer_exposure_alert, perform: { val in
            if val {
                my_alert.title = "Exposure control not supported"
                my_alert.message = "You have selected exposure control other than \"Off\" in Preferences, which is not supported for your camera. Press OK to disable exposure control."
                my_alert.dismiss_button = .default(Text("OK"))
                my_alert.show = true
                settings.exposure_control = " Off"
            }
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
            
            Text("Drag & drop a burst of RAW images")
                .multilineTextAlignment(.center)
                .font(.system(size: 20, weight: .medium))
                .opacity(0.8)
                .frame(width: 200)
                .padding()
            
            Spacer()
            
            DropIcon(drop_active: $drop_active)
            
            Spacer()
            
            Text("*.DNG, *.ARW, *.NEF, *.CR2…")
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
        
        if progress.includes_conversion && progress.int < 10000000 {
            return "Converting images to DNG (this might take a while)..."
        } else if progress.int < 20000000 {
            return "Loading images..."
        } else if progress.int < 100000000 {
            
            // use a very high number for the 100% mark to minimize any rounding errors
            let percent = round(Double(progress.int-20000000)/800000*10)/10.0
                       
            return "Processing images (\(percent)%)..."
            
        } else {
            return "Saving processed image..."
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
    @ObservedObject var settings: AppSettings
    let tile_sizes = ["Small", "Medium", "Large"]
    let search_distances = ["Small", "Medium", "Large"]
    let merging_algorithms = ["Fast", "Higher quality"]
    let exposure_controls = [" Off", " Linear (full bit range)", " Linear (relative +1 EV)", " Non-linear (target ±0 EV)", " Non-linear (target +1 EV)"]
    
    @State private var user_changing_nr = false
    @State private var skip_haptic_feedback = false
     
    var body: some View {
        VStack {
            
            HStack {            
                Text("Tile size").font(.system(size: 14, weight: .medium))
                Spacer()
                Picker(selection: settings.$tile_size, label: EmptyView()) {
                    ForEach(tile_sizes, id: \.self) {
                        Text($0)
                    }
                }.pickerStyle(SegmentedPickerStyle()).frame(width: 222)
            }.padding(.horizontal, 15).padding(.top, 20).padding(.bottom, 11)
            
            HStack {
                Text("Search distance").font(.system(size: 14, weight: .medium))
                Spacer()
                Picker(selection: settings.$search_distance, label: EmptyView()) {
                    ForEach(search_distances, id: \.self) {
                        Text($0)
                    }
                }.pickerStyle(SegmentedPickerStyle()).frame(width: 222)
            }.padding(.horizontal, 15).padding(.vertical, 11)
                     
            HStack {
                Text("Merging algorithm").font(.system(size: 14, weight: .medium))
                Spacer()
                Picker(selection: settings.$merging_algorithm, label: EmptyView()) {
                    ForEach(merging_algorithms, id: \.self) {
                        Text($0)
                    }
                }.pickerStyle(SegmentedPickerStyle()).frame(width: 222)
            }.padding(.horizontal, 15).padding(.vertical, 11)
            
            HStack {
                Text("Exposure control").font(.system(size: 14, weight: .medium))
                Spacer()
                Picker(selection: settings.$exposure_control, label: EmptyView()) {
                    ForEach(exposure_controls, id: \.self) {
                        Text($0)
                    }
                }.pickerStyle(MenuPickerStyle()).frame(width: 222)
            }.padding(.horizontal, 15).padding(.vertical, 11)
          
            VStack(alignment: .leading) {
                
                (Text("Noise reduction: ") +
                 (settings.noise_reduction == 23
                   ? Text("max. ").foregroundColor(.accentColor) + Text("(simple average w/o alignment)")
                   : Text("\(Int(settings.noise_reduction))")
                )).font(.system(size: 14, weight: .medium))
                .opacity(user_changing_nr ? 0.75 : 1.0)
                    
                // the slider/stepper should provide haptic feedback when value changes
                // but there's one exception: we don't provide feedback on the first click of the stepper,
                // to avoid providing haptic feedback together with a system click
                HStack {
                    Slider(value: settings.$noise_reduction, in: 1...23, step: 1,
                        onEditingChanged: { editing in user_changing_nr = editing }
                    ).onChange(of: settings.noise_reduction) { _ in
                        if !skip_haptic_feedback {
                            NSHapticFeedbackManager.defaultPerformer.perform(.alignment, performanceTime: .now)
                        }
                        skip_haptic_feedback = false
                    }
                    Stepper("", value: settings.$noise_reduction, in: 1...23,
                        onEditingChanged: { editing in
                            user_changing_nr = editing
                            skip_haptic_feedback = editing // avoid proving haptic feedback on the first click
                    })
                }
                Text("  <      daylight scene      >          ....        <     night scene     >")
                    .font(.system(size: 12))
                    .foregroundColor(.primary)
                Text("")
                Text("Small values increase motion robustness and image sharpness")
                    .font(.system(size: 12))
                    .foregroundColor(.secondary)
                Text("Large values increase the strength of noise reduction")
                    .font(.system(size: 12))
                    .foregroundColor(.secondary)
            }.padding(.horizontal, 15).padding(.top, 11).padding(.bottom, 15)
            
        }
        .frame(width: 400)
        .navigationTitle("Preferences")
    }
}


struct MyDropDelegate: DropDelegate {
    @ObservedObject var settings: AppSettings
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

        // check input parameter tile size
        if settings.tile_size != "Small" && settings.tile_size != "Medium" && settings.tile_size != "Large" {
           settings.tile_size = "Medium"
        }
        
        // check input parameter search distance
        if settings.search_distance != "Small" && settings.search_distance != "Medium" && settings.search_distance != "Large" {
            settings.search_distance = "Medium"
        }
        
        // check input parameter merging algorithm
        if settings.merging_algorithm != "Fast" && settings.merging_algorithm != "Higher quality" {
            settings.merging_algorithm = "Fast"
        }
        
        // check input parameter exposure control
        if settings.exposure_control != " Off" && settings.exposure_control != " Linear (full bit range)" && settings.exposure_control != " Linear (relative +1 EV)" && settings.exposure_control != " Non-linear (target ±0 EV)" && settings.exposure_control != " Non-linear (target +1 EV)" {
            settings.exposure_control = " Linear (full bit range)"
        }
        
        // check input parameter noise reduction
        if settings.noise_reduction < 1 || settings.noise_reduction > 23 {
            settings.noise_reduction = 13
        }
        
        // set simplified value for parameter exposure control
        let exposure_control_dict = [
            " Off"                       : "Off",
            " Linear (full bit range)"   : "LinearFullRange",
            " Linear (relative +1 EV)"   : "Linear1EV",
            " Non-linear (target ±0 EV)" : "Curve0EV",
            " Non-linear (target +1 EV)" : "Curve1EV",
        ]
        let exposure_control_short = exposure_control_dict[settings.exposure_control]!
        
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
                // align and merge the burst
                out_url = try perform_denoising(image_urls: image_urls, progress: progress, merging_algorithm: settings.merging_algorithm, tile_size: settings.tile_size, search_distance: settings.search_distance, noise_reduction: settings.noise_reduction, exposure_control: exposure_control_short)
                   
                // inform the user about the saved image
                app_state = .image_saved

            } catch ImageIOError.load_error {
                my_alert.title = "Unsupported format"
                my_alert.message = "Image format not supported. Please only use unprocessed RAW or DNG images. Using RAW images requires Adobe DNG Converter to be installed on your Mac."
                my_alert.dismiss_button = .cancel()
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch ImageIOError.save_error {
                my_alert.title = "Image could not be saved"
                my_alert.message = "The processed image could not be saved for an unknown reason. Sorry."
                my_alert.dismiss_button = .cancel()
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.less_than_two_images {
                my_alert.title = "Burst required"
                my_alert.message = "Please drag & drop at least 2 images."
                my_alert.dismiss_button = .cancel()
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.inconsistent_extensions {
                my_alert.title = "Inconsistent formats"
                my_alert.message = "Please make sure that all images have the same format."
                my_alert.dismiss_button = .cancel()
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.inconsistent_resolutions {
                my_alert.title = "Inconsistent resolution"
                my_alert.message = "Please make sure that all images have the same resolution."
                my_alert.dismiss_button = .cancel()
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.missing_dng_converter {
                my_alert.title = "Missing Adobe DNG Converter"
                my_alert.message = "Only DNG files are supported natively. If you wish to use other RAW formats, please download and install Adobe DNG Converter. Burst Photo will then be able to process most RAW formats automatically."
                my_alert.dismiss_button = .cancel()
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.conversion_failed {
                my_alert.title = "Conversion failed"
                my_alert.message = "Image format not supported. Please only use unprocessed RAW or DNG images."
                my_alert.dismiss_button = .cancel()
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch AlignmentError.non_bayer_exposure_bracketing {
                my_alert.title = "Unsupported exposure bracketing"
                my_alert.message = "Exposure bracketing is not supported for your camera. Please only use images with uniform exposure."
                my_alert.dismiss_button = .cancel()
                my_alert.show = true
                DispatchQueue.main.async { app_state = .main }
            } catch {
                my_alert.title = "Unknown error"
                my_alert.message = "Something went wrong. Sorry."
                my_alert.dismiss_button = .cancel()
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
