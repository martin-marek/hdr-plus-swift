/// Functions related to handling of exposure differences in bracketed bursts.
import Foundation
import MetalPerformanceShaders

let correct_exposure_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "correct_exposure")!)
let correct_exposure_linear_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "correct_exposure_linear")!)
let max_x_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "max_x")!)
let max_y_state = try! device.makeComputePipelineState(function: mfl.makeFunction(name: "max_y")!)

/// Apply tone mapping if the reference image is underexposed.
/// A curve is applied to lift the shadows and protect the highlights from burning.
/// By lifting the shadows they suffer less from quantization errors, this is especially beneficial as the bit-depth of the image decreases.
///
/// Inspired by https://www-old.cs.utah.edu/docs/techreports/2002/pdf/UUCS-02-001.pdf
func correct_exposure(_ final_texture: MTLTexture, _ white_level: Int, _ black_level: [[Int]], _ exposure_control: String, _ exposure_bias: [Int], _ uniform_exposure: Bool, _ color_factors: [[Double]], _ ref_idx: Int) {
              
    // only apply exposure correction if reference image has an exposure, which is lower than the target exposure
    if (exposure_control != "Off" && white_level != -1 && black_level[0][0] != -1) {
          
        var final_texture_blurred = blur(final_texture, with_pattern_width: 2, using_kernel_size: 2)
        let max_texture_buffer = texture_max(final_texture_blurred)
        
        // find index of image with longest exposure to use the most robust black level value
        var exp_idx = 0
        for comp_idx in 0..<exposure_bias.count {
             if (exposure_bias[comp_idx] > exposure_bias[exp_idx]) {
                exp_idx = comp_idx
            }
        }
        
        var black_level0 = 0.0
        var black_level1 = 0.0
        var black_level2 = 0.0
        var black_level3 = 0.0
        
        // if exposure levels are uniform, calculate mean value of all exposures
        if uniform_exposure {
            
            for comp_idx in 0..<exposure_bias.count {
                black_level0 += Double(black_level[comp_idx][0])
                black_level1 += Double(black_level[comp_idx][1])
                black_level2 += Double(black_level[comp_idx][2])
                black_level3 += Double(black_level[comp_idx][3])
            }
            
            black_level0 /= Double(exposure_bias.count)
            black_level1 /= Double(exposure_bias.count)
            black_level2 /= Double(exposure_bias.count)
            black_level3 /= Double(exposure_bias.count)
            
        } else {
            black_level0 = Double(black_level[exp_idx][0])
            black_level1 = Double(black_level[exp_idx][1])
            black_level2 = Double(black_level[exp_idx][2])
            black_level3 = Double(black_level[exp_idx][3])
        }
        
        let command_buffer = command_queue.makeCommandBuffer()!
        command_buffer.label = "Correct Exposure"
        let command_encoder = command_buffer.makeComputeCommandEncoder()!
        let state = (exposure_control=="Curve0EV" || exposure_control=="Curve1EV") ? correct_exposure_state : correct_exposure_linear_state
        command_encoder.setComputePipelineState(state)
        let threads_per_grid = MTLSize(width: final_texture.width, height: final_texture.height, depth: 1)
        let threads_per_thread_group = get_threads_per_thread_group(state, threads_per_grid)
       
        if (exposure_control=="Curve0EV" || exposure_control=="Curve1EV") {
            let color_factor_mean = 0.25*(color_factors[ref_idx][0]+2.0*color_factors[ref_idx][1]+color_factors[ref_idx][2])
            
            // the blurred texture serves as an approximation of local luminance
            final_texture_blurred = blur(final_texture, with_pattern_width: 1, using_kernel_size: 1)
            
            command_encoder.setTexture(final_texture_blurred, index: 0)
            command_encoder.setTexture(final_texture, index: 1)
            command_encoder.setBytes([Int32(exposure_bias[ref_idx])], length: MemoryLayout<Int32>.stride, index: 0)
            command_encoder.setBytes([Int32(exposure_control=="Curve0EV" ? 0 : 100)], length: MemoryLayout<Int32>.stride, index: 1)
            command_encoder.setBytes([Float32(white_level)], length: MemoryLayout<Float32>.stride, index: 2)
            command_encoder.setBytes([Float32(black_level0)], length: MemoryLayout<Float32>.stride, index: 3)
            command_encoder.setBytes([Float32(black_level1)], length: MemoryLayout<Float32>.stride, index: 4)
            command_encoder.setBytes([Float32(black_level2)], length: MemoryLayout<Float32>.stride, index: 5)
            command_encoder.setBytes([Float32(black_level3)], length: MemoryLayout<Float32>.stride, index: 6)
            command_encoder.setBytes([Float32(color_factor_mean)], length: MemoryLayout<Float32>.stride, index: 7)
            command_encoder.setBuffer(max_texture_buffer, offset: 0, index: 8)
        } else {
            command_encoder.setTexture(final_texture, index: 0)
            command_encoder.setBytes([Float32(white_level)], length: MemoryLayout<Float32>.stride, index: 0)
            command_encoder.setBytes([Float32(black_level0)], length: MemoryLayout<Float32>.stride, index: 1)
            command_encoder.setBytes([Float32(black_level1)], length: MemoryLayout<Float32>.stride, index: 2)
            command_encoder.setBytes([Float32(black_level2)], length: MemoryLayout<Float32>.stride, index: 3)
            command_encoder.setBytes([Float32(black_level3)], length: MemoryLayout<Float32>.stride, index: 4)
            command_encoder.setBuffer(max_texture_buffer, offset: 0, index: 5)
            command_encoder.setBytes([Float32(exposure_control=="LinearFullRange" ? -1.0 : 2.0)], length: MemoryLayout<Float32>.stride, index: 6)
        }
        command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
        command_encoder.endEncoding()
        command_buffer.commit()
    }
}


