#ifndef __IThreeWayMergeResult_h__
#define __IThreeWayMergeResult_h__ 1

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
	//! @brief Version1 of the interface that represents the result of the 3 way merge.
	//! Provides all the functions to
	//!		- get the status of the merge.
	//!		- get information on all the resolved conflicts
	//!		- get information on all the unresolved conflicts.
	//!		- get the merged metadata object, if all conflicts have been resolved.
	//!
	class XMP_PUBLIC IThreeWayMergeResult_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:
		//!
		//! @brief An enum which represents the status of the 3 way merge operation.
		//!
		typedef enum {
			//! Indicates no conflicts where found in the 3 way comparison operation.
			kMSNoConflictFound							= 0,

			//! Indicates all the conflicts have been resolved either due to 3 way merge or by applying user supplied strategies.
			kMSAllConflictsResolved						= 1,

			//! Indicates that there are still some pending conflicts.
			kMSConflictsPending							= 2,

			//! the maximum value this enum can hold, should be treated as invalid value.
			kMSMaxValue				= kMaxEnumValue
		} eMergeStatus;

		//!
		//! @brief return the status of the 3 way merge operation.
		//! \return an value of type \#eMergeStatus indicating the result of the merge operation.
		//!
		virtual eMergeStatus APICALL GetMergedStatus() const = 0;

		//!
		//! @brief Return the count of resolved conflicts after the 3 way merge operation is done.
		//! \return a value of \#sizet indicating the number of conflicts resolved by 3 way merging engine as well as by the user supplied strategies.
		//!
		virtual sizet APICALL CountOfResolvedConflicts() const __NOTHROW__ = 0;

		//!
		//! @brief Return the count of unresolved conflicts after the 3 way merge operation is done.
		//! \return a value of \#sizet indicating the number of unresolved conflicts.
		//!
		virtual sizet APICALL CountOfUnResolvedConflicts() const __NOTHROW__ = 0;

		//!
		//! @brief Return a resolved conflict specified by the index.
		//! \param[in] index a value of \#sizet.
		//! \return a shared pointer to \#IThreeWayResolvedConflict object.
		//! \attention Error is thrown in case
		//!		- index provided is greater than or equal to count of resolved conflicts.
		//! \note index are 0 - based.
		//!
		virtual spcIThreeWayResolvedConflict APICALL GetResolvedConflict( sizet index ) const = 0;

		//!
		//! @brief Return an unresolved conflict specified by the index.
		//! \param[in] index a value of \#sizet.
		//! \return a shared pointer to \#IThreeWayUnResolvedConflict object.
		//! \attention Error is thrown in case
		//!		- index provided is greater than or equal to count of unresolved conflicts.
		//! \note index are 0 - based.
		//!
		virtual spcIThreeWayUnResolvedConflict APICALL GetUnResolvedConflict( sizet index ) const = 0;

		//!
		//! @brief Return all the resolved conflicts as a vector of spIThreeWayResolvedConflict.
		//! \return a shared pointer to vector of spIThreeWayResolvedConflict.
		//!
		virtual spcIThreeWayResolvedConflicts_const APICALL GetResolvedConflicts() const = 0;

		//!
		//! @brief Return all the unresolved conflicts as a vector of spIThreeWayUnResolvedConflict.
		//! \return a shared pointer to vector of spIThreeWayUnResolvedConflict.
		//!
		virtual spcIThreeWayUnResolvedConflicts_const APICALL GetUnResolvedConflicts() const = 0;

		//!
		//! @brief Return the merged metadata object taking into consideration all the resolved conflicts.
		//! \return a shared pointer of \#IMetadata object.
		//! \attention Error is thrown in case
		//!		- there are still some pending unresolved conflicts.
		//!
		virtual spIMetadata APICALL GetMergedMetadata() const = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Return the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return either a const or non const pointer to IThreeWayMergeResult interface.
		//!
		virtual pIThreeWayMergeResult APICALL GetActualIThreeWayMergeResult() __NOTHROW__ = 0;
		XMP_PRIVATE pcIThreeWayMergeResult GetActualIThreeWayMergeResult() const __NOTHROW__ {
			return const_cast< IThreeWayMergeResult_v1 * >( this )->GetActualIThreeWayMergeResult();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Return the pointer to internal interfaces.
		//! \return either a const or non const pointer to IThreeWayMergeResult_I interface.
		//!
		virtual AdobeXMPCompareAndMerge_Int::pIThreeWayMergeResult_I APICALL GetIThreeWayMergeResult_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPCompareAndMerge_Int::pcIThreeWayMergeResult_I GetIThreeWayMergeResult_I() const __NOTHROW__ {
			return const_cast< IThreeWayMergeResult_v1 * >( this )->GetIThreeWayMergeResult_I();
		}
		//!
		//! @}
		
		//!
		//! {@
		//! @brief Convert raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIThreeWayMergeResult MakeShared( pIThreeWayMergeResult_base ptr );
		XMP_PRIVATE static spcIThreeWayMergeResult MakeShared( pcIThreeWayMergeResult_base ptr )  {
			return MakeShared( const_cast< pIThreeWayMergeResult_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief return the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIThreeWayMergeResultID; }

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
		virtual ~IThreeWayMergeResult_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual uint32 APICALL getMergedStatus( pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcIThreeWayUnResolvedConflict_base APICALL getUnResolvedConflict( sizet index, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcIThreeWayResolvedConflict_base APICALL getResolvedConflict( sizet index, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pIMetadata_base APICALL getMergedMetadata( pcIError_base & err ) const __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __IThreeWayMergeResult_h__
