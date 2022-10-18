#ifndef __XMPCompareAndMergeDefines_h__
#define __XMPCompareAndMergeDefines_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

// =================================================================================================
// XMP_CompareAndMergeDefines.h - Common Defines for XMP Compare And Merge component

// ================================================================
//
// This header defines common definitions to be used in XMP Compare And Merge component.
//
// =================================================================================================

// =================================================================================================
// All Platform Settings
// ===========================
#include "XMPCommon/XMPCommonDefines.h"

#if ENABLE_CPP_DOM_MODEL
// =================================================================================================
// Macintosh Specific Settings
// ===========================
#if XMP_MacBuild
	#define ENABLE_XMP_COMPARE_AND_MERGE 1
#endif

// =================================================================================================
// IOS Specific Settings
// ===========================
#if XMP_iOSBuild
	#define ENABLE_XMP_COMPARE_AND_MERGE 1
#endif

// =================================================================================================
// Windows Specific Settings
// =========================
#if XMP_WinBuild
	#define ENABLE_XMP_COMPARE_AND_MERGE 1
#endif

// =================================================================================================
// UNIX Specific Settings
// ======================
#if XMP_UNIXBuild
	#define ENABLE_XMP_COMPARE_AND_MERGE 1
#endif

#if XMP_AndroidBuild
#define ENABLE_XMP_COMPARE_AND_MERGE 1
#endif
#endif  // ENABLE_CPP_DOM_MODEL

#ifndef ENABLE_XMP_COMPARE_AND_MERGE
	#define ENABLE_XMP_COMPARE_AND_MERGE 0
#endif

#if ENABLE_XMP_COMPARE_AND_MERGE
	#if SOURCE_COMPILING_XMP_ALL
		#define SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB 1
	#endif

	#ifndef SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB
		#define SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB 0
	#endif

	#ifndef BUILDING_XMPCOMPAREANDMERGE_LIB
		#define  BUILDING_XMPCOMPAREANDMERGE_LIB 0
	#endif

	#if BUILDING_XMPCOMPAREANDMERGE_LIB
		#if !BUILDING_XMPCOMPAREANDMERGE_AS_STATIC && !BUILDING_XMPCOMPAREANDMERGE_AS_DYNAMIC
			#error "Define either BUILDING_XMPCOMPAREANDMERGE_AS_STATIC as 1 or BUILDING_XMPCOMPAREANDMERGE_AS_DYNAMIC as 1"
		#endif
	#endif

	#ifndef LINKING_XMPCOMPAREANDMERGE_LIB
		#define LINKING_XMPCOMPAREANDMERGE_LIB 1
	#endif

	namespace AdobeXMPCompareAndMerge {};

#endif  // ENABLE_XMP_COMPARE_AND_MERGE

#endif  // XMPCompareAndMergeDefines_h__
