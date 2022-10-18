#ifndef IExtensionsConfigurationManager_h__
#define IExtensionsConfigurationManager_h__ 1

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
// 03-01-15 SKP 1.0-a073 Enable XMPAssetUnit test runner and fixed the PK build break.
// 03-01-15 SKP 1.0-a072 Reverted back changed in cl: 157586; fixed build break.
// 03-01-15 SKP 1.0-a071 Fixed PK build break. Stopped building XMPAM unit test project.
// 02-16-15 SKP 1.0-a053 Fixed XMPToolkit build break
// 02-11-15 SKP 1.0-a050 Implementing support for XMPDateTime, History update policy and action
// 02-11-15 SKP 1.0-a049 Added initial support for client strategies. Using these strategies client
//						 can set prefix, AppName, history update policy and it's own datetime
//						 function which will be used to get time for ModifyDate, Metadata date etc.
//
// =================================================================================================
#endif	// AdobePrivate

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IThreadSafe.h"

#include "XMPCommon/Interfaces/IConfigurationManager.h"
#include "XMPMarker/XMPMarkerFwdDeclarations.h"


namespace AdobeXMPAM {

	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCore;
	//!
	//! \brief Version 1 of the interface that represents configuration settings which client can configure.
	//! \details Provides functions through which client can set its own UUID prefix, AppName, register their
	//! function for setting XMP date time to update xmpMM:ModifyDate, xmpMM:MetadataDate, stEvt:when in 
	//! history and policy to update the history entry.
	//!

