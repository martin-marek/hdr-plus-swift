// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================
namespace AdobeXMPAM {
	class ITemporalAssetManagerProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::ITemporalAssetManagerProxy;

#include "XMPMarker/Interfaces/ITemporalAssetManager.h"


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

	using namespace AdobeXMPCommon;
#if XMP_WinBuild
#pragma warning( push )
#pragma warning( disable : 4250 )
#endif

	class ITemporalAssetManagerProxy : public virtual ITemporalAssetManager {
		//! \cond XMP_INTERNAL_DOCUMENTATION
	private:
		pITemporalAssetManager mRawPtr;
	public:

		ITemporalAssetManagerProxy(pITemporalAssetManager ptr) : mRawPtr(ptr) {
			mRawPtr->Acquire();
		}

		~ITemporalAssetManagerProxy() {
			mRawPtr->Release();
		}

		pITemporalAssetManager APICALL GetActualITemporalAssetManager() __NOTHROW__{
			return mRawPtr;
		}

			AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{
			return mRawPtr->GetISharedObject_I();
		}

			AdobeXMPAM_Int::pITemporalAssetManager_I APICALL GetITemporalAssetManager_I() __NOTHROW__{
			return mRawPtr->GetITemporalAssetManager_I();
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





		

		AdobeXMPAM::spITrack APICALL AddTrack(spITrack trackToBeAdded) {

			pcIError_base error(NULL);
			pITrack_base returnValue = mRawPtr->addTrack( trackToBeAdded.get(), error);
			spITrack sp = ITrack::MakeShared(returnValue);// MakeSharedPointer< ITrack_base >(returnValue);
			if (error)
				throw  IError::MakeShared(error);
			return sp;//dynamic_pointer_cast<IMarker>(sp);
		}

		AdobeXMPAM::pITrack_base  APICALL addTrack( pITrack_base trackToBeAdded, pcIError_base & error ) {
			return mRawPtr->addTrack(trackToBeAdded, error);

		}

		static void * MyMalloc(sizet size) { return malloc(size); }

		static spITrackList CreateTrackList(pITrack_base * ptr, sizet nElements) {
			spITrackList list =  shared_ptr<ITrackList>(
				new ITrackList());
			list->reserve(nElements);
			for (sizet i = 0; i < nElements; i++) {
				auto sp =  ITrack::MakeShared(ptr[i]);
				list->push_back(sp);
			}
			free(ptr);
			return list;
		}

		static spIMarkerList CreateMarkerList(pIMarker_base * ptr, sizet nElements) {
			//VERIFY
			spIMarkerList list =  shared_ptr<IMarkerList>(
				new IMarkerList());
			list->reserve(nElements);
			for (sizet i = 0; i < nElements; i++) {

				auto sp =  IMarker::MakeShared(ptr[i]);
				list->push_back( sp );
			}
			free(ptr);
			return list;
		}

		sizet  APICALL getAllTracks(pITrack_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error) const {

			return mRawPtr->getAllTracks(array, allocFunc, error);
		}

		AdobeXMPAM::spITrackList  APICALL GetAllTracks() const {
			pITrack_base * ptr(NULL);
			sizet nElements =  CallConstSafeFunction<
				ITemporalAssetManager, sizet, sizet, pITrack_base *&, MemAllocatorFunc
			>(mRawPtr, &ITemporalAssetManager::getAllTracks, ptr, &MyMalloc);
			return CreateTrackList(ptr, nElements);
		}

		sizet  APICALL removeAllTracks( pITrack_base *& array, MemAllocatorFunc allocFunc, pcIError_base & error ) {

			return mRawPtr->removeAllTracks(array, allocFunc, error);
		}

		AdobeXMPAM::spITrackList  APICALL RemoveAllTracks() {
			pITrack_base * ptr(NULL);
			sizet nElements =  CallSafeFunction<
				ITemporalAssetManager, sizet, sizet, pITrack_base *&, MemAllocatorFunc
			>(mRawPtr, &ITemporalAssetManager::removeAllTracks, ptr, &MyMalloc);
			return CreateTrackList(ptr, nElements);
		}

		AdobeXMPAM::spIMarkerList  APICALL RemoveMarkers( uint64 markerTypes /*= UINT64_MAX*/, bool matchAllCriteria /*= false*/, const std::vector<std::pair<const char *, sizet> > &markerTypeName /*= std::vector<const char *> ()*/ ) {

			const void * ptr = &markerTypeName;
			sizet count = markerTypeName.size();
			pcIError_base error(NULL);
			pIMarker_base *markerPtr(NULL);
			if (!error) {

				sizet nElements =  CallSafeFunction< ITemporalAssetManager, sizet, sizet, uint64, uint32, void *, sizet, pIMarker_base *&, MemAllocatorFunc>(mRawPtr, &ITemporalAssetManager::removeMarkers, markerTypes,
					static_cast<uint32>(matchAllCriteria), const_cast< void *> (ptr), count, markerPtr, &MyMalloc);
				return CreateMarkerList(markerPtr, nElements);

			}
			return spIMarkerList();
		}

		sizet  APICALL removeMarkers(uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet size, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) {

			return mRawPtr->removeMarkers(markerTypes, matchAllCriteria, rawptr, size, array, allocFunc, error);
		}

		AdobeXMPAM::spIMarkerList  APICALL GetMarkers(uint64 markerTypes /*= UINT64_MAX*/, bool matchAllCriteria /*= false*/, const std::vector<std::pair<const char *, sizet> > &markerTypeName /*= std::vector<const char *> ()*/) const {

			const void * ptr = (&markerTypeName);
			sizet count = markerTypeName.size();
			pcIError_base error(NULL);
			pIMarker_base *markerPtr(NULL);
			if (!error) {

				sizet nElements =  CallConstSafeFunction< ITemporalAssetManager, sizet, sizet, uint64, uint32, void *, sizet, pIMarker_base *&, MemAllocatorFunc>(mRawPtr, &ITemporalAssetManager::getMarkers, markerTypes,
					static_cast<uint32>(matchAllCriteria), const_cast<void *>( ptr ), count, markerPtr, &MyMalloc);
				return CreateMarkerList(markerPtr, nElements);

			}
			return spIMarkerList();
		}

		sizet  APICALL getMarkers(uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const {
			return mRawPtr->getMarkers(markerTypes, matchAllCriteria, rawptr, count, array, allocFunc, error);
		}

		sizet  APICALL getMarkersByName(const char * name, sizet nameLen, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const {

			return mRawPtr->getMarkersByName(name, nameLen, array, allocFunc, error);
		}

		AdobeXMPAM::spIMarkerList  APICALL GetMarkersByName(const char * markerName, sizet markerNameLen) const {
			pcIError_base error(NULL);
			pIMarker_base *markerPtr(NULL);
			sizet nElements =  CallConstSafeFunction< ITemporalAssetManager, sizet, sizet, const char *, sizet, pIMarker_base *&, MemAllocatorFunc>(mRawPtr, &ITemporalAssetManager::getMarkersByName, markerName, markerNameLen, markerPtr, &MyMalloc);
			return CreateMarkerList(markerPtr, nElements);

		}

		AdobeXMPAM::spITrackList APICALL  RemoveTracks( uint64 markerTypes /*= UINT64_MAX*/, bool matchAllCriteria /*= false*/, const std::vector<std::pair<const char *, sizet> > &markerTypeName /*= std::vector<const char *> ()*/ ) {

			const void * ptr = &markerTypeName;
			sizet count = markerTypeName.size();
			pcIError_base error(NULL);
			pITrack_base *markerPtr(NULL);
			if (!error) {

				sizet nElements =  CallSafeFunction< ITemporalAssetManager, sizet, sizet, uint64, uint32, void *, sizet, pITrack_base *&, MemAllocatorFunc>(mRawPtr, &ITemporalAssetManager::removeTracks, markerTypes,
					static_cast<uint32>(matchAllCriteria), const_cast<void *>(ptr), count, markerPtr, &MyMalloc);
				return CreateTrackList(markerPtr, nElements);

			}
			return spITrackList();
		}

		sizet  APICALL removeTracks( uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pITrack_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error ) {
			return mRawPtr->removeTracks(markerTypes, matchAllCriteria, rawptr, count, array, allocFunc, error);
		}


		AdobeXMPAM::spITrackList  APICALL GetTracks( uint64 markerTypes /*= UINT64_MAX*/, bool matchAllCriteria /*= false*/, const std::vector<std::pair<const char *, sizet> > &markerTypeName /*= std::vector<const char *> ()*/ ) const {

			const void * ptr = &markerTypeName;
			sizet count = markerTypeName.size();
			pcIError_base error(NULL);
			pITrack_base *markerPtr(NULL);
			if (!error) {

				sizet nElements =  CallConstSafeFunction< ITemporalAssetManager, sizet, sizet, uint64, uint32, void *, sizet, pITrack_base *&, MemAllocatorFunc>(mRawPtr, &ITemporalAssetManager::getTracks, markerTypes,
					static_cast<uint32>(matchAllCriteria),  const_cast< void *>(ptr), count, markerPtr, &MyMalloc);
				return CreateTrackList(markerPtr, nElements);

			}
			return spITrackList();
		}

		sizet  APICALL getTracks(uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pITrack_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const  {
			return mRawPtr->getTracks(markerTypes, matchAllCriteria, rawptr, count, array, allocFunc, error);
		}

		sizet  APICALL getMarkersInTimeRange( uint64 startFrameCount, uint64 endFrameCount, uint32 timeBoundary, uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error ) const {

			return mRawPtr->getMarkersInTimeRange(startFrameCount, endFrameCount, timeBoundary, markerTypes, matchAllCriteria, rawptr, count, array, allocFunc, error);
		}

		AdobeXMPAM::spIMarkerList  APICALL GetMarkersInTimeRange(uint64 startFrameCount /*= 0*/, uint64 endFrameCount /*= UINT64_MAX*/, ITrack::eTrackTimeBoundary timeBoundary /*= kTTBothStartTimeAndEndTimeIncluded*/, uint64 markerTypes /*= UINT64_MAX*/, bool matchAllCriteria /*= false*/, const std::vector<std::pair<const char *, sizet> > &markerTypeName /*= std::vector<const char *>()*/) const {

			const void * ptr = &markerTypeName;
			sizet count = markerTypeName.size();
			pcIError_base error(NULL);
			pIMarker_base *markerPtr(NULL);
			if (!error) {

				sizet nElements =  CallConstSafeFunction< ITemporalAssetManager, sizet, sizet, uint64, uint64, uint32, uint64, uint32, void *, sizet, pIMarker_base *&, MemAllocatorFunc>(
					mRawPtr, &ITemporalAssetManager::getMarkersInTimeRange, startFrameCount, endFrameCount, static_cast<uint32>(timeBoundary), markerTypes,
					static_cast<uint32>(matchAllCriteria),  const_cast< void *>(ptr), count, markerPtr, &MyMalloc);
				return CreateMarkerList(markerPtr, nElements);

			}
			return spIMarkerList();

		}
	};
	spITemporalAssetManager ITemporalAssetManager_v1::MakeShared(pITemporalAssetManager_base ptr) {
		pITemporalAssetManager p = ITemporalAssetManager::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< ITemporalAssetManager >() : ptr;
		return shared_ptr< ITemporalAssetManager >(new ITemporalAssetManagerProxy(p));
	}

	spITemporalAssetManager ITemporalAssetManager_v1::CreateTemporalAssetManager( const spIMetadata & metadata)
	{
		auto spBase =  CallSafeFunctionReturningPointer<
			IExtensionsObjectFactory, pITemporalAssetManager_base, ITemporalAssetManager, pIMetadata_base
		>(
		IExtensionsObjectFactory::GetExtensionsObjectFactory(),
		&IExtensionsObjectFactory::CreateTemporalAssetManager,
		metadata.get());
		return spBase;// MakeUncheckedSharedPointerWithDefaultDelete(new ITemporalAssetManager(spBase));

		pcIError_base error(NULL);
		auto returnValue = IExtensionsObjectFactory::GetExtensionsObjectFactory()->CreateTemporalAssetManager(metadata.get(), error);
		if (error)
			throw IError::MakeShared(error);
		return ITemporalAssetManager_v1::MakeShared(returnValue);
		}

	

}

#endif  // #if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
