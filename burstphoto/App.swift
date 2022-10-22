import SwiftUI
import AppKit


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

@main
struct burstphotoApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    
    var body: some Scene {
        WindowGroup {
            ContentView()
                .onReceive(NotificationCenter.default.publisher(for: NSApplication.willUpdateNotification), perform: { _ in disable_window_resizing()})
                .onAppear {initialize_xmp_sdk()}
                .onDisappear {terminate_xmp_sdk()}
        }
        .windowStyle(HiddenTitleBarWindowStyle())
        .commands {
            CommandGroup(replacing: .newItem, addition: {}) // disables creating any new windows
            CommandGroup(replacing: .help, addition: {}) // disables help menu
        }
        Settings {
            SettingsView()
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
