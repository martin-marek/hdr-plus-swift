#ifndef __IThreeWayMerge_h__
#define __IThreeWayMerge_h__ 1

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

#include <string>
#define TXMP_STRING_TYPE std::string
#include "XMP.hpp"

namespace AdobeXMPCompareAndMerge {
	//!
	//! @brief Version1 of the interface that represents an object which performs 3 way Merge.
	//! Provides all the functions
	//!		- to perform 3 way merge.
	//!		- to register 3 way conflict resolution strategies for custom conflict resolution.
	//!
	class XMP_PUBLIC IThreeWayMerge_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:

		//!
		//! @brief An enum to indicate the strategies for auto merging
		//!
		typedef enum {
			//! no AutoMergeStrategy, should be treated as invalid value.
			kAMSNone = 0,

			//!A strategy that picks the property on the basis of latest date. 
			kAMSLatestDate = 1 << 0,

			//!A strategy that picks the property on the basis of oldest date. 
			kAMSOldestDate = 1 << 1,
		
			//!A strategy that resolves History conflicts.
			kAMSHistory = 1 << 2 ,

			//!A strategy that resolves Versions conflicts.
			kAMSVersions = 1 << 3 ,

			//!A strategy that resolves Ingredients conflicts.
			kAMSIngredients = 1 << 4 ,

			//!A strategy that resolves Pantry conflicts.
			kAMSPantry = 1 << 5 ,

			//!A strategy that resolves the conflicts of Array of Date type properties
			kAMSArrayOfDate = 1 << 6 ,

			//!A strategy that resolves the conflicts of Array of Structure of Simple Node which have a primary key.
			kAMSArrayOfStructOfSimpleNodeWithPrimaryKey = 1 << 7,

			//!A strategy that resolves the conflicts of Array of Structure of Simple Node type properties
			kAMSArrayOfStructOfSimpleNode = 1 << 8 ,
			
			//!A strategy that resolves the conflicts of Array of Structures which have Simple Nodes and 
			//!Structure of Simple Nodes as its fields.
			kAMSArrayOfStructureOfSimpleAndStructOfSimple = 1 << 9 ,

			//!A strategy that resolves the conflicts of Array of Structures which have Simple Nodes and
			//!Ordered Array of Simple Nodes as its fields.
			kAMSArrayOfStructureOfSimpleAndOrderedArrOfSimple = 1 << 10 ,

			//!A strategy that resolves the conflicts of Array of Simple Node type properties
			kAMSArrayOfSimpleNode = 1 << 11 ,

			//!A strategy that resolves the conflicts of Simple Node type properties
			kAMSSimpleNode = 1 << 12 ,

			//!A strategy that resolves the conflicts purely on priority basis.
			kAMSBlind = 1 << 13,

			//!All strategies
			kAMSAll = ~( 1 << 31 ),

			//! the maximum value this enum can hold and should be treated as invalid value.
			kAMSMaxValue = 1 << 31
		} eAutoMergeStrategy;

		//!
		//! @brief Tries to merge 3 metadata objects which are at the different versions.
        //! \param[in] baseVersionMetadata shared pointer to const \#IMetadata object representing the common base version to the user and latest metadata objects.
        //! \param[in] latestVersionMetadata a shared pointer to const \#IMetadata object representing the latest version available with the client.
        //! \param[in] userVersionMetadata a shared pointer to const \#IMetadata object representing the user version available with the client.
		//! \return a shared pointer to \#IThreeWayMergeResult containing all the information regarding the merge status and results.
		//!
		virtual spcIThreeWayMergeResult APICALL Merge( const spcIMetadata & baseVersionMetadata, const spcIMetadata & latestVersionMetadata, const spcIMetadata & userVersionMetadata ) const = 0;

		//!
		//! @brief Merges 3 metadata objects which are at the different versions. For this, different autoMerge strategies are being registered.
		//! In case of unresolvable conflicts, priority is given to latest version. If baseVersionMetadata is invalid, then merged packet would be the latestVersionMetadata.
        //! \param[in] baseVersionMetadata reference to a shared pointer to const \#IMetadata object representing the common base version to the user and latest metadata objects.
        //! \param[in] latestVersionMetadata reference to a shared pointer to const \#IMetadata object representing the latest version available with the client.
        //! \param[in] userVersionMetadata reference to a shared pointer to const \#IMetadata object representing the user version available with the client.
		//! \param[in] skipAutoMergeStrategy a value indicating the strategies which client does not want to register. 
		//! \return a shared pointer to \#IThreeWayMergeResult containing all the information regarding the merge status and results.
		//! \attention Error is thrown in case
		//!		- In case of property type mismatch and wherever the type of specific properties are evaluated and found to be invalid.
		//!     - In case latestVersionMetadata and userVersionMetadata is invalid.
		//! \attention Warning is thrown in case
		//!     - In case of duplicate primary key. 
		//! For more information on different strategies refer to https://wiki.corp.adobe.com/display/XMP/AutoMerge+in+XMPCompareAndMerge
		//!
		virtual spcIThreeWayMergeResult APICALL AutoMerge ( const spcIMetadata & baseVersionMetadata, const spcIMetadata & latestVersionMetadata, const spcIMetadata & userVersionMetadata , eAutoMergeStrategy skipAutoMergeStrategy = kAMSNone ) const = 0;

		//!
		//! @brief Merges 3 metadata objects which are at the different versions. For this, different autoMerge strategies are being registered.
		//! In case of unresolvable conflicts, priority is given to latest version. If baseVersionMetadata is null, then merged packet would be the latestVersionMetadata.
		//! \param[in] baseVersionMetadata reference to to a constant char buffer containing representing the common base version to the user and latest metadata objects.
		//! \param[in] latestVersionMetadata reference to to a constant char buffer containing representing the latest version available with the client.
		//! \param[in] userVersionMetadata reference to to a constant char buffer containing representing the user version available with the client.
        //! \param[in] isMerged a bool value indicating whether the merge operation is performed.
		//!   \li true if user and latest were different and merging is successfully performed. Merged packet is returned. 
		//!   \li false if user and latest were different and some conflicts are unresolved. Invalid shared pointer is returned.
		//!             if latest and user were same and there were no conflicts. Merged packet will be same as user/latest. 
		//! \param[in] skipAutoMergeStrategy a value indicating the strategies which client does not want to register. 
		//! \return a shared pointer to \#IUTF8String containing serialized merged packet. 
		//! \attention Error is thrown in case
		//!		- In case of property type mismatch and wherever the type of specific properties are evaluated and found to be invalid.
		//!		- In case latestVersionMetadata and userVersionMetadata is null.
		//! \attention Warning is thrown in case
		//!     - In case of duplicate primary key. 
		//! For more information on different strategies refer to https://wiki.corp.adobe.com/display/XMP/AutoMerge+in+XMPCompareAndMerge
		//!
		virtual spcIUTF8String APICALL AutoMerge ( const char * baseVersionMetadata, sizet baseVersionMetadataLength, const char * latestVersionMetadata, sizet latestVersionMetadataLength, const char * userVersionMetadata, sizet userVersionMetadataLength, bool & isMerged , eAutoMergeStrategy skipAutoMergeStrategy = kAMSNone ) const = 0;
        
        //!
        //! @brief Merges 3 metadata objects which are at the different versions. For this, different autoMerge strategies are being registered.
        //! In case of unresolvable conflicts, priority is given to latest version. If baseVersionMetadata is invalid, then merged packet would be the latestVersionMetadata.
        //! \param[in] baseVersionMetadata reference to a pointer to const \#SXMPMeta object representing the common base version to the user and latest metadata objects.
        //! \param[in] latestVersionMetadata reference to a pointer to const \#SXMPMeta object representing the latest version available with the client.
        //! \param[in] userVersionMetadata reference to a pointer to const \#SXMPMeta object representing the user version available with the client.
        //! \param[in] isMerged a bool value indicating whether the merge operation is performed.
        //! \param[in] skipAutoMergeStrategy a value indicating the strategies which client does not want to register.
        //! \return a object of \#SXMPMeta containing merged metadata.
        //! \attention Error is thrown in case
        //!		- In case of property type mismatch and wherever the type of specific properties are evaluated and found to be invalid.
        //!     - In case latestVersionMetadata and userVersionMetadata is invalid.
        //! \attention Warning is thrown in case
        //!     - In case of duplicate primary key.
        //! For more information on different strategies refer to https://wiki.corp.adobe.com/display/XMP/AutoMerge+in+XMPCompareAndMerge
        //!

