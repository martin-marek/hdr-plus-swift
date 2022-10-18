#ifndef XMPCompareAndMerge_h__
#define XMPCompareAndMerge_h__ 1

// =================================================================================================
// Copyright 2014 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#include "XMPCompareAndMerge/XMPCompareAndMergeDefines.h"
#include "XMPCommon/XMPCommonFwdDeclarations.h"
#include "XMPCommon/Interfaces/IConfigurationManager.h"

#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCompareAndMerge/XMPCompareAndMergeFwdDeclarations.h"
#include "XMP_Version.h"

extern "C" {
	XMP_PUBLIC void XMPCM_GetVersionInfo( XMP_VersionInfo * info );
	
	XMP_PUBLIC AdobeXMPCommon::pIConfigurationManager XMPCM_Initialize(AdobeXMPCore::pICoreObjectFactory_base xmpCoreObjFactoryPtr);
	
	XMP_PUBLIC void XMPCM_Terminate();

	XMP_PUBLIC AdobeXMPCompareAndMerge::pICompareAndMergeObjectFactory_base XMPCM_GetObjectFactoryInstance();
}

#if !BUILDING_XMPCOMPAREANDMERGE_LIB
	#include "XMPCore/Interfaces/ICoreObjectFactory.h"
	#include "XMPCompareAndMerge/Interfaces/ICompareAndMergeObjectFactory.h"

	namespace AdobeXMPCompareAndMerge {
		using namespace AdobeXMPCommon;

		inline pIConfigurationManager XMPCM_ClientInitialize() {
			pICoreObjectFactory_base xmpCoreObjFactoryPtr = ICoreObjectFactory::GetCoreObjectFactory();
			pIConfigurationManager confManager = dynamic_cast< pIConfigurationManager >(
				XMPCM_Initialize(xmpCoreObjFactoryPtr) );
			return confManager;
		}

		inline void XMPCM_ClientTerminate() {
			XMPCM_Terminate();
		}

		inline pICompareAndMergeObjectFactory XMPCM_ClientGetObjectFactoryInstance() {
			return ICompareAndMergeObjectFactory::GetCompareAndMergeObjectFactory();
		}
	}

#endif  // !(BUILDING_XMPCOMPAREANDMERGE_LIB)


// =================================================================================================

#endif // XMPCompareAndMerge_h__
