#ifndef __ICompareAndMergeObjectFactory_h__
#define __ICompareAndMergeObjectFactory_h__ 1

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
#include "XMPCommon/Interfaces/IObjectFactory.h"

namespace AdobeXMPCompareAndMerge {

	//!
	//! @brief Version1 of the interface that represents a factory to create various artifacts of XMP Compare and Merge objects.
	//!
	class XMP_PUBLIC ICompareAndMergeObjectFactory_v1
		: public virtual IObjectFactory_v1
	{
	public:

		//!
		//! @brief Creates a 3 way merge object.
		//! \return a pointer to an IThreeWayMerge_v1 object.
		//!
		virtual pIThreeWayMerge_base APICALL CreateThreeWayMerge( pcIError_base & error ) __NOTHROW__ = 0;

		//!
		//! @brief Provides the reference to the database of Generic strategies available with the library.
		//! \return a pointer to \#IGenericStrategyDatabase_v1 object containing all the entries for generic strategies.
		//!
		virtual pcIGenericStrategyDatabase_base APICALL GetGenericStrategyDatabase(pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \cond XMP_INTERNAL_DOCUMENTATION
		
		//!
		//! @{
		//! @brief Return the actual raw pointer from the shared pointer, which can be a shared pointer of a proxy class.
		//! \return either a const or non const pointer to ICompareAndMergeObjectFactory interface.
		//!
		virtual pICompareAndMergeObjectFactory APICALL GetActualICompareAndMergeObjectFactory() __NOTHROW__ = 0;
		XMP_PRIVATE pcICompareAndMergeObjectFactory GetActualICompareAndMergeObjectFactory() const __NOTHROW__ {
			return const_cast< ICompareAndMergeObjectFactory_v1 * >( this )->GetActualICompareAndMergeObjectFactory();
		}
		//!
		//! @}

		//!
		//! @{
		//! @brief Return the pointer to internal interfaces.
		//! \return either a const or non const pointer to ICompareAndMergeObjectFactory_I interface.
		//!
		virtual AdobeXMPCompareAndMerge_Int::pICompareAndMergeObjectFactory_I APICALL GetICompareAndMergeObjectFactory_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPCompareAndMerge_Int::pcICompareAndMergeObjectFactory_I GetICompareAndMergeObjectFactory_I() const __NOTHROW__ {
			return const_cast< ICompareAndMergeObjectFactory_v1 * >( this )->GetICompareAndMergeObjectFactory_I();
		}
		//!
		//! @}
		
		//!
		//! {@
		//! @brief Convert raw pointer to base version to pointer to client interested version.
		//! The raw pointer is of version 1 interface where as the returned pointer depends on the version client is interested in.
		//! \return pointer to const or non constant interface.
		//!
		XMP_PRIVATE static pICompareAndMergeObjectFactory MakeCompareAndMergeObjectFactory( pICompareAndMergeObjectFactory_base ptr );
		XMP_PRIVATE static pcICompareAndMergeObjectFactory MakeCompareAndMergeObjectFactory( pcICompareAndMergeObjectFactory_base ptr )  {
			return MakeCompareAndMergeObjectFactory( const_cast< pICompareAndMergeObjectFactory_base >( ptr ) );
		}
		//!
		//! @}

		//!
		//! @brief return the unique ID assigned to the interface.
		//! \return 64 bit unsigned integer representing the unique ID assigned to the interface.
		//!
		XMP_PRIVATE static uint64 GetInterfaceID() { return kICompareAndMergeObjectFactoryID; }

		//!
		//! @brief return the version of the interface.
		//! \return 32 bit unsigned integer representing the version of the interface.
		//!
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		//! \endcond

		// static factory functions

		//!
		//! @brief Get an object of \#ICompareAndMergeOjbectFactory.
		//! \return a pointer to an \#ICompareAndMergeOjbectFactory object.
		//!
		XMP_PRIVATE static pICompareAndMergeObjectFactory GetCompareAndMergeObjectFactory();

		//!
		//! @{
		//! @brief Set up the compare and merge object factory.
		//! \param[in] coreObjectFactory a pointer to an \#pICompareAndMergeOjbectFactory_base object.
		//! \note compareAndMergeObjectFactory is an optional parameter and only required for clients who don't directly link with the library
		//! but want to use its functionality.
		//!
	#if LINKING_XMPCOMPAREANDMERGE_LIB
		XMP_PRIVATE static void SetupCompareAndMergeObjectFactory();
	#else
		XMP_PRIVATE static void SetupCompareAndMergeObjectFactory( pICompareAndMergeObjectFactory_base coreObjectFactory );
	#endif
		//! @}

		//!
		//! @brief Destroy everything related to core object factory.
		//!
		XMP_PRIVATE static void DestroyCompareAndMergeObjectFactory();

	protected:
		//!
		//! @brief Destructor
		//! 
		virtual ~ICompareAndMergeObjectFactory_v1() __NOTHROW__ {}

		//! \cond XMP_INTERNAL_DOCUMENTATION
		//! REMOVE: list all ABI safe virtual functions, don't forget to add APICALL.

		#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
		#endif
		REQ_FRIEND_CLASS_DECLARATION();
		//! \endcond

	};
}

#endif  // __ICompareAndMergeObjectFactory_h__
