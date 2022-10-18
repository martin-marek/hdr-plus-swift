/*****************************************************************************/
// Copyright 2006-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:	Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/** \file
 * Support for DNG image reading.
 */

/*****************************************************************************/

#ifndef __dng_read_image__
#define __dng_read_image__

/*****************************************************************************/

#include "dng_area_task.h"
#include "dng_auto_ptr.h"
#include "dng_classes.h"
#include "dng_image.h"
#include "dng_memory.h"
#include "dng_mutex.h"
#include "dng_types.h"

/******************************************************************************/

bool DecodePackBits (dng_stream &stream,
					 uint8 *dPtr,
					 int32 dstCount);

/*****************************************************************************/

class dng_row_interleaved_image: public dng_image
	{
	
	private:
	
		dng_image &fImage;
		
		uint32 fFactor;
		
	public:
	
		dng_row_interleaved_image (dng_image &image,
								   uint32 factor);
					  
		virtual void DoGet (dng_pixel_buffer &buffer) const;
			
		virtual void DoPut (const dng_pixel_buffer &buffer);
		
	private:
	
		int32 MapRow (int32 row) const;
	
	};

/*****************************************************************************/

class dng_read_image
	{
	
	friend class dng_read_tiles_task;
	
	protected:
	
		enum
			{
			
			// Target size for buffer used to copy data to the image.
			
			kImageBufferSize = 128 * 1024
			
			};
			
		AutoPtr<dng_memory_block> fJPEGTables;
	
	public:
	
		dng_read_image ();
		
		virtual ~dng_read_image ();
		
		virtual bool CanRead (const dng_ifd &ifd);
		
		virtual void Read (dng_host &host,
						   const dng_ifd &ifd,
						   dng_stream &stream,
						   dng_image &image,
						   dng_jpeg_image *jpegImage,
						   dng_fingerprint *jpegDigest);
						   
	protected:
								
		virtual bool ReadUncompressed (dng_host &host,
									   const dng_ifd &ifd,
									   dng_stream &stream,
									   dng_image &image,
									   const dng_rect &tileArea,
									   uint32 plane,
									   uint32 planes,
									   AutoPtr<dng_memory_block> &uncompressedBuffer,
									   AutoPtr<dng_memory_block> &subTileBlockBuffer);
									   
		virtual void DecodeLossyJPEG (dng_host &host,
									  dng_image &image,
									  const dng_rect &tileArea,
									  uint32 plane,
									  uint32 planes,
									  uint32 photometricInterpretation,
									  uint32 jpegDataSize,
									  uint8 *jpegDataInMemory,
									  bool usingMultipleThreads);
	
		virtual bool ReadBaselineJPEG (dng_host &host,
									   const dng_ifd &ifd,
									   dng_stream &stream,
									   dng_image &image,
									   const dng_rect &tileArea,
									   uint32 plane,
									   uint32 planes,
									   uint32 tileByteCount,
									   uint8 *jpegDataInMemory,
									   bool usingMultipleThreads);
	
		virtual bool ReadLosslessJPEG (dng_host &host,
									   const dng_ifd &ifd,
									   dng_stream &stream,
									   dng_image &image,
									   const dng_rect &tileArea,
									   uint32 plane,
									   uint32 planes,
									   uint32 tileByteCount,
									   AutoPtr<dng_memory_block> &uncompressedBuffer,
									   AutoPtr<dng_memory_block> &subTileBlockBuffer);
									   
		virtual bool CanReadTile (const dng_ifd &ifd);
		
		virtual bool NeedsCompressedBuffer (const dng_ifd &ifd);
	
		virtual void ByteSwapBuffer (dng_host &host,
									 dng_pixel_buffer &buffer);

		virtual void DecodePredictor (dng_host &host,
									  const dng_ifd &ifd,
									  dng_pixel_buffer &buffer);

		virtual void ReadTile (dng_host &host,
							   const dng_ifd &ifd,
							   dng_stream &stream,
							   dng_image &image,
							   const dng_rect &tileArea,
							   uint32 plane,
							   uint32 planes,
							   uint32 tileByteCount,
							   AutoPtr<dng_memory_block> &compressedBuffer,
							   AutoPtr<dng_memory_block> &uncompressedBuffer,
							   AutoPtr<dng_memory_block> &subTileBlockBuffer,
							   bool usingMultipleThreads);

		virtual void DoReadTiles (dng_host &host,
								  const dng_ifd &ifd,
								  dng_stream &stream,
								  dng_image &image,
								  dng_jpeg_image *jpegImage,
								  dng_fingerprint *jpegTileDigest,
								  uint32 outerSamples,
								  uint32 innerSamples,
								  uint32 tilesDown,
								  uint32 tilesAcross,
								  uint64 *tileOffset,
								  uint32 *tileByteCount,
								  uint32 compressedSize,
								  uint32 uncompressedSize);
	
	};

/*****************************************************************************/

class dng_read_tiles_task : public dng_area_task,
							private dng_uncopyable
	{
	
	protected:
	
		dng_read_image &fReadImage;
		
		dng_host &fHost;
		
		const dng_ifd &fIFD;
		
		dng_stream &fStream;
		
		dng_image &fImage;
		
		dng_jpeg_image *fJPEGImage;
		
		dng_fingerprint *fJPEGTileDigest;
		
		uint32 fOuterSamples;
		
		uint32 fInnerSamples;
		
		uint32 fTilesDown;
		
		uint32 fTilesAcross;
		
		uint64 *fTileOffset;
		
		uint32 *fTileByteCount;
		
		uint32 fCompressedSize;
		
		uint32 fUncompressedSize;
		
		dng_mutex fMutex;
		
		uint32 fNextTileIndex;
		
	public:
	
		dng_read_tiles_task (dng_read_image &readImage,
							 dng_host &host,
							 const dng_ifd &ifd,
							 dng_stream &stream,
							 dng_image &image,
							 dng_jpeg_image *jpegImage,
							 dng_fingerprint *jpegTileDigest,
							 uint32 outerSamples,
							 uint32 innerSamples,
							 uint32 tilesDown,
							 uint32 tilesAcross,
							 uint64 *tileOffset,
							 uint32 *tileByteCount,
							 uint32 compressedSize,
							 uint32 uncompressedSize);

		void Process (uint32 threadIndex,
					  const dng_rect &tile,
					  dng_abort_sniffer *sniffer);

	protected:

		void ReadTask (uint32 tileIndex,
					   uint32 &byteCount,
					   dng_memory_block *compressedBuffer);

		void ProcessTask (uint32 tileIndex,
						  uint32 byteCount,
						  dng_abort_sniffer *sniffer,
						  AutoPtr<dng_memory_block> &compressedBuffer,
						  AutoPtr<dng_memory_block> &uncompressedBuffer,
						  AutoPtr<dng_memory_block> &subTileBlockBuffer);
		
	};

/*****************************************************************************/

#endif
	
/*****************************************************************************/
