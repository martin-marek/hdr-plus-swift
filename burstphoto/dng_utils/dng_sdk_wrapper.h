#ifndef __dng_sdk_wrapper__
#define __dng_sdk_wrapper__

// use a c wrapper around c++ code
// https://stackoverflow.com/q/35229149/6495494
#ifdef __cplusplus
extern "C" {
#endif

    // initialize / terminate Adobe XMP SDK
    void initialize_xmp_sdk();
    void terminate_xmp_sdk();

    // function to read a dng image and store its pixel values
    int read_image(const char* in_path, void** pixel_bytes_pointer, int* width, int* height, int* mosaic_pattern_width, int* white_level, int* black_level0, int* black_level1, int* black_level2, int* black_level3, int* exposure_bias, float* color_factor_r, float* color_factor_g, float* color_factor_b);

    // function to read a dng image, overwrite its pixel values, and save the result
    int write_image(const char *in_path, const char *out_path, void** pixel_bytes_pointer, const int white_level);

#ifdef __cplusplus
}
#endif


#endif
