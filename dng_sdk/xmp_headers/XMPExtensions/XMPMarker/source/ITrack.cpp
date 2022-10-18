// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================
namespace AdobeXMPAM {
	class ITrackProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::ITrackProxy;

#include "XMPMarker/Interfaces/ITrack.h"


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

	class ITrackProxy : public virtual ITrack {
		//! \cond XMP_INTERNAL_DOCUMENTATION
	private:
		pITrack mRawPtr;
	public:

		ITrackProxy(pITrack ptr) : mRawPtr(ptr) {
			mRawPtr->Acquire();
		}

		~ITrackProxy() {
			mRawPtr->Release();
		}

		pITrack APICALL GetActualITrack() __NOTHROW__{
			return mRawPtr;
		}

			AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{
			return mRawPtr->GetISharedObject_I();
		}

			AdobeXMPAM_Int::pITrack_I APICALL GetITrack_I() __NOTHROW__{
			return mRawPtr->GetITrack_I();
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

	

	


			pIMarker_base  APICALL addMarker(const char * markerName, sizet markerNameLen, uint64 markerTypes, const char * markerTypeName, sizet markerTypeNameLen, const char * guid, sizet guidLen, pcIError_base & error) {

		return mRawPtr->AddMarker(markerName, markerNameLen, markerTypes, markerTypeName, markerTypeNameLen, guid, guidLen).get();
	}

		spIMarker  APICALL AddMarker(const char * markerName, sizet markerNameLen, uint64 markerTypes, const char * markerTypeName, sizet markerTypeNameLen, const char * guid, sizet guidLen) {

		//return NS_XMPCOMMON::CallSafeFunctionReturningPointer< ITrack, pIMarker_base, IMarker, const char *, uint64, const char *, const char * >(mRawPtr, &ITrack::AddMarker, markerName, markerTypes, markerTypeName, guid);
		//return NS_XMPCOMMON::CallSafeFunctionReturningPointer< ITrack_v1, pIMarker_base, IMarker , const char *, uint64, const char *, const char *, uint32> (mRawPtr, &AddMarker, markerName, markerTypes, markerTypeName, guid, static_cast<uint32> (multiThreadingSupport));

		pcIError_base error(NULL);
		pIMarker_base returnValue = mRawPtr->addMarker(markerName, markerNameLen, markerTypes, markerTypeName, markerTypeNameLen, guid, guidLen, error);
		spIMarker sp = IMarker::MakeShared(returnValue);
		if (error)
			throw error;
		return sp;//dynamic_pointer_cast<IMarker>(sp);
	}



		pIMarker_base  APICALL addMarker(pIMarker_base markerToBeAdded, pcIError_base &error) {

		return mRawPtr->addMarker(markerToBeAdded, error);
	}

		spIMarker  APICALL AddMarker(spIMarker markerToBeAdded) {
		//VERIFY
		pcIError_base error(NULL);
		pIMarker_base returnValue = mRawPtr->addMarker(markerToBeAdded.get(), error);
		spIMarker sp = IMarker::MakeShared(returnValue);
		if (error)
				throw error;
		//return std::make_shared< IMarker >(sp);//dynamic_pointer_cast<IMarker>(sp);
		return sp;
	}


		spITrack  APICALL Clone() const {
		//VERIFY
		spITrack spProxyBase =  CallConstSafeFunctionReturningPointer<
			ITrack, pITrack_base, ITrack
		> (mRawPtr, &ITrack::clone);
		return  spProxyBase;
	}

		pITrack_base  APICALL clone(pcIError_base & error) const {
		return mRawPtr->clone(error);
	}

		spcIUTF8String  APICALL GetName() const {
		return  CallConstSafeFunctionReturningPointer< ITrack, pcIUTF8String_base, const IUTF8String >(mRawPtr, &ITrack::getName);
	}

		pcIUTF8String_base APICALL  getName(pcIError_base & error) const {

		return mRawPtr->getName(error);
	}

		void  APICALL SetName(const char * name, sizet nameLen) {

		return  CallSafeFunctionReturningVoid<ITrack, const char *, sizet>(mRawPtr, &ITrack::setName, name, nameLen);
	}

		void APICALL  setName(const char * name,	sizet nameLen, pcIError_base & error) {
		return mRawPtr->setName(name, nameLen, error);
	}


		uint64 APICALL  setTypes(uint64 inputMask, pcIError_base & error) {

		return mRawPtr->setTypes(inputMask, error);
	}

		IMarker::eMarkerType  APICALL SetTypes(IMarker::eMarkerType inputMask) {

		return  CallSafeFunction<
			ITrack, uint64, uint64, uint64
		>(mRawPtr, &ITrack::setTypes, inputMask);
	}

		IMarker::eMarkerType APICALL  removeTypes(IMarker::eMarkerType inputMask, pcIError_base & error) {

		return mRawPtr->removeTypes(inputMask, error);
	}

		uint64  APICALL RemoveTypes(uint64 inputMask) {

		return  CallSafeFunction<
			ITrack, uint64, uint64, uint64
		>(mRawPtr, &ITrack::removeTypes, inputMask);
	}

		uint64  APICALL getTypes(pcIError_base & error) const {

		return mRawPtr->getTypes(error);
	}

		uint64  APICALL GetTypes() const {

		return  CallConstSafeFunction<
			ITrack, uint64, uint64
		>(mRawPtr, &ITrack::getTypes);
	}


		bool  APICALL AppendCustomType(const char * userDefinedType, sizet userDefinedTypeLen) {

		return  CallSafeFunction<
			ITrack, bool, bool, const char *, sizet
		>(mRawPtr, &ITrack::appendCustomType, userDefinedType, userDefinedTypeLen);
	}

		bool  APICALL appendCustomType(const char * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error) {

		return mRawPtr->AppendCustomType(userDefinedType, userDefinedTypeLen);
	}

		bool  APICALL RemoveCustomType(const char * userDefinedType, sizet userDefinedTypeLen) {

		return  CallSafeFunction<
			ITrack, bool, bool, const char *, sizet
		>(mRawPtr, &ITrack::removeCustomType, userDefinedType, userDefinedTypeLen);
	}

		bool  APICALL removeCustomType(const char * userDefinedType, sizet userDefinedTypeLen, pcIError_base & error) {

		return mRawPtr->RemoveCustomType(userDefinedType, userDefinedTypeLen);
	}

		sizet  APICALL GetCustomTypeCount() const {

		return  CallConstSafeFunction<
			ITrack, sizet, sizet
		>(mRawPtr, &ITrack::getCustomTypeCount);
	}

		sizet  APICALL getCustomTypeCount(pcIError_base & error) const {

		return mRawPtr->GetCustomTypeCount();
	}

	static void * MyMalloc(sizet size) { return malloc(size); }

	static  spcIUTF8StringList CreateStringList(pcIUTF8String_base * ptr, sizet nElements) {
		spcIUTF8StringList list =  shared_ptr<cIUTF8StringList>(
			new cIUTF8StringList());
		list->reserve(nElements);
		for (sizet i = 0; i < nElements; i++) {
			auto sp =  IUTF8String::MakeShared(ptr[i]);
			list->push_back(sp);
		}
		free(ptr);
		return list;
	}

	spcIUTF8StringList APICALL  GetCustomTypes() const {
		pcIUTF8String_base * ptr(NULL);
		sizet nElements =  CallConstSafeFunction<
			ITrack, sizet, sizet, pcIUTF8String_base *&, MemAllocatorFunc
		>(mRawPtr, &ITrack::getCustomTypes, ptr, &MyMalloc);
		return CreateStringList(ptr, nElements);
	}

	sizet  APICALL getCustomTypes(pcIUTF8String_base *& array, MemAllocatorFunc allocFunc,
		pcIError_base & error) const __NOTHROW__
	{
		return mRawPtr->getCustomTypes(array, allocFunc, error);
	}

		void  APICALL GetFrameRate(uint64 & numerator, uint64 & denominator) const {

		return mRawPtr->GetFrameRate(numerator, denominator);
	}

	void  APICALL setFrameRate( uint64 numerator, uint64 denominator, pcIError_base & error ) {

		return mRawPtr->setFrameRate(numerator, denominator, error);
	}

	void  APICALL SetFrameRate( uint64 numerator /*= 1*/, uint64 denominator /*= 1*/ ) {

		return mRawPtr->SetFrameRate(numerator, denominator);
		/*	return  CallSafeFunctionReturningVoid(mRawPtr, &ITr;ack::setFrameRate,numerator, denominator);*/
	}

	static spIMarkerList CreateMarkerList( pIMarker_base * ptr, sizet nElements ) {
		//verify
		spIMarkerList list =  shared_ptr<IMarkerList>(
			new IMarkerList() );
		list->reserve( nElements );
		for ( sizet i = 0; i < nElements; i++ ) {
		//	auto sp =  IMarker::MakeShared( ptr[ i ] );
			list->push_back(  shared_ptr<IMarker>( ptr[i] ) );
		}
		//	free( ptr );
		return list;
	}

	//	static void * MyMalloc( sizet size ) { return malloc( size ); }

	spIMarkerList  APICALL GetAllMarkers() const {
		pIMarker_base * ptr(NULL);
		sizet nElements =  CallConstSafeFunction<
			ITrack, sizet, sizet, pIMarker_base *&, MemAllocatorFunc
		>(mRawPtr, &ITrack::getAllMarkers, ptr, &MyMalloc);
		return CreateMarkerList(ptr, nElements);
	}

	sizet APICALL  getAllMarkers(pIMarker_base *& array, MemAllocatorFunc allocFunc,
		pcIError_base & error) const
	{
		return mRawPtr->getAllMarkers(array, allocFunc, error);
	}

	AdobeXMPAM::pIMarker_base  APICALL removeMarker( const char * guid, sizet guidLen, pcIError_base & error ) {

		return mRawPtr->removeMarker(guid, guidLen, error);
	}

	AdobeXMPAM::pIMarker_base  APICALL removeMarker( const sizet & markerIdx, pcIError_base & error ) {

		return mRawPtr->removeMarker(markerIdx, error);
	}

	AdobeXMPAM::spIMarker  APICALL RemoveMarker( const sizet & markerIdx ) {
		pcIError_base error(NULL);
		pIMarker_base returnValue = mRawPtr->removeMarker(markerIdx, error);
		spIMarker sp =  IMarker::MakeShared(returnValue);
		if (error)
			throw  IError_v1::MakeShared(error);
		return sp;//dynamic_pointer_cast<IMarker>(sp);
	}

	AdobeXMPAM::spIMarker  APICALL RemoveMarker( const char * guid, sizet guidLen ) {
		pcIError_base error(NULL);
		pIMarker_base returnValue = mRawPtr->removeMarker(guid, guidLen, error);
		spIMarker sp =  IMarker::MakeShared(returnValue);
		if (error)
			throw  IError_v1::MakeShared(error);
		return sp;//dynamic_pointer_cast<IMarker>(sp);
	}



	AdobeXMPAM::spIMarker  APICALL GetMarker(const sizet & markerIdx) const{

		pcIError_base error(NULL);
		pIMarker_base returnValue = mRawPtr->getMarker(markerIdx, error);
		spIMarker sp =  IMarker::MakeShared(returnValue);
		if (error)
			throw  IError::MakeShared(error);
		return sp;//dynamic_pointer_cast<IMarker>(sp);

	}

	AdobeXMPAM::spIMarker  APICALL GetMarker(const char * guid, sizet guidLen) const {

		pcIError_base error(NULL);
		pIMarker_base returnValue = mRawPtr->getMarker(guid, guidLen, error);
		spIMarker sp =  IMarker::MakeShared(returnValue);
		if (error)
			throw  IError::MakeShared(error);
		return sp;//dynamic_pointer_cast<IMarker>(sp);

	}

	AdobeXMPAM::pIMarker_base  APICALL getMarker(const char * guid, sizet guidLen, pcIError_base & error) const {

		return mRawPtr->getMarker(guid, guidLen, error);
	}

	AdobeXMPAM::pIMarker_base  APICALL getMarker(const sizet & markerIdx, pcIError_base & error) const {

		return mRawPtr->getMarker(markerIdx, error);
	}

	sizet  APICALL GetMarkerCount() const {

		return mRawPtr->GetMarkerCount();
	}

	void APICALL  foo(const std::vector< std::pair< const char *, sizet> > & v) {

		const void * ptr = &v;
		sizet count = v.size();
		pcIError_base error(NULL);
		if (!error) {

			return foo(const_cast<void *>(ptr), count, error);
		}
		/*sizet nElements = NS_XMPCOMMON::CallSafeFunction<
			ITrack, sizet, sizet, pIMarker_base *&, MemAllocatorFunc
			>(mRawPtr, &ITrack::GetAllMarkers, ptr, &MyMalloc);
			return CreateMarkerList(ptr, nElements);*/
	}

	void  APICALL foo(void * v, sizet size, pcIError_base & error) {

		return mRawPtr->foo(v, size, error);
	}

	AdobeXMPAM::spIMarkerList  APICALL RemoveMarkers( uint64 markerTypes /*= UINT64_MAX*/, bool matchAllCriteria /*= false*/, const std::vector<std::pair<const char *, sizet> > &markerTypeName /*= std::vector<const char *> ()*/ ) {

		const void * ptr = &markerTypeName;
		sizet count = markerTypeName.size();
		pcIError_base error(NULL);
		pIMarker_base *markerPtr(NULL);
		if (!error) {

			sizet nElements =  CallSafeFunction< ITrack, sizet, sizet, uint64, uint32, void *, sizet, pIMarker_base *&,MemAllocatorFunc> (mRawPtr, &ITrack::removeMarkers, markerTypes,
				static_cast<uint32>(matchAllCriteria), const_cast< void *>( ptr), count, markerPtr, &MyMalloc);
			return CreateMarkerList(markerPtr, nElements);

		}
		return spIMarkerList();

	}

	sizet  APICALL removeMarkers( uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet size, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error ) {

		return mRawPtr->removeMarkers(markerTypes, matchAllCriteria, rawptr, size, array,  allocFunc, error);
	}

	AdobeXMPAM::spIMarkerList  APICALL GetMarkers( uint64 markerTypes /*= UINT64_MAX*/, bool matchAllCriteria /*= false*/, const std::vector<std::pair< const char *, sizet> > &markerTypeName /*= std::vector<const char *> ()*/ ) const {

		const void * ptr = &markerTypeName;
		sizet count = markerTypeName.size();
		pcIError_base error(NULL);
		pIMarker_base *markerPtr(NULL);
		if (!error) {

			sizet nElements =  CallConstSafeFunction< ITrack, sizet, sizet, uint64, uint32, void *, sizet, pIMarker_base *&,MemAllocatorFunc> (mRawPtr, &ITrack::getMarkers, markerTypes,
				static_cast<uint32>(matchAllCriteria), const_cast< void *>(ptr), count, markerPtr, &MyMalloc);
			return CreateMarkerList(markerPtr, nElements);

		}
		return spIMarkerList();
	}

	sizet  APICALL getMarkers( uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error ) const {
		return mRawPtr->getMarkers(markerTypes, matchAllCriteria, rawptr, count, array,  allocFunc, error);
	}

	sizet  APICALL getMarkersByName(const char * name, sizet nameLen, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error) const {

		return mRawPtr->getMarkersByName(name, nameLen, array, allocFunc, error);
	}

	AdobeXMPAM::spIMarkerList  APICALL GetMarkersByName(const char * markerName, sizet markerNameLen) const {
		pcIError_base error(NULL);
		pIMarker_base *markerPtr(NULL);
		sizet nElements =  CallConstSafeFunction< ITrack, sizet, sizet, const char *, sizet, pIMarker_base *&, MemAllocatorFunc>(mRawPtr, &ITrack::getMarkersByName, markerName, markerNameLen, markerPtr, &MyMalloc);
		return CreateMarkerList(markerPtr, nElements);

	}

	sizet  APICALL getMarkersInTimeRange( uint64 startFrameCount, uint64 endFrameCount, uint32 timeBoundary, uint64 markerTypes, uint32 matchAllCriteria, void * rawptr, sizet count, pIMarker_base *&array, MemAllocatorFunc allocFunc, pcIError_base & error ) const {

		return mRawPtr->getMarkersInTimeRange(startFrameCount, endFrameCount, timeBoundary, markerTypes, matchAllCriteria, rawptr, count, array, allocFunc, error);
	}

	AdobeXMPAM::spIMarkerList  APICALL GetMarkersInTimeRange( uint64 startFrameCount /*= 0*/, uint64 endFrameCount /*= UINT64_MAX*/, eTrackTimeBoundary timeBoundary /*= kTTBothStartTimeAndEndTimeIncluded*/, uint64 markerTypes /*= UINT64_MAX*/, bool matchAllCriteria /*= false*/, const std::vector<std::pair<const char *, sizet> > &markerTypeName /*= std::vector<const char *>()*/ ) const {

		const void * ptr = &markerTypeName;
		sizet count = markerTypeName.size();
		pcIError_base error(NULL);
		pIMarker_base *markerPtr(NULL);
		if (!error) {

			sizet nElements =  CallConstSafeFunction< ITrack, sizet, sizet, uint64, uint64, uint32, uint64, uint32, void *, sizet, pIMarker_base *&,MemAllocatorFunc> (
				mRawPtr, &ITrack::getMarkersInTimeRange, startFrameCount, endFrameCount, static_cast<uint32>(timeBoundary), markerTypes,
				static_cast<uint32>(matchAllCriteria), const_cast< void *>(ptr), count, markerPtr, &MyMalloc);
			return CreateMarkerList(markerPtr, nElements);

		}
		return spIMarkerList();

	}

	bool  APICALL HasCustomType( const char * customType, sizet customTypeLen ) const {

		return mRawPtr->HasCustomType(customType, customTypeLen);
	}

	uint32  APICALL HasCustomType( const char * customType, sizet customTypeLen, pcIError_base & error ) const {
		return mRawPtr->HasCustomType(customType, customTypeLen);
	}

	virtual bool  APICALL HasTypes(IMarker::eMarkerType  markerTypeMask) const {
	
		return mRawPtr->HasTypes(markerTypeMask);
	}


	};

	spITrack ITrack_v1::MakeShared(pITrack_base ptr) {
		pITrack p = ITrack::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< ITrack >() : ptr;
		return shared_ptr< ITrack >(new ITrackProxy(p));
	}

	spITrack ITrack_v1::CreateTrack(const char * trackName, sizet trackNameLen, uint64 trackTypes, const char * trackTypeName /* = NULL */, sizet trackTypeNameLen) {
		//VERIFY
		auto spBase =  CallSafeFunctionReturningPointer<
			IExtensionsObjectFactory, pITrack_base, ITrack, const char *, sizet, uint64, const char *, sizet
		>(
		IExtensionsObjectFactory::GetExtensionsObjectFactory(),
		&IExtensionsObjectFactory::CreateTrack,
		trackName, trackNameLen, trackTypes, trackTypeName, trackTypeNameLen);
		return spBase;
	}
}

#endif  // #if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE