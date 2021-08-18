/*****************************************************************************/
// Copyright 2007-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#ifndef __dng_preview__
#define __dng_preview__

/*****************************************************************************/

#include "dng_auto_ptr.h"
#include "dng_classes.h"
#include "dng_ifd.h"
#include "dng_opcode_list.h"
#include "dng_point.h"
#include "dng_sdk_limits.h"
#include "dng_uncopyable.h"

/*****************************************************************************/

class dng_preview: private dng_uncopyable
	{
	
	public:
	
		dng_preview_info fInfo;
		
	protected:
	
		dng_preview ();
		
	public:
	
		virtual ~dng_preview ();
		
		virtual dng_basic_tag_set * AddTagSet (dng_tiff_directory &directory) const = 0;
		
		virtual void WriteData (dng_host &host,
								dng_image_writer &writer,
								dng_basic_tag_set &basic,
								dng_stream &stream) const = 0;
		
	};
		
/*****************************************************************************/

class dng_image_preview: public dng_preview
	{
	
	public:
	
		AutoPtr<dng_image> fImage;
		
	private:
		
		mutable dng_ifd fIFD;
		
	public:
	
		dng_image_preview ();
		
		virtual ~dng_image_preview ();
		
		virtual dng_basic_tag_set * AddTagSet (dng_tiff_directory &directory) const;
		
		virtual void WriteData (dng_host &host,
								dng_image_writer &writer,
								dng_basic_tag_set &basic,
								dng_stream &stream) const;
		
	};

/*****************************************************************************/

class dng_jpeg_preview: public dng_preview
	{
	
	public:
	
		dng_point fPreviewSize;
		
		uint16 fPhotometricInterpretation;
		
		dng_point fYCbCrSubSampling;
		
		uint16 fYCbCrPositioning;
		
		AutoPtr<dng_memory_block> fCompressedData;

	public:
	
		dng_jpeg_preview ();
		
		virtual ~dng_jpeg_preview ();
		
		virtual dng_basic_tag_set * AddTagSet (dng_tiff_directory &directory) const;
		
		virtual void WriteData (dng_host &host,
								dng_image_writer &writer,
								dng_basic_tag_set &basic,
								dng_stream &stream) const;
		
		void SpoolAdobeThumbnail (dng_stream &stream) const;
		
	};

/*****************************************************************************/

class dng_raw_preview: public dng_preview
	{
	
	public:
	
		AutoPtr<dng_image> fImage;
		
		AutoPtr<dng_memory_block> fOpcodeList2Data;
  
        real64 fBlackLevel [kMaxSamplesPerPixel];
		
		int32 fCompressionQuality;

	private:
		
		mutable dng_ifd fIFD;
		
	public:
	
		dng_raw_preview ();
		
		virtual ~dng_raw_preview ();
		
		virtual dng_basic_tag_set * AddTagSet (dng_tiff_directory &directory) const;
		
		virtual void WriteData (dng_host &host,
								dng_image_writer &writer,
								dng_basic_tag_set &basic,
								dng_stream &stream) const;
		
	};

/*****************************************************************************/

class dng_depth_preview: public dng_preview
	{
	
	public:
	
		AutoPtr<dng_image> fImage;
		
		int32 fCompressionQuality;
  
        bool fFullResolution;

	private:
		
		mutable dng_ifd fIFD;
		
	public:
	
		dng_depth_preview ();
		
		virtual ~dng_depth_preview ();
		
		virtual dng_basic_tag_set * AddTagSet (dng_tiff_directory &directory) const;
		
		virtual void WriteData (dng_host &host,
								dng_image_writer &writer,
								dng_basic_tag_set &basic,
								dng_stream &stream) const;
		
	};

/*****************************************************************************/

class dng_mask_preview: public dng_preview
    {
    
    public:
    
        AutoPtr<dng_image> fImage;
        
        int32 fCompressionQuality;

    private:
        
        mutable dng_ifd fIFD;
        
    public:
    
        dng_mask_preview ();
        
        virtual ~dng_mask_preview ();
        
        virtual dng_basic_tag_set * AddTagSet (dng_tiff_directory &directory) const;
        
        virtual void WriteData (dng_host &host,
                                dng_image_writer &writer,
                                dng_basic_tag_set &basic,
                                dng_stream &stream) const;
        
    };

/*****************************************************************************/

class dng_preview_list
	{
	
	private:
	
		uint32 fCount;
		
		AutoPtr<dng_preview> fPreview [kMaxDNGPreviews];
		
	public:
	
		dng_preview_list ();
		
		~dng_preview_list ();
		
		uint32 Count () const
			{
			return fCount;
			}
			
		const dng_preview & Preview (uint32 index) const
			{
			return *(fPreview [index]);
			}
			
		void Append (AutoPtr<dng_preview> &preview);
	
	};

/*****************************************************************************/

#endif
	
/*****************************************************************************/
