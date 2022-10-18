// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================
namespace AdobeXMPAM {
	class IMarkerProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IMarkerProxy;

#include "XMPMarker/Interfaces/IMarker.h"


namespace AdobeXMPAM {
    
#ifndef MarkerTypesDefined
#define MarkerTypesDefined 1
    
    
    
    const IMarker::eMarkerType IMarker::kMTComment = 1 << 0;
    
    
    //! "Chapter"
    const IMarker::eMarkerType   IMarker::kMTChapter = 1 << 1;
    
    //!	"FLVCuePoint"
    IMarker::eMarkerType const  IMarker::kMTFLVCuePoint = 1 << 2;
    
    //! "WebLink"
    IMarker::eMarkerType const  IMarker::kMTWebLink = 1 << 3;
    
    //!	"Cue"
    IMarker::eMarkerType const  IMarker::kMTCue = 1 << 4;
    
    //! "Beat"
    IMarker::eMarkerType const  IMarker::kMTBeat = 1 << 5;
    
    //! "Track"
    IMarker::eMarkerType const IMarker::kMTTrack = 1 << 6;
    
    //! "Index"
    IMarker::eMarkerType const  IMarker::kMTIndex = 1 << 7;
    
    //! "Speech"
    IMarker::eMarkerType const  IMarker::kMTSpeech = 1 << 8;
    
    //! "InOut"
    IMarker::eMarkerType const  IMarker::kMTSubClip = 1 << 9;
    
    //!  "Segmentation"
    IMarker::eMarkerType const  IMarker::kMTSegmentation = 1 << 10;
    
    //!"AlertVideoTooLow"
    IMarker::eMarkerType const  IMarker::kMTAlertVideoTooLow = 1 << 11;
    
    //! "AlertVideoTooHigh"
    IMarker::eMarkerType const  IMarker::kMTAlertVideoTooHigh = 1 << 12;
    
    //! "AlertAudioClippingLeft"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioClippingLeft = 1 << 13;
    
    //! "AlertAudioClippingRight"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioClippingRight = 1 << 14;
    
    //! "AlertAudioLowVolumeLeft"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioLowVolumeLeft = 1 << 15;
    
    //! "AlertAudioLowVolumeRight"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioLowVolumeRight = 1 << 16;
    
    //! "AlertAudioLoudNoiseLeft"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioLoudNoiseLeft = 1 << 17;
    
    //! "AlertAudioLoudNoiseRight"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioLoudNoiseRight = 1 << 18;
    
    //! "AlertAudioHummingLeft"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioHummingLeft = 1 << 19;
    
    //! "AlertAudioHummingRight"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioHummingRight = 1 << 20;
    
    //! "AlertAudioNoSignalLeft"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioNoSignalLeft = 1 << 21;
    
    //! "AlertAudioNoSignalRight"
    IMarker::eMarkerType const  IMarker::kMTAlertAudioNoSignalRight = 1 << 22;
    
    //! "Navigation"
    IMarker::eMarkerType const  IMarker::kMTFLVCuePoint_Navigation = 1 << 23;
    
    //! "Event"
    IMarker::eMarkerType const  IMarker::kMTFLVCuePoint_Event = 1 << 24;
    
    //! "For user defined/custom markers"
    IMarker::eMarkerType const  IMarker::kMTUserDefinedMarker = 1 << 25;
    
    //! Maximum value of the enum
    IMarker::eMarkerType const  IMarker::kMTMax = -1;
#endif
}

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPExtensions/include/IExtensionsObjectFactory.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCore/Interfaces/IMetadata.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPMarker/XMPMarkerFwdDeclarations.h"
#include <cassert>

namespace AdobeXMPAM {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif





	class IMarkerProxy : public virtual IMarker {
		//! \cond XMP_INTERNAL_DOCUMENTATION
	private:
		pIMarker mRawPtr;
	public:

		IMarkerProxy(pIMarker ptr) : mRawPtr(ptr) {
			mRawPtr->Acquire();
		}

		~IMarkerProxy() {
			mRawPtr->Release();
		}

		pIMarker APICALL GetActualIMarker() __NOTHROW__{
			return mRawPtr;
		}

			AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{
			return mRawPtr->GetISharedObject_I();
		}

			AdobeXMPAM_Int::pIMarker_I APICALL GetIMarker_I() __NOTHROW__{
			return mRawPtr->GetIMarker_I();
		}

			pvoid APICALL getInterfacePointer(uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error) __NOTHROW__{
			assert(false);
			return mRawPtr->getInterfacePointer(interfaceID, interfaceVersion, error);
		}

			pvoid APICALL GetInterfacePointer(uint64 interfaceID, uint32 interfaceVersion) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion);
		}

		void APICALL Acquire() const __NOTHROW__{
			assert(false);
		}

			void APICALL Release() const __NOTHROW__{
			assert(false);
		}

			/*virtual pcIUTF8String_base getName(pcIError_base &error) const;
			virtual spcIUTF8String GetName() const;
			virtual void setName( const char * name, sizet nameLen, pcIError_base & error);
			virtual void  SetName(const char * markerName, sizet markerNameLen);
			virtual uint64 removeTypes(eMarkerType inputMask, pcIError_base & error);
			virtual uint64 RemoveTypes(eMarkerType inputMask);
			virtual uint64 setTypes(uint64 inputMask, pcIError_base & error);
			virtual uint64 SetTypes(uint64 inputMask) ;
			virtual uint64 GetTypes() const ;
			virtual uint64 getTypes(pcIError_base & error) const ;
			virtual bool AppendCustomType(const char * userDefinedType, sizet userDefinedTypeLen );
			virtual bool appendCustomType(const char * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error);
			virtual bool RemoveCustomType(const char  * userDefinedType, sizet userDefinedTypeLen);
			virtual bool removeCustomType(const char  * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error);
			virtual sizet GetCustomTypeCount() const;
			virtual sizet getCustomTypeCount(pcIError_base & error) const;
			virtual spcIUTF8StringList GetCustomTypes() const;
			virtual sizet getCustomTypes(pcIUTF8String_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error) const;
			virtual spcIUTF8StringList GetCuePointParams(const char * inputKey, sizet inputKeyLen) const;
			virtual sizet getCuePointParams(const char * inputKey, sizet inputKeyLen, pcIUTF8String_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error) const;
			virtual sizet RemoveCuePointParams(const char * inputKey, sizet inputKeyLen, const char * inputValue = NULL, sizet inputValueLen = 0);
			virtual sizet removeCuePointParams(const char * inputKey, sizet inputKeyLen, const char * inputValue, sizet inputValueLen, pcIError_base & error);
			virtual sizet AddCuePointParam(const char * inputKey, sizet inputKeyLen, const char *inputValue, sizet inputValueLen);
			virtual sizet addCuePointParam(const char * inputKey, sizet inputKeyLen, const char *inputValue, sizet inputValueLen, pcIError_base & error);

			virtual spcICuePointParamList GetAllCuePointParams() const;
			virtual sizet getAllCuePointParams( pcIUTF8String_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error ) const;

			virtual spIMarker Clone() const;
			virtual pIMarker_base clone(pcIError_base & error) const;
			virtual void SetFrameCountSpan(uint64 startFrameCount = 0, uint64 countOfFrames = Max_XMP_Uns64) ;
			virtual void setFrameCountSpan(uint64 startFrameCount, uint64 countOfFrames, pcIError_base& error) ;
			virtual void GetTimeSpan(double & startTimeInSeconds, double & durationInSeconds) const;
			virtual void GetFrameCountSpan(uint64 & startFrameCount, uint64 & countOfFrames) const;
			virtual void GetFrameRateForStartFrameCount(uint64 & numerator, uint64 & denominator) const;
			virtual void GetFrameRateForCountOfFrames(uint64 & numerator, uint64 & denominator) const;

			virtual void setTimeSpan(double startTimeInSeconds, double durationInSeconds, pcIError_base & error) ;

			virtual void SetTimeSpan(double startTimeInSeconds = 0, double durationInSeconds = DBL_MAX);

			virtual void setFrameRateForStartFrameCount(uint64 numerator, uint64 denominator, pcIError_base & error);

			virtual void setFrameRateForCountOfFrames(uint64 numerator, uint64 denominator, pcIError_base & error);

			virtual void setFrameRates(uint64 numerator, uint64 denominator, pcIError_base & error);

			virtual void SetFrameRateForStartFrameCount(uint64 numerator, uint64 denominator);

			virtual void SetFrameRateForCountOfFrames(uint64 numerator, uint64 denominator);

			virtual void SetFrameRates(uint64 numerator, uint64 denominator) ;

			virtual double GetProbability() const;

			virtual void setProbability(double markerProbability, pcIError_base & error);

			virtual void SetProbability(double markerProbability);

			virtual pcIUTF8String_base getComment(pcIError_base &error) const;

			virtual pcIUTF8String_base getLocation(pcIError_base &error) const;

			virtual pcIUTF8String_base getSpeaker(pcIError_base &error) const;

			virtual pcIUTF8String_base getTarget(pcIError_base &error) const;

			virtual spcIUTF8String GetComment() const;

			virtual spcIUTF8String GetLocation() const;

			virtual spcIUTF8String GetSpeaker() const;

			virtual spcIUTF8String GetTarget() const;

			virtual pcIUTF8String_base getGUID(pcIError_base &error) const;

			virtual spcIUTF8String GetGUID() const;

			virtual void setLocation(const char * location, sizet locationLen, pcIError_base & error);

			virtual void setSpeaker(const char * name, sizet speakerLen, pcIError_base & error) ;

			virtual void setTarget(const char * target, sizet targetLen, pcIError_base & error) ;

			virtual void setComment(const char * comment, sizet commentLen, pcIError_base & error);

			virtual void  SetLocation(const char * location, sizet locationLen);

			virtual void  SetSpeaker(const char * speaker, sizet speakerLen);

			virtual void  SetTarget(const char * target, sizet targetLen);

			virtual void  SetComment(const char * comment, sizet CommentLen);

			virtual bool HasTypes(uint64  markerTypeMask) const;

			virtual bool AddExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtension extension);

			virtual uint32 addExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtension extension, pcIError_base & error);

			virtual pIExtension getExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pcIError_base & error) const;

			virtual pIExtension GetExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) const;

			virtual sizet RemoveAllExtensions() __NOTHROW__;

			virtual sizet GetExtensionCount() const __NOTHROW__;

			virtual bool HasExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) const __NOTHROW__;

			virtual uint32 hasExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pcIError_base & error) const;

			virtual sizet removeExtension(const char  * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pcIError_base & error);

			virtual sizet RemoveExtension(const char  * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen);

			virtual sizet RemoveAllCuePointParams();

			virtual uint32 hasCustomType(const char * customType, sizet customTypeLen, pcIError_base & error) const;

			virtual bool HasCustomType(const char * customType, sizet customTypeLen) const;*/
			//! \endcond



			AdobeXMPCommon::spcIUTF8String  APICALL GetName() const {
			return CallConstSafeFunctionReturningPointer< IMarker, pcIUTF8String_base, const IUTF8String>(mRawPtr, &IMarker::getName);
		}

			AdobeXMPCommon::pcIUTF8String_base APICALL getName(pcIError_base & error) const {

			return mRawPtr->getName(error);
		}

			void APICALL setName(const char * name, sizet nameLen, pcIError_base & error) {

			return mRawPtr->setName(name, nameLen, error);
		}

			void APICALL SetName(const char * markerName, sizet markerNameLen) {

			return CallSafeFunctionReturningVoid<IMarker, const char *, sizet >(mRawPtr, &IMarker::setName, markerName, markerNameLen);

		}


			uint64  APICALL  setTypes(uint64 inputMask, pcIError_base & error) {

			return mRawPtr->setTypes(inputMask, error);
		}

			uint64  APICALL SetTypes(uint64 inputMask) {

			return CallSafeFunction<
				IMarker, uint64, uint64, uint64
			>(mRawPtr, &IMarker::setTypes, inputMask);
		}


			uint64  APICALL removeTypes(eMarkerType inputMask, pcIError_base & error) {

			return mRawPtr->removeTypes(inputMask, error);
		}

			uint64  APICALL RemoveTypes(eMarkerType inputMask) {

			return CallSafeFunction<
				IMarker, uint64, uint64, uint64
			>(mRawPtr, &IMarker::removeTypes, inputMask);
		}

			uint64  APICALL getTypes(pcIError_base & error) const {

			return mRawPtr->getTypes(error);
		}

			uint64  APICALL GetTypes() const {

			return CallConstSafeFunction<
				IMarker, uint64, uint64
			>(mRawPtr, &IMarker::getTypes);
		}


			bool  APICALL AppendCustomType(const char * userDefinedType, sizet userDefinedTypeLen) {

			return CallSafeFunction<
				IMarker, bool, bool, const char *, sizet
			>(mRawPtr, &IMarker::appendCustomType, userDefinedType, userDefinedTypeLen);
		}

			bool  APICALL appendCustomType(const char * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error) {

			return mRawPtr->appendCustomType(userDefinedType, userDefinedTypeLen, error);
		}

			bool  APICALL RemoveCustomType(const char * userDefinedType, sizet userDefinedTypeLen) {

			return CallSafeFunction<
				IMarker, bool, bool, const char *, sizet
			>(mRawPtr, &IMarker::removeCustomType, userDefinedType, userDefinedTypeLen);
		}

			bool  APICALL removeCustomType(const char * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error) {

			return mRawPtr->removeCustomType(userDefinedType, userDefinedTypeLen, error);
		}

			sizet  APICALL GetCustomTypeCount() const {

			return CallConstSafeFunction<
				IMarker, sizet, sizet
			>(mRawPtr, &IMarker::getCustomTypeCount);
		}

			sizet  APICALL getCustomTypeCount(pcIError_base & error) const {

			return mRawPtr->getCustomTypeCount(error);
		}

		static void * MyMalloc(sizet size) { return malloc(size); }

		static spcIUTF8StringList CreateStringList(pcIUTF8String_base * ptr, sizet nElements) {
			spcIUTF8StringList list = shared_ptr<cIUTF8StringList>(
				new cIUTF8StringList());
			list->reserve(nElements);
			for (sizet i = 0; i < nElements; i++) {
				auto sp = AdobeXMPCommon::IUTF8String::MakeShared(ptr[i]);
				list->push_back(sp);
			}
			free(ptr);
			return list;
		}

		spcIUTF8StringList  APICALL GetCustomTypes() const {
			pcIUTF8String_base * ptr(NULL);
			sizet nElements = CallConstSafeFunction<
				IMarker, sizet, sizet, pcIUTF8String_base *&, MemAllocatorFunc
			>(mRawPtr, &IMarker::getCustomTypes, ptr, &MyMalloc);
			return CreateStringList(ptr, nElements);
		}

		sizet  APICALL getCustomTypes(pcIUTF8String_base *& array, MemAllocatorFunc allocFunc,
			pcIError_base & error) const __NOTHROW__
		{
			return mRawPtr->getCustomTypes(array, allocFunc, error);
		}

			spcIUTF8StringList  APICALL GetCuePointParams(const char * inputKey, sizet inputKeyLen) const {
			pcIUTF8String_base * ptr(NULL);
			sizet nElements = CallConstSafeFunction<
				IMarker, sizet, sizet, const char *, sizet, pcIUTF8String_base *&, MemAllocatorFunc
			>(mRawPtr, &IMarker::getCuePointParams, inputKey, inputKeyLen, ptr, &MyMalloc);
			return CreateStringList(ptr, nElements);
		}

		sizet  APICALL getCuePointParams(const char * inputKey, sizet inputKeyLen, pcIUTF8String_base *& array, MemAllocatorFunc allocFunc,
			pcIError_base & error) const  {
			return mRawPtr->getCuePointParams(inputKey, inputKeyLen, array, allocFunc, error);
		}

		sizet  APICALL removeCuePointParams(const char * inputKey, sizet inputKeyLen, const char * inputValue, sizet inputValueLen, pcIError_base & error) {

			return mRawPtr->removeCuePointParams(inputKey, inputKeyLen, inputValue, inputValueLen, error);
		}

		sizet  APICALL RemoveCuePointParams(const char * inputKey /* = NULL */, sizet inputKeyLen, const char * inputValue /* = NULL */, sizet inputValueLen) {

			return CallSafeFunction<
				IMarker, sizet, sizet, const char *, sizet, const char *, sizet
			>(mRawPtr, &IMarker::removeCuePointParams, inputKey, inputKeyLen, inputValue, inputValueLen);

		}

