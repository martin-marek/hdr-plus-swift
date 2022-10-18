#ifndef __XMP_Const_h__
#define __XMP_Const_h__ 1

// =================================================================================================
// Copyright 2002 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#if AdobePrivate
// =================================================================================================
// Change history
// ==============
//
// Writers:
//  AWL Alan Lillich
//  SM  Stefan Makswit
//  IJS Inder Jeet singh
//  ADC Amandeep Chawla
//  AB  Amit Bhatti
//
// mm-dd-yy who Description of changes, most recent on top.
//
// 03-27-15 ADC 5.5-c066 Porting changes to gcc 4.4.4.
// 03-24-15 ADC 5.6-c061 Adding inbuilt support for ixml namespace.
// 03-18-15 AJ  5.6-c052 Adding shared ptr for the private data in the marker structure
// 03-16-15 AJ  5.6-c050 Fixing build break in pdfl due to warning encountered
// 03-16-15 AJ  5.6-c049 Adding a copy constructor and assignment operator to MarkerStructure to handle shallow copy scenarios for extensions
// 03-16-15 AJ  5.6-c047 Completing Rework of Get/SetBulkMarkers()
// 10-21-14 AB  5.6.f120 [3836549] Making kXMPFiles_OptimizeFileLayout flag public for XMP SDK.
// 01-31-14 IJS 5.5-c022 Adding serialize flag kXMP_IncludeRDFHash to  include the  hash of the RDF in xmppacket.
// 01-31-14 IJS 5.6-f091 Reverting Changes Done in 5.6-f089
// 01-20-14 ADC 5.6-f089 [3696245] Generic Handler: Generic handler returns kXMP_Unknown type for handler->format.
// 01-16-14 IJS 5.6-f088 Generic Handling Feature - Adding support for kXMPFiles_OpenUseGenericHandler OpenFile() flag.
// 06-26-13 ADC 5.5-c019 [3083403] Failure to open a PDF file.
//						 GetProperty_Int, GetProperty_Int64 and GetProperrty_Float will tolerate presence of
//						 whitespace characters at the beginning and end.//
// 06-11-13 IJS 5.5-f066 Moving #include vector out of AdobePrivate as now non-AdobePrivate function APIs in XMPFiles also 
//						 need declaration for vector to compile successfully
// 05-13-13 ADC 5.5-c018 Removing usage of bool in APIs exposed at DLL boundaries.
// 05-13-13 ADC 5.6-f060 Removing usage of bool in APIs exposed at DLL boundaries.
// 02-27-13 ADC 5.6-f043 Adding support for preloading plugins based on handler flags. PDF Handler will support preloading.
// 11-01-12 IJS 5.5-f055 Changes done to support R3D Plugin Handler
// 10-18-12 ADC 5.5-f051 [3347406] XMPFiles::OpenFile should throw an error when it is given a path to a file that is of an
//						 unsupported extension type along with the flag kXMPFiles_OpenUseSmartHandler.
// 10-12-12 ADC 5.5-f049 [3345417] XMPFiles::OpenFile() does not throw any error when the path to a folder is given to the function.
//						 Added support for new Error code kXMPErr_UnsupportedFileExtension.
// 10-10-12 ADC 5.5-c012 Changed internal implementation of common error notification infrastructure.
// 09-21-12 AWL 5.5-c011 Remove Transform XMP.
// 09-20-12 IJS 5.5-c010 Add progress notifications flag for file handlers.
// 08-01-12 AWL 5.5-c004 Implement public API and glue layer for XMPCore error notifications.
// 06-22-12 AWL 5.5-f019 Add file update progress tracking to the MPEG-4 handler.
// 06-20-12 AWL 5.5-f018 Add outer layers for XMPFiles progress notifications, no handlers use it yet.
//
// 10-27-11 AWL 5.4-f030 [3007461] Improve performance of GetFileModDate.
//
// 07-28-11 AWL 5.3-c007 Add the AEScart: namespace for the AES cart chunk in WAV files.
// 07-21-11 AWL 5.3-c006 Add the exifx: namespace for CIPA definition of XMP with Exif 2.3.
// 12-15-10 AWL 5.3-c005 Add support for canonical RDF serialization.
// 10-26-10 PW  5.3-c004 Integration from winwood: Adding transient namespace RIFFINFO.
// 09-30-10	AWL	5.3-f010 Add Canon XF handler from Peter Lee.
// 08-20-10	SM 	5.3-c001 Adding namespace for IPTC Extension Schema (IPTC4xmpExt)
// 08-19-10 AWL 5.3-f004 Move the seek mode constants to XMP_Const.
//
// 05-27-10 AWL 5.1-c006 [2598001] Add namespace for PLUS.
//
// 12-01-09 AWL 5.0-c059 [2415230] Fix IsInternalProperty for recent xmpDM: and xmpScript: external properties.
// 10-05-09 AWL 5.0-f083 Add MPEG-4 support for kXMPFiles_OptimizeFileLayout.
// 08-18-09 FNO 5.0-c050f63 RIFF-Handler: Add full bext chunk read support.
// 07-21-09 AWL 5.0-c047 Add XMPUtils::ApplyTemplate.
// 05-12-09 AWL 5.0-c029-f037 Finish deprecated function removal.
// 05-04-09 AWL 5.0-c026 Remove compact structs and arrays from TransformXMP.
// 02-17-09 FNO 5.0-c023 [0656083] Remove moot kXMP_WriteAliasComments option.
// 04-03-09 AWL 5.0-c022 Change PlainXMP to TransformXMP.
// 11-13-08 AWL 5.0-f004 Add server mode support that ignores local text. Enable all handlers except
//				MOV for generic UNIX - that will be handled as part of the rewrite.
//
// 10-13-08 AWL 4.4-c009 Add support for zone-less times. Get rid of old XMPUtils forms of DocOps.
// 07-05-08 AWL 4.4-f004 [1750725,1831407] Add pre-checks for QT file updates with optional repair.
//
// 07-01-08 AWL 4.3-c060 Hide all uses of PlainXMP.
//
// 04-29-08 AWL 4.2.2-c052 [1768777,1768782] Add private performance enhancements for temporal metadata.
// 04-24-08 AWL 4.2.2-c051 [1765983] Add SXMPDocOps::IsDirty overload to return reason mask.
// 04-04-08 AWL 4.2.2-c049 [1744200] Add the DICOM namespace.
// 03-31-08 AWL 4.2.2-c047 Add kXMP_NS_CreatorAtom standard namespace.
//
// 03-06-08 AWL 4.2-f093 Change the XDCAM file type constants to be explicit about all 3: FAM/SAM/EX.
// 02-22-08 AWL 4.2-c042 Finish implementation of SXMPDocOps::EnsureIDsExist.
// 02-05-08 AWL 4.2-f069 Integrate latest advanced video handlers.
// 02-05-08 AWL Add the standard SXMPDocOps part names.
// 12-31-07 AWL 4.2-f051 Add initial Sony HDV handler under NewHandlers.
// 12-20-07 AWL Change the XMP_PacketInfo constructor to initialize the offset and length to -1
//				(unknown) instead of 0.
// 12-05-07 AWL Add file type constants for XDCAM and AVCHD.
// 11-16-07 AWL 4.2-f037 Introduce basic infrastructure for "directory oriented" handlers.
// 10-31-07 AWL 4.2-c023 Add new class XMPDocOps.
// 10-10-07 AWL 4.2-f027 Add a no-packet-wrapper flag to XMP_PacketInfo.
// 10-09-07 AWL 4.2-c021 Add support for kXMP_OmitXMPMetaElement, no x:xmpmeta when serializing.
// 08-02-07 AWL 4.2 Incorporate major revamp to doxygen comments.
//
// 03-30-07 AWL 4.2-c014 [1519329] Tolerate unknown namespaces in XMPIterator.cpp, AddSchemaAliases.
//				Add  PDF/X-ID to the set of standard namespaces.
// 03-26-07 AWL 4.2-c012 Add standard namespace for the WAV legacy digest.
// 01-11-07 AWL 4.2-f007 [1454747] Change QuickTime init/term calls to handle main and background thread issues.
// 01-02-07 AWL 4.2-c004 [1449038] Add option to XMPUtils::AppendProperties to delete things with
//				empty values. Used in FileInfo templates to delete sensitive info.
//
// 10-20-06 AWL 4.1-f047 [1403500] Add support to call Mac QuickTime properly from background threads.
// 10-12-06 AWL 4.1-c021 [1235816] Remove the new/delete overrides from static builds.
// 09-15-06 AWL 4.1-f035 Revamp the MPEG handler to be platform neutral.
// 08-30-06 AWL 4.1-c019 [1361874] Fix the PDF/A namespace URI strings.
//
// 07-18-06 AWL 4.0-c011 [1239739] Add namespaces for PDF/A.
// 07-13-06 AWL 4.0-f016 Add a handler flag for file formats that need to have read-only packets.
// 05-16-06 AWL 4.0-c006 Add SXMPUtils::PackageForJPEG and SXMPUtils::MergeFromJPEG.
// 04-18-06 AWL Add error constants for TIFF_Support.
// 04-07-06 AWL 4.0-f002 Add XMPFiles::GetThumbnail. Change XMPFiles::OpenFile to close the disk file
//				early for read-only access. Change XMPFileHandler::ExtractXMP to CacheFileData.
// 03-24-06 AWL 4.0 Adapt for move to ham-perforce, integrate XMPFiles, bump version to 4.
//
// 02-21-06 AWL 3.3-015 Expose some of the "File Info" utils. The Adobe Labs build of XMPFiles needs
//				SXMPUtils::RemoveProperties and SXMPUtils::AppendProperties.
//
// 06-01-05 AWL 3.2-105 [1110051] Add delete-existing option for SetProperty.
// 05-02-05 AWL 3.2-019 Add Dynamic Media namespace, kXMP_NS_DM, "http://ns.adobe.com/xmp/1.0/DynamicMedia/".
// 04-19-05 AWL Improve Doxygen comments for SDK.
// 04-11-05 AWL 3.2-016 Add AdobePrivate conditionals where appropriate.
// 04-08-05 AWL 3.2-015 Undo unnecessary constant changes in XMP_Const.h - keep compatibility over
//              "perfection" so that SDK and internal code are easier to merge.
// 03-17-05 AWL 3.2-006 Revise Plain XMP parsing and serialization for latest proposal.
// 02-16-05 AWL 3.2-005 Add first cut of Plain XMP parsing and serialization.
// 01-28-05 AWL 3.2-001 Remove BIB.
//
// 12-21-04 AWL Clarify character form constants.
// 12-14-04 AWL 3.1.1-100 [1022350,1075328] Add more namespaces and internal/external properties.
// 12-03-04 AWL Minor cleanup of comments, packet encoding, reserve constants for future features.
// 11-05-04 AWL 3.1.1-092 [1106408] Add new IPTC namespace.
// 11-03-04 AWL Add more DVA file types.
// 10-12-04 AWL 3.1.1-084 [616293] Add value and name checking to SetProperty.
// 09-22-04 AWL Add add error constant for user abort (used by DocMetaLib).
//
// 08-27-04 AWL 3.1-078 Add EXIF_Aux and CameraRaw namespaces.
// 08-25-04 AWL 3.1-077 Add padding field to XMP_PacketInfo.
// 08-23-04 AWL 3.1-076 Add kXMP_NS_PSAlbum as a predefined namespace.
// 08-23-04 AWL Add file type constants for MOV and AVI.
// 08-20-04 AWL Fix value for kXMP_InDesignFile.
// 08-11-04 AWL Add character size macro and XMP_PacketInfo initializer.
// 07-15-04 AWL 3.1-064 [1016805] Get rid of kXMP_PropValueIsXML.
// 05-26-04 AWL 3.1-043 Change XMP_PacketInfo to use a 32 bit length.
//
// 03-29-04 AWL Add Q&D versions of GetMainPacket and UpdateMainPacket.
// 03-25-04 AWL Move XMP_InternalRef from TXMPMeta.hpp to XMP_Const.h.
// 03-17-04 AWL Add new exception codes.
// 03-15-04 AWL Remove C++-isms.
// 02-17-04 AWL Add multi-file utilities.
// 02-09-04 AWL Add XMP_FileFormat for the new Document Operation utilities.
// 01-27-04 AWL Remove most iterator "omit" options.  Add kXMP_ExactPacketLength.
// 01-17-04 AWL Move into new Perforce depot, cosmetic cleanup.
// 05-21-03 AWL Make all of the Date/Time fields signed, needed to adjust and normalize times.
// 05-06-03 AWL Introduce callback model for output (e.g. dump routines).
// 05-02-03 AWL Improve iterators.
// 03-12-03 AWL Add exception info.
// 03-03-03 AWL Add Doxygen comments.
// 09-12-02 AWL Tweak to clean compile, using stubs for BRV types.
// 09-06-02 AWL First version for general review.
// 08-22-02 AWL First draft sent to Perry Caro and Chris Deighan.
// 08-15-02 AWL Started first draft.
//
// =================================================================================================
#endif // AdobePrivate

#include "XMP_Environment.h"

	#include <stddef.h>

#if XMP_MacBuild | XMP_iOSBuild	// ! No stdint.h on Windows and some UNIXes.
    #include <stdint.h>
#endif
//Android has both inttypes and stdint. But inttypes includes stdint plus other functions
#if XMP_UNIXBuild | XMP_AndroidBuild		// hopefully an inttypes.h on all UNIXes...
		#include <inttypes.h>
#endif

#ifndef XMP_MARKER_EXTENSIBILITY_BACKWARD_COMPATIBILITY
	#define XMP_MARKER_EXTENSIBILITY_BACKWARD_COMPATIBILITY 1
#endif

#if XMP_iOSBuild
    #define __AVAILABILITY_INTERNAL_DEPRECATED_XMP __AVAILABILITY_INTERNAL_DEPRECATED
#else
    #define __AVAILABILITY_INTERNAL_DEPRECATED_XMP
#endif

#if __cplusplus
extern "C" {
#endif

// =================================================================================================
/// \file XMP_Const.h
/// \brief Common C/C++ types and constants for the XMP toolkit.
// =================================================================================================

// =================================================================================================
// Basic types and constants
// =========================

// The XMP_... types are used on the off chance that the ..._t types present a problem. In that
// case only the declarations of the XMP_... types needs to change, not all of the uses. These
// types are used where fixed sizes are required in order to have a known ABI for a DLL build.

#if XMP_MacBuild | XMP_iOSBuild

    typedef int8_t   XMP_Int8;
    typedef int16_t  XMP_Int16;
    typedef int32_t  XMP_Int32;
    typedef int64_t  XMP_Int64;

    typedef uint8_t  XMP_Uns8;
    typedef uint16_t XMP_Uns16;
    typedef uint32_t XMP_Uns32;
    typedef uint64_t XMP_Uns64;

#elif XMP_WinBuild

    typedef signed char XMP_Int8;
    typedef signed short XMP_Int16;
    typedef signed long XMP_Int32;
    typedef signed long long XMP_Int64;

    typedef unsigned char XMP_Uns8;
    typedef unsigned short XMP_Uns16;
    typedef unsigned long XMP_Uns32;
    typedef unsigned long long XMP_Uns64;

#elif XMP_UNIXBuild | XMP_AndroidBuild

	#if ! XMP_64

		typedef signed char XMP_Int8;
		typedef signed short XMP_Int16;
		typedef signed long XMP_Int32;
		typedef signed long long XMP_Int64;

		typedef unsigned char XMP_Uns8;
		typedef unsigned short XMP_Uns16;
		typedef unsigned long XMP_Uns32;
		typedef unsigned long long XMP_Uns64;

	#else

		typedef signed char XMP_Int8;
		typedef signed short XMP_Int16;
		typedef signed int XMP_Int32;
		typedef signed long long XMP_Int64;

		typedef unsigned char XMP_Uns8;
		typedef unsigned short XMP_Uns16;
		typedef unsigned int XMP_Uns32;
		typedef unsigned long long XMP_Uns64;

	#endif

#else

	#error "XMP environment error - must define one of XMP_MacBuild, XMP_WinBuild, XMP_UNIXBuild, XMP_AndroidBuild or XMP_iOSBuild"

#endif

typedef XMP_Uns8 XMP_Bool;

const XMP_Uns8 kXMP_Bool_False = 0;

#define ConvertXMP_BoolToBool(a) (a) != kXMP_Bool_False
#define ConvertBoolToXMP_Bool(a) (a) ? !kXMP_Bool_False : kXMP_Bool_False

static const XMP_Uns8 Min_XMP_Uns8		= ( (XMP_Uns8) 0x00 );
static const XMP_Uns8 Max_XMP_Uns8		= ( (XMP_Uns8) 0xFF );
static const XMP_Uns16 Min_XMP_Uns16	= ( (XMP_Uns16) 0x00 );
static const XMP_Uns16 Max_XMP_Uns16	= ( (XMP_Uns16) 0xFFFF );
static const XMP_Uns32 Min_XMP_Uns32	= ( (XMP_Uns32) 0x00 );
static const XMP_Uns32 Max_XMP_Uns32	= ( (XMP_Uns32) 0xFFFFFFFF );
static const XMP_Uns64 Min_XMP_Uns64	= ( (XMP_Uns64) 0x00 );
static const XMP_Uns64 Max_XMP_Uns64	= ( (XMP_Uns64) 0xFFFFFFFFFFFFFFFFLL );

static const XMP_Int8 Min_XMP_Int8		= ( (XMP_Int8) 0x80 );
static const XMP_Int8 Max_XMP_Int8		= ( (XMP_Int8) 0x7F );
static const XMP_Int16 Min_XMP_Int16	= ( (XMP_Int16) 0x8000 );
static const XMP_Int16 Max_XMP_Int16	= ( (XMP_Int16) 0x7FFF );
static const XMP_Int32 Min_XMP_Int32	= ( (XMP_Int32) 0x80000000 );
static const XMP_Int32 Max_XMP_Int32	= ( (XMP_Int32) 0x7FFFFFFF );
static const XMP_Int64 Min_XMP_Int64	= ( (XMP_Int64) 0x8000000000000000LL );
static const XMP_Int64 Max_XMP_Int64	= ( (XMP_Int64) 0x7FFFFFFFFFFFFFFFLL );


/// @brief An "ABI safe" pointer to the internal part of an XMP object. Use to pass an XMP object across
///        client DLL boundaries. See \c TXMPMeta::GetInternalRef().
typedef struct __XMPMeta__ *        XMPMetaRef;

/// @brief An "ABI safe" pointer to the internal part of an XMP iteration object. Use to pass an XMP
/// iteration object across client DLL boundaries. See \c TXMPIterator.
typedef struct __XMPIterator__ *    XMPIteratorRef;

/// @brief An "ABI safe" pointer to the internal part of an XMP document operations object. Use to pass an
/// XMP document operations object across client DLL boundaries. See \c TXMPDocOps.
typedef struct __XMPDocOps__ *    XMPDocOpsRef;

/// @brief An "ABI safe" pointer to the internal part of an XMP file-handling object. Use to pass an XMP
/// file-handling object across  client DLL boundaries. See \c TXMPFiles.
typedef struct __XMPFiles__ *       XMPFilesRef;

#if AdobePrivate
    typedef XMPMetaRef XMP_InternalRef;  // Preserve old spelling.
#endif // AdobePrivate

// =================================================================================================

/// \name General scalar types and constants
/// @{

/// \typedef XMP_StringPtr
/// \brief The type for input string parameters. A <tt>const char *</tt>, a null-terminated UTF-8
/// string.

/// \typedef XMP_StringLen
/// \brief The type for string length parameters. A 32-bit unsigned integer, as big as will be
/// practically needed.

/// \typedef XMP_Index
/// \brief The type for offsets and indices. A 32-bit signed integer. It is signed to allow -1 for
/// loop termination.

/// \typedef XMP_OptionBits
/// \brief The type for a collection of 32 flag bits. 
///	@details Individual flags are defined as enum value bit
///  masks; see \c #kXMP_PropValueIsURI and following. A number of macros provide common set or set
///  operations, such as \c XMP_PropIsSimple. For other tests use an expression like <code>options &
/// kXMP_<theOption></code>. When passing multiple option flags use the bitwise-OR operator. '|',
/// not the arithmatic plus, '+'.

typedef const char * XMP_StringPtr;  // Points to a null terminated UTF-8 string.
typedef XMP_Uns32    XMP_StringLen;
typedef XMP_Int32    XMP_Index;      // Signed, sometimes -1 is handy.
typedef XMP_Uns32    XMP_OptionBits; // Used as 32 individual bits.

/// \def kXMP_TrueStr
/// \brief The canonical true string value for Booleans in serialized XMP.
///
/// Code that converts from string to bool should be case insensitive, and also allow "1".

/// \def kXMP_FalseStr
/// \brief The canonical false string value for Booleans in serialized XMP.
///
/// Code that converts	from string to bool should be case insensitive, and also allow "0".

#define kXMP_TrueStr  "True"  // Serialized XMP spellings, not for the type bool.
#define kXMP_FalseStr "False"

///@brief Type for yes/no/maybe answers. The values are picked to allow Boolean-like usage. The yes 
///values are true (non-zero), the no value is false (zero).
enum {
	/// The part or parts have definitely changed.
	kXMPTS_Yes = 1,
	/// The part or parts have definitely not changed.
	kXMPTS_No = 0,
	/// The part or parts might, or might not, have changed.
	kXMPTS_Maybe = -1
};
typedef XMP_Int8 XMP_TriState;

/// @}

// =================================================================================================

/// \struct XMP_DateTime
/// \brief The expanded type for a date and time.
///
/// Dates and time in the serialized XMP are ISO 8601 strings. The \c XMP_DateTime struct allows
/// easy conversion with other formats.
///
/// All of the fields are 32 bit, even though most could be 8 bit. This avoids overflow when doing
/// carries for arithmetic or normalization. All fields have signed values for the same reasons.
///
/// Date-time values are occasionally used with only a date or only a time component. A date without
/// a time has zeros in the \c XMP_DateTime struct for all time fields. A time without a date has
/// zeros for all date fields (year, month, and day).
///
/// \c TXMPUtils provides utility functions for manipulating date-time values.
///
/// @see \c TXMPUtils::ConvertToDate(), \c TXMPUtils::ConvertFromDate(),
/// \c TXMPUtils::CompareDateTime(), \c TXMPUtils::ConvertToLocalTime(),
/// \c TXMPUtils::ConvertToUTCTime(), \c TXMPUtils::CurrentDateTime(),
/// \c TXMPUtils::SetTimeZone()

struct XMP_DateTime {

	/// The year, can be negative.
    XMP_Int32 year;

	/// The month in the range 1..12.
    XMP_Int32 month;

	/// The day of the month in the range 1..31.
    XMP_Int32 day;

	/// The hour in the range 0..23.
    XMP_Int32 hour;

	/// The minute in the range 0..59.
    XMP_Int32 minute;

	/// The second in the range 0..59.
    XMP_Int32 second;

    /// Is the date portion meaningful?
    XMP_Bool hasDate;

    /// Is the time portion meaningful?
    XMP_Bool hasTime;

    /// Is the time zone meaningful?
    XMP_Bool hasTimeZone;

	/// The "sign" of the time zone, \c #kXMP_TimeIsUTC (0) means UTC, \c #kXMP_TimeWestOfUTC (-1)
	/// is west, \c #kXMP_TimeEastOfUTC (+1) is east.
    XMP_Int8 tzSign;

	/// The time zone hour in the range 0..23.
    XMP_Int32 tzHour;

	/// The time zone minute in the range 0..59.
    XMP_Int32 tzMinute;

	/// Nanoseconds within a second, often left as zero.
    XMP_Int32 nanoSecond;

	#if __cplusplus
		XMP_DateTime() : year(0), month(0), day(0), hour(0), minute(0), second(0),
		                 hasDate(false),hasTime(false), hasTimeZone(false), tzSign(0), tzHour(0), tzMinute(0), nanoSecond(0){};
	#endif

};

/// Constant values for \c XMP_DateTime::tzSign field.
enum {
	/// Time zone is west of UTC.
    kXMP_TimeWestOfUTC = -1,
	/// UTC time.
    kXMP_TimeIsUTC     =  0,
	/// Time zone is east of UTC.
    kXMP_TimeEastOfUTC = +1
};

#define XMPDateTime_IsDateOnly(dt) ((dt).hasDate & (! (dt).hasTime))
#define XMPDateTime_IsTimeOnly(dt) ((dt).hasTime & (! (dt).hasDate))

#define XMPDateTime_ClearTimeZone(dt) { (dt).hasTimeZone = (dt).tzSign = (dt).tzHour = (dt).tzMinute = 0; }

// =================================================================================================
// Standard namespace URI constants
// ================================

/// \name XML namespace constants for standard XMP schema.
/// @{
///
/// \def kXMP_NS_XMP
/// \brief The XML namespace for the XMP "basic" schema.
///
/// \def kXMP_NS_XMP_Rights
/// \brief The XML namespace for the XMP copyright schema.
///
/// \def kXMP_NS_XMP_MM
/// \brief The XML namespace for the XMP digital asset management schema.
///
/// \def kXMP_NS_XMP_BJ
/// \brief The XML namespace for the job management schema.
///
/// \def kXMP_NS_XMP_T
/// \brief The XML namespace for the XMP text document schema.
///
/// \def kXMP_NS_XMP_T_PG
/// \brief The XML namespace for the XMP paged document schema.
///
/// \def kXMP_NS_PDF
/// \brief The XML namespace for the PDF schema.
///
/// \def kXMP_NS_Photoshop
/// \brief The XML namespace for the Photoshop custom schema.
///
/// \def kXMP_NS_EXIF
/// \brief The XML namespace for Adobe's EXIF schema.
///
/// \def kXMP_NS_TIFF
/// \brief The XML namespace for Adobe's TIFF schema.
///
/// @}

#define kXMP_NS_XMP        "http://ns.adobe.com/xap/1.0/"

#define kXMP_NS_XMP_Rights "http://ns.adobe.com/xap/1.0/rights/"
#define kXMP_NS_XMP_MM     "http://ns.adobe.com/xap/1.0/mm/"
#define kXMP_NS_XMP_BJ     "http://ns.adobe.com/xap/1.0/bj/"

#define kXMP_NS_PDF        "http://ns.adobe.com/pdf/1.3/"
#define kXMP_NS_Photoshop  "http://ns.adobe.com/photoshop/1.0/"
#define kXMP_NS_PSAlbum    "http://ns.adobe.com/album/1.0/"
#define kXMP_NS_EXIF       "http://ns.adobe.com/exif/1.0/"
#define kXMP_NS_EXIF_Aux   "http://ns.adobe.com/exif/1.0/aux/"
#define kXMP_NS_TIFF       "http://ns.adobe.com/tiff/1.0/"
#define kXMP_NS_PNG        "http://ns.adobe.com/png/1.0/"
#define kXMP_NS_SWF        "http://ns.adobe.com/swf/1.0/"
#define kXMP_NS_JPEG       "http://ns.adobe.com/jpeg/1.0/"
#define kXMP_NS_JP2K       "http://ns.adobe.com/jp2k/1.0/"
#define kXMP_NS_CameraRaw  "http://ns.adobe.com/camera-raw-settings/1.0/"
#define kXMP_NS_DM         "http://ns.adobe.com/xmp/1.0/DynamicMedia/"
#define kXMP_NS_Script     "http://ns.adobe.com/xmp/1.0/Script/"
#define kXMP_NS_ASF        "http://ns.adobe.com/asf/1.0/"
#define kXMP_NS_WAV        "http://ns.adobe.com/xmp/wav/1.0/"
#define kXMP_NS_BWF        "http://ns.adobe.com/bwf/bext/1.0/"
#define kXMP_NS_AEScart    "http://ns.adobe.com/aes/cart/"
#define kXMP_NS_RIFFINFO   "http://ns.adobe.com/riff/info/"
#define kXMP_NS_iXML       "http://ns.adobe.com/ixml/1.0/"
#define kXMP_NS_XMP_Note   "http://ns.adobe.com/xmp/note/"

#define kXMP_NS_AdobeStockPhoto "http://ns.adobe.com/StockPhoto/1.0/"
#define kXMP_NS_CreatorAtom "http://ns.adobe.com/creatorAtom/1.0/"

#define kXMP_NS_ExifEX		"http://cipa.jp/exif/1.0/"

#if AdobePrivate
#define kXMP_NS_Transient  "http://ns.adobe.com/xmp/transient/1.0/"
#endif // AdobePrivate

/// \name XML namespace constants for qualifiers and structured property fields.
/// @{
///
/// \def kXMP_NS_XMP_IdentifierQual
/// \brief The XML namespace for qualifiers of the xmp:Identifier property.
///
/// \def kXMP_NS_XMP_Dimensions
/// \brief The XML namespace for fields of the Dimensions type.
///
/// \def kXMP_NS_XMP_Image
/// \brief The XML namespace for fields of a graphical image. Used for the Thumbnail type.
///
/// \def kXMP_NS_XMP_ResourceEvent
/// \brief The XML namespace for fields of the ResourceEvent type.
///
/// \def kXMP_NS_XMP_ResourceRef
/// \brief The XML namespace for fields of the ResourceRef type.
///
/// \def kXMP_NS_XMP_ST_Version
/// \brief The XML namespace for fields of the Version type.
///
/// \def kXMP_NS_XMP_ST_Job
/// \brief The XML namespace for fields of the JobRef type.
///
/// @}

#define kXMP_NS_XMP_IdentifierQual "http://ns.adobe.com/xmp/Identifier/qual/1.0/"
#define kXMP_NS_XMP_Dimensions     "http://ns.adobe.com/xap/1.0/sType/Dimensions#"
#define kXMP_NS_XMP_Text           "http://ns.adobe.com/xap/1.0/t/"
#define kXMP_NS_XMP_PagedFile      "http://ns.adobe.com/xap/1.0/t/pg/"
#define kXMP_NS_XMP_Graphics       "http://ns.adobe.com/xap/1.0/g/"
#define kXMP_NS_XMP_Image          "http://ns.adobe.com/xap/1.0/g/img/"
#define kXMP_NS_XMP_Font           "http://ns.adobe.com/xap/1.0/sType/Font#"
#define kXMP_NS_XMP_ResourceEvent  "http://ns.adobe.com/xap/1.0/sType/ResourceEvent#"
#define kXMP_NS_XMP_ResourceRef    "http://ns.adobe.com/xap/1.0/sType/ResourceRef#"
#define kXMP_NS_XMP_ST_Version     "http://ns.adobe.com/xap/1.0/sType/Version#"
#define kXMP_NS_XMP_ST_Job         "http://ns.adobe.com/xap/1.0/sType/Job#"
#define kXMP_NS_XMP_ManifestItem   "http://ns.adobe.com/xap/1.0/sType/ManifestItem#"

// Deprecated XML namespace constants
#define kXMP_NS_XMP_T     "http://ns.adobe.com/xap/1.0/t/"
#define kXMP_NS_XMP_T_PG  "http://ns.adobe.com/xap/1.0/t/pg/"
#define kXMP_NS_XMP_G_IMG "http://ns.adobe.com/xap/1.0/g/img/"

/// \name XML namespace constants from outside Adobe.
/// @{
///
/// \def kXMP_NS_DC
/// \brief The XML namespace for the Dublin Core schema.
///
/// \def kXMP_NS_IPTCCore
/// \brief The XML namespace for the IPTC Core schema.
///
/// \def kXMP_NS_IPTCExt
/// \brief The XML namespace for the IPTC Extension schema.
///
/// \def kXMP_NS_RDF
/// \brief The XML namespace for RDF.
///
/// \def kXMP_NS_XML
/// \brief The XML namespace for XML.
///
/// @}

#define kXMP_NS_DC             "http://purl.org/dc/elements/1.1/"

#define kXMP_NS_IPTCCore       "http://iptc.org/std/Iptc4xmpCore/1.0/xmlns/"
#define kXMP_NS_IPTCExt        "http://iptc.org/std/Iptc4xmpExt/2008-02-29/"

#define kXMP_NS_DICOM          "http://ns.adobe.com/DICOM/"

#define kXMP_NS_PLUS           "http://ns.useplus.org/ldf/xmp/1.0/"

#define kXMP_NS_PDFA_Schema    "http://www.aiim.org/pdfa/ns/schema#"
#define kXMP_NS_PDFA_Property  "http://www.aiim.org/pdfa/ns/property#"
#define kXMP_NS_PDFA_Type      "http://www.aiim.org/pdfa/ns/type#"
#define kXMP_NS_PDFA_Field     "http://www.aiim.org/pdfa/ns/field#"
#define kXMP_NS_PDFA_ID        "http://www.aiim.org/pdfa/ns/id/"
#define kXMP_NS_PDFA_Extension "http://www.aiim.org/pdfa/ns/extension/"

#define kXMP_NS_PDFX           "http://ns.adobe.com/pdfx/1.3/"
#define	kXMP_NS_PDFX_ID        "http://www.npes.org/pdfx/ns/id/"

#define kXMP_NS_RDF            "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
#define kXMP_NS_XML            "http://www.w3.org/XML/1998/namespace"

// =================================================================================================
// Enums and macros used for option bits
// =====================================

/// \name Macros for standard option selections.
/// @{
///
/// \def kXMP_ArrayLastItem
/// \brief Options macro accesses last array item.
///
/// \def kXMP_UseNullTermination
/// \brief Options macro sets string style.
///
/// \def kXMP_NoOptions
/// \brief Options macro clears all property-type bits.
///
/// @}

#define kXMP_ArrayLastItem      ((XMP_Index)(-1L))
#define kXMP_UseNullTermination ((XMP_StringLen)(~0UL))
#define kXMP_NoOptions          ((XMP_OptionBits)0UL)

/// \name Macros for setting and testing general option bits.
/// @{
///
/// \def XMP_SetOption
/// \brief Macro sets an option flag bit.
///	\param var A variable storing an options flag.
/// \param opt The bit-flag constant to set.
///
/// \def XMP_ClearOption
/// \brief Macro clears an option flag bit.
///	\param var A variable storing an options flag.
/// \param opt The bit-flag constant to clear.
///
/// \def XMP_TestOption
/// \brief Macro reports whether an option flag bit is set.
///	\param var A variable storing an options flag.
/// \param opt The bit-flag constant to test.
/// \return True if the bit is set.
///
/// \def XMP_OptionIsSet
/// \brief Macro reports whether an option flag bit is set.
///	\param var A variable storing an options flag.
/// \param opt The bit-flag constant to test.
/// \return True if the bit is set.
///
/// \def XMP_OptionIsClear
/// \brief Macro reports whether an option flag bit is clear.
///	\param var A variable storing an options flag.
/// \param opt The bit-flag constant to test.
/// \return True if the bit is clear.
///
/// @}

#define XMP_SetOption(var,opt)      var |= (opt)
#define XMP_ClearOption(var,opt)    var &= ~(opt)
#define XMP_TestOption(var,opt)     (((var) & (opt)) != 0)
#define XMP_OptionIsSet(var,opt)    (((var) & (opt)) != 0)
#define XMP_OptionIsClear(var,opt)  (((var) & (opt)) == 0)

/// \name Macros for setting and testing specific option bits.
/// @{
///
/// \def XMP_PropIsSimple
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_PropIsStruct
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_PropIsArray
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_ArrayIsUnordered
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_ArrayIsOrdered
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_ArrayIsAlternate
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_ArrayIsAltText
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_PropHasQualifiers
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_PropIsQualifier
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_PropHasLang
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_NodeIsSchema
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// \def XMP_PropIsAlias
/// \brief Macro reports the property type specified by an options flag.
/// \param opt The options flag to check.
///
/// @}

#define XMP_PropIsSimple(opt)       (((opt) & kXMP_PropCompositeMask) == 0)
#define XMP_PropIsStruct(opt)       (((opt) & kXMP_PropValueIsStruct) != 0)
#define XMP_PropIsArray(opt)        (((opt) & kXMP_PropValueIsArray) != 0)

#define XMP_ArrayIsUnordered(opt)   (((opt) & kXMP_PropArrayIsOrdered) == 0)
#define XMP_ArrayIsOrdered(opt)     (((opt) & kXMP_PropArrayIsOrdered) != 0)
#define XMP_ArrayIsAlternate(opt)   (((opt) & kXMP_PropArrayIsAlternate) != 0)
#define XMP_ArrayIsAltText(opt)     (((opt) & kXMP_PropArrayIsAltText) != 0)

#define XMP_PropHasQualifiers(opt)  (((opt) & kXMP_PropHasQualifiers) != 0)
#define XMP_PropIsQualifier(opt)    (((opt) & kXMP_PropIsQualifier) != 0)
#define XMP_PropHasLang(opt)        (((opt) & kXMP_PropHasLang) != 0)

#define XMP_NodeIsSchema(opt)       (((opt) & kXMP_SchemaNode) != 0)
#define XMP_PropIsAlias(opt)        (((opt) & kXMP_PropIsAlias) != 0)

// -------------------------------------------------------------------------------------------------

#if AdobePrivate
enum {  // Option bit flags for TXMPMeta::MarkStaleProperties.
    kXMP_DeleteUnknownProperties = 0x0001UL,  // Default is to record unknown properties.
    kXMP_RecordDerivedProperties = 0x0002UL   // Default is to delete derived properties.
};

// -------------------------------------------------------------------------------------------------
#endif

/// @brief Option bit flags for the \c TXMPMeta property accessor functions.
enum {

	/// The XML string form of the property value is a URI, use rdf:resource attribute. DISCOURAGED
    kXMP_PropValueIsURI       = 0x00000002UL,

	// ------------------------------------------------------
    // Options relating to qualifiers attached to a property.

	/// The property has qualifiers, includes \c rdf:type and \c xml:lang.
    kXMP_PropHasQualifiers    = 0x00000010UL,

	/// This is a qualifier for some other property, includes \c rdf:type and \c xml:lang.
	/// Qualifiers can have arbitrary structure, and can themselves have qualifiers. If the
	/// qualifier itself has a structured value, this flag is only set for the top node of the
	/// qualifier's subtree.
    kXMP_PropIsQualifier      = 0x00000020UL,

	/// Implies \c #kXMP_PropHasQualifiers, property has \c xml:lang.
    kXMP_PropHasLang          = 0x00000040UL,

	/// Implies \c #kXMP_PropHasQualifiers, property has \c rdf:type.
    kXMP_PropHasType          = 0x00000080UL,

	// --------------------------------------------
    // Options relating to the data structure form.

	/// The value is a structure with nested fields.
    kXMP_PropValueIsStruct    = 0x00000100UL,

	/// The value is an array (RDF alt/bag/seq). The "ArrayIs..." flags identify specific types
	/// of array; default is a general unordered array, serialized using an \c rdf:Bag container.
    kXMP_PropValueIsArray     = 0x00000200UL,

	/// The item order does not matter.
    kXMP_PropArrayIsUnordered = kXMP_PropValueIsArray,

	/// Implies \c #kXMP_PropValueIsArray, item order matters. It is serialized using an \c rdf:Seq container.
    kXMP_PropArrayIsOrdered   = 0x00000400UL,

	/// Implies \c #kXMP_PropArrayIsOrdered, items are alternates. It is serialized using an \c rdf:Alt container.
    kXMP_PropArrayIsAlternate = 0x00000800UL,

	// ------------------------------------
    // Additional struct and array options.

	/// Implies \c #kXMP_PropArrayIsAlternate, items are localized text.  Each array element is a
	/// simple property with an \c xml:lang attribute.
    kXMP_PropArrayIsAltText   = 0x00001000UL,

    // kXMP_InsertBeforeItem  = 0x00004000UL,  ! Used by SetXyz functions.
    // kXMP_InsertAfterItem   = 0x00008000UL,  ! Used by SetXyz functions.

	// ----------------------------
    // Other miscellaneous options.

	/// This property is an alias name for another property.  This is only returned by
	/// \c TXMPMeta::GetProperty() and then only if the property name is simple, not an path expression.
    kXMP_PropIsAlias          = 0x00010000UL,

	/// This property is the base value (actual) for a set of aliases.This is only returned by
	/// \c TXMPMeta::GetProperty() and then only if the property name is simple, not an path expression.
    kXMP_PropHasAliases       = 0x00020000UL,

	/// The value of this property is "owned" by the application, and should not generally be editable in a UI.
    kXMP_PropIsInternal       = 0x00040000UL,

	/// The value of this property is not derived from the document content.
    kXMP_PropIsStable         = 0x00100000UL,

	/// The value of this property is derived from the document content.
    kXMP_PropIsDerived        = 0x00200000UL,

#if AdobePrivate
    kXMP_PropIsStale          = 0x00400000UL,  // This property is in the stale properties list.
#endif

    // kXMPUtil_AllowCommas   = 0x10000000UL,  ! Used by TXMPUtils::CatenateArrayItems and ::SeparateArrayItems.
    // kXMP_DeleteExisting    = 0x20000000UL,  ! Used by TXMPMeta::SetXyz functions to delete any pre-existing property.
    // kXMP_SchemaNode        = 0x80000000UL,  ! Returned by iterators - #define to avoid warnings

	// ------------------------------
    // Masks that are multiple flags.

	/// Property type bit-flag mask for all array types
    kXMP_PropArrayFormMask    = kXMP_PropValueIsArray | kXMP_PropArrayIsOrdered | kXMP_PropArrayIsAlternate | kXMP_PropArrayIsAltText,

	/// Property type bit-flag mask for composite types (array and struct)
    kXMP_PropCompositeMask    = kXMP_PropValueIsStruct | kXMP_PropArrayFormMask,

	/// Mask for bits that are reserved for transient use by the implementation.
    kXMP_ImplReservedMask     = 0x70000000L

};

#define kXMP_SchemaNode ((XMP_OptionBits)0x80000000UL)

/// @brief Option bit flags for the \c TXMPMeta property setting functions. 
/// @details These option bits are shared with the accessor functions:
///   \li \c #kXMP_PropValueIsURI
///   \li \c #kXMP_PropValueIsStruct
///   \li \c #kXMP_PropValueIsArray
///   \li \c #kXMP_PropArrayIsOrdered
///   \li \c #kXMP_PropArrayIsAlternate
///   \li \c #kXMP_PropArrayIsAltText
enum {

    /// Option for array item location: Insert a new item before the given index.
    kXMP_InsertBeforeItem      = 0x00004000UL,

    /// Option for array item location: Insert a new item after the given index.
    kXMP_InsertAfterItem       = 0x00008000UL,

    /// Delete any pre-existing property.
    kXMP_DeleteExisting        = 0x20000000UL,

	/// Bit-flag mask for property-value option bits
    kXMP_PropValueOptionsMask  = kXMP_PropValueIsURI,

	/// Bit-flag mask for array-item location bits
    kXMP_PropArrayLocationMask = kXMP_InsertBeforeItem | kXMP_InsertAfterItem

};

// -------------------------------------------------------------------------------------------------

/// @brief Option bit flags for \c TXMPMeta::ParseFromBuffer().
enum {

	/// Require a surrounding \c x:xmpmeta element.
    kXMP_RequireXMPMeta   = 0x0001UL,

	/// This is the not last input buffer for this parse stream.
    kXMP_ParseMoreBuffers = 0x0002UL,

	/// Do not reconcile alias differences, throw an exception.
    kXMP_StrictAliasing   = 0x0004UL

};

/// @brief Option bit flags for \c TXMPMeta::SerializeToBuffer().
enum {

    // *** Option to remove empty struct/array, or leaf with empty value?

	/// Omit the XML packet wrapper.
    kXMP_OmitPacketWrapper   = 0x0010UL,

	/// Default is a writeable packet.
    kXMP_ReadOnlyPacket      = 0x0020UL,

	/// Use a compact form of RDF.
    kXMP_UseCompactFormat    = 0x0040UL,

	/// Use a canonical form of RDF.
    kXMP_UseCanonicalFormat    = 0x0080UL,

	/// Include a padding allowance for a thumbnail image.
    kXMP_IncludeThumbnailPad = 0x0100UL,

	/// The padding parameter is the overall packet length.
    kXMP_ExactPacketLength   = 0x0200UL,

	/// Omit all formatting whitespace.
    kXMP_OmitAllFormatting   = 0x0800UL,

    /// Omit the x:xmpmeta element surrounding the rdf:RDF element.
	kXMP_OmitXMPMetaElement  = 0x1000UL,    
	
	/// Include a rdf Hash and Merged flag in x:xmpmeta element.
	kXMP_IncludeRDFHash      = 0x2000UL,

    _XMP_LittleEndian_Bit    = 0x0001UL,  // ! Don't use directly, see the combined values below!
    _XMP_UTF16_Bit           = 0x0002UL,
    _XMP_UTF32_Bit           = 0x0004UL,

	/// Bit-flag mask for encoding-type bits
    kXMP_EncodingMask        = 0x0007UL,

	/// Use UTF8 encoding
    kXMP_EncodeUTF8          = 0UL,

	/// Use UTF16 big-endian encoding
    kXMP_EncodeUTF16Big      = _XMP_UTF16_Bit,

	/// Use UTF16 little-endian encoding
    kXMP_EncodeUTF16Little   = _XMP_UTF16_Bit | _XMP_LittleEndian_Bit,

	/// Use UTF32 big-endian encoding
    kXMP_EncodeUTF32Big      = _XMP_UTF32_Bit,

	/// Use UTF13 little-endian encoding
    kXMP_EncodeUTF32Little   = _XMP_UTF32_Bit | _XMP_LittleEndian_Bit

};

// -------------------------------------------------------------------------------------------------

/// @brief Option bit flags for \c TXMPIterator construction.
enum {

	/// The low 8 bits are an enum of what data structure to iterate.
    kXMP_IterClassMask      = 0x00FFUL,

    /// Iterate the property tree of a TXMPMeta object.
    kXMP_IterProperties     = 0x0000UL,

	/// Iterate the global alias table.
    kXMP_IterAliases        = 0x0001UL,

	/// Iterate the global namespace table.
    kXMP_IterNamespaces     = 0x0002UL,

	#if AdobePrivate
    kXMP_IterPropTraits     = 0x0003UL,  // Iterate the global property traits table.
	#endif

	/// Just do the immediate children of the root, default is subtree.
    kXMP_IterJustChildren   = 0x0100UL,

	/// Just do the leaf nodes, default is all nodes in the subtree.
    kXMP_IterJustLeafNodes  = 0x0200UL,

	/// Return just the leaf part of the path, default is the full path.
    kXMP_IterJustLeafName   = 0x0400UL,

	 /// Omit all qualifiers.
    kXMP_IterOmitQualifiers = 0x1000UL

};

/// @brief Option bit flags for \c TXMPIterator::Skip().
enum {

	/// Skip the subtree below the current node.
    kXMP_IterSkipSubtree    = 0x0001UL,

	/// Skip the subtree below and remaining siblings of the current node.
    kXMP_IterSkipSiblings   = 0x0002UL

};

// -------------------------------------------------------------------------------------------------

/// @brief Option bit flags for \c TXMPUtils::CatenateArrayItems() and \c TXMPUtils::SeparateArrayItems().
/// @details These option bits are shared with the accessor functions:
///   \li \c #kXMP_PropValueIsArray,
///   \li \c #kXMP_PropArrayIsOrdered,
///   \li \c #kXMP_PropArrayIsAlternate,
///   \li \c #kXMP_PropArrayIsAltText
enum {

	/// Allow commas in item values, default is separator.
    kXMPUtil_AllowCommas      = 0x10000000UL

};

/// @brief Option bit flags for \c TXMPUtils::ApplyTemplate().
enum {

	 /// Do all properties, default is just external properties.
    kXMPTemplate_IncludeInternalProperties = 0x0001UL,

	/// Perform a Replace operation, add new properties and modify existing ones.
    kXMPTemplate_ReplaceExistingProperties = 0x0002UL,

	/// Similar to Replace, also delete if the template has an empty value.
    kXMPTemplate_ReplaceWithDeleteEmpty = 0x0004UL,

	/// Perform an Add operation, add properties if they don't already exist.
    kXMPTemplate_AddNewProperties = 0x0008UL,

    /// Perform a Clear operation, keep named properties and delete everything else.
    kXMPTemplate_ClearUnnamedProperties = 0x0010UL

};

/// @brief Option bit flags for \c TXMPUtils::RemoveProperties() and \c TXMPUtils::AppendProperties().
enum {

	 /// Do all properties, default is just external properties.
    kXMPUtil_DoAllProperties   = 0x0001UL,

	/// Replace existing values, default is to leave them.
    kXMPUtil_ReplaceOldValues  = 0x0002UL,

	/// Delete properties if the new value is empty.
    kXMPUtil_DeleteEmptyValues = 0x0004UL,

	/// Include aliases, default is just actual properties.
    kXMPUtil_IncludeAliases    = 0x0800UL

};

#if AdobePrivate
	#define kXMPUI_AllowCommas      kXMPUtil_AllowCommas
	#define kXMPUI_DoAllProperties  kXMPUtil_DoAllProperties
	#define kXMPUI_ReplaceOldValues kXMPUtil_ReplaceOldValues
	#define kXMPUI_IncludeAliases   kXMPUtil_IncludeAliases
#endif // AdobePrivate

// =================================================================================================
// Types and Constants for XMPFiles
// ================================

/// @brief Seek mode constants for use with XMP_IO and inside XMPFiles library code.
enum SeekMode { kXMP_SeekFromStart, kXMP_SeekFromCurrent, kXMP_SeekFromEnd };

/// @brief File format constants for use with XMPFiles.
enum {

    // ! Hex used to avoid gcc warnings. Leave the constants so the text reads big endian. There
    // ! seems to be no decent way on UNIX to determine the target endianness at compile time.
    // ! Forcing it on the client isn't acceptable.

	// --------------------
    // Public file formats.

	/// Public file format constant: 'PDF '
    kXMP_PDFFile             = 0x50444620UL,
	/// Public file format constant: 'PS  ', general PostScript following DSC conventions
    kXMP_PostScriptFile      = 0x50532020UL,
	/// Public file format constant: 'EPS ', encapsulated PostScript
    kXMP_EPSFile             = 0x45505320UL,

	/// Public file format constant: 'JPEG'
    kXMP_JPEGFile            = 0x4A504547UL,
	/// Public file format constant: 'JPX ', JPEG 2000, ISO 15444-1
    kXMP_JPEG2KFile          = 0x4A505820UL,
	/// Public file format constant: 'TIFF'
    kXMP_TIFFFile            = 0x54494646UL,
	/// Public file format constant: 'GIF '
    kXMP_GIFFile             = 0x47494620UL,
	/// Public file format constant: 'PNG '
    kXMP_PNGFile             = 0x504E4720UL,

	/// Public file format constant: 'SWF '
    kXMP_SWFFile             = 0x53574620UL,
	/// Public file format constant: 'FLA '
    kXMP_FLAFile             = 0x464C4120UL,
	/// Public file format constant: 'FLV '
    kXMP_FLVFile             = 0x464C5620UL,

	/// Public file format constant: 'MOV ', Quicktime
    kXMP_MOVFile             = 0x4D4F5620UL,
	/// Public file format constant: 'AVI '
    kXMP_AVIFile             = 0x41564920UL,
	/// Public file format constant: 'CIN ', Cineon
    kXMP_CINFile             = 0x43494E20UL,
 	/// Public file format constant: 'WAV '
    kXMP_WAVFile             = 0x57415620UL,
	/// Public file format constant: 'MP3 '
    kXMP_MP3File             = 0x4D503320UL,
	/// Public file format constant: 'SES ', Audition session
    kXMP_SESFile             = 0x53455320UL,
	/// Public file format constant: 'CEL ', Audition loop
    kXMP_CELFile             = 0x43454C20UL,
	/// Public file format constant: 'MPEG'
    kXMP_MPEGFile            = 0x4D504547UL,
	/// Public file format constant: 'MP2 '
    kXMP_MPEG2File           = 0x4D503220UL,
	/// Public file format constant: 'MP4 ', ISO 14494-12 and -14
    kXMP_MPEG4File           = 0x4D503420UL,
	/// Public file format constant: 'MXF '
    kXMP_MXFFile             = 0x4D584620UL,
	/// Public file format constant: 'WMAV', Windows Media Audio and Video
    kXMP_WMAVFile            = 0x574D4156UL,
	/// Public file format constant:  'AIFF'
    kXMP_AIFFFile            = 0x41494646UL,
	/// Public file format constant:  'RED ', RED file format
    kXMP_REDFile            = 0x52454420UL,
    /// Public file format constant:  'ARRI', ARRI file format
    kXMP_ARRIFile           = 0x41525249UL,
	/// Public file format constant:  'HEIF', HEIF file format
	kXMP_HEIFFile = 0x48454946UL,
	/// Public file format constant:  'P2  ', a collection not really a single file
    kXMP_P2File              = 0x50322020UL,
	/// Public file format constant:  'XDCF', a collection not really a single file
    kXMP_XDCAM_FAMFile       = 0x58444346UL,
	/// Public file format constant:  'XDCS', a collection not really a single file
    kXMP_XDCAM_SAMFile       = 0x58444353UL,
	/// Public file format constant:  'XDCX', a collection not really a single file
    kXMP_XDCAM_EXFile        = 0x58444358UL,
	/// Public file format constant:  'AVHD', a collection not really a single file
    kXMP_AVCHDFile           = 0x41564844UL,
	/// Public file format constant:  'SHDV', a collection not really a single file
    kXMP_SonyHDVFile         = 0x53484456UL,
	/// Public file format constant:  'CNXF', a collection not really a single file
    kXMP_CanonXFFile         = 0x434E5846UL,
	/// Public file format constant:  'AVCU', a collection not really a single file
	kXMP_AVCUltraFile		 = 0x41564355UL,

	/// Public file format constant: 'HTML'
    kXMP_HTMLFile            = 0x48544D4CUL,
	/// Public file format constant: 'XML '
    kXMP_XMLFile             = 0x584D4C20UL,
	/// Public file format constant:  'text'
    kXMP_TextFile            = 0x74657874UL,
	/// Public file format constant:  'SVG '
	kXMP_SVGFile			 = 0x53564720UL,
	/// Public file format constant: 'WEBP '
	kXMP_WebPFile            = 0x57454250UL,

	// -------------------------------
    // Adobe application file formats.

	/// Adobe application file format constant: 'PSD '
    kXMP_PhotoshopFile       = 0x50534420UL,
	/// Adobe application file format constant: 'AI  '
    kXMP_IllustratorFile     = 0x41492020UL,
	/// Adobe application file format constant: 'INDD'
    kXMP_InDesignFile        = 0x494E4444UL,
	/// Adobe application file format constant: 'AEP '
    kXMP_AEProjectFile       = 0x41455020UL,
	/// Adobe application file format constant: 'AET ', After Effects Project Template
    kXMP_AEProjTemplateFile  = 0x41455420UL,
	/// Adobe application file format constant: 'FFX '
    kXMP_AEFilterPresetFile  = 0x46465820UL,
	/// Adobe application file format constant: 'NCOR'
    kXMP_EncoreProjectFile   = 0x4E434F52UL,
	/// Adobe application file format constant: 'PRPJ'
    kXMP_PremiereProjectFile = 0x5052504AUL,
	/// Adobe application file format constant: 'PRTL'
    kXMP_PremiereTitleFile   = 0x5052544CUL,
	/// Adobe application file format constant: 'UCF ', Universal Container Format
	kXMP_UCFFile             = 0x55434620UL,

	// -------
    // Others.

	/// Unknown file format constant: '    '
    kXMP_UnknownFile         = 0x20202020UL

};

/// Type for file format identification constants. See \c #kXMP_PDFFile and following.
typedef XMP_Uns32 XMP_FileFormat;

// -------------------------------------------------------------------------------------------------

/// @brief Byte-order masks, do not use directly
enum {
    kXMP_CharLittleEndianMask = 1,
    kXMP_Char16BitMask        = 2,
    kXMP_Char32BitMask        = 4
};

/// @brief Constants to allow easy testing for 16/32 bit and big/little endian.
enum {
	/// 8-bit
    kXMP_Char8Bit        = 0,
	/// 16-bit big-endian
    kXMP_Char16BitBig    = kXMP_Char16BitMask,
	/// 16-bit little-endian
    kXMP_Char16BitLittle = kXMP_Char16BitMask | kXMP_CharLittleEndianMask,
	/// 32-bit big-endian
    kXMP_Char32BitBig    = kXMP_Char32BitMask,
	/// 32-bit little-endian
    kXMP_Char32BitLittle = kXMP_Char32BitMask | kXMP_CharLittleEndianMask,
	/// Variable or not-yet-known cases
    kXMP_CharUnknown     = 1
};

/// \name Macros to test components of the character form mask
/// @{
///
/// \def XMP_CharFormIs16Bit
/// \brief Macro reports the encoding of a character.
/// \param f The character to check.
///
/// \def XMP_CharFormIs32Bit
/// \brief Macro reports the encoding of a character.
/// \param f The character to check.
///
/// \def XMP_CharFormIsBigEndian
/// \brief Macro reports the byte-order of a character.
/// \param f The character to check.
///
/// \def XMP_CharFormIsLittleEndian
/// \brief Macro reports the byte-order of a character.
/// \param f The character to check.
///
/// \def XMP_GetCharSize
/// \brief Macro reports the byte-size of a character.
/// \param f The character to check.
///
/// \def XMP_CharToSerializeForm
/// \brief Macro converts \c XMP_Uns8 to \c XMP_OptionBits.
/// \param cf The character to convert.
///
/// \def XMP_CharFromSerializeForm
/// \brief Macro converts \c XMP_OptionBits to \c XMP_Uns8.
/// \param sf The character to convert.
///
/// @}

#define XMP_CharFormIs16Bit(f)         ( ((int)(f) & kXMP_Char16BitMask) != 0 )
#define XMP_CharFormIs32Bit(f)         ( ((int)(f) & kXMP_Char32BitMask) != 0 )
#define XMP_CharFormIsBigEndian(f)     ( ((int)(f) & kXMP_CharLittleEndianMask) == 0 )
#define XMP_CharFormIsLittleEndian(f)  ( ((int)(f) & kXMP_CharLittleEndianMask) != 0 )
#define XMP_GetCharSize(f)             ( ((int)(f)&6) == 0 ? 1 : (int)(f)&6 )
#define XMP_CharToSerializeForm(cf)    ( (XMP_OptionBits)(cf) )
#define XMP_CharFromSerializeForm(sf)  ( (XMP_Uns8)(sf) )

/// \def kXMPFiles_UnknownOffset
/// \brief Constant for an unknown packet offset within a file.
#define kXMPFiles_UnknownOffset	((XMP_Int64)-1)

/// \def kXMPFiles_UnknownLength
/// \brief Constant for an unknown packet length within a file.
#define kXMPFiles_UnknownLength	((XMP_Int32)-1)

/// @brief XMP packet description
struct XMP_PacketInfo {

	/// Packet offset in the file in bytes, -1 if unknown.
    XMP_Int64 offset;
	/// Packet length in the file in bytes, -1 if unknown.
    XMP_Int32 length;
	/// Packet padding size in bytes, zero if unknown.
    XMP_Int32 padSize;   // Zero if unknown.

	/// Character format using the values \c kXMP_Char8Bit, \c kXMP_Char16BitBig, etc.
    XMP_Uns8  charForm;
	/// True if there is a packet wrapper and the trailer says writeable by dumb packet scanners.
    XMP_Bool  writeable;
    /// True if there is a packet wrapper, the "<?xpacket...>" XML processing instructions.
    XMP_Bool  hasWrapper;

	/// Padding to make the struct's size be a multiple 4.
    XMP_Uns8  pad;

	/// Default constructor.
	XMP_PacketInfo() : offset(kXMPFiles_UnknownOffset), length(kXMPFiles_UnknownLength),
					   padSize(0), charForm(0), writeable(0), hasWrapper(0), pad(0) {};

};

/// @brief Version of the XMP_PacketInfo type
enum {
	/// Version of the XMP_PacketInfo type
	kXMP_PacketInfoVersion = 3
};

// -------------------------------------------------------------------------------------------------

/// @brief Option bit flags for \c TXMPFiles::Initialize().
enum {
    /// Ignore non-XMP text that uses an undefined "local" encoding.
    kXMPFiles_IgnoreLocalText = 0x0002,
    /// Combination of flags necessary for server products using XMPFiles.
    kXMPFiles_ServerMode      = kXMPFiles_IgnoreLocalText
};

/// @brief Option bit flags for \c TXMPFiles::GetFormatInfo().
enum {

	/// Can inject first-time XMP into an existing file.
    kXMPFiles_CanInjectXMP        = 0x00000001,

	/// Can expand XMP or other metadata in an existing file.
    kXMPFiles_CanExpand           = 0x00000002,

	/// Can copy one file to another, writing new metadata.
    kXMPFiles_CanRewrite          = 0x00000004,

	 /// Can expand, but prefers in-place update.
    kXMPFiles_PrefersInPlace      = 0x00000008,

	/// Supports reconciliation between XMP and other forms.
    kXMPFiles_CanReconcile        = 0x00000010,

	 /// Allows access to just the XMP, ignoring other forms.
    kXMPFiles_AllowsOnlyXMP       = 0x00000020,

	/// File handler returns raw XMP packet information.
    kXMPFiles_ReturnsRawPacket    = 0x00000040,

	/// The file handler does the file open and close.
    kXMPFiles_HandlerOwnsFile     = 0x00000100,

	/// The file handler allows crash-safe file updates.
    kXMPFiles_AllowsSafeUpdate    = 0x00000200,

	/// The file format needs the XMP packet to be read-only.
    kXMPFiles_NeedsReadOnlyPacket = 0x00000400,

	/// The file handler uses a "sidecar" file for the XMP.
    kXMPFiles_UsesSidecarXMP      = 0x00000800,

	/// The format is folder oriented, for example the P2 video format.
    kXMPFiles_FolderBasedFormat   = 0x00001000,

	/// The file Handler is capable of notifying progress notifications
    kXMPFiles_CanNotifyProgress   = 0x00002000,

	/// The plugin handler is not capable for delay loading
	kXMPFiles_NeedsPreloading      = 0x00004000,

#if AdobePrivate
	/// The file handler is supporting album arts in a file
	kXMPFiles_SupportsAlbumArt	   = 0x00008000,
#endif

	/// The format needs file during Process XMP even if it's Read Only Operation.
	/// So that file is not closed after OpenFile operation when kXMPFiles_OpenForUpdate not passed
	kXMPFiles_NeedsLocalFileOpened = 0x00010000
};

/// @brief Option bit flags for \c TXMPFiles::OpenFile().
enum {

	/// Open for read-only access.
    kXMPFiles_OpenForRead           = 0x00000001,

	/// Open for reading and writing.
    kXMPFiles_OpenForUpdate         = 0x00000002,

	/// Only the XMP is wanted, allows space/time optimizations.
    kXMPFiles_OpenOnlyXMP           = 0x00000004,

	/// Force use of the given handler (format), do not even verify the format.
    kXMPFiles_ForceGivenHandler     = 0x00000008,
	
	/// Be strict about only attempting to use the designated file handler, no fallback to other handlers.
    kXMPFiles_OpenStrictly          = 0x00000010,

	/// Require the use of a smart handler.
    kXMPFiles_OpenUseSmartHandler   = 0x00000020,

	/// Force packet scanning, do not use a smart handler.
    kXMPFiles_OpenUsePacketScanning = 0x00000040,

	/// Only packet scan files "known" to need scanning.
    kXMPFiles_OpenLimitedScanning   = 0x00000080,

    /// Attempt to repair a file opened for update, default is to not open (throw an exception).
    kXMPFiles_OpenRepairFile        = 0x00000100,

	/// When updating a file, spend the effort necessary to optimize file layout.
	kXMPFiles_OptimizeFileLayout    = 0x00000200,
	
	/// When updating a PDF preserve state of document
	kXMPFiles_PreservePDFState    =  0x00000400
};

#if AdobePrivate
	enum {		
		/// Open the file using Generic Handler if there is no Smart handler for the file format.
		kXMPFiles_OpenUseGenericHandler = 0x00000400
	};
#endif

/// @brief Option bit flags for \c TXMPFiles::CloseFile().
enum {
	/// Write into a temporary file and swap for crash safety.
    kXMPFiles_UpdateSafely = 0x0001
};


#if AdobePrivate
    
    /// @brief Option bit flags for OpenFile for ARRI Plugin
    enum {
        kXMPFiles_ARRI_ColorSpace_ITU709                    = 0x00000800,
        kXMPFiles_ARRI_ColorSpace_CameraNative              = 0x00001000,
        kXMPFiles_ARRI_ColorSpace_P3                        = 0x00002000,
        /// Primaries of the output color space are according to the Academy Color Encoding Specification
        kXMPFiles_ARRI_ColorSpace_ACES                      = 0x00003000,
        /// Film-style color mapping.
        kXMPFiles_ARRI_ColorSpace_Film                      = 0x00004000,
        /// The camera internal color space. Used for V3 LogC encoding
        kXMPFiles_ARRI_ColorSpace_WideGamut                 = 0x00008000,
        
        kXMPFiles_ARRI_ColorSpace_Video_ITU709              = kXMPFiles_ARRI_ColorSpace_ITU709,
        
        kXMPFiles_ARRI_ColorSpace_LogC_CameraNative         = kXMPFiles_ARRI_ColorSpace_CameraNative,
        
        kXMPFiles_ARRI_ColorSpace_Video_P3                  = kXMPFiles_ARRI_ColorSpace_P3,
        
        kXMPFiles_ARRI_ColorSpace_SceneLinear_Aces          = kXMPFiles_ARRI_ColorSpace_ACES,
        
        kXMPFiles_ARRI_ColorSpace_LogC_Film                 = kXMPFiles_ARRI_ColorSpace_Film,
        
        kXMPFiles_ARRI_ColorSpace_LogC_WideGamut            = kXMPFiles_ARRI_ColorSpace_WideGamut,
        
        kXMPFiles_ARRI_ColorSpace_LogC_Monochrome           = 0x00010000,
        
        kXMPFiles_ARRI_ColorSpace_Video_Monochrome          = 0x00020000,
        
        kXMPFiles_ARRI_ColorSpace_SceneLinear_WideGamut     = 0x00040000,
        
        kXMPFiles_ARRI_ColorSpace_SceneLinear_CameraNative  = 0x00080000,
        /// Primaries of the output color space are according to ITU Rec. 2020
        kXMPFiles_ARRI_ColorSpace_Video_ITU2020             = 0x00100000,
        /// Primaries of the output color space are according to the DCI color space with D65 white point
        kXMPFiles_ARRI_ColorSpace_Video_DCI_D60             = 0x00200000,
        /// Primaries of the output color space are according to the DCI color space with D65 white point
        kXMPFiles_ARRI_ColorSpace_Video_DCI_D65             = 0x00400000,
        
        kXMPFiles_ARRI_ColorSpace_LastColorSpace            = kXMPFiles_ARRI_ColorSpace_Video_DCI_D65
    };
    
    /// @brief Option bit flags for OpenFile for ARRI Plugin for BitsPerSample parameter
    enum {
        kXMPFiles_ARRI_10_bits      = 0x00800000,
        
        kXMPFiles_ARRI_12_bits      = 0x01000000,
        
        kXMPFiles_ARRI_14_bits      = 0x02000000,
        
        kXMPFiles_ARRI_16_bits      = 0x04000000
    };
    
/// @brief Usage type of album art for \c TXMPAlbumArt.
enum {

	/// Image is used for showing information other than the predefined values i.e., from 1 to 21
	kXMPFiles_AlbumArt_Use_Other = 0,
	/// Image is used as 32*32 PNG file icon
	kXMPFiles_AlbumArt_Use_32x32PNGFileIcon = 1,
	/// Image is used as file icon other than 32*32 PNG file icon
	kXMPFiles_AlbumArt_Use_OtherFileIcon = 2,
	/// Image is used for front cover
	kXMPFiles_AlbumArt_Use_FrontCover = 3,
	/// Image is used for back cover
	kXMPFiles_AlbumArt_Use_BackCover = 4,
	/// Image is used for leaflet page
	kXMPFiles_AlbumArt_Use_LeafletPage = 5,
	/// Image is related with media
	kXMPFiles_AlbumArt_Use_Media = 6,
	/// Image is used for Lead artist or Lead performer or soloist
	kXMPFiles_AlbumArt_Use_LeadArtist = 7,
	/// Image is used for artists other than lead artist
	kXMPFiles_AlbumArt_Use_OtherArtist = 8,
	/// Image is used for showing conductor
	kXMPFiles_AlbumArt_Use_Conductor = 9,
	/// Image is used for showing Band or Orchestra
	kXMPFiles_AlbumArt_Use_Band = 10,
	/// Image is used for showing composer of the media
	kXMPFiles_AlbumArt_Use_Composer = 11,
	/// Image is used for showing lyricst of the media
	kXMPFiles_AlbumArt_Use_Lyricst = 12,
	/// Image is used for showing recording location
	kXMPFiles_AlbumArt_Use_RecordingLocation = 13,
	/// Image was captured during recording time
	kXMPFiles_AlbumArt_Use_DuringRecording = 14,
	/// Image was captured during performance time
	kXMPFiles_AlbumArt_Use_DuringPerformance = 15,
	/// Image is used as a screenshot of the media
	kXMPFiles_AlbumArt_Use_Screenshot = 16,
	/// No clear information regarding this usage. Derived from ID3 specification
	kXMPFiles_AlbumArt_Use_ABrightColoredFish = 17,
	/// Image is used for showing illustration
	kXMPFiles_AlbumArt_Use_Illustration = 18,
	/// Image is used for showing logotype of the Artist
	kXMPFiles_AlbumArt_Use_ArtistLogotype = 19,
	/// Image is used for showing logotype of the studio
	kXMPFiles_AlbumArt_Use_StudioLogotype = 20,
	/// No usage is specified,
	kXMPFiles_AlbumArt_Use_None = 0xFF,
};

/// @brief format type for \c TXMPAlbumArt.
enum {

	/// No format info exits
	kXMPFiles_AlbumArt_NoFormat				= 0,
	/// Album art is of JPEG format
	kXMPFiles_AlbumArt_JPEG					= 1,
	/// Album art is of PNG format
	kXMPFiles_AlbumArt_PNG					= 2,
	/// Album art is of TIFF format
	kXMPFiles_AlbumArt_TIFF					= 3,
	/// Album art is of GIF format
	kXMPFiles_AlbumArt_GIF					= 4,
	/// Image is not directly embedded but URL of the image resource exits in imageData. Used for external resource dependency album art.
	kXMPFiles_AlbumArt_URLImage				= 5,
	/// Album art is of unsupported format i.e., Not among the defined types
	kXMPFiles_AlbumArt_UnSupportedFormat	= 6
};

/// @brief encoding type for \c TXMPAlbumArt.
enum {

	/// Description field uses UTF8 encoding in file
	kXMPFiles_AlbumArt_Enc_UTF8 = 0,
	/// Description field has UTF16 little-endian encoding in file
	kXMPFiles_AlbumArt_Enc_UTF16Big = 1,
	/// Description field has UTF16 little-endian encoding in file
	kXMPFiles_AlbumArt_Enc_UTF16Little = 2,
	/// Description field has UTF32 big-endian encoding in file
	kXMPFiles_AlbumArt_Enc_UTF32Big = 3,
	/// Description field has UTF32 little-endian encoding in file
	kXMPFiles_AlbumArt_Enc_UTF32Little = 4,
	/// Description field has Latin1 (ISO-8859-1) encoding in file
	kXMPFiles_AlbumArt_Enc_Latin1 = 5,
	/// Description field has unknown encoding in file
	kXMPFiles_AlbumArt_Enc_Unknown = 6
};

/// Represents binary data in XMP
typedef XMP_Uns8 * XMP_BinaryData;


// =================================================================================================
// Types and Constants for High Level Application Support Utilities
// ================================================================

// Standard part names for use with \c TXMPDocOps.
//
// kXMP_Part_All           Indicates "any" or "all"
// kXMP_Part_Metadata      Portions of the metadata changed
// kXMP_Part_Content       Any/all of the content (non-metadata)
// kXMP_Part_Audio         Any/all sound
// kXMP_Part_Visual        Any/all image data (video or still)
// kXMP_Part_Video         Video or animation (moving image)
// kXMP_Part_Raster        Static raster image
// kXMP_Part_Vector        Static vector image
// kXMP_Part_FormData      Form field data
// kXMP_Part_FormTemplate  Form template
// kXMP_Part_Annots        Applied annotations (comments)

#define kXMP_Part_All           "/"
#define kXMP_Part_Metadata      "/metadata"
#define kXMP_Part_Content       "/content"
#define kXMP_Part_Audio         "/content/audio"
#define kXMP_Part_Visual        "/content/visual"
#define kXMP_Part_Video         "/content/visual/video"
#define kXMP_Part_Raster        "/content/visual/raster"
#define kXMP_Part_Vector        "/content/visual/vector"
#define kXMP_Part_FormData      "/content/visual/form/data"
#define kXMP_Part_FormTemplate  "/content/visual/form/template"
#define kXMP_Part_Annots        "/content/visual/annots"

/// Option flags for \c TXMPDOcOps::EnsureIDsExist.
enum {
	kXMPDocOps_IgnoreDocumentID         = 0x01,	// Don't bother creating xmpMM:DocumentID.
	kXMPDocOps_IgnoreOriginalDocumentID = 0x02	// Don't bother creating xmpMM:OriginalDocumentID.
};

/// @brief Reasons returned by \c TXMPDocOps::IsDirty.
enum {
	kXMPDirtyDoc_New               = 0x0001,
	kXMPDirtyDoc_Derived           = 0x0002,
	kXMPDirtyDoc_NewIDs            = 0x0004,
	kXMPDirtyDoc_ContentChanged    = 0x0008,
	kXMPDirtyDoc_MetadataChanged   = 0x0010,
	kXMPDirtyDoc_TypeChangedAtOpen = 0x0020,
	kXMPDirtyDoc_DateChangedAtOpen = 0x0040
};

// *** Declarations for the defunct TXMPUtils version of DocOps:

enum {
   kXMP_EmbedByCopy,
   kXMP_EmbedByReference
};
typedef XMP_Uns8 XMP_EmbeddingForm;

struct XMP_DerivedDocInfo {
    XMP_StringPtr newDocName;
    XMP_StringPtr newFilePath;
    XMP_StringPtr rendClass;
    XMP_StringPtr rendParams;
    XMP_StringPtr oldFilePath;
    XMP_DerivedDocInfo() : newDocName(""), newFilePath(""), rendClass(""), rendParams(""), oldFilePath("") {};
};
enum { kXMP_DerivedDocInfoVersion = 1 };    // *** Add versioning to the internal calls.

struct XMP_EmbeddedDocInfo {
    XMP_StringPtr embeddedFilePath;
    XMP_EmbeddedDocInfo() : embeddedFilePath("") {};
};
enum { kXMP_EmbeddedDocInfoVersion = 1 };

#endif // AdobePrivate

// =================================================================================================
// Error notification and Exceptions
// =================================

/// \name Error notification and Exceptions
/// @{
///
/// @details From the beginning through version 5.5, XMP Tookit errors result in throwing an \c XMP_Error
/// exception. For the most part exceptions were thrown early and thus API calls aborted as soon as
/// an error was detected. Starting in version 5.5, support has been added for notifications of
/// errors arising in calls to \c TXMPMeta and \c TXMPFiles functions.
///
/// A client can register an error notification callback function for a \c TXMPMeta or \c TXMPFiles
/// object. This can be done as a global default or individually to each object. The global default
/// applies to all objects created after it is registered. Within the object there is no difference
/// between the global default or explicitly registered callback. The callback function returns a
/// \c bool value indicating if recovery should be attempted (true) or an exception thrown (false).
/// If no callback is registered, a best effort at recovery and continuation will be made with an
/// exception thrown if recovery is not possible. More details can be found in the \c TXMPMeta and
/// \c TXMPFiles documentation.
///
/// The \c XMP_Error class contains a numeric code and an English explanation. New numeric codes may
/// be added at any time. There are typically many possible explanations for each numeric code. The
/// explanations try to be precise about the specific circumstances causing the error.
///
/// \note The explanation string is for debugging use only. It must not be shown to users in a
/// final product. It is written for developers not users, and never localized.

typedef XMP_Uns8 XMP_ErrorSeverity;

/// @brief Severity codes for error notifications
enum {
    /// Partial recovery and continuation is possible.
    kXMPErrSev_Recoverable    = 0,
    /// Recovery is not possible, an exception will be thrown aborting the API call.
    kXMPErrSev_OperationFatal = 1,
    /// Recovery is not possible, an exception will be thrown, the file is corrupt and possibly unusable.
    kXMPErrSev_FileFatal      = 2,
    /// Recovery is not possible, an exception will be thrown, the entire process should be aborted.
    kXMPErrSev_ProcessFatal   = 3,
	/// Recovery is not possible, an exception will be thrown, the entire process should be aborted.
	kXMPErrSev_StreamFatal = 4
};

// -------------------------------------------------------------------------------------------------
/// @brief The signature of a client-defined callback for TXMPMeta error notifications.
///
/// @param context A pointer used to carry client-private context.
///
/// @param severity The severity of the error, see the \c XMP_ErrorSeverity values.
///
/// @param cause A numeric code for the cause of the error, from the XMP_Error exception codes.
/// Codes used with TXMPMeta error notifications:
/// \li \c kXMPErr_BadXML - An XML syntax error found during parsing.
/// \li \c kXMPErr_BadRDF - A syntax or semantic parsing error in the XMP subset of RDF.
/// \li \c kXMPErr_BadXMP - A semantic XMP data model error.
/// \li \c kXMPErr_BadValue - An XMP value error, wrong type, out of range, etc.
/// \li \c kXMPErr_NoMemory - A heap allocation failure.
///
/// @param message An explanation of the error, for debugging use only. This should not be displayed
/// to users in a final product.
///
/// @return True if the operation should continue with a best effort attempt at recovery, false if
/// it should be aborted with an exception thrown from the library back to the original caller.
/// Recovery is possible only if the severity is kXMPErrSev_Recoverable, an exception will be
/// thrown on return from the callback in all other cases.
///
/// @see \c TXMPMeta::SetDefaultErrorCallback() and \c TXMPMeta::SetErrorCallback()

typedef bool (* XMPMeta_ErrorCallbackProc) ( void* context, XMP_ErrorSeverity severity, XMP_Int32 cause, XMP_StringPtr message );

// -------------------------------------------------------------------------------------------------
/// @brief The signature of a client-defined callback for TXMPFiles error notifications.
///
/// @param context A pointer used to carry client-private context.
///
/// @param filePath The path for the file involved in the error.
///
/// @param severity The severity of the error, see the \c XMP_ErrorSeverity values.
///
/// @param cause A numeric code for the cause of the error, from the XMP_Error exception codes.
/// Codes used with TXMPFiles error notifications:
/// \li \c kXMPErr_NoFile - A file does not exist
/// \li \c kXMPErr_FilePermission - A file exists but cannot be opened
/// \li \c kXMPErr_FilePathNotAFile - A path exists which is not a file
/// \li \c dXMPErr_RejectedFileExtension - Any Operation called on rejected file extension
/// \li \c KXMPErr_NoFileHandler - No suitable handler is found for the file
/// \li \c kXMPErr_DiskSpace - A file write fails due to lack of disk space
/// \li \c kXMPErr_ReadError - A file read fails
/// \li \c kXMPErr_WriteError - A file write fails for some other reason than space
/// \li \c kXMPErr_BadFileFormat - A file is corrupt or ill-formed
/// \li \c kXMPErr_BadBlockFormat - A portion of a file is corrupt or ill-formed
/// \li \c kXMPErr_BadValue - An XMP or non-XMP metadata item has an invalid value
/// \li \c kXMPErr_NoMemory - A heap allocation failure
///
/// @param message An explanation of the error, for debugging use only. This should not be displayed
/// to users in a final product.
///
/// @return True if the operation should continue with a best effort attempt at recovery, false if
/// it should be aborted with an exception thrown from the library back to the original caller.
/// Recovery is possible only if the severity is kXMPErrSev_Recoverable, an exception will be
/// thrown on return from the callback in all other cases.
///
/// @see \c TXMPFiles::SetDefaultErrorCallback() and \c TXMPFiles::SetErrorCallback()

typedef bool (* XMPFiles_ErrorCallbackProc) ( void* context, XMP_StringPtr filePath, XMP_ErrorSeverity severity, XMP_Int32 cause, XMP_StringPtr message );

// -------------------------------------------------------------------------------------------------
/// Internal: The signatures of client-side wrappers for the error notification callbacks.

typedef XMP_Bool (* XMPMeta_ErrorCallbackWrapper) ( XMPMeta_ErrorCallbackProc clientProc, void* context,
                                    	        XMP_ErrorSeverity severity, XMP_Int32 cause, XMP_StringPtr message );

typedef XMP_Bool (* XMPFiles_ErrorCallbackWrapper) ( XMPFiles_ErrorCallbackProc clientProc, void* context,
                                     	         XMP_StringPtr filePath, XMP_ErrorSeverity severity,
                                    	         XMP_Int32 cause, XMP_StringPtr message );

/// XMP Toolkit error, associates an error code with a descriptive error string.
class XMP_Error {
public:

	/// @brief Constructor for an XMP_Error.
	///
	/// @param _id The numeric code.
	///
	/// @param _errMsg The descriptive string, for debugging use only. It must not be shown to users
	/// in a final product. It is written for developers, not users, and never localized.
	XMP_Error ( XMP_Int32 _id, XMP_StringPtr _errMsg ) : id(_id), errMsg(_errMsg), notified(false) {};

	/// Retrieves the numeric code from an XMP_Error.
	inline XMP_Int32     GetID() const     { return id; };

	/// Retrieves the descriptive string from an XMP_Error.
	inline XMP_StringPtr GetErrMsg() const { return errMsg; };

	/// Retrieves the information whether particular error is notified or not
	inline XMP_Bool      IsNotified() const { return notified; }

	/// Sets the notification status for an error
	inline void          SetNotified() { notified = true; };

private:
	/// Exception code. See constants \c #kXMPErr_Unknown and following.
	XMP_Int32     id;
	/// Descriptive string, for debugging use only. It must not be shown to users in a final
	/// product. It is written for developers, not users, and never localized.
	XMP_StringPtr errMsg;
	/// Variable to store whether this particular error is notified to user or not
	XMP_Bool notified;
};

/// @brief XMP_Error exception code constants
enum {

	//  --------------------
    /// Generic error codes.

	/// No error
	kXMPErr_NoError          =  -1,

	/// Generic unknown error
    kXMPErr_Unknown          =   0,
	/// Generic undefined error
    kXMPErr_TBD              =   1,
	/// Generic unavailable error
    kXMPErr_Unavailable      =   2,
	/// Generic bad object error
    kXMPErr_BadObject        =   3,
	/// Generic bad parameter error
    kXMPErr_BadParam         =   4,
	/// Generic bad value error
    kXMPErr_BadValue         =   5,
	/// Generic assertion failure
    kXMPErr_AssertFailure    =   6,
	/// Generic enforcement failure
    kXMPErr_EnforceFailure   =   7,
	/// Generic unimplemented error
    kXMPErr_Unimplemented    =   8,
	/// Generic internal failure
    kXMPErr_InternalFailure  =   9,
	/// Generic deprecated error
    kXMPErr_Deprecated       =  10,
	/// Generic external failure
    kXMPErr_ExternalFailure  =  11,
	/// Generic user abort error
    kXMPErr_UserAbort        =  12,
	/// Generic standard exception
    kXMPErr_StdException     =  13,
	/// Generic unknown exception
    kXMPErr_UnknownException =  14,
	/// Generic out-of-memory error
    kXMPErr_NoMemory         =  15,
	/// Progress reporting callback requested abort
    kXMPErr_ProgressAbort    =  16,

	// ------------------------------------
    // More specific parameter error codes.

	/// Bad schema parameter
    kXMPErr_BadSchema        = 101,
	/// Bad XPath parameter
    kXMPErr_BadXPath         = 102,
	/// Bad options parameter
    kXMPErr_BadOptions       = 103,
	/// Bad index parameter
    kXMPErr_BadIndex         = 104,
	/// Bad iteration position
    kXMPErr_BadIterPosition  = 105,
	/// XML parsing error (deprecated)
    kXMPErr_BadParse         = 106,
	/// Serialization error
    kXMPErr_BadSerialize     = 107,
	/// File format error
    kXMPErr_BadFileFormat    = 108,
	/// No file handler found for format
    kXMPErr_NoFileHandler    = 109,
	/// Data too large for JPEG file format
    kXMPErr_TooLargeForJPEG  = 110,
    /// A file does not exist
    kXMPErr_NoFile           = 111,
    /// A file exists but cannot be opened
    kXMPErr_FilePermission   = 112,
    /// A file write failed due to lack of disk space
    kXMPErr_DiskSpace        = 113,
    /// A file read failed
    kXMPErr_ReadError        = 114,
    /// A file write failed for a reason other than lack of disk space
    kXMPErr_WriteError       = 115,
    /// A block of a file is ill-formed, e.g. invalid IPTC-IIM in a photo
    kXMPErr_BadBlockFormat   = 116,
    /// File Path is not a file
    kXMPErr_FilePathNotAFile = 117,
	/// Rejected File extension
	kXMPErr_RejectedFileExtension = 118,
	/// Rejected File extension
	kXMPErr_BadStreamParameters = 119,

	// -----------------------------------------------
    // File format and internal structure error codes.

	/// XML format error
    kXMPErr_BadXML           = 201,
	/// RDF format error
    kXMPErr_BadRDF           = 202,
	/// XMP format error
    kXMPErr_BadXMP           = 203,
	/// Empty iterator
    kXMPErr_EmptyIterator    = 204,
	/// Unicode error
    kXMPErr_BadUnicode       = 205,
	/// TIFF format error
    kXMPErr_BadTIFF          = 206,
	/// JPEG format error
    kXMPErr_BadJPEG          = 207,
	/// PSD format error
    kXMPErr_BadPSD           = 208,
	/// PSIR format error
    kXMPErr_BadPSIR          = 209,
	/// IPTC format error
    kXMPErr_BadIPTC          = 210,
	/// MPEG format error
    kXMPErr_BadMPEG          = 211,
	/// HEIF format: Modify Operation is not supported for Construction Method 1 or 2
	kXMPErr_HEIFConstructionMethodNotSupported = 212,
	/// PNG format error
	kXMPErr_BadPNG			= 213,
	/// WebP format error
	kXMPErr_BadWEBP          = 214

};

/// @}

// =================================================================================================
// Client callbacks
// ================

// -------------------------------------------------------------------------------------------------
/// \name Special purpose callback functions
/// @{

#if AdobePrivate
/// @brief Internal. The signature of a client-defined callback function to provide user notification of
/// debugging assertion failures. Set using \c TXMPMeta::RegisterAssertNotify()
///
/// @details If the client does not register a callback, asserts use the standard C assert macro. If the
/// client does register a callback, it is first called with the message string containing at least
/// the check expression, source file name, and line number. The message might also contain an
/// explanation of the check. The callback should display the message in an alert so that the user
/// will know of the failure. If the callback function returns (as it should), a C++ exception is
/// thrown with the \c #kXMPErr_AssertFailure code.
///
/// There can be only one callback per process, so the first one registered is used. This lets an
/// application register its callback first, while still allowing intermediaries like FileInfo to
/// register a callback in case the application does not.
///
/// @param refCon A pointer to client-defined data passed to the AssertNotifyProc.
///
/// @param message The message string passed from the notification, containing at least the check
/// expression, source file name, and line number. The message might also contain an explanation of
/// the check.

typedef void (* XMP_AssertNotifyProc)   ( void * refCon, XMP_StringPtr message );
#endif	// AdobePrivate

/// @brief A signed 32-bit integer used as a status result for the output callback routine,
/// \c XMP_TextOutputProc. Zero means no error, all other values except -1 are private to the callback.
/// The callback is wrapped to prevent exceptions being thrown across DLL boundaries. Any exceptions
/// thrown out of the callback cause a return status of -1.

typedef XMP_Int32 XMP_Status;

// -------------------------------------------------------------------------------------------------
/// @brief The signature of a client-defined callback for text output from XMP Toolkit debugging
/// operations. 
/// @details The callback is invoked one or more times for each line of output. The end of a line
/// is signaled by a '\\n' character at the end of the buffer. Formatting newlines are never present
/// in the middle of a buffer, but values of properties might contain any UTF-8 characters.
///
/// @param refCon A pointer to client-defined data passed to the TextOutputProc.
///
/// @param buffer  A string containing one line of output.
///
/// @param bufferSize The number of characters in the output buffer.
///
/// @return A success/fail status value. Any failure result aborts the output.
///
/// @see \c TXMPMeta::DumpObject()

typedef XMP_Status (* XMP_TextOutputProc) ( void *        refCon,
                                            XMP_StringPtr buffer,
                                            XMP_StringLen bufferSize );

#if AdobePrivate
// -------------------------------------------------------------------------------------------------
/// Internal. The signature of a client-defined memory allocation routine. 
/// @details By default the XMP
/// Toolkit uses the built-in C++ implementations for memory allocation and deallocation. You can
/// supply an allocation routine conforming to this prototype when calling
/// \c TXMPMeta::Initialize(). The allocation function should return null (0) if memory is not
/// available. It should never throw an exception.

typedef void * (* XMP_AllocateProc) ( size_t size );

// -------------------------------------------------------------------------------------------------
/// @brief Internal. The signature of a client-defined memory deallocation routine.
/// @details By default the XMP
/// Toolkit uses the built-in C++ implementations for memory allocation and deallocation. You can
/// supply a deallocation routine conforming to this prototype when calling
/// \c TXMPMeta::Initialize().

typedef void   (* XMP_DeleteProc)   ( void * ptr );
#endif	// AdobePrivate

// -------------------------------------------------------------------------------------------------
/// @brief The signature of a client-defined callback to check for a user request to abort a time-consuming
/// operation within XMPFiles.
///
/// @param arg A pointer to caller-defined data passed from the registration call.
///
/// @return True to abort the current operation, which results in an exception being thrown.
///
/// @see \c TXMPFiles::SetAbortProc()

typedef bool (* XMP_AbortProc) ( void * arg );

// -------------------------------------------------------------------------------------------------
/// @brief The signature of a client-defined callback for progress report notifications.
///
/// @param context A pointer used to carry client-private context.
///
/// @param elapsedTime The time in seconds since the progress reporting started.
///
/// @param fractionDone A float value estimating the amount of work already done, in the range of
/// 0.0 to 1.0. A value of 0.0 is given if the amount is not known, this happens if there is no
/// estimate total for the total work. The units of work are not defined, but should usually be
/// related to the number of bytes of I/O. This will go backwards if total work estimate changes.
///
/// @param secondsToGo A float value estimating the number of seconds left to complete the file
/// operation. A value of 0.0 is given if the amount is not known, this happens if the amount of
/// total work is unknown. This can go backwards according to throughput or if work estimate changes.
///
/// @return True if the file operation should continue, false if it should be aborted with an
/// exception being thrown from the XMPFiles library back to the original caller.
///
/// @see \c TXMPFiles::SetDefaultProgressCallback() and \c TXMPFiles::SetProgressCallback()

typedef bool (* XMP_ProgressReportProc) ( void * context, float elapsedTime, float fractionDone, float secondsToGo );

// -------------------------------------------------------------------------------------------------
/// Internal: The signature of a client-side wrapper for the progress report callback.

typedef XMP_Bool (* XMP_ProgressReportWrapper) ( XMP_ProgressReportProc proc, void * context,
											 float elapsedTime, float fractionDone, float secondsToGo );

/// @}

// =================================================================================================
// Stuff with no better place to be
// ================================

/// XMP Toolkit version information
typedef struct XMP_VersionInfo {
	/// The primary release number, the "1" in version "1.2.3".
    XMP_Uns8      major;
	/// The secondary release number, the "2" in version "1.2.3".
    XMP_Uns8      minor;
	/// The tertiary release number, the "3" in version "1.2.3".
    XMP_Uns8      micro;
	 /// A 0/1 boolean value, true if this is a debug build.
    XMP_Bool      isDebug;
	 /// A rolling build number, monotonically increasing in a release.
    XMP_Uns32     build;
	 /// Individual feature implementation flags.
    XMP_Uns32     flags;
	 /// A comprehensive version information string.
    XMP_StringPtr message;
} XMP_VersionInfo;

// =================================================================================================

#if __cplusplus
} // extern "C"
#endif

