#ifndef IThreeWayGenericStrategy_h__
#define IThreeWayGenericStrategy_h__ 1

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
	//! @brief Version1 of the interface that allows clients to use generic purpose 3 way conflict resolution strategy and bind it
	//! with specific node paths.
	//! Provides all the functions
	//!		- to bind a path to a list of paths
	//!		- to get the list of bound paths.
	//!		- to remove a path.
	//!
	class XMP_PUBLIC IThreeWayGenericStrategy_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:

		//!
		//! @brief Append a particular path to the list of paths bound with the strategy.
		//! \param[in] path a pointer to const char buffer containing the path of the node. The path must contain the long form of the path using
		//! the full namespace strings and not the prefixes.
		//! \param[in] pathSize number of characters in value. In case path is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return a boolean value; true in case of success false otherwise.
		//!
		virtual bool APICALL AppendPath( const char * path, sizet pathSize ) = 0;

		//!
		//! @brief Get the count of paths bound with the strategy.
		//! \return a value of size_t indicating the count of paths bound with the strategy.
		//!
		virtual sizet APICALL PathCount() const __NOTHROW__ = 0;

		//!
		//! @brief Get the path at a particular index in the list of paths bound with the strategy.
		//! \param[in] index a value indicating the index in the list of paths bounds with the strategy which of interest to client.
		//! \return a shared pointer to a const IUTF8String object indicating the path.
		//! \attention Error is thrown in case
		//!		- index is greater than or equal to count of paths in the list.
		//!
		virtual spcIUTF8String APICALL GetPath( sizet index ) const = 0;

		//!
		//! @brief Get all the paths bounded with the strategy.
		//! \return a shared pointer to a vector of spcIUTF8String objects, each containing a path bound with the strategy.
		//!
		virtual const spcIUTF8Strings_const APICALL GetPaths() const = 0;

		//!
		//! @brief Remove a path from the list of paths bounded with the strategy.
		//! \param[in] path a pointer to const char buffer containing the path of the node.
		//! \param[in] pathSize number of characters in value. In case path is null terminated set it to \#AdobeXMPCommon::npos.
		//! \return a shared pointer to a IUTF8String object indicating the path removed from the list of paths bound with the strategy.
		//!
		virtual spIUTF8String APICALL RemovePath( const char * path, sizet pathSize ) = 0;


		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Return the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return either a const or non const pointer to IThreeWayGenericStrategy interface.
		//!
		virtual pIThreeWayGenericStrategy APICALL GetActualIThreeWayGenericStrategy() __NOTHROW__ = 0;
		XMP_PRIVATE pcIThreeWayGenericStrategy GetActualIThreeWayGenericStrategy() const __NOTHROW__ {
			return const_cast< IThreeWayGenericStrategy_v1 * >( this )->GetActualIThreeWayGenericStrategy();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Return the pointer to internal interfaces.
		//! \return either a const or non const pointer to IThreeWayGenericStrategy_I interface.
		//!
		virtual AdobeXMPCompareAndMerge_Int::pIThreeWayGenericStrategy_I APICALL GetIThreeWayGenericStrategy_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPCompareAndMerge_Int::pcIThreeWayGenericStrategy_I GetIThreeWayGenericStrategy_I() const __NOTHROW__ {
			return const_cast< IThreeWayGenericStrategy_v1 * >( this )->GetIThreeWayGenericStrategy_I();
		}
		//!
		//! @}
		
		//!
		//! {@
		//! @brief Convert raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIThreeWayGenericStrategy MakeShared( pIThreeWayGenericStrategy_base ptr );
		XMP_PRIVATE static spcIThreeWayGenericStrategy MakeShared( pcIThreeWayGenericStrategy_base ptr )  {
			return MakeShared( const_cast< pIThreeWayGenericStrategy_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief return the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIThreeWayGenericStrategyID; }

		//!
		//! @brief return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

	protected:
		//!
		//! @brief Destructor
		//! 
		virtual ~IThreeWayGenericStrategy_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL appendPath( const char * path, sizet pathSize, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getPath( sizet index, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pIUTF8String_base APICALL removePath( const char * path, sizet pathSize, pcIError_base & error ) __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // IThreeWayGenericStrategy_h__
