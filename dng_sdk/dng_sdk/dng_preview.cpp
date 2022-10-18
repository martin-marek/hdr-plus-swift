/*****************************************************************************/
// Copyright 2007-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:	Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#include "dng_preview.h"

#include "dng_assertions.h"
#include "dng_image.h"
#include "dng_image_writer.h"
#include "dng_memory.h"
#include "dng_stream.h"
#include "dng_tag_codes.h"
#include "dng_tag_values.h"

/*****************************************************************************/

class dng_preview_tag_set: public dng_basic_tag_set
	{
	
	private:
	
		tag_string fApplicationNameTag;
		
		tag_string fApplicationVersionTag;
		
		tag_string fSettingsNameTag;
		
		dng_fingerprint fSettingsDigest;
		
		tag_uint8_ptr fSettingsDigestTag;
		
		tag_uint32 fColorSpaceTag;
		
		tag_string fDateTimeTag;
		
		tag_real64 fRawToPreviewGainTag;
		
		tag_uint32 fCacheVersionTag;
		
	public:
	
		dng_preview_tag_set (dng_tiff_directory &directory,
							 const dng_preview &preview,
							 const dng_ifd &ifd);
		
		virtual ~dng_preview_tag_set ();
	
	};

/*****************************************************************************/

dng_preview_tag_set::dng_preview_tag_set (dng_tiff_directory &directory,
										  const dng_preview &preview,
										  const dng_ifd &ifd)
										  
	:	dng_basic_tag_set (directory, ifd)
										  
	,	fApplicationNameTag (tcPreviewApplicationName,
							 preview.fInfo.fApplicationName,
							 false)
							 
	,	fApplicationVersionTag (tcPreviewApplicationVersion,
								preview.fInfo.fApplicationVersion,
								false)
								
	,	fSettingsNameTag (tcPreviewSettingsName,
						  preview.fInfo.fSettingsName,
						  false)
						  
	,	fSettingsDigest (preview.fInfo.fSettingsDigest)
	
	,	fSettingsDigestTag (tcPreviewSettingsDigest,
							fSettingsDigest.data,
							16)
							
	,	fColorSpaceTag (tcPreviewColorSpace,
						preview.fInfo.fColorSpace)
						
	,	fDateTimeTag (tcPreviewDateTime,
					  preview.fInfo.fDateTime,
					  true)
	
	,	fRawToPreviewGainTag (tcRawToPreviewGain,
							  preview.fInfo.fRawToPreviewGain)
							  
	,	fCacheVersionTag (tcCacheVersion,
						  preview.fInfo.fCacheVersion)
						 
	{
	
	if (preview.fInfo.fApplicationName.NotEmpty ())
		{
		
		directory.Add (&fApplicationNameTag);
		
		}
		
	if (preview.fInfo.fApplicationVersion.NotEmpty ())
		{
		
		directory.Add (&fApplicationVersionTag);
		
		}
		
	if (preview.fInfo.fSettingsName.NotEmpty ())
		{
		
		directory.Add (&fSettingsNameTag);
		
		}
		
	if (preview.fInfo.fSettingsDigest.IsValid ())
		{
		
		directory.Add (&fSettingsDigestTag);
		
		}
		
	if (preview.fInfo.fColorSpace != previewColorSpace_MaxEnum)
		{
		
		directory.Add (&fColorSpaceTag);
		
		}
		
	if (preview.fInfo.fDateTime.NotEmpty ())
		{
		
		directory.Add (&fDateTimeTag);
		
		}
		
	if (preview.fInfo.fRawToPreviewGain != 1.0)
		{
		
		directory.Add (&fRawToPreviewGainTag);
		
		}
		
	if (preview.fInfo.fCacheVersion != 0)
		{
		
		directory.Add (&fCacheVersionTag);
		
		}
		
	}

/*****************************************************************************/

dng_preview_tag_set::~dng_preview_tag_set ()
	{
	
	}

/*****************************************************************************/

dng_preview::dng_preview ()

	:	fInfo ()
	
	{
	
	}
		
/*****************************************************************************/

dng_preview::~dng_preview ()
	{
	
	}
		
/*****************************************************************************/

dng_image_preview::dng_image_preview ()

	:	fImage ()
	,	fIFD   ()
	
	{
	
	}

/*****************************************************************************/

dng_image_preview::~dng_image_preview ()
	{
	
	}

/*****************************************************************************/

void dng_image_preview::SetIFDInfo (const dng_image &image)
	{
	
	fIFD.fNewSubFileType = fInfo.fIsPrimary ? sfPreviewImage
											: sfAltPreviewImage;
	
	fIFD.fImageWidth  = image.Width	 ();
	fIFD.fImageLength = image.Height ();
	
	fIFD.fSamplesPerPixel = image.Planes ();

	fIFD.fPhotometricInterpretation = fIFD.fSamplesPerPixel == 1 ? piBlackIsZero
																 : piRGB;
																 
	fIFD.fBitsPerSample [0] = TagTypeSize (image.PixelType ()) * 8;
	
	for (uint32 j = 1; j < fIFD.fSamplesPerPixel; j++)
		{
		fIFD.fBitsPerSample [j] = fIFD.fBitsPerSample [0];
		}
		
	fIFD.SetSingleStrip ();
	
	}
		
/*****************************************************************************/

dng_basic_tag_set * dng_image_preview::AddTagSet (dng_tiff_directory &directory) const
	{
	
	return new dng_preview_tag_set (directory, *this, fIFD);
	
	}

/*****************************************************************************/

void dng_image_preview::WriteData (dng_host &host,
								   dng_image_writer &writer,
								   dng_basic_tag_set &basic,
								   dng_stream &stream) const
	{
	
	writer.WriteImage (host,
					   fIFD,
					   basic,
					   stream,
					   *fImage.Get ());
					
	}

/*****************************************************************************/

uint64 dng_image_preview::MaxImageDataByteCount () const
	{
	
	return fIFD.MaxImageDataByteCount ();
	
	}

/*****************************************************************************/

class dng_jpeg_preview_tag_set: public dng_preview_tag_set
	{
	
	private:
	
		dng_urational fCoefficientsData [3];
		
		tag_urational_ptr fCoefficientsTag;
		
		uint16 fSubSamplingData [2];
		
		tag_uint16_ptr fSubSamplingTag;
		
		tag_uint16 fPositioningTag;
		
		dng_urational fReferenceData [6];
		
		tag_urational_ptr fReferenceTag;
		
	public:
	
		dng_jpeg_preview_tag_set (dng_tiff_directory &directory,
								  const dng_jpeg_preview &preview,
								  const dng_ifd &ifd);
		
		virtual ~dng_jpeg_preview_tag_set ();
	
	};

/******************************************************************************/

dng_jpeg_preview_tag_set::dng_jpeg_preview_tag_set (dng_tiff_directory &directory,
													const dng_jpeg_preview &preview,
													const dng_ifd &ifd)
					
	:	dng_preview_tag_set (directory, preview, ifd)
	
	,	fCoefficientsTag (tcYCbCrCoefficients, fCoefficientsData, 3)
	
	,	fSubSamplingTag (tcYCbCrSubSampling, fSubSamplingData, 2)
	
	,	fPositioningTag (tcYCbCrPositioning, ifd.fYCbCrPositioning)
	
	,	fReferenceTag (tcReferenceBlackWhite, fReferenceData, 6)
	
	{
	
	if (ifd.fPhotometricInterpretation == piYCbCr)
		{
	
		fCoefficientsData [0] = dng_urational (299, 1000);
		fCoefficientsData [1] = dng_urational (587, 1000);
		fCoefficientsData [2] = dng_urational (114, 1000);
		
		directory.Add (&fCoefficientsTag);
		
		fSubSamplingData [0] = (uint16) ifd.fYCbCrSubSampleH;
		fSubSamplingData [1] = (uint16) ifd.fYCbCrSubSampleV;

		directory.Add (&fSubSamplingTag);
		
		directory.Add (&fPositioningTag);
		
		fReferenceData [0] = dng_urational (  0, 1);
		fReferenceData [1] = dng_urational (255, 1);
		fReferenceData [2] = dng_urational (128, 1);
		fReferenceData [3] = dng_urational (255, 1);
		fReferenceData [4] = dng_urational (128, 1);
		fReferenceData [5] = dng_urational (255, 1);
		
		directory.Add (&fReferenceTag);
		
		}
	
	}
	
/*****************************************************************************/

dng_jpeg_preview_tag_set::~dng_jpeg_preview_tag_set ()
	{
	
	}
	
/*****************************************************************************/

dng_jpeg_preview::dng_jpeg_preview ()

	:	fCompressedData ()
	
	{
	
	fIFD.fCompression = ccJPEG;
	
	}

/*****************************************************************************/

dng_jpeg_preview::~dng_jpeg_preview ()
	{
	
	}

/*****************************************************************************/

dng_basic_tag_set * dng_jpeg_preview::AddTagSet (dng_tiff_directory &directory) const
	{
	
	return new dng_jpeg_preview_tag_set (directory, *this, fIFD);
	
	}
		
/*****************************************************************************/

void dng_jpeg_preview::WriteData (dng_host &host,
								  dng_image_writer &writer,
								  dng_basic_tag_set &basic,
								  dng_stream &stream) const
	{
	
	if (fCompressedData.Get ())
		{
	
		basic.SetTileOffset (0, stream.Position ());
		
		basic.SetTileByteCount (0, fCompressedData->LogicalSize ());
		
		stream.Put (fCompressedData->Buffer		 (),
					fCompressedData->LogicalSize ());

		if (fCompressedData->LogicalSize () & 1)
			{
			stream.Put_uint8 (0);
			}
			
		}
		
	else
		{
		
		// Force the data to be written using lossy JPEG.
		
		fIFD.fCompression = ccLossyJPEG;
		
		dng_image_preview::WriteData (host,
									  writer,
									  basic,
									  stream);
									  
		// But we still want to use the normal jpeg compression code in the IFD.
		
		fIFD.fCompression = ccJPEG;
		
		}
			
	}
		
/*****************************************************************************/

uint64 dng_jpeg_preview::MaxImageDataByteCount () const
	{
	
	if (fCompressedData.Get ())
		{
	
		return RoundUp2 (fCompressedData->LogicalSize ());
		
		}
		
	else
		{
		
		return dng_image_preview::MaxImageDataByteCount ();
		
		}
	
	}

/*****************************************************************************/

void dng_jpeg_preview::SpoolAdobeThumbnail (dng_stream &stream) const
	{
	
	DNG_ASSERT (fCompressedData.Get (),
				"SpoolAdobeThumbnail: no data");
	
	DNG_ASSERT (fIFD.fPhotometricInterpretation == piYCbCr,
				"SpoolAdobeThumbnail: Non-YCbCr");
	
	uint32 compressedSize = fCompressedData->LogicalSize ();
	
	stream.Put_uint32 (DNG_CHAR4 ('8','B','I','M'));
	stream.Put_uint16 (1036);
	stream.Put_uint16 (0);
	
	stream.Put_uint32 (compressedSize + 28);
	
	uint32 widthBytes = (fIFD.fImageWidth * 24 + 31) / 32 * 4;
	
	stream.Put_uint32 (1);
	stream.Put_uint32 (fIFD.fImageWidth);
	stream.Put_uint32 (fIFD.fImageLength);
	stream.Put_uint32 (widthBytes);
	stream.Put_uint32 (widthBytes * fIFD.fImageLength);
	stream.Put_uint32 (compressedSize);
	stream.Put_uint16 (24);
	stream.Put_uint16 (1);
	
	stream.Put (fCompressedData->Buffer (),
				compressedSize);
				
	if (compressedSize & 1)
		{
		stream.Put_uint8 (0);
		}
	
	}
		
/*****************************************************************************/

class dng_raw_preview_tag_set: public dng_preview_tag_set
	{
	
	private:
	
		tag_data_ptr fOpcodeList2Tag;
		
		tag_uint32_ptr fWhiteLevelTag;
		
		uint32 fWhiteLevelData [kMaxColorPlanes];
  
		tag_urational_ptr fBlackLevelTag;
		
		dng_urational fBlackLevelData [kMaxColorPlanes];
		
	public:
	
		dng_raw_preview_tag_set (dng_tiff_directory &directory,
								 const dng_raw_preview &preview,
								 const dng_ifd &ifd);
		
		virtual ~dng_raw_preview_tag_set ();
	
	};

/*****************************************************************************/

dng_raw_preview_tag_set::dng_raw_preview_tag_set (dng_tiff_directory &directory,
												  const dng_raw_preview &preview,
												  const dng_ifd &ifd)
										  
	:	dng_preview_tag_set (directory, preview, ifd)
	
	,	fOpcodeList2Tag (tcOpcodeList2,
						 ttUndefined,
						 0,
						 NULL)
						 
	,	fWhiteLevelTag (tcWhiteLevel,
						fWhiteLevelData,
						preview.fImage->Planes ())

	,	fBlackLevelTag (tcBlackLevel,
						fBlackLevelData,
						preview.fImage->Planes ())
						
	{
									 
	if (preview.fOpcodeList2Data.Get ())
		{
		
		fOpcodeList2Tag.SetData	 (preview.fOpcodeList2Data->Buffer		());
		fOpcodeList2Tag.SetCount (preview.fOpcodeList2Data->LogicalSize ());
		
		directory.Add (&fOpcodeList2Tag);
		
		}
		
	if (preview.fImage->PixelType () == ttFloat)
		{
		
		for (uint32 j = 0; j < kMaxColorPlanes; j++)
			{
			fWhiteLevelData [j] = 32768;
			}
			
		directory.Add (&fWhiteLevelTag);
		
		}
  
	else
		{
		
		bool nonZeroBlack = false;
		
		for (uint32 j = 0; j < preview.fImage->Planes (); j++)
			{
			
			fBlackLevelData [j].Set_real64 (preview.fBlackLevel [j], 1);
			
			nonZeroBlack = nonZeroBlack || (preview.fBlackLevel [j] != 0.0);
			
			}
			
		if (nonZeroBlack)
			{
			
			directory.Add (&fBlackLevelTag);

			}
		
		}
		
	}

/*****************************************************************************/

dng_raw_preview_tag_set::~dng_raw_preview_tag_set ()
	{
	
	}

/*****************************************************************************/

dng_raw_preview::dng_raw_preview ()

	:	fImage				()
	,	fOpcodeList2Data	()
	,	fCompressionQuality (-1)
	,	fIFD				()
	
	{
 
	for (uint32 n = 0; n < kMaxColorPlanes; n++)
		{
		fBlackLevel [n] = 0.0;
		}
	
	}

/*****************************************************************************/

dng_raw_preview::~dng_raw_preview ()
	{
	
	}

/*****************************************************************************/

dng_basic_tag_set * dng_raw_preview::AddTagSet (dng_tiff_directory &directory) const
	{
	
	fIFD.fNewSubFileType = sfPreviewImage;
	
	fIFD.fImageWidth  = fImage->Width  ();
	fIFD.fImageLength = fImage->Height ();
	
	fIFD.fSamplesPerPixel = fImage->Planes ();
	
	fIFD.fPhotometricInterpretation = piLinearRaw;
	
	if (fImage->PixelType () == ttFloat)
		{
		
		fIFD.fCompression = ccDeflate;
		
		fIFD.fCompressionQuality = fCompressionQuality;

		fIFD.fPredictor = cpFloatingPoint;
		
		for (uint32 j = 0; j < fIFD.fSamplesPerPixel; j++)
			{
			fIFD.fBitsPerSample [j] = 16;
			fIFD.fSampleFormat	[j] = sfFloatingPoint;
			}
			
		fIFD.FindTileSize (512 * 1024);
		
		}
		
	else
		{
	
		fIFD.fCompression = ccLossyJPEG;
		
		fIFD.fCompressionQuality = fCompressionQuality;
																	 
		fIFD.fBitsPerSample [0] = TagTypeSize (fImage->PixelType ()) * 8;
		
		for (uint32 j = 1; j < fIFD.fSamplesPerPixel; j++)
			{
			fIFD.fBitsPerSample [j] = fIFD.fBitsPerSample [0];
			}
			
		fIFD.FindTileSize (512 * 512 * fIFD.fSamplesPerPixel);
		
		}
	
	return new dng_raw_preview_tag_set (directory, *this, fIFD);
	
	}

/*****************************************************************************/

void dng_raw_preview::WriteData (dng_host &host,
								 dng_image_writer &writer,
								 dng_basic_tag_set &basic,
								 dng_stream &stream) const
	{
	
	writer.WriteImage (host,
					   fIFD,
					   basic,
					   stream,
					   *fImage.Get ());
					
	}
		
/*****************************************************************************/

uint64 dng_raw_preview::MaxImageDataByteCount () const
	{
	
	return fIFD.MaxImageDataByteCount ();
	
	}

/*****************************************************************************/

dng_mask_preview::dng_mask_preview ()

	:	fImage				()
	,	fCompressionQuality (-1)
	,	fIFD				()
	
	{
	
	}

/*****************************************************************************/

dng_mask_preview::~dng_mask_preview ()
	{
	
	}

/*****************************************************************************/

dng_basic_tag_set * dng_mask_preview::AddTagSet (dng_tiff_directory &directory) const
	{
	
	fIFD.fNewSubFileType = sfPreviewMask;
	
	fIFD.fImageWidth  = fImage->Width  ();
	fIFD.fImageLength = fImage->Height ();
	
	fIFD.fSamplesPerPixel = 1;
	
	fIFD.fPhotometricInterpretation = piTransparencyMask;
	
	fIFD.fCompression = ccDeflate;
	fIFD.fPredictor	  = cpHorizontalDifference;
	
	fIFD.fCompressionQuality = fCompressionQuality;
	
	fIFD.fBitsPerSample [0] = TagTypeSize (fImage->PixelType ()) * 8;
	
	fIFD.FindTileSize (512 * 512 * fIFD.fSamplesPerPixel);
	
	return new dng_basic_tag_set (directory, fIFD);
	
	}

/*****************************************************************************/

void dng_mask_preview::WriteData (dng_host &host,
								  dng_image_writer &writer,
								  dng_basic_tag_set &basic,
								  dng_stream &stream) const
	{
	
	writer.WriteImage (host,
					   fIFD,
					   basic,
					   stream,
					   *fImage.Get ());
					
	}

/*****************************************************************************/

uint64 dng_mask_preview::MaxImageDataByteCount () const
	{
	
	return fIFD.MaxImageDataByteCount ();
	
	}

/*****************************************************************************/

dng_basic_tag_set * dng_semantic_mask_preview::AddTagSet (dng_tiff_directory &directory) const
	{

	DNG_REQUIRE (fImage, "Missing fImage in dng_semantic_mask_preview");
	
	fIFD.fNewSubFileType = fOriginalSize ? sfSemanticMask : sfPreviewSemanticMask;
	
	fIFD.fImageWidth  = fImage->Width  ();
	fIFD.fImageLength = fImage->Height ();
	
	fIFD.fSamplesPerPixel = 1;
	
	fIFD.fPhotometricInterpretation = piPhotometricMask;
	
	fIFD.fCompression = ccDeflate;
	fIFD.fPredictor	  = cpHorizontalDifference;
	
	fIFD.fCompressionQuality = fCompressionQuality;
	
	fIFD.fBitsPerSample [0] = TagTypeSize (fImage->PixelType ()) * 8;
	
	fIFD.FindTileSize (512 * 512 * fIFD.fSamplesPerPixel);

	// Need to write the name and instance ID to the directory, too. Otherwise
	// we won't have corresponding labels when reading the image back in.

	fTagName.reset (new tag_string (tcSemanticName,
									fName,
									false));

	fTagInstanceID.reset (new tag_string (tcSemanticInstanceID,
										  fInstanceID,
										  false));

	directory.Add (fTagName.get ());

	directory.Add (fTagInstanceID.get ());

	// Also need to add the MaskSubArea info.

	fTagMaskSubArea.reset (new tag_uint32_ptr (tcMaskSubArea,
											   & fMaskSubArea [0],
											   4));
	
	directory.Add (fTagMaskSubArea.get ());

	return new dng_basic_tag_set (directory, fIFD);
	
	}

/*****************************************************************************/

void dng_semantic_mask_preview::WriteData (dng_host &host,
										   dng_image_writer &writer,
										   dng_basic_tag_set &basic,
										   dng_stream &stream) const
	{
	
	DNG_REQUIRE (fImage, "Missing fImage in dng_semantic_mask_preview");
	
	writer.WriteImage (host,
					   fIFD,
					   basic,
					   stream,
					   *fImage);
					
	}
		
/*****************************************************************************/

uint64 dng_semantic_mask_preview::MaxImageDataByteCount () const
	{
	
	return fIFD.MaxImageDataByteCount ();
	
	}

/*****************************************************************************/

dng_depth_preview::dng_depth_preview ()

	:	 fImage				 ()
	,	 fCompressionQuality (-1)
	,	 fFullResolution	 (false)
	,	 fIFD				 ()

	{
	
	}

/*****************************************************************************/

dng_depth_preview::~dng_depth_preview ()
	{
	
	}

/*****************************************************************************/

dng_basic_tag_set * dng_depth_preview::AddTagSet (dng_tiff_directory &directory) const
	{
	
	fIFD.fNewSubFileType = fFullResolution ? sfDepthMap
										   : sfPreviewDepthMap;
	
	fIFD.fImageWidth  = fImage->Width  ();
	fIFD.fImageLength = fImage->Height ();
	
	fIFD.fSamplesPerPixel = 1;
	
	fIFD.fPhotometricInterpretation = piDepth;
	
	fIFD.fCompression = ccDeflate;
	fIFD.fPredictor	  = cpHorizontalDifference;
	
	fIFD.fCompressionQuality = fCompressionQuality;
	
	fIFD.fBitsPerSample [0] = TagTypeSize (fImage->PixelType ()) * 8;
	
	fIFD.FindTileSize (512 * 512 * fIFD.fSamplesPerPixel);
	
	return new dng_basic_tag_set (directory, fIFD);
	
	}

/*****************************************************************************/

void dng_depth_preview::WriteData (dng_host &host,
								   dng_image_writer &writer,
								   dng_basic_tag_set &basic,
								   dng_stream &stream) const
	{
	
	writer.WriteImage (host,
					   fIFD,
					   basic,
					   stream,
					   *fImage.Get ());
		
	}

/*****************************************************************************/

uint64 dng_depth_preview::MaxImageDataByteCount () const
	{
	
	return fIFD.MaxImageDataByteCount ();
	
	}

/*****************************************************************************/

dng_preview_list::dng_preview_list ()
	
	:	fCount (0)
	
	{
	
	}
		
/*****************************************************************************/

dng_preview_list::~dng_preview_list ()
	{
	
	}
		
/*****************************************************************************/

void dng_preview_list::Append (AutoPtr<dng_preview> &preview)
	{
	
	if (preview.Get ())
		{
	
		DNG_ASSERT (fCount < kMaxDNGPreviews, "DNG preview list overflow");
		
		if (fCount < kMaxDNGPreviews)
			{
			
			fPreview [fCount++] . Reset (preview.Release ());
			
			}
			
		}
	
	}
		
/*****************************************************************************/
