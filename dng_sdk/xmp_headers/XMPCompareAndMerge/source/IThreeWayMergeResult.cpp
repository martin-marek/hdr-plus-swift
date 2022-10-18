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
	class IThreeWayMergeResultProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCompareAndMerge::IThreeWayMergeResultProxy;

#include "XMPCompareAndMerge/Interfaces/IThreeWayMergeResult.h"

#if !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>

#include "XMPCompareAndMerge/Interfaces/IThreeWayResolvedConflict.h"
#include "XMPCompareAndMerge/Interfaces/IThreeWayUnResolvedConflict.h"

#include "XMPCore/Interfaces/IMetadata.h"

namespace AdobeXMPCompareAndMerge {

	class IThreeWayMergeResultProxy
		: public virtual IThreeWayMergeResult
	{
	private:
		pIThreeWayMergeResult mRawPtr;

	public:
		IThreeWayMergeResultProxy( pIThreeWayMergeResult ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IThreeWayMergeResultProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIThreeWayMergeResult APICALL GetActualIThreeWayMergeResult() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCompareAndMerge_Int::pIThreeWayMergeResult_I APICALL GetIThreeWayMergeResult_I() __NOTHROW__ {
			return mRawPtr->GetIThreeWayMergeResult_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual sizet APICALL CountOfResolvedConflicts() const __NOTHROW__ {
			return mRawPtr->CountOfResolvedConflicts();
		}

		virtual sizet APICALL CountOfUnResolvedConflicts() const __NOTHROW__ {
			return mRawPtr->CountOfUnResolvedConflicts();
		}

		virtual spIMetadata APICALL GetMergedMetadata() const {
			return CallConstSafeFunctionReturningPointer< IThreeWayMergeResult, pIMetadata_base, IMetadata >(
				mRawPtr, &IThreeWayMergeResult::getMergedMetadata );
		}

		virtual eMergeStatus APICALL GetMergedStatus() const {
			return CallConstSafeFunction< IThreeWayMergeResult, eMergeStatus, uint32 >(
				mRawPtr, &IThreeWayMergeResult::getMergedStatus );
		}

		virtual spcIThreeWayResolvedConflict APICALL GetResolvedConflict( sizet index ) const {
			return CallConstSafeFunctionReturningPointer< IThreeWayMergeResult, pcIThreeWayResolvedConflict_base, const IThreeWayResolvedConflict, sizet >(
				mRawPtr, &IThreeWayMergeResult::getResolvedConflict, index );
		}

		virtual spcIThreeWayResolvedConflicts_const APICALL GetResolvedConflicts() const {
			sizet nCount = CountOfResolvedConflicts();
			spcIThreeWayResolvedConflicts list = shared_ptr< cIThreeWayResolvedConflicts >( new cIThreeWayResolvedConflicts( nCount ) );
			for ( sizet i = 0; i < nCount; ++i ) {
				list->at( i ) = GetResolvedConflict( i );
			}
			return list;
		}

		virtual spcIThreeWayUnResolvedConflict APICALL GetUnResolvedConflict( sizet index ) const {
			return CallConstSafeFunctionReturningPointer< IThreeWayMergeResult, pcIThreeWayUnResolvedConflict_base, const IThreeWayUnResolvedConflict, sizet >(
				mRawPtr, &IThreeWayMergeResult::getUnResolvedConflict, index );
		}

		virtual spcIThreeWayUnResolvedConflicts_const APICALL GetUnResolvedConflicts() const {
			sizet nCount = CountOfUnResolvedConflicts();
			spcIThreeWayUnResolvedConflicts list = shared_ptr< cIThreeWayUnResolvedConflicts >( new cIThreeWayUnResolvedConflicts( nCount ) );
			for ( sizet i = 0; i < nCount; ++i ) {
				list->at( i ) = GetUnResolvedConflict( i );
			}
			return list;
		}

		virtual pIMetadata_base APICALL getMergedMetadata( pcIError_base & err ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getMergedMetadata( err );
		}

		virtual uint32 APICALL getMergedStatus( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getMergedStatus( error );
		}

		virtual pcIThreeWayResolvedConflict_base APICALL getResolvedConflict( sizet index, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getResolvedConflict( index, error );
		}

		virtual pcIThreeWayUnResolvedConflict_base APICALL getUnResolvedConflict( sizet index, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getUnResolvedConflict( index, error );
		}

	};

	spIThreeWayMergeResult IThreeWayMergeResult_v1::MakeShared( pIThreeWayMergeResult_base ptr ) {
		if ( !ptr ) return spIThreeWayMergeResult();
		pIThreeWayMergeResult p = IThreeWayMergeResult::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< IThreeWayMergeResult >() : ptr;
		return shared_ptr< IThreeWayMergeResult >( new IThreeWayMergeResultProxy( p ) );
	}

}

#endif  // !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB
