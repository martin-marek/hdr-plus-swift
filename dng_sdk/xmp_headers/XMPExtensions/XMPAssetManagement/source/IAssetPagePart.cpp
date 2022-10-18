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
//  SKP Sunil Kishor Pathak
//
// mm-dd-yy who Description of changes, most recent first.
//
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 12-01-14 ADC 1.0-a026 Fixing inheritance hierarchy issue in public exposed proxy classes
//						 for specialized part interfaces.
// 12-01-14 ADC 1.0-a023 Fixed the issue with public source not compiling on iOS.
// 11-23-14 SKP 1.0-a006 Support for IAssetPagePart interface.
//
// =================================================================================================
#endif  // AdobePrivate

namespace AdobeXMPAM {
	class IAssetPagePartProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IAssetPagePartProxy;
#include "XMPAssetManagement/Interfaces/IAssetPagePart.h"

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

	class IAssetPagePartProxy 
		: public virtual IAssetPagePart
		, public virtual IAssetPartProxy 
	{
	private:
		pIAssetPagePart mRawPtr;
	public:
		IAssetPagePartProxy( pIAssetPagePart ptr )
			: mRawPtr( ptr )
			, IAssetPartProxy( ptr ) {}

		~IAssetPagePartProxy() {}

		pIAssetPagePart APICALL GetActualIAssetPagePart() __NOTHROW__ { return mRawPtr; }

			AdobeXMPAM_Int::pIAssetPagePart_I APICALL GetIAssetPagePart_I() __NOTHROW__ {
			return mRawPtr->GetIAssetPagePart_I();
		}

		sizet APICALL GetNumber() const __NOTHROW__ {
			return mRawPtr->GetNumber();
		}

		void APICALL SetDescriptivePageNumber( const char * name, sizet length ) __NOTHROW__ {
			mRawPtr->SetDescriptivePageNumber( name, length );
		}

		spcIUTF8String APICALL GetDescriptivePageNumber() const {
			return CallConstSafeFunctionReturningPointer< IAssetPagePart, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &IAssetPagePart::getDescriptivePageNumber );
		}

		pcIUTF8String_base APICALL getDescriptivePageNumber( pcIError_base & error ) const __NOTHROW__ {
			return mRawPtr->getDescriptivePageNumber( error );
		}

		spIAssetPart APICALL Clone() const {
			return CallConstSafeFunctionReturningPointer<IAssetPagePart, pIAssetPart_base, IAssetPagePart
			>( mRawPtr, &IAssetPagePart::clone );
		}

		pIAssetPart_base APICALL clone( pcIError_base & error ) const __NOTHROW__ {
			return mRawPtr->clone( error );
		}
	};

	spIAssetPagePart IAssetPagePart_v1::CreateStandardPagePart( eAssetPartComponent component, sizet no)
	{
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetPagePart_base, IAssetPagePart, uint32, sizet>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateStandardPagePart,
			static_cast< uint32 >( component ), no );
	}

	spIAssetPagePart IAssetPagePart_v1::CreateUserDefinedPagePart(const char * component, sizet length, sizet no)
	{
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetPagePart_base, IAssetPagePart, const char *, sizet, sizet>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateUserDefinedPagePart,
			component, length, no);
	}

	spIAssetPagePart IAssetPagePart_v1::MakeShared( pIAssetPart_base ptr ) {
		if ( !ptr ) return spIAssetPagePart();
		pIAssetPart p = IAssetPart::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IAssetPart >() : ptr;
		// need to downcast the pointer when raw pointer is kept
		pIAssetPagePart pt = dynamic_cast<pIAssetPagePart>(p);
		return shared_ptr< IAssetPagePart >(new IAssetPagePartProxy(pt));
	}
#if XMP_WinBuild
	#pragma warning( pop )
#endif

}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
