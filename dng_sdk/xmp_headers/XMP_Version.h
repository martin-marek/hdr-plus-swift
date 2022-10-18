#ifndef __XMP_Version_h__
#define __XMP_Version_h__ 1

// =================================================================================================
// Copyright 2002-2008 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

// =================================================================================================
/// @file XMP_Version.h
/// @brief XMP Toolkit static version information
///
/// @details This header defines a static version number for the XMP Toolkit, which describes the version of
/// the API used at client compile time. It is not necessarily the same as the runtime version.
/// Runtime version information for the XMP toolkit is stored in the executable and available
/// through a runtime call to \c TXMPMeta::GetVersionInfo(). Runtime verion information is about
/// implementation internals.
///
/// \li Do not display the static values defined here to users as the version of XMP in use.
/// \li Do not base runtime decisions on the static version alone; you can, however, compare the
/// 	runtime and static versions.
// =================================================================================================

#define XMPCORE_API_VERSION_MAJOR  7
#define XMPCORE_API_VERSION_MINOR  0

#define XMPFILES_API_VERSION_MAJOR  7
#define XMPFILES_API_VERSION_MINOR  0

#define XMPSCRIPT_API_VERSION_MAJOR  6
#define XMPSCRIPT_API_VERSION_MINOR  0

#define XMPCOMPAREANDMERGE_API_VERSION_MAJOR	3
#define XMPCOMPAREANDMERGE_API_VERSION_MINOR	0

#define XMPEXTENSIONS_API_VERSION_MAJOR	2
#define XMPEXTENSIONS_API_VERSION_MINOR	0

#define XMPCORE_API_VERSION MACRO_PASTE(XMPCORE_API_VERSION_MAJOR, ., XMPCORE_API_VERSION_MINOR)
#define XMPFILES_API_VERSION MACRO_PASTE(XMPFILES_API_VERSION_MAJOR, ., XMPFILES_API_VERSION_MINOR)
#define XMPSCRIPT_API_VERSION MACRO_PASTE(XMPSCRIPT_API_VERSION_MAJOR, ., XMPSCRIPT_API_VERSION_MINOR)
#define XMPCOMPAREANDMERGE_API_VERSION MACRO_PASTE(XMPCOMPAREANDMERGE_API_VERSION_MAJOR, ., XMPCOMPAREANDMERGE_API_VERSION_MINOR)
#define XMPEXTENSIONS_API_VERSION MACRO_PASTE(XMPEXTENSIONS_API_VERSION_MAJOR, ., XMPEXTENSIONS_API_VERSION_MINOR)

#define XMPCORE_API_VERSION_STRING MAKESTR(XMPCORE_API_VERSION)
#define XMPFILES_API_VERSION_STRING MAKESTR(XMPFILES_API_VERSION)
#define XMPSCRIPT_API_VERSION_STRING MAKESTR(XMPSCRIPT_API_VERSION)
#define XMPCOMPAREANDMERGE_API_VERSION_STRING MAKESTR(XMPCOMPAREANDMERGE_API_VERSION)
#define XMPEXTENSIONS_API_VERSION_STRING MAKESTR(XMPEXTENSIONS_API_VERSION)

//#define XMP_API_VERSION 5.2
//#define XMP_API_VERSION_STRING "5.2"


// =================================================================================================

#endif /* __XMP_Version_h__ */
