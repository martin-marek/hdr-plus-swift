/*****************************************************************************/
// Copyright 2015-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#include "dng_big_table.h"

#include "dng_memory_stream.h"
#include "dng_mutex.h"
#include "dng_stream.h"
#include "dng_xmp.h"

#include "zlib.h"

#include <map>

/*****************************************************************************/

class dng_big_table_cache
    {

    private:

		dng_std_mutex fMutex;

        typedef std::pair <dng_fingerprint,
                           int32> RefCountsPair;

        typedef std::map <dng_fingerprint,
                          int32,
                          dng_fingerprint_less_than> RefCountsMap;
		
		RefCountsMap fRefCounts;

        std::vector<dng_fingerprint> fRecentlyUsed;

    protected:

        enum
            {
            kDefaultRecentlyUsedLimit = 5
            };

        uint32 fRecentlyUsedLimit;

    protected:

        dng_big_table_cache ();

        void UseTable (dng_lock_std_mutex &lock,
                       const dng_fingerprint &fingerprint);

        virtual void CacheIncrement (dng_lock_std_mutex &lock,
                                     const dng_fingerprint &fingerprint);

        virtual void CacheDecrement (dng_lock_std_mutex &lock,
                                     const dng_fingerprint &fingerprint);

        virtual void CacheAdd (dng_lock_std_mutex &lock,
                               const dng_big_table &table);

        virtual bool CacheExtract (dng_lock_std_mutex &lock,
                                   const dng_fingerprint &fingerprint,
                                   dng_big_table &table);

        virtual void InsertTableData (dng_lock_std_mutex &lock,
                                      const dng_big_table &table) = 0;

        virtual void EraseTableData (dng_lock_std_mutex &lock,
                                     const dng_fingerprint &fingerprint) = 0;

        virtual void ExtractTableData (dng_lock_std_mutex &lock,
                                       const dng_fingerprint &fingerprint,
                                       dng_big_table &table) = 0;

    public:

        virtual ~dng_big_table_cache ();

        void FlushRecentlyUsed ();

        static void Increment (dng_big_table_cache *cache,
                               const dng_fingerprint &fingerprint);

        static void Decrement (dng_big_table_cache *cache,
                               const dng_fingerprint &fingerprint);

        static void Add (dng_big_table_cache *cache,
                         const dng_big_table &table);

        static bool Extract (dng_big_table_cache *cache,
                             const dng_fingerprint &fingerprint,
                             dng_big_table &table);

    };

/*****************************************************************************/

dng_big_table_cache::dng_big_table_cache ()

	:	fMutex ()

    ,   fRefCounts ()

    ,   fRecentlyUsed      ()
    ,   fRecentlyUsedLimit (kDefaultRecentlyUsedLimit)

    {

    }

/*****************************************************************************/

dng_big_table_cache::~dng_big_table_cache ()
    {

    }

/*****************************************************************************/

void dng_big_table_cache::UseTable (dng_lock_std_mutex &lock,
                                    const dng_fingerprint &fingerprint)
    {

    // See if fingerprint is in recently used list.

    int32 lastIndex = (int32) fRecentlyUsed.size () - 1;

    for (int32 index = lastIndex; index >= 0; index--)
        {

        if (fingerprint == fRecentlyUsed [index])
            {

            // Move to end of list if not already there.

            if (index != lastIndex)
                {

                fRecentlyUsed.erase (fRecentlyUsed.begin () + index);

                fRecentlyUsed.push_back (fingerprint);

                }

            // Item is in recently used list, so we are done.

            return;

            }

        }

    // Is the recently used list full?

    if (fRecentlyUsed.size () == fRecentlyUsedLimit)
        {

        CacheDecrement (lock, fRecentlyUsed.front ());

        fRecentlyUsed.erase (fRecentlyUsed.begin ());

        }

    // Add to end of list.

    fRecentlyUsed.push_back (fingerprint);

    CacheIncrement (lock, fingerprint);

    }

/*****************************************************************************/

void dng_big_table_cache::FlushRecentlyUsed ()
    {
    
    dng_lock_std_mutex lock (fMutex);

    while (!fRecentlyUsed.empty ())
        {

        CacheDecrement (lock, fRecentlyUsed.back ());

        fRecentlyUsed.pop_back ();

        }

    }

/*****************************************************************************/

void dng_big_table_cache::CacheIncrement (dng_lock_std_mutex &lock,
                                          const dng_fingerprint &fingerprint)
    {

    if (fingerprint.IsValid ())
        {

        RefCountsMap::iterator it = fRefCounts.find (fingerprint);

        if (it == fRefCounts.end ())
            {

            DNG_REPORT ("dng_big_table_cache::CacheIncrement"
                        "fingerprint not in cache");

            }

        else
            {

            it->second++;

            UseTable (lock, fingerprint);

            }

        }

    }

/*****************************************************************************/

void dng_big_table_cache::CacheDecrement (dng_lock_std_mutex &lock,
                                          const dng_fingerprint &fingerprint)
    {

    if (fingerprint.IsValid ())
        {

        RefCountsMap::iterator it = fRefCounts.find (fingerprint);

        if (it == fRefCounts.end ())
            {

            DNG_REPORT ("dng_big_table_cache::CacheDecrement"
                        "fingerprint not in cache");

            }

        else if (--(it->second) == 0)
            {

            fRefCounts.erase (it);

            EraseTableData (lock, fingerprint);

            }

        }

   }

/*****************************************************************************/

void dng_big_table_cache::CacheAdd (dng_lock_std_mutex &lock,
                                    const dng_big_table &table)
    {

    if (table.Fingerprint ().IsValid ())
        {

        RefCountsMap::iterator it = fRefCounts.find (table.Fingerprint ());

        if (it == fRefCounts.end ())
            {

            fRefCounts.insert (RefCountsPair (table.Fingerprint (), 1));

            InsertTableData (lock, table);

            }

        else
            {

            it->second++;

            }

        UseTable (lock, table.Fingerprint ());

        }

    }

/*****************************************************************************/

bool dng_big_table_cache::CacheExtract (dng_lock_std_mutex &lock,
                                        const dng_fingerprint &fingerprint,
                                        dng_big_table &table)
    {

    if (fingerprint.IsValid ())
        {

        RefCountsMap::iterator it = fRefCounts.find (fingerprint);

        if (it != fRefCounts.end ())
            {

            it->second++;

            ExtractTableData (lock, fingerprint, table);

            UseTable (lock, fingerprint);
            
            return true;

            }

        }

    return false;

    }

/*****************************************************************************/

void dng_big_table_cache::Increment (dng_big_table_cache *cache,
                                     const dng_fingerprint &fingerprint)
    {

    if (cache)
        {
        
        dng_lock_std_mutex lock (cache->fMutex);

        cache->CacheIncrement (lock, fingerprint);

        }

    }

/*****************************************************************************/

void dng_big_table_cache::Decrement (dng_big_table_cache *cache,
                                     const dng_fingerprint &fingerprint)
    {

    if (cache)
        {

        dng_lock_std_mutex lock (cache->fMutex);

        cache->CacheDecrement (lock, fingerprint);

        }

    }

/*****************************************************************************/

void dng_big_table_cache::Add (dng_big_table_cache *cache,
                               const dng_big_table &table)
    {

    if (cache)
        {

        dng_lock_std_mutex lock (cache->fMutex);

        cache->CacheAdd (lock, table);

        }

    }

/*****************************************************************************/

bool dng_big_table_cache::Extract (dng_big_table_cache *cache,
                                   const dng_fingerprint &fingerprint,
                                   dng_big_table &table)
    {

    if (cache)
        {

        dng_lock_std_mutex lock (cache->fMutex);

        return cache->CacheExtract (lock, fingerprint, table);

        }

    return false;

    }

/*****************************************************************************/

class dng_look_table_cache : public dng_big_table_cache
    {

    private:

        typedef std::pair <dng_fingerprint,
                           dng_look_table::table_data> TableDataPair;

        typedef std::map <dng_fingerprint,
                          dng_look_table::table_data,
                          dng_fingerprint_less_than> TableDataMap;
		
		TableDataMap fTableData;

    public:

        dng_look_table_cache ()

            :   fTableData ()

            {
            }

        virtual void InsertTableData (dng_lock_std_mutex & /* lock */,
                                      const dng_big_table &table)
            {

            const dng_look_table *lookTable = static_cast
                                              <const dng_look_table *>
                                              (&table);

            fTableData.insert (TableDataPair (lookTable->Fingerprint (),
                                              lookTable->fData));

            }

        virtual void EraseTableData (dng_lock_std_mutex & /* lock */,
                                     const dng_fingerprint &fingerprint)
            {

            TableDataMap::iterator it = fTableData.find (fingerprint);

            if (it != fTableData.end ())
                {

                fTableData.erase (it);

                }

            else
                {

                DNG_REPORT ("dng_look_table_cache::EraseTableData"
                            "fingerprint not in cache");

                }

            }

        virtual void ExtractTableData (dng_lock_std_mutex & /* lock */,
                                       const dng_fingerprint &fingerprint,
                                       dng_big_table &table)
            {

            TableDataMap::iterator it = fTableData.find (fingerprint);

            if (it != fTableData.end ())
                {

                dng_look_table *lookTable = static_cast
                                            <dng_look_table *>
                                            (&table);

                lookTable->fData = it->second;

                }

            else
                {

                DNG_REPORT ("dng_look_table_cache::ExtractTableData"
                            "fingerprint not in cache");

                }

            }

    };

static dng_look_table_cache gLookTableCache;

/*****************************************************************************/

class dng_rgb_table_cache : public dng_big_table_cache
    {

    private:

        typedef std::pair <dng_fingerprint,
                           dng_rgb_table::table_data> TableDataPair;

        typedef std::map <dng_fingerprint,
                          dng_rgb_table::table_data,
                          dng_fingerprint_less_than> TableDataMap;
		
		TableDataMap fTableData;

    public:

        dng_rgb_table_cache ()

            :   fTableData ()

            {
            }

        virtual void InsertTableData (dng_lock_std_mutex & /* lock */,
                                      const dng_big_table &table)
            {

            const dng_rgb_table *rgbTable = static_cast
                                            <const dng_rgb_table *>
                                            (&table);

            fTableData.insert (TableDataPair (rgbTable->Fingerprint (),
                                              rgbTable->fData));

            }

        virtual void EraseTableData (dng_lock_std_mutex & /* lock */,
                                     const dng_fingerprint &fingerprint)
            {

            TableDataMap::iterator it = fTableData.find (fingerprint);

            if (it != fTableData.end ())
                {

                fTableData.erase (it);

                }

            else
                {

                DNG_REPORT ("dng_rgb_table_cache::EraseTableData"
                            "fingerprint not in cache");

                }

            }

        virtual void ExtractTableData (dng_lock_std_mutex & /* lock */,
                                       const dng_fingerprint &fingerprint,
                                       dng_big_table &table)
            {

            TableDataMap::iterator it = fTableData.find (fingerprint);

            if (it != fTableData.end ())
                {

                dng_rgb_table *rgbTable = static_cast
                                          <dng_rgb_table *>
                                          (&table);

                rgbTable->fData = it->second;

                }

            else
                {

                DNG_REPORT ("dng_rgb_table_cache::ExtractTableData"
                            "fingerprint not in cache");

                }

            }

    };

static dng_rgb_table_cache gRGBTableCache;

/*****************************************************************************/

void dng_big_table_cache_flush ()
    {

    gLookTableCache.FlushRecentlyUsed ();

    gRGBTableCache.FlushRecentlyUsed ();

    }

/*****************************************************************************/

dng_big_table::dng_big_table (dng_big_table_cache *cache)

    :   fFingerprint ()
    ,   fCache       (cache)
    ,   fIsMissing   (false)

    {

    }
    
/*****************************************************************************/

dng_big_table::dng_big_table (const dng_big_table &table)

    :   fFingerprint (table.fFingerprint)
    ,   fCache       (table.fCache      )
    ,   fIsMissing   (false             )

    {

    dng_big_table_cache::Increment (fCache, fFingerprint);

    }
    
/*****************************************************************************/

dng_big_table & dng_big_table::operator= (const dng_big_table &table)
    {

    if (fFingerprint != table.fFingerprint ||
        fCache       != table.fCache       )
        {

        dng_big_table_cache::Decrement (fCache, fFingerprint);

        fFingerprint = table.fFingerprint;
        fCache       = table.fCache;

        dng_big_table_cache::Increment (fCache, fFingerprint);

        }

    return *this;

    }
    
/*****************************************************************************/

dng_big_table::~dng_big_table ()
    {

    dng_big_table_cache::Decrement (fCache, fFingerprint);

    }

/*****************************************************************************/

const dng_fingerprint & dng_big_table::Fingerprint () const
	{

	return fFingerprint;

	}

/*****************************************************************************/

void dng_big_table::RecomputeFingerprint ()
    {

    dng_big_table_cache::Decrement (fCache, fFingerprint);

    fFingerprint.Clear ();

    if (IsValid ())
        {

            {

            dng_md5_printer_stream stream;

            stream.SetLittleEndian ();

            PutStream (stream, true);

            fFingerprint = stream.Result ();

            }

        // Try extract first to force sharing of table data memory if
        // table data is already in cache.

        if (!dng_big_table_cache::Extract (fCache, fFingerprint, *this))
            {

            // Otherwise add to cache.

            dng_big_table_cache::Add (fCache, *this);

            }

        }

    }

/*****************************************************************************/

bool dng_big_table::DecodeFromBinary (const uint8 *compressedData,
                                      uint32 compressedSize,
                                      dng_memory_allocator &allocator)
    {

    // Decompress the data.

    AutoPtr<dng_memory_block> block3;

        {

        if (compressedSize < 5)
            {
            return false;
            }

        // Uncompressed size is stored in first four bytes of decoded data,
        // little endian order.

        uint32 uncompressedSize = (((uint32) compressedData [0])      ) +
                                  (((uint32) compressedData [1]) <<  8) +
                                  (((uint32) compressedData [2]) << 16) +
                                  (((uint32) compressedData [3]) << 24);

        block3.Reset (allocator.Allocate (uncompressedSize));

        uLongf destLen = uncompressedSize;

        int zResult = ::uncompress (block3->Buffer_uint8 (),
                                    &destLen,
                                    compressedData + 4,
                                    compressedSize - 4);

        if (zResult != Z_OK)
            {
            return false;
            }

        }

    // Now read in the table data from the uncompressed stream.

        {

        dng_stream stream (block3->Buffer      (),
                           block3->LogicalSize ());

        stream.SetLittleEndian ();

        GetStream (stream);

        block3.Reset ();

        }

    // Force recomputation of fingerprint.

    RecomputeFingerprint ();

    return true;
    
    }

/*****************************************************************************/

