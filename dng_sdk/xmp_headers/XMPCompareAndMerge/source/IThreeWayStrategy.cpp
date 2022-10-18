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

#include "XMPCompareAndMerge/Interfaces/IThreeWayStrategy.h"
#include "XMPCore/Interfaces/INode.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPCommon/Interfaces/IError.h"

namespace AdobeXMPCompareAndMerge {

	AdobeXMPCommon::uint32 IThreeWayStrategy_v1::interestedInResolvingConflict( const pcIUTF8String_base & nodePath,
		uint32 nodeType, uint32 reason, uint32 alreadyResolved, pcIError_base & error, uint32 & unknownExceptionCaught ) __NOTHROW__
	{
		unknownExceptionCaught = 0;
		error = NULL;
		try {
			auto returnValue = InterestedInResolvingConflict( IUTF8String::MakeShared( nodePath ), static_cast< INode::eNodeType >( nodeType ),
				static_cast< IThreeWayUnResolvedConflict::eConflictReason >( reason ), alreadyResolved != 0 );
			return returnValue ? 1 : 0;
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->Acquire();
		} catch ( ... ) {
			unknownExceptionCaught = 1;
		}
		return 0;
	}

	AdobeXMPCommon::uint32 IThreeWayStrategy_v1::resolveConflict( uint32 conflictReason, pcINode_base baseVersionNode, pcINode_base latestVersionNode,
		pcINode_base userVersionNode, pcINode & resolvedNode, uint32 & conflictResolutionReason, uint32 & resolvedSource,
		pcIError_base & error, uint32 & unknownExceptionCaught ) __NOTHROW__
	{
		unknownExceptionCaught = 0;
		error = NULL;
		try {
			spcINode _ResolvedNode;
			IThreeWayResolvedConflict::eConflictResolutionReason _ConflictResolutionReason( IThreeWayResolvedConflict::kCRRUnResolved );
			IThreeWayResolvedConflict::eResolvedSource _ResolvedSource( IThreeWayResolvedConflict::kRSMaxValue );
			auto returnValue = ResolveConflict( static_cast< IThreeWayUnResolvedConflict::eConflictReason >( conflictReason ), INode::MakeShared( baseVersionNode ),
				INode::MakeShared( latestVersionNode ), INode::MakeShared( userVersionNode ), _ResolvedNode, _ConflictResolutionReason, _ResolvedSource );
			if ( _ResolvedNode ) {
				_ResolvedNode->GetActualINode()->Acquire();
				resolvedNode = _ResolvedNode->GetActualINode();
			} else {
				resolvedNode = NULL;
			}
			conflictResolutionReason = static_cast< uint32 >( _ConflictResolutionReason );
			resolvedSource = static_cast< uint32 >( _ResolvedSource );
			return static_cast< uint32 >( returnValue );
		} catch ( spcIError err ) {
			error = err->GetActualIError();
			error->Acquire();
		} catch ( ... ) {
			unknownExceptionCaught = 1;
		}
		return static_cast< uint32 >( IThreeWayUnResolvedConflict::kCRMaxValue );
	}

}
