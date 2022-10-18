#ifndef __IThreeWayResolvedConflict_h__
#define __IThreeWayResolvedConflict_h__ 1

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
	//! @brief Version1 of the interface that represents a conflict that has been resolved by applying one of the strategies.
	//! Provides all the functions to get all the information regarding the initial conflict and final resolution of the conflict.
	//! \attention: It is not thread safe and anyways since the client is provided with a constant object its not a need.
	//!
	class XMP_PUBLIC IThreeWayResolvedConflict_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:
		//!
		//! @brief An enum to indicate the source of the resolved node.
		//!
		typedef enum {
			//! no source for the resolved node, should be treated as invalid value.
			kRSNone					= 0,

			//! node from base metadata object is the one picked up as the resolved value.
			kRSBaseVersion			= 1 << 0,

			//! node from latest metadata object is the one picked up as the resolved value.
			kRSLatestVersion		= 1 << 1,

			//! node from user metadata object is the one picked up as the resolved value.
			kRSUserVersion			= 1 << 2,

			//! node contains merged fields from all the three metadata objects.
			kRSFieldsMerged			= 1 << 3,

			//! node contains custom merged fields from all the three metadata objects.
			kRSCustomMerged			= 1 << 29,

			//! node is the one defined by the strategy applied for resolving the conflict.
			kRSStrategyDefined		= 1 << 30,

			//! node is resolved because of any reason
			kRSAll					= ~( 1 << 31 ),

			//! the maximum value this enum can hold and should be treated as invalid value.
			kRSMaxValue				= 1 << 31
		} eResolvedSource;

		//!
		//! @brief An enum to indicate the reason for the resolution of the conflict.
		//!
		typedef enum {
			//! conflict is still unresolved.
			kCRRUnResolved					= 0,

			//! node in the base and user metadata object are equal.
			kCRRBaseAndUserVersionEqual		= 1 << 0,

			//! node in the base and latest metadata object are equal.
			kCRRBaseAndLatestVersionEqual	= 1 << 1,

			//! node is only present in the user metadata object.
			kCRRPresentOnlyInUserVersion	= 1 << 2,

			//! node is only present in the latest metadata object.
			kCRRPresentOnlyInLatestVersion	= 1 << 3,

			//! node is only deleted in the user metadata object.
			kCRRDeletionOnlyInUserVersion	= 1 << 4,

			//! node is only deleted in the latest metadata object.
			kCRRDeletionOnlyInLatestVersion = 1 << 5,

			//! qualifier node is only present in the user metadata object.
			kCRRQualifierPresentOnlyInUserVersion = 1 << 6,

			//! qualifier node is only present in the latest metadata object.
			kCRRQualifierPresentOnlyInLatestVersion = 1 << 7,

			//! qualifier node is only deleted in the user metadata object.
			kCRRDeletionOfQualifierOnlyInUserVersion = 1 << 8,

			//! qualifier node is only deleted in the latest metadata object.
			kCRRDeletionOfQualifierOnlyInLatestVersion = 1 << 9,

			//! node is the one defined by the strategy applied for resolving the conflict.
			kCRRStrategyResolved			= 1 << 30,

			//! node is defined by any method
			kCRRAll							= ~( 1 << 31 ),

			//! the maximum value this enum can hold and should be treated as invalid value.
			kCRRMaxValue					= 1 << 31

		} eConflictResolutionReason;

		//!
		//! @brief Get the reason for the conflict resolution.
		//! \return a value of \#eConflictResolutionReason indicating the reason for the conflict resolution.
		//!
		virtual eConflictResolutionReason APICALL GetConflictResolutionReason() const = 0;

		//!
		//! @brief Get the source of the resolved node.
		//! \return a value of \#eResolvedSource indicating the source of the resolved node.
		//!
		virtual eResolvedSource APICALL GetResolvedSource() const = 0;

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
		//! @brief Get the resolved node.
		//! \return a shared pointer to const \#AdobeXMPCore::INode interface.
		//!
		virtual spcINode APICALL GetResolvedNode() const = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Return the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return either a const or non const pointer to IThreeWayResolvedConflict interface.
		//!
		virtual pIThreeWayResolvedConflict APICALL GetActualIThreeWayResolvedConflict() __NOTHROW__ = 0;
		XMP_PRIVATE pcIThreeWayResolvedConflict GetActualIThreeWayResolvedConflict() const __NOTHROW__ {
			return const_cast< IThreeWayResolvedConflict_v1 * >( this )->GetActualIThreeWayResolvedConflict();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Return the pointer to internal interfaces.
		//! \return either a const or non const pointer to IThreeWayResolvedConflict_I interface.
		//!
		virtual AdobeXMPCompareAndMerge_Int::pIThreeWayResolvedConflict_I APICALL GetIThreeWayResolvedConflict_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPCompareAndMerge_Int::pcIThreeWayResolvedConflict_I GetIThreeWayResolvedConflict_I() const __NOTHROW__ {
			return const_cast< IThreeWayResolvedConflict_v1 * >( this )->GetIThreeWayResolvedConflict_I();
		}
		//!
		//! @}
		
		//!
		//! {@
		//! @brief Convert raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIThreeWayResolvedConflict MakeShared( pIThreeWayResolvedConflict_base ptr );
		XMP_PRIVATE static spcIThreeWayResolvedConflict MakeShared( pcIThreeWayResolvedConflict_base ptr )  {
			return MakeShared( const_cast< pIThreeWayResolvedConflict_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief return the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIThreeWayResolvedConflictID; }

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
		virtual ~IThreeWayResolvedConflict_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL getConflictResolutionReason( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual uint32 APICALL getResolvedSource( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcINode_base APICALL getNode( sizet whichNode, pcIError_base & error ) const __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __IThreeWayResolvedConflict_h__
