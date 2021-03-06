#include "my_utils.h"
#include "dng_exceptions.h"
#include "dng_file_stream.h"
#include "dng_host.h"
#include "dng_ifd.h"
#include "dng_image_writer.h"
#include "dng_info.h"
#include "dng_negative.h"
#include "dng_simple_image.h"
#include "dng_xmp_sdk.h"


int read_image(const char* in_path, void** pixel_bytes_pointer, int* width, int* height) {
    
    try {
        
        // initialize the sdk
        dng_xmp_sdk::InitializeSDK();
        
        // read image
        dng_host host;
        dng_info info;
        dng_file_stream stream(in_path);
        AutoPtr<dng_negative> negative; {
            info.Parse(host, stream);
            info.PostParse(host);
            if(!info.IsValidDNG()) {return dng_error_bad_format;}
            negative.Reset(host.Make_dng_negative());
            negative->Parse(host, stream, info);
            negative->PostParse(host, stream, info);
        }
        
        // load pixel buffer
        // - it would be possible to do:
        //   >>> negative->ReadStage1Image(host, stream, info);
        //   however, ReadStage1Image will create a dng_simple_image and convert it to
        //   a dng_image. For our use case, we require a dng_simple_image,
        //   hence I have reimplemented dng_negative::ReadStage1Image here.
        dng_ifd& rawIFD = *info.fIFD [info.fMainIndex];
        AutoPtr<dng_simple_image> image_pointer (new dng_simple_image(rawIFD.Bounds(), rawIFD.fSamplesPerPixel, rawIFD.PixelType(), host.Allocator()));
        dng_simple_image& image = *image_pointer.Get();
        rawIFD.ReadImage(host, stream, image);
        dng_pixel_buffer pixel_buffer;
        image.GetPixelBuffer(pixel_buffer);
        
        // copy data
        *width = image.Width();
        *height = image.Height();
        int image_size = image.Width() * image.Height() * image.PixelSize();
        void* pixel_bytes = malloc(image_size);
        *pixel_bytes_pointer = pixel_bytes;
        memcpy(pixel_bytes, pixel_buffer.DirtyPixel(0, 0), image_size);
        
        // terminate the sdk
        dng_xmp_sdk::TerminateSDK();
        
    }
    catch(...) {
        dng_xmp_sdk::TerminateSDK();
        return 1;
    }
    return 0;
}

int write_image(const char *in_path, const char *out_path, void** pixel_bytes_pointer) {
    
    try {
        
        // initialize the sdk
        dng_xmp_sdk::InitializeSDK();
        
        // read image
        dng_host host;
        dng_info info;
        dng_file_stream stream(in_path);
        AutoPtr<dng_negative> negative; {
            info.Parse(host, stream);
            info.PostParse(host);
            if(!info.IsValidDNG()) {return dng_error_bad_format;}
            negative.Reset(host.Make_dng_negative());
            negative->Parse(host, stream, info);
            negative->PostParse(host, stream, info);
        }
        
        // load pixel buffer
        dng_ifd& rawIFD = *info.fIFD [info.fMainIndex];
        AutoPtr<dng_simple_image> image_pointer (new dng_simple_image(rawIFD.Bounds(), rawIFD.fSamplesPerPixel, rawIFD.PixelType(), host.Allocator()));
        dng_simple_image& image = *image_pointer.Get();
        // rawIFD.ReadImage(host, stream, image);
        
        // overwrite pixel buffer
        void* pixel_bytes = *pixel_bytes_pointer;
        int image_size = image.Width() * image.Height() * image.PixelSize();
        memcpy(image.fBuffer.DirtyPixel(0, 0), pixel_bytes, image_size);
        
        // store modified pixel buffer to the negative
        negative->fStage1Image.Reset(image_pointer.Release());
        
        // validate the modified image
        // - this resets some of the image stats like md5 checksums
        // - running this function will print a warning "NewRawImageDigest does not match raw image"
        //   but won't halt the program
        // - without running this function, the output dng file would be considered 'damaged'
        negative->ValidateRawImageDigest(host);
        
        // read metadata
        // - this doesn't seem to affect my test dng files but maybe it makes
        //   a difference for other files
        // - this is used in the dng_validate script
        negative->SynchronizeMetadata();
            
        // write dng
        host.SetSaveDNGVersion(dngVersion_SaveDefault);
        host.SetSaveLinearDNG(false);
        host.SetKeepOriginalFile(false);
        dng_file_stream stream2(out_path, true); {
            dng_image_writer writer;
            writer.WriteDNG(host, stream2, *negative.Get());
        }
        
        // terminate the sdk
        dng_xmp_sdk::TerminateSDK();

    }
    catch(...) {
        dng_xmp_sdk::TerminateSDK();
        return 1;
    }
    return 0;
}