#include <vector>
#if AdobePrivate

#include "XMPCore/XMPCoreDefines.h"

#if ENABLE_CPP_DOM_MODEL
namespace AdobeXMPCore {
	class IStructureNode_v1;
};
#endif

struct XMPDMO_StringInfo {
	XMP_StringPtr ptr;
	XMP_StringLen len;
	XMPDMO_StringInfo() : ptr(0), len(0) {};
};

struct XMPDMO_CuePointInfo {
	XMPDMO_StringInfo key;
	XMPDMO_StringInfo value;
};

struct XMPDMO_MarkerInfo_v1 {

	XMPDMO_StringInfo startTime;
	XMPDMO_StringInfo duration;
	XMPDMO_StringInfo comment;
	XMPDMO_StringInfo name;
	XMPDMO_StringInfo location;
	XMPDMO_StringInfo target;
	XMPDMO_StringInfo type;
	XMPDMO_StringInfo cuePointType;
	XMPDMO_StringInfo speaker;
	XMPDMO_StringInfo probability;
	std::vector<XMPDMO_CuePointInfo> cuePointParams;

};

#if ENABLE_CPP_DOM_MODEL || XMP_MARKER_EXTENSIBILITY_BACKWARD_COMPATIBILITY

#if SUPPORT_SHARED_POINTERS_IN_STD
#include <memory>
#include <functional>
#elif SUPPORT_SHARED_POINTERS_IN_TR1
#if XMP_WinBuild
#include <memory>
#include <functional>
#else
#include <tr1/memory>
#include <tr1/functional>
#endif
#else
#error "location of shared pointer stuff is unknown"
#endif

