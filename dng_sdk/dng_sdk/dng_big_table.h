/*****************************************************************************/
// Copyright 2015-2021 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:	Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#ifndef __dng_big_table__
#define __dng_big_table__

/*****************************************************************************/

#include "dng_classes.h"
#include "dng_camera_profile.h"
#include "dng_rect.h"
#include "dng_safe_arithmetic.h"
#include "dng_tag_types.h"

#include <map>
#include <memory>
#include <vector>

/*****************************************************************************/

void dng_big_table_cache_flush ();

/*****************************************************************************/

class dng_big_table
	{

	protected:

		enum BigTableTypeEnum
			{
			btt_LookTable  = 0,
			btt_RGBTable   = 1,
			btt_ImageTable = 2
			};

	private:

		dng_fingerprint fFingerprint;

		dng_big_table_cache * fCache;
		
		bool fIsMissing;

	protected:

		dng_big_table (dng_big_table_cache *cache);

		dng_big_table (const dng_big_table &table);

		dng_big_table & operator= (const dng_big_table &table);

	public:

		virtual ~dng_big_table ();
		
		bool IsMissing () const
			{
			return fIsMissing;
			}
		
		void SetMissing ()
			{
			fIsMissing = true;
			}
		
		bool IsEmbedNever () const
			{
			return (GetFlags () & 1) != 0;
			}
		
		void SetEmbedNever ()
			{
			SetFlags (GetFlags () | 1);
			}

		virtual bool IsValid () const = 0;

		const dng_fingerprint & Fingerprint () const;

		bool DecodeFromBinary (dng_host &host,
							   const uint8 * compressedData,
							   uint32 compressedSize,
							   AutoPtr<dng_memory_block> *uncompressedCache = nullptr);
		
		static void ASCIItoBinary (dng_memory_allocator &allocator,
								   const char *sPtr,
								   uint32 sCount,
								   AutoPtr<dng_memory_block> &dBlock,
								   uint32 &dCount);
								   
		bool DecodeFromString (dng_host &host,
							   const dng_string &block1);
		
		dng_memory_block * EncodeAsBinary (dng_memory_allocator &allocator,
										   uint32 &compressedSize) const;
		
		dng_memory_block * EncodeAsString (dng_memory_allocator &allocator) const;

		bool ExtractFromCache (const dng_fingerprint &fingerprint);

		
		#if qDNGUseXMP
		
		bool ReadTableFromXMP (const dng_xmp &xmp,
							   const char *ns,
							   const dng_fingerprint &fingerprint,
							   dng_abort_sniffer *sniffer = NULL);
		
		bool ReadFromXMP (const dng_xmp &xmp,
						  const char *ns,
						  const char *path,
						  dng_big_table_storage &storage,
						  dng_abort_sniffer *sniffer = NULL);

		void WriteToXMP (dng_xmp &xmp,
						 const char *ns,
						 const char *path,
						 dng_big_table_storage &storage) const;

		#endif	// qDNGUseXMP

	protected:
	
		virtual dng_fingerprint ComputeFingerprint () const;

		void RecomputeFingerprint ();
		
		virtual bool UseCompression () const
			{
			return true;
			}

		virtual bool GetStream (dng_stream &stream) = 0;

		virtual void PutStream (dng_stream &stream,
								bool forFingerprint) const = 0;
		
		virtual uint32 GetFlags () const = 0;
		
		virtual void SetFlags (uint32 flags) = 0;

	};

/*****************************************************************************/

class dng_big_table_accessor
	{
	
	public:
	
		dng_big_table_accessor ()
			{
			}
			
		virtual ~dng_big_table_accessor ()
			{
			}
									   
		virtual bool HasTable (const dng_fingerprint & /* fingerprint */) const
			{
			return false;
			}
		
		virtual bool GetTable (const dng_fingerprint & /* fingerprint */,
							   dng_ref_counted_block & /* block */) const
			{
			return false;
			}
	
		virtual void AddTable (const dng_fingerprint & /* fingerprint */,
							   const dng_ref_counted_block & /* block */)
			{
			ThrowProgramError ("AddTable not implemented");
			}
			
		virtual void CopyToDictionary (dng_big_table_dictionary & /* dictionary */) const
			{
			ThrowProgramError ("CopyToDictionary not implemented");
			}
	
	};
	
/*****************************************************************************/

class dng_big_table_dictionary : public dng_big_table_accessor
	{
	
	private:
	
		std::map<dng_fingerprint, dng_ref_counted_block> fMap;
		
	public:
	
		const std::map<dng_fingerprint, dng_ref_counted_block> & Map () const
			{
			return fMap;
			}
		
		bool IsEmpty () const
			{
			return fMap.empty ();
			}
			
		virtual bool HasTable (const dng_fingerprint &fingerprint) const;
		
		virtual bool GetTable (const dng_fingerprint &fingerprint,
							   dng_ref_counted_block &block) const;
	
		virtual void AddTable (const dng_fingerprint &fingerprint,
							   const dng_ref_counted_block &block);
	
		virtual void CopyToDictionary (dng_big_table_dictionary & /* dictionary */) const;
			
	};
	
/*****************************************************************************/

class dng_big_table_index
	{
	
	public:
	
		struct IndexEntry
			{
			uint32 fTableSize;
			uint64 fTableOffset;
			};
			
	private:
	
		std::map<dng_fingerprint, IndexEntry> fMap;
		
	public:
	
		dng_big_table_index ();
			
		bool IsEmpty () const
			{
			return fMap.empty ();
			}
			
		const std::map<dng_fingerprint, IndexEntry> & Map () const
			{
			return fMap;
			}
			
		bool HasEntry (const dng_fingerprint &fingerprint) const;

		bool GetEntry (const dng_fingerprint &fingerprint,
					   uint32 &tableSize,
					   uint64 &tableOffset) const;

		void AddEntry (const dng_fingerprint &fingerprint,
					   uint32 tableSize,
					   uint64 tableOffset);
					   
	};

/*****************************************************************************/

class dng_big_table_storage
	{
	
	public:
		
		dng_big_table_storage ();
		
		virtual ~dng_big_table_storage ();
		
		virtual bool ReadTable (dng_big_table &table,
								const dng_fingerprint &fingerprint,
								dng_memory_allocator &allocator);
	
		virtual bool WriteTable (const dng_big_table &table,
								 const dng_fingerprint &fingerprint,
								 dng_memory_allocator &allocator);
		
		virtual void MissingTable (const dng_fingerprint &fingerprint);
	
	};

/*****************************************************************************/

class dng_look_table : public dng_big_table
	{

	friend class dng_look_table_cache;

	public:

		enum
			{
			
			// Tables are are allowed to trade off resolution
			// between dimensions, but need to keep total
			// samples below this limit.  This results in
			// 216K memory footprint (12 byte per sample)
			// which is similar in size to the RGB table
			// size limit.
			
			kMaxTotalSamples = 36 * 32 * 16,
			
			// Also each must be within their own limits.
			
			kMaxHueSamples = 360,
			kMaxSatSamples = 256,
			kMaxValSamples = 256

			};

	private:

		enum
			{
			kLookTableVersion1 = 1,
			kLookTableVersion2 = 2
			};

		// Table data affecting fingerprint and caching.

		struct table_data
			{

			// 3-D hue/sat table to apply a "look".
			
			dng_hue_sat_map fMap;

			// Value (V of HSV) encoding for look table.

			uint32 fEncoding;
			
			// Minimum and maximum scale amounts supported by table.

			real64 fMinAmount;
			real64 fMaxAmount;

			// Does this table have only monochrome output (when amount is 1.0)?
			
			bool fMonochrome;
			
			// Flags.
			
			uint32 fFlags;

			// Constructor to set defaults.

			table_data ()

				:	fMap		()
				,	fEncoding	(encoding_Linear)
				,	fMinAmount	(1.0)
				,	fMaxAmount	(1.0)
				,	fMonochrome (false)
				,	fFlags		(0)

				{
				}
				
			// Compute monchrome flag.
			
			void ComputeMonochrome ()
				{
				
				fMonochrome = true;
				
				uint32 count = fMap.DeltasCount ();

				dng_hue_sat_map::HSBModify * deltas = fMap.GetDeltas ();
				
				for (uint32 index = 0; index < count; index++)
					{

					if (deltas [index] . fSatScale != 0.0f)
						{
						
						fMonochrome = false;
						
						return;
						
						}
					
					}
				
				}

			};

		table_data fData;

		// Amount to apply at runtime (does not affect fingerprint).

		real64 fAmount;

	public:

		dng_look_table ();

		dng_look_table (const dng_look_table &table);

		dng_look_table & operator= (const dng_look_table &table);

		virtual ~dng_look_table ();

		bool operator== (const dng_look_table &table) const
			{
			return Fingerprint () == table.Fingerprint () &&
				   Amount	   () == table.Amount	   () &&
				   IsMissing   () == table.IsMissing   ();
			}

		bool operator!= (const dng_look_table &table) const
			{
			return !(*this == table);
			}

		void Set (const dng_hue_sat_map &map,
				  uint32 encoding);

		virtual bool IsValid () const;

		void SetInvalid ();

		real64 MinAmount () const
			{
			return fData.fMinAmount;
			}

		real64 MaxAmount () const
			{
			return fData.fMaxAmount;
			}

		void SetAmountRange (real64 minAmount,
							 real64 maxAmount)
			{

			fData.fMinAmount = Pin_real64 (0.0,
										   Round_int32 (minAmount * 100.0) * 0.01,
										   1.0);
				
			fData.fMaxAmount = Pin_real64 (1.0,
										   Round_int32 (maxAmount * 100.0) * 0.01,
										   2.0);
			
			fAmount = Pin_real64 (fData.fMinAmount, fAmount, fData.fMaxAmount);

			RecomputeFingerprint ();

			}

		real64 Amount () const
			{
			return fAmount;
			}

		void SetAmount (real64 amount)
			{

			fAmount = Pin_real64 (fData.fMinAmount,
								  Round_int32 (amount * 100.0) * 0.01,
								  fData.fMaxAmount);

			// Not part of fingerprint.

			}

		const dng_hue_sat_map & Map () const
			{
			return fData.fMap;
			}

		uint32 Encoding () const
			{
			return fData.fEncoding;
			}
		
		bool Monochrome () const
			{
			return IsValid () && fAmount == 1.0 && fData.fMonochrome;
			}

	protected:

		virtual bool GetStream (dng_stream &stream);

		virtual void PutStream (dng_stream &stream,
								bool forFingerprint) const;

		virtual uint32 GetFlags () const
			{
			return fData.fFlags;
			}
		
		virtual void SetFlags (uint32 flags)
			{
			
			fData.fFlags = flags;
			
			RecomputeFingerprint ();

			}

	};

/*****************************************************************************/

class dng_rgb_table : public dng_big_table
	{

	friend class dng_rgb_table_cache;

	public:

		enum
			{

			kMinDivisions1D = 2,
			kMaxDivisions1D = 4096,

			kMinDivisions3D = 2,
			kMaxDivisions3D = 32,

			kMaxDivisions3D_InMemory = 130

			};

		enum primaries_enum
			{

			primaries_sRGB = 0,
			primaries_Adobe,
			primaries_ProPhoto,
			primaries_P3,
			primaries_Rec2020,

			primaries_count

			};

		enum gamma_enum
			{

			gamma_Linear = 0,
			gamma_sRGB,
			gamma_1_8,
			gamma_2_2,
			gamma_Rec2020,

			gamma_count

			};

		enum gamut_enum
			{

			gamut_clip = 0,
			gamut_extend,

			gamut_count

			};

	private:

		enum
			{
			kRGBTableVersion = 1
			};

		// Table data affecting fingerprint and caching.

		struct table_data
			{

			// Number of dimensions of the table (1 or 3).

			uint32 fDimensions;

			// Number of samples per side of table.
			
			uint32 fDivisions;

			// Sample data.	 16-bit unsigned encoding.
			// Right zero padded to 64 bits per sample (i.e. RGB0).
			
			dng_ref_counted_block fSamples;

			// Color primaries for table.

			primaries_enum fPrimaries;

			// Gamma encoding for table.

			gamma_enum fGamma;

			// Gamut processing option for table.

			gamut_enum fGamut;

			// Minimum and maximum scale amounts supported by table.

			real64 fMinAmount;
			real64 fMaxAmount;

			// Does this table have only monochrome output (when amount is 1.0)?
			
			bool fMonochrome;
			
			// Flags.
			
			uint32 fFlags;

			// Constructor to set defaults.

			table_data ()

				:	fDimensions (0)
				,	fDivisions	(0)
				,	fSamples	()
				,	fPrimaries	(primaries_sRGB)
				,	fGamma		(gamma_sRGB)
				,	fGamut		(gamut_clip)
				,	fMinAmount	(0.0)
				,	fMaxAmount	(2.0)
				,	fMonochrome (false)
				,	fFlags		(0)

				{
				}

			// Compute monchrome flag.
			
			void ComputeMonochrome ()
				{
				
				if (fPrimaries != primaries_ProPhoto &&
					fGamut	   != gamut_clip)
					{
					
					fMonochrome = false;
					
					return;
					
					}
					
				if (fDimensions != 3)
					{
					
					fMonochrome = false;
					
					return;
					
					}
				
				fMonochrome = true;
				
				uint32 count = fDivisions * fDivisions * fDivisions;

				const uint16 * sample = fSamples.Buffer_uint16 ();
				
				for (uint32 index = 0; index < count; index++)
					{
					
					if (sample [0] != sample [1] ||
						sample [0] != sample [2])
						{

						fMonochrome = false;
						
						return;
						
						}
						
					sample += 4;
					
					}
					
				}

			};

		table_data fData;

		// Amount to apply at runtime (does not affect fingerprint).

		real64 fAmount;

	public:

		dng_rgb_table ();

		dng_rgb_table (const dng_rgb_table &table);

		dng_rgb_table & operator= (const dng_rgb_table &table);

		virtual ~dng_rgb_table ();

		bool operator== (const dng_rgb_table &table) const
			{
			return Fingerprint () == table.Fingerprint () &&
				   Amount	   () == table.Amount	   () &&
				   IsMissing   () == table.IsMissing   ();
			}

		bool operator!= (const dng_rgb_table &table) const
			{
			return !(*this == table);
			}

		virtual bool IsValid () const;

		void SetInvalid ();

		primaries_enum Primaries () const
			{
			return fData.fPrimaries;
			}

		void SetPrimaries (primaries_enum primaries)
			{

			fData.fPrimaries = primaries;

			fData.ComputeMonochrome ();

			RecomputeFingerprint ();

			}

		gamma_enum Gamma () const
			{
			return fData.fGamma;
			}

		void SetGamma (gamma_enum gamma)
			{

			fData.fGamma = gamma;

			RecomputeFingerprint ();

			}

		gamut_enum Gamut () const
			{
			return fData.fGamut;
			}

		void SetGamut (gamut_enum gamut)
			{

			fData.fGamut = gamut;

			fData.ComputeMonochrome ();

			RecomputeFingerprint ();

			}

		real64 MinAmount () const
			{
			return fData.fMinAmount;
			}

		real64 MaxAmount () const
			{
			return fData.fMaxAmount;
			}

		void SetAmountRange (real64 minAmount,
							 real64 maxAmount)
			{

			fData.fMinAmount = Pin_real64 (0.0,
										   Round_int32 (minAmount * 100.0) * 0.01,
										   1.0);
				
			fData.fMaxAmount = Pin_real64 (1.0,
										   Round_int32 (maxAmount * 100.0) * 0.01,
										   2.0);
			
			fAmount = Pin_real64 (fData.fMinAmount, fAmount, fData.fMaxAmount);

			RecomputeFingerprint ();

			}

		real64 Amount () const
			{
			return fAmount;
			}

		void SetAmount (real64 amount)
			{

			fAmount = Pin_real64 (fData.fMinAmount,
								  Round_int32 (amount * 100.0) * 0.01,
								  fData.fMaxAmount);

			// Not part of fingerprint.

			}

		uint32 Dimensions () const
			{
			return fData.fDimensions;
			}

		uint32 Divisions () const
			{
			return fData.fDivisions;
			}

		const uint16 * Samples () const
			{
			return fData.fSamples.Buffer_uint16 ();
			}

		bool Monochrome () const
			{
			return IsValid () && fAmount == 1.0 && fData.fMonochrome;
			}

		void Set (uint32 dimensions,
				  uint32 divisions,
				  dng_ref_counted_block samples);

	protected:

		virtual bool GetStream (dng_stream &stream);

		virtual void PutStream (dng_stream &stream,
								bool forFingerprint) const;

		virtual uint32 GetFlags () const
			{
			return fData.fFlags;
			}

		virtual void SetFlags (uint32 flags)
			{
			
			fData.fFlags = flags;
			
			RecomputeFingerprint ();

			}

	};

/*****************************************************************************/

class dng_image_table : public dng_big_table
	{
	
	friend class dng_image_table_cache;
	
	private:

		enum
			{
			kImageTableVersion = 1
			};

	protected:
	
		std::shared_ptr<const dng_image> fImage;
		
	public:
	
		dng_image_table ();

		dng_image_table (const dng_image_table &table);

		dng_image_table & operator= (const dng_image_table &table);

		virtual ~dng_image_table ();
	
		virtual bool IsValid () const;
		
		void SetInvalid ();

		bool operator== (const dng_image_table &table) const
			{
			return Fingerprint () == table.Fingerprint () &&
				   IsMissing   () == table.IsMissing   ();
			}

		bool operator!= (const dng_image_table &table) const
			{
			return !(*this == table);
			}
			
		const dng_image & Image () const
			{
			return *fImage;
			}
			
		std::shared_ptr<const dng_image> ShareImage () const
			{
			return fImage;
			};
			
		void SetImage (const dng_image *image);

		void SetImage (const std::shared_ptr<const dng_image> &image);

	protected:

		virtual dng_host * MakeHost () const;

		virtual dng_fingerprint ComputeFingerprint () const;
		
		virtual bool UseCompression () const
			{
			return false;
			}
			
		virtual bool GetStream (dng_stream &stream);

		virtual void PutStream (dng_stream &stream,
								bool forFingerprint) const;
		
		virtual uint32 GetFlags () const
			{
			return 0;
			}

		virtual void SetFlags (uint32 /* flags */)
			{
			}

	};
	
/*****************************************************************************/

// Implements RGBTables tag from DNG 1.6.

class dng_masked_rgb_table: private dng_uncopyable
	{
		
	private:

		// TableSemanticName and LengthTableSemanticName fields.

		dng_string fTableSemanticName;

		// PixelType field. Note that this is NOT the list of pixel types in
		// dng_tag_types.h but is instead 0, 1, or 2 per DNG 1.6 spec for
		// RGBTables tag. Only three values are supported:
		//
		// 0 = ttByte
		// 1 = ttShort
		// 2 = ttFloat
		
		uint32 fPixelType = 0;

		// The following RGBTables tag fields are represented inside the
		// dng_rgb_table data structure, along with the table data itself.
		//
		// Divisions	  dng_rgb_table::table_data::fDivisions
		// GammaEncoding  dng_rgb_table::table_data::fGamma
		// ColorPrimaries dng_rgb_table::table_data::fPrimaries
		// GamutExtension dng_rgb_table::table_data::fGamut

		dng_rgb_table fTable;

		// fTable's internal representation is ttShort (unsigned 16-bit
		// integer). To avoid round-tripping errors, optionally store a copy
		// of the sample data in the file.

		std::shared_ptr<dng_memory_block> fStoredData;

	public:

		dng_masked_rgb_table ();

		dng_masked_rgb_table (dng_string tableSemanticName,
							  uint32 pixelType,
							  const dng_rgb_table &table);

		void Validate () const;
		
		void GetStream (dng_host &host,
						dng_stream &stream);
		
		void PutStream (dng_stream &stream) const;

		void AddDigest (dng_md5_printer &printer) const;

		const dng_string & SemanticName () const
			{
			return fTableSemanticName;
			}

		const dng_rgb_table & Table () const
			{
			return fTable;
			}

		#if qDNGValidate
		void Dump (uint32 indent) const;
		#endif

	private:

		static void CheckDivisions (uint32 divisions);

		static void CheckPixelType (uint32 pixelType);

		static void CheckGammaEncoding (dng_rgb_table::gamma_enum gamma);

		static void CheckGamutExtension (dng_rgb_table::gamut_enum gamut);

		static void CheckColorPrimaries (dng_rgb_table::primaries_enum primaries);
		
	};

/*****************************************************************************/

// Implements RGBTables tag from DNG 1.6.

class dng_masked_rgb_tables: private dng_uncopyable
	{

	public:
		
		static const uint32 kMaxTables = 20;

		enum composite_method
			{
			kWeightedSum = 0,
			kSequential	 = 1,
			};

	private:

		composite_method fCompositeMethod = kWeightedSum;

		std::vector<std::shared_ptr<dng_masked_rgb_table> > fTables;

	public:

		dng_masked_rgb_tables ();

		dng_masked_rgb_tables
			(const std::vector<std::shared_ptr<dng_masked_rgb_table> > &tables,
			 composite_method compositeMethod);

		const std::vector<std::shared_ptr<dng_masked_rgb_table> > & Tables () const
			{
			return fTables;
			}
		
		bool IsNOP () const;

		void Validate () const;

		void AddDigest (dng_md5_printer &printer) const;

		void PutStream (dng_stream &stream) const;

		static dng_masked_rgb_tables * GetStream (dng_host &host,
												  dng_stream &stream,
												  bool isDraft);

		composite_method CompositeMethod () const
			{
			return fCompositeMethod;
			}

		bool UseSequentialMethod () const
			{
			return (fCompositeMethod == kSequential);
			}

		bool UseWeightedSumMethod () const
			{
			return !UseSequentialMethod ();
			}

		void SetCompositeMethod (composite_method method)
			{
			fCompositeMethod = method;
			}

		#if qDNGValidate
		void Dump () const;
		#endif
		
	};

/*****************************************************************************/

// Big tables in the XMP_NS_CRS and XMP_NS_CRSS are removed from the
// xmp and added to the big table dictionary.

void MoveBigTablesToDictionary (dng_xmp &xmp,
								dng_big_table_dictionary &dictionary);

/******************************************************************************/

// Parses an XMP block into decoded xmp and a big table dictionary.
// Big tables in the XMP_NS_CRS and XMP_NS_CRSS are removed from the decoded
// xmp and added to the big table dictionary.

void DualParseXMP (dng_host &host,
				   dng_xmp &xmp,
				   dng_big_table_dictionary &dictionary,
				   const void *blockData,
				   uint32 blockSize);

/*****************************************************************************/

#endif	// __dng_big_table__
	
/*****************************************************************************/