        virtual SXMPMeta APICALL AutoMerge ( const SXMPMeta* baseVersionMetadata, const SXMPMeta* latestVersionMetadata, const SXMPMeta* userVersionMetadata, bool & isMerged , eAutoMergeStrategy skipAutoMergeStrategy = kAMSNone ) const = 0;

		//!
		//! @brief Allows clients to plug in their own conflict resolution strategies.
		//! \param[in] strategy a raw pointer to \#IThreeWayStrategy object which will be used by the conflict resolution engine
		//! to resolve the conflict.
		//!
		virtual bool APICALL RegisterThreeWayStrategy( pIThreeWayStrategy_base strategy ) = 0;

		//!
		//! @brief Allows clients to plug in generic strategies for conflict resolution.
		//! \param[in] strategy a shared pointer to \#IThreeWayGenericStrategy object which will be used by the conflict resolution engine
		//! to resolve the conflict.
		//!
		virtual bool APICALL RegisterThreeWayGenericStrategy( const spIThreeWayGenericStrategy & strategy ) = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Return the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return either a const or non const pointer to IThreeWayMerge interface.
		//!
		virtual pIThreeWayMerge APICALL GetActualIThreeWayMerge() __NOTHROW__ = 0;
		XMP_PRIVATE pcIThreeWayMerge GetActualIThreeWayMerge() const __NOTHROW__ {
			return const_cast< IThreeWayMerge_v1 * >( this )->GetActualIThreeWayMerge();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Return the pointer to internal interfaces.
		//! \return either a const or non const pointer to IThreeWayMerge_I interface.
		//!
		virtual AdobeXMPCompareAndMerge_Int::pIThreeWayMerge_I APICALL GetIThreeWayMerge_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPCompareAndMerge_Int::pcIThreeWayMerge_I GetIThreeWayMerge_I() const __NOTHROW__ {
			return const_cast< IThreeWayMerge_v1 * >( this )->GetIThreeWayMerge_I();
		}
		//!
		//! @}
		
		//!
		//! {@
		//! @brief Convert raw pointer to shared pointer. The raw pointer is of version 1 interface
		//! where as the returned shared pointer depends on the version client is interested in.
		//! \return shared pointer to const or non constant interface.
		//!
		XMP_PRIVATE static spIThreeWayMerge MakeShared( pIThreeWayMerge_base ptr );
		XMP_PRIVATE static spcIThreeWayMerge MakeShared( pcIThreeWayMerge_base ptr )  {
			return MakeShared( const_cast< pIThreeWayMerge_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief return the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIThreeWayMergeID; }

		//!
		//! @brief return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		// static factory functions

		//!
		//! @brief Creates an IThreeWayMerge object.
		//! \return a shared pointer to an IThreeWayMerge object
		//!
		XMP_PRIVATE static spIThreeWayMerge CreateThreeWayMerge();


	protected:
		//!
		//! Destructor
		//! 
		virtual ~IThreeWayMerge_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pcIThreeWayMergeResult_base APICALL merge( pcIMetadata_base baseVersionMetadata, pcIMetadata_base latestVersionMedata, pcIMetadata_base userVersionMetadata, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual uint32 APICALL registerThreeWayStrategy( pIThreeWayStrategy_base strategy, pcIError_base & error ) __NOTHROW__ = 0;
		virtual uint32 APICALL registerThreeWayGenericStrategy( pIThreeWayGenericStrategy_base strategy, pcIError_base & error ) __NOTHROW__ = 0;
		virtual pcIThreeWayMergeResult_base APICALL autoMerge ( pcIMetadata_base baseVersionMetadata, pcIMetadata_base latestVersionMetadata, pcIMetadata_base userVersionMetadata, uint32 skipAutoMergeStrategy, pcIError_base & error ) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL autoMerge ( const char * baseVersionMetadata, sizet baseVersionMetadataLength, const char * latestVersionMetadata, sizet latestVersionMetadataLength, const char * userVersionMetadata, sizet userVersionMetadataLength, uint32 & isMerged, uint32 skipAutoMergeStrategy, pcIError_base & error ) const __NOTHROW__= 0;
        virtual XMPMetaRef APICALL autoMerge ( XMPMetaRef baseVersionMetadata, XMPMetaRef latestVersionMetadata, XMPMetaRef userVersionMetadata, uint32 & isMerged, uint32 skipAutoMergeStrategy, pcIError_base & error ) const __NOTHROW__ = 0;

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __IThreeWayMerge_h__
