// simple list
List {
    Text("Element 1")//.listRowBackground(Color.red)
    Text("Element 2")
    Text("Element 3")
}

// more advanced list
List(0..<5) { item in
    Image(systemName: "photo")
    VStack(alignment: .leading) {
        Text("Simon Ng")
        Text("Founder of AppCoda")
            .font(.subheadline)
            // .color(Color.gray)
    }
    .listRowBackground(Color.red)
}

// table (supported in macOS >=12)
// https://developer.apple.com/documentation/swiftui/table
// https://stackoverflow.com/questions/61862802/table-with-alternating-row-colors-in-swiftui
struct Person: Identifiable {
    let givenName: String
    let familyName: String
    let id = UUID()
}
private var people = [
    Person(givenName: "Juan", familyName: "Chavez"),
    Person(givenName: "Mei", familyName: "Chen"),
    Person(givenName: "Tom", familyName: "Clark"),
    Person(givenName: "Gita", familyName: "Kumar"),
]
Table(people) {
    TableColumn("Given Name", value: \.givenName)
    TableColumn("Family Name", value: \.familyName)
}
.listStyle(.inset(alternatesRowBackgrounds: true)) // should work in macOS 12
.listStyle(.bordered(alternatesRowBackgrounds: true)) // should work in macOS 12
.background(Rectangle().fill(Color.gray))
.background(index % 2 == 0 ? Color.blue : Color.red)

// grid cell
GridCell(url: imageUrls[0])
struct GridCell: View {
    let url: URL?
    
    var body: some View {
        // Image(nsImage: url != nil ? NSImage(byReferencing: url!) : NSImage())
        // Image(nsImage: NSImage(byReferencing: URL(fileURLWithPath: "/Usmartinmarek/Google Drive/Burst/burstphoto/burstphoto/drop_icon.png")))
        Image(nsImage: NSImage(named: NSImage.Name("drop_icon"))!)
            .resizable()
            .frame(width: 150, height: 150)
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


struct MyButtonStyle: ButtonStyle {
    func makeBody(configuration: Self.Configuration) -> some View {
        configuration.label
            .foregroundColor(configuration.isPressed ? Color.accentColor : Color.white)
            .background(configuration.isPressed ? Color.white : Color.accentColor)
            .cornerRadius(6.0)
            .padding()
    }
}
