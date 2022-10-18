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
//  ADC	Amandeep Chawla
//
// mm-dd-yy who Description of changes, most recent first.
//
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 11-24-14 ADC 1.0-a008 Support for ICompositionRelationship interface.
//
// =================================================================================================
#endif  // AdobePrivate

namespace AdobeXMPAM {
	class ICompositionRelationshipProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::ICompositionRelationshipProxy;


#include "XMPAssetManagement/Interfaces/ICompositionRelationship.h"

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPExtensions/include/IExtensionsObjectFactory.h"
#include "XMPAssetManagement/Interfaces/IAssetPart.h"
#include "XMPCore/Interfaces/IStructureNode.h"
#include <assert.h>

namespace AdobeXMPAM {
	class ICompositionRelationshipProxy
		: public virtual ICompositionRelationship
	{
	private:
		pICompositionRelationship mRawPtr;

	public:
		ICompositionRelationshipProxy(pICompositionRelationship ptr)
			: mRawPtr(ptr)
		{
			mRawPtr->Acquire();
		}

		~ICompositionRelationshipProxy() { mRawPtr->Release(); }

		pICompositionRelationship APICALL GetActualICompositionRelationship() __NOTHROW__{ return mRawPtr; }

			void APICALL Acquire() const __NOTHROW__{ assert( false ); 
		}

		void APICALL Release() const __NOTHROW__{ assert( false ); 
		}

		AdobeXMPCommon_Int::pIThreadSafe_I APICALL GetIThreadSafe_I() __NOTHROW__{
			return mRawPtr->GetIThreadSafe_I();
		}

		void APICALL EnableThreadSafety() const __NOTHROW__{
			return mRawPtr->EnableThreadSafety();
		}

		void APICALL DisableThreadSafety() const __NOTHROW__{
			return mRawPtr->DisableThreadSafety();
		}

		bool APICALL IsThreadSafe() const {
			return mRawPtr->isThreadSafe() != 0;
		}

		uint32 APICALL isThreadSafe() const __NOTHROW__{
			assert(false);
			return mRawPtr->isThreadSafe();
		}

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{
			return mRawPtr->GetISharedObject_I();
		}

		AdobeXMPAM_Int::pICompositionRelationship_I APICALL GetICompositionRelationship_I() __NOTHROW__{
			return mRawPtr->GetICompositionRelationship_I();
		}

			pvoid APICALL GetInterfacePointer(uint64 interfaceID, uint32 interfaceVersion) {
			return CallSafeFunction< IVersionable, pvoid, pvoid, uint64, uint32 >(
				mRawPtr, &IVersionable::getInterfacePointer, interfaceID, interfaceVersion);
		}

		pvoid APICALL getInterfacePointer(uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error) __NOTHROW__{
			//assert( false );
			return mRawPtr->getInterfacePointer(interfaceID, interfaceVersion, error);
		}


		spcIStructureNode APICALL GetComponentMetadata() const {
			return CallConstSafeFunctionReturningPointer<
				ICompositionRelationship, pcIStructureNode_base, const IStructureNode
			>(mRawPtr, &ICompositionRelationship::getComponentMetadata);
		}

		pcIStructureNode_base APICALL getComponentMetadata(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getComponentMetadata(error);
		}

		spcIAssetPart APICALL GetComponentPart() const {
			return CallConstSafeFunctionReturningPointer<
				ICompositionRelationship, pcIAssetPart_base, const IAssetPart>(
				mRawPtr, &ICompositionRelationship::getComponentPart);
		}

		pcIAssetPart_base APICALL getComponentPart(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getComponentPart(error);
		}

		spcIStructureNode APICALL GetComposedMetadata() const {
			return CallConstSafeFunctionReturningPointer<
				ICompositionRelationship, pcIStructureNode_base, const IStructureNode>(
				mRawPtr, &ICompositionRelationship::getComposedMetadata);
		}

		pcIStructureNode_base APICALL getComposedMetadata(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getComposedMetadata(error);
		}

		spcIAssetPart APICALL GetComposedPart() const {
			return CallConstSafeFunctionReturningPointer<
				ICompositionRelationship, pcIAssetPart_base, const IAssetPart>(
				mRawPtr, &ICompositionRelationship::getComposedPart);
		}

		pcIAssetPart_base APICALL getComposedPart(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getComposedPart(error);
		}

		spcIUTF8String APICALL GetMappingFunctionName() const {
			return CallConstSafeFunctionReturningPointer<
				ICompositionRelationship, pcIUTF8String_base, const IUTF8String
			>(mRawPtr, &ICompositionRelationship::getMappingFunctionName);
		}

		pcIUTF8String_base APICALL getMappingFunctionName(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getMappingFunctionName(error);
		}

		void APICALL SetMappingFunctionName(const char * name, sizet nameLength) {
			mRawPtr->SetMappingFunctionName(name, nameLength);
		}

		ICompositionRelationship_v1::eCompositionMaskMarkers APICALL GetMaskMarkersFlag() const {
			return CallConstSafeFunction< ICompositionRelationship, eCompositionMaskMarkers, uint32 >(
				mRawPtr, &ICompositionRelationship::getMaskMarkersFlag);
		}

		uint32 APICALL getMaskMarkersFlag(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getMaskMarkersFlag(error);
		}

		void APICALL SetMaskMarkersFlag(eCompositionMaskMarkers flag) {
			CallSafeFunctionReturningVoid< ICompositionRelationship, uint32 >(
				mRawPtr, &ICompositionRelationship::setMaskMarkersFlag, static_cast<uint32>(flag));
		}

		void APICALL setMaskMarkersFlag(uint32 flag, pcIError_base & error) __NOTHROW__{
			return mRawPtr->setMaskMarkersFlag(flag, error);
		}

		spcIUTF8String APICALL GetComponentDocumentID() const {
			return CallConstSafeFunctionReturningPointer< ICompositionRelationship, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &ICompositionRelationship::getComponentDocumentID);
		}

		pcIUTF8String_base APICALL getComponentDocumentID(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getComponentDocumentID(error);
		}

		spcIUTF8String APICALL GetComposedDocumentID() const {
			return CallConstSafeFunctionReturningPointer< ICompositionRelationship, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &ICompositionRelationship::getComposedDocumentID);
		}

		pcIUTF8String_base APICALL getComposedDocumentID(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getComposedDocumentID(error);
		}

		spcIUTF8String APICALL GetComponentInstanceID() const {
			return CallConstSafeFunctionReturningPointer< ICompositionRelationship, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &ICompositionRelationship::getComponentInstanceID);
		}

		pcIUTF8String_base APICALL getComponentInstanceID(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getComponentInstanceID(error);
		}

		spcIUTF8String APICALL GetComposedInstanceID() const {
			return CallConstSafeFunctionReturningPointer< ICompositionRelationship, pcIUTF8String_base, const IUTF8String >(
				mRawPtr, &ICompositionRelationship::getComposedInstanceID);
		}

		pcIUTF8String_base APICALL getComposedInstanceID(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getComposedInstanceID(error);
		}

		void APICALL SetComponentFilePath(const char * filePath, sizet filePathLength) {
			mRawPtr->SetComponentFilePath(filePath, filePathLength);
		}

		spcIUTF8String APICALL GetComponentFilePath() const {
			return CallConstSafeFunctionReturningPointer<
				ICompositionRelationship, pcIUTF8String_base, const IUTF8String
			>(mRawPtr, &ICompositionRelationship::getComponentFilePath);
		}

		pcIUTF8String_base APICALL getComponentFilePath(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->getComponentFilePath(error);
		}

		void APICALL AppendComponentAlternatePath(const char * alternatePath, sizet altPathLength) {
			CallSafeFunctionReturningVoid< ICompositionRelationship, const char * , sizet>(
				mRawPtr, &ICompositionRelationship::appendComponentAlternatePath, alternatePath, altPathLength);
		}

		void APICALL appendComponentAlternatePath(const char * alternatePath, sizet altPathLength, pcIError_base & error) __NOTHROW__{
			return mRawPtr->appendComponentAlternatePath(alternatePath, altPathLength, error);
		}

		spcIUTF8Strings APICALL GetComponentAlternatePaths() const {
			pcIError_base error(NULL);
			spcIUTF8Strings list = shared_ptr<cIUTF8Strings>(new cIUTF8Strings());
			sizet count = mRawPtr->getComponentAlternatePathsSize();
			list->reserve(count);
			for (sizet i = 0; i < count; i++) {
				spcIUTF8String string = IUTF8String::MakeShared(mRawPtr->getComponentAlternatePathAtIndex(i, error));
				if (error)
					//throw NS_COMMON::MakeSharedPointer< const IError >(error);  //refactor  MakeSharedPointer used to throw error if value is NULL 
					throw IError::MakeShared(error);
				list->push_back(string);
			}
			return list;
		}

		void APICALL ClearComponentAlternatePaths() {
			mRawPtr->ClearComponentAlternatePaths();
		}

		sizet APICALL getComponentAlternatePathsSize() __NOTHROW__{
			return mRawPtr->getComponentAlternatePathsSize();
		}

		pcIUTF8String_base APICALL getComponentAlternatePathAtIndex(sizet index, pcIError_base & error) __NOTHROW__{
			return mRawPtr->getComponentAlternatePathAtIndex(index, error);
		}
	};
	spICompositionRelationship ICompositionRelationship_v1::MakeShared(pICompositionRelationship_base ptr) {
		if ( !ptr ) return spICompositionRelationship();
		pICompositionRelationship p = ICompositionRelationship::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< ICompositionRelationship >() : ptr;
		return shared_ptr< ICompositionRelationship >(new ICompositionRelationshipProxy(p));
	}

}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
