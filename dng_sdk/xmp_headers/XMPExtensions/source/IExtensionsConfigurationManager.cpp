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
//  SKP	Sunil Kishor Pathak
//
// mm-dd-yy who Description of changes, most recent first.
//
// 02-11-15 SKP 1.0-a049 Added initial support for client strategies. Using these strategies client
//						 can set prefix, AppName, history update policy and it's own datetime
//						 function which will be used to get time for ModifyDate, Metadata date etc.
//
// =================================================================================================
#endif  // AdobePrivate

namespace AdobeXMPAM {
	class IExtensionsConfigurationManagerProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IExtensionsConfigurationManagerProxy;

#include "XMPExtensions/include/IExtensionsConfigurationManager.h"

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPExtensions/include/IExtensionsObjectFactory.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPExtensions/include/IExtensionsConfigurationManager.h"
#include <assert.h>

namespace AdobeXMPAM {

	IExtensionsConfigurationManagerProxy::IExtensionsConfigurationManagerProxy(pIExtensionsConfigurationManager ptr)
		: IConfigurationManagerProxy(ptr)
		, mRawPtr(ptr) {}

	IExtensionsConfigurationManagerProxy::~IExtensionsConfigurationManagerProxy() {}

	pIExtensionsConfigurationManager APICALL IExtensionsConfigurationManagerProxy::GetActualIExtensionsConfigurationManager() __NOTHROW__{ return mRawPtr; }

	void APICALL IExtensionsConfigurationManagerProxy::Acquire() const __NOTHROW__{ assert(false); }

	void APICALL IExtensionsConfigurationManagerProxy::Release() const __NOTHROW__{ assert(false); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL IExtensionsConfigurationManagerProxy::GetISharedObject_I() __NOTHROW__{
		return mRawPtr->GetISharedObject_I();
	}

		AdobeXMPAM_Int::pIExtensionsConfigurationManager_I APICALL IExtensionsConfigurationManagerProxy::GetIExtensionsConfigurationManager_I() __NOTHROW__{
		return mRawPtr->GetIExtensionsConfigurationManager_I();
	}

		pvoid APICALL IExtensionsConfigurationManagerProxy::GetInterfacePointer(uint64 interfaceID, uint32 interfaceVersion) {
		return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
			mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion);
	}

	pvoid APICALL IExtensionsConfigurationManagerProxy::getInterfacePointer(uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error) __NOTHROW__{
		assert(false);
		return mRawPtr->getInterfacePointer(interfaceID, interfaceVersion, error);
	}

		bool APICALL IExtensionsConfigurationManagerProxy::RegisterAppName(const char * appName, sizet appNameLength) {
		return CallSafeFunction< IExtensionsConfigurationManager, bool, uint32, const char *, sizet >(
			mRawPtr, &IExtensionsConfigurationManager::registerAppName, appName, appNameLength);
	}

	uint32 APICALL IExtensionsConfigurationManagerProxy::registerAppName(const char * appName, sizet appNameLength, pcIError_base & error) __NOTHROW__{
		return mRawPtr->registerAppName(appName, appNameLength, error);
	}


		bool APICALL IExtensionsConfigurationManagerProxy::RegisterPrefix(const char * docIDPrefix, sizet docIDPrefixLength, const eXMPIDPrefix prefixType) {
		return CallSafeFunction<IExtensionsConfigurationManager, bool, uint32, const char *, sizet, eXMPIDPrefix>(
			mRawPtr, &IExtensionsConfigurationManager::registerPrefix, docIDPrefix, docIDPrefixLength, prefixType);
	}

	uint32 APICALL IExtensionsConfigurationManagerProxy::registerPrefix(const char * docIDPrefix, sizet docIDPrefixLength, const eXMPIDPrefix prefixType, pcIError_base & error) __NOTHROW__{
		return mRawPtr->registerPrefix(docIDPrefix, docIDPrefixLength, prefixType, error);
	}

		bool APICALL IExtensionsConfigurationManagerProxy::RegisterHistoryUpdatePolicy(const eHistoryUpdatePolicy historyUpdatePolicy, const eHistoryUpdateAction HistoryUpdateAction) {
		return CallSafeFunction<IExtensionsConfigurationManager, bool, uint32, eHistoryUpdatePolicy, eHistoryUpdateAction>(
			mRawPtr, &IExtensionsConfigurationManager::registerHistoryUpdatePolicy, historyUpdatePolicy, HistoryUpdateAction);
	}

	uint32 APICALL IExtensionsConfigurationManagerProxy::registerHistoryUpdatePolicy(const eHistoryUpdatePolicy historyUpdatePolicy, const eHistoryUpdateAction HistoryUpdateAction, pcIError_base & error) __NOTHROW__{
		return mRawPtr->registerHistoryUpdatePolicy(historyUpdatePolicy, HistoryUpdateAction, error);
	}


		bool APICALL IExtensionsConfigurationManagerProxy::RegisterGetTimeCallback(GetDateTimeCallbackPtr datetimeCallback) {
		return CallSafeFunction< IExtensionsConfigurationManager, bool, uint32, GetDateTimeCallbackPtr >(
			mRawPtr, &IExtensionsConfigurationManager::registerGetTimeCallback, datetimeCallback);
	}

	uint32 APICALL IExtensionsConfigurationManagerProxy::registerGetTimeCallback(GetDateTimeCallbackPtr datetimeCallback, pcIError_base & error) __NOTHROW__{
		return mRawPtr->registerGetTimeCallback(datetimeCallback, error);
	}

	uint32 APICALL IExtensionsConfigurationManagerProxy::registerExtensionHandler(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtensionHandler customHandler, pcIError_base & error) __NOTHROW__{

		return mRawPtr->registerExtensionHandler(extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen, customHandler, error);
	}

	bool APICALL IExtensionsConfigurationManagerProxy::RegisterExtensionHandler(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtensionHandler customHandler) {

		return CallSafeFunction< IExtensionsConfigurationManager, bool, uint32, const char *, sizet, const char *, sizet, pIExtensionHandler>(
			mRawPtr, &IExtensionsConfigurationManager::registerExtensionHandler, extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen, customHandler);
	}


		spIExtensionsConfigurationManager IExtensionsConfigurationManager_v1::MakeShared(pIExtensionsConfigurationManager_base ptr) {
		if (!ptr) return spIExtensionsConfigurationManager();
		pIExtensionsConfigurationManager p = GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IExtensionsConfigurationManager >() : ptr;
		return shared_ptr< IExtensionsConfigurationManager >(new IExtensionsConfigurationManagerProxy(p));
	}
}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

