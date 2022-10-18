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
//  ADC	Amandeep Chawla
//	SKP Sunil Kishor Pathak
//
// mm-dd-yy who Description of changes, most recent first.
//
// 03-01-15 SKP 1.0-a073 Enable XMPAssetUnit test runner and fixed the PK build break.
// 03-01-15 SKP 1.0-a071 Fixed PK build break. Stopped building XMPAM unit test project.
// 02-11-15 SKP 1.0-a049 Added initial support for client strategies. Using these strategies client
//						 can set prefix, AppName, history update policy and it's own datetime
//						 function which will be used to get time for ModifyDate, Metadata date etc.
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPAssetManagement/XMPAssetManagement.h"
#include "XMPCommon/Interfaces/IConfigurationManager.h"
#include "XMPExtensions/include/IExtensionsConfigurationManager.h"
#include "XMPCore/Interfaces/ICoreObjectFactory.h"

extern "C" {

	XMP_PUBLIC AdobeXMPAM::pIExtensionsConfigurationManager_base XMPAM_Initialize(AdobeXMPCore::pICoreObjectFactory_base xmpCoreObjFactoryPtr);

	XMP_PUBLIC void XMPAM_Terminate();
}

#if !BUILDING_XMPEXTENSIONS_LIB

namespace AdobeXMPAM {

	pIExtensionsConfigurationManager_base XMPAM_ClientInitialize() {
		pICoreObjectFactory_base xmpCoreObjFactoryPtr = ICoreObjectFactory::GetCoreObjectFactory();
#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
		return new IExtensionsConfigurationManagerProxy(XMPAM_Initialize(xmpCoreObjFactoryPtr));
#else
		return XMPAM_Initialize(xmpCoreObjFactoryPtr);
#endif //#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	}

#if !SOURCE_COMPILING_XMPEXTENSIONS_LIB
	pIExtensionsObjectFactory getExtensionsObjectFactory( bool reset );
	pIAssetUtilities getAssetUtilities( bool reset );
#endif

	void XMPAM_ClientTerminate() {
	#if !SOURCE_COMPILING_XMPEXTENSIONS_LIB
		getAssetUtilities( true );
		getExtensionsObjectFactory( true );
	#endif
		XMPAM_Terminate();
	}

}

#endif  // !BUILDING_XMPEXTENSIONS_LIB
