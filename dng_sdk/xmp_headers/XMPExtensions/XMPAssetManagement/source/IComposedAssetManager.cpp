// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

namespace AdobeXMPAM {
	class IComposedAssetManagerProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IComposedAssetManagerProxy;

#include "XMPAssetManagement/Interfaces/IComposedAssetManager.h"

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPExtensions/include/IExtensionsObjectFactory.h"
#include "XMPAssetManagement/Interfaces/ICompositionRelationship.h"
#include "XMPAssetManagement/Interfaces/IAssetPart.h"
#include "XMPCore/Interfaces/IStructureNode.h"
#include "XMPCore/Interfaces/IMetadata.h"
#include <cstdlib>

namespace AdobeXMPAM {

#if XMP_WinBuild
	#pragma warning( push )
	#pragma warning( disable : 4250 )
#endif

	class IComposedAssetManagerProxy
		: public virtual IComposedAssetManager
	{
	private:
		pIComposedAssetManager mRawPtr;

	public:
		IComposedAssetManagerProxy(pIComposedAssetManager ptr) : mRawPtr(ptr) {
			mRawPtr->Acquire();
		}

		~IComposedAssetManagerProxy() {
			mRawPtr->Release();
		}

		pIComposedAssetManager APICALL GetActualIComposedAssetManager() __NOTHROW__{
			return mRawPtr;
		}

		AdobeXMPAM_Int::pIComposedAssetManager_I APICALL GetIComposedAssetManager_I() __NOTHROW__{
			return mRawPtr->GetIComposedAssetManager_I();
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

		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__{
			return mRawPtr->GetISharedObject_I();
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



		spICompositionRelationship APICALL AddComponent(const spcIAssetPart & composedPart,
			const spcIStructureNode & component, const spcIAssetPart & componentPart)
		{
			return CallSafeFunctionReturningPointer<IComposedAssetManager, pICompositionRelationship_base, ICompositionRelationship,
				pcIAssetPart_base, pcIStructureNode_base, pcIAssetPart_base >(
				mRawPtr, &IComposedAssetManager::addComponent, composedPart? composedPart->GetActualIAssetPart() : NULL, component? component->GetActualIStructureNode():NULL, componentPart ? componentPart->GetActualIAssetPart() : NULL);
		}
		
		pICompositionRelationship_base APICALL addComponent(pcIAssetPart_base composedPart,
			pcIStructureNode_base component, pcIAssetPart_base componentPart, pcIError_base & error) __NOTHROW__
		{
			return mRawPtr->addComponent(composedPart, component, componentPart, error);
		}

			sizet APICALL RemoveAllComponents() __NOTHROW__{
			return mRawPtr->RemoveAllComponents();
		}

			sizet APICALL RemoveComponents(const spcIStructureNode & component,
			const spcIAssetPart & componentPart /*= spcIAssetPart() */)
		{
			return CallSafeFunction<IComposedAssetManager, sizet, sizet, pcIStructureNode_base, pcIAssetPart_base
			>(mRawPtr, &IComposedAssetManager::removeComponents, component ?component->GetActualIStructureNode():NULL, componentPart ? componentPart->GetActualIAssetPart() : NULL);
		}

		sizet APICALL RemoveComponents(const char * documentID,sizet docIDLength, const char * instanceID /*= NULL*/,sizet insIDLength,
			const spcIAssetPart & componentPart /*= spcIAssetPart() */)
		{
			return CallSafeFunction<IComposedAssetManager, sizet, sizet, const char *,sizet, const char *,sizet, pcIAssetPart_base>(
				mRawPtr, &IComposedAssetManager::removeComponents, documentID, docIDLength, instanceID, insIDLength, componentPart ? componentPart->GetActualIAssetPart() : NULL);
		}

		sizet APICALL RemoveComponents(const spcIAssetPart & composedPart) {
			return CallSafeFunction<IComposedAssetManager, sizet, sizet, pcIAssetPart_base>(
				mRawPtr, &IComposedAssetManager::removeComponents, composedPart ? composedPart->GetActualIAssetPart() : NULL);
		}
		//refactor CoreAPI should check this way

		bool APICALL RemoveComponent(const spcICompositionRelationship& relationship) {
			return CallSafeFunction<IComposedAssetManager, bool, bool, pcICompositionRelationship_base>(
				mRawPtr, &IComposedAssetManager::removeComponent, relationship ? relationship->GetActualICompositionRelationship() : NULL);
		}

		sizet APICALL removeComponents(pcIStructureNode_base component, pcIAssetPart_base componentPart,
			pcIError_base & error) __NOTHROW__
		{
			return mRawPtr->removeComponents(component, componentPart, error);
		}

			sizet APICALL removeComponents(const char * documentID, sizet docIDLength, const char * instanceID, sizet insIDLength,
			pcIAssetPart_base componentPart, pcIError_base & error) __NOTHROW__
		{
			return mRawPtr->removeComponents(documentID, docIDLength, instanceID, insIDLength, componentPart, error);
		}

			sizet APICALL removeComponents(pcIAssetPart_base composedPart, pcIError_base & error) __NOTHROW__{
			return mRawPtr->removeComponents(composedPart, error);
		}

			bool APICALL removeComponent(pcICompositionRelationship_base relationship, pcIError_base & error) __NOTHROW__{
			return mRawPtr->removeComponent(relationship, error);
		}

		static spcICompositionRelationshipList CreateRelationshipList(pcICompositionRelationship_base * ptr, sizet nElements) {
			spcICompositionRelationshipList list = shared_ptr<cICompositionRelationshipList>(new cICompositionRelationshipList());
			
			list->reserve(nElements);
			for (sizet i = 0; i < nElements; i++) {
				//auto sp = NS_XMPCOMMON::MakeSharedPointer< const ICompositionRelationship >(ptr[i]);
				list->push_back(ICompositionRelationship::MakeShared(ptr[i]));
			}
			free(ptr);
			return list;
		}

		static void * MyMalloc(sizet size) { return malloc(size); }

		spcICompositionRelationshipList APICALL GetAllRelationships() const {
			pcICompositionRelationship_base * ptr(NULL);
			sizet nElements = CallConstSafeFunction<IComposedAssetManager, sizet, sizet, pcICompositionRelationship_base *&, MemAllocateProc>(
				mRawPtr, &IComposedAssetManager::getAllRelationships, ptr, &MyMalloc);
			return CreateRelationshipList(ptr, nElements);
		}



		sizet APICALL getAllRelationships(pcICompositionRelationship_base *& array, MemAllocateProc allocFunc,
			pcIError_base & error) const __NOTHROW__
		{
			return mRawPtr->getAllRelationships(array, allocFunc, error);
		}

		spcICompositionRelationshipList APICALL GetRelationships(const spcIStructureNode & component,
			const spcIAssetPart & componentPart /*= spcIAssetPart() */) const
		{
			pcICompositionRelationship_base * ptr(NULL);
			sizet nElements = CallConstSafeFunction<
				IComposedAssetManager, sizet, sizet, pcIStructureNode_base, pcIAssetPart_base,
				pcICompositionRelationship_base *&, MemAllocateProc
			>(mRawPtr, &IComposedAssetManager::getRelationships, component?component->GetActualIStructureNode():NULL, componentPart ? componentPart->GetActualIAssetPart() : NULL, ptr, &MyMalloc);
			return CreateRelationshipList(ptr, nElements);
		}

		spcICompositionRelationshipList APICALL GetRelationships(const char * documentID, sizet docIDLength,
			const char * instanceID /*= NULL*/, sizet insIDLength, const spcIAssetPart & componentPart /*= spcIAssetPart() */) const
		{
			pcICompositionRelationship_base * ptr(NULL);
			sizet nElements = CallConstSafeFunction<
				IComposedAssetManager, sizet, sizet, const char *, sizet, const char *, sizet, pcIAssetPart_base,
				pcICompositionRelationship_base *&, MemAllocateProc
			>(mRawPtr, &IComposedAssetManager::getRelationships, documentID,docIDLength, instanceID, insIDLength, componentPart ? componentPart->GetActualIAssetPart() : NULL, ptr, &MyMalloc);
			return CreateRelationshipList(ptr, nElements);
		}

		spcICompositionRelationshipList APICALL GetRelationships(const spcIAssetPart & composedPart) const {
			pcICompositionRelationship_base * ptr(NULL);
			sizet nElements = CallConstSafeFunction<
				IComposedAssetManager, sizet, sizet, pcIAssetPart_base,
				pcICompositionRelationship_base *&, MemAllocateProc
			>(mRawPtr, &IComposedAssetManager::getRelationships, composedPart? composedPart->GetActualIAssetPart():NULL, ptr, &MyMalloc);
			return CreateRelationshipList(ptr, nElements);
		}

		sizet APICALL getRelationships(pcIStructureNode_base component, pcIAssetPart_base componentPart,
			pcICompositionRelationship_base *& array, MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__
		{
			return mRawPtr->getRelationships(component, componentPart, array, allocFunc, error);
		}

		sizet APICALL getRelationships(const char * documentID, sizet docIDLength, const char * instanceID, sizet insIDLength,
			pcIAssetPart_base componentPart, pcICompositionRelationship_base *& array,
			MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__
		{
			return mRawPtr->getRelationships(documentID, docIDLength, instanceID, insIDLength, componentPart, array, allocFunc, error);
		}

		sizet APICALL getRelationships(pcIAssetPart_base composedPart, pcICompositionRelationship_base *& array,
			MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__
		{
			return mRawPtr->getRelationships(composedPart, array, allocFunc, error);
		}

		bool APICALL IsDirectlyComposedOf(const spcIStructureNode & component,
			const spcIAssetPart & componentPart /*= spcIAssetPart() */) const
		{
			return CallConstSafeFunction<IComposedAssetManager, bool, uint32, pcIStructureNode_base, pcIAssetPart_base
			>(this, &IComposedAssetManager::isDirectlyComposedOf, component? component->GetActualIStructureNode():NULL, componentPart? componentPart->GetActualIAssetPart() : NULL);
		}

		bool APICALL IsDirectlyComposedOf(const char * documentID, sizet docIDLength, const char * instanceID /*= NULL*/,  sizet insIDLength,
			const spcIAssetPart & componentPart /*= spcIAssetPart() */) const
		{
			return CallConstSafeFunction<
				IComposedAssetManager, bool, uint32, const char *, sizet, const char *, sizet, pcIAssetPart_base
			>(this, &IComposedAssetManager::isDirectlyComposedOf, documentID, docIDLength, instanceID, insIDLength, componentPart ? componentPart->GetActualIAssetPart() : NULL);
		}

		uint32 APICALL isDirectlyComposedOf(pcIStructureNode_base component, pcIAssetPart_base componentPart,
			pcIError_base & error) const __NOTHROW__
		{
			return mRawPtr->isDirectlyComposedOf(component, componentPart, error);
		}

		uint32 APICALL isDirectlyComposedOf(const char * documentID, sizet docIDLength, const char * instanceID, sizet insIDLength,
			pcIAssetPart_base componentPart, pcIError_base & error) const __NOTHROW__
		{
			return mRawPtr->isDirectlyComposedOf(documentID, docIDLength, instanceID, insIDLength, componentPart, error);
		}

		bool APICALL IsComposedOf(const spcIStructureNode & component,
			const spcIAssetPart & componentPart /*= spcIAssetPart() */) const
		{
			return CallConstSafeFunction<
				IComposedAssetManager, bool, uint32, pcIStructureNode_base, pcIAssetPart_base
			>(this, &IComposedAssetManager::isComposedOf, component? component->GetActualIStructureNode():NULL, componentPart ? componentPart->GetActualIAssetPart() : NULL);
		}

		bool APICALL IsComposedOf(const char * documentID, sizet docIDLength, const char * instanceID /*= NULL*/, sizet insIDLength,
			const spcIAssetPart & componentPart /*= spcIAssetPart() */) const
		{
			return CallConstSafeFunction<
				IComposedAssetManager, bool, uint32, const char *, sizet, const char *, sizet, pcIAssetPart_base
			>(this, &IComposedAssetManager::isComposedOf, documentID, docIDLength, instanceID, insIDLength, componentPart? componentPart->GetActualIAssetPart() : NULL);
		}

		uint32 APICALL isComposedOf(pcIStructureNode_base component, pcIAssetPart_base componentPart,
			pcIError_base & error) const __NOTHROW__
		{
			return mRawPtr->isComposedOf(component, componentPart, error);
		}

		uint32 APICALL isComposedOf(const char * documentID, sizet docIDLength, const char * instanceID, sizet insIDLength,
			pcIAssetPart_base componentPart, pcIError_base & error) const __NOTHROW__
		{
			return mRawPtr->isComposedOf(documentID, docIDLength, instanceID, insIDLength, componentPart, error);
		}
	};

	spIComposedAssetManager IComposedAssetManager_v1::CreateComposedAssetManager( const spIMetadata & metadata)
	{
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIComposedAssetManager_base, IComposedAssetManager, pIMetadata_base>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateComposedAssetManager,
			metadata? metadata->GetActualIMetadata():NULL );
	}

	spIComposedAssetManager IComposedAssetManager_v1::MakeShared(pIComposedAssetManager_base ptr) {
		if ( !ptr ) return spIComposedAssetManager();
		pIComposedAssetManager p = IComposedAssetManager::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IComposedAssetManager >() : ptr;
		return shared_ptr< IComposedAssetManager >(new IComposedAssetManagerProxy(p));
	}
#if XMP_WinBuild
	#pragma warning( pop )
#endif

}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE