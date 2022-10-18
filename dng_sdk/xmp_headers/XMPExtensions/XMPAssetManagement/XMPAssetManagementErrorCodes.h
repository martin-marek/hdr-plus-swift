#ifndef XMPAssetManagementErrorCodes_h__
#define XMPAssetManagementErrorCodes_h__ 1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
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
//  ADC	Amandeep Chawla
//
// mm-dd-yy who Description of changes, most recent first.
//
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPAssetManagement/XMPAssetManagementDefines.h"

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon_Int;

	class IErrorAssetManagement {
	public:
		typedef enum {
			//! Indicates that the part's rdf string is not well formed.
			kPartNotWellFormed				= 0,
			//! Indicates that the page part's rdf string is not well formed.
			kPagePartValueNotWellFormed		= 1,
			//! Indicates that the time part's rdf string is not well formed.
			kTimePartValueNotWellFormed		= 2,

			//! Maximum value this enum can hold, should be treated as invalid value.
			kAssetManagementCodeMaxValue = kMaxEnumValue
		} eAssetManagementErrorCodes;
	};
}
#endif  // XMPAssetManagementErrorCodes_h__
