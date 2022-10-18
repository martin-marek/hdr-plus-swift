#ifndef IAssetPart_h__
#define IAssetPart_h__ 1

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
//  SKP Sunil Kishor Pathak
//  ADC	Amandeep Chawla
//
// mm-dd-yy who Description of changes, most recent first.
//
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 11-27-14 ADC 1.0-a016 Submitting changes related to iOS build.
// 11-21-14 ADC 1.0-a004 Support for IAssetLayerPart interface.
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"
#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IThreadSafe.h"
#include <assert.h>

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;

	//!
	//! \brief Version1 of the interface that represents a generic part of an asset.
	//! \details This part can be
	//!		-# some part of an asset (or whole) that can be used in another composite asset, or
	//!		-# some part of a composite asset (or whole) that is composed from other other assets.
	//! It provides all the functions to 
	//!		-# set/get various properties of a part.
	//!		-# factory function to create a generic part.
	//! \attention Support multi threading through locks but can be disabled by the client.
	//!
	class XMP_PUBLIC IAssetPart_v1
		: public virtual ISharedObject
		, public virtual IVersionable
		, public virtual IThreadSafe
	{
	public:

		//!
		//! \brief Indicates various types of Asset parts supported in XMP Asset Management.
		//!
		typedef enum {
			//! Part is of generic type.
			kAssetPartTypeGeneric			= 0,
			//! Part indicates a time slice.
			kAssetPartTypeTime				= 1,
			//! Part indicates an art board
			kAssetPartTypeArtboard			= 2,
			//! Part indicates a layer
			kAssetPartTypeLayer				= 3,
			//! Part indicates a physical page
			kAssetPartTypePage				= 4,

			// add new part types here

			//! Maximum value this enum can hold, should be treated as invalid value
			kAssetPartTypeMaxValue			= AdobeXMPCommon::kMaxEnumValue
		} eAssetPartType;

		//!
		//! \brief Indicates various types of asset components available in XMP Asset Management.
		//!
		typedef enum {
			//! Any (specific part unknown).
			kAssetPartComponentNone				= 0,
			//! all (all parts of the content and metadata).
			kAssetPartComponentAll				= 1,
			//! Portions of the metadata.
			kAssetPartComponentMetadata			= 2,
			//! Any or all of the content (non-metadata).
			kAssetPartComponentContent			= 3,
			//! Any or all sound.
			kAssetPartComponentAudio			= 4,
			//! Some image data( video or still ).
			kAssetPartComponentVisual			= 5,
			//! Video or animation.
			kAssetPartComponentVideo			= 6,
			//! Static raster image.
			kAssetPartComponentRaster			= 7,
			//! Static vector image.
			kAssetPartComponentVector			= 8,
			//! Form field data.
			kAssetPartComponentFormData			= 9,
			//! Form template.
			kAssetPartComponentFormTemplate		= 10,
			//! Applied annotations (comments).
			kAssetPartComponentFormAnnotations	= 11,

			// Add New Part Components here

			//! Any User defined component description which is not part of specification.
			kAssetPartComponentUserDefined		= kMaxEnumValue - 1,

			//! Maximum value this enum can hold, should be treated as invalid value
			kAssetPartComponentMaxValue			= kMaxEnumValue
		} eAssetPartComponent;

		//!
		//! \brief Get the type of the part.
		//! \return an object of type #eAssetPartType indicating the type of the part.
		//!
		virtual eAssetPartType APICALL GetType() const = 0;

		//!
		//! \brief Get the component of the asset this part object is referring to.
		//! \return an object of type \#eAssetPartComponent.
		//! \note In case the component is user defined then the value returned is
		//! kAssetPartComponentUserDefined.
		//!
		virtual eAssetPartComponent APICALL GetComponent() const = 0;

		//!
		//! \brief Get the user defined component of the part.
		//! \return a shared pointer to const \#NS_XMPCOMMON::IUTF8String object containing component string
		//! of the part of an asset.
		//! \note In case component is amongst one of the standard components recognized by XMP then an
		//! invalid shared pointer will be returned.
		//!
		virtual spcIUTF8String APICALL GetUserDefinedComponent() const = 0;

		//!
		//! \brief Virtual copy constructor
		//! Clones the object creating an exact replica of the asset's part.
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetPart object.
		//!
		virtual spIAssetPart APICALL Clone() const = 0;

		//!
		//! \brief Creates a standard component of a Generic Asset Part
		//! \param[in] component a value of type \#eAssetPartComponent indicating the component of the
		//! asset this object is referring to.
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetPart object.
		//! \attention  In case the component value is one which is not supported by the library then an error is thrown.
		//!
		XMP_PRIVATE static spIAssetPart CreateStandardGenericPart( eAssetPartComponent component);

		//!
		//! Creates a user defined component of a Generic Asset Part
		//! \param[in] component pointer to a const NULL terminated char buffer containing component string of
		//! the asset this string is referring to.
		//! \param[in] length Length of char buffer containing component string
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetPart object.
		//! \attention In case the component is a NULL pointer then an error is thrown.
		//!
		XMP_PRIVATE static spIAssetPart CreateUserDefinedGenericPart( const char * component, sizet length);

		virtual pIAssetPart APICALL GetActualIAssetPart() __NOTHROW__ = 0;
		XMP_PRIVATE pcIAssetPart GetActualIAssetPart() const __NOTHROW__{
			return const_cast<IAssetPart_v1 *>(this)->GetActualIAssetPart();
		}

		virtual AdobeXMPAM_Int::pIAssetPart_I APICALL GetIAssetPart_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcIAssetPart_I GetIAssetPart_I() const __NOTHROW__{
			return const_cast< IAssetPart_v1 * >(this)->GetIAssetPart_I();
		}
		
		XMP_PRIVATE static spIAssetPart MakeShared(pIAssetPart_base ptr);
		XMP_PRIVATE static spcIAssetPart MakeShared(pcIAssetPart_base ptr) {
			return MakeShared(const_cast<pIAssetPart_base>(ptr));
		}

		XMP_PRIVATE static uint64 GetInterfaceID() { return kIAssetPartID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		// all safe functions
		virtual uint32 APICALL getType(pcIError_base & error) const __NOTHROW__ = 0;
		virtual uint32 APICALL getComponent(pcIError_base & error) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getUserDefinedComponent(pcIError_base & error) const __NOTHROW__ = 0;
		virtual pIAssetPart_base APICALL clone(pcIError_base & error ) const __NOTHROW__ = 0;
		//! \endcond

		//!
		//! protected Virtual Destructor
		//!
		virtual ~IAssetPart_v1() __NOTHROW__ = 0;

	#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
	#endif
			REQ_FRIEND_CLASS_DECLARATION();

	};

	inline IAssetPart_v1::~IAssetPart_v1() __NOTHROW__ { }

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	//typedef BASE_CLASS( IAssetPart, CLIENT_IASSETPART_VERSION )			IAssetPartBase;

	class IAssetPartProxy : public virtual IAssetPart{
	//! \cond XMP_INTERNAL_DOCUMENTATION
	public:
		/*typedef IAssetPartBase *											pIAssetPartBase;
		typedef const IAssetPartBase *										pcIAssetPartBase;
		typedef shared_ptr< IAssetPartBase >								spIAssetPartBase;
		typedef shared_ptr< const IAssetPartBase >							spcIAssetPartBase;*/

		

		IAssetPartProxy( pIAssetPart ptr );
		virtual ~IAssetPartProxy();

		pIAssetPart APICALL GetActualIAssetPart() __NOTHROW__;
		AdobeXMPAM_Int::pIAssetPart_I APICALL GetIAssetPart_I() __NOTHROW__;

		void APICALL Acquire() const __NOTHROW__;
		void APICALL Release() const __NOTHROW__;
		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__;
		virtual void APICALL EnableThreadSafety() const __NOTHROW__;
		virtual void APICALL DisableThreadSafety() const __NOTHROW__;
		virtual bool APICALL IsThreadSafe() const;
		virtual AdobeXMPCommon_Int::pIThreadSafe_I APICALL GetIThreadSafe_I() __NOTHROW__;

		pvoid APICALL GetInterfacePointer( uint64 interfaceID, uint32 interfaceVersion );
		
		virtual eAssetPartType APICALL GetType() const;
		virtual eAssetPartComponent APICALL GetComponent() const;
		virtual spcIUTF8String APICALL GetUserDefinedComponent() const;
		virtual spIAssetPart APICALL Clone() const;

	protected:
		virtual uint32 APICALL getType( pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL getComponent( pcIError_base & error ) const __NOTHROW__;
		virtual pcIUTF8String_base APICALL getUserDefinedComponent( pcIError_base & error ) const __NOTHROW__;
		virtual pIAssetPart_base APICALL clone(pcIError_base & error ) const __NOTHROW__;
		virtual uint32 APICALL isThreadSafe() const __NOTHROW__;
		pvoid APICALL getInterfacePointer( uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error ) __NOTHROW__;
		

	//	pcIAssetPartBase GetRawPointer() const { return _sp.get(); }
	//	pIAssetPartBase GetRawPointer() { return _sp.get(); }

		
		
	protected:
		//virtual void AcquireSharedObject() const __NOTHROW__ {};
		//virtual void ReleaseSharedObject() const __NOTHROW__ {};

	private:
		pIAssetPart								mRawPtr;
	//! \endcond
	};

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

}

#endif  // IAssetPart_h__
