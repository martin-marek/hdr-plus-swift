#ifndef IAssetUtilities_h__
#define IAssetUtilities_h__ 1

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
//	ANS	Ankita Sharma
//	SKP Sunil Kishor Pathak
//
// mm-dd-yy who Description of changes, most recent first.
//
// 12-05-14 SKP 1.0-a038 Changed API name from IsTrackAble() to IsTrackable() and from 
//						 MakeTrackAble() to MakeTrackable()
// 11-27-14 ADC 1.0-a016 Submitting changes related to iOS build.
// 11-21-14 ANS 1.0-a005 Fixing build issues in client glue code for AssetUtilities
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IThreadSafe.h"

#include "XMPCore/XMPCoreFwdDeclarations.h"

namespace AdobeXMPAM {

	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCore;

	//!
	//! \brief Version1 of the interface that provides some basic utilities on an Asset's metadata.
	//! \details Provides functions to 
	//!		-# check whether an asset is track able.
	//!		-# add sufficient information to an asset's metadata to make it track able.
	//! \attention Support Multi-threading at object level.
	//!
	class XMP_PUBLIC IAssetUtilities_v1
		: public virtual ISharedObject
		, public virtual IVersionable
	{
	public:

		//!
		//! \brief Indicates whether an asset's metadata has sufficient information so that it can be tracked.
		//! \param[in] metadata a shared pointer to const \#NS_XMPCORE::IXMPMetadata object containing assets metadata.
		//! \return a bool value indicating whether asset can be tracked or not.
		//! \note In case metadata is invalid it returns false.
		//!
		virtual bool APICALL IsTrackable( const spcIMetadata & metadata ) = 0;

		//!
		//! \brief Enrich an asset's metadata if required so that there is sufficient information available to track it.
		//! \param[in] metadata a shared pointer to \#NS_XMPCORE::IXMPMetadata object containing assets metadata.
		//! \return a bool value indicating whether any enrichment is performed or not.
		//! \attention Error is thrown in case:
		//!		-# metadata is an invalid shared pointer.
		//!
		virtual bool APICALL MakeTrackable(const spIMetadata & metadata) = 0;
	
		XMP_PRIVATE static pIAssetUtilities GetAssetUtilities();
		
		XMP_PRIVATE static spIAssetUtilities MakeShared(pIAssetUtilities_base ptr);
		XMP_PRIVATE static spcIAssetUtilities MakeShared(pcIAssetUtilities_base ptr) {
			return MakeShared(const_cast< pIAssetUtilities_base >(ptr));
		}

		virtual pIAssetUtilities APICALL GetActualIAssetUtilities() __NOTHROW__ = 0;
		XMP_PRIVATE pcIAssetUtilities GetActualIAssetUtilities() const __NOTHROW__{
			return const_cast< IAssetUtilities_v1 * >(this)->GetActualIAssetUtilities();
		}

		virtual AdobeXMPAM_Int::pIAssetUtilities_I APICALL GetIAssetUtilities_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPAM_Int::pcIAssetUtilities_I GetIAssetUtilities_I() const __NOTHROW__{
			return const_cast< IAssetUtilities_v1 * >(this)->GetIAssetUtilities_I();
		}

		XMP_PRIVATE static uint64 GetInterfaceID() { return kIAssetUtilitiesID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

		virtual ~IAssetUtilities_v1(){}
	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		// all safe functions
		virtual uint32 APICALL isTrackable(const pcIMetadata_base metadata, pcIError_base & error) __NOTHROW__ = 0;
		virtual uint32 APICALL makeTrackable(const pIMetadata_base metadata, pcIError_base & error) __NOTHROW__ = 0;
		//! \endcond

	protected:
		//!
		//! protected Virtual Destructor
		//!
		//virtual ~IAssetUtilities_v1(){}

	#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
	#endif
			REQ_FRIEND_CLASS_DECLARATION();

	};

/*#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	typedef BASE_CLASS( IAssetUtilities, CLIENT_IASSETUTILITIES_VERSION )		IAssetUtilitiesBase;

	class IAssetUtilities : public virtual IAssetUtilitiesBase {
	//! \cond XMP_INTERNAL_DOCUMENTATION
	public:
		typedef IAssetUtilitiesBase *							pIAssetUtilitiesBase;
		typedef const IAssetUtilitiesBase *						pcIAssetUtilitiesBase;

		IAssetUtilities( pIAssetUtilitiesBase ptr ) : _ptr( ptr ) {}
		IAssetUtilities( pcIAssetUtilitiesBase cptr ) : _ptr( const_cast< pIAssetUtilitiesBase >( cptr ) ) {}
		virtual bool APICALL IsTrackable( const spcIMetadata & metadata );
		virtual bool APICALL MakeTrackable(const spIMetadata & metadata);
		virtual ~IAssetUtilities() __NOTHROW__ {}

	protected:
		virtual uint32 APICALL IsTrackable(const pcIMetadata_base metadata, pcIError_base & error) __NOTHROW__;
		virtual uint32 APICALL MakeTrackable(const pIMetadata_base metadata, pcIError_base & error) __NOTHROW__;

	private:
		IAssetUtilitiesBase *		_ptr;
	//! \endcond
	};

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	*/
}

#endif  // IAssetUtilities_h__
