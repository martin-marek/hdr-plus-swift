// =================================================================================================
// Copyright Adobe
// Copyright 2015 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

namespace AdobeXMPCompareAndMerge {
	class IThreeWayMergeProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCompareAndMerge::IThreeWayMergeProxy;

#include "XMPCompareAndMerge/Interfaces/IThreeWayMerge.h"

#if !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB

#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCompareAndMerge/Interfaces/ICompareAndMergeObjectFactory.h"
#include "client-glue/WXMPMeta.hpp"
#include <assert.h>
#include "XMPCore/Interfaces/ICoreObjectFactory.h"
#include "XMPCore/XMPCoreErrorCodes.h"
#include "XMPCore/Interfaces/IMetadata.h"
#include "XMPCompareAndMerge/Interfaces/IThreeWayMergeResult.h"
#include "XMPCompareAndMerge/Interfaces/IThreeWayGenericStrategy.h"

namespace AdobeXMPCompareAndMerge {

	class IThreeWayMergeProxy
		: public virtual IThreeWayMerge
	{
	private:
		pIThreeWayMerge mRawPtr;

	public:
		IThreeWayMergeProxy( pIThreeWayMerge ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IThreeWayMergeProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIThreeWayMerge APICALL GetActualIThreeWayMerge() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCompareAndMerge_Int::pIThreeWayMerge_I APICALL GetIThreeWayMerge_I() __NOTHROW__ {
			return mRawPtr->GetIThreeWayMerge_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spcIThreeWayMergeResult APICALL Merge( const spcIMetadata & baseVersionMetadata, const spcIMetadata & latestVersionMedata, const spcIMetadata & userVersionMetadata ) const {
			return CallConstSafeFunctionReturningPointer< IThreeWayMerge_v1, pcIThreeWayMergeResult_base, const IThreeWayMergeResult, pcIMetadata_base, pcIMetadata_base, pcIMetadata_base >(
				mRawPtr, &IThreeWayMerge_v1::merge, baseVersionMetadata ? baseVersionMetadata->GetActualIMetadata() : NULL, latestVersionMedata ? latestVersionMedata->GetActualIMetadata() : NULL,
				userVersionMetadata ? userVersionMetadata->GetActualIMetadata() : NULL );
		}

		virtual spcIThreeWayMergeResult APICALL AutoMerge ( const spcIMetadata & baseVersionMetadata, const spcIMetadata & latestVersionMetadata, const spcIMetadata & userVersionMetadata, eAutoMergeStrategy skipAutoMergeStrategy ) const {
			return CallConstSafeFunctionReturningPointer< IThreeWayMerge_v1, pcIThreeWayMergeResult_base, const IThreeWayMergeResult, pcIMetadata_base, pcIMetadata_base, pcIMetadata_base, uint32 > (
				mRawPtr, &IThreeWayMerge_v1::autoMerge, baseVersionMetadata ? baseVersionMetadata->GetActualIMetadata () : NULL, latestVersionMetadata ? latestVersionMetadata->GetActualIMetadata () : NULL,
				userVersionMetadata ? userVersionMetadata->GetActualIMetadata () : NULL, static_cast<uint32> ( skipAutoMergeStrategy ) );
		}

		virtual spcIUTF8String APICALL AutoMerge ( const char * baseVersionMetadata, sizet baseVersionMetadataLength, const char * latestVersionMetadata, sizet latestVersionMetadataLength, const char * userVersionMetadata, sizet userVersionMetadataLength, bool & isMerged, eAutoMergeStrategy skipAutoMergeStrategy ) const {
			uint32 boolCast = 0; 
			spcIUTF8String returnValue = CallConstSafeFunctionReturningPointer< IThreeWayMerge_v1, pcIUTF8String_base, const IUTF8String, const char *, sizet, const char *, sizet, const char *, sizet, uint32 &, uint32 > (
				mRawPtr, &IThreeWayMerge_v1::autoMerge, baseVersionMetadata, baseVersionMetadataLength, latestVersionMetadata , latestVersionMetadataLength,
				userVersionMetadata, userVersionMetadataLength, boolCast, static_cast<uint32> (skipAutoMergeStrategy) );
			isMerged = static_cast<bool> (boolCast);
			return returnValue;
		}
        
        virtual SXMPMeta APICALL AutoMerge ( const SXMPMeta* baseVersionMetadata, const SXMPMeta* latestVersionMetadata, const SXMPMeta* userVersionMetadata, bool & isMerged , eAutoMergeStrategy skipAutoMergeStrategy = kAMSNone ) const
        {
            if(!baseVersionMetadata || !latestVersionMetadata || !userVersionMetadata)
            {
				spcIError err = IError::CreateError(ICoreObjectFactory_v1::GetCoreObjectFactory(), AdobeXMPCommon::IError::kEDGeneral, AdobeXMPCommon::kGECParametersNotAsExpected, AdobeXMPCommon::IError::kESProcessFatal);
				throw err;
            }
            pcIError_base error( NULL );
            uint32 boolCast = 0;
            XMPMetaRef returnValue = mRawPtr->autoMerge(baseVersionMetadata->GetInternalRef(),latestVersionMetadata->GetInternalRef(),userVersionMetadata->GetInternalRef(),boolCast,skipAutoMergeStrategy,error);
            if ( error ) throw IError::MakeShared( error );
            isMerged = static_cast<bool> (boolCast);
            SXMPMeta mergedMeta(returnValue);
            WXMPMeta_DecrementRefCount_1(returnValue);
            return mergedMeta;
        }

		virtual bool APICALL RegisterThreeWayGenericStrategy( const spIThreeWayGenericStrategy & strategy ) {
			return CallSafeFunction< IThreeWayMerge_v1, bool, uint32, pIThreeWayGenericStrategy_base >(
				mRawPtr, &IThreeWayMerge_v1::registerThreeWayGenericStrategy, strategy ? strategy->GetActualIThreeWayGenericStrategy() : NULL );
		}

		virtual uint32 APICALL registerThreeWayGenericStrategy( pIThreeWayGenericStrategy_base strategy, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->registerThreeWayGenericStrategy( strategy, error );
		}

		virtual bool APICALL RegisterThreeWayStrategy( pIThreeWayStrategy_base strategy ) {
			return CallSafeFunction< IThreeWayMerge_v1, bool, uint32, pIThreeWayStrategy_base >(
				mRawPtr, &IThreeWayMerge_v1::registerThreeWayStrategy, strategy );
		}

		virtual pcIThreeWayMergeResult_base APICALL merge( pcIMetadata_base baseVersionMetadata, pcIMetadata_base latestVersionMedata, pcIMetadata_base userVersionMetadata, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->merge( baseVersionMetadata, latestVersionMedata, userVersionMetadata, error );
		}

		virtual pcIThreeWayMergeResult_base APICALL autoMerge ( pcIMetadata_base baseVersionMetadata, pcIMetadata_base latestVersionMetadata, pcIMetadata_base userVersionMetadata, uint32 skipAutoMergeStrategy, pcIError_base & error ) const __NOTHROW__ {
		assert ( false );
			return mRawPtr->autoMerge ( baseVersionMetadata, latestVersionMetadata , userVersionMetadata, skipAutoMergeStrategy, error );
		}

		virtual pcIUTF8String_base APICALL autoMerge ( const char * baseVersionMetadata, sizet baseVersionMetadataLength, const char * latestVersionMetadata, sizet latestVersionMetadataLength, const char * userVersionMetadata, sizet userVersionMetadataLength, uint32 & isMerged, uint32 skipAutoMergeStrategy, pcIError_base & error ) const __NOTHROW__ {
			assert ( false );
			return mRawPtr->autoMerge ( baseVersionMetadata, baseVersionMetadataLength, latestVersionMetadata , latestVersionMetadataLength, userVersionMetadata, userVersionMetadataLength, isMerged,skipAutoMergeStrategy, error );
		}
        
        virtual XMPMetaRef APICALL autoMerge ( XMPMetaRef baseVersionMetadata, XMPMetaRef latestVersionMetadata, XMPMetaRef userVersionMetadata, uint32 & isMerged, uint32 skipAutoMergeStrategy, pcIError_base & error ) const __NOTHROW__ {
            assert ( false );
            return mRawPtr->autoMerge ( baseVersionMetadata, latestVersionMetadata, userVersionMetadata, isMerged,skipAutoMergeStrategy, error );
        }

		virtual uint32 APICALL registerThreeWayStrategy( pIThreeWayStrategy_base strategy, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->registerThreeWayStrategy( strategy, error );
		}

	};

	spIThreeWayMerge IThreeWayMerge_v1::MakeShared( pIThreeWayMerge_base ptr ) {
		if ( !ptr ) return spIThreeWayMerge();
		pIThreeWayMerge p = IThreeWayMerge::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< IThreeWayMerge >() : ptr;
		return shared_ptr< IThreeWayMerge >( new IThreeWayMergeProxy( p ) );
	}

	spIThreeWayMerge IThreeWayMerge_v1::CreateThreeWayMerge() {
		return CallSafeFunctionReturningPointer< ICompareAndMergeObjectFactory, pIThreeWayMerge_base, IThreeWayMerge >(
			ICompareAndMergeObjectFactory::GetCompareAndMergeObjectFactory(), &ICompareAndMergeObjectFactory::CreateThreeWayMerge);
	}

}

#endif  // !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB
