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
//
// mm-dd-yy who Description of changes, most recent first.
//
// 11-24-14 ADC 1.0-a008 Support for ICompositionRelationship interface.
//
// =================================================================================================
#endif  // AdobePrivate

namespace AdobeXMPAM {
	class IAssetManagerProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class IAssetManagerProxy;

#include "XMPAssetManagement/Interfaces/IAssetManager.h"

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPExtensions/include/IExtensionsObjectFactory.h"
#include "XMPCore/Interfaces/IMetadata.h"
#include<assert.h>

#if 0
namespace AdobeXMPAM {
	spIAssetManager IAssetManager_v1::CreateAssetManager( const spIMetadata & metadata )
	{
		return CallSafeFunctionReturningPointer<
			IExtensionsObjectFactory, pIAssetManager_base, IAssetManager, pIMetadata_base
		>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(),
			&IExtensionsObjectFactory::CreateAssetManager,
			metadata? metadata->GetActualIMetadata():NULL);
		
	}

	IAssetManagerProxy::IAssetManagerProxy(pIAssetManager ptr) : mRawPtr(ptr) {
		mRawPtr->Acquire();
	}

	IAssetManagerProxy::~IAssetManagerProxy() {
		mRawPtr->Release();
	}

	pIAssetManager APICALL IAssetManagerProxy::GetActualIAssetManager() __NOTHROW__{
		return mRawPtr;
	}

		AdobeXMPAM_Int::pIAssetManager_I APICALL IAssetManagerProxy::GetIAssetManager_I() __NOTHROW__{
		return mRawPtr->GetIAssetManager_I();
	}

		pvoid APICALL IAssetManagerProxy::getInterfacePointer(uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error) __NOTHROW__{
		assert(false);
		return mRawPtr->getInterfacePointer(interfaceID, interfaceVersion, error);
	}

		pvoid APICALL IAssetManagerProxy::GetInterfacePointer(uint64 interfaceID, uint32 interfaceVersion) {
		return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
			mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion);
	}

	void APICALL IAssetManagerProxy::Acquire() const __NOTHROW__{
		assert(false);
	}

		void APICALL IAssetManagerProxy::Release() const __NOTHROW__{
		assert(false);
	}

		AdobeXMPCommon_Int::pISharedObject_I APICALL IAssetManagerProxy::GetISharedObject_I() __NOTHROW__{
		return mRawPtr->GetISharedObject_I();
	}

		AdobeXMPCommon_Int::pIThreadSafe_I APICALL IAssetManagerProxy::GetIThreadSafe_I() __NOTHROW__{
		return mRawPtr->GetIThreadSafe_I();
	}

		void APICALL IAssetManagerProxy::EnableThreadSafety() const __NOTHROW__{
		return mRawPtr->EnableThreadSafety();
	}

		void APICALL IAssetManagerProxy::DisableThreadSafety() const __NOTHROW__{
		return mRawPtr->DisableThreadSafety();
	}

		bool APICALL IAssetManagerProxy::IsThreadSafe() const {
		return mRawPtr->isThreadSafe() != 0;
	}

	uint32 APICALL IAssetManagerProxy::isThreadSafe() const __NOTHROW__{
		assert(false);
		return mRawPtr->isThreadSafe();
	}

	spIAssetManager IAssetManager_v1::MakeShared(pIAssetManager_base ptr) {
	if (!ptr) return spIAssetManager();
	pIAssetManager p = IAssetManager::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IAssetManager >() : ptr;
	return shared_ptr< IAssetManager >(new IAssetManagerProxy(p));
	}

}
#endif

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE