#ifndef IAssetPagePart_h__
#define IAssetPagePart_h__ 1

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
//	ANS Ankita Sharma
//
// mm-dd-yy who Description of changes, most recent first.
//
// 12-07-14 ADC 1.0-a041 Refactored Part Classes and Adding support for IAssetArtboardPart.
// 12-01-14 ADC 1.0-a026 Fixing inheritance hierarchy issue in public exposed proxy classes
//						 for specialized part interfaces.
// 12-01-14 ADC 1.0-a023 Fixed the issue with public source not compiling on iOS.
// 11-27-14 ADC 1.0-a016 Submitting changes related to iOS build.
// 11-24-14 ANS 1.0-a007 Fixing build errors
// 11-23-14 SKP 1.0-a006 Support for IAssetPagePart interface.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"
#include "XMPAssetManagement/Interfaces/IAssetPart.h"

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;

	//!
	//! \brief Version1 of the interface that represents a page of an asset as a part.
	//! \details This part can be
	//!		-# a page of an asset that can be used in another composite asset, or
	//!		-# a page of a composite asset that is composed from other other assets.
	//! It provides all the functions to 
	//!		-# set/get various properties of a page part.
	//!		-# factory function to create a page part.
	//! \attention Support multi threading through locks but can be disabled by the client.
	//!
	class XMP_PUBLIC IAssetPagePart_v1
		: public virtual IAssetPart_v1
	{
	public:
		
		//!
		//! \brief Get the physical page number of the asset.
		//! \return the number of page.
		//!
		virtual sizet APICALL GetNumber() const __NOTHROW__ = 0;

		//!
		//! \brief Set the descriptive page number of the page.
		//! \param[in] number a pointer to a const NULL terminated char buffer containing descriptive
		//! number of the page.
		//! \param[in] length Length of char buffer containing descriptive
		//! number of the page.
		//! \note providing NULL means client wants to remove the descriptive number altogether from
		//! the page part.
		//!
		virtual void APICALL SetDescriptivePageNumber(const char * number, sizet length) __NOTHROW__ = 0;

		//!
		//! \brief Get the descriptive page number of the page.
		//! \return a shared pointer of a const \#NS_XMPCOMMON::IUTF8String object containing the
		//! descriptive number of the page.
		//! \note an invalid shared pointer is returned if no descriptive number is set or it is
		//! explicitly removed.
		//!
		virtual spcIUTF8String APICALL GetDescriptivePageNumber() const = 0;

        //!
		//! \brief Virtual copy constructor
		//! Clones the object creating an exact replica of the asset's part.
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetPart object.
		//!
		virtual spIAssetPart APICALL Clone() const = 0;
        
		// Factories to create the specific segments

		//!
		//! \brief Creates a Page based Asset Part.
		//! \param[in] component a value of type \#eAssetPartComponent indicating the component of the
		//! asset this object is referring to.
		//! \param[in] pageNumber containing page number
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetPagePart object.
		//! \attention Error is thrown in case
		//!		-# the component value is one which is not supported by the library.
		//!		-# the pageNumber is less than or equal to zero.
		//! \note the number of the page is considered as unique and its client's responsibility to make sure the no it
		//! provides while creating a page part is unique.
		//!
		XMP_PRIVATE static spIAssetPagePart CreateStandardPagePart( eAssetPartComponent component,
			sizet pageNumber);

		//!
		//! \brief Creates a Page based Asset Part.
		//! \param[ in ] component pointer to a const NULL terminated char buffer containing component string of
		//! the asset this string is referring to.
		//! \param[in] pageNumber containing page number
		//! \param[in] length Length of char buffer containing component string
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetPagePart object.
		//! \attention Error is thrown in case
		//!		-# component is a NULL pointer.
		//!		-# the pageNumber is less than or equal to zero.
		//! \note the number of the page is considered as unique and its client's responsibility to make sure the no it
		//! provides while creating a page part is unique.
		//!
		XMP_PRIVATE static spIAssetPagePart CreateUserDefinedPagePart( const char * component, sizet length,
			sizet pageNumber);

		virtual pIAssetPagePart APICALL GetActualIAssetPagePart() __NOTHROW__ = 0;
		XMP_PRIVATE pcIAssetPagePart GetActualIAssetPagePart() const __NOTHROW__{
			return const_cast<IAssetPagePart_v1 *>(this)->GetActualIAssetPagePart();
		}

		virtual AdobeXMPAM_Int::pIAssetPagePart_I APICALL GetIAssetPagePart_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcIAssetPagePart_I GetIAssetPagePart_I() const __NOTHROW__{
				return const_cast< IAssetPagePart_v1 * >(this)->GetIAssetPagePart_I();
		}

		XMP_PRIVATE static spIAssetPagePart MakeShared(pIAssetPart_base ptr);
		XMP_PRIVATE static spcIAssetPagePart MakeShared(pcIAssetPart_base ptr) {
			return MakeShared(const_cast<pIAssetPart_base>(ptr));
		}
		XMP_PRIVATE static uint64 GetInterfaceID() { return kIAssetPagePartID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
		
	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		// all safe functions
		virtual pcIUTF8String_base APICALL getDescriptivePageNumber(pcIError_base & error) const __NOTHROW__ = 0;
        virtual pIAssetPart_base APICALL clone( pcIError_base & error ) const __NOTHROW__ = 0;
		//! \endcond

	protected:
		//!
		//! protected Virtual Destructor
		//!
		virtual ~IAssetPagePart_v1() { };

	#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
	#endif
			REQ_FRIEND_CLASS_DECLARATION();

	};

	//inline IAssetPagePart_v1::~IAssetPagePart_v1() __NOTHROW__ { }

/*#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	typedef BASE_CLASS( IAssetPagePart, CLIENT_IASSETPAGEPART_VERSION ) IAssetPagePartBase;

	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4250 )
	#endif

	class IAssetPagePart : public virtual IAssetPagePartBase, public virtual IAssetPart {
	//! \cond XMP_INTERNAL_DOCUMENTATION
	public:
		typedef IAssetPagePartBase *											pIAssetPagePartBase;
		typedef const IAssetPagePartBase *										pcIAssetPagePartBase;
		typedef shared_ptr< IAssetPagePartBase >								spIAssetPagePartBase;
		typedef shared_ptr< const IAssetPagePartBase >							spcIAssetPagePartBase;

		IAssetPagePart( const spIAssetPagePartBase & sp )
			: IAssetPart( static_pointer_cast< IAssetPartBase >( sp ) )
			, _sp( sp ) {}

		IAssetPagePart( const spcIAssetPagePartBase & csp )
			: IAssetPart( static_pointer_cast< const IAssetPartBase >( csp ) )
			, _sp( const_pointer_cast< IAssetPagePartBase > ( csp ) ) {}

		virtual sizet APICALL GetNumber() const __NOTHROW__;
		virtual void APICALL SetDescriptivePageNumber( const char * number, sizet length ) __NOTHROW__;
		virtual spIAssetPart APICALL Clone() const;

		virtual spcIUTF8String APICALL GetDescriptivePageNumber() const;
		virtual pcIUTF8String_base APICALL GetDescriptivePageNumber( pcIError_base & error ) const __NOTHROW__;
		
		virtual pIAssetPart_base APICALL Clone( pcIError_base & error ) const __NOTHROW__;

		virtual ~IAssetPagePart() __NOTHROW__ {}

		pcIAssetPagePartBase GetRawPointer() const { return _sp.get(); }
		pIAssetPagePartBase GetRawPointer() { return _sp.get(); }

	protected:
		//virtual void AcquireSharedObject() const __NOTHROW__ {};
		//virtual void ReleaseSharedObject() const __NOTHROW__ {};

	private:
		spIAssetPagePartBase					_sp;
	//! \endcond
	};

	#if XMP_WinBuild
		#pragma warning( pop )
	#endif

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	*/
}

#endif  // IAssetPagePart_h__
