#ifndef XMPAssetManagement_h__
#define XMPAssetManagement_h__ 1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#if AdobePrivate
// =================================================================================================
// Change history
// ==============
//
// Writers:
// ADC	Amandeep Chawla
// SKP Sunil Kishor Pathak
//
// mm-dd-yy who Description of changes, most recent first.
//
// 03-01-15 SKP 1.0-a073 Enable XMPAssetUnit test runner and fixed the PK build break.
// 02-11-15 SKP 1.0-a049 Added initial support for client strategies. Using these strategies client
//						 can set prefix, AppName, history update policy and it's own datetime
//						 function which will be used to get time for ModifyDate, Metadata date etc.
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"

#include "XMP_Version.h"
extern "C" {

	//!
	//! \brief Get the version information of the XMP Asset Management Module.
	//! \param[out] info pointer to an object of type XMP_VersionInfo. The function will fill this
	//! structure with all the information.
	//!
	XMP_PUBLIC void XMPAM_GetVersionInfo( XMP_VersionInfo * info );

}

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCore;

	//!
	//! \brief Initialize the client layer code along with the Asset Management Library.
	//! \return a pointer to NS_XMPCOMMON::IConfigurationManager object which client can use to set
	//! other parameters of the library. In case initialization succeeds a valid pointer is returned
	//! else NULL is the return value.
	//! \note In case of an error NULL is returned.
	//!
	pIExtensionsConfigurationManager_base XMPAM_ClientInitialize();

	//!
	//! \brief Terminates the client layer code along with the Asset Management Library.
	//!
	void XMPAM_ClientTerminate();
}

#endif  // XMPAssetManagement_h__
