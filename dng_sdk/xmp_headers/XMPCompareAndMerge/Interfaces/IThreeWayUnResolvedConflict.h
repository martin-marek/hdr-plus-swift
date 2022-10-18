#ifndef __IThreeWayUnResolvedConflict_h__
#define __IThreeWayUnResolvedConflict_h__ 1

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

#include "XMPCompareAndMerge/XMPCompareAndMergeFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPCompareAndMerge {
	//!
	//! @brief Version1 of the interface that represents a conflict that has not been resolved by applying any of the strategies.
	//! Provides all the functions to get all the information regarding the conflict.
	//! \attention: It is not thread safe and anyways since the client is provided with a constant object its not a need.
	//!
	class XMP_PUBLIC IThreeWayUnResolvedConflict_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:
		//!
		//! @brief An enum to indicate the reason for the conflict.
		//!
		typedef enum {
			//! conflict is resolved, the nodes in the latest and user metadata objects are equal.
			kCRResolved							= 0,

			//! Types of the node are not equal in the latest and user metadata objects.
			kCRTypesMismatch					= 1 << 0,

			//! Values of the node are not equal in the latest and user metadata objects.
			kCRValuesMismatch					= 1 << 1,

			//! Node is deleted in the latest but edited in the user metadata object.
			kCRDeletedInLatestEditedInUser		= 1 << 2,

			//! Node is deleted in the user but edited in the latest metadata object.
			kCRDeletedInUserEditedInLatest		= 1 << 3,

			//! Qualifiers on the node in the latest and user nodes are not equal.
			kCRQualifiersMismatch				= 1 << 4,

			//! URI Flag in the latest and user metadata objects are not equal
			kCRURIMismatch						= 1 << 5,

			//! one of the reason exists because of which nodes in the latest and user metadata objects are not equal.
			kCRAll								= ~(1 << 31),

			// the maximum value this enum can hold and should be treated as invalid value.
			kCRMaxValue							= 1 << 31
		} eConflictReason;

		//!
		//! @brief Get the reason for the conflict.
		//! \return a value of \#eConflictReason indicating the reason for the conflict.
		//!
		virtual eConflictReason APICALL GetConflictReason() const = 0;

		//!
		//! @brief Get the conflicting node as it is present in the base metadata object.
		//! \return a shared pointer to const \#AdobeXMPCore::INode interface.
		//!
		virtual spcINode APICALL GetBaseVersionNode() const = 0;

		//!
		//! @brief Get the conflicting node as it is present in the user metadata object.
		//! \return a shared pointer to const \#AdobeXMPCore::INode interface.
		//!
		virtual spcINode APICALL GetUserVersionNode() const = 0;

		//!
		//! @brief Get the conflicting node as it is present in the latest metadata object.
		//! \return a shared pointer to const \#AdobeXMPCore::INode interface.
		//!
		virtual spcINode APICALL GetLatestVersionNode() const = 0;
		
		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Return the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return either a const or non const pointer to IThreeWayUnResolvedConflict interface.
		//!
		virtual pIThreeWayUnResolvedConflict APICALL GetActualIThreeWayUnResolvedConflict() __NOTHROW__ = 0;
		XMP_PRIVATE pcIThreeWayUnResolvedConflict GetActualIThreeWayUnResolvedConflict() const __NOTHROW__ {
			return const_cast< IThreeWayUnResolvedConflict_v1 * >( this )->GetActualIThreeWayUnResolvedConflict();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Return the pointer to internal interfaces.
		//! \return either a const or non const pointer to IThreeWayUnResolvedConflict_I interface.
		//!
		virtual AdobeXMPCompareAndMerge_Int::pIThreeWayUnResolvedConflict_I APICALL GetIThreeWayUnResolvedConflict_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPCompareAndMerge_Int::pcIThreeWayUnResolvedConflict_I GetIThreeWayUnResolvedConflict_I() const __NOTHROW__ {
			return const_cast< IThreeWayUnResolvedConflict_v1 * >( this )->GetIThreeWayUnResolvedConflict_I();
		}
		//!
		//! @}
		
		//!
		//! {@
		//! @brief Convert raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIThreeWayUnResolvedConflict MakeShared( pIThreeWayUnResolvedConflict_base ptr );
		XMP_PRIVATE static spcIThreeWayUnResolvedConflict MakeShared( pcIThreeWayUnResolvedConflict_base ptr )  {
			return MakeShared( const_cast< pIThreeWayUnResolvedConflict_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief return the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIThreeWayUnResolvedConflictID; }

		//!
		//! @brief return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		// static factory functions

	protected:
		//!
		//! Destructor
		//! 
		virtual ~IThreeWayUnResolvedConflict_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL getConflictReason( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcINode_base APICALL getNode( sizet whichNode, pcIError_base & error ) const __NOTHROW__ = 0;


		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __IThreeWayUnResolvedConflict_h__