		sizet  APICALL addCuePointParam(const char * inputKey, sizet inputKeyLen, const char *inputValue, sizet inputValueLen, pcIError_base & error) {

			return mRawPtr->addCuePointParam(inputKey, inputKeyLen, inputValue, inputValueLen, error);
		}

		sizet  APICALL AddCuePointParam(const char * inputKey, sizet inputKeyLen, const char *inputValue, sizet inputValueLen) {

			return CallSafeFunction<
				IMarker, sizet, sizet, const char *, sizet, const char *, sizet
			>(mRawPtr, &IMarker::addCuePointParam, inputKey, inputKeyLen, inputValue, inputValueLen);

		}


		bool  APICALL AddExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtension extension) {

			return CallSafeFunction<
				IMarker, bool, uint32, const char *, sizet, const char *, sizet, pIExtension
			>(mRawPtr, &IMarker::addExtension, extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen, extension);
		}

		AdobeXMPCommon::uint32  APICALL addExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pIExtension extension, pcIError_base & error) {

			return mRawPtr->addExtension(extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen, extension, error);
		}


		AdobeXMPAM::pIExtension  APICALL getExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pcIError_base & error) const {

			return mRawPtr->getExtension(extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen, error);
		}

		AdobeXMPAM::pIExtension  APICALL GetExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) const {

			return CallConstSafeFunction< IMarker, pIExtension, pIExtension, const char *, sizet, const char *, sizet >(mRawPtr, &IMarker::getExtension, extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen);
		}

		AdobeXMPCommon::sizet  APICALL removeExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pcIError_base & error) {

			return mRawPtr->removeExtension(extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen, error);
		}

		AdobeXMPCommon::sizet  APICALL RemoveExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) {

			return CallSafeFunction< IMarker, sizet, sizet, const char *, sizet, const char *, sizet >(mRawPtr, &IMarker::removeExtension, extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen);

		}

		sizet  APICALL getAllCuePointParams(pcIUTF8String_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error) const {
			return mRawPtr->getAllCuePointParams(array, allocFunc, error);
		}

		spcICuePointParamList  APICALL GetAllCuePointParams() const {
			pcIError_base error(NULL);
			pcIUTF8String_base * array(NULL);
			sizet nElements = mRawPtr->getAllCuePointParams(array, MyMalloc, error);
			if (error) throw error;
			//XMP_Assert( nElements % 2 == 0 );
			spcICuePointParamList list = shared_ptr< cICuePointParamList>(
				new cICuePointParamList());
			list->reserve(nElements / 2);
			for (sizet i = 0; i < nElements; i = i + 2) {
				auto sp1 = AdobeXMPCommon::IUTF8String::MakeShared  (array[i]);
				auto sp2 = AdobeXMPCommon::IUTF8String::MakeShared  (array[i + 1]);
				list->push_back(std::make_pair(sp1, sp2));
			}
			free(array);
			return list;
		}

		spIMarker  APICALL Clone() const{
			return CallConstSafeFunctionReturningPointer<IMarker, pIMarker_base, IMarker>( 
				mRawPtr, &IMarker::clone );
		}

		pIMarker_base  APICALL clone(pcIError_base & error) const {
			return mRawPtr->clone(error);
		}

		void  APICALL SetFrameCountSpan(uint64 startFrameCount /*= 0*/, uint64 countOfFrames /*= Max_XMP_Uns64*/) {

			return CallSafeFunctionReturningVoid<
				IMarker, uint64, uint64
			>(this, &IMarker::setFrameCountSpan, startFrameCount, countOfFrames);
		}

		void  APICALL setFrameCountSpan(uint64 startFrameCount, uint64 countOfFrames, pcIError_base& error) {

			return mRawPtr->setFrameCountSpan(startFrameCount, countOfFrames, error);
		}

		void  APICALL GetTimeSpan(double & startTimeInSeconds, double & durationInSeconds) const {

			return mRawPtr->GetTimeSpan(startTimeInSeconds, durationInSeconds);
		}

		void APICALL  GetFrameCountSpan(uint64 & startFrameCount, uint64 & countOfFrames) const {

			return mRawPtr->GetFrameCountSpan(startFrameCount, countOfFrames);
		}

		void  APICALL GetFrameRateForStartFrameCount(uint64 & numerator, uint64 & denominator) const {

			return mRawPtr->GetFrameRateForStartFrameCount(numerator, denominator);
		}

		void  APICALL GetFrameRateForCountOfFrames(uint64 & numerator, uint64 & denominator) const {

			return mRawPtr->GetFrameRateForCountOfFrames(numerator, denominator);
		}

		void APICALL  setTimeSpan(double startTimeInSeconds, double durationInSeconds, pcIError_base & error) {

			return mRawPtr->setTimeSpan(startTimeInSeconds, durationInSeconds, error);
		}

		void APICALL  SetTimeSpan(double startTimeInSeconds /*= 0*/, double durationInSeconds /*= DBL_MAX*/) {

			CallSafeFunctionReturningVoid< IMarker, double, double>(mRawPtr, &IMarker::setTimeSpan, startTimeInSeconds, durationInSeconds);
		}

		void  APICALL setFrameRateForStartFrameCount(uint64 numerator, uint64 denominator, pcIError_base & error) {

			return mRawPtr->setFrameRateForStartFrameCount(numerator, denominator, error);
		}

		void  APICALL SetFrameRateForStartFrameCount(uint64 numerator, uint64 denominator) {


			return CallSafeFunctionReturningVoid<
				IMarker, uint64, uint64
			>(mRawPtr, &IMarker::setFrameRateForStartFrameCount, numerator, denominator);
		}

		void  APICALL setFrameRateForCountOfFrames(uint64 numerator, uint64 denominator, pcIError_base & error) {

			return mRawPtr->setFrameRateForCountOfFrames(numerator, denominator, error);
		}

		void  APICALL SetFrameRateForCountOfFrames(uint64 numerator, uint64 denominator) {


			return CallSafeFunctionReturningVoid<
				IMarker, uint64, uint64
			>(mRawPtr, &IMarker::setFrameRateForCountOfFrames, numerator, denominator);
		}

		void  APICALL setFrameRates(uint64 numerator, uint64 denominator, pcIError_base & error) {

			return mRawPtr->setFrameRates(numerator, denominator, error);
		}

		void  APICALL SetFrameRates(uint64 numerator, uint64 denominator) {


			return CallSafeFunctionReturningVoid<
				IMarker, uint64, uint64
			>(mRawPtr, &IMarker::setFrameRates, numerator, denominator);
		}

		double  APICALL GetProbability() const {

			return mRawPtr->GetProbability();
		}

		void  APICALL setProbability(double markerProbability, pcIError_base & error) {

			return mRawPtr->setProbability(markerProbability, error);
		}

		void  APICALL SetProbability(double markerProbability) {

			return CallSafeFunctionReturningVoid<
				IMarker, double
			>(mRawPtr, &IMarker::setProbability, markerProbability);
		}

		AdobeXMPCommon::pcIUTF8String_base  APICALL getComment(pcIError_base &error) const {

			return mRawPtr->getComment(error);
		}

		AdobeXMPCommon::spcIUTF8String APICALL GetComment() const {
			return CallConstSafeFunctionReturningPointer< IMarker, pcIUTF8String_base, const IUTF8String >(mRawPtr, &IMarker::getComment);

		}

		AdobeXMPCommon::pcIUTF8String_base  APICALL getLocation(pcIError_base &error) const {
			return mRawPtr->getLocation(error);
		}

		AdobeXMPCommon::spcIUTF8String  APICALL GetLocation() const {
			return CallConstSafeFunctionReturningPointer< IMarker, pcIUTF8String_base, const IUTF8String >(mRawPtr, &IMarker::getLocation);

		}

		AdobeXMPCommon::pcIUTF8String_base  APICALL getSpeaker(pcIError_base &error) const {
			return mRawPtr->getSpeaker(error);
		}

		AdobeXMPCommon::spcIUTF8String  APICALL GetSpeaker() const {
			return CallConstSafeFunctionReturningPointer< IMarker, pcIUTF8String_base, const IUTF8String >(mRawPtr, &IMarker::getSpeaker);

		}

		AdobeXMPCommon::pcIUTF8String_base  APICALL getTarget(pcIError_base &error) const {
			return mRawPtr->getTarget(error);
		}

		AdobeXMPCommon::spcIUTF8String APICALL GetTarget() const {
			return CallConstSafeFunctionReturningPointer< IMarker, pcIUTF8String_base, const IUTF8String >(mRawPtr, &IMarker::getTarget);

		}

		AdobeXMPCommon::pcIUTF8String_base  APICALL getGUID(pcIError_base &error) const {
			return mRawPtr->getGUID(error);
		}

		AdobeXMPCommon::spcIUTF8String  APICALL GetGUID() const {
			return CallConstSafeFunctionReturningPointer< IMarker, pcIUTF8String_base, const IUTF8String >(mRawPtr, &IMarker::getGUID);

		}
		void  APICALL SetLocation(const char * location, sizet locationLen) {
			return CallSafeFunctionReturningVoid<
				IMarker, const char *, sizet
			>(mRawPtr, &IMarker::setLocation, location, locationLen);
		}

		void  APICALL setLocation(const char * location, sizet locationLen, pcIError_base & error) {
			return mRawPtr->setLocation(location, locationLen, error);
		}

		void  APICALL SetSpeaker(const char * speaker, sizet speakerLen) {
			return CallSafeFunctionReturningVoid<
				IMarker, const char *, sizet
			>(mRawPtr, &IMarker::setSpeaker, speaker, speakerLen);
		}

		void  APICALL setSpeaker(const char * speaker, sizet speakerLen, pcIError_base & error) {
			return mRawPtr->setSpeaker(speaker, speakerLen, error);
		}

		void  APICALL SetComment(const char * name, sizet commentLen) {
			return AdobeXMPCommon::CallSafeFunctionReturningVoid<
				IMarker, const char *, sizet
			>(mRawPtr, &IMarker::setComment, name, commentLen);
		}

		void  APICALL setComment(const char * comment, sizet commentLen, pcIError_base & error) {
			return mRawPtr->setComment(comment, commentLen, error);
		}

		void  APICALL SetTarget(const char * target, sizet targetLen) {

			return AdobeXMPCommon::CallSafeFunctionReturningVoid<
				IMarker, const char *, sizet
			>(mRawPtr, &IMarker::setTarget, target, targetLen);
		}

		void  APICALL setTarget(const char * target, sizet targetLen, pcIError_base & error) {
			return mRawPtr->setTarget(target, targetLen, error);
		}

		bool APICALL  HasTypes(uint64 markerTypeMask) const {

			uint64 presentMask = GetTypes();
			if (presentMask & markerTypeMask) {
				return true;
			}
			return false;
		}

		AdobeXMPCommon::sizet  APICALL RemoveAllExtensions() __NOTHROW__{

			return mRawPtr->RemoveAllExtensions();
		}

			AdobeXMPCommon::sizet  APICALL GetExtensionCount() const __NOTHROW__{

			return mRawPtr->GetExtensionCount();
		}

			bool  APICALL HasExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen) const __NOTHROW__{

			return CallConstSafeFunction<
			IMarker, bool, uint32, const char *, sizet, const char *, sizet
			>(mRawPtr, &IMarker::hasExtension, extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen);
		}

			AdobeXMPCommon::uint32  APICALL hasExtension(const char * extensionNameSpace, sizet extensionNameSpaceLen, const char * extensionName, sizet extensionNameLen, pcIError_base & error) const {

			return mRawPtr->hasExtension(extensionNameSpace, extensionNameSpaceLen, extensionName, extensionNameLen, error);
		}

		AdobeXMPCommon::sizet  APICALL RemoveAllCuePointParams() {

			return mRawPtr->RemoveAllCuePointParams();
		}

		AdobeXMPCommon::uint32  APICALL hasCustomType(const char * customType, sizet customTypeLen, pcIError_base & error) const {

			return mRawPtr->hasCustomType(customType, customTypeLen, error);
		}
		
		bool  APICALL HasCustomType(const char * customType, sizet customTypeLen) const {

			return CallConstSafeFunction<
				IMarker, bool, uint32, const char *, sizet
			>(mRawPtr, &IMarker::hasCustomType, customType, customTypeLen);
		}

	
	};
	
	
	spIMarker IMarker_v1::MakeShared(pIMarker_base ptr) {
		pIMarker p = IMarker::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IMarker >() : ptr;
		return shared_ptr< IMarker >(new IMarkerProxy(p));
	}
}

#endif  // #if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE