#ifndef __XMPCompareAndMergeFwdDeclarations_h__
#define __XMPCompareAndMergeFwdDeclarations_h__ 1

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


#include "XMPCompareAndMerge/XMPCompareAndMergeDefines.h"
#include "XMPCommon/XMPCommonFwdDeclarations.h"
#include "XMPCompareAndMerge/XMPCompareAndMergeLatestInterfaceVersions.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"

namespace AdobeXMPCompareAndMerge {
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCore;

	// IThreeWayResolvedConflict
	class IThreeWayResolvedConflict_v1;
	typedef IThreeWayResolvedConflict_v1													IThreeWayResolvedConflict_base;
	typedef IThreeWayResolvedConflict_v1 *													pIThreeWayResolvedConflict_base;
	typedef const IThreeWayResolvedConflict_v1 *											pcIThreeWayResolvedConflict_base;
	typedef BASE_CLASS( IThreeWayResolvedConflict, ITHREEWAYRESOLVEDCONFLICT_VERSION )		IThreeWayResolvedConflict;
	typedef IThreeWayResolvedConflict *														pIThreeWayResolvedConflict;
	typedef const IThreeWayResolvedConflict *												pcIThreeWayResolvedConflict;
	typedef shared_ptr< IThreeWayResolvedConflict >											spIThreeWayResolvedConflict;
	typedef shared_ptr< const IThreeWayResolvedConflict >									spcIThreeWayResolvedConflict;
	static const uint64 kIThreeWayResolvedConflictID										( 0x6d33775243666c74 /* m3wRCflt */ );

	// IThreeWayUnResolvedConflict
	class IThreeWayUnResolvedConflict_v1;
	typedef IThreeWayUnResolvedConflict_v1													IThreeWayUnResolvedConflict_base;
	typedef IThreeWayUnResolvedConflict_v1 *												pIThreeWayUnResolvedConflict_base;
	typedef const IThreeWayUnResolvedConflict_v1 *											pcIThreeWayUnResolvedConflict_base;
	typedef BASE_CLASS( IThreeWayUnResolvedConflict, ITHREEWAYUNRESOLVEDCONFLICT_VERSION )	IThreeWayUnResolvedConflict;
	typedef IThreeWayUnResolvedConflict *													pIThreeWayUnResolvedConflict;
	typedef const IThreeWayUnResolvedConflict *												pcIThreeWayUnResolvedConflict;
	typedef shared_ptr< IThreeWayUnResolvedConflict >										spIThreeWayUnResolvedConflict;
	typedef shared_ptr< const IThreeWayUnResolvedConflict >									spcIThreeWayUnResolvedConflict;
	static const uint64 kIThreeWayUnResolvedConflictID										( 0x6d33775543666c74 /* m3wUCflt */ );

	// IThreeWayMergeResult
	class IThreeWayMergeResult_v1;
	typedef IThreeWayMergeResult_v1															IThreeWayMergeResult_base;
	typedef IThreeWayMergeResult_v1 *														pIThreeWayMergeResult_base;
	typedef const IThreeWayMergeResult_v1 *													pcIThreeWayMergeResult_base;
	typedef BASE_CLASS( IThreeWayMergeResult, ITHREEWAYMERGERESULT_VERSION )				IThreeWayMergeResult;
	typedef IThreeWayMergeResult *															pIThreeWayMergeResult;
	typedef const IThreeWayMergeResult *													pcIThreeWayMergeResult;
	typedef shared_ptr< IThreeWayMergeResult >												spIThreeWayMergeResult;
	typedef shared_ptr< const IThreeWayMergeResult >										spcIThreeWayMergeResult;
	static const uint64 kIThreeWayMergeResultID												( 0x6d337752736c7420 /* m3wRslt  */ );

	// IThreeWayStrategy
	class IThreeWayStrategy_v1;
	typedef IThreeWayStrategy_v1															IThreeWayStrategy_base;
	typedef IThreeWayStrategy_v1 *															pIThreeWayStrategy_base;
	typedef const IThreeWayStrategy_v1 *													pcIThreeWayStrategy_base;
	typedef BASE_CLASS( IThreeWayStrategy, ITHREEWAYSTRATEGY_VERSION )						IThreeWayStrategy;
	typedef IThreeWayStrategy *																pIThreeWayStrategy;
	typedef const IThreeWayStrategy *														pcIThreeWayStrategy;

	// IThreeWayGenericStrategy
	class IThreeWayGenericStrategy_v1;
	typedef IThreeWayGenericStrategy_v1														IThreeWayGenericStrategy_base;
	typedef IThreeWayGenericStrategy_v1 *													pIThreeWayGenericStrategy_base;
	typedef const IThreeWayGenericStrategy_v1 *												pcIThreeWayGenericStrategy_base;
	typedef BASE_CLASS( IThreeWayGenericStrategy, ITHREEWAYGENERICSTRATEGY_VERSION )		IThreeWayGenericStrategy;
	typedef IThreeWayGenericStrategy *														pIThreeWayGenericStrategy;
	typedef const IThreeWayGenericStrategy *												pcIThreeWayGenericStrategy;
	typedef shared_ptr< IThreeWayGenericStrategy >											spIThreeWayGenericStrategy;
	typedef shared_ptr< const IThreeWayGenericStrategy >									spcIThreeWayGenericStrategy;
	static const uint64 kIThreeWayGenericStrategyID											( 0x6d33774753746779 /* m3wGStgy */ );

	// IGenericStrategyDatabase
	class IGenericStrategyDatabase_v1;
	typedef IGenericStrategyDatabase_v1														IGenericStrategyDatabase_base;
	typedef IGenericStrategyDatabase_v1 *													pIGenericStrategyDatabase_base;
	typedef const IGenericStrategyDatabase_v1 *												pcIGenericStrategyDatabase_base;
	typedef BASE_CLASS( IGenericStrategyDatabase, IGENERICSTRATEGYDATABASE_VERSION )		IGenericStrategyDatabase;
	typedef IGenericStrategyDatabase *														pIGenericStrategyDatabase;
	typedef const IGenericStrategyDatabase *												pcIGenericStrategyDatabase;
	typedef shared_ptr< IGenericStrategyDatabase >											spIGenericStrategyDatabase;
	typedef shared_ptr< const IGenericStrategyDatabase >									spcIGenericStrategyDatabase;
	static const uint64 kIGenericStrategyDatabaseID											( 0x6d47537467794462 /* mGStgyDb */ );

	// IThreeWayMerge
	class IThreeWayMerge_v1;
	typedef IThreeWayMerge_v1																IThreeWayMerge_base;
	typedef IThreeWayMerge_v1 *																pIThreeWayMerge_base;
	typedef const IThreeWayMerge_v1 *														pcIThreeWayMerge_base;
	typedef BASE_CLASS( IThreeWayMerge, ITHREEWAYMERGE_VERSION )							IThreeWayMerge;
	typedef IThreeWayMerge *																pIThreeWayMerge;
	typedef const IThreeWayMerge *															pcIThreeWayMerge;
	typedef shared_ptr< IThreeWayMerge >													spIThreeWayMerge;
	typedef shared_ptr< const IThreeWayMerge >												spcIThreeWayMerge;
	static const uint64 kIThreeWayMergeID													( 0x6d33774d65726765 /* m3wMerge */ );

	// ICompareAndMergeObjectFactory
	class ICompareAndMergeObjectFactory_v1;
	typedef ICompareAndMergeObjectFactory_v1												ICompareAndMergeObjectFactory_base;
	typedef ICompareAndMergeObjectFactory_v1 *												pICompareAndMergeObjectFactory_base;
	typedef const ICompareAndMergeObjectFactory_v1 *										pcICompareAndMergeObjectFactory_base;
	typedef BASE_CLASS( ICompareAndMergeObjectFactory, ICOMPAREANDMERGEOBJFACTORY_VERSION )	ICompareAndMergeObjectFactory;
	typedef ICompareAndMergeObjectFactory *													pICompareAndMergeObjectFactory;
	typedef const ICompareAndMergeObjectFactory *											pcICompareAndMergeObjectFactory;
	typedef shared_ptr< ICompareAndMergeObjectFactory >										spICompareAndMergeObjectFactory;
	typedef shared_ptr< const ICompareAndMergeObjectFactory >								spcICompareAndMergeObjectFactory;
	static const uint64 kICompareAndMergeObjectFactoryID									( 0x6d4f626a46616374 /* mObjFact */ );

	// typedefs for vectors and their corresponding shared pointers.
	typedef std::vector< spIThreeWayResolvedConflict >									IThreeWayResolvedConflicts;
	typedef shared_ptr< IThreeWayResolvedConflicts >									spIThreeWayResolvedConflicts;
	typedef shared_ptr< const IThreeWayResolvedConflicts >								spIThreeWayResolvedConflicts_const;

	typedef std::vector< spcIThreeWayResolvedConflict >									cIThreeWayResolvedConflicts;
	typedef shared_ptr< cIThreeWayResolvedConflicts >									spcIThreeWayResolvedConflicts;
	typedef shared_ptr< const cIThreeWayResolvedConflicts >								spcIThreeWayResolvedConflicts_const;

	typedef std::vector< spIThreeWayUnResolvedConflict >								IThreeWayUnResolvedConflicts;
	typedef shared_ptr< IThreeWayUnResolvedConflicts >									spIThreeWayUnResolvedConflicts;
	typedef shared_ptr< const IThreeWayUnResolvedConflicts >							spIThreeWayUnResolvedConflicts_const;

	typedef std::vector< spcIThreeWayUnResolvedConflict >								cIThreeWayUnResolvedConflicts;
	typedef shared_ptr< cIThreeWayUnResolvedConflicts >									spcIThreeWayUnResolvedConflicts;
	typedef shared_ptr< const cIThreeWayUnResolvedConflicts >							spcIThreeWayUnResolvedConflicts_const;
}

namespace AdobeXMPCompareAndMerge_Int {

	// IThreeWayResolvedConflict_I
	class IThreeWayResolvedConflict_I;
	typedef IThreeWayResolvedConflict_I *													pIThreeWayResolvedConflict_I;
	typedef const IThreeWayResolvedConflict_I *												pcIThreeWayResolvedConflict_I;

	// IThreeWayUnResolvedConflict_I
	class IThreeWayUnResolvedConflict_I;
	typedef IThreeWayUnResolvedConflict_I *													pIThreeWayUnResolvedConflict_I;
	typedef const IThreeWayUnResolvedConflict_I *											pcIThreeWayUnResolvedConflict_I;

	// IThreeWayMergeResult_I
	class IThreeWayMergeResult_I;
	typedef IThreeWayMergeResult_I *														pIThreeWayMergeResult_I;
	typedef const IThreeWayMergeResult_I *													pcIThreeWayMergeResult_I;

	// IThreeWayGenericStrategy_I
	class IThreeWayGenericStrategy_I;
	typedef IThreeWayGenericStrategy_I *													pIThreeWayGenericStrategy_I;
	typedef const IThreeWayGenericStrategy_I *												pcIThreeWayGenericStrategy_I;

	// IGenericStrategyDatabase_I
	class IGenericStrategyDatabase_I;
	typedef IGenericStrategyDatabase_I *													pIGenericStrategyDatabase_I;
	typedef const IGenericStrategyDatabase_I *												pcIGenericStrategyDatabase_I;

	// IThreeWayMerge_I
	class IThreeWayMerge_I;
	typedef IThreeWayMerge_I *																pIThreeWayMerge_I;
	typedef const IThreeWayMerge_I *														pcIThreeWayMerge_I;

	// ICompareAndMergeObjectFactory_I
	class ICompareAndMergeObjectFactory_I;
	typedef ICompareAndMergeObjectFactory_I *												pICompareAndMergeObjectFactory_I;
	typedef const ICompareAndMergeObjectFactory_I *											pcICompareAndMergeObjectFactory_I;
}
#endif  // __XMPCompareAndMergeFwdDeclarations_h__
