#ifndef __IThreeWayStrategy_h__
#define __IThreeWayStrategy_h__ 1

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
#include "XMPCompareAndMerge/Interfaces/IThreeWayUnResolvedConflict.h"
#include "XMPCompareAndMerge/Interfaces/IThreeWayResolvedConflict.h"

#include "XMPCore/Interfaces/INode.h"

namespace AdobeXMPCompareAndMerge {

	//!
	//! @brief Version1 of the interface that allows clients to resolve conflicts during 3 way merge operation.
	//! Provides all the functions
	//!		- show inclination for resolving the conflict
	//!		- try to solve the conflict
	//!
	class XMP_PUBLIC IThreeWayStrategy_v1
	{
	public:
		//!
		//! @brief Let the library know that the strategy object is interested in resolving such a conflict.
		//! \param[in] nodePath a shared pointer to const IUTF8String object containing the path of the node.
		//! \param[in] nodeType a value of type eNodeType indicating the type of the node.
		//! \param[in] reason a value of type eConflictReason indicating the reason for the conflict at first place.
		//! \param[in] alreadyResolved a bool value indicating that whether some other strategy higher in the order
		//! has been able to resolve the conflict.
		//! \return a boolean value; true in case strategy is interested in resolving the conflict, otherwise return false.
		//!
		virtual bool APICALL InterestedInResolvingConflict( const spcIUTF8String & nodePath, INode::eNodeType nodeType,
			IThreeWayUnResolvedConflict::eConflictReason reason, bool alreadyResolved ) = 0;

		//!
		//! @brief Library calls this function to give chance to library to resolve the conflict on which strategy object has shown interest in.
		//! \param[in] conflictReason a value of type eConflictReason indicating the reason(s) for the conflict at the first place.
		//! \param[in] baseVersionNode a shared pointer to const INode object containing the node from the base metadata object for which conflict exists.
		//! \param[in] latestVersionNode a shared pointer to const INode object containing the node from the latest metadata object for which conflict exists.
		//! \param[in] userVersionNode a shared pointer to const INode object containing the node from the user metadata object for which conflict exists.
		//! \param[out] resolvedNode a shared pointer to INode object which should be filled by strategy object in case it is able to resolve the conflict.
		//! Fill it will invalid node in case it is not able to resolve the conflict or strategy wants to remove this node from the merged result.
		//! \param[out] conflictResolutionReason a value of type eConflictResolutionReason to be filled appropriately by the strategy object in case it is
		//! able to resolve the conflict. In case client is note able to resolve the conflict, it is advisable to set it to kCRRUnResolved.
		//! \param[out] resolvedSource a value of type eResolvedSource to be filled appropriately by the strategy object in case it is
		//! able to resolve the conflict. In case client is note able to resolve the conflict, it is advisable to set it to kRSMaxValue.
		//! \return a value of type eConflictReason; set it to kCRResolved in case strategy is able to resolve the conflict.
		//!
		virtual IThreeWayUnResolvedConflict::eConflictReason APICALL ResolveConflict( IThreeWayUnResolvedConflict::eConflictReason conflictReason, const spcINode & baseVersionNode,
			const spcINode & latestVersionNode, const spcINode & userVersionNode, spcINode & resolvedNode,
			IThreeWayResolvedConflict::eConflictResolutionReason & conflictResolutionReason, IThreeWayResolvedConflict::eResolvedSource & resolvedSource ) = 0;

	protected:
		//!
		//! @brief Destructor
		//! 
		virtual ~IThreeWayStrategy_v1() {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 interestedInResolvingConflict( const pcIUTF8String_base & nodePath, uint32 nodeType, uint32 reason, uint32 alreadyResolved, pcIError_base & error, uint32 & unknownExceptionCaught ) __NOTHROW__ ;
		virtual uint32 resolveConflict( uint32 conflictReason, pcINode_base baseVersionNode, pcINode_base latestVersionNode, pcINode_base userVersionNode,
			pcINode & resolvedNode, uint32 & conflictResolutionReason, uint32 & resolvedSource, pcIError_base & error, uint32 & unknownExceptionCaught ) __NOTHROW__;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __IThreeWayStrategy_h__
