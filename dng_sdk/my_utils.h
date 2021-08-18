#ifndef __my_utils__
#define __my_utils__

// function to read a dng image and store its pixel values
int read_image(const char* in_path, void** pixel_bytes_pointer, int* width, int* height);

// function to read a dng image, overwrite its pixel values, and save the result
int write_image(const char *in_path, const char *out_path, void** pixel_bytes_pointer);

#endif
