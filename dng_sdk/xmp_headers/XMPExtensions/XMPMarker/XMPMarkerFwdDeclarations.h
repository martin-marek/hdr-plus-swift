#ifndef XMPMarkerFwdDeclarations_h__
#define XMPMarkerFwdDeclarations_h__ 1

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
#include "XMPAssetManagement/XMPAssetManagementDefines.h"
#include "XMPCommon/XMPCommonFwdDeclarations.h"
#include "XMPAssetManagement/XMPAssetManagementLatestInterfaceVersions.h"
#include <vector>
#include <utility>
#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"

#ifdef XMPEXTENSIONS_CLIENT_HEADER
	#define QUOTEME(FILE_NAME) #FILE_NAME
	#include QUOTEME(XMPEXTENSIONS_CLIENT_HEADER)
#endif

namespace AdobeXMPAM {
	using AdobeXMPCommon::shared_ptr;
	using AdobeXMPCommon::uint32;
	using AdobeXMPCommon::uint64;

	class IMarker_v1;
	typedef IMarker_v1																	IMarker_base;
	typedef IMarker_v1 *																pIMarker_base;
	typedef const IMarker_v1 *															pcIMarker_base;
	typedef BASE_CLASS(IMarker, IMARKER_VERSION)										IMarker;
	typedef IMarker *																	pIMarker;
	typedef const IMarker *																pcIMarker;
	typedef shared_ptr< IMarker >														spIMarker;
	typedef shared_ptr< const IMarker >													spcIMarker;
	static const uint64 kIMarkerID(0X654d6172/* eMar    */);



	


	class ITemporalAssetManager_v1;
	typedef ITemporalAssetManager_v1																ITemporalAssetManager_base;
	typedef ITemporalAssetManager_v1 *																pITemporalAssetManager_base;
	typedef const ITemporalAssetManager_v1 *														pcITemporalAssetManager_base;
	typedef BASE_CLASS(ITemporalAssetManager, ITEMPORALASSETMANAGER_VERSION)						ITemporalAssetManager;
	typedef ITemporalAssetManager *																	pITemporalAssetManager;
	typedef const ITemporalAssetManager *															pcITemporalAssetManager;
	typedef shared_ptr< ITemporalAssetManager >														spITemporalAssetManager;
	typedef shared_ptr< const ITemporalAssetManager >												spcITemporalAssetManager;
	static const uint64 kITemporalAssetManagerID(0x65547261/* eTemporalAssetManager    */);

	typedef std::pair<AdobeXMPCommon::spIUTF8String, AdobeXMPCommon::spIUTF8String> 								ICuePointParam;
	typedef std::pair<AdobeXMPCommon::spcIUTF8String, AdobeXMPCommon::spcIUTF8String> 								cICuePointParam;
	typedef shared_ptr< ICuePointParam >																			spICuePointParam;
	typedef shared_ptr< cICuePointParam >																			spcICuePointParam;


	typedef std::vector< ICuePointParam>																			ICuePointParamList;
	typedef std::vector< cICuePointParam >																			cICuePointParamList;
	typedef shared_ptr< ICuePointParamList >																		spICuePointParamList;
	typedef shared_ptr< cICuePointParamList >																		spcICuePointParamList;

	class ITrack_v1;
	typedef ITrack_v1																	ITrack_base;
	typedef ITrack_v1 *																	pITrack_base;
	typedef const ITrack_v1 *															pcITrack_base;
	typedef BASE_CLASS(ITrack, ITRACK_VERSION)											ITrack;
	typedef ITrack *																	pITrack;
	typedef const ITrack *																pcITrack;
	typedef shared_ptr< ITrack >														spITrack;
	typedef shared_ptr< const ITrack >													spcITrack;
	static const uint64 kITrackID(0x65547261/* eTra    */);

	
	typedef std::vector< spIMarker >									IMarkerList;
	typedef std::vector< spcIMarker >									cIMarkerList;
	typedef shared_ptr< IMarkerList >									spIMarkerList;
	typedef shared_ptr< cIMarkerList >									spcIMarkerList;

	typedef std::vector< spITrack >										ITrackList;
	typedef std::vector< spcITrack >									cITrackList;
	typedef shared_ptr< ITrackList >									spITrackList;
	typedef shared_ptr< cITrackList >									spcITrackList;

	class IExtension;
	typedef IExtension *												pIExtension;
	typedef const IExtension *											pcIExtension;
	typedef shared_ptr< IExtension >									spIExtension;
	typedef shared_ptr< const IExtension >								spcIExtension;

	// IExtensionHandler
	class IExtensionHandler;
	typedef IExtensionHandler *											pIExtensionHandler;
	typedef const IExtensionHandler *									pcIExtensionHandler;
	typedef shared_ptr< IExtensionHandler >								spIExtensionHandler;
	typedef shared_ptr< const IExtensionHandler >						spcIExtensionHandler;

	typedef void * (*MemAllocatorFunc)(AdobeXMPCommon::sizet size);

	typedef std::vector< AdobeXMPCommon::spIUTF8String>					IUTF8StringList;
	typedef std::vector< AdobeXMPCommon::spcIUTF8String >				cIUTF8StringList;

	typedef shared_ptr< IUTF8StringList >								spIUTF8StringList;
	typedef shared_ptr< cIUTF8StringList>								spcIUTF8StringList;

	
}

namespace AdobeXMPAM_Int {
	// IMarker_I
	class IMarker_I;
	typedef IMarker_I *																	pIMarker_I;
	typedef const IMarker_I *															pcIMarker_I;

	// ITrack_I
	class ITrack_I;
	typedef ITrack_I *																	pITrack_I;
	typedef const ITrack_I *															pcITrack_I;

	// ITemporalAssetManager_I
	class ITemporalAssetManager_I;
	typedef ITemporalAssetManager_I *													pITemporalAssetManager_I;
	typedef const ITemporalAssetManager_I *												pcITemporalAssetManager_I;
};

#endif  // XMPMarkerFwdDeclarations_h__

