#ifndef XMPAssetManagementLatestInterfaceVersions_h__
#define XMPAssetManagementLatestInterfaceVersions_h__ 1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2015 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

//!
//! Macro to include a client file through with client can control the interface versions he wants to stick with
//! if not the latest ones.
//!
#if !SOURCE_COMPILING_XMPEXTENSIONS_LIB
#ifdef XMPEXTENSIONS_CLIENT_VERSION_NUMBER_FILE
#include QUOTEME(XMPEXTENSIONS_CLIENT_VERSION_NUMBER_FILE)
#endif
#endif

#ifndef IASSETPART_VERSION
#define IASSETPART_VERSION 1
#endif

#ifndef IASSETPAGEPART_VERSION
#define IASSETPAGEPART_VERSION 1
#endif

#ifndef IASSETLAYERPART_VERSION
#define IASSETLAYERPART_VERSION 1
#endif

#ifndef IASSETTIMEPART_VERSION
#define IASSETTIMEPART_VERSION 1
#endif

#ifndef IASSETARTBOARDPART_VERSION
#define IASSETARTBOARDPART_VERSION 1
#endif

#ifndef IExtensionsObjectFactory_VERSION
#define IExtensionsObjectFactory_VERSION 1
#endif

#ifndef IASSETUTILITIES_VERSION
#define IASSETUTILITIES_VERSION 1
#endif

#ifndef ICOMPOSEDASSETMANAGER_VERSION
#define ICOMPOSEDASSETMANAGER_VERSION 1
#endif


#ifndef IEXTENSIONSMANAGER_VERSION
#define IEXTENSIONSMANAGER_VERSION 1
#endif


#ifndef ICOMPOSITIONRELATIONSHIP_VERSION
#define ICOMPOSITIONRELATIONSHIP_VERSION 1
#endif

#ifndef IASSETMANAGER_VERSION
#define IASSETMANAGER_VERSION 1
#endif


#ifndef ITRACK_VERSION
#define ITRACK_VERSION 1
#endif

#ifndef IMARKER_VERSION
#define IMARKER_VERSION 1
#endif

#ifndef ITEMPORALASSETMANAGER_VERSION
#define ITEMPORALASSETMANAGER_VERSION 1
#endif

#endif  // XMPCoreLatestInterfaceVersions_h__

