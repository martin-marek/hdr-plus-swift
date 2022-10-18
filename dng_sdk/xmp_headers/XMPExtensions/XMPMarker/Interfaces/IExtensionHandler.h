#ifndef IExtensionHandler_h__
#define IExtensionHandler_h__ 1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2015 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "XMPMarker/XMPMarkerFwdDeclarations.h"

namespace AdobeXMPAM {

	using namespace AdobeXMPAM;
	using namespace AdobeXMPCommon;

	class IExtensionHandler {
	public:
		virtual uint32 BeginExtension( const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) __NOTHROW__= 0;
		virtual pIExtension EndExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) __NOTHROW__= 0;

		virtual uint32 BeginStructure(const char * structureNameSpace, sizet structureNameSpaceLen, const char * structureName, sizet structureNameLen ) __NOTHROW__= 0;
		virtual uint32 EndStructure( const char * structureNameSpace, sizet structureNameSpaceLen, const char * structureName, sizet structureNameLen ) __NOTHROW__= 0;

		virtual uint32 BeginArray(const char * arrayNameSpace, sizet arrayNameSpaceLen, const char * arrayName, sizet arrayNameLen) __NOTHROW__= 0;
		virtual uint32 EndArray(const char * arrayNameSpace, sizet arrayNameSpaceLen, const char * arrayName, sizet arrayNameLen) __NOTHROW__= 0;

		virtual uint32 AddKeyValuePair(const char * keyNameSpace, sizet keyNameSpaceLen, const char * keyName, sizet keyNameLen, const char * value, sizet valueLen) __NOTHROW__= 0;

		virtual uint32 Serialize( pIExtension  data, pIExtensionHandler extensionCreator ) const __NOTHROW__ = 0;

		virtual ~IExtensionHandler() = 0;
	};

	inline IExtensionHandler::~IExtensionHandler() { }

}

#endif  // IExtensionHandler_h__
