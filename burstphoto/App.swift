import SwiftUI
import AppKit

class AppSettings: ObservableObject {
    @AppStorage("tile_size") var tile_size: String = "Medium"
    @AppStorage("search_distance") var search_distance: String = "Medium"
    @AppStorage("merging_algorithm") var merging_algorithm: String = "Fast"
    @AppStorage("noise_reduction") var noise_reduction: Double = 13.0
    @AppStorage("exposure_control") var exposure_control: String = " Linear (full bit range)"
    @AppStorage("output_bit_depth") var output_bit_depth: String = "Native"
}

class AppDelegate: NSObject, NSApplicationDelegate {
    
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        // ensures that the app is terminated when the window is closed
        // https://stackoverflow.com/a/65743682/6495494
        return true
    }
    
    func applicationDidFinishLaunching(_ notification: Notification) {
        // ensure that tabs cannot be created
        // let _ = NSApplication.shared.windows.map { $0.tabbingMode = .disallowed }
        NSWindow.allowsAutomaticWindowTabbing = false
    }
}

extension Scene {
    // disables window resizability on macos 13
    // https://developer.apple.com/forums/thread/719389?answerId=735997022#735997022
    func windowResizabilityContentSize() -> some Scene {
        if #available(macOS 13.0, *) {
            return windowResizability(.contentSize)
        } else {
            return self
        }
    }
}

@main
struct burstphotoApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    @StateObject var settings = AppSettings()
    
    var body: some Scene {
        WindowGroup {
            ContentView(settings: settings)
                .onReceive(NotificationCenter.default.publisher(for: NSApplication.willUpdateNotification), perform: { _ in disable_window_resizing()})
                .onAppear {initialize_xmp_sdk()}
                .onDisappear {terminate_xmp_sdk()}
        }
        .windowStyle(HiddenTitleBarWindowStyle())
        .windowResizabilityContentSize()
        .commands {
            CommandGroup(replacing: .newItem, addition: {}) // disables creating any new windows
            CommandGroup(replacing: .help) { // open Burst Photo website
                Button(action: {NSWorkspace.shared.open(URL(string: "https://burst.photo/help/")!)}) {
                    Text("Burst Photo Help")
                }
            }
        }
        Settings {
            SettingsView(settings: settings)
        }
    }
    
    func disable_window_resizing() {
        for window in NSApplication.shared.windows {
            // hides the "full screen" green button
            if let zoom_button = window.standardWindowButton(NSWindow.ButtonType.zoomButton) {
                zoom_button.isHidden = true
            }
            
            // disables full screen mode
            window.collectionBehavior = .fullScreenNone
        }
    }
}
