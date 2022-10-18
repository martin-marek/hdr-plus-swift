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
	class IThreeWayGenericStrategyProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCompareAndMerge::IThreeWayGenericStrategyProxy;

#include "XMPCompareAndMerge/Interfaces/IThreeWayGenericStrategy.h"

#if !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>

#include "XMPCommon/Interfaces/IUTF8String.h"

namespace AdobeXMPCompareAndMerge {

	class IThreeWayGenericStrategyProxy
		: public virtual IThreeWayGenericStrategy
	{
	private:
		pIThreeWayGenericStrategy mRawPtr;

	public:
		IThreeWayGenericStrategyProxy( pIThreeWayGenericStrategy ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IThreeWayGenericStrategyProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIThreeWayGenericStrategy APICALL GetActualIThreeWayGenericStrategy() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCompareAndMerge_Int::pIThreeWayGenericStrategy_I APICALL GetIThreeWayGenericStrategy_I() __NOTHROW__ {
			return mRawPtr->GetIThreeWayGenericStrategy_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual bool APICALL AppendPath( const char * path, sizet pathSize ) {
			return CallSafeFunction< IThreeWayGenericStrategy_v1, bool, uint32, const char *, sizet >(
				mRawPtr, &IThreeWayGenericStrategy_v1::appendPath, path, pathSize );
		}

		virtual spcIUTF8String APICALL GetPath( sizet index ) const {
			return CallConstSafeFunctionReturningPointer< IThreeWayGenericStrategy_v1, pcIUTF8String_base, const IUTF8String, sizet >(
				mRawPtr, &IThreeWayGenericStrategy_v1::getPath, index );
		}

		virtual const spcIUTF8Strings_const APICALL GetPaths() const {
			sizet nCount = PathCount();
			spcIUTF8Strings paths = shared_ptr< cIUTF8Strings >( new cIUTF8Strings( nCount ) );
			paths->reserve( nCount );
			for ( sizet i = 0; i < nCount; ++i ) {
				paths->at( i ) = GetPath( i );
			}
			return paths;
		}

		virtual sizet APICALL PathCount() const __NOTHROW__ {
			return mRawPtr->PathCount(  );
		}

		virtual spIUTF8String APICALL RemovePath( const char * path, sizet pathSize ) {
			return CallSafeFunctionReturningPointer< IThreeWayGenericStrategy_v1, pIUTF8String_base, IUTF8String, const char *, sizet >(
				mRawPtr, &IThreeWayGenericStrategy_v1::removePath, path, pathSize );
		}

		virtual uint32 APICALL appendPath( const char * path, sizet pathSize, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->appendPath( path, pathSize, error );
		}

		virtual pcIUTF8String_base APICALL getPath( sizet index, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getPath( index, error );
		}

		virtual pIUTF8String_base APICALL removePath( const char * path, sizet pathSize, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->removePath( path, pathSize, error );
		}

	};

	spIThreeWayGenericStrategy IThreeWayGenericStrategy_v1::MakeShared( pIThreeWayGenericStrategy_base ptr ) {
		if ( !ptr ) return spIThreeWayGenericStrategy();
		pIThreeWayGenericStrategy p = IThreeWayGenericStrategy::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< IThreeWayGenericStrategy >() : ptr;
		return shared_ptr< IThreeWayGenericStrategy >( new IThreeWayGenericStrategyProxy( p ) );
	}

}

#endif  // !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB
