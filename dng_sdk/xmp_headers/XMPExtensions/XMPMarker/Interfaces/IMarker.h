#ifndef __IMarker_h__
#define __IMarker_h__ 1

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
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"
#include <float.h>

namespace AdobeXMPAM {

	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCore;
	using namespace AdobeXMPCommon;


	//! \class IMarker_v1
	//!
	//! \brief Version1 of the IMarker interface that serves as a base for performing operations on a marker
	//! \details Provides all the functions to get/set/remove various properties of the marker.
	//! \attention Support multi threading through locks but can be disabled by the client.
	//!
    class XMP_PUBLIC IMarker_v1
		: public virtual ISharedObject,
		public virtual IVersionable
	{
	public:

		typedef uint64 eMarkerType;

		
			//! "Comment"
			static eMarkerType const kMTComment;

			//! "Chapter"
			static  eMarkerType const kMTChapter;

			//!	"FLVCuePoint"
			static  eMarkerType const kMTFLVCuePoint;

			//! "WebLink"
			static  eMarkerType const kMTWebLink;

			//!	"Cue
			static  eMarkerType const kMTCue;

			//! "Beat"
			static  eMarkerType const kMTBeat ;

			//! "Track"
			static  eMarkerType const kMTTrack;

			//! "Index"
			static  eMarkerType const kMTIndex;

			//! "Speech"
			static  eMarkerType const kMTSpeech;

			//! "InOut"
			static  eMarkerType const kMTSubClip;

			//!  "Segmentation"
			static  eMarkerType const kMTSegmentation;

			//!"AlertVideoTooLow"
			static  eMarkerType const kMTAlertVideoTooLow;

			//! "AlertVideoTooHigh"	
			static  eMarkerType const kMTAlertVideoTooHigh;

			//! "AlertAudioClippingLeft"
			static  eMarkerType const kMTAlertAudioClippingLeft;

			//! "AlertAudioClippingRight"
			static  eMarkerType const kMTAlertAudioClippingRight;

			//! "AlertAudioLowVolumeLeft"
			static  eMarkerType const kMTAlertAudioLowVolumeLeft;

			//! "AlertAudioLowVolumeRight"
			static  eMarkerType const kMTAlertAudioLowVolumeRight;

			//! "AlertAudioLoudNoiseLeft"
            static  eMarkerType const kMTAlertAudioLoudNoiseLeft;

			//! "AlertAudioLoudNoiseRight"
			static  eMarkerType const kMTAlertAudioLoudNoiseRight;

			//! "AlertAudioHummingLeft"
			static  eMarkerType const kMTAlertAudioHummingLeft;

			//! "AlertAudioHummingRight"
            static  eMarkerType const kMTAlertAudioHummingRight;

			//! "AlertAudioNoSignalLeft"
			static eMarkerType const kMTAlertAudioNoSignalLeft;

				//! "AlertAudioNoSignalRight"
			static  eMarkerType const kMTAlertAudioNoSignalRight;

				//! "Navigation"
			static  eMarkerType const kMTFLVCuePoint_Navigation;

			//! "Event"
			static  eMarkerType const kMTFLVCuePoint_Event;

			//! "For user defined/custom markers"
			static  eMarkerType const kMTUserDefinedMarker;

			//! Maximum value of the enum
			static  eMarkerType const kMTMax;

		//!
		//! \brief Returns the name of the marker
		//! \return a shared pointer of \#NS_XMPCOMMON::IUTF8String containing
		//! the marker name
		//! 
		virtual spcIUTF8String APICALL GetName() const = 0;

		////!
		////! \brief Sets the name of the marker
		////! \param[in] markerName a const char * which would be the name that has to be assigned to the marker 
		////! \attention Warning is thrown in case
		////!		-# markerName supplied is a null pointer
		////!

		virtual void  APICALL SetName(const char * markerName, sizet markerNameLen) = 0;

		////!
		////! \brief Returns  the guid of the marker
		////! \return a shared pointer of \#NS_XMPCOMMON::IUTF8String containing
		////! the guid of the marker
		////! 

		virtual spcIUTF8String APICALL GetGUID() const = 0;

		////!
		////! \brief Sets the comment of the marker, also adds kMTComment to the markerTypes if not already present
		////! \param[in] markerComment a const char * which would be the name that has to be assigned to the marker 
		////! \return a bool which return true if markerComment parameter provided was non NULL, otherwise false is returned.
		////! \attention Error is thrown in case
		////!		-# markerComment supplied is a null pointer
		////!

		virtual void  APICALL SetComment(const char * markerComment, sizet markerCommentLen) = 0;

		////!
		////! \brief Sets speaker of the marker, also adds kMTSpeech to the markerTypes if not already present
		////! \param[in] markerSpeaker a const char * which would be the name that has to be assigned to the marker 
		////! \return a bool which return true if markerSpeaker parameter provided was non NULL, otherwise false is returned.
		////! \attention Warning is thrown in case
		////!		-# markerSpeaker supplied is a null pointer
		////!
		virtual void APICALL SetSpeaker(const char * markerSpeaker, sizet markerSpeakerLen) = 0;

		////!
		////! \brief Adds a cue point param with the specified key and value to the marker.
		////! \param[in] inputKey pointer to a null terminated constant char buffer containing key to be inserted in the cue point param
		////! \param[in] inputValue pointer to a null terminated constant char buffer containing value to be inserted in the cue point param
		////! \return a sizet which will represent the size of the list after addition of the cue point param.
		////!

		virtual sizet APICALL AddCuePointParam(const char * inputKey, sizet inputKeyLen,  const char *inputValue, sizet inputValueLen) = 0;

		////!
		////! \brief Returns the comment of the marker
		////! \return a shared pointer of \#NS_XMPCOMMON::IUTF8String containing
		////! the marker comment
		////! 
		virtual spcIUTF8String APICALL GetComment() const = 0;

		////!
		////! \brief Returns the name of the marker
		////! \return a shared pointer of \#NS_XMPCOMMON::IUTF8String containing
		////! the marker location
		////! 
		virtual spcIUTF8String APICALL GetLocation() const = 0;

		////!
		////! \brief Returns the speaker of the marker
		////! \return a shared pointer of \#NS_XMPCOMMON::IUTF8String containing
		////! the marker speaker
		////! 
		virtual spcIUTF8String APICALL GetSpeaker() const = 0;

		////!
		////! \brief Returns  the target of the marker
		////! \return a shared pointer of \#NS_XMPCOMMON::IUTF8String containing
		////! the target of the marker
		////!

		virtual spcIUTF8String APICALL GetTarget() const = 0;


		////!
		////! \brief Sets target of the marker, also adds kMTWebLink to the markerTypes if not already present
		////! \param[in] markerTarget a const char * which would be the name that has to be assigned to the marker 
		////! \return a bool which return true if markerTarget parameter provided was non NULL, otherwise false is returned.
		////! \attention Warning is thrown in case
		////!		-# markerTarget supplied is a null pointer
		////!
		virtual void APICALL SetTarget(const char * target, sizet targetLen) = 0;


		////!
		////! \brief Sets location of the marker, also adds kMTWebLink to the markerTypes if not already present
		////! \param[in] markerLocation a const char * which would be the name that has to be assigned to the marker 
		////! \return a bool which return true if markerLocation parameter provided was non NULL, otherwise false is returned.
		////! \attention Warning is thrown in case
		////!		-# markerLocation supplied is a null pointer
		////!
		virtual void APICALL SetLocation(const char * location, sizet locationLen) = 0;

		////!
		////! \brief Sets probability of the marker, also adds kMTSpeech to the markerTypes if not already present
		////! \param[in] markerProbability - a double which would be the probability that has to be assigned to the marker 
		////! \return a bool which return true if markerProbability parameter provided was between 0 and 1 (both included), otherwise false is returned.
		////! \attention Warning is thrown in case
		////!		-# markerProbability supplied is a null pointer
		////!

		virtual void APICALL SetProbability(double markerProbability) = 0;


		////!
		////! \brief Returns the marker probability
		////! \return a double which will be the current value of the marker probability
		////! \attention 0.0 will be returned if marker probability has not been set yet

		virtual double APICALL GetProbability() const = 0;

		////!
		////! \brief Virtual copy constructor
		////! Clones the marker creating an exact replica of the marker which is not part of any track. A new GUID will be generated and provided to the new marker
		////! \param[in] multiThreadingSupport a value of type \#NS_XMPCOMMON::eMultiThreadingSupport controlling whether
		////! the new object created should be thread safe or not. By default new object created will inherit its setting from
		////! the object begin cloned.
		////! \return - a shared pointer to the new marker
		////!

		virtual spIMarker APICALL Clone() const = 0;

		////!
		////! \brief Set the duration of time slice in terms of Frame Counts.
		////! \param[in] startFrameCount a value of \#NS_XMPCOMMON::uint64 containing the frame count of first frame included in
		////! the time slice.
		////! \param[in] countOfFrames a value of \#NS_XMPCOMMON::uint64 containing the count of frames in the time slice.
		////!

		virtual void APICALL SetFrameCountSpan(uint64 startFrameCount = 0, uint64 countOfFrames = Max_XMP_Uns64) = 0;

		////!
		////! \brief Set the duration of time slice in terms of seconds.
		////! \param[in] startTimeInSeconds a value of double containing the starting time of the first frame included in the
		////! time slice.
		////! \param[in] durationInSeconds a value of double containing the duration of the frames in the time slice.
		////! \return a bool value indicating whether the operation was successful or not.
		////! \attention returns false in case startTimeInSeconds or durationInSeconds is negative.
		////!

		virtual void APICALL SetTimeSpan(double startTimeInSeconds = 0, double durationInSeconds = DBL_MAX) = 0;

		////!
		////! \brief Get the duration of time slice in terms of seconds.
		////! \param[out] startTimeInSeconds a value of double filled with the starting time of the first frame included in the
		////! time slice.
		////! \param[in] durationInSeconds a value of double filled with the duration of the frames in the time slice.
		////!

		virtual void APICALL GetTimeSpan(double & startTimeInSeconds, double & durationInSeconds) const = 0;

		////!
		////! \brief Get the duration of the time slice in terms of Frame Counts.
		////! \param[in] startFrameCount a value of \#NS_XMPCOMMON::uint64 filled with the frame count of first frame included in
		////! the time slice.
		////! \param[in] countOfFrames a value of \#NS_XMPCOMMON::uint64 filled with the count of frames in the time slice.
		////!
		virtual void APICALL GetFrameCountSpan(uint64 & startFrameCount, uint64 & countOfFrames) const = 0;

		////!
		////! \brief Get the frame rate associated with the start frame count.
		////! \param[out] numerator a value of \#NS_XMPCOMMON::uint64 filled with the total number of frames in number of seconds
		////! denoted by denominator.
		////! \param[out] denominator a value of \#NS_XMPCOMMON::uint64 filled with the number of seconds it takes for number of
		////! frames denoted by numerator to be played.
		////!

		virtual void APICALL GetFrameRateForStartFrameCount(uint64 & numerator, uint64 & denominator) const = 0;

		////!
		////! \brief Get the frame rate associated with the count of Frames.
		////! \param[out] numerator a value of \#NS_XMPCOMMON::uint64 filled with the total number of frames in number of seconds
		////! denoted by denominator.
		////! \param[out] denominator a value of \#NS_XMPCOMMON::uint64 filled with the number of seconds it takes for number of
		////! frames denoted by numerator to be played.
		////!

		virtual void APICALL GetFrameRateForCountOfFrames(uint64 & numerator, uint64 & denominator) const = 0;

		////!
		////! \brief Set the frame rate associated with the startFrameCount.
		////! \param[in] numerator a value of \#NS_XMPCOMMON::uint64 containing the total number of frames in number of seconds
		////! denoted by denominator.
		////! \param[in] denominator a value of \#NS_XMPCOMMON::uint64 containing the number of seconds it takes for number of
		////! frames denoted by numerator to be played.
		////! \attention throws error in case numerator, denominator or both are zero.
		////!

		virtual void APICALL SetFrameRateForStartFrameCount(uint64 numerator = 1, uint64 denominator = 1) = 0;

		////!
		////! \brief Set the same frame rate associated with the count of frames.
		////! \param[in] numerator a value of \#NS_XMPCOMMON::uint64 containing the total number of frames in number of seconds
		////! denoted by denominator.
		////! \param[in] denominator a value of \#NS_XMPCOMMON::uint64 containing the number of seconds it takes for number of
		////! frames denoted by numerator to be played.
		////! \attention throws error in case numerator, denominator or both are zero.
		////!

		virtual void APICALL SetFrameRateForCountOfFrames(uint64 numerator = 1, uint64 denominator = 1) = 0;

		////!
		////! \brief Set the same frame rate associated with the start frame count as well as count of frames.
		////! \param[in] numerator a value of \#NS_XMPCOMMON::uint64 containing the total number of frames in number of seconds
		////! denoted by denominator.
		////! \param[in] denominator a value of \#NS_XMPCOMMON::uint64 containing the number of seconds it takes for number of
		////! frames denoted by numerator to be played.
		////! \attention throws error in case numerator, denominator or both are zero.
		////!

		virtual void  APICALL SetFrameRates(uint64 numerator = 1, uint64 denominator = 1) = 0;


		////!
		////! \brief Removes the cue point params which have the key equal to inputKey and the value equal to inputValue.If the inputValue provided is NULL, 
		////! all cuePointParms matching the given inputKey will be removed.If both the inputKey and inputValue are NULL, all cuePointParams will be removed 
		////! \param[in] inputKey a value of cons char * containing the key for which the cue point params are to be removed.
		////! \param[in] inputValue a value of const char * containg the value for which the cue point params are to be removed having key equal to inputKey
		////! \return sizet the resultant size of the cue point params after removing the matched cue point params
		////!

		virtual sizet APICALL RemoveCuePointParams(const char * inputKey, sizet inputKeyLen, const char * inputValue = NULL, sizet inputValueLen = 0) = 0;

		////!
		////! \brief Returns the list of the cue point params which have the key equal to inputKey.
		////! \param[in] inputKey a value of const char * containing the key for which the cue point params are to be obtained. if inputKey provided is NULL
		////! \return std::vector of shared_ptr of NS_XMPCommon::IUTF8String which will represent the list of the values.
		////!

		virtual spcIUTF8StringList APICALL GetCuePointParams(const char * inputKey, sizet inputKeyLen) const = 0;

		////!
		////! \brief Returns the list of all cue point params  in the marker
		////! \return a std::vector of std:pair of shared_ptr of NS_XMPCommon:IUTF8String, with the first element of the pair 
		////! containing the key of the cue point param and the second element of the cue point param containing the value of the cue point param
		////!

		virtual spcICuePointParamList APICALL GetAllCuePointParams() const = 0;

		////!
		////! \brief Sets marker types, mask should be a bitwise OR of predefined marker types.
		////! \param[in] inputMask uint64 - which should be bitwise OR of predefined marker types. 
		////! \return uint64 which will be the bitwise OR of the marker types after the adding types specified by inputMask
		////!

		virtual uint64 APICALL SetTypes(uint64 markerTypes) = 0;


		////!
		////! \brief Sets marker types, mask should be a bitwise OR of predefined marker types.
		////! \param[in] inputMask uint64 - which should be bitwise OR of predefined marker types. 
		////! \return uint64 which will be the bitwise OR of the marker types after the adding types specified by inputMask
		////!

		virtual eMarkerType APICALL RemoveTypes(eMarkerType markerTypes) = 0;

		////!
		////! \brief Returns the bitwise OR of the predefined marker types associated with the marker. 
		////! \return uint64 - it will be the bitwise OR of the marker types currently associated with the marker. 
		////! If marker contains a user-defined type, the return value will also have the KMTUserDefined bit set.
		////!

		virtual uint64 APICALL GetTypes() const = 0;

		////!
		////! \brief Appends userDefinedType to the marker type if it is already not present in the markerType
		////! \param[in] userDefinedType const char * pointing to a user defined marker type name.  if userDefinedType is NULL, false is returned. 
		////! \return bool If marker already contained userDefinedType in the markerType or userDefinedType is NULL, false is returned, otherwise true is returned.
		////!

		virtual bool APICALL AppendCustomType(const char * customType, sizet customTypeLen) = 0;

		////!
		////! \brief Removes userDefinedType to the marker type if it is present in the markerType
		////! \param[in] userDefinedType const char * pointing to a user defined marker type name.  if userDefinedType is NULL, false is returned. 
		////! \return bool If marker doesn't contain userDefinedType in the markerType or userDefinedType is NULL, false is returned, otherwise true is returned.
		////!

		virtual bool APICALL RemoveCustomType(const char  * customType, sizet customTypeLen) = 0;

		////!
		////! \brief Returns number of user-defined/custom types which have been assigned to the marker
		////! \return sizet - The number of user-defined/custom types which have been assigned to the marker
		////!

		virtual sizet APICALL GetCustomTypeCount() const = 0;


		////!
		////! \brief Returns the list of user defined types associated with the marker
		////! \return std::vector of shared_ptr of NS_XMPCOMMON::IUTF8String containing user defined marker types associated with the vector, empty vector will be returned 
		////!	if the marker is not user defined
		////!

		virtual spcIUTF8StringList APICALL GetCustomTypes() const = 0;

		////!
		////!  \brief Adds an extension with the specified namespace and name to the marker 
		////! \param[in] extensionNameSpace pointer to a null terminated constant char buffer containing namespace of the extension to be added
		////! \param[in] extensionNameSpaceLen length of the extensionNameSpace
		////! \param[in] extensionName pointer to a null terminated constant char buffer containing name of the extension to be added
		////! \param[in] extensionNameLen length of the extensionName
		////! \param[in] pIExtension a pointer to an IExtension object
		////! \return a shared_ptr to NS_XMPCORE::IExtension object which will be the extension that was present in the marker
		////! 

		virtual bool APICALL  AddExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtension extension) = 0;

		////!
		////! \brief Returns the extension with the specified namespace and name (If present) from the marker 
		////! \param[in] extensionNameSpace pointer to a null terminated constant char buffer containing namespace of the extension to be returned
		////! \param[in] extensionNameSpaceLen length of the extensionNameSpace
		////! \param[in] extensionName pointer to a null terminated constant char buffer containing name of the extension to be returned
		////! \param[in] extensionNameLen length of the extensionName
		////! \return a shared_ptr to NS_XMPCORE::IExtension object which will be the extension that is present in the marker
		////! \attention an empty pointer will be returned if marker doesn't contain an extension with the provided namespace and name or if namespace or name provided is NULL
		////!

		virtual pIExtension APICALL GetExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) const = 0;


		////!
		////! \brief Removes all extensions present in the marker
		////! \return returns number of extensions initially present in the marker
		////!

		virtual sizet APICALL RemoveAllExtensions() __NOTHROW__ = 0;


		////!
		////! \brief Returns the number of the extensions present in the marker
		////! \return a sizet which will be equal to number of extensions currently present in the marker structure
		////!  0 will be return if there is no extension node

		virtual sizet APICALL GetExtensionCount() const __NOTHROW__ = 0;

		////!
		////! \brief Returns true if the extension with the specified namespace and name is present in the marker
		////! \param[in] extensionNameSpace pointer to a null terminated constant char buffer containing namespace of the extension to be inserted
		////! \param[in] extensionNameSpaceLen length of the extensionNameSpace
		////! \param[in] extensionName pointer to a null terminated constant char buffer containing name of the extension to be inserted
		////! \param[in] extensionNameLen length of the extensionName
		////! \return sizet the number of extensions present in the marker
		////!

		virtual bool APICALL HasExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) const __NOTHROW__ = 0;


		///!
		////! \brief Removes an extension with the specified namespace and name (If present) from the marker 
		////! \param[in] extensionNameSpace pointer to a null terminated constant char buffer containing namespace of the extension to be removed
		////! \param[in] extensionNameSpaceLen length of the extensionNameSpace
		////! \param[in] extensionName pointer to a null terminated constant char buffer containing name of the extension to be removed
		////! \param[in] extensionNameLen length of the extensionName
		////! \return a shared_ptr to NS_XMPCORE::IExtension object which will be the extension that was present in the marker
		////! \attention an empty pointer will be returned if marker doesn't contain an extension with the provided namespace and name or if namespace or name provided is NULL
		////!

		virtual sizet APICALL RemoveExtension(const char  * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) = 0;



		////!
		////! \brief Removes all cuePointParams
		////! \return sizet the resultant size of the cue point params before removing the matched cue point params
		////!

		virtual sizet APICALL  RemoveAllCuePointParams() = 0;

		////!
		////! \brief Returns true if the customType is associated with the marker
		////! \param[in] customType const char * pointing to a custom marker type name.  if customType is NULL, false is returned.
		////! \param[in] customTypeLen - length of the customType
		////! \return bool - true if customType is associated with the marker, false otherwise
		////!
		virtual bool APICALL HasCustomType(const char * customType, sizet customTypeLen) const = 0;

		////!
		////! \brief Returns true if the markerTypeMask is associated with the marker
		////! \param[in] markerTypeMask eMarkerType the logical OR of prefined marker types that are to be checked in the marker
		////! \return bool - true if markerTypeMask is associated with the marker, false otherwise
		////!

		virtual bool APICALL HasTypes(eMarkerType  markerTypeMask) const = 0;

		XMP_PRIVATE static uint64 GetInterfaceID() { return kIMarkerID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

		XMP_PRIVATE static spIMarker MakeShared(pIMarker_base ptr);
		XMP_PRIVATE static spcIMarker MakeShared(pcIMarker_base ptr) {
			return MakeShared(const_cast<pIMarker_base>(ptr));
		}

		virtual pIMarker APICALL GetActualIMarker() __NOTHROW__ = 0;
		XMP_PRIVATE pcIMarker GetActualIMarker() const __NOTHROW__{
			return const_cast<IMarker_v1 *>(this)->GetActualIMarker();
		}

		virtual AdobeXMPAM_Int::pIMarker_I APICALL GetIMarker_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcIMarker_I GetIMarker_I() const __NOTHROW__{
			return const_cast<IMarker_v1 *>(this)->GetIMarker_I();
		}


	protected:

		virtual pcIUTF8String_base APICALL getName(pcIError_base &error) const = 0;

		virtual void APICALL setName(const char * name, sizet nameLen, pcIError_base & error) = 0;

		virtual uint64 APICALL setTypes(uint64 inputMask, pcIError_base & error) = 0;

		virtual eMarkerType APICALL removeTypes(eMarkerType inputMask, pcIError_base & error) = 0;

		virtual uint64 APICALL getTypes(pcIError_base & error) const = 0;

		virtual bool APICALL appendCustomType(const char * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error) = 0;

		virtual bool APICALL removeCustomType(const char  * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error) = 0;

		virtual sizet APICALL getCustomTypeCount(pcIError_base & error) const = 0;

		virtual sizet APICALL getCustomTypes(pcIUTF8String_base *& array, MemAllocatorFunc allocFunc,
			pcIError_base & error) const = 0;

		virtual sizet APICALL getCuePointParams(const char * inputKey, sizet inputKeyLen, pcIUTF8String_base *& array, MemAllocatorFunc allocFunc,
			pcIError_base & error) const = 0;

		virtual sizet APICALL removeCuePointParams(const char * inputKey, sizet inputKeyLen, const char * inputValue, sizet inputValueLen, pcIError_base & error) = 0;

		virtual sizet APICALL addCuePointParam(const char * inputKey, sizet inputKeyLen, const char * inputValue, sizet inputValueLen, pcIError_base & error) = 0;

		virtual sizet APICALL getAllCuePointParams(pcIUTF8String_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error) const = 0;

		virtual pIMarker_base APICALL clone(pcIError_base & error) const = 0;

		virtual void APICALL setFrameCountSpan(uint64 startFrameCount, uint64 countOfFrames, pcIError_base& error) = 0;


		virtual void APICALL setTimeSpan(double startTimeInSeconds, double durationInSeconds, pcIError_base & error) = 0;

		virtual void APICALL setFrameRateForStartFrameCount(uint64 numerator, uint64 denominator, pcIError_base & error) = 0;

		virtual void APICALL setFrameRateForCountOfFrames(uint64 numerator, uint64 denominator, pcIError_base & error) = 0;

		virtual void APICALL setFrameRates(uint64 numerator, uint64 denominator, pcIError_base & error) = 0;

		virtual void APICALL setProbability(double markerProbability, pcIError_base & error) = 0;

		virtual pcIUTF8String_base APICALL getComment(pcIError_base &error) const = 0;
	
		virtual pcIUTF8String_base APICALL getLocation(pcIError_base &error) const = 0;

		virtual pcIUTF8String_base APICALL getSpeaker(pcIError_base &error) const = 0;
	
		virtual pcIUTF8String_base APICALL getTarget(pcIError_base &error) const = 0;

		virtual pcIUTF8String_base APICALL  getGUID(pcIError_base &error) const = 0;

		virtual void APICALL setLocation(const char * location, sizet locationLen, pcIError_base & error) = 0;

		virtual void APICALL setSpeaker(const char * speaker, sizet speakerLen, pcIError_base & error) = 0;

		virtual void APICALL setTarget(const char * target, sizet targetLen, pcIError_base & error) = 0;

		virtual void APICALL setComment(const char * name, sizet commentLen, pcIError_base & error) = 0;

		virtual uint32 APICALL addExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtension extension, pcIError_base & error) = 0;

		virtual pIExtension APICALL getExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pcIError_base & error) const = 0;

		virtual uint32 APICALL hasExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pcIError_base & error) const = 0;

		virtual sizet APICALL removeExtension(const char  * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pcIError_base & error) = 0;

		virtual uint32 APICALL hasCustomType(const char * customType, sizet customTypeLen, pcIError_base & error) const = 0;

		public:
		//!
		//! \brief protected Virtual Destructor
		//!
		virtual ~IMarker_v1() __NOTHROW__ = 0;
#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
#endif
		REQ_FRIEND_CLASS_DECLARATION();

	};
	inline IMarker_v1::~IMarker_v1() __NOTHROW__{}




}

#endif //__IMarker_h__
