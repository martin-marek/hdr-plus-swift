#ifndef __ITrack_h__
#define __ITrack_h__ 1

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
//
//
// =================================================================================================
#endif	// AdobePrivate

#include "XMPMarker/XMPMarkerFwdDeclarations.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPMarker/Interfaces/IMarker.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCore;
	using namespace AdobeXMPCommon;

	class XMP_PUBLIC ITrack_v1
		: public virtual ISharedObject,
		public virtual IVersionable
	{


	public:

		typedef enum{

			//! Include markers which lie within both start and end time boundary
			kTTBothStartTimeAndEndTimeIncluded  = 1 << 1,

			//! Include markers which lie outside both start and end time boundary
			kTTBothStartTimeAndEndTimeExcluded = 1 << 2,

			//! Include markers which lie after the start time boundary
			kTTOnlyStartTimeIncluded = 1 << 3,

			//! Include markers which lie before the end time boundary
			kTTOnlyEndTimeIncluded = 1 <<4,

			kTTMax = AdobeXMPCommon::kMaxEnumValue

		}	eTrackTimeBoundary;


		// Factories to create the structure node
		//!
		//! \brief Creates a track which is not part of any temporal asset
		//! \param[in] trackName pointer to a const NULL terminated char buffer containing name of the trackName
		//! \param[in] trackTypes uint64 representing bitwise OR of predefined marker types
		//! \param[in] trackNameLen Length of char buffer containing name of the trackName
		//! \param[in] trackTypeNameLen Length of char buffer containing name of the user defined type to be associated with the track.
		//! \param[in] trackTypeName pointer to a const NULL terminated char buffer containing name of the user defined type to be associated with the track.
		//!  If trackTypes had kMTUserDefinedMarker as true, only then this parameter should be non-NULL, otherwise it will be ignored.
		//!  In case the kMTUserDefinedMarker was provided in trackTypes, but trackTypeName is NULL, then kMTUserDefinedMarker will not be added to the track type.
		//! \return a shared pointer to a \#AdobeXMPAM::ITrack object.
		//! \attention error is thrown in the following cases:
		//!		-# trackName is NULL or its contents are empty.
		//!		-# trackTypes is invalid is NULL or is 0
		//!
		static  XMP_PRIVATE spITrack CreateTrack(const char * trackName, sizet trackNameLen, uint64 trackTypes, const char * trackTypeName = NULL, sizet trackTypeNameLen = 0);



		//!
		//! \brief Creates a marker with specified name and adds it to the current track
		//! \param[in] markerName pointer to a const NULL terminated char buffer containing name of the trackName
		//! \param[in] markerTypes uint64 representing bitwise OR of predefined marker types
		//! \param[in] markerTypeName pointer to a const NULL terminated char buffer containing name of the user defined type to be associated with the marker.
		//!  If markerTypes had kMTUserDefinedMarker as true, only then this parameter should be non-NULL, otherwise it will be ignored.
		//!  In case the kMTUserDefinedMarker was provided in markerTypes, but markerTypeName is NULL, then kMTUserDefinedMarker will not be added to the marker type.
		//! \param[in] guid pointer to a const NULL terminated char buffer containing guid of the marker to be created.
		//! \param[in] markerNameLen Length of char buffer containing name of the trackName
		//! \param[in] markerTypeNameLen Length of char buffer containing user defined type to be associated with the marker
		//! \param[in] guidLen Length of char buffer containing guid of the marker to be created.
		//! \return a shared pointer to a \#AdobeXMPAM::IMarker object.
		//! \attention error is thrown in the following cases:
		//!		-# markerName is NULL or its contents are empty.
		//!		-# markerTypes is invalid is 0
		//!


		virtual spIMarker  APICALL AddMarker(const char * markerName, sizet markerNameLen, uint64 markerTypes, const char * markerTypeName = NULL, sizet markerTypeNameLen = 0, const char * guid = NULL, sizet guidLen = 0) = 0;


		////!
		////! \brief Adds inMarker to the track
		////! \param[in] inMarker shared pointer to an object of NS_ASSETMANAGEMENT::IMarker, in case the marker object is already associated with a track object, the marker will not be added
		////! \attention error is thrown in the following cases:
		////!		-# inMarker is an invalid shared pointer
		////!

		virtual spIMarker  APICALL AddMarker(spIMarker inMarker) = 0;



		////!
		////! \brief Removes the marker present at a particular index in the track
		////! \param[in] markerIdx  sizet - indicating the 1 based index of the marker in the track
		////! \return uint64 the number of markers present in the track after removal of the marker
		////!

		virtual spIMarker  APICALL RemoveMarker(const sizet & markerIdx) = 0;



		////!
		////! \brief Removes the marker with the guid equal to inputGuid
		////! \param[in] inputGuid  pointer to a const NULL terminated char buffer containing the GUID 
		////! \return a shared pointer to the AdobeXMPAM::IMarker object. An invalid shared pointer will be returned
		////! in case the track doesn't contain any marker with the specified GUID.
		////!

		virtual spIMarker  APICALL RemoveMarker(const char * inputGuid, sizet inputGuidLen) = 0;

		////!
		////! \brief Returns the marker present at a particular index in the track
		////! \param[in] markerIdx  sizet - indicating the 1 based index of the marker in the track
		////! \return a shared pointer to the AdobeXMPAM::IMarker object. An invalid shared pointer will be returned
		////! in case the index is invalid
		////!

		virtual spIMarker  APICALL GetMarker(const sizet & markerIdx) const = 0;
		//
		////!
		////! \brief Returns list of all markers present in the track
		////! \return a shared pointer to std::vector of shared pointers of  AdobeXMPAM::IMarker objects. 
		////!

		virtual spIMarkerList  APICALL GetAllMarkers()  const = 0;


		////!
		////! \brief Sets the frame rate of the track which will be used in case the markers contained inside the track don't specify a frame rate
		////! \param[in] numerator a uint64 to specify the frame rate numerator
		////! \param[in] denominator a uint64 to specify the frame rate denominator
		////! \attention Error is thrown in case
		////!		-# denominator specified in 0
		////!

		virtual void  APICALL SetFrameRate(uint64 numerator = 1, uint64 denominator = 1) = 0;

		////!
		////! \brief Gets the frame rate numerator and the denominator associated with the track
		////! \param[out] numerator a uint64 to specify the frame rate numerator, 1 will be returned if numerator hasn't been specified explicitly
		////! \param[out] denominator a uint64 to specify the frame rate denominator, 1 will be returned if denominator hasn't been specified explicitly
		////!
		virtual void  APICALL GetFrameRate(uint64 & numerator, uint64 & denominator) const = 0;


		////!
		////! \brief Returns the name of the track
		////! \return a shared pointer of \#NS_XMPCOMMON::IUTF8String containing
		////! the track name
		////! 

		virtual spcIUTF8String  APICALL GetName() const = 0;


		////!
		////! \brief Sets the name of the track
		////! \param[in] trackName a const char * which would be the name that has to be assigned to the track
		////! \attention Warning is thrown in case
		////!		-# trackName supplied is a null pointer
		////!

		virtual void APICALL  SetName(const char * trackName, sizet trackNameLen) = 0;


		////!
		////! \brief Returns the marker with the guid equal to inputGuid
		////! \param[in] inputGuid  pointer to a const NULL terminated char buffer containing the GUID to be searched
		////! \return a shared pointer to the AdobeXMPAM::IMarker object. An invalid shared pointer will be present
		////! in case the track doesn't contain any marker with the specified GUID.
		////!

		virtual spIMarker  APICALL GetMarker(const char * inputGuid, sizet inputGuidLen) const = 0;

		////!
		////! \brief Returns count of markers present in the asset
		////! \return - sizet which will represent the number of markers currently present in the marker
		////!

		virtual sizet  APICALL GetMarkerCount() const = 0;

		////!
		////! \brief Virtual copy constructor
		////! Clones the track creating an exact replica of the marker which is not part of any temporal asset.
		////! \param[in] multiThreadingSupport a value of type \#NS_XMPCOMMON::eMultiThreadingSupport controlling whether
		////! the new object created should be thread safe or not. By default new object created will inherit its setting from
		////! the object begin cloned.
		////! \return - a shared pointer to the new track
		////!
		virtual spITrack  APICALL Clone() const = 0;


		//
		////!
		////! \brief Returns list of markers present in the track
		////! \param[in] startFrameCount uint64 the start frame for comparison
		////! \param[in] endFrameCount uint64 the end frame for 
		////! \param[in] markerTypes uint64 the bitwise OR of markerTypes to be searched
		////! \param[in] timeBoundary enum of eTrackTimeBoundary to specify how comparison should be done
		////! \param[in] matchAllCriteria boolean - should be specified as true if marker matching all the criteria should only be returned
		////! \param[in] markerTypeName std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::IMarker objects
		////!		

		virtual spIMarkerList  APICALL GetMarkersInTimeRange(uint64 startFrameCount = 0, uint64 endFrameCount = UINT64_MAX, eTrackTimeBoundary timeBoundary = kTTBothStartTimeAndEndTimeIncluded, uint64 markerTypes = UINT64_MAX, bool matchAllCriteria = false, const std::vector<std::pair< const char *, sizet> >&markerTypeName = std::vector<std::pair<const char *, sizet> >()) const = 0;


		////!
		////! \brief Returns list of markers present in the track
		////! \param[in] markerName pointer to a null terminated constant char buffer containing name of the marker
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::IMarker objects
		////!		

		virtual spIMarkerList  APICALL GetMarkersByName(const char * markerName, sizet markerNameLen) const = 0;

		////!
		////!  \brief Removes markers present in the track
		////! \param[in] markerTypes uint64 the bitwise OR of markerTypes to be searched
		////! \param[in] matchAllCriteria boolean - should be specified as true if marker matching all the criteria should only be returned
		////! \param[in] markerTypeName std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::IMarker objects
		////!	

		virtual spIMarkerList APICALL  RemoveMarkers(uint64 markerTypes = UINT64_MAX, bool matchAllCriteria = false, const std::vector<std::pair< const char *, sizet> > &markerTypeName = std::vector<std::pair<  const char *, sizet> >()) = 0;

		////!
		////!  \brief Returns list of markers present in the track
		////! \param[in] markerTypes uint64 the bitwise OR of markerTypes to be searched
		////! \param[in] matchAllCriteria boolean - should be specified as true if marker matching all the criteria should only be returned
		////! \param[in] markerTypeName std::vector of shared pointers of NS_XMPCOMMON:IUTF8String objects specifying the names of the user defined types
		////! \return a shared pointer to a std::vector of \#AdobeXMPAM::IMarker objects
		////!		
		virtual spIMarkerList  APICALL GetMarkers(uint64 markerTypes = UINT64_MAX, bool matchAllCriteria = false, const std::vector<std::pair< const char *, sizet> > &markerTypeName = std::vector< std::pair< const char *, sizet> >())  const = 0;

		////!
		////! \brief Sets marker types, mask should be a bitwise OR of predefined marker types.
		////! \param[in] inputMask uint64 - which should be bitwise OR of predefined marker types. 
		////! \return uint64 which will be the bitwise OR of the marker types after the adding types specified by inputMask
		////!

		virtual uint64  APICALL SetTypes(uint64 inputMask) = 0;

		////!
		////! \brief Returns the bitwise OR of the predefined marker types associated with the marker. 
		////! \return uint64 - it will be the bitwise OR of the marker types currently associated with the marker. 
		////! If marker contains a user-defined type, the return value will also have the KMTUserDefined bit set.
		////!

		virtual uint64 APICALL  GetTypes() const = 0;

		////!
		////! \brief Appends userDefinedType to the track type if it is already not present in the markerType
		////! \param[in] userDefinedType const char * pointing to a user defined track type name.  if userDefinedType is NULL, false is returned. 
		////! \return bool If track already contained userDefinedType in the markerType or userDefinedType is NULL, false is returned, otherwise true is returned.
		////!

		virtual bool APICALL  AppendCustomType(const char * userDefinedType, sizet userDefinedTypeLen) = 0;

		////!
		////! \brief Removes userDefinedType to the track type if it is present in the markerType
		////! \param[in] userDefinedType const char * pointing to a user defined track type name.  if userDefinedType is NULL, false is returned. 
		////! \return bool If track doesn't contain userDefinedType in the track type or userDefinedType is NULL, false is returned, otherwise true is returned.
		////!

		virtual bool APICALL  RemoveCustomType(const char  * userDefinedType, sizet userDefinedTypeLen) = 0;

		////!
		////! \brief Returns number of user-defined/custom types which have been assigned to the track
		////! \return sizet - The number of user-defined/custom types which have been assigned to the track
		////!

		virtual sizet  APICALL GetCustomTypeCount() const = 0;

		////!
		////! \brief Returns the list of user defined types associated with the track
		////! \return std::vector of shared_ptr of NS_XMPCOMMON::IUTF8String containing user defined marker types associated with the vector, empty vector will be returned 
		////!	if the marker is not user defined
		////!

		virtual spcIUTF8StringList APICALL  GetCustomTypes() const = 0;


		////!
		////! \brief Retus true if the customType is associated with the marker
		////! \param[in] customType const char * pointing to a custom marker type name.  if customType is NULL, false is returned.
		////! \param[in] customTypeLen - length of the customType
		////! \return bool - true if customType is associated with the marker, false otherwise
		////!
		virtual bool  APICALL HasCustomType(const char * customType, sizet customTypeLen) const = 0;

		////!
		////! \brief Returns true if the markerTypeMask is associated with the marker
		////! \param[in] markerTypeMask eMarkerType the logical OR of prefined marker types that are to be checked in the marker
		////! \return bool - true if markerTypeMask is associated with the marker, false otherwise
		////!

		virtual bool  APICALL HasTypes(IMarker::eMarkerType  markerTypeMask) const = 0;


		////!
		////! \brief Sets marker types, mask should be a bitwise OR of predefined marker types.
		////! \param[in] inputMask uint64 - which should be bitwise OR of predefined marker types. 
		////! \return uint64 which will be the bitwise OR of the marker types after the adding types specified by inputMask
		////!

		virtual IMarker::eMarkerType  APICALL RemoveTypes(IMarker::eMarkerType markerTypes) = 0;
		
		
		XMP_PRIVATE static uint64 GetInterfaceID() { return kITrackID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

		XMP_PRIVATE static spITrack MakeShared(pITrack_base ptr);
		XMP_PRIVATE static spcITrack MakeShared(pcITrack_base ptr) {
			return MakeShared(const_cast<pITrack_base>(ptr));
		}


		virtual pITrack APICALL GetActualITrack() __NOTHROW__ = 0;
		XMP_PRIVATE pcITrack GetActualITrack() const __NOTHROW__{
			return const_cast<ITrack_v1 *>(this)->GetActualITrack();
		}

		virtual AdobeXMPAM_Int::pITrack_I APICALL GetITrack_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcITrack_I GetITrack_I() const __NOTHROW__{
			return const_cast<ITrack_v1 *>(this)->GetITrack_I();
		}

	protected:

		virtual pIMarker_base  APICALL addMarker(const char * markerName, sizet markerNameLen, uint64 markerTypes, const char * markerTypeName, sizet markerTypeNameLen, const char * guid, sizet guidLen, pcIError_base & error) = 0;

		virtual pIMarker_base  APICALL addMarker(pIMarker_base  inMarker, pcIError_base & error) = 0;

		virtual pIMarker_base APICALL  removeMarker(const char * guid, sizet guidLen, pcIError_base & error) = 0;

		virtual pIMarker_base APICALL  removeMarker(const sizet & markerIdx, pcIError_base & error) = 0;

		virtual pIMarker_base  APICALL getMarker(const char * guid, sizet guidLen, pcIError_base & error) const = 0;

		virtual pIMarker_base  APICALL getMarker(const sizet & markerIdx, pcIError_base & error) const = 0;

		virtual pITrack_base APICALL  clone(pcIError_base & error) const = 0;

		virtual uint64  APICALL setTypes(uint64 inputMask, pcIError_base & error) = 0;

		virtual IMarker::eMarkerType  APICALL removeTypes(IMarker::eMarkerType inputMask, pcIError_base & error) = 0;

		virtual uint64  APICALL getTypes(pcIError_base & error) const = 0;

		virtual bool  APICALL appendCustomType(const char * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error) = 0;

		virtual bool APICALL  removeCustomType(const char  * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error) = 0;

		virtual sizet  APICALL getCustomTypeCount(pcIError_base & error) const = 0;

		virtual sizet  APICALL getCustomTypes(pcIUTF8String_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

		virtual pcIUTF8String_base  APICALL getName(pcIError_base &error) const = 0;

		virtual void  APICALL setName(const char * name, sizet nameLen, pcIError_base & error) = 0;

		virtual void  APICALL setFrameRate(uint64 numerator, uint64 denominator, pcIError_base & error) = 0;

		virtual sizet  APICALL getAllMarkers(pIMarker_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

		virtual void APICALL  foo(void * v, sizet size, pcIError_base & error) = 0;

		virtual sizet APICALL  removeMarkers(uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) = 0;

		virtual sizet  APICALL getMarkers(uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

		virtual sizet  APICALL getMarkersByName(const char * name, sizet nameLen, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

		virtual sizet APICALL  getMarkersInTimeRange(uint64 startFrameCount, uint64 endFrameCount, uint32 timeBoundary, uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

	
	//	virtual sizet GetMarkers(uint64 markerTypes, bool matchAllCriteria, const char ** markerTypeName, sizet stringCount, IMarker_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error )  = 0;
	//	virtual spIMarkerList GetMarkersInTimeRange(uint64 startFrameCount = 0, uint64 endFrameCount = UINT64_MAX, uint64 markerTypes = UINT64_MAX, bool matchAllCriteria = false, const spcIStringList & markerTypeName = spcIStringList());
		//	virtual sizet GetMarkersInTimeRange(uint64 startFrameCount, uint64 endFrameCount, uint64 markerTypes, uint32 matchAllCriteria, const spcIStringList & markerTypeName, pIMarker_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error);


	protected:
		//!
		//! protected Virtual Destructor
		//!
		virtual ~ITrack_v1() __NOTHROW__ = 0;
	
#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
#endif
		REQ_FRIEND_CLASS_DECLARATION();
		
	};
inline ITrack_v1::~ITrack_v1() __NOTHROW__{}




}

#endif //__ITrack_h__