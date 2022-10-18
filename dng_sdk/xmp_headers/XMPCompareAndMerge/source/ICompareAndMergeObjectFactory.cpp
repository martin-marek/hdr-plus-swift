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

#include "XMPCompareAndMerge/Interfaces/ICompareAndMergeObjectFactory.h"

#if !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include <assert.h>

#if LINKING_XMPCOMPAREANDMERGE_LIB
	#include "XMPCompareAndMerge/XMPCompareAndMerge.h"
#endif

namespace AdobeXMPCompareAndMerge {

	pICompareAndMergeObjectFactory ICompareAndMergeObjectFactory_v1::MakeCompareAndMergeObjectFactory( pICompareAndMergeObjectFactory_base ptr ) {
		if ( ICompareAndMergeObjectFactory::GetInterfaceVersion() == 1 )
			return ptr;
		else
			return ptr->GetInterfacePointer< ICompareAndMergeObjectFactory >();
	}

#if LINKING_XMPCOMPAREANDMERGE_LIB
	static pICompareAndMergeObjectFactory ManageCompareAndMergeObjectFactory( bool destroy = false ) {
		static pICompareAndMergeObjectFactory sCompareAndMergeObjectFactoryPtr( NULL );
		if ( destroy && sCompareAndMergeObjectFactoryPtr ) {
			sCompareAndMergeObjectFactoryPtr = NULL;
			return sCompareAndMergeObjectFactoryPtr;
		}

		if ( !sCompareAndMergeObjectFactoryPtr ) {
			if ( ICompareAndMergeObjectFactory::GetInterfaceVersion() != 1 )
				sCompareAndMergeObjectFactoryPtr = XMPCM_GetObjectFactoryInstance()->GetInterfacePointer< ICompareAndMergeObjectFactory >();
			else
				sCompareAndMergeObjectFactoryPtr = XMPCM_GetObjectFactoryInstance();
		}
		return sCompareAndMergeObjectFactoryPtr;
	}


	void ICompareAndMergeObjectFactory_v1::SetupCompareAndMergeObjectFactory() {
		ManageCompareAndMergeObjectFactory();
	}
#else
	static pICompareAndMergeObjectFactory ManageCompareAndMergeObjectFactory( bool destroy = false, pICompareAndMergeObjectFactory_base compareAndMergeObjectFactory = NULL ) {
		static pICompareAndMergeObjectFactory sCompareAndMergeObjectFactoryPtr( NULL );
		if ( destroy && sCompareAndMergeObjectFactoryPtr ) {
			sCompareAndMergeObjectFactoryPtr = NULL;
			return sCompareAndMergeObjectFactoryPtr;
		}

		if ( !sCompareAndMergeObjectFactoryPtr && compareAndMergeObjectFactory ) {
			if ( ICompareAndMergeObjectFactory::GetInterfaceVersion() != 1 )
				sCompareAndMergeObjectFactoryPtr = compareAndMergeObjectFactory->GetInterfacePointer< ICompareAndMergeObjectFactory >();
			else
				sCompareAndMergeObjectFactoryPtr = compareAndMergeObjectFactory;
		}
		return sCompareAndMergeObjectFactoryPtr;
	}

	void ICompareAndMergeObjectFactory_v1::SetupCompareAndMergeObjectFactory( pICompareAndMergeObjectFactory_base compareAndMergeObjectFactory ) {
		ManageCompareAndMergeObjectFactory( false, compareAndMergeObjectFactory );
	}
#endif

	pICompareAndMergeObjectFactory ICompareAndMergeObjectFactory_v1::GetCompareAndMergeObjectFactory() {
		return ManageCompareAndMergeObjectFactory();
	}

	void ICompareAndMergeObjectFactory_v1::DestroyCompareAndMergeObjectFactory() {
		ManageCompareAndMergeObjectFactory( true );
	}
}

#endif  // !BUILDING_XMPCOMPAREANDMERGE_LIB && !SOURCE_COMPILING_XMPCOMPAREANDMERGE_LIB
