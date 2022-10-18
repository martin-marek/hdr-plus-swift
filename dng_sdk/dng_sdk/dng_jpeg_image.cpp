/*****************************************************************************/
// Copyright 2011-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:	Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#include "dng_jpeg_image.h"

#include "dng_abort_sniffer.h"
#include "dng_area_task.h"
#include "dng_assertions.h"
#include "dng_host.h"
#include "dng_ifd.h"
#include "dng_image.h"
#include "dng_image_writer.h"
#include "dng_memory_stream.h"
#include "dng_safe_arithmetic.h"
#include "dng_uncopyable.h"

#include <atomic>

/*****************************************************************************/

dng_jpeg_image::dng_jpeg_image ()

	:	fImageSize	()
	,	fTileSize	()
	,	fUsesStrips (false)
	,	fJPEGTables ()
	,	fJPEGData	()
	
	{
	
	}

/*****************************************************************************/

class dng_jpeg_image_encode_task : public dng_area_task,
								   private dng_uncopyable
	{
	
	private:
	
		dng_host &fHost;
		
		dng_image_writer &fWriter;
		
		const dng_image &fImage;
	
		dng_jpeg_image &fJPEGImage;
		
		uint32 fTileCount;
		
		const dng_ifd &fIFD;
				
		std::atomic_uint fNextTileIndex;
		
	public:
	
		dng_jpeg_image_encode_task (dng_host &host,
									dng_image_writer &writer,
									const dng_image &image,
									dng_jpeg_image &jpegImage,
									uint32 tileCount,
									const dng_ifd &ifd)

			:	dng_area_task ("dng_jpeg_image_encode_task")
		
			,	fHost			  (host)
			,	fWriter			  (writer)
			,	fImage			  (image)
			,	fJPEGImage		  (jpegImage)
			,	fTileCount		  (tileCount)
			,	fIFD			  (ifd)
			,	fNextTileIndex	  (0)
			
			{
			
			fMinTaskArea = 16 * 16;
			fUnitCell	 = dng_point (16, 16);
			fMaxTileSize = dng_point (16, 16);
			
			}
	
		void Process (uint32 /* threadIndex */,
					  const dng_rect & /* tile */,
					  dng_abort_sniffer *sniffer)
			{
			
			AutoPtr<dng_memory_block> compressedBuffer;
			AutoPtr<dng_memory_block> uncompressedBuffer;
			AutoPtr<dng_memory_block> subTileBlockBuffer;
			AutoPtr<dng_memory_block> tempBuffer;
			
			uint32 uncompressedSize = SafeUint32Mult (fIFD.fTileLength, 
													  fIFD.fTileWidth, 
													  fIFD.fSamplesPerPixel);
			
			uncompressedBuffer.Reset (fHost.Allocate (uncompressedSize));
			
			uint32 tilesAcross = fIFD.TilesAcross ();
	
			while (true)
				{

				// Note: fNextTileIndex is atomic
				
				uint32 tileIndex = fNextTileIndex++;

				if (tileIndex >= fTileCount)
					{
					return;
					}

				dng_abort_sniffer::SniffForAbort (sniffer);
				
				uint32 rowIndex = tileIndex / tilesAcross;
				uint32 colIndex = tileIndex % tilesAcross;
				
				dng_rect tileArea = fIFD.TileArea (rowIndex, colIndex);
				
				dng_memory_stream stream (fHost.Allocator ());
				
				fWriter.WriteTile (fHost,
								   fIFD,
								   stream,
								   fImage,
								   tileArea,
								   1,
								   compressedBuffer,
								   uncompressedBuffer,
								   subTileBlockBuffer,
								   tempBuffer,
								   true);
								  
				fJPEGImage.fJPEGData [tileIndex].Reset (stream.AsMemoryBlock (fHost.Allocator ()));
					
				}
			
			}
		
	};

/*****************************************************************************/

void dng_jpeg_image::Encode (dng_host &host,
							 const dng_negative &negative,
							 dng_image_writer &writer,
							 const dng_image &image)
	{
	
	#if qDNGValidate
	dng_timer timer ("Encode JPEG Proxy time");
	#endif
	
	DNG_ASSERT (image.PixelType () == ttByte, "Cannot JPEG encode non-byte image");
	
	fImageSize = image.Bounds ().Size ();
	
	dng_ifd ifd;
	
	ifd.fImageWidth	 = fImageSize.h;
	ifd.fImageLength = fImageSize.v;
	
	ifd.fSamplesPerPixel = image.Planes ();
	
	ifd.fBitsPerSample [0] = 8;
	ifd.fBitsPerSample [1] = 8;
	ifd.fBitsPerSample [2] = 8;
	ifd.fBitsPerSample [3] = 8;
	
	ifd.fPhotometricInterpretation = piLinearRaw;
	
	ifd.fCompression = ccLossyJPEG;
	
	ifd.FindTileSize (512 * 512 * ifd.fSamplesPerPixel);
	
	fTileSize.h = ifd.fTileWidth;
	fTileSize.v = ifd.fTileLength;
	
	// Need a higher quality for raw proxies than non-raw proxies, since users
	// often perform much greater color changes. Also, if we are targeting a
	// "large" size proxy (larger than 5 MP), or this is a full size proxy,
	// then use a higher quality.
	
	bool useHigherQuality = (uint64) ifd.fImageWidth *
							(uint64) ifd.fImageLength > 5000000 ||
							image.Bounds ().Size () == negative.OriginalDefaultFinalSize ();
	
	if (negative.ColorimetricReference () == crSceneReferred)
		{
		ifd.fCompressionQuality = useHigherQuality ? 11 : 10;
		}
	else
		{
		ifd.fCompressionQuality = useHigherQuality ? 10 : 8;
		}
	
	uint32 tilesAcross = ifd.TilesAcross ();
	uint32 tilesDown   = ifd.TilesDown	 ();
	
	uint32 tileCount = tilesAcross * tilesDown;
	
	fJPEGData.Reset (tileCount);
	
	uint32 threadCount = Min_uint32 (tileCount,
									 host.PerformAreaTaskThreads ());
										 
	dng_jpeg_image_encode_task task (host,
									 writer,
									 image,
									 *this,
									 tileCount,
									 ifd);
									  
	host.PerformAreaTask (task,
						  dng_rect (0, 0, 16, 16 * threadCount));
		
	}
			
/*****************************************************************************/

class dng_jpeg_image_find_digest_task : public dng_area_task,
										private dng_uncopyable
	{
	
	private:
	
		const dng_jpeg_image &fJPEGImage;
		
		uint32 fTileCount;
		
		dng_fingerprint *fDigests;
				
		std::atomic_uint fNextTileIndex;
		
	public:
	
		dng_jpeg_image_find_digest_task (const dng_jpeg_image &jpegImage,
										 uint32 tileCount,
										 dng_fingerprint *digests)

			:	dng_area_task ("dng_jpeg_image_find_digest_task")
		
			,	fJPEGImage		  (jpegImage)
			,	fTileCount		  (tileCount)
			,	fDigests		  (digests)
			,	fNextTileIndex	  (0)
			
			{
			
			fMinTaskArea = 16 * 16;
			fUnitCell	 = dng_point (16, 16);
			fMaxTileSize = dng_point (16, 16);
			
			}
	
		void Process (uint32 /* threadIndex */,
					  const dng_rect & /* tile */,
					  dng_abort_sniffer *sniffer)
			{
			
			while (true)
				{
				
				// Note: fNextTileIndex is atomic
				
				uint32 tileIndex = fNextTileIndex++;

				if (tileIndex >= fTileCount)
					{
					return;
					}
					
				dng_abort_sniffer::SniffForAbort (sniffer);
				
				dng_md5_printer printer;
				
				printer.Process (fJPEGImage.fJPEGData [tileIndex]->Buffer	   (),
								 fJPEGImage.fJPEGData [tileIndex]->LogicalSize ());
								 
				fDigests [tileIndex] = printer.Result ();
					
				}
			
			}
		
	};

/*****************************************************************************/

dng_fingerprint dng_jpeg_image::FindDigest (dng_host &host) const
	{
	
	uint32 tileCount = TileCount ();
	
	uint32 arrayCount = tileCount + (fJPEGTables.Get () ? 1 : 0);
	
	AutoArray<dng_fingerprint> digests (arrayCount);
	
	// Compute digest of each compressed tile.

		{
		
		uint32 threadCount = Min_uint32 (tileCount,
										 host.PerformAreaTaskThreads ());
										 
		dng_jpeg_image_find_digest_task task (*this,
											  tileCount,
											  digests.Get ());
										  
		host.PerformAreaTask (task,
							  dng_rect (0, 0, 16, 16 * threadCount));
		
		}
	
	// Compute digest of JPEG tables, if any.
		
	if (fJPEGTables.Get ())
		{
		
		dng_md5_printer printer;
		
		printer.Process (fJPEGTables->Buffer	  (),
						 fJPEGTables->LogicalSize ());
						 
		digests [tileCount] = printer.Result ();
		
		}
		
	// Combine digests into a single digest.
	
		{
		
		dng_md5_printer printer;
		
		for (uint32 k = 0; k < arrayCount; k++)
			{
		
			printer.Process (digests [k].data,
							 dng_fingerprint::kDNGFingerprintSize);
							 
			}
			
		return printer.Result ();
		
		}
	
	}
			
/*****************************************************************************/

