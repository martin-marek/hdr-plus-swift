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
// 12-01-14 ADC 1.0-a026 Fixing inheritance hierarchy issue in public exposed proxy classes
//						 for specialized part interfaces.
// 12-01-14 ADC 1.0-a023 Fixed the issue with public source not compiling on iOS.
// 11-21-14 ADC 1.0-a004 Support for IAssetLayerPart interface.
//
// =================================================================================================
#endif  // AdobePrivate

namespace AdobeXMPAM {
	class IAssetLayerPartProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IAssetLayerPartProxy;

#include "XMPAssetManagement/Interfaces/IAssetLayerPart.h"

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPExtensions/include/IExtensionsObjectFactory.h"

namespace AdobeXMPAM {
#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	class IAssetLayerPartProxy 
		: public virtual IAssetLayerPart
		, public virtual IAssetPartProxy 
	{
	private:
		pIAssetLayerPart mRawPtr;
	public:
		IAssetLayerPartProxy( pIAssetLayerPart ptr )
			: mRawPtr( ptr )
			, IAssetPartProxy( ptr ) {}

		~IAssetLayerPartProxy() {}

		pIAssetLayerPart APICALL GetActualIAssetLayerPart() __NOTHROW__ { return mRawPtr; }

		AdobeXMPAM_Int::pIAssetLayerPart_I APICALL GetIAssetLayerPart_I() __NOTHROW__ {
			return mRawPtr->GetIAssetLayerPart_I();
		}

		spcIUTF8String APICALL GetID() const {
			return CallConstSafeFunctionReturningPointer< IAssetLayerPart, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &IAssetLayerPart::getID );
		}

		pcIUTF8String_base APICALL getID(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getID( error );
		}

			void APICALL SetName(const char * name, sizet length) {
			CallSafeFunctionReturningVoid< IAssetLayerPart, const char *, sizet>( mRawPtr, &IAssetLayerPart::setName, name, length );
		}

		void APICALL setName(const char * name, sizet length, pcIError_base & error) __NOTHROW__{
			mRawPtr->setName( name, length, error );
		}

		spcIUTF8String APICALL GetName() const {
			return CallConstSafeFunctionReturningPointer< IAssetLayerPart, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &IAssetLayerPart::getName );
		}

		pcIUTF8String_base APICALL getName(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getName( error );
		}

		spIAssetPart APICALL Clone() const {
			return CallConstSafeFunctionReturningPointer<IAssetLayerPart, pIAssetPart_base, IAssetLayerPart>(
				mRawPtr, &IAssetLayerPart::clone);

		}

		pIAssetPart_base APICALL clone(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->clone( error );
		}
	};

	spIAssetLayerPart IAssetLayerPart_v1::CreateStandardLayerPart( eAssetPartComponent component, const char * id, sizet idLength )
	{
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetLayerPart_base, IAssetLayerPart, uint32, const char *, sizet>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateStandardLayerPart,
			static_cast< uint32 >( component ), id, idLength );
	}

	spIAssetLayerPart IAssetLayerPart_v1::CreateUserDefinedLayerPart(const char * component, sizet componentLength, const char * id, sizet idLength)
	{
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetLayerPart_base, IAssetLayerPart, const char *,sizet, const char *, sizet>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateUserDefinedLayerPart,
			component,componentLength, id, idLength);
	}

	spIAssetLayerPart IAssetLayerPart_v1::MakeShared(pIAssetPart_base ptr) {
		if ( !ptr ) return spIAssetLayerPart();
		pIAssetPart p = IAssetPart::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IAssetPart >() : ptr;
		// need to downcast the pointer when raw pointer is kept
		pIAssetLayerPart pt = dynamic_cast<pIAssetLayerPart>(p);
		return shared_ptr< IAssetLayerPart >(new IAssetLayerPartProxy(pt));
	}
#if XMP_WinBuild
#pragma warning( pop )
#endif

}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