/// Calculate the maximum value of the texture.
/// This is used for adjusting the exposure of the final image in order to prevent channels from being clipped.
func texture_max(_ in_texture: MTLTexture) -> MTLBuffer {
    
    // create a 1d texture that will contain the maxima of the input texture along the x-axis
    let texture_descriptor = MTLTextureDescriptor()
    texture_descriptor.textureType = .type1D
    texture_descriptor.pixelFormat = in_texture.pixelFormat
    texture_descriptor.width = in_texture.width
    texture_descriptor.usage = [.shaderRead, .shaderWrite]
    texture_descriptor.storageMode = .private
    let max_y = device.makeTexture(descriptor: texture_descriptor)!
    max_y.label = "\(in_texture.label!.components(separatedBy: ":")[0]): Max y"
    
    // average the input texture along the y-axis
    let command_buffer = command_queue.makeCommandBuffer()!
    command_buffer.label = "Texture Max"
    let command_encoder = command_buffer.makeComputeCommandEncoder()!
    let state = max_y_state
    command_encoder.setComputePipelineState(state)
    let threads_per_grid = MTLSize(width: in_texture.width, height: 1, depth: 1)
    let max_threads_per_thread_group = state.threadExecutionWidth
    let threads_per_thread_group = MTLSize(width: max_threads_per_thread_group, height: 1, depth: 1)
    command_encoder.setTexture(in_texture, index: 0)
    command_encoder.setTexture(max_y, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    
    // average the generated 1d texture along the x-axis
    let state2 = max_x_state
    command_encoder.setComputePipelineState(state2)
    let max_buffer = device.makeBuffer(length: MemoryLayout<Float32>.size, options: .storageModeShared)!
    max_buffer.label = "\(in_texture.label!.components(separatedBy: ":")[0]): Max"
    command_encoder.setTexture(max_y, index: 0)
    command_encoder.setBuffer(max_buffer, offset: 0, index: 0)
    command_encoder.setBytes([Int32(in_texture.width)], length: MemoryLayout<Int32>.stride, index: 1)
    command_encoder.dispatchThreads(threads_per_grid, threadsPerThreadgroup: threads_per_thread_group)
    command_encoder.endEncoding()
    command_buffer.commit()
    
    // return the average of all pixels in the input array
    return max_buffer
}