	class XMP_PUBLIC IExtensionsConfigurationManager_v1
		: public virtual IConfigurationManager_v1
	{
	public:
		//!
		//! \brief Option bit flags for various strategies to update the history on composition
		//!
		typedef enum {
			//! Default policy to make to update history for every composition.
			kUpdateEveryTime = 0x00000001,

			//! For all composition make a single entry
			kUpdateOneTime = 0x00000002,

			//! Ignore history entry
			kUpdateSkip = 0x00000004,

			//Add new history update policy here

			//! Maximum value this enum can hold, should be treated as invalid value.
			kHistoryUpdateMaxValue = kMaxEnumValue
		} eHistoryUpdatePolicy;

		//!
		//! \brief Option bit flags for action(s) for which history entry will be made
		//!
		typedef enum {
			//! [Default] On every action for which history should be updated, make an entry
			kHistoryActionAll = 0x00000001,

			//! Relationship has been created/added
			kHistoryActionAdd = 0x00000002,

			//! Relationship has been deleted
			kHistoryActionDelete = 0x00000004,

			//Add new history update action here
			//! Maximum value this enum can hold, should be treated as invalid value.
			kHistoryActionMaxValue = kMaxEnumValue
		} eHistoryUpdateAction;

		//!
		//! \brief Indicates prefix which will be added before UUID
		//!
		typedef enum {
			//! [Default] Add prefix for all ids
			kAllIDPrefix = 0x00000001,

			//! Prefix for xmpMM:DocumentID
			kDocumentIDPrefix = 0x00000002,

			//! Prefix for xmpMM:OriginalDocumentID
			kOriginalDocumentIDPrefix = 0x00000004,

			//! Prefix for xmpMM:InstanceID and stEvt:instanceID
			kInstanceIDPrefix = 0x00000008,

			//Add new ID Prefix here

			//! Maximum value this enum can hold, should be treated as invalid value.
			kIDPrefixMaxValue = kMaxEnumValue
		} eXMPIDPrefix;

		//!
		//! \brief Callback function pointer for XMP_DateTime. Return type bool and argument is address of XMP_DateTime
		//! structure
		//!
		typedef bool(*GetDateTimeCallbackPtr)(XMP_DateTime*);

		virtual pIExtensionsConfigurationManager APICALL GetActualIExtensionsConfigurationManager() __NOTHROW__ = 0;
		XMP_PRIVATE pcIExtensionsConfigurationManager GetActualIExtensionsConfigurationManager() const __NOTHROW__{
			return const_cast< IExtensionsConfigurationManager_v1 * >(this)->GetActualIExtensionsConfigurationManager();
		}

		virtual AdobeXMPAM_Int::pIExtensionsConfigurationManager_I APICALL GetIExtensionsConfigurationManager_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPAM_Int::pcIExtensionsConfigurationManager_I GetIExtensionsConfigurationManager_I() const __NOTHROW__{
			return const_cast< IExtensionsConfigurationManager_v1 * >(this)->GetIExtensionsConfigurationManager_I();
		}

		XMP_PRIVATE static spIExtensionsConfigurationManager MakeShared(pIExtensionsConfigurationManager_base ptr);
		XMP_PRIVATE static spcIExtensionsConfigurationManager MakeShared(pcIExtensionsConfigurationManager_base ptr) {
			return MakeShared(const_cast< pIExtensionsConfigurationManager_base >(ptr));
		}
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIExtensionsConfigurationManagerID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//!
		//! \brief Allows the client to set the application name which will be used for xmp:creator and stEvt:SoftwareAgent
		//! \param appName pointer to const char *
		//! \param appNameLength Length of char buffer containing appName 
		//! \return true if success otherwise false.
		//!
		virtual bool APICALL RegisterAppName(const char * appName, sizet appNameLength) = 0;

		//!
		//! \brief Allows the client to set prefix which will be used as prefix for xmpMM:OriginalDocumentID and xmpMM:DocumentID
		//! \param[in]	docIDPrefix DocumentID prefix in const char*
		//! \param docIDPrefixLength Length of char buffer containing docIDPrefix
		//! \return true if success otherwise false.
		//! \attention Error is thrown in case
		//!		-# Prefix is not supported
		//!
		virtual bool APICALL RegisterPrefix(const char * docIDPrefix, sizet docIDPrefixLength, const eXMPIDPrefix) = 0;

		//!
		//! \brief Allows the client to register HistoryUpdatePolicy. By default for every composition a history entry will be made
		//! \param historyUpdatePolicy Policy to make event history
		//! \param HistoryUpdateAction action for which history entry will make (based on the eHistoryUpdatePolicy)
		//! \return a value of bool type; true means successful and false otherwise.
		//!
		virtual bool APICALL RegisterHistoryUpdatePolicy(const eHistoryUpdatePolicy historyUpdatePolicy, const eHistoryUpdateAction HistoryUpdateAction) = 0;

		//!
		//! \brief Allows the client to set set call back for time which will be used for stEvt:When, xmp:ModifyDate etc. Default is current time
		// \param GetTimeCallbackPtr
		//! \return true if success otherwise false.
		//!
		virtual bool APICALL RegisterGetTimeCallback(GetDateTimeCallbackPtr) = 0;

		////!
		////! \brief Registers the extension handler and returns true if the extension handler is successfully registered.
		////! \param[in] extensionNameSpace const char * which would be the namespace of the extension for which the extension handler is to be registered
		////! \param[in] extensionNameSpaceLen Size which would be the length of the namespace of the extension for which the extension handler is to be registered
		////! \param[in] extensionName const char * which would be the name of the extension for which the extension handler is to be registered
		////! \param[in] extensionNameLen Size which would be the length of the name of the extension for which the extension handler is to be registered
		////! \param[in] customHandler a pointer to the ExtensionHandler object which is to be registerd
		////! \return bool
		////!
		virtual bool APICALL RegisterExtensionHandler(const char * extensionNameSpace, sizet  extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtensionHandler customHandler) = 0;

	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL registerAppName(const char * appName, sizet appNameLength, pcIError_base & error) __NOTHROW__ = 0;
		virtual uint32 APICALL registerPrefix(const char * docIDPrefix, sizet docIDPrefixLength, const eXMPIDPrefix, pcIError_base & error) __NOTHROW__ = 0;
		virtual uint32 APICALL registerHistoryUpdatePolicy(const eHistoryUpdatePolicy historyUpdatePolicy, const eHistoryUpdateAction HistoryUpdateAction, pcIError_base & error) __NOTHROW__ = 0;
		virtual uint32 APICALL registerGetTimeCallback(GetDateTimeCallbackPtr, pcIError_base & error) __NOTHROW__ = 0;
		virtual uint32 APICALL registerExtensionHandler(const char * extensionNameSpace, sizet  extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtensionHandler customHandler, pcIError_base & error) __NOTHROW__ = 0;
		//! \endcond

	protected:
		//!
		//! protected virtual destructor
		//!
		virtual ~IExtensionsConfigurationManager_v1() {}
#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
#if XMP_WinBuild
#pragma warning (push)
#pragma warning (disable : 4250)
#endif //#if XMP_WinBuild

	class IExtensionsConfigurationManagerProxy
		: public virtual IExtensionsConfigurationManager
		, public virtual IConfigurationManagerProxy
	{
	private:
		pIExtensionsConfigurationManager mRawPtr;

	public:
		IExtensionsConfigurationManagerProxy(pIExtensionsConfigurationManager ptr);

		~IExtensionsConfigurationManagerProxy();

		pIExtensionsConfigurationManager APICALL GetActualIExtensionsConfigurationManager() __NOTHROW__;

		void APICALL Acquire() const __NOTHROW__;

		void APICALL Release() const __NOTHROW__;

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__;

		AdobeXMPAM_Int::pIExtensionsConfigurationManager_I APICALL GetIExtensionsConfigurationManager_I() __NOTHROW__;
		pvoid APICALL GetInterfacePointer(uint64 interfaceID, uint32 interfaceVersion);
		bool APICALL RegisterAppName(const char * appName, sizet appNameLength);
		bool APICALL RegisterPrefix(const char * docIDPrefix, sizet docIDPrefixLength, const eXMPIDPrefix prefixType);
		bool APICALL RegisterHistoryUpdatePolicy(const eHistoryUpdatePolicy historyUpdatePolicy, const eHistoryUpdateAction HistoryUpdateAction);
		bool APICALL RegisterGetTimeCallback(GetDateTimeCallbackPtr datetimeCallback);

	protected:
		pvoid APICALL getInterfacePointer(uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error) __NOTHROW__;
		uint32 APICALL registerAppName(const char * appName, sizet appNameLength, pcIError_base & error) __NOTHROW__;
		uint32 APICALL registerPrefix(const char * docIDPrefix, sizet docIDPrefixLength, const eXMPIDPrefix prefixType, pcIError_base & error) __NOTHROW__;
		uint32 APICALL registerHistoryUpdatePolicy(const eHistoryUpdatePolicy historyUpdatePolicy, const eHistoryUpdateAction HistoryUpdateAction, pcIError_base & error) __NOTHROW__;
		uint32 APICALL registerGetTimeCallback(GetDateTimeCallbackPtr datetimeCallback, pcIError_base & error) __NOTHROW__;
		uint32 APICALL registerExtensionHandler(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtensionHandler customHandler, pcIError_base & error) __NOTHROW__;
		bool APICALL RegisterExtensionHandler(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtensionHandler customHandler);
	};
#if XMP_WinBuild
#pragma warning(pop)
#endif //#if XMP_WinBuild
#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
}

#endif // IExtensionsConfigurationManager_h__
