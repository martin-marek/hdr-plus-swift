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
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
//
// =================================================================================================
#endif  // AdobePrivate

namespace AdobeXMPAM {
	class IAssetArtboardPartProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IAssetArtboardPartProxy;

#include "XMPAssetManagement/Interfaces/IAssetArtboardPart.h"

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPExtensions/include//IExtensionsObjectFactory.h"

namespace AdobeXMPAM {

#if XMP_WinBuild
#pragma warning( push )
#pragma warning( disable : 4250 )
#endif

	class IAssetArtboardPartProxy 
		: public virtual IAssetArtboardPart
		, public virtual IAssetPartProxy 
	{
	private:
		pIAssetArtboardPart mRawPtr;
	public:
		IAssetArtboardPartProxy( pIAssetArtboardPart ptr )
			: mRawPtr( ptr )
			, IAssetPartProxy( ptr ) {}

		~IAssetArtboardPartProxy() {}

		pIAssetArtboardPart APICALL GetActualIAssetArtboardPart() __NOTHROW__ { return mRawPtr; }

		AdobeXMPAM_Int::pIAssetArtboardPart_I APICALL GetIAssetArtboardPart_I() __NOTHROW__ {
			return mRawPtr->GetIAssetArtboardPart_I();
		}

		spcIUTF8String APICALL GetID() const {
			return CallConstSafeFunctionReturningPointer<IAssetArtboardPart, pcIUTF8String_base, const IUTF8String>(
				mRawPtr, &IAssetArtboardPart::getID );
		}

		pcIUTF8String_base APICALL getID( pcIError_base & error ) const __NOTHROW__ {
			return mRawPtr->getID( error );
		}

		void APICALL SetName( const char * name, sizet length ) {
			CallSafeFunctionReturningVoid< IAssetArtboardPart, const char *,sizet >(
				mRawPtr, &IAssetArtboardPart::setName, name, length );
		}

		void APICALL setName( const char * name, sizet length, pcIError_base & error ) __NOTHROW__ {
			mRawPtr->setName( name, length, error );
		}

		spcIUTF8String APICALL GetName() const {
			return CallConstSafeFunctionReturningPointer<IAssetArtboardPart, pcIUTF8String_base, const IUTF8String>(
				mRawPtr, &IAssetArtboardPart::getName );
		}

		pcIUTF8String_base APICALL getName( pcIError_base & error ) const __NOTHROW__ {
			return mRawPtr->getName( error );
		}

		spIAssetPart APICALL Clone() const {
			return CallConstSafeFunctionReturningPointer<IAssetArtboardPart, pIAssetPart_base, IAssetArtboardPart>( 
				mRawPtr, &IAssetArtboardPart::clone );
		}

		pIAssetPart_base APICALL clone( pcIError_base & error ) const __NOTHROW__ {
			return mRawPtr->clone( error );
		}
	};

	spIAssetArtboardPart IAssetArtboardPart_v1::CreateStandardArtboardPart( eAssetPartComponent component, const char * id, sizet idLength )
	{
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetArtboardPart_base, IAssetArtboardPart, uint32, const char *, sizet>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateStandardArtboardPart,
			static_cast< uint32 >( component ), id, idLength );
	}

	spIAssetArtboardPart IAssetArtboardPart_v1::CreateUserDefinedArtboardPart( const char * component, sizet componentLength, const char * id, sizet idLength )
	{
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetArtboardPart_base, IAssetArtboardPart, const char *, sizet, const char *, sizet>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateUserDefinedArtboardPart,
			component, componentLength, id, idLength );
	}

	spIAssetArtboardPart IAssetArtboardPart_v1::MakeShared(pIAssetPart_base ptr) {
		if ( !ptr ) return spIAssetArtboardPart();
		pIAssetPart p = IAssetPart::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IAssetPart >() : ptr;
		// need to downcast the pointer when raw pointer is kept
		pIAssetArtboardPart pt = dynamic_cast<pIAssetArtboardPart>(p);
		return shared_ptr< IAssetArtboardPart >(new IAssetArtboardPartProxy(pt));
	}
#if XMP_WinBuild
#pragma warning(pop)
#endif //#if XMP_WinBuild
}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
