// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

namespace AdobeXMPCompareAndMerge {
	class IGenericStrategyDatabaseProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPCompareAndMerge::IGenericStrategyDatabaseProxy;

#include "XMPCompareAndMerge/Interfaces/IGenericStrategyDatabase.h"

#if !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>

#include "XMPCompareAndMerge/Interfaces/IThreeWayGenericStrategy.h"
#include "XMPCompareAndMerge/Interfaces/ICompareAndMergeObjectFactory.h"

namespace AdobeXMPCompareAndMerge {

	class IGenericStrategyDatabaseProxy
		: public virtual IGenericStrategyDatabase
	{
	private:
		pIGenericStrategyDatabase mRawPtr;

	public:
		IGenericStrategyDatabaseProxy( pIGenericStrategyDatabase ptr )
			: mRawPtr( ptr )
		{
			mRawPtr->Acquire();
		}

		~IGenericStrategyDatabaseProxy() __NOTHROW__ { mRawPtr->Release(); }

		pIGenericStrategyDatabase APICALL GetActualIGenericStrategyDatabase() __NOTHROW__ { return mRawPtr; }

		void APICALL Acquire() const __NOTHROW__ { assert( false ); }

		void APICALL Release() const __NOTHROW__ { assert( false ); }

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__ {
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPCompareAndMerge_Int::pIGenericStrategyDatabase_I APICALL GetIGenericStrategyDatabase_I() __NOTHROW__ {
			return mRawPtr->GetIGenericStrategyDatabase_I();
		}

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion ) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion );
		}

		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__ {
			assert( false );
			return mRawPtr->getInterfacePointer( interfaceID, interfaceVersion, error );
		}

		virtual spIThreeWayGenericStrategy APICALL GetThreeWayGenericStrategy( eGenericStrategyType genericStrategyType ) const {
			return CallConstSafeFunctionReturningPointer< IGenericStrategyDatabase_v1, pIThreeWayGenericStrategy_base, IThreeWayGenericStrategy, uint32 >(
				mRawPtr, &IGenericStrategyDatabase::getThreeWayGenericStrategy, static_cast< uint32 >( genericStrategyType ) );
		}

		virtual pIThreeWayGenericStrategy_base APICALL getThreeWayGenericStrategy( uint32 genericStrategyType, pcIError_base & error ) const __NOTHROW__ {
			assert( false );
			return mRawPtr->getThreeWayGenericStrategy( genericStrategyType, error );
		}

	};

	spIGenericStrategyDatabase IGenericStrategyDatabase_v1::MakeShared( pIGenericStrategyDatabase_base ptr ) {
		if ( !ptr ) return spIGenericStrategyDatabase();
		pIGenericStrategyDatabase p = IGenericStrategyDatabase::GetInterfaceVersion() > 1 ?
			ptr->GetInterfacePointer< IGenericStrategyDatabase >() : ptr;
		return shared_ptr< IGenericStrategyDatabase >( new IGenericStrategyDatabaseProxy( p ) );
	}

	spcIGenericStrategyDatabase IGenericStrategyDatabase_v1::GetGenericStrategyDatabase() {
		return CallSafeFunctionReturningPointer< ICompareAndMergeObjectFactory, pcIGenericStrategyDatabase_base, const IGenericStrategyDatabase >(
			ICompareAndMergeObjectFactory::GetCompareAndMergeObjectFactory(), &ICompareAndMergeObjectFactory::GetGenericStrategyDatabase);
	}

}

#endif  // !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB
