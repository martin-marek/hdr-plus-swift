#ifndef __IGenericStrategyDatabase_h__
#define __IGenericStrategyDatabase_h__ 1

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

#include "XMPCompareAndMerge/XMPCompareAndMergeFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPCompareAndMerge {
	//!
	//! @brief Version1 of the interface that represents a database of generic strategies.
	//! Provides functions to 
	//!		- Retrieve generic strategy objects from the database.
	//!
	class XMP_PUBLIC IGenericStrategyDatabase_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:

		//!
		//! @brief Indicates various types of generic strategies available in the library.
		//!
		typedef enum {
			//! a generic strategy that picks the latest date amongst all the available sources.
			kGSTLatestDate							= 0,

			//! a generic strategy that picks the oldest data amongst all the available sources.
			kGSTOldestDate							= 1,

			//!a generic strategy that resolves History conflicts.
			kGSTHistory								= 2,

			//!a generic strategy that resolves Marker conflicts.
			kGSTMarker								= 3,

			//! the maximum value this enum can hold, should be treated as invalid value.
			kGSTMaxValue							= kMaxEnumValue
		} eGenericStrategyType;

		//!
		//! @brief Get the generic strategy from the database.
		//! \param[in] genericStrategyType a value indicating the generic strategy type client is interested in.
		//! \return a shared pointer to an object of IThreeWayGenericStrategy, if it is available with the database
		//! \attention Error is thrown in case
		//!		- strategy indicated by the genericStrategyType is not available or not valid.
		//!
		virtual spIThreeWayGenericStrategy APICALL GetThreeWayGenericStrategy( eGenericStrategyType genericStrategyType ) const = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Return the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return either a const or non const pointer to IGenericStrategyDatabase interface.
		//!
		virtual pIGenericStrategyDatabase APICALL GetActualIGenericStrategyDatabase() __NOTHROW__ = 0;
		XMP_PRIVATE pcIGenericStrategyDatabase GetActualIGenericStrategyDatabase() const __NOTHROW__ {
			return const_cast< IGenericStrategyDatabase_v1 * >( this )->GetActualIGenericStrategyDatabase();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Return the pointer to internal interfaces.
		//! \return either a const or non const pointer to IGenericStrategyDatabase_I interface.
		//!
		virtual AdobeXMPCompareAndMerge_Int::pIGenericStrategyDatabase_I APICALL GetIGenericStrategyDatabase_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPCompareAndMerge_Int::pcIGenericStrategyDatabase_I GetIGenericStrategyDatabase_I() const __NOTHROW__ {
			return const_cast< IGenericStrategyDatabase_v1 * >( this )->GetIGenericStrategyDatabase_I();
		}
		//!
		//! @}
		
		//!
		//! {@
		//! @brief Convert raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIGenericStrategyDatabase MakeShared( pIGenericStrategyDatabase_base ptr );
		XMP_PRIVATE static spcIGenericStrategyDatabase MakeShared( pcIGenericStrategyDatabase_base ptr )  {
			return MakeShared( const_cast< pIGenericStrategyDatabase_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief return the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIGenericStrategyDatabaseID; }

		//!
		//! @brief return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		//!
		//! @brief Provides the reference to the database of Generic strategies available with the library.
		//! \return a shared pointer to const \#IGenericStrategyDatabase object containing all the entries for generic strategies.
		//!
		XMP_PRIVATE static spcIGenericStrategyDatabase GetGenericStrategyDatabase();

	protected:
		//!
		//! @brief Destructor
		//! 
		virtual ~IGenericStrategyDatabase_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pIThreeWayGenericStrategy_base APICALL getThreeWayGenericStrategy( uint32 genericStrategyType, pcIError_base & error ) const __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __IGenericStrategyDatabase_h__
