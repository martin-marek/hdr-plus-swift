#ifndef XMPExtensionsDefines_h__
#define XMPExtensionsDefines_h__ 1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

// =================================================================================================
// XMPExtensionsDefines.h - Common Defines for XMP Asset Management component
// =================================================================================================
//
// This header defines common definitions to be used in XMP Asset Management component.
//
// =================================================================================================

#if AdobePrivate
// =================================================================================================
// Change history
// ==============
//
// Writers:
//  ADC	Amandeep Chawla
//
// mm-dd-yy who Description of changes, most recent first.
//
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 11-27-14 ADC 1.0-a016 Submitting changes related to iOS build.
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

// =================================================================================================
// All Platform Settings
// ===========================
#include "XMPCommon/XMPCommonDefines.h"

// =================================================================================================
// Macintosh Specific Settings
// ===========================
#if XMP_MacBuild
	#define ENABLE_XMP_ASSET_MANAGEMENT 1
#endif

// =================================================================================================
// IOS Specific Settings
// ===========================
#if XMP_iOSBuild
	#define ENABLE_XMP_ASSET_MANAGEMENT 1
#endif

// =================================================================================================
// Windows Specific Settings
// =========================
#if XMP_WinBuild
	#define ENABLE_XMP_ASSET_MANAGEMENT 1
#endif

// =================================================================================================
// UNIX Specific Settings
// ======================
#if XMP_UNIXBuild
	#define ENABLE_XMP_ASSET_MANAGEMENT 0
#endif

#ifndef ENABLE_XMP_ASSET_MANAGEMENT
	#define ENABLE_XMP_ASSET_MANAGEMENT 0
#endif

#if ENABLE_XMP_ASSET_MANAGEMENT

namespace AdobeXMPAM {};

#if SOURCE_COMPILING_XMP_ALL
#define SOURCE_COMPILING_XMPEXTENSIONS_LIB 1
#endif

#ifndef BUILDING_XMPEXTENSIONS_LIB
#define  BUILDING_XMPEXTENSIONS_LIB 0
#endif

#if BUILDING_XMPEXTENSIONS_LIB
#if !BUILDING_XMPEXTENSIONS_AS_STATIC && !BUILDING_XMPEXTENSIONS_AS_DYNAMIC
#error "Define either BUILDING_XMPEXTENSIONS_AS_STATIC as 1 or BUILDING_XMPEXTENSIONS_AS_DYNAMIC as 1"
#endif
#endif

#ifndef SOURCE_COMPILING_XMPEXTENSIONS_LIB
#define SOURCE_COMPILING_XMPEXTENSIONS_LIB 0
#endif

#if SOURCE_COMPILING_XMPEXTENSIONS_LIB
#if BUILDING_XMPEXTENSIONS_LIB
#error "You can't have both SOURCE_COMPILATION and BUILDING_LIB set together"
#endif
#define GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE 0
#define GENERATE_XMPEXTENSIONS_DLL_LAYER_CODE 0
#endif

#if BUILDING_XMPEXTENSIONS_LIB
#if SOURCE_COMPILING_XMPEXTENSIONS_LIB
#error "You can't have both SOURCE_COMPILATION and BUILDING_LIB set together"
#endif
#define GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE 0
#define GENERATE_XMPEXTENSIONS_DLL_LAYER_CODE 1
#endif

#ifndef GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
#define GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE 1
#endif

#ifndef GENERATE_XMPEXTENSIONS_DLL_LAYER_CODE
#define GENERATE_XMPEXTENSIONS_DLL_LAYER_CODE 1
#endif

#ifndef LINKING_XMPEXTENSIONS_LIB
#define LINKING_XMPEXTENSIONS_LIB 1
#endif

#endif  // ENABLE_XMP_EXTENSIONS

#endif  // XMPExtensionsDefines_h__
