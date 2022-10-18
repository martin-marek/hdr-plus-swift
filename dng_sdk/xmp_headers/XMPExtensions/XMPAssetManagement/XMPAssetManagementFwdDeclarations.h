#ifndef XMPAssetManagementFwdDeclarations_h__
#define XMPAssetManagementFwdDeclarations_h__ 1

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
//  SKP Sunil Kishor Pathak
//  ADC	Amandeep Chawla
//
// mm-dd-yy who Description of changes, most recent first.
//
// 02-11-15 SKP 1.0-a049 Added initial support for client strategies. Using these strategies client
//						 can set prefix, AppName, history update policy and it's own datetime
//						 function which will be used to get time for ModifyDate, Metadata date etc.
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 11-26-14 ADC 1.0-a013 Added the full skeleton for IComposedAssetManager interface.
// 11-24-14 ADC 1.0-a008 Support for ICompositionRelationship interface.
// 11-21-14 ADC 1.0-a004 Support for IAssetLayerPart interface.
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPAssetManagement/XMPAssetManagementDefines.h"
#include "XMPCommon/XMPCommonFwdDeclarations.h"
#include "XMPAssetManagement/XMPAssetManagementLatestInterfaceVersions.h"
#include <vector>

#ifdef XMPEXTENSIONS_CLIENT_HEADER
#define QUOTEME(FILE_NAME) #FILE_NAME
#include QUOTEME(XMPEXTENSIONS_CLIENT_HEADER)
#endif

namespace AdobeXMPAM {
	using AdobeXMPCommon::uint64;
	using namespace AdobeXMPCommon;
	using AdobeXMPCommon::shared_ptr;
	using AdobeXMPCommon::sizet;

	// IExtensionsObjectFactory

	class IExtensionsObjectFactory_v1;
	typedef IExtensionsObjectFactory_v1														IExtensionsObjectFactory_base;
	typedef IExtensionsObjectFactory_v1 *														pIExtensionsObjectFactory_base;
	typedef const IExtensionsObjectFactory_v1 *												pcIExtensionsObjectFactory_base;
	typedef BASE_CLASS(IExtensionsObjectFactory, IExtensionsObjectFactory_VERSION)		IExtensionsObjectFactory;
	typedef IExtensionsObjectFactory *														pIExtensionsObjectFactory;
	typedef const IExtensionsObjectFactory *													pcIExtensionsObjectFactory;
	static const uint64 kIExtensionsObjectFactoryID(0x654f626a46616374 /* cObjFact */);


	// IAssetUtilities
	class IAssetUtilities_v1;
	typedef IAssetUtilities_v1																	IAssetUtilities_base;
	typedef IAssetUtilities_v1 *																	pIAssetUtilities_base;
	typedef const IAssetUtilities_v1 *															pcIAssetUtilities_base;
	typedef BASE_CLASS(IAssetUtilities, IASSETUTILITIES_VERSION)										IAssetUtilities;
	typedef IAssetUtilities *																	pIAssetUtilities;
	typedef const IAssetUtilities *																pcIAssetUtilities;
	typedef shared_ptr< IAssetUtilities >														spIAssetUtilities;
	typedef shared_ptr< const IAssetUtilities >													spcIAssetUtilities;
	static const uint64 kIAssetUtilitiesID(0x6541506172742020 /* eAPart    */);

	/*
	class IAssetUtilities_v1;

	typedef IAssetUtilities_v1									IAssetUtilities_base;
	typedef IAssetUtilities_v1 *								pIAssetUtilities_base;
	typedef const IAssetUtilities_v1 *							pcIAssetUtilities_base;

	typedef IAssetUtilities_v1									IAssetUtilities_latest;

	#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	#ifndef CLIENT_IASSETUTILITIES_VERSION
	#define CLIENT_IASSETUTILITIES_VERSION 1
	#endif
	class IAssetUtilities;
	#else
	typedef IAssetUtilities_latest							IAssetUtilities;
	#endif

	typedef IAssetUtilities *									pIAssetUtilities;
	typedef const IAssetUtilities *								pcIAssetUtilities;
	typedef shared_ptr< IAssetUtilities >						spIAssetUtilities;
	typedef shared_ptr< const IAssetUtilities >					spcIAssetUtilities;
	*/

	// IAssetPart
	/*class IAssetPart_v1;

	typedef IAssetPart_v1										IAssetPart_base;
	typedef IAssetPart_v1 *										pIAssetPart_base;
	typedef const IAssetPart_v1 *								pcIAssetPart_base;

	typedef IAssetPart_v1										IAssetPart_latest;

	#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	#ifndef CLIENT_IASSETPART_VERSION
	#define CLIENT_IASSETPART_VERSION 1
	#endif
	class IAssetPart;
	#else
	typedef IAssetPart_latest								IAssetPart;
	#endif

	typedef IAssetPart *										pIAssetPart;
	typedef const IAssetPart *									pcIAssetPart;
	typedef shared_ptr< IAssetPart >							spIAssetPart;
	typedef shared_ptr< const IAssetPart >						spcIAssetPart;
	*/

	class IAssetPart_v1;
	typedef IAssetPart_v1																	IAssetPart_base;
	typedef IAssetPart_v1 *																	pIAssetPart_base;
	typedef const IAssetPart_v1 *															pcIAssetPart_base;
	typedef BASE_CLASS(IAssetPart, IASSETPART_VERSION)										IAssetPart;
	typedef IAssetPart *																	pIAssetPart;
	typedef const IAssetPart *																pcIAssetPart;
	typedef shared_ptr< IAssetPart >														spIAssetPart;
	typedef shared_ptr< const IAssetPart >													spcIAssetPart;
	static const uint64 kIAssetPartID(0x6541506172742020 /* eAPart    */);

	// IAssetTimePart
	class IAssetTimePart_v1;
	typedef IAssetTimePart_v1																	IAssetTimePart_base;
	typedef IAssetTimePart_v1 *																	pIAssetTimePart_base;
	typedef const IAssetTimePart_v1 *															pcIAssetTimePart_base;
	typedef BASE_CLASS(IAssetTimePart, IASSETTIMEPART_VERSION)										IAssetTimePart;
	typedef IAssetTimePart *																	pIAssetTimePart;
	typedef const IAssetTimePart *																pcIAssetTimePart;
	typedef shared_ptr< IAssetTimePart >														spIAssetTimePart;
	typedef shared_ptr< const IAssetTimePart >													spcIAssetTimePart;
	static const uint64 kIAssetTimePartID(0x6554415061727420/* eTAPart    */);

	/*	class IAssetTimePart_v1;

	typedef IAssetTimePart_v1									IAssetTimePart_base;
	typedef IAssetTimePart_v1 *									pIAssetTimePart_base;
	typedef const IAssetTimePart_v1 *							pcIAssetTimePart_base;

	typedef IAssetTimePart_v1									IAssetTimePart_latest;

	#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	#ifndef CLIENT_IASSETTIMEPART_VERSION
	#define CLIENT_IASSETTIMEPART_VERSION 1
	#endif
	class IAssetTimePart;
	#else
	typedef IAssetTimePart_latest							IAssetTimePart;
	#endif

	typedef IAssetTimePart *									pIAssetTimePart;
	typedef const IAssetTimePart *								pcIAssetTimePart;
	typedef shared_ptr< IAssetTimePart >						spIAssetTimePart;
	typedef shared_ptr< const IAssetTimePart >					spcIAssetTimePart;
	*/
	// IAssetLayerPart
	class IAssetLayerPart_v1;
	typedef IAssetLayerPart_v1																	IAssetLayerPart_base;
	typedef IAssetLayerPart_v1 *																	pIAssetLayerPart_base;
	typedef const IAssetLayerPart_v1 *															pcIAssetLayerPart_base;
	typedef BASE_CLASS(IAssetLayerPart, IASSETLAYERPART_VERSION)										IAssetLayerPart;
	typedef IAssetLayerPart *																	pIAssetLayerPart;
	typedef const IAssetLayerPart *																pcIAssetLayerPart;
	typedef shared_ptr< IAssetLayerPart >														spIAssetLayerPart;
	typedef shared_ptr< const IAssetLayerPart >													spcIAssetLayerPart;
	static const uint64 kIAssetLayerPartID(0x654C415061727420/* eLAPart   */);

	/*class IAssetLayerPart_v1;

	typedef IAssetLayerPart_v1									IAssetLayerPart_base;
	typedef IAssetLayerPart_v1 *								pIAssetLayerPart_base;
	typedef const IAssetLayerPart_v1 *							pcIAssetLayerPart_base;

	typedef IAssetLayerPart_v1									IAssetLayerPart_latest;

	#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	#ifndef CLIENT_IASSETLAYERPART_VERSION
	#define CLIENT_IASSETLAYERPART_VERSION 1
	#endif
	class IAssetLayerPart;
	#else
	typedef IAssetLayerPart_latest							IAssetLayerPart;
	#endif

	typedef IAssetLayerPart *									pIAssetLayerPart;
	typedef const IAssetLayerPart *								pcIAssetLayerPart;
	typedef shared_ptr< IAssetLayerPart >						spIAssetLayerPart;
	typedef shared_ptr< const IAssetLayerPart >					spcIAssetLayerPart;
	*/
	// IAssetPagePart
	class IAssetPagePart_v1;
	typedef IAssetPagePart_v1																	IAssetPagePart_base;
	typedef IAssetPagePart_v1 *																	pIAssetPagePart_base;
	typedef const IAssetPagePart_v1 *															pcIAssetPagePart_base;
	typedef BASE_CLASS(IAssetPagePart, IASSETPAGEPART_VERSION)									IAssetPagePart;
	typedef IAssetPagePart *																	pIAssetPagePart;
	typedef const IAssetPagePart *																pcIAssetPagePart;
	typedef shared_ptr< IAssetPagePart >														spIAssetPagePart;
	typedef shared_ptr< const IAssetPagePart >													spcIAssetPagePart;
	static const uint64 kIAssetPagePartID(0x6550415061727420/* ePAPart    */);
	/*class IAssetPagePart_v1;

	typedef IAssetPagePart_v1									IAssetPagePart_base;
	typedef IAssetPagePart_v1 *									pIAssetPagePart_base;
	typedef const IAssetPagePart_v1 *							pcIAssetPagePart_base;

	typedef IAssetPagePart_v1									IAssetPagePart_latest;

	#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	#ifndef CLIENT_IASSETPAGEPART_VERSION
	#define CLIENT_IASSETPAGEPART_VERSION 1
	#endif
	class IAssetPagePart;
	#else
	typedef IAssetPagePart_latest							IAssetPagePart;
	#endif

	typedef IAssetPagePart *									pIAssetPagePart;
	typedef const IAssetPagePart *								pcIAssetPagePart;
	typedef shared_ptr< IAssetPagePart >						spIAssetPagePart;
	typedef shared_ptr< const IAssetPagePart >					spcIAssetPagePart;

	*/
	// IAssetArtboardPart
	class IAssetArtboardPart_v1;
	typedef IAssetArtboardPart_v1																	IAssetArtboardPart_base;
	typedef IAssetArtboardPart_v1 *																	pIAssetArtboardPart_base;
	typedef const IAssetArtboardPart_v1 *															pcIAssetArtboardPart_base;
	typedef BASE_CLASS(IAssetArtboardPart, IASSETARTBOARDPART_VERSION)										IAssetArtboardPart;
	typedef IAssetArtboardPart *																	pIAssetArtboardPart;
	typedef const IAssetArtboardPart *																pcIAssetArtboardPart;
	typedef shared_ptr< IAssetArtboardPart >														spIAssetArtboardPart;
	typedef shared_ptr< const IAssetArtboardPart >													spcIAssetArtboardPart;
	static const uint64 kIAssetArtboardPartID(0x6541415061727420/* eAAPart    */);
	/*class IAssetArtboardPart_v1;

	typedef IAssetArtboardPart_v1								IAssetArtboardPart_base;
	typedef IAssetArtboardPart_v1 *								pIAssetArtboardPart_base;
	typedef const IAssetArtboardPart_v1 *						pcIAssetArtboardPart_base;

	typedef IAssetArtboardPart_v1								IAssetArtboardPart_latest;

	#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	#ifndef CLIENT_IASSETARTBOARDPART_VERSION
	#define CLIENT_IASSETARTBOARDPART_VERSION 1
	#endif
	class IAssetArtboardPart;
	#else
	typedef IAssetArtboardPart_latest						IAssetArtboardPart;
	#endif

	typedef IAssetArtboardPart *								pIAssetArtboardPart;
	typedef const IAssetArtboardPart *							pcIAssetArtboardPart;
	typedef shared_ptr< IAssetArtboardPart >					spIAssetArtboardPart;
	typedef shared_ptr< const IAssetArtboardPart >				spcIAssetArtboardPart;
	*/

	// IAssetManager
	class IAssetManager_v1;
	typedef IAssetManager_v1																	IAssetManager_base;
	typedef IAssetManager_v1 *																	pIAssetManager_base;
	typedef const IAssetManager_v1 *															pcIAssetManager_base;
	typedef BASE_CLASS(IAssetManager, IASSETMANAGER_VERSION)										IAssetManager;
	typedef IAssetManager *																	pIAssetManager;
	typedef const IAssetManager *																pcIAssetManager;
	typedef shared_ptr< IAssetManager >														spIAssetManager;
	typedef shared_ptr< const IAssetManager >													spcIAssetManager;
	static const uint64 kIAssetManagerID(0x6541506172742020 /* eAPart    */);

	// ICompositionRelationship
	class ICompositionRelationship_v1;
	typedef ICompositionRelationship_v1																	ICompositionRelationship_base;
	typedef ICompositionRelationship_v1 *																	pICompositionRelationship_base;
	typedef const ICompositionRelationship_v1 *															pcICompositionRelationship_base;
	typedef BASE_CLASS(ICompositionRelationship, ICOMPOSITIONRELATIONSHIP_VERSION)										ICompositionRelationship;
	typedef ICompositionRelationship *																	pICompositionRelationship;
	typedef const ICompositionRelationship *																pcICompositionRelationship;
	typedef shared_ptr< ICompositionRelationship >														spICompositionRelationship;
	typedef shared_ptr< const ICompositionRelationship >													spcICompositionRelationship;
	static const uint64 kICompositionRelationshipID(0x6541506172742020 /* eAPart    */);






	/*class ICompositionRelationship_v1;

	typedef ICompositionRelationship_v1							ICompositionRelationship_base;
	typedef ICompositionRelationship_v1 *						pICompositionRelationship_base;
	typedef const ICompositionRelationship_v1 *					pcICompositionRelationship_base;

	typedef ICompositionRelationship_v1							ICompositionRelationship_latest;

	#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	#ifndef CLIENT_ICOMPOSITIONRELATIONSHIP_VERSION
	#define CLIENT_ICOMPOSITIONRELATIONSHIP_VERSION 1
	#endif
	class ICompositionRelationship;
	#else
	typedef ICompositionRelationship_latest					ICompositionRelationship;
	#endif

	typedef ICompositionRelationship *							pICompositionRelationship;
	typedef const ICompositionRelationship *					pcICompositionRelationship;
	typedef shared_ptr< ICompositionRelationship >				spICompositionRelationship;
	typedef shared_ptr< const ICompositionRelationship >		spcICompositionRelationship;*/

	// IComposedAssetManager

	class IComposedAssetManager_v1;
	typedef IComposedAssetManager_v1																	IComposedAssetManager_base;
	typedef IComposedAssetManager_v1 *																	pIComposedAssetManager_base;
	typedef const IComposedAssetManager_v1 *															pcIComposedAssetManager_base;
	typedef BASE_CLASS(IComposedAssetManager, ICOMPOSEDASSETMANAGER_VERSION)										IComposedAssetManager;
	typedef IComposedAssetManager *																	pIComposedAssetManager;
	typedef const IComposedAssetManager *																pcIComposedAssetManager;
	typedef shared_ptr< IComposedAssetManager >														spIComposedAssetManager;
	typedef shared_ptr< const IComposedAssetManager >													spcIComposedAssetManager;
	static const uint64 kIComposedAssetManagerID(0x6541506172742020 /* eAPart    */);

	/*	class IComposedAssetManager_v1;

	typedef IComposedAssetManager_v1							IComposedAssetManager_base;
	typedef IComposedAssetManager_v1 *							pIComposedAssetManager_base;
	typedef const IComposedAssetManager_v1 *					pcIComposedAssetManager_base;

	typedef IComposedAssetManager_v1							IComposedAssetManager_latest;

	#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	#ifndef CLIENT_ICOMPOSEDASSETMANAGER_VERSION
	#define CLIENT_ICOMPOSEDASSETMANAGER_VERSION 1
	#endif
	class IComposedAssetManager;
	#else
	typedef IComposedAssetManager_latest					IComposedAssetManager;
	#endif

	typedef IComposedAssetManager *								pIComposedAssetManager;
	typedef const IComposedAssetManager *						pcIComposedAssetManager;
	typedef shared_ptr< IComposedAssetManager >					spIComposedAssetManager;
	typedef shared_ptr< const IComposedAssetManager >			spcIComposedAssetManager;
	*/
	// typedefs for vectors and their corresponding shared pointers.
	typedef std::vector< spICompositionRelationship >			ICompositionRelationshipList;
	typedef std::vector< spcICompositionRelationship >			cICompositionRelationshipList;

	typedef shared_ptr< ICompositionRelationshipList >			spICompositionRelationshipList;
	typedef shared_ptr< cICompositionRelationshipList >			spcICompositionRelationshipList;



	// IExtensionsConfigurationManager
	class IExtensionsConfigurationManager_v1;
	typedef IExtensionsConfigurationManager_v1												IExtensionsConfigurationManager_base;
	typedef IExtensionsConfigurationManager_v1 *												pIExtensionsConfigurationManager_base;
	typedef const IExtensionsConfigurationManager_v1 *										pcIExtensionsConfigurationManager_base;
	typedef BASE_CLASS(IExtensionsConfigurationManager, IEXTENSIONSMANAGER_VERSION)	IExtensionsConfigurationManager;
	typedef IExtensionsConfigurationManager *													pIExtensionsConfigurationManager;
	typedef const IExtensionsConfigurationManager *											pcIExtensionsConfigurationManager;
	typedef shared_ptr< IExtensionsConfigurationManager >										spIExtensionsConfigurationManager;
	typedef shared_ptr< const IExtensionsConfigurationManager >								spcIExtensionsConfigurationManager;
	static const uint64 kIExtensionsConfigurationManagerID(0x63436f6e664d6772 /* cConfMgr */);
}

namespace AdobeXMPAM_Int {

	// IAssetPart_I
	class IAssetPart_I;
	typedef IAssetPart_I *														pIAssetPart_I;
	typedef const IAssetPart_I *												pcIAssetPart_I;

	class IAssetPagePart_I;
	typedef IAssetPagePart_I *														pIAssetPagePart_I;
	typedef const IAssetPagePart_I *												pcIAssetPagePart_I;

	class IAssetLayerPart_I;
	typedef IAssetLayerPart_I *														pIAssetLayerPart_I;
	typedef const IAssetLayerPart_I *												pcIAssetLayerPart_I;

	class IAssetArtboardPart_I;
	typedef IAssetArtboardPart_I *														pIAssetArtboardPart_I;
	typedef const IAssetArtboardPart_I *												pcIAssetArtboardPart_I;

	class IAssetTimePart_I;
	typedef IAssetTimePart_I *														pIAssetTimePart_I;
	typedef const IAssetTimePart_I *												pcIAssetTimePart_I;

	class IAssetUtilities_I;
	typedef IAssetUtilities_I *														pIAssetUtilities_I;
	typedef const IAssetUtilities_I *												pcIAssetUtilities_I;

	class IComposedAssetManager_I;
	typedef IComposedAssetManager_I *													pIComposedAssetManager_I;
	typedef const IComposedAssetManager_I *												pcIComposedAssetManager_I;

	class IExtensionsConfigurationManager_I;
	typedef IExtensionsConfigurationManager_I *											pIExtensionsConfigurationManager_I;
	typedef const IExtensionsConfigurationManager_I *									pcIExtensionsConfigurationManager_I;

	class ICompositionRelationship_I;
	typedef ICompositionRelationship_I *												pICompositionRelationship_I;
	typedef const ICompositionRelationship_I *											pcICompositionRelationship_I;

	class IExtensionsObjectFactory_I;
	typedef IExtensionsObjectFactory_I *												pIExtensionsObjectFactory_I;
	typedef const IExtensionsObjectFactory_I *											pcIExtensionsObjectFactory_I;

	// IAssetManager_I
	class IAssetManager_I;
	typedef IAssetManager_I *						pIAssetManager_I;
	typedef const IAssetManager_I *					pcIAssetManager_I;
	

}
#endif  // XMPAssetManagementFwdDeclarations_h__

