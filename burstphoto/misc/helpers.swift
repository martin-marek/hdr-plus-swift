import Foundation
import MetalPerformanceShaders

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

func create_pipeline(with_function_name function_name: String, and_label label: String) -> MTLComputePipelineState {
    let _descriptor = MTLComputePipelineDescriptor()
    _descriptor.computeFunction = mfl.makeFunction(name: function_name)
    _descriptor.label = label
    return try! device.makeComputePipelineState(descriptor: _descriptor, options: []).0
}
