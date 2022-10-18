#ifndef IExtensionsObjectFactory_h__
#define IExtensionsObjectFactory_h__ 1

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
//  SKP	Sunil Kishor Pathak
//  ANS	Ankita Sharma
//
// mm-dd-yy who Description of changes, most recent first.
//
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 11-27-14 SKP 1.0-a017 Implemented Asset composition related functionality for basic part addition.
// 11-26-14 ADC 1.0-a013 Added the full skeleton for IComposedAssetManager interface.
// 11-24-14 ADC 1.0-a008 Support for ICompositionRelationship interface.
// 11-24-14 ANS 1.0-a007 Fixing build errors
// 11-23-14 SKP 1.0-a006 Support for IAssetPagePart interface.
// 11-21-14 ADC 1.0-a004 Support for IAssetLayerPart interface.
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"
#include "XMPCore/XMPCoreFwdDeclarations.h"
#include "XMPCommon/Interfaces/IObjectFactory.h"
#include "XMPMarker/XMPMarkerFwdDeclarations.h"

#if GENERATE_XMPEXTENSIONS_DLL_LAYER_CODE

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCore;

	//!
	//! \brief Version1 of the interface that represents a factory interface which allows to create
	//! various artifacts for Asset Management.
	//!
	//! \details Provides all the functions to create instances of various artifacts for Asset
	//! Management and return them as raw pointers to the clients. This is the interface through
	//! which client glue code of the library actually get access to all other interfaces.
	//! \attention Supports multi threading.
	//!
	class XMP_PUBLIC IExtensionsObjectFactory_v1
		:public virtual IObjectFactory_v1
	{
	public:

		//!
		//! \brief Get a pointer to an Asset Utilities object.
		//! \param[out] error filled with an address of an error object in case any error is
		//! encountered during the operation.
		//! \return On success, a pointer to #AdobeXMPAM::IAssetUtitlies_base object.
		//!
		virtual pIAssetUtilities_base APICALL GetAssetUtilities(pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of standard IAssetPart type.
		//! \param[in] component a value of type uint32 indicating the component of the asset this object is
		//! referring to.
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is
		//! encountered during the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetPart_base object.
		//!
		virtual pIAssetPart_base APICALL CreateStandardGenericPart(uint32 component, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of user defined IAssetPart type.
		//! \param[in] component pointer to a const NULL terminated char buffer containing component string of
		//! the asset this string is referring to.
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is
		//! encountered during the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetPart_base object.
		//!
		virtual pIAssetPart_base APICALL CreateUserDefinedGenericPart(const char * component, sizet length, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of standard IAssetTimePart type.
		//! \param[in] component a value of type uint32 indicating the component of the asset this object is
		//! referring to.
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is
		//! encountered during the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetTimePart_base object.
		//!
		virtual pIAssetTimePart_base APICALL CreateStandardTimePart(uint32 component, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of user defined IAssetTimePart type.
		//! \param[in] component pointer to a const NULL terminated char buffer containing component string of
		//! the asset this string is referring to.
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is
		//! encountered during the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetTimePart_base object.
		//!
		virtual pIAssetTimePart_base APICALL CreateUserDefinedTimePart(const char * component, sizet componentLength, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of standard IAssetLayerPart type.
		//! \param[in] component a value of type uint32 indicating the component of the asset this object is
		//! referring to.
		//! \param[in] id pointer to a const NULL terminated char buffer containing containing id of the layer.
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is encountered during
		//! the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetLayerPart_base object.
		//!
		virtual pIAssetLayerPart_base APICALL CreateStandardLayerPart(uint32 component, const char * id, sizet idLength, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of standard IAssetLayerPart type.
		//! \param[in] component pointer to a const NULL terminated char buffer containing component string of
		//! the asset this string is referring to.
		//! \param[in] id pointer to a const NULL terminated char buffer containing containing id of the layer.
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is encountered during
		//! the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetLayerPart_base object.
		//!
		virtual pIAssetLayerPart_base APICALL CreateUserDefinedLayerPart(const char * component, sizet componentLength, const char * id, sizet idLength, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of standard IAssetPagePart type.
		//! \param[in] component a value of type uint32 indicating the component of the asset this object is
		//! referring to.
		//! \param[in] no containing page number
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is encountered during
		//! the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetPagePart_base object.
		//!
		virtual pIAssetPagePart_base APICALL CreateStandardPagePart(uint32 component, sizet pageNumber, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of user defined IAssetPagePart type.
		//! \param[in] component pointer to a const NULL terminated char buffer containing component string of
		//! the asset this string is referring to.
		//! \param[in] pageNumber containing page number
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is encountered during
		//! the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetPagePart_base object.
		//!
		virtual pIAssetPagePart_base APICALL CreateUserDefinedPagePart(const char * component, sizet length, sizet pageNumber, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of standard IAssetArtboardPart type.
		//! \param[in] component a value of type uint32 indicating the component of the asset this object is
		//! referring to.
		//! \param[in] id pointer to a const NULL terminated char buffer containing containing id of the layer.
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is encountered during
		//! the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetArtboardPart_base object.
		//!
		virtual pIAssetArtboardPart_base APICALL CreateStandardArtboardPart(uint32 component, const char * id, sizet idLength, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of user defined IAssetArtboardPart type.
		//! \param[in] component pointer to a const NULL terminated char buffer containing component string of
		//! the asset this string is referring to.
		//! \param[in] id pointer to a const NULL terminated char buffer containing containing id of the layer.
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is encountered during
		//! the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IAssetArtboardPart_base object.
		//!
		virtual pIAssetArtboardPart_base APICALL CreateUserDefinedArtboardPart(const char * component, sizet componentLength, const char * id, sizet idLength, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Create an object of composed Asset Manager.
		//! \param[in] composedAssetMetadata pointer to an object of \#NS_XMPCORE::IXMPMetadata_base type containing
		//! all the metadata associated with the composed asset.
		//! \param[in] multiThreadingSupport a value of type uint32 controlling whether the new object created
		//! should be thread safe or not.
		//! \param[out] error filled with an address of an error object in case any error is encountered during
		//! the operation.
		//! \return On success, a pointer to \#AdobeXMPAM::IComposedAssetManager_base object.
		//!
		virtual pIComposedAssetManager_base APICALL CreateComposedAssetManager(pIMetadata_base composedAssetMetadata, pcIError_base & error) __NOTHROW__ = 0;


		virtual pITrack_base APICALL CreateTrack(const char * trackName, sizet trackNameLen, uint64 trackTypes, const char * trackTypeName /* = NULL */, sizet trackTypeNameLen, pcIError_base &error) __NOTHROW__ = 0;


		virtual pITemporalAssetManager_base APICALL CreateTemporalAssetManager(pIMetadata_base composedAssetMetadata, pcIError_base & error) __NOTHROW__ = 0;

		//!
		//! \brief Get a pointer to an Asset Management Object factory.
		//! \return a pointer to AdobeXMPAM::IExtensionsObjectFactory object.
		//!
		XMP_PRIVATE static pIExtensionsObjectFactory GetExtensionsObjectFactory();

		XMP_PRIVATE static pIExtensionsObjectFactory MakeExtensionsObjectFactory(pIExtensionsObjectFactory_base ptr);
		XMP_PRIVATE static pcIExtensionsObjectFactory MakeExtensionsObjectFactory(pcIExtensionsObjectFactory_base ptr) {
            
            return MakeExtensionsObjectFactory(const_cast< pcIExtensionsObjectFactory_base >(ptr));
           
		}

		virtual pIExtensionsObjectFactory APICALL GetActualIExtensionsObjectFactory() __NOTHROW__ = 0;
		XMP_PRIVATE pcIExtensionsObjectFactory GetActualIExtensionsObjectFactory() const __NOTHROW__{
			return const_cast<IExtensionsObjectFactory_v1 *>(this)->GetActualIExtensionsObjectFactory();
		}
		virtual AdobeXMPAM_Int::pIExtensionsObjectFactory_I APICALL GetIExtensionsObjectFactory_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcIExtensionsObjectFactory_I GetIExtensionsObjectFactory_I() const __NOTHROW__{
			return const_cast< IExtensionsObjectFactory_v1 * >(this)->GetIExtensionsObjectFactory_I();
		}

		XMP_PRIVATE static uint64 GetInterfaceID() { return kIExtensionsObjectFactoryID; }

		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

		virtual ~IExtensionsObjectFactory_v1() {}
	protected:
		//!
		//! protected Virtual Destructor
		//!
		//virtual ~IExtensionsObjectFactory_v1() {}

#ifdef FRIEND_CLASS_DECLARATION
		FRIEND_CLASS_DECLARATION();
#endif
		REQ_FRIEND_CLASS_DECLARATION();
	};

	/*#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	typedef BASE_CLASS( IExtensionsObjectFactory, CLIENT_IExtensionsObjectFACTORY_VERSION )
	IExtensionsObjectFactoryBase;

	class XMP_PRIVATE IExtensionsObjectFactory
	: public virtual IExtensionsObjectFactoryBase
	{
	public:
	IExtensionsObjectFactory( IExtensionsObjectFactoryBase * ptr ) : _ptr( ptr ) {}
	IExtensionsObjectFactory( const IExtensionsObjectFactoryBase * cptr ) : _ptr( const_cast< IExtensionsObjectFactoryBase * >( cptr ) ) {}

	virtual pIAssetUtilities_base APICALL GetAssetUtilities( pcIError_base & error ) __NOTHROW__;

	virtual pIAssetPart_base APICALL CreateStandardGenericPart(uint32 component, pcIError_base & error) __NOTHROW__;
	virtual pIAssetPart_base APICALL CreateUserDefinedGenericPart(const char * component, pcIError_base & error) __NOTHROW__;

	virtual pIAssetTimePart_base APICALL CreateStandardTimePart(uint32 component, pcIError_base & error) __NOTHROW__;
	virtual pIAssetTimePart_base APICALL CreateUserDefinedTimePart(const char * component, pcIError_base & error) __NOTHROW__;

	virtual pIAssetLayerPart_base APICALL CreateStandardLayerPart(uint32 component, const char * id, pcIError_base & error) __NOTHROW__;
	virtual pIAssetLayerPart_base APICALL CreateUserDefinedLayerPart(const char * component, const char * id, pcIError_base & error) __NOTHROW__;

	virtual pIAssetPagePart_base APICALL CreateStandardPagePart(uint32 component, sizet pageNumber, pcIError_base & error) __NOTHROW__;
	virtual pIAssetPagePart_base APICALL CreateUserDefinedPagePart(const char * component, sizet pageNumber, pcIError_base & error) __NOTHROW__;

	virtual pIAssetArtboardPart_base APICALL CreateStandardArtboardPart(uint32 component, const char * id, pcIError_base & error) __NOTHROW__;
	virtual pIAssetArtboardPart_base APICALL CreateUserDefinedArtboardPart(const char * component, const char * id, pcIError_base & error) __NOTHROW__;

	pIComposedAssetManager_base CreateComposedAssetManager(pIMetadata_base composedAssetMetadata, pcIError_base & error) __NOTHROW__;

	virtual ~IExtensionsObjectFactory() __NOTHROW__ {};

	private:
	IExtensionsObjectFactoryBase * _ptr;
	};


	#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	*/
}

#endif  // GENERATE_XMPEXTENSIONS_DLL_LAYER_CODE

#endif  // IExtensionsObjectFactory_h__
