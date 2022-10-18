#ifndef __ITSingleton_h__
#define __ITSingleton_h__ 1

// =================================================================================================
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#if AdobePrivate
// =================================================================================================
// Change history
// ==============
//
// Writers:
//  ADC Amandeep Chawla
//
// mm-dd-yy who Description of changes, most recent on top
//
// 07-10-14 ADC 5.6-c015 Refactoring, partial documentation and bug fixes of XMPCommon and XMPCore C++ APIs.
// 05-19-14 ADC 5.6-c012 Refactoring XMPCommon code and appropriate changes in XMPCore and XMPCompareAndMerge.
// 05-19-14 ADC 1.0-m020 Refactoring XMPCommon code and appropriate changes in XMPCore and XMPCompareAndMerge.
// 02-24-14 ADC 5.6-c001 XMPCommon Framework and XMPCore new APIs ported to Mac Environment.
//
// 02-09-14 ADC 1.0-m003 Re-factoring of XMPCommon Framework
// 02-05-14 ADC 1.0-m002 Some changes in the common framework
// 01-30-14 ADC 1.0-m001 First Version of the file
//
// =================================================================================================
#endif // AdobePrivate

#include "XMPCommon/XMPCommonDefines.h"

namespace AdobeXMPAM {

	//!
	//! \brief Template class that can serve as the base class for the interfaces that needs
	//! to be of Singleton type.
	//! \details Provide functions to Create, Access and Destroy Singleton instances.
	//!
	template< typename Ty >
	class XMP_PUBLIC ITSingleton {

	public:
		//!
		//! type defined to pointer of type
		//!
		typedef Ty * pointer;

		//!
		//! Provides access to the singleton instance of the type through a pointer
		//!
		XMP_PRIVATE static pointer GetInstance();

		//!
		//! Creates, if not already created, the singleton instance of the type
		//!
		XMP_PRIVATE static void CreateInstance();

		//!
		//! Destroys, if already created, the singleton instance of the type
		//!
		XMP_PRIVATE static void DestroyInstance();

	protected:
	};
}
#endif  // __ITSingleton_h__
