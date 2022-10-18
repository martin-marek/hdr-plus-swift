#ifndef __ITemporalAssetManager_h__
#define __ITemporalAssetManager_h__ 1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedOwnership.h"
#include "XMPAssetManagement/Interfaces/ITrack.h"


namespace NS_XMPASSETMANAGEMENT {
	using namespace NS_XMPCOMMON;
	using namespace NS_XMPCORE;
	using namespace NS_XMPCOMMON;

	class XMP_PUBLIC ITemporalAssetManager_v1
		: public virtual ISharedOwnership_v1
	{


	public:

		// Factories to create temporal asset
		//!
		//! Creates a temporal asset with the provided shared pointer to IXMPMetadata object
		//! \param[in] inMetadata shared pointer to IXMPMetadata object for which temporal asset is to be created
		//! \param[in] multiThreadingSupport a value of type #NS_XMPCOMMON::eMultiThreadingSupport controlling whether
		//! the new object created should be thread safe or not. By default new object created will inherit its setting
		//! \return a shared pointer to a #NS_XMPASSETMANAGEMENT::ITemporalAssetManager object.
		//! \attention error is thrown in the following cases:
		//!		-# inMetadata is an invalid shared pointer
		//!		

		XMP_PRIVATE static  spITemporalAssetManager CreateTemporalAssetManager(const spIXMPMetadata & inMetadata,
			eMultiThreadingSupport multiThreadingSupport = kMultiThreadingConfiguredAtInitialization);


		//!
		//! Adds the trackToBeAdded to the temporal asset
		//! \param[in] trackToBeAdded shared pointer to ITrack object. If the trackToBeAdded already belongs to the 
		//! \return a shared pointer to a #NS_XMPASSETMANAGEMENT::ITrack object which has been added.
		//! \attention error is thrown in the following cases:
		//!		-# trackToBeAdded is an invalid shared pointer
		//!		

		virtual spITrack AddTrack(const spcITrack & trackToBeAdded) = 0;

		//!
		//! Returns the list of tracks currently present in the temporal asset
		//! \return a shared pointer to a std::vector of shared pointers to NS_XMPASSETMANAGEMENT::ITrack objects
		//!		

		virtual spcITrackList GetAllTracks() const = 0;


		//!
		//! Removes all the tracks from the temporal asset
		//!	\return void
		//!

		virtual void RemoveAllTracks() = 0;


		//!
		//!  Removes markers present in the tracks in the temporal asset matching certain criteria
		//! \param[in] trackTypes UInt64 the bitwise OR of trackTypes to be searched
		//! \param[in] matchAllCriteria boolean - should be specified as true if tracks matching all the criteria should only be returned
		//! \param[in] trackTypeNames std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		//! \return a shared pointer to a std::vector of #NS_XMPASSETMANAGEMENT::IMarker objects
		//!	

		virtual spITrackList RemoveTracks(UInt64 trackTypes = UINT64_MAX,  bool matchAllCriteria = false, std::vector<spcIUTF8String> trackTypeNames = {}) = 0;


		//!
		//! Returns list of markers present in the tracks in the temporal asset 
		//! \param[in] startFrameCount UInt64 the start frame for comparison
		//! \param[in] endFrameCount UInt64 the end frame for 
		//! \param[in] markerTypes UInt64 the bitwise OR of markerTypes to be searched
		//! \param[in] matchAllCriteria boolean - should be specified as true if marker matching all the criteria should only be returned
		//! \param[in] markerTypeNames std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		//! \return a shared pointer to a std::vector of #NS_XMPASSETMANAGEMENT::IMarker objects
		//!		

		virtual spcIMarkerList GetMarkersInTimeRange(UInt64 startFrameCount, UInt64 endFrameCount, UInt64 markerTypes = UINT64_MAX, bool matchAllCriteria = false, std::vector<spcIUTF8String> markerTypeNames = {}) const = 0;


		//!
		//! Returns list of markers present in the tracks in the temporal asset with a particular marker name
		//! \param[in] markerName pointer to a null terminated constant char buffer containing name of the marker
		//! \return a shared pointer to a std::vector of #NS_XMPASSETMANAGEMENT::IMarker objects
		//!		

		virtual spcIMarkerList GetMarkersByName(const char * markerName) const = 0;

		//!
		//!  Removes markers present in the tracks in the temporal asset matching certain criteria
		//! \param[in] markerTypes UInt64 the bitwise OR of markerTypes to be searched
		//! \param[in] matchAllCriteria boolean - should be specified as true if marker matching all the criteria should only be returned
		//! \param[in] markerTypeNames std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		//! \return a shared pointer to a std::vector of #NS_XMPASSETMANAGEMENT::IMarker objects
		//!	

		virtual SizeT RemoveMarkers(UInt64 markerTypes = UINT64_MAX, bool matchAllCriteria = false, std::vector<spcIUTF8String> markerTypeNames = {}) = 0;

		//!
		//!  Returns list of markers present in the tracks in the temporal asset matching certain criteria
		//! \param[in] markerTypes UInt64 the bitwise OR of markerTypes to be searched
		//! \param[in] matchAllCriteria boolean - should be specified as true if marker matching all the criteria should only be returned
		//! \param[in] markerTypeNames std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		//! \return a shared pointer to a std::vector of #NS_XMPASSETMANAGEMENT::IMarker objects
		//!		
		virtual spcIMarkerList GetMarkers(UInt64 markerTypes = UINT64_MAX, bool matchAllCriteria = false, std::vector<spcIUTF8String> markerTypeNames = {}) const = 0;
	protected:
		//!
		//! protected Virtual Destructor
		//!
		virtual ~ITemporalAssetManager_v1() = 0;

#if GENERATE_XMPASSETMANAGEMENT_CLIENT_LAYER_CODE
		friend class ITemporalAssetManager;
#endif
	};

}
#endif