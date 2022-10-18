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
// 12-05-14 SKP 1.0-a038 Changed API name from IsTrackAble() to IsTrackable() and from 
//						 MakeTrackAble() to MakeTrackable()
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

namespace AdobeXMPAM {
	class IAssetUtilitiesProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IAssetUtilitiesProxy;

#include "XMPAssetManagement/Interfaces/IAssetUtilities.h"

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPExtensions/include/IExtensionsObjectFactory.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCore/Interfaces/IMetadata.h"
#include <assert.h>

namespace AdobeXMPAM {
	class IAssetUtilitiesProxy
		: public virtual IAssetUtilities
	{
	private:
		pIAssetUtilities mRawPtr;

	public:
		IAssetUtilitiesProxy( pIAssetUtilities ptr )
			: mRawPtr( ptr )
		{
			//mRawPtr->Acquire();
		}

		~IAssetUtilitiesProxy() { //mRawPtr->Release(); 
		}

		pIAssetUtilities APICALL GetActualIAssetUtilities() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); 
		}

		void APICALL Release() const __NOTHROW__ { assert( false ); 
		}

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPAM_Int::pIAssetUtilities_I APICALL GetIAssetUtilities_I() __NOTHROW__ {
			return mRawPtr->GetIAssetUtilities_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}



		bool APICALL IsTrackable( const spcIMetadata & metadata ) {
			return CallSafeFunction< IAssetUtilities, bool, uint32, pcIMetadata_base >(
				mRawPtr, &IAssetUtilities::isTrackable, metadata? metadata->GetActualIMetadata():NULL );
		}

		uint32  APICALL isTrackable( const pcIMetadata_base metadata, pcIError_base & error ) __NOTHROW__ {
			return mRawPtr->isTrackable( metadata, error );
		}

		bool APICALL MakeTrackable( const spIMetadata & metadata ) {
			return CallSafeFunction< IAssetUtilities, bool, uint32, pIMetadata_base >(
				mRawPtr, &IAssetUtilities::makeTrackable, metadata ? metadata->GetActualIMetadata() : NULL);
		}

		uint32 APICALL makeTrackable( const pIMetadata_base metadata, pcIError_base & error ) __NOTHROW__ {
			return mRawPtr->makeTrackable( metadata, error );
		}
	};
	spIAssetUtilities IAssetUtilities_v1::MakeShared(pIAssetUtilities_base ptr) {
		if ( !ptr ) return spIAssetUtilities();
		pIAssetUtilities p = IAssetUtilities::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IAssetUtilities >() : ptr;
		return shared_ptr< IAssetUtilities >(new IAssetUtilitiesProxy(p));
	}
	pIAssetUtilities getAssetUtilities( bool reset ) {
		static pIAssetUtilities ptr( NULL );
		if ( reset ) {
			delete ptr;
			ptr = NULL;
			return ptr;
		}

		if ( NULL == ptr ) {
			pcIError_base error;
			IAssetUtilities * basePtr = dynamic_cast< IAssetUtilities * >(
				IExtensionsObjectFactory::GetExtensionsObjectFactory()->GetAssetUtilities( error ) );
			if ( error ) {
				throw IError::MakeShared( error );
			}
			if ( basePtr ) {
				pIAssetUtilities p = IAssetUtilities::GetInterfaceVersion() > 1 ? basePtr->GetInterfacePointer< IAssetUtilities >() : basePtr;
				ptr = new IAssetUtilitiesProxy( p );
			}
			return ptr;
		} else {
			return ptr;
		}
	}

	pIAssetUtilities IAssetUtilities_v1::GetAssetUtilities() {
		return getAssetUtilities( false );
	}


}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

