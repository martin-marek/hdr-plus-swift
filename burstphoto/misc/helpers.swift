import Foundation

/// Returns the amount of free disk space left on the system.
/// 
/// Based on https://stackoverflow.com/questions/36006713/how-to-get-the-total-disk-space-and-free-disk-space-using-attributesoffilesystem
func systemFreeDiskSpace() -> Double {
    let paths = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)

    guard
        let lastPath = paths.last,
        let attributeDictionary = try? FileManager.default.attributesOfFileSystem(forPath: lastPath)
    else {
        return 0.0
    }

    if let size = attributeDictionary[.systemFreeSize] as? NSNumber {
        return Double(size.int64Value) / 1000 / 1000 / 1000
    } else {
        return 0.0
    }
}
