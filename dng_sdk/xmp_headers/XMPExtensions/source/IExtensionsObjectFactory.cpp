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
//	ANS Ankita Sharma
//
// mm-dd-yy who Description of changes, most recent first.
//
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 11-27-14 SKP 1.0-a017 Implemented Asset composition related functionality for basic part addition.
// 11-24-14 ANS 1.0-a007 Fixing build errors
// 11-23-14 SKP 1.0-a006 Support for IAssetPagePart interface.
// 11-21-14 ADC 1.0-a004 Support for IAssetLayerPart interface.
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

namespace AdobeXMPAM {
	class IExtensionsObjectFactoryProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IExtensionsObjectFactoryProxy;

#include "XMPExtensions/include/IExtensionsObjectFactory.h"

#if GENERATE_XMPEXTENSIONS_DLL_LAYER_CODE

extern "C" {
	XMP_PUBLIC AdobeXMPAM::pIExtensionsObjectFactory_base XMPAM_GetObjectFactoryInstance();
}

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
#include "XMPCommon/Utilities/TWrapperFunctions.h"

#include <assert.h>
namespace AdobeXMPAM {

	class IExtensionsObjectFactoryProxy
		: public virtual IExtensionsObjectFactory
	{
	private:
		pIExtensionsObjectFactory mRawPtr;

	public:
		IExtensionsObjectFactoryProxy(pIExtensionsObjectFactory ptr)
			: mRawPtr(ptr)
		{}

		~IExtensionsObjectFactoryProxy() {}

		pIExtensionsObjectFactory APICALL GetActualIExtensionsObjectFactory() __NOTHROW__{ return mRawPtr; }


			AdobeXMPAM_Int::pIExtensionsObjectFactory_I APICALL GetIExtensionsObjectFactory_I() __NOTHROW__{
			return mRawPtr->GetIExtensionsObjectFactory_I();
		}

			pvoid APICALL GetInterfacePointer(uint64 interfaceID, uint32 interfaceVersion) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion);
		}

		pvoid APICALL getInterfacePointer(uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error) __NOTHROW__{
			assert(false);
			return mRawPtr->getInterfacePointer(interfaceID, interfaceVersion, error);
		}

			pIAssetPart_base APICALL CreateStandardGenericPart(uint32 component, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateStandardGenericPart(component, error);
		}

			pIAssetPart_base APICALL CreateUserDefinedGenericPart(const char * component, sizet componentLength, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateUserDefinedGenericPart(component, componentLength, error);
		}

			pIAssetTimePart_base APICALL CreateStandardTimePart(uint32 component, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateStandardTimePart(component, error);
		}

			pIAssetTimePart_base APICALL CreateUserDefinedTimePart(const char * component, sizet componentLength, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateUserDefinedTimePart(component, componentLength, error);
		}

			pIAssetLayerPart_base APICALL CreateStandardLayerPart(uint32 component, const char * id, sizet idLength, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateStandardLayerPart(component, id, idLength, error);
		}

			pIAssetLayerPart_base APICALL CreateUserDefinedLayerPart(const char * component, sizet componentLength, const char * id, sizet idLength, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateUserDefinedLayerPart(component, componentLength, id, idLength, error);
		}

			pIAssetPagePart_base APICALL CreateStandardPagePart(uint32 component, sizet pageNumber, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateStandardPagePart(component, pageNumber, error);
		}

			pIAssetPagePart_base APICALL CreateUserDefinedPagePart(const char * component, sizet length, sizet pageNumber, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateUserDefinedPagePart(component, length, pageNumber, error);
		}

			pIAssetArtboardPart_base APICALL CreateStandardArtboardPart(uint32 component, const char * id, sizet idLength, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateStandardArtboardPart(component, id, idLength, error);
		}

			pIAssetArtboardPart_base APICALL CreateUserDefinedArtboardPart(const char * component, sizet componentLength, const char * id, sizet idLength, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateUserDefinedArtboardPart(component, componentLength, id, idLength, error);
		}

			pIAssetUtilities_base APICALL GetAssetUtilities(pcIError_base & error) __NOTHROW__{
			return mRawPtr->GetAssetUtilities(error);
		}

			pIComposedAssetManager_base APICALL CreateComposedAssetManager(
			pIMetadata_base composedAssetMetadata, pcIError_base & error) __NOTHROW__
		{
			return mRawPtr->CreateComposedAssetManager(composedAssetMetadata, error);
		}

			pITrack_base APICALL CreateTrack(const char * trackName, sizet trackNameLen, uint64 trackTypes, const char * trackTypeName, sizet trackTypeNameLen, pcIError_base &error) __NOTHROW__  {

			return mRawPtr->CreateTrack(trackName, trackNameLen, trackTypes, trackTypeName, trackTypeNameLen, error);
		}


			pITemporalAssetManager_base APICALL CreateTemporalAssetManager(pIMetadata_base composedAssetMetadata, pcIError_base & error) __NOTHROW__{

			return mRawPtr->CreateTemporalAssetManager(composedAssetMetadata, error);
		}
			
		virtual pIUTF8String_base APICALL CreateUTF8String(const char * buf, sizet count, pcIError_base & error) __NOTHROW__{
			return mRawPtr->CreateUTF8String(buf, count, error);
		
		}

		virtual pIError_base APICALL CreateError(uint32 domain, uint32 code, uint32 severity, pcIError_base & error) __NOTHROW__{
		
			return mRawPtr->CreateError(domain, code, severity, error);
		}
	};
	pIExtensionsObjectFactory IExtensionsObjectFactory_v1::MakeExtensionsObjectFactory(pIExtensionsObjectFactory_base ptr) {
		if (IExtensionsObjectFactory::GetInterfaceVersion() == 1)
			return ptr;
		else
			return ptr->GetInterfacePointer< IExtensionsObjectFactory >();
	}

	

	pIExtensionsObjectFactory getExtensionsObjectFactory(bool reset) {
		static pIExtensionsObjectFactory ptr(NULL);
		if (reset) {
#if !SOURCE_COMPILING_XMPEXTENSIONS_LIB
			delete ptr;
#endif
			ptr = NULL;
			return ptr;
		}

		if (NULL == ptr) {
#if !SOURCE_COMPILING_XMPEXTENSIONS_LIB
			IExtensionsObjectFactory * basePtr = dynamic_cast<IExtensionsObjectFactory *>(XMPAM_GetObjectFactoryInstance());
			if (basePtr) {
				ptr = new IExtensionsObjectFactoryProxy(basePtr);
			}
#else
			ptr = dynamic_cast<pIExtensionsObjectFactory>(XMPAM_GetObjectFactoryInstance());
#endif
			return ptr;
		}
		else {
			return ptr;
		}
	}
	pIExtensionsObjectFactory IExtensionsObjectFactory_v1::GetExtensionsObjectFactory() {
		return getExtensionsObjectFactory(false);
	}
}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#endif  // GENERATE_XMPEXTENSIONS_DLL_LAYER_CODE

