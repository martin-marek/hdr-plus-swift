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
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

namespace AdobeXMPAM {
	class IAssetPartProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IAssetPartProxy;

#include "XMPAssetManagement/Interfaces/IAssetPart.h"

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPExtensions/include/IExtensionsObjectFactory.h"

namespace AdobeXMPAM {

	IAssetPartProxy::IAssetPartProxy( pIAssetPart ptr ) : mRawPtr( ptr ) {
		mRawPtr->Acquire();
	}

	IAssetPartProxy::~IAssetPartProxy() {
		mRawPtr->Release();
	}

	pIAssetPart APICALL IAssetPartProxy::GetActualIAssetPart() __NOTHROW__ {
		return mRawPtr;
	}

	AdobeXMPAM_Int::pIAssetPart_I APICALL IAssetPartProxy::GetIAssetPart_I() __NOTHROW__ {
		return mRawPtr->GetIAssetPart_I();
	}

	pvoid APICALL IAssetPartProxy::getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
		assert( false );
		return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
	}

	pvoid APICALL IAssetPartProxy::GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
		return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
			mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
	}
	
	void APICALL IAssetPartProxy::Acquire() const __NOTHROW__{
		assert(false);
	}

	void APICALL IAssetPartProxy::Release() const __NOTHROW__{
		assert(false);
	}

	AdobeXMPCommon_Int::pISharedObject_I APICALL IAssetPartProxy::GetISharedObject_I() __NOTHROW__{
		return mRawPtr->GetISharedObject_I();
	}

	AdobeXMPCommon_Int::pIThreadSafe_I APICALL IAssetPartProxy::GetIThreadSafe_I() __NOTHROW__{
		return mRawPtr->GetIThreadSafe_I();
	}

	void APICALL IAssetPartProxy::EnableThreadSafety() const __NOTHROW__{
		return mRawPtr->EnableThreadSafety();
	}

	void APICALL IAssetPartProxy::DisableThreadSafety() const __NOTHROW__{
		return mRawPtr->DisableThreadSafety();
	}

	bool APICALL IAssetPartProxy::IsThreadSafe() const {
		return mRawPtr->isThreadSafe() != 0;
	}

	uint32 APICALL IAssetPartProxy::isThreadSafe() const __NOTHROW__{
		assert(false);
		return mRawPtr->isThreadSafe();
	}

	spIAssetPart IAssetPart_v1::CreateStandardGenericPart( eAssetPartComponent component )
	{
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetPart_base, IAssetPart, uint32>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateStandardGenericPart,
			static_cast< uint32 >( component ) );
	}

	spIAssetPart IAssetPart_v1::CreateUserDefinedGenericPart( const char * component, sizet length)
	{
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetPart_base, IAssetPart, const char *, sizet>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateUserDefinedGenericPart,
			component, length);
	}

	IAssetPart_v1::eAssetPartType APICALL IAssetPartProxy::GetType() const {
		return CallConstSafeFunction< IAssetPart, eAssetPartType, uint32 >( mRawPtr, &IAssetPart::getType );
	}

	uint32 APICALL IAssetPartProxy::getType( pcIError_base & error ) const __NOTHROW__ {
		return mRawPtr->getType( error );
	}

	IAssetPart_v1::eAssetPartComponent APICALL IAssetPartProxy::GetComponent() const {
		return CallConstSafeFunction< IAssetPart, eAssetPartComponent, uint32 >( mRawPtr, &IAssetPart::getComponent );
	}

	uint32 APICALL IAssetPartProxy::getComponent( pcIError_base & error ) const __NOTHROW__ {
		return mRawPtr->getComponent( error );
	}

	spcIUTF8String APICALL IAssetPartProxy::GetUserDefinedComponent() const {
		return CallConstSafeFunctionReturningPointer< IAssetPart, pcIUTF8String_base, const IUTF8String >(
			mRawPtr, &IAssetPart::getUserDefinedComponent );
	}

	pcIUTF8String_base APICALL IAssetPartProxy::getUserDefinedComponent( pcIError_base & error ) const __NOTHROW__ {
		return mRawPtr->getUserDefinedComponent( error );
	}

	spIAssetPart APICALL IAssetPartProxy::Clone() const {
		return CallConstSafeFunctionReturningPointer<IAssetPart, pIAssetPart_base, IAssetPart>(
			mRawPtr, &IAssetPart::clone);
	}

	pIAssetPart_base APICALL IAssetPartProxy::clone(pcIError_base & error ) const __NOTHROW__ {
		return mRawPtr->clone( error );
	}

	spIAssetPart IAssetPart_v1::MakeShared(pIAssetPart_base ptr) {
		if ( !ptr ) return spIAssetPart();
		pIAssetPart p = IAssetPart::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IAssetPart >() : ptr;
		return shared_ptr< IAssetPart >(new IAssetPartProxy(p));
	}

}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

