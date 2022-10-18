#ifndef __my_utils__
#define __my_utils__

// use a c wrapper around c++ code
// https://stackoverflow.com/q/35229149/6495494
#ifdef __cplusplus
extern "C" {
#endif

    // initialize / terminate Adobe XMP SDK
    void initialize_xmp_sdk();
    void terminate_xmp_sdk();

    // function to read a dng image and store its pixel values
    int read_image(const char* in_path, void** pixel_bytes_pointer, int* width, int* height);

    // function to read a dng image, overwrite its pixel values, and save the result
    int write_image(const char *in_path, const char *out_path, void** pixel_bytes_pointer);

#ifdef __cplusplus
}
#endif


#endif
