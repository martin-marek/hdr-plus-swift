#ifndef IComposedAssetManager_h__
#define IComposedAssetManager_h__ 1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"

#include "XMPAssetManagement/Interfaces/IAssetManager.h"

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;

	//!
	//! \brief Version1 of the interface that represents an object which can use to manage
	//! relationships for a composite document.
	//! \details It provides functions to
	//!		-# To add new components to a composite asset.
	//!		-# Remove components.
	//!		-# Get relationships between composite asset and the component asset.
	//!		-# Check whether a particular asset is using a component asset.
	//! \attention Support multi threading through locks but can be disabled by the client.
	//!
	class XMP_PUBLIC IComposedAssetManager_v1
		: public virtual IAssetManager_v1
	{
	public:

		//!
		//! \brief Add a component asset to a composite asset mentioning the part of component that is
		//! being used along with the part of the composite asset where it will be placed.
		//! \param[in] composedPart a shared pointer to a const \#AdobeXMPAM::IAssetPart
		//! mentioning the part in the composite asset where the component asset's part will be
		//! placed.
		//! \param[in] component a shared pointer of const \#NS_XMPCORE::IXMPStructureNode containing
		//! the metadata of the component asset.
		//! \param[in] componentPart a shared pointer of const \#AdobeXMPAM::IAssetPart
		//! mentioning the part of the component asset that is being used.
		//! \return a shared pointer of \#AdobeXMPAM::ICompositionRelationship containing
		//! all the aspects of the relationship between the composite asset's part with the component
		//! asset's part.
		//! \attention Error is thrown in case
		//!		-# any of the shared pointer is invalid.
		//!		-# component is non track able asset.
		//!
		virtual spICompositionRelationship APICALL AddComponent( const spcIAssetPart & composedPart,
			const spcIStructureNode & component, const spcIAssetPart & componentPart ) = 0;

		//!
		//! \brief Remove all the composition relationships maintained in the metadata of the composite document.
		//! \return a value of type \#NS_XMPCOMMON::sizet containing the number of relationships removed.
		//!
		virtual sizet APICALL RemoveAllComponents() __NOTHROW__ = 0;

		//!
		//! \brief Remove all the composition relationships maintained in the metadata of the composite document
		//! for the given component whose metadata is provided and an optional component part.
		//! \param[in] component a shared pointer of const \#NS_XMPCORE::IXMPStructureNode containing
		//! the metadata of the component asset.
		//! \param[in] componentPart a shared pointer of const \#AdobeXMPAM::IAssetPart
		//! mentioning the part of the component asset that is being used.
		//! \return a value of type \#NS_XMPCOMMON::sizet containing the number of relationships removed.
		//! \attention Error is thrown in case
		//!		-# component is an invalid shared pointer.
		//!		-# component is non track able asset.
		//! \note setting componentPart as invalid pointer means ignore the part.
		//!
		virtual sizet APICALL RemoveComponents(const spcIStructureNode & component, const spcIAssetPart & componentPart = spcIAssetPart()) = 0;

		//!
		//! \brief Remove all the composition relationships maintained in the metadata of the composite document
		//! for the component whose documentID and instanceID (optional ) are provided and an optional
		//! component part.
		//! \param[in] documentID a pointer to a const NULL terminated char buffer containing the document
		//! ID of the component's asset.
		//! \param[in] instanceID a pointer to a const NULL terminated char buffer containing the instance
		//! ID of the component's asset.
		//! \param[in] componentPart a shared pointer of const \#AdobeXMPAM::IAssetPart
		//! mentioning the part of the component asset that is being used.
		//! \param[in] docIDLength Length of char buffer containing the document
		//! ID of the component's asset.
		//! \param[in] insIDLength Length of char buffer containing the instance
		//! ID of the component's asset.
		//! \return a value of type \#NS_XMPCOMMON::sizet containing the number of relationships removed.
		//! \attention Error is thrown in case
		//!		-# documentID is a null pointer or it contents are empty.
		//!		-# instanceID is non null pointer and its contents are empty.
		//! \note setting instanceID as NULL means ignore the instanceIDs.
		//! \note setting componentPart as invalid pointer means ignore the part.
		//!
		virtual sizet APICALL RemoveComponents(const char * documentID, sizet docIDLength, const char * instanceID = NULL,sizet insIDLength = AdobeXMPCommon::npos, const spcIAssetPart & componentPart = spcIAssetPart()) = 0;

		//!
		//! \brief Remove all the composition relationships maintained in the metadata of the composite document
		//! for a given composite asset part.
		//! \param[in] composedPart a shared pointer of const \#AdobeXMPAM::IAssetPart mentioning
		//! the part of the composite asset whose relationships are no longer required.
		//! \return a value of type \#NS_XMPCOMMON::sizet containing the number of relationships removed.
		//! \attention Error is thrown in case
		//!		-# composedPart is an invalid shared pointer.
		//! 
		virtual sizet APICALL RemoveComponents(const spcIAssetPart & composedPart) = 0;

		//!
		//! \brief Remove the composition relationship maintained in the metadata of the composite document for a
		//! given composite asset part.
		//! \param[in] relationship of a shared pointer of const \#AdobeXMPAM::ICompositionRelationship
		//!  mentioning the relationship which is no longer required.
		//! \return a value of type true if relationship has been removed otherwise false.
		//! \attention Error is thrown in case
		//!		-# relationship is an invalid shared pointer.
		//! 
		virtual bool APICALL RemoveComponent(const spcICompositionRelationship& relationship) = 0;

		//!
		//! \brief Get the list of all the composition relationships for a composite asset.
		//! \return a vector consisting of shared pointers of const \#AdobeXMPAM::ICompositionRelationships.
		//
		virtual spcICompositionRelationshipList APICALL GetAllRelationships() const = 0;

		//!
		//! \brief Get the list of all the composition relationships for a composite asset for a given component
		//! and an optional componentPart.
		//! \param[in] component a shared pointer of const \#NS_XMPCORE::IXMPStructureNode containing
		//! the metadata of the component asset.
		//! \param[in] componentPart a shared pointer of const \#AdobeXMPAM::IAssetPart
		//! mentioning the part of the component asset that is being used.
		//! \return a a vector consisting of shared pointers of const \#AdobeXMPAM::ICompositionRelationships
		//! which meets the given criteria.
		//! \attention Error is thrown in case
		//!		-# component is an invalid shared pointer.
		//!		-# component is non track able asset.
		//! \note setting componentPart as invalid pointer means ignore the part.
		//!
		virtual spcICompositionRelationshipList APICALL GetRelationships(const spcIStructureNode & component,
			const spcIAssetPart & componentPart = spcIAssetPart() ) const = 0;

		//!
		//! \brief Get the list of all the composition relationships for a composite asset whose documentID and
		//! instanceID( optional ) are provided and an optional component part.
		//! \param[in] documentID a pointer to a const NULL terminated char buffer containing the document
		//! ID of the component's asset.
		//! \param[in] instanceID a pointer to a const NULL terminated char buffer containing the instance
		//! ID of the component's asset.
		//! \param[in] componentPart a shared pointer of const \#AdobeXMPAM::IAssetPart
		//! mentioning the part of the component asset that is being used.
		//! \param[in] docIDLength Length of char buffer containing the document
		//! ID of the component's asset.
		//! \param[in] insIDLength Length of char buffer containing the instance
		//! ID of the component's asset.
		//! \return a a vector consisting of shared pointers of const \#AdobeXMPAM::ICompositionRelationships
		//! which meets the given criteria.
		//! \attention Error is thrown in case
		//!		-# documentID is a null pointer or it contents are empty.
		//!		-# instanceID is non null pointer and its contents are empty.
		//! \note setting instanceID as NULL means ignore the instanceIDs.
		//! \note setting componentPart as invalid pointer means ignore the part.for a given component
		//! and an optional componentPart.
		//!
		virtual spcICompositionRelationshipList APICALL GetRelationships(const char * documentID, sizet docIDLength, const char * instanceID = NULL, sizet insIDLength=AdobeXMPCommon::npos,
			const spcIAssetPart & componentPart = spcIAssetPart() ) const = 0;

		//!
		//! \brief Get the list of all the composition relationships for a composite asset for a given composite
		//! asset part.
		//! \param[in] composedPart a shared pointer of const \#AdobeXMPAM::IAssetPart mentioning
		//! the part of the composite asset whose relationships are no longer required.
		//! \return a a vector consisting of shared pointers of const \#AdobeXMPAM::ICompositionRelationships
		//! which meets the given criteria.
		//! \attention Error is thrown in case composedPart is an invalid shared pointer.
		//!
		virtual spcICompositionRelationshipList APICALL GetRelationships(const spcIAssetPart & composedPart) const = 0;

		//!
		//! \brief Indicates whether a given component whose metadata is provided is directly used in the composite.
		//! \param[in] component a shared pointer of const \#NS_XMPCORE::IXMPStructureNode containing
		//! the metadata of the component asset.
		//! \param[in] componentPart a shared pointer of const \#AdobeXMPAM::IAssetPart
		//! mentioning the part of the component asset that is being used.
		//! \return a bool value returning true in case a given component is directly used in the composite.
		//! \attention Error is thrown in case
		//!		-# component is an invalid shared pointer.
		//!		-# component is non track able asset.
		//! \note setting componentPart as invalid pointer means ignore the part.
		//!
		virtual bool APICALL IsDirectlyComposedOf(const spcIStructureNode & component, const spcIAssetPart & componentPart = spcIAssetPart()) const = 0;

		//!
		//! \brief Indicates whether a given component whose metadata is provided is directly used in the composite.
		//! \param[in] documentID a pointer to a const NULL terminated char buffer containing the document
		//! ID of the component's asset.
		//! \param[in] instanceID a pointer to a const NULL terminated char buffer containing the instance
		//! ID of the component's asset.
		//! \param[in] componentPart a shared pointer of const \#AdobeXMPAM::IAssetPart
		//! mentioning the part of the component asset that is being used.
		//! \param[in] docIDLength Length of char buffer containing the document
		//! ID of the component's asset.
		//! \param[in] insIDLength Length of char buffer containing the instance
		//! ID of the component's asset.
		//! \return a bool value returning true in case a given component is directly used in the composite.
		//! \attention Error is thrown in case
		//!		-# component is an invalid shared pointer.
		//!		-# component is non track able asset.
		//! \note setting instanceID as NULL means ignore the instanceIDs.
		//! \note setting componentPart as invalid pointer means ignore the part.
		//!
		virtual bool APICALL IsDirectlyComposedOf(const char * documentID, sizet docIDLength, const char * instanceID = NULL, sizet insIDLength = AdobeXMPCommon::npos,
			const spcIAssetPart & componentPart = spcIAssetPart() ) const = 0;
		
		//!
		//! \brief Indicates whether a given component whose metadata is provided is [in]directly used in the composite.
		//! \param[in] component a shared pointer of const \#NS_XMPCORE::IXMPStructureNode containing
		//! the metadata of the component asset.
		//! \param[in] componentPart a shared pointer of const \#AdobeXMPAM::IAssetPart
		//! mentioning the part of the component asset that is being used.
		//! \return a bool value returning true in case a given component is [in]directly used in the composite.
		//! \attention Error is thrown in case
		//!		-# component is an invalid shared pointer.
		//!		-# component is non track able asset.
		//! \note setting componentPart as invalid pointer means ignore the part.
		//!
		virtual bool APICALL IsComposedOf(const spcIStructureNode & component, const spcIAssetPart & componentPart = spcIAssetPart()) const = 0;

		//!
		//! \brief Indicates whether a given component whose metadata is provided is [in]directly used in the composite.
		//! \param[in] documentID a pointer to a const NULL terminated char buffer containing the document
		//! ID of the component's asset.
		//! \param[in] instanceID a pointer to a const NULL terminated char buffer containing the instance
		//! ID of the component's asset.
		//! \param[in] componentPart a shared pointer of const \#AdobeXMPAM::IAssetPart
		//! mentioning the part of the component asset that is being used.
		//! \param[in] docIDLength Length of char buffer containing the document
		//! ID of the component's asset.
		//! \param[in] insIDLength Length of char buffer containing the instance
		//! ID of the component's asset.
		//! \return a bool value returning true in case a given component is [in]directly used in the composite.
		//! \attention Error is thrown in case
		//!		-# component is an invalid shared pointer.
		//!		-# component is non track able asset.
		//! \note setting instanceID as NULL means ignore the instanceIDs.
		//! \note setting componentPart as invalid pointer means ignore the part.
		//!
		virtual bool APICALL IsComposedOf(const char * documentID, sizet docIDLength, const char * instanceID = NULL, sizet insIDLength = AdobeXMPCommon::npos,
			const spcIAssetPart & componentPart = spcIAssetPart() ) const = 0;

		// all static functions

		XMP_PRIVATE static spIComposedAssetManager CreateComposedAssetManager( const spIMetadata & metadata);

		virtual pIComposedAssetManager APICALL GetActualIComposedAssetManager() __NOTHROW__ = 0;
		XMP_PRIVATE pcIComposedAssetManager GetActualIComposedAssetManager() const __NOTHROW__{
			return const_cast<IComposedAssetManager_v1 *>(this)->GetActualIComposedAssetManager();
		}

		virtual AdobeXMPAM_Int::pIComposedAssetManager_I APICALL GetIComposedAssetManager_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcIComposedAssetManager_I GetIComposedAssetManager_I() const __NOTHROW__{
			return const_cast< IComposedAssetManager_v1 * >(this)->GetIComposedAssetManager_I();
		}

		XMP_PRIVATE static uint64 GetInterfaceID() { return kIComposedAssetManagerID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

		XMP_PRIVATE static spIComposedAssetManager MakeShared(pIComposedAssetManager_base ptr);
		XMP_PRIVATE static spcIComposedAssetManager MakeShared(pcIComposedAssetManager_base ptr) {
			return MakeShared(const_cast<pIComposedAssetManager_base>(ptr));
		}

	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		// all safe functions
		virtual pICompositionRelationship_base APICALL addComponent(pcIAssetPart_base composedPart,
			pcIStructureNode_base component, pcIAssetPart_base componentPart, pcIError_base & error ) __NOTHROW__ = 0;

		virtual sizet APICALL removeComponents(pcIStructureNode_base component, pcIAssetPart_base componentPart, pcIError_base & error) __NOTHROW__ = 0;
		virtual sizet APICALL removeComponents(const char * documentID, sizet docIDLength, const char * instanceID, sizet insIDLength, pcIAssetPart_base componentPart, pcIError_base & error) __NOTHROW__ = 0;
		virtual sizet APICALL removeComponents(pcIAssetPart_base composedPart, pcIError_base & error) __NOTHROW__ = 0;
		virtual bool APICALL removeComponent(pcICompositionRelationship_base relationship, pcIError_base & error) __NOTHROW__ = 0;

		virtual sizet APICALL getAllRelationships(pcICompositionRelationship_base *& array, MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__ = 0;
		virtual sizet APICALL getRelationships(pcIStructureNode_base component, pcIAssetPart_base, pcICompositionRelationship_base *& array, MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__ = 0;
		virtual sizet APICALL getRelationships(const char * documentID, sizet docIDLength, const char * instanceID, sizet insIDLength, pcIAssetPart_base componentPart, pcICompositionRelationship_base *& array, MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__ = 0;
		virtual sizet APICALL getRelationships(pcIAssetPart_base composedPart, pcICompositionRelationship_base *& array, MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__ = 0;

		virtual uint32 APICALL isDirectlyComposedOf(pcIStructureNode_base component, pcIAssetPart_base componentPart, pcIError_base & error) const __NOTHROW__ = 0;
		virtual uint32 APICALL isDirectlyComposedOf(const char * documentID, sizet docIDLength, const char * instanceID, sizet insIDLength, pcIAssetPart_base, pcIError_base & error) const __NOTHROW__ = 0;

		virtual uint32 APICALL isComposedOf(pcIStructureNode_base component, pcIAssetPart_base componentPart, pcIError_base & error) const __NOTHROW__ = 0;
		virtual uint32 APICALL isComposedOf(const char * documentID, sizet docIDLength, const char * instanceID, sizet insIDLength, pcIAssetPart_base componentPart, pcIError_base & error) const __NOTHROW__ = 0;
		//! \endcond

	protected:
		//!
		//! protected Virtual Destructor
		//!
		virtual ~IComposedAssetManager_v1() {};
	#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
	#endif
			REQ_FRIEND_CLASS_DECLARATION();
	
	};

	

/*#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	typedef BASE_CLASS( IComposedAssetManager, CLIENT_ICOMPOSEDASSETMANAGER_VERSION ) IComposedAssetManagerBase;

	class IComposedAssetManager : public virtual IComposedAssetManagerBase {
	//! \cond XMP_INTERNAL_DOCUMENTATION
	public:
		typedef IComposedAssetManagerBase *											pIComposedAssetManagerBase;
		typedef const IComposedAssetManagerBase *									pcIComposedAssetManagerBase;
		typedef shared_ptr< IComposedAssetManagerBase >								spIComposedAssetManagerBase;
		typedef shared_ptr< const IComposedAssetManagerBase >						spcIComposedAssetManagerBase;

		public:
		IComposedAssetManager( const spIComposedAssetManagerBase & sp ) : _sp( sp ) {}
		IComposedAssetManager( const spcIComposedAssetManagerBase & csp ) : _sp( const_pointer_cast< IComposedAssetManagerBase > ( csp ) ) {}
		
		//virtual void AcquireSharedOwnership() const __NOTHROW__ {};
		//virtual void ReleaseSharedOwnership() const __NOTHROW__ {};

		

		virtual ~IComposedAssetManager() __NOTHROW__ {}

		pcIComposedAssetManagerBase GetRawPointer() const { return _sp.get(); }
		pIComposedAssetManagerBase GetRawPointer() { return _sp.get(); }

		virtual spICompositionRelationship APICALL AddComponent( const spcIAssetPart & composedPart, const spcIStructureNode & component, const spcIAssetPart & componentPart );
		virtual pICompositionRelationship_base APICALL AddComponent(pcIAssetPart_base composedPart, pcIStructureNode_base component, pcIAssetPart_base componentPart, pcIError_base & error) __NOTHROW__;

		virtual sizet APICALL RemoveAllComponents() __NOTHROW__;

		virtual sizet APICALL RemoveComponents(const spcIStructureNode & component, const spcIAssetPart & componentPart = spcIAssetPart());
		virtual sizet APICALL RemoveComponents(const char * documentID, const char * instanceID = NULL, const spcIAssetPart & componentPart = spcIAssetPart());
		virtual sizet APICALL RemoveComponents(const spcIAssetPart & composedPart);
		virtual bool APICALL RemoveComponent(const spcICompositionRelationship& relationship);

		virtual sizet APICALL RemoveComponents(pcIStructureNode_base component, pcIAssetPart_base componentPart, pcIError_base & error) __NOTHROW__;
		virtual sizet APICALL RemoveComponents(const char * documentID, const char * instanceID, pcIAssetPart_base componentPart, pcIError_base & error) __NOTHROW__;
		virtual sizet APICALL RemoveComponents(pcIAssetPart_base composedPart, pcIError_base & error) __NOTHROW__;
		virtual bool APICALL RemoveComponent(pcICompositionRelationship_base relationship, pcIError_base & error) __NOTHROW__;

		virtual spcICompositionRelationshipList APICALL GetAllRelationships() const;
		virtual sizet APICALL GetAllRelationships(pcICompositionRelationship_base *& array, MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__;

		virtual spcICompositionRelationshipList APICALL GetRelationships(const spcIStructureNode & component, const spcIAssetPart & componentPart = spcIAssetPart()) const;
		virtual spcICompositionRelationshipList APICALL GetRelationships(const char * documentID, const char * instanceID = NULL, const spcIAssetPart & componentPart = spcIAssetPart()) const;
		virtual spcICompositionRelationshipList APICALL GetRelationships(const spcIAssetPart & composedPart) const;
		virtual sizet APICALL GetRelationships(pcIStructureNode_base component, pcIAssetPart_base, pcICompositionRelationship_base *& array, MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__;
		virtual sizet APICALL GetRelationships(const char * documentID, const char * instanceID, pcIAssetPart_base componentPart, pcICompositionRelationship_base *& array, MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__;
		virtual sizet APICALL GetRelationships(pcIAssetPart_base composedPart, pcICompositionRelationship_base *& array, MemAllocateProc allocFunc, pcIError_base & error) const __NOTHROW__;

		virtual bool APICALL IsDirectlyComposedOf(const spcIStructureNode & component, const spcIAssetPart & componentPart = spcIAssetPart()) const;
		virtual bool APICALL IsDirectlyComposedOf(const char * documentID, const char * instanceID = NULL, const spcIAssetPart & componentPart = spcIAssetPart()) const;
		virtual uint32 APICALL IsDirectlyComposedOf(pcIStructureNode_base component, pcIAssetPart_base componentPart, pcIError_base & error) const __NOTHROW__;
		virtual uint32 APICALL IsDirectlyComposedOf(const char * documentID, const char * instanceID, pcIAssetPart_base, pcIError_base & error) const __NOTHROW__;

		virtual bool APICALL IsComposedOf(const spcIStructureNode & component, const spcIAssetPart & componentPart = spcIAssetPart()) const;
		virtual bool APICALL IsComposedOf(const char * documentID, const char * instanceID = NULL, const spcIAssetPart & componentPart = spcIAssetPart()) const;
		virtual uint32 APICALL IsComposedOf(pcIStructureNode_base component, pcIAssetPart_base componentPart, pcIError_base & error) const __NOTHROW__;
		virtual uint32 APICALL IsComposedOf(const char * documentID, const char * instanceID, pcIAssetPart_base componentPart, pcIError_base & error) const __NOTHROW__;

		void APICALL Acquire() const __NOTHROW__;
		void APICALL Release() const __NOTHROW__;
		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__;
		virtual void APICALL EnableThreadSafety() const __NOTHROW__;
		virtual void APICALL DisableThreadSafety() const __NOTHROW__;
		virtual bool APICALL IsThreadSafe() const;
		virtual AdobeXMPCommon_Int::pIThreadSafe_I APICALL GetIThreadSafe_I() __NOTHROW__;
	private:
		spIComposedAssetManagerBase					_sp;
	//! \endcond
	};

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	*/
}

#endif  // IComposedAssetManager_h__