bool dng_big_table::DecodeFromString (const dng_string &block1,
                                      dng_memory_allocator &allocator)
    {

    // Decode the text to binary.

    AutoPtr<dng_memory_block> block2;

    uint32 compressedSize = 0;

        {

        // This binary to text encoding is very similar to the Z85
        // encoding, but the exact charactor set has been adjusted to
        // encode more cleanly into XMP.

        static uint8 kDecodeTable [96] =
            {

            0xFF,   // space
            0x44,   // !
            0xFF,   // "
            0x54,   // #
            0x53,   // $
            0x52,   // %
            0xFF,   // &
            0x49,   // '
            0x4B,   // (
            0x4C,   // )
            0x46,   // *
            0x41,   // +
            0xFF,   // ,
            0x3F,   // -
            0x3E,   // .
            0x45,   // /

            0x00, 0x01, 0x02, 0x03, 0x04,   // 01234
            0x05, 0x06, 0x07, 0x08, 0x09,   // 56789

            0x40,   // :
            0xFF,   // ;
            0xFF,   // <
            0x42,   // =
            0xFF,   // >
            0x47,   // ?
            0x51,   // @

            0x24, 0x25, 0x26, 0x27, 0x28,   // ABCDE
            0x29, 0x2A, 0x2B, 0x2C, 0x2D,   // FGHIJ
            0x2E, 0x2F, 0x30, 0x31, 0x32,   // KLMNO
            0x33, 0x34, 0x35, 0x36, 0x37,   // PQRST
            0x38, 0x39, 0x3A, 0x3B, 0x3C,   // UVWXY
            0x3D,                           // Z

            0x4D,   // [
            0xFF,   // backslash
            0x4E,   // ]
            0x43,   // ^
            0xFF,   // _
            0x48,   // `

            0x0A, 0x0B, 0x0C, 0x0D, 0x0E,   // abcde
            0x0F, 0x10, 0x11, 0x12, 0x13,   // fghij
            0x14, 0x15, 0x16, 0x17, 0x18,   // klmno
            0x19, 0x1A, 0x1B, 0x1C, 0x1D,   // pqrst
            0x1E, 0x1F, 0x20, 0x21, 0x22,   // uvwxy
            0x23,                           // z

            0x4F,   // {
            0x4A,   // |
            0x50,   // }
            0xFF,   // ~
            0xFF    // del

            };

        uint32 encodedSize = block1.Length ();

        uint32 maxCompressedSize = (encodedSize + 4) / 5 * 4;

        block2.Reset (allocator.Allocate (maxCompressedSize));

        uint32 phase = 0;
        uint32 value;

        uint8 *dPtr = block2->Buffer_uint8 ();

        for (const char *sPtr = block1.Get (); *sPtr; sPtr++)
            {

            uint8 e = (uint8) *sPtr;

            if (e < 32 || e > 127)
                {
                continue;
                }

            uint32 d = kDecodeTable [e - 32];

            if (d > 85)
                {
                continue;
                }

            phase++;

            if (phase == 1)
                {
                value = d;
                }

            else if (phase == 2)
                {
                value += d * 85;
                }

            else if (phase == 3)
                {
                value += d * (85 * 85);
                }

            else if (phase == 4)
                {
                value += d * (85 * 85 * 85);
                }

            else
                {

                value += d * (85 * 85 * 85 * 85);

                dPtr [0] = (uint8) (value      );
                dPtr [1] = (uint8) (value >>  8);
                dPtr [2] = (uint8) (value >> 16);
                dPtr [3] = (uint8) (value >> 24);

                dPtr += 4;

                compressedSize += 4;

                phase = 0;

                }

            }

        if (phase > 3)
            {

            dPtr [2] = (uint8) (value >> 16);

            compressedSize++;

            }

        if (phase > 2)
            {

            dPtr [1] = (uint8) (value >> 8);

            compressedSize++;

            }

        if (phase > 1)
            {

            dPtr [0] = (uint8) (value);

            compressedSize++;

            }

        }

    return DecodeFromBinary (block2->Buffer_uint8 (),
                             compressedSize,
                             allocator);

    }

/*****************************************************************************/

dng_memory_block* dng_big_table::EncodeAsBinary (dng_memory_allocator &allocator,
                                                 uint32 &compressedSize) const
    {

    // Stream to a binary block..

    AutoPtr<dng_memory_block> block1;

        {

        dng_memory_stream stream (allocator);

        stream.SetLittleEndian ();

        PutStream (stream, false);

        block1.Reset (stream.AsMemoryBlock (allocator));

        }

    // Compress the block.

    AutoPtr<dng_memory_block> block2;

        {

        uint32 uncompressedSize = block1->LogicalSize ();

        uint32 safeCompressedSize = uncompressedSize + (uncompressedSize >> 8) + 64;

        block2.Reset (allocator.Allocate (safeCompressedSize + 4));

        // Store uncompressed size in first four bytes of compressed block.

        uint8 *dPtr = block2->Buffer_uint8 ();

        dPtr [0] = (uint8) (uncompressedSize      );
        dPtr [1] = (uint8) (uncompressedSize >>  8);
        dPtr [2] = (uint8) (uncompressedSize >> 16);
        dPtr [3] = (uint8) (uncompressedSize >> 24);

        uLongf dCount = safeCompressedSize;

        int zResult = ::compress2 (dPtr + 4,
                                   &dCount,
                                   block1->Buffer_uint8 (),
                                   uncompressedSize,
                                   Z_DEFAULT_COMPRESSION);
                                  
        if (zResult != Z_OK)
            {
            ThrowMemoryFull ();
            }

        compressedSize = (uint32) dCount + 4;

        block1.Reset ();

        }
        
    return block2.Release ();

    }
        
/*****************************************************************************/

dng_memory_block* dng_big_table::EncodeAsString (dng_memory_allocator &allocator) const
    {

    // Get compressed binary data.
    
    uint32 compressedSize;

    AutoPtr<dng_memory_block> block2 (EncodeAsBinary (allocator, compressedSize));

    // Encode binary to text.

    AutoPtr<dng_memory_block> block3;

        {

        // This binary to text encoding is very similar to the Z85
        // encoding, but the exact charactor set has been adjusted to
        // encode more cleanly into XMP.

        static const char *kEncodeTable =
            "0123456789"
            "abcdefghij" 
            "klmnopqrst" 
            "uvwxyzABCD"
            "EFGHIJKLMN" 
            "OPQRSTUVWX" 
            "YZ.-:+=^!/" 
            "*?`'|()[]{"
            "}@%$#";

        uint32 safeEncodedSize = compressedSize +
                                 (compressedSize >> 2) +
                                 (compressedSize >> 6) +
                                 16;

        block3.Reset (allocator.Allocate (safeEncodedSize));

        uint8 *sPtr = block2->Buffer_uint8 ();

        sPtr [compressedSize    ] = 0;
        sPtr [compressedSize + 1] = 0;
        sPtr [compressedSize + 2] = 0;

        uint8 *dPtr = block3->Buffer_uint8 ();

        while (compressedSize)
            {

            uint32 x0 = (((uint32) sPtr [0])      ) +
                        (((uint32) sPtr [1]) <<  8) +
                        (((uint32) sPtr [2]) << 16) +
                        (((uint32) sPtr [3]) << 24);

            sPtr += 4;

            uint32 x1 = x0 / 85;

            *dPtr++ = kEncodeTable [x0 - x1 * 85];

            uint32 x2 = x1 / 85;

            *dPtr++ = kEncodeTable [x1 - x2 * 85];

            if (!--compressedSize)
                break;

            uint32 x3 = x2 / 85;

            *dPtr++ = kEncodeTable [x2 - x3 * 85];

            if (!--compressedSize)
                break;

            uint32 x4 = x3 / 85;

            *dPtr++ = kEncodeTable [x3 - x4 * 85];

            if (!--compressedSize)
                break;

            *dPtr++ = kEncodeTable [x4];

            compressedSize--;

            }

        *dPtr = 0;

        block2.Reset ();

        }

    return block3.Release ();

    }
        
/*****************************************************************************/

bool dng_big_table::ExtractFromCache (const dng_fingerprint &fingerprint)
    {

    if (dng_big_table_cache::Extract (fCache, fingerprint, *this))
        {

        fFingerprint = fingerprint;

        return true;

        }

    return false;
	
    }
        
/*****************************************************************************/

bool dng_big_table::ReadTableFromXMP (const dng_xmp &xmp,
									  const char *ns,
                                      const dng_fingerprint &fingerprint)
    {

	// Read in the table data.
	
	dng_string tablePath;
	
	tablePath.Set ("Table_");
	
	tablePath.Append (dng_xmp::EncodeFingerprint (fingerprint).Get ());
	
	dng_string block1;
	
	if (!xmp.GetString (ns,
						tablePath.Get (),
						block1))
		{
		
		DNG_REPORT ("Missing big table data");
		
		return false;
		
		}
	
	bool ok = DecodeFromString (block1,
								xmp.Allocator ());
		
	block1.Clear ();
		
	// Validate fingerprint match.
	
	DNG_ASSERT (Fingerprint () == fingerprint,
				"dng_big_table fingerprint mismatch");
	
	// It worked!

	return ok;

    }
        
/*****************************************************************************/

bool dng_big_table::ReadFromXMP (const dng_xmp &xmp,
								 const char *ns,
								 const char *path,
                                 dng_big_table_storage &storage)
	{
	
	dng_fingerprint fingerprint;
	
	if (!xmp.GetFingerprint (ns, path, fingerprint))
		{
		
		return false;
		
		}
        
    // See if we can skip reading the table data, and just grab from cache.

    if (ExtractFromCache (fingerprint))
        {

        return true;

        }
        
    // Next see if we can get the table from the storage object.
    
    if (storage.ReadTable (*this, fingerprint, xmp.Allocator ()))
        {
        
        return true;
        
        }

	// Read in the table data.

    if (ReadTableFromXMP (xmp, ns, fingerprint))
        {
        
        return true;
        
        }
        
    // Unable to find table data anywhere.  Notify storage object.
    
    storage.MissingTable (fingerprint);
    
    // Also make a note that this table is missing.
    
    SetMissing ();
        
    return false;
	
	}

/*****************************************************************************/

void dng_big_table::WriteToXMP (dng_xmp &xmp,
                                const char *ns,
                                const char *path,
                                dng_big_table_storage &storage) const
    {
        
    const dng_fingerprint &fingerprint = Fingerprint ();
    
    if (!fingerprint.IsValid () || IsMissing ())
        {
        
        xmp.Remove (ns, path);
        
        return;
        
        }
    
    xmp.SetFingerprint (ns, path, fingerprint);
    
    // See if we can just use the storage object to store the table.
    
    if (storage.WriteTable (*this, fingerprint, xmp.Allocator ()))
        {
        
        return;
        
        }
    
    dng_string tablePath;
    
    tablePath.Set ("Table_");
    
    tablePath.Append (dng_xmp::EncodeFingerprint (fingerprint).Get ());
    
    if (xmp.Exists (ns, tablePath.Get ()))
        {
        
        return;
        
        }
        
    AutoPtr<dng_memory_block> block;
    
    block.Reset (EncodeAsString (xmp.Allocator ()));

    xmp.Set (ns,
             tablePath.Get (),
             block->Buffer_char ());

    }

/*****************************************************************************/

dng_big_table_storage::dng_big_table_storage ()
    {
    
    }

/*****************************************************************************/

dng_big_table_storage::~dng_big_table_storage ()
    {
    
    }
    
/*****************************************************************************/

bool dng_big_table_storage::ReadTable (dng_big_table & /* table */,
                                       const dng_fingerprint & /* fingerprint */,
                                       dng_memory_allocator & /* allocator */)
    {
    
    return false;
    
    }
    
/*****************************************************************************/

bool dng_big_table_storage::WriteTable (const dng_big_table & /* table */,
                                        const dng_fingerprint & /* fingerprint */,
                                        dng_memory_allocator & /* allocator */)
    {
    
    return false;
    
    }
    
/*****************************************************************************/

void dng_big_table_storage::MissingTable (const dng_fingerprint & /* fingerprint */)
    {
    
    }
    
/*****************************************************************************/

dng_look_table::dng_look_table ()

    :   dng_big_table (&gLookTableCache)

	,	fData   ()
    ,   fAmount (1.0)

    {

    }

/*****************************************************************************/

dng_look_table::dng_look_table (const dng_look_table &table)

    :   dng_big_table (table)

    ,   fData   (table.fData)
    ,   fAmount (table.fAmount)

    {

    }

/*****************************************************************************/

dng_look_table & dng_look_table::operator= (const dng_look_table &table)
    {

    dng_big_table::operator= (table);

    fData   = table.fData;
    fAmount = table.fAmount;

    return *this;

    }

/*****************************************************************************/

dng_look_table::~dng_look_table ()
    {

    }

/*****************************************************************************/

void dng_look_table::Set (const dng_hue_sat_map &map,
                          uint32 encoding)
    {

    fData.fMap      = map;
    fData.fEncoding = encoding;
	
	fData.ComputeMonochrome ();
	
    RecomputeFingerprint ();

    }

/*****************************************************************************/

bool dng_look_table::IsValid () const
    {
    
    if (IsMissing ())
        {
        return false;
        }

    return fData.fMap.IsValid ();

    }

/*****************************************************************************/

void dng_look_table::SetInvalid ()
    {

    *this = dng_look_table ();

    }

/*****************************************************************************/

void dng_look_table::GetStream (dng_stream &stream)
    {

    table_data data;

    if (stream.Get_uint32 () != btt_LookTable)
        {
        ThrowBadFormat ("Not a look table");
        }
        
    uint32 version = stream.Get_uint32 ();

    if (version != kLookTableVersion1 &&
        version != kLookTableVersion2)
        {
        ThrowBadFormat ("Unknown look table version");
        }

    uint32 hueDivisions = stream.Get_uint32 ();
    uint32 satDivisions = stream.Get_uint32 ();
    uint32 valDivisions = stream.Get_uint32 ();

    if (hueDivisions < 1 || hueDivisions > kMaxHueSamples ||
        satDivisions < 1 || satDivisions > kMaxSatSamples ||
        valDivisions < 1 || valDivisions > kMaxValSamples ||
        (dng_safe_uint32 (hueDivisions) *
         dng_safe_uint32 (satDivisions) *
         dng_safe_uint32 (valDivisions)).Get () > kMaxTotalSamples)
        {
        ThrowBadFormat ();
        }

    data.fMap.SetDivisions (hueDivisions,
                            satDivisions,
                            valDivisions);

    uint32 count = data.fMap.DeltasCount ();

    dng_hue_sat_map::HSBModify * deltas = data.fMap.GetDeltas ();
	
    for (uint32 index = 0; index < count; index++)
        {

        deltas->fHueShift = stream.Get_real32 ();
        deltas->fSatScale = stream.Get_real32 ();
        deltas->fValScale = stream.Get_real32 ();
		
        deltas++;

        }

	data.fMap.AssignNewUniqueRuntimeFingerprint ();

    data.fEncoding = stream.Get_uint32 ();
	
    if (data.fEncoding != encoding_Linear &&
        data.fEncoding != encoding_sRGB)
        {
        ThrowBadFormat ("Unknown look table encoding");
        }
        
    if (version != kLookTableVersion1)
        {
        
        data.fMinAmount = stream.Get_real64 ();
        data.fMaxAmount = stream.Get_real64 ();

        if (data.fMinAmount < 0.0 || data.fMinAmount > 1.0 || data.fMaxAmount < 1.0)
            {
            ThrowBadFormat ("Invalid min/max amount for look table");
            }
        
        }
        
    else
        {
        
        data.fMinAmount = 1.0;
        data.fMaxAmount = 1.0;
        
        }

	data.ComputeMonochrome ();

    fData = data;

    }

/*****************************************************************************/

void dng_look_table::PutStream (dng_stream &stream,
                                bool /* forFingerprint */) const
    {

    DNG_REQUIRE (IsValid (), "Invalid Look Table");

    stream.Put_uint32 (btt_LookTable);
    
    uint32 version = kLookTableVersion1;
    
    if (fData.fMinAmount != 1.0 ||
        fData.fMaxAmount != 1.0)
        {
        version = kLookTableVersion2;
        }

    stream.Put_uint32 (version);

    uint32 hueDivisions;
    uint32 satDivisions;
    uint32 valDivisions;

    fData.fMap.GetDivisions (hueDivisions,
                             satDivisions,
                             valDivisions);

    stream.Put_uint32 (hueDivisions);
    stream.Put_uint32 (satDivisions);
    stream.Put_uint32 (valDivisions);

    uint32 count = fData.fMap.DeltasCount ();

    const dng_hue_sat_map::HSBModify * deltas = fData.fMap.GetConstDeltas ();

    for (uint32 index = 0; index < count; index++)
        {

        stream.Put_real32 (deltas->fHueShift);
        stream.Put_real32 (deltas->fSatScale);
        stream.Put_real32 (deltas->fValScale);

        deltas++;

        }

    stream.Put_uint32 (fData.fEncoding);

    if (version != kLookTableVersion1)
        {
        
        stream.Put_real64 (fData.fMinAmount);
        stream.Put_real64 (fData.fMaxAmount);

        }
        
    }

/*****************************************************************************/

dng_rgb_table::dng_rgb_table ()

    :   dng_big_table (&gRGBTableCache)

    ,   fData   ()
    ,   fAmount (1.0)

    {

    }

/*****************************************************************************/

dng_rgb_table::dng_rgb_table (const dng_rgb_table &table)

    :   dng_big_table (table)

    ,   fData   (table.fData)
    ,   fAmount (table.fAmount)

    {

    }

/*****************************************************************************/

dng_rgb_table & dng_rgb_table::operator= (const dng_rgb_table &table)
    {

    dng_big_table::operator= (table);

    fData   = table.fData;
    fAmount = table.fAmount;

    return *this;

    }

/*****************************************************************************/

dng_rgb_table::~dng_rgb_table ()
    {

    }

/*****************************************************************************/

bool dng_rgb_table::IsValid () const
    {

    if (IsMissing ())
        {
        return false;
        }

    // If table itself is invalid, then invalid.

    if (fData.fDimensions == 0)
        return false;

    // If table has some effect, then valid.

    if (fAmount > 0.0)
        return true;

    // Does the matrix itself do any clipping?

    if (fData.fPrimaries == primaries_ProPhoto ||
        fData.fGamut     == gamut_extend       )
        return false;

    // Table is a NOP but there is some gamut clipping.

    return true;

    }

/*****************************************************************************/

void dng_rgb_table::SetInvalid ()
    {

    *this = dng_rgb_table ();

    }

/*****************************************************************************/

void dng_rgb_table::Set (uint32 dimensions,
                         uint32 divisions,
                         dng_ref_counted_block samples)
    {

    if (dimensions == 1)
        {

        if (divisions < kMinDivisions1D ||
            divisions > kMaxDivisions1D)
            {

            ThrowProgramError ("Bad 1D divisions");

            }

        if (samples.LogicalSize () != divisions * 4 * sizeof (uint16))
            {

            ThrowProgramError ("Bad 1D sample count");

            }

        }

    else if (dimensions == 3)
        {

        if (divisions < kMinDivisions3D ||
            divisions > kMaxDivisions3D_InMemory)
            {

            ThrowProgramError ("Bad 3D divisions");

            }

        if (samples.LogicalSize () != divisions *
                                      divisions *
                                      divisions * 4 * sizeof (uint16))
            {

            ThrowProgramError ("Bad 3D sample count");

            }

        }

    else
        {

        ThrowProgramError ("Bad dimensions");

        }

    fData.fDimensions = dimensions;

    fData.fDivisions = divisions;

    fData.fSamples = samples;

	fData.ComputeMonochrome ();

    RecomputeFingerprint ();

    }

/*****************************************************************************/

void dng_rgb_table::GetStream (dng_stream &stream)
    {

    table_data data;

    if (stream.Get_uint32 () != btt_RGBTable)
        {
        ThrowBadFormat ("Not a RGB table");
        }

    if (stream.Get_uint32 () != kRGBTableVersion)
        {
        ThrowBadFormat ("Unknown RGB table version");
        }

    data.fDimensions = stream.Get_uint32 ();

    data.fDivisions = stream.Get_uint32 ();

    if (data.fDimensions == 1)
        {

        if (data.fDivisions < kMinDivisions1D ||
            data.fDivisions > kMaxDivisions1D)
            {
            ThrowBadFormat ("Invalid 1D divisions");
            }

        }

    else if (data.fDimensions == 3)
        {

        if (data.fDivisions < kMinDivisions3D ||
            data.fDivisions > kMaxDivisions3D)
            {
            ThrowBadFormat ("Invalid 3D divisions");
            }

        }

    else
        {
        ThrowBadFormat ("Invalid dimensions");
        }

    uint16 nopValue [kMaxDivisions1D > kMaxDivisions3D ? kMaxDivisions1D
                                                       : kMaxDivisions3D];

    for (uint32 index = 0; index < data.fDivisions; index++)
        {

        nopValue [index] = (uint16)
                           ((index * 0x0FFFF + (data.fDivisions >> 1)) /
                            (data.fDivisions - 1));

        }

    if (data.fDimensions == 1)
        {

        data.fSamples.Allocate (data.fDivisions * 4 * sizeof (uint16));

        uint16 *samples = data.fSamples.Buffer_uint16 ();

        for (uint32 index = 0; index < data.fDivisions; index++)
            {

            samples [0] = stream.Get_uint16 () + nopValue [index];
            samples [1] = stream.Get_uint16 () + nopValue [index];
            samples [2] = stream.Get_uint16 () + nopValue [index];
            samples [3] = 0;

            samples += 4;

            }

        }

    else
        {

        data.fSamples.Allocate (data.fDivisions *
                                data.fDivisions *
                                data.fDivisions * 4 * sizeof (uint16));

        uint16 *samples = data.fSamples.Buffer_uint16 ();

        for (uint32 rIndex = 0; rIndex < data.fDivisions; rIndex++)
            {

            for (uint32 gIndex = 0; gIndex < data.fDivisions; gIndex++)
                {

                for (uint32 bIndex = 0; bIndex < data.fDivisions; bIndex++)
                    {

                    samples [0] = stream.Get_uint16 () + nopValue [rIndex];
                    samples [1] = stream.Get_uint16 () + nopValue [gIndex];
                    samples [2] = stream.Get_uint16 () + nopValue [bIndex];
                    samples [3] = 0;

                    samples += 4;

                    }

                }

            }

        }

    uint32 primaries = stream.Get_uint32 ();

    if (primaries >= primaries_count)
        {
        ThrowBadFormat ("Unknown RGB table primaries");
        }

    data.fPrimaries = (primaries_enum) primaries;

    uint32 gamma = stream.Get_uint32 ();

    if (gamma >= gamma_count)
        {
        ThrowBadFormat ("Unknown RGB table gamma");
        }

    data.fGamma = (gamma_enum) gamma;

    uint32 gamut = stream.Get_uint32 ();

    if (gamut >= gamut_count)
        {
        ThrowBadFormat ("Unknown RGB table gamut processing option");
        }

    data.fGamut = (gamut_enum) gamut;

    data.fMinAmount = stream.Get_real64 ();
    data.fMaxAmount = stream.Get_real64 ();

    if (data.fMinAmount < 0.0 || data.fMinAmount > 1.0 || data.fMaxAmount < 1.0)
        {
        ThrowBadFormat ("Invalid min/max amount for RGB table");
        }
		
	data.ComputeMonochrome ();

    fData = data;

    }

/*****************************************************************************/

void dng_rgb_table::PutStream (dng_stream &stream,
                               bool /* forFingerprint */) const
    {

    DNG_REQUIRE (IsValid (), "Invalid RGB Table");

    stream.Put_uint32 (btt_RGBTable);

    stream.Put_uint32 (kRGBTableVersion);

    stream.Put_uint32 (fData.fDimensions);

    stream.Put_uint32 (fData.fDivisions);
    
    uint16 nopValue [kMaxDivisions1D > kMaxDivisions3D_InMemory ? kMaxDivisions1D
                                                                : kMaxDivisions3D_InMemory];

    for (uint32 index = 0; index < fData.fDivisions; index++)
        {

        nopValue [index] = (uint16)
                           ((index * 0x0FFFF + (fData.fDivisions >> 1)) /
                            (fData.fDivisions - 1));

        }

    const uint16 *samples = fData.fSamples.Buffer_uint16 ();

    if (fData.fDimensions == 1)
        {

        for (uint32 index = 0; index < fData.fDivisions; index++)
            {

            stream.Put_uint16 (samples [0] - nopValue [index]);
            stream.Put_uint16 (samples [1] - nopValue [index]);
            stream.Put_uint16 (samples [2] - nopValue [index]);

            samples += 4;

            }

        }

    else
        {

        for (uint32 rIndex = 0; rIndex < fData.fDivisions; rIndex++)
            {

            for (uint32 gIndex = 0; gIndex < fData.fDivisions; gIndex++)
                {

                for (uint32 bIndex = 0; bIndex < fData.fDivisions; bIndex++)
                    {

                    stream.Put_uint16 (samples [0] - nopValue [rIndex]);
                    stream.Put_uint16 (samples [1] - nopValue [gIndex]);
                    stream.Put_uint16 (samples [2] - nopValue [bIndex]);

                    samples += 4;

                    }

                }

            }

        }

    stream.Put_uint32 ((uint32) fData.fPrimaries);

    stream.Put_uint32 ((uint32) fData.fGamma);

    stream.Put_uint32 ((uint32) fData.fGamut);

    stream.Put_real64 (fData.fMinAmount);
    stream.Put_real64 (fData.fMaxAmount);

    }

/*****************************************************************************/
