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
	class IThreeWayUnResolvedConflictProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCompareAndMerge::IThreeWayUnResolvedConflictProxy;

#include "XMPCompareAndMerge/Interfaces/IThreeWayUnResolvedConflict.h"

#if !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCore/Interfaces/INode.h"
#include <assert.h>

namespace AdobeXMPCompareAndMerge {

	class IThreeWayUnResolvedConflictProxy
		: public virtual IThreeWayUnResolvedConflict
	{
	private:
		pIThreeWayUnResolvedConflict mRawPtr;

	public:
		IThreeWayUnResolvedConflictProxy( pIThreeWayUnResolvedConflict ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IThreeWayUnResolvedConflictProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIThreeWayUnResolvedConflict APICALL GetActualIThreeWayUnResolvedConflict() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCompareAndMerge_Int::pIThreeWayUnResolvedConflict_I APICALL GetIThreeWayUnResolvedConflict_I() __NOTHROW__ {
			return mRawPtr->GetIThreeWayUnResolvedConflict_I();
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
			return CallConstSafeFunctionReturningPointer< IThreeWayUnResolvedConflict, pcINode_base, const INode, sizet >(
				mRawPtr, &IThreeWayUnResolvedConflict::getNode, ( sizet ) 1 );
		}

		virtual eConflictReason APICALL GetConflictReason() const {
			return CallConstSafeFunction < IThreeWayUnResolvedConflict, eConflictReason, uint32 >(
				mRawPtr, &IThreeWayUnResolvedConflict::getConflictReason );
		}

		virtual spcINode APICALL GetLatestVersionNode() const {
			return CallConstSafeFunctionReturningPointer< IThreeWayUnResolvedConflict, pcINode_base, const INode, sizet >(
				mRawPtr, &IThreeWayUnResolvedConflict::getNode, ( sizet ) 2 );
		}

		virtual spcINode APICALL GetUserVersionNode() const {
			return CallConstSafeFunctionReturningPointer< IThreeWayUnResolvedConflict, pcINode_base, const INode, sizet >(
				mRawPtr, &IThreeWayUnResolvedConflict::getNode, ( sizet ) 3 );
		}

		virtual uint32 APICALL getConflictReason( pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getConflictReason( error );
		}

		virtual pcINode_base APICALL getNode( sizet whichNode, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getNode( whichNode, error );
		}

	};

	spIThreeWayUnResolvedConflict IThreeWayUnResolvedConflict_v1::MakeShared( pIThreeWayUnResolvedConflict_base ptr ) {
		if ( !ptr ) return spIThreeWayUnResolvedConflict();
		pIThreeWayUnResolvedConflict p = IThreeWayUnResolvedConflict::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< IThreeWayUnResolvedConflict >() : ptr;
		return shared_ptr< IThreeWayUnResolvedConflict >( new IThreeWayUnResolvedConflictProxy( p ) );
	}

}

#endif  // !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB
