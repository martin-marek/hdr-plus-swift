#ifndef IAssetLayerPart_h__
#define IAssetLayerPart_h__ 1

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
// 12-01-14 ADC 1.0-a026 Fixing inheritance hierarchy issue in public exposed proxy classes
//						 for specialized part interfaces.
// 12-01-14 ADC 1.0-a023 Fixed the issue with public source not compiling on iOS.
// 11-27-14 ADC 1.0-a016 Submitting changes related to iOS build.
// 11-21-14 ADC 1.0-a004 Support for IAssetLayerPart interface.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"
#include "XMPAssetManagement/Interfaces/IAssetPart.h"

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;

	//!
	//! \brief Version1 of the interface that represents a layer of an asset as a part.
	//! \details This part can be
	//!		-# a layer of an asset that can be used in another composite asset, or
	//!		-# a layer of a composite asset that is composed from other other assets.
	//! It provides all the functions to 
	//!		-# set/get various properties of a layer part.
	//!		-# factory function to create a layer part.
	//! \attention Support multi threading through locks but can be disabled by the client.
	//!
	class XMP_PUBLIC IAssetLayerPart_v1
		: public virtual IAssetPart_v1
	{
	public:

		//!
		//! \brief Get the id of the layer.
		//! \return a shared pointer of a const \#NS_XMPCOMMON::IUTF8String object containing id of the layer.
		//!
		virtual spcIUTF8String APICALL GetID() const = 0;

		//!
		//! \brief Set the name of the layer.
		//! \param[in] name a pointer to a const NULL terminated char buffer containing name of the layer.
		//! \param[in] length Length of  char buffer containing name of the layer..
		//! \note providing NULL means client wants to remove the name altogether from the layer part.
		//!
		virtual void APICALL SetName(const char * name, sizet length) = 0;

		//!
		//! \brief Get the name of the layer.
		//! \return a shared pointer of a const \#NS_XMPCOMMON::IUTF8String object containing the name of the layer.
		//! \note an invalid shared pointer is returned if no name is set or it is explicitly removed.
		//!
		virtual spcIUTF8String APICALL GetName() const = 0;

        //!
		//! \brief Virtual copy constructor
		//! Clones the object creating an exact replica of the asset's part.
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetPart object.
		//!
		virtual spIAssetPart APICALL Clone() const = 0;
        
		// Factories to create the specific segments

		//!
		//! \brief Creates a Layer based Asset Part.
		//! \param[in] component a value of type \#eAssetPartComponent indicating the component of the
		//! asset this object is referring to.
		//! \param[in] id pointer to a const NULL terminated char buffer containing containing id of the layer.
		//! \param[in] idLength Length of  char buffer containing containing id of the layer.
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetLayerPart object.
		//! \attention Error is thrown in case
		//!		-# the component value is one which is not supported by the library.
		//!		-# id is a NULL pointer or its contents are empty.
		//! \note the id of the layer is considered as unique and its client's responsibility to make sure the ids it
		//! provides while creating a layer part is unique.
		//!
		XMP_PRIVATE static spIAssetLayerPart CreateStandardLayerPart(eAssetPartComponent component, const char * id, sizet idLength);

		//!
		//! \brief Creates a Layer based Asset Part.
		//! \param[ in ] component pointer to a const NULL terminated char buffer containing component string of
		//! the asset this string is referring to.
		//! \param[in] id pointer to a const NULL terminated char buffer containing containing id of the layer.
		//! \param[in] idLength Length of  char buffer containing id of the layer.
		//! \param[in]  componentLength of  char buffer containing component string
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetLayerPart object.
		//! \attention Error is thrown in case
		//!		-# component is a NULL pointer.
		//!		-# id is a NULL pointer or its contents are empty.
		//! \note the id of the layer is considered as unique and its client's responsibility to make sure the ids it
		//! provides while creating a layer part is unique.
		//!
		XMP_PRIVATE static spIAssetLayerPart CreateUserDefinedLayerPart( const char * component, sizet componentLength, const char * id, sizet idLength );
		
		virtual pIAssetLayerPart APICALL GetActualIAssetLayerPart() __NOTHROW__ = 0;
		XMP_PRIVATE pcIAssetLayerPart GetActualIAssetLayerPart() const __NOTHROW__{
			return const_cast<IAssetLayerPart_v1 *>(this)->GetActualIAssetLayerPart();
		}

		virtual AdobeXMPAM_Int::pIAssetLayerPart_I APICALL GetIAssetLayerPart_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcIAssetLayerPart_I GetIAssetLayerPart_I() const __NOTHROW__{
				return const_cast< IAssetLayerPart_v1 * >(this)->GetIAssetLayerPart_I();
		}

		XMP_PRIVATE static spIAssetLayerPart MakeShared(pIAssetPart_base ptr);
		XMP_PRIVATE static spcIAssetLayerPart MakeShared(pcIAssetPart_base ptr) {
			return MakeShared(const_cast<pIAssetPart_base>(ptr));
		}

		XMP_PRIVATE static uint64 GetInterfaceID() { return kIAssetLayerPartID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		// all safe functions
		virtual pcIUTF8String_base APICALL getID(pcIError_base & error) const __NOTHROW__ = 0;
		virtual void APICALL setName(const char * name, sizet length, pcIError_base & error) __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getName(pcIError_base & error) const __NOTHROW__ = 0;
		virtual pIAssetPart_base APICALL clone(pcIError_base & error) const __NOTHROW__ = 0;
		//! \endcond

	protected:
		//!
		//! protected Virtual Destructor
		//!
		virtual ~IAssetLayerPart_v1() {}
	#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
	#endif
			REQ_FRIEND_CLASS_DECLARATION();
	

	};


/*#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	typedef BASE_CLASS( IAssetLayerPart, CLIENT_IASSETLAYERPART_VERSION ) IAssetLayerPartBase;

	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4250 )
	#endif

	class IAssetLayerPart : public virtual IAssetLayerPartBase, public virtual IAssetPart {
	//! \cond XMP_INTERNAL_DOCUMENTATION
	public:
		typedef IAssetLayerPartBase *											pIAssetLayerPartBase;
		typedef const IAssetLayerPartBase *										pcIAssetLayerPartBase;
		typedef shared_ptr< IAssetLayerPartBase >								spIAssetLayerPartBase;
		typedef shared_ptr< const IAssetLayerPartBase >							spcIAssetLayerPartBase;

		IAssetLayerPart( const spIAssetLayerPartBase & sp )
			: IAssetPart( static_pointer_cast< IAssetPartBase >( sp ) )
			, _sp( sp ) {}

		IAssetLayerPart( const spcIAssetLayerPartBase & csp )
			: IAssetPart( static_pointer_cast< const IAssetPartBase >( csp ) )
			, _sp( const_pointer_cast< IAssetLayerPartBase > ( csp ) ) {}

		virtual spcIUTF8String APICALL GetID() const;
		virtual void APICALL SetName( const char * name, sizet length );
		virtual spcIUTF8String APICALL GetName() const;
		virtual spIAssetPart APICALL Clone() const;

		virtual pcIUTF8String_base APICALL GetID( pcIError_base & error ) const __NOTHROW__;
		virtual void APICALL setName( const char * name, sizet length, pcIError_base & error ) __NOTHROW__;
		virtual pcIUTF8String_base APICALL GetName( pcIError_base & error ) const __NOTHROW__;
		virtual pIAssetPart_base APICALL Clone( pcIError_base & error ) const __NOTHROW__;

		virtual ~IAssetLayerPart() __NOTHROW__ {}

		pcIAssetLayerPartBase GetRawPointer() const { return _sp.get(); }
		pIAssetLayerPartBase GetRawPointer() { return _sp.get(); }

	protected:
		//virtual void AcquireSharedOwnership() const __NOTHROW__ {};
		//virtual void ReleaseSharedOwnership() const __NOTHROW__ {};

	private:
		spIAssetLayerPartBase					_sp;
	//! \endcond
	};

	#if XMP_WinBuild
		#pragma warning( pop )
	#endif

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
*/
}

#endif  // IAssetLayerPart_h__