namespace AdobeXMPCore {
#if SUPPORT_SHARED_POINTERS_IN_STD
	using std::shared_ptr;
#elif SUPPORT_SHARED_POINTERS_IN_TR1
	using std::tr1::shared_ptr;
#endif
}

#endif

struct XMPDMO_MarkerInfo : public XMPDMO_MarkerInfo_v1 {

#if XMP_MARKER_EXTENSIBILITY_BACKWARD_COMPATIBILITY

	XMPDMO_StringInfo guid;
	typedef void(*ReleaseExtensionDataProc)(void * extension);
	AdobeXMPCore::shared_ptr<void>    spExtension;

#endif
	
private:
#if XMP_MARKER_EXTENSIBILITY_BACKWARD_COMPATIBILITY
	void *							extension;
	ReleaseExtensionDataProc		extensionReleaseProc;
	void SetPrivateData(void * _extension, ReleaseExtensionDataProc _extensionReleaseProc) {
		this->extension = _extension;
		this->extensionReleaseProc = _extensionReleaseProc;
	}

	void * GetPrivateData() {

		return  extension;
	}
#endif
public:
#if  XMP_MARKER_EXTENSIBILITY_BACKWARD_COMPATIBILITY
	XMPDMO_MarkerInfo()
		: extension(NULL)
		, extensionReleaseProc(NULL) {}
#endif

private:
	template< typename t> friend class TXMPUtils;
	friend class XMPUtils;
	
};

#endif // AdobePrivate

#endif  // __XMP_Const_h__
