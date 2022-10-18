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
	class IThreeWayResolvedConflictProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCompareAndMerge::IThreeWayResolvedConflictProxy;

#include "XMPCompareAndMerge/Interfaces/IThreeWayResolvedConflict.h"

#if !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCore/Interfaces/INode.h"

#include <assert.h>

namespace AdobeXMPCompareAndMerge {

	class IThreeWayResolvedConflictProxy
		: public virtual IThreeWayResolvedConflict
	{
	private:
		pIThreeWayResolvedConflict mRawPtr;

	public:
		IThreeWayResolvedConflictProxy( pIThreeWayResolvedConflict ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IThreeWayResolvedConflictProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIThreeWayResolvedConflict APICALL GetActualIThreeWayResolvedConflict() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCompareAndMerge_Int::pIThreeWayResolvedConflict_I APICALL GetIThreeWayResolvedConflict_I() __NOTHROW__ {
			return mRawPtr->GetIThreeWayResolvedConflict_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spcINode APICALL GetBaseVersionNode() const {
			return CallConstSafeFunctionReturningPointer< IThreeWayResolvedConflict, pcINode_base, const INode, sizet >(
				mRawPtr, &IThreeWayResolvedConflict::getNode, ( sizet ) 1 );
		}

		virtual eConflictResolutionReason APICALL GetConflictResolutionReason() const {
			return CallConstSafeFunction< IThreeWayResolvedConflict, eConflictResolutionReason, uint32 >(
				mRawPtr, &IThreeWayResolvedConflict::getConflictResolutionReason );
		}

		virtual spcINode APICALL GetLatestVersionNode() const {
			return CallConstSafeFunctionReturningPointer< IThreeWayResolvedConflict, pcINode_base, const INode, sizet >(
				mRawPtr, &IThreeWayResolvedConflict::getNode, ( sizet ) 2 );
		}

		virtual spcINode APICALL GetResolvedNode() const {
			return CallConstSafeFunctionReturningPointer< IThreeWayResolvedConflict, pcINode_base, const INode, sizet >(
				mRawPtr, &IThreeWayResolvedConflict::getNode, ( sizet ) 4 );
		}

		virtual eResolvedSource APICALL GetResolvedSource() const {
			return CallConstSafeFunction< IThreeWayResolvedConflict, eResolvedSource, uint32 >(
				mRawPtr, &IThreeWayResolvedConflict::getResolvedSource );
		}

		virtual spcINode APICALL GetUserVersionNode() const {
			return CallConstSafeFunctionReturningPointer< IThreeWayResolvedConflict, pcINode_base, const INode, sizet >(
				mRawPtr, &IThreeWayResolvedConflict::getNode, ( sizet ) 3 );
		}

		virtual pcINode_base APICALL getNode( sizet whichNode, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getNode( whichNode, error );
		}

		virtual uint32 APICALL getConflictResolutionReason( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getConflictResolutionReason( error );
		}

		virtual uint32 APICALL getResolvedSource( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getResolvedSource( error );
		}

	};

	spIThreeWayResolvedConflict IThreeWayResolvedConflict_v1::MakeShared( pIThreeWayResolvedConflict_base ptr ) {
		if ( !ptr ) return spIThreeWayResolvedConflict();
		pIThreeWayResolvedConflict p = IThreeWayResolvedConflict::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< IThreeWayResolvedConflict >() : ptr;
		return shared_ptr< IThreeWayResolvedConflict >( new IThreeWayResolvedConflictProxy( p ) );
	}

}

#endif  // !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB
