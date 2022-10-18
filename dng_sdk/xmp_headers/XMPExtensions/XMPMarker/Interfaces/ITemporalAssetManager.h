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

#include "XMPMarker/XMPMarkerFwdDeclarations.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPMarker/Interfaces/ITrack.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCore;
	using namespace AdobeXMPCommon;

	class XMP_PUBLIC ITemporalAssetManager_v1
		: public virtual ISharedObject,
		  public virtual IVersionable
	{


	public:

		//!
		//! \brief Factories to create temporal asset manager to manage tracks and markers present in a IXMPMetadata object
		//!
		//! Creates a temporal asset with the provided shared pointer to IXMPMetadata object
		//! \param[in] inMetadata shared pointer to IXMPMetadata object for which temporal asset is to be created
		//! \return a shared pointer to a \#AdobeXMPAM::ITemporalAssetManager object.
		//! \attention error is thrown in the following cases:
		//!		-# inMetadata is an invalid shared pointer
		//!		

		static  XMP_PRIVATE spITemporalAssetManager CreateTemporalAssetManager(const spIMetadata & inMetadata);


		////!
		////! \brief Adds the trackToBeAdded to the temporal asset
		////! \param[in] trackToBeAdded shared pointer to ITrack object. If the trackToBeAdded already belongs to the 
		////! \return a shared pointer to a \#AdobeXMPAM::ITrack object which has been added.
		////! \attention error is thrown in the following cases:
		////!		-# trackToBeAdded is an invalid shared pointer
		////!		

		virtual spITrack APICALL AddTrack( spITrack  trackToBeAdded) = 0;

		////!
		////! \brief Returns the list of tracks currently present in the temporal asset
		////! \return a shared pointer to a std::vector of shared pointers to AdobeXMPAM::ITrack objects
		////!		

		virtual spITrackList APICALL GetAllTracks() const = 0;


		////!
		////! \brief Removes all the tracks from the temporal asset
		////!	\return void
		////!

		virtual spITrackList APICALL RemoveAllTracks() = 0;

		////!
		////!  \brief Returns a list of tracks present in the temporal asset matching certain criteria
		////! \param[in] trackTypes uint64 the bitwise OR of trackTypes to be searched
		////! \param[in] matchAllCriteria boolean - should be specified as true if tracks matching all the criteria should only be returned
		////! \param[in] trackTypeNames std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::ITrack objects
		////!	

		virtual spITrackList APICALL GetTracks(IMarker::eMarkerType markerTypes = UINT64_MAX, bool matchAllCriteria = false, const std::vector<std::pair< const char *, sizet> > &markerTypeName = std::vector<std::pair<const char *, sizet> >()) const = 0;

		////!
		////!  \brief Removes tracks present in the temporal asset matching certain criteria
		////! \param[in] trackTypes uint64 the bitwise OR of trackTypes to be searched
		////! \param[in] matchAllCriteria boolean - should be specified as true if tracks matching all the criteria should only be returned
		////! \param[in] trackTypeNames std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::ITrack objects
		////!	

		virtual spITrackList APICALL  RemoveTracks(IMarker::eMarkerType markerTypes = UINT64_MAX, bool matchAllCriteria = false, const std::vector<std::pair<const char *, sizet> > &markerTypeName = std::vector<std::pair<const char *, sizet> >()) = 0;


		////!
		////! \brief Returns list of markers present in the tracks in the temporal asset 
		////! \param[in] startFrameCount uint64 the start frame for comparison
		////! \param[in] endFrameCount uint64 the end frame for
		////! \param[in] timeBoundary enum of eTrackTimeBoundary to specify how comparison should be done
		////! \param[in] markerTypes uint64 the bitwise OR of markerTypes to be searched
		////! \param[in] matchAllCriteria boolean - should be specified as true if marker matching all the criteria should only be returned
		////! \param[in] markerTypeNames std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::IMarker objects
		////!		

		virtual spIMarkerList  APICALL GetMarkersInTimeRange(uint64 startFrameCount, uint64 endFrameCount, ITrack::eTrackTimeBoundary timeBoundary = ITrack::eTrackTimeBoundary::kTTBothStartTimeAndEndTimeIncluded, IMarker::eMarkerType markerTypes = UINT64_MAX, bool matchAllCriteria = false, const std::vector<std::pair<const char *, sizet> > &markerTypeNames = std::vector<std::pair< const char *, sizet> >()) const = 0;


		////!
		////! \brief Returns list of markers present in the tracks in the temporal asset with a particular marker name
		////! \param[in] markerName pointer to a null terminated constant char buffer containing name of the marker
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::IMarker objects
		////!		

		virtual spIMarkerList  APICALL GetMarkersByName(const char * markerName, sizet markerNameLen) const = 0;

		////!
		////!  \brief Removes markers present in the tracks in the temporal asset matching certain criteria
		////! \param[in] markerTypes uint64 the bitwise OR of markerTypes to be searched
		////! \param[in] matchAllCriteria boolean - should be specified as true if marker matching all the criteria should only be returned
		////! \param[in] markerTypeNames std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::IMarker objects
		////!	

		virtual spIMarkerList APICALL  RemoveMarkers(IMarker::eMarkerType markerTypes = UINT64_MAX, bool matchAllCriteria = false, const std::vector<std::pair<const char *, sizet> > &markerTypeName = std::vector<std::pair<const char *, sizet> >()) = 0;

		////!
		////! \brief Returns list of markers present in the tracks in the temporal asset matching certain criteria
		////! \param[in] markerTypes uint64 the bitwise OR of markerTypes to be searched
		////! \param[in] matchAllCriteria boolean - should be specified as true if marker matching all the criteria should only be returned
		////! \param[in] markerTypeNames std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::IMarker objects
		////!		
		virtual spIMarkerList  APICALL GetMarkers(IMarker::eMarkerType markerTypes = UINT64_MAX, bool matchAllCriteria = false, const std::vector<std::pair<const char *, sizet> > &markerTypeName = std::vector< std::pair<const char *, sizet > >()) const = 0;

		XMP_PRIVATE static uint64 GetInterfaceID() { return kITrackID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

		XMP_PRIVATE static spITemporalAssetManager MakeShared(pITemporalAssetManager_base ptr);
		XMP_PRIVATE static spcITemporalAssetManager MakeShared(pcITemporalAssetManager_base ptr) {
			return MakeShared(const_cast<pITemporalAssetManager_base>(ptr));
		}

		virtual pITemporalAssetManager APICALL GetActualITemporalAssetManager() __NOTHROW__ = 0;
		XMP_PRIVATE pcITemporalAssetManager GetActualITemporalAssetManager() const __NOTHROW__{
			return const_cast<ITemporalAssetManager_v1 *>(this)->GetActualITemporalAssetManager();
		}

		virtual AdobeXMPAM_Int::pITemporalAssetManager_I APICALL GetITemporalAssetManager_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcITemporalAssetManager_I GetITemporalAssetManager_I() const __NOTHROW__{
			return const_cast<ITemporalAssetManager_v1 *>(this)->GetITemporalAssetManager_I();
		}

	protected:
		virtual pITrack_base APICALL  addTrack(pITrack_base  trackToBeAdded, pcIError_base & error) = 0;

		virtual sizet  APICALL getAllTracks(pITrack_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

		virtual sizet APICALL  removeAllTracks(pITrack_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error) = 0;

		virtual sizet  APICALL removeMarkers(uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) = 0;

		virtual sizet  APICALL getMarkers(uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

		virtual sizet  APICALL getMarkersByName(const char * name, sizet nameLen, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

		virtual sizet  APICALL removeTracks(uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pITrack_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) = 0;

		virtual sizet APICALL  getTracks(uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pITrack_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

		virtual sizet APICALL  getMarkersInTimeRange(uint64 startFrameCount, uint64 endFrameCount, uint32 timeBoundary, uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;


	protected:
		//!
		//! \brief protected Virtual Destructor
		//!
		virtual ~ITemporalAssetManager_v1() = 0;

#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};
	inline ITemporalAssetManager_v1::~ITemporalAssetManager_v1() __NOTHROW__{}

}


#endif