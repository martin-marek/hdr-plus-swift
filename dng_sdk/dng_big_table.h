/*****************************************************************************/
// Copyright 2015-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#ifndef __dng_big_table__
#define __dng_big_table__

/*****************************************************************************/

#include "dng_classes.h"
#include "dng_camera_profile.h"

/*****************************************************************************/

class dng_big_table_cache;
class dng_big_table_storage;

/*****************************************************************************/

void dng_big_table_cache_flush ();

/*****************************************************************************/

class dng_big_table
    {

    protected:

        enum BigTableTypeEnum
            {
            btt_LookTable = 0,
            btt_RGBTable  = 1
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

        virtual bool IsValid () const = 0;

        const dng_fingerprint & Fingerprint () const;

        bool DecodeFromBinary (const uint8 * compressedData,
                               uint32 compressedSize,
                               dng_memory_allocator &allocator);
        
        bool DecodeFromString (const dng_string &block1,
                               dng_memory_allocator &allocator);
        
        dng_memory_block * EncodeAsBinary (dng_memory_allocator &allocator,
                                           uint32 &compressedSize) const;
        
        dng_memory_block * EncodeAsString (dng_memory_allocator &allocator) const;

        bool ExtractFromCache (const dng_fingerprint &fingerprint);

        bool ReadTableFromXMP (const dng_xmp &xmp,
							   const char *ns,
                               const dng_fingerprint &fingerprint);
        
        bool ReadFromXMP (const dng_xmp &xmp,
                          const char *ns,
                          const char *path,
                          dng_big_table_storage &storage);

        void WriteToXMP (dng_xmp &xmp,
                         const char *ns,
                         const char *path,
                         dng_big_table_storage &storage) const;

    protected:

        void RecomputeFingerprint ();

        virtual void GetStream (dng_stream &stream) = 0;

        virtual void PutStream (dng_stream &stream,
                                bool forFingerprint) const = 0;

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

            // Constructor to set defaults.

            table_data ()

                :   fMap        ()
                ,   fEncoding   (encoding_Linear)
                ,   fMinAmount  (1.0)
                ,   fMaxAmount  (1.0)
				,	fMonochrome (false)

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
                   Amount      () == table.Amount      () &&
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

        virtual void GetStream (dng_stream &stream);

        virtual void PutStream (dng_stream &stream,
                                bool forFingerprint) const;

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
            
            kMaxDivisions3D_InMemory = 128

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

            // Sample data.  16-bit unsigned encoding.
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

            // Constructor to set defaults.

            table_data ()

                :   fDimensions (0)
                ,	fDivisions  (0)
                ,	fSamples    ()
                ,   fPrimaries  (primaries_sRGB)
                ,   fGamma      (gamma_sRGB)
                ,   fGamut      (gamut_clip)
                ,   fMinAmount  (0.0)
                ,   fMaxAmount  (2.0)
				,	fMonochrome (false)

                {
                }

			// Compute monchrome flag.
			
			void ComputeMonochrome ()
				{
				
				if (fPrimaries != primaries_ProPhoto &&
					fGamut     != gamut_clip)
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
                   Amount      () == table.Amount      () &&
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

        virtual void GetStream (dng_stream &stream);

        virtual void PutStream (dng_stream &stream,
                                bool forFingerprint) const;

    };

/*****************************************************************************/

#endif
	
/*****************************************************************************/
