#ifndef IAssetTimePart_h__
#define IAssetTimePart_h__ 1

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
// 11-21-14 ADC 1.0-a002 Support for IAssetPart and IAssetTimePart interfaces.
// 11-19-14 ADC 1.0-a001 Framework in place for Asset Management Library.
//
// =================================================================================================
#endif  // AdobePrivate

#include "XMPAssetManagement/XMPAssetManagementFwdDeclarations.h"
#include "XMPAssetManagement/Interfaces/IAssetPart.h"
#include <float.h>

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;

	//!
	//! \brief Version1 of the interface that represents a timed slice/part of an asset.
	//! \details This time slice can be
	//!		-# some time slice of an asset (or whole) that can be used in another composite asset, or
	//!		-# some time slice of a composite asset (or whole) that is composed from other assets.
	//! It provides all the functions to 
	//!		-# set/get various properties of a time slice.
	//!		-# factory function to create a time slice part.
	//! \attention Support multi threading through locks but can be disabled by the client.
	//!
	class XMP_PUBLIC IAssetTimePart_v1
		: public virtual IAssetPart_v1
	{
	public:

		//!
		//! \brief Set the duration of time slice in terms of Frame Counts.
		//! \param[in] startFrameCount a value of \#NS_XMPCOMMON::uint64 containing the frame count of first frame included in
		//! the time slice.
		//! \param[in] countOfFrames a value of \#NS_XMPCOMMON::uint64 containing the count of frames in the time slice.
		//!
		virtual void APICALL SetFrameCountDuration( uint64 startFrameCount = 0, uint64 countOfFrames = Max_XMP_Uns64 ) __NOTHROW__ = 0;

		//!
		//! \brief Set the duration of time slice in terms of seconds.
		//! \param[in] startTimeInSeconds a value of double containing the starting time of the first frame included in the
		//! time slice.
		//! \param[in] durationInSeconds a value of double containing the duration of the frames in the time slice.
		//! \return a bool value indicating whether the operation was successful or not.
		//! \attention returns false in case startTimeInSeconds or durationInSeconds is negative.
		//!
		virtual bool APICALL SetTimeDuration(double startTimeInSeconds = 0, double durationInSeconds = DBL_MAX) __NOTHROW__;

		//!
		//! \brief Get the duration of time slice in terms of seconds.
		//! \param[out] startTimeInSeconds a value of double filled with the starting time of the first frame included in the
		//! time slice.
		//! \param[in] durationInSeconds a value of double filled with the duration of the frames in the time slice.
		//!
		virtual void APICALL GetTimeDuration(double & startTimeInSeconds, double & durationInSeconds) const __NOTHROW__;

		//!
		//! \brief Get the duration of the time slice in terms of Frame Counts.
		//! \param[in] startFrameCount a value of \#NS_XMPCOMMON::uint64 filled with the frame count of first frame included in
		//! the time slice.
		//! \param[in] countOfFrames a value of \#NS_XMPCOMMON::uint64 filled with the count of frames in the time slice.
		//!
		virtual void APICALL GetFrameCountDuration(uint64 & startFrameCount, uint64 & countOfFrames) const __NOTHROW__ = 0;

		//!
		//! \brief Get the frame rate associated with the start frame count.
		//! \param[out] numerator a value of \#NS_XMPCOMMON::uint64 filled with the total number of frames in number of seconds
		//! denoted by denominator.
		//! \param[out] denominator a value of \#NS_XMPCOMMON::uint64 filled with the number of seconds it takes for number of
		//! frames denoted by numerator to be played.
		//!
		virtual void APICALL GetFrameRateForStartFrameCount(uint64 & numerator, uint64 & denominator) const __NOTHROW__ = 0;

		//!
		//! \brief Get the frame rate associated with the count of Frames.
		//! \param[out] numerator a value of \#NS_XMPCOMMON::uint64 filled with the total number of frames in number of seconds
		//! denoted by denominator.
		//! \param[out] denominator a value of \#NS_XMPCOMMON::uint64 filled with the number of seconds it takes for number of
		//! frames denoted by numerator to be played.
		//!
		virtual void APICALL GetFrameRateForCountOfFrames(uint64 & numerator, uint64 & denominator) const __NOTHROW__ = 0;

		//!
		//! \brief Set the frame rate associated with the startFrameCount.
		//! \param[in] numerator a value of \#NS_XMPCOMMON::uint64 containing the total number of frames in number of seconds
		//! denoted by denominator.
		//! \param[in] denominator a value of \#NS_XMPCOMMON::uint64 containing the number of seconds it takes for number of
		//! frames denoted by numerator to be played.
		//! \attention throws error in case numerator, denominator or both are zero.
		//!
		virtual void APICALL SetFrameRateForStartFrameCount(uint64 numerator, uint64 denominator = 1) = 0;

		//!
		//! \brief Set the same frame rate associated with the count of frames.
		//! \param[in] numerator a value of \#NS_XMPCOMMON::uint64 containing the total number of frames in number of seconds
		//! denoted by denominator.
		//! \param[in] denominator a value of \#NS_XMPCOMMON::uint64 containing the number of seconds it takes for number of
		//! frames denoted by numerator to be played.
		//! \attention throws error in case numerator, denominator or both are zero.
		//!
		virtual void APICALL SetFrameRateForCountOfFrames(uint64 numerator, uint64 denominator = 1) = 0;

		//!
		//! \brief Set the same frame rate associated with the start frame count as well as count of frames.
		//! \param[in] numerator a value of \#NS_XMPCOMMON::uint64 containing the total number of frames in number of seconds
		//! denoted by denominator.
		//! \param[in] denominator a value of \#NS_XMPCOMMON::uint64 containing the number of seconds it takes for number of
		//! frames denoted by numerator to be played.
		//! \attention throws error in case numerator, denominator or both are zero.
		//!
		virtual void APICALL SetBothFrameRates(uint64 numerator, uint64 denominator = 1) = 0;

        //!
		//! \brief Virtual copy constructor
		//! Clones the object creating an exact replica of the asset's part.
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetPart object.
		//!
		virtual spIAssetPart APICALL Clone() const = 0;
        
		//!
		//! \brief Creates a standard component of a Time based Asset Part
		//! \param[in] component a value of type \#eAssetPartComponent indicating the component of the
		//! asset this object is referring to.
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetTimePart object.
		//! \attention  In case the component value is one which is not supported by the library then an error is thrown.
		//!
		XMP_PRIVATE static spIAssetTimePart CreateStandardTimePart( eAssetPartComponent component );

		//!
		//! \brief Creates a user defined component of a Time based Asset Part
		//! \param[in] component pointer to a const NULL terminated char buffer containing component string of
		//! the asset this string is referring to.
		//! \param[in] componentLength Length of char buffer containing component string
		//! \return a shared pointer to a \#AdobeXMPAM::IAssetTimePart object.
		//! \attention In case the component is a NULL pointer then an error is thrown.
		//!
		XMP_PRIVATE static spIAssetTimePart CreateUserDefinedTimePart( const char * component, sizet componentLength );
		
		virtual pIAssetTimePart APICALL GetActualIAssetTimePart() __NOTHROW__ = 0;
		XMP_PRIVATE pcIAssetTimePart GetActualIAssetTimePart() const __NOTHROW__{
			return const_cast<IAssetTimePart_v1 *>(this)->GetActualIAssetTimePart();
		}

		virtual AdobeXMPAM_Int::pIAssetTimePart_I APICALL GetIAssetTimePart_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcIAssetTimePart_I GetIAssetTimePart_I() const __NOTHROW__{
			return const_cast< IAssetTimePart_v1 * >(this)->GetIAssetTimePart_I();
		}

		XMP_PRIVATE static spIAssetTimePart MakeShared(pIAssetPart_base ptr);
		XMP_PRIVATE static spcIAssetTimePart MakeShared(pcIAssetPart_base ptr) {
			return MakeShared(const_cast<pIAssetPart_base>(ptr));
		}

		XMP_PRIVATE static uint64 GetInterfaceID() { return kIAssetTimePartID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		// all safe functions
		virtual void APICALL setFrameRate(uint32 typeOfFrameRate, uint64 numerator, uint64 denominator, pcIError_base & error) __NOTHROW__ = 0;
		virtual pIAssetPart_base APICALL clone(pcIError_base & error) const __NOTHROW__ = 0;
		//! \endcond

		//!
		//! protected Virtual Destructor
		//!
		virtual ~IAssetTimePart_v1(){}
	#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
	#endif
			REQ_FRIEND_CLASS_DECLARATION();
	
	};

	

/*#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	typedef BASE_CLASS( IAssetTimePart, CLIENT_IASSETTIMEPART_VERSION )		IAssetTimePartBase;

	#if XMP_WinBuild
		#pragma warning( push )
		#pragma warning( disable : 4250 )
	#endif

	class IAssetTimePart : public virtual IAssetTimePartBase, public virtual IAssetPart
	{
	//! \cond XMP_INTERNAL_DOCUMENTATION
	public:
		typedef IAssetTimePartBase *											pIAssetTimePartBase;
		typedef const IAssetTimePartBase *										pcIAssetTimePartBase;
		typedef shared_ptr< IAssetTimePartBase >								spIAssetTimePartBase;
		typedef shared_ptr< const IAssetTimePartBase >							spcIAssetTimePartBase;

		IAssetTimePart( const spIAssetTimePartBase & sp )
			: IAssetPart( static_pointer_cast< IAssetPartBase >( sp ) )
			, _sp( sp ) {}

		IAssetTimePart( const spcIAssetTimePartBase & csp )
			: IAssetPart( static_pointer_cast< const IAssetPartBase >( csp ) )
			, _sp( const_pointer_cast< IAssetTimePartBase >( csp ) ) {}

		virtual void APICALL SetFrameCountDuration( uint64 startFrameCount = 0, uint64 countOfFrames = Max_XMP_Uns64 ) __NOTHROW__;
		virtual bool APICALL SetTimeDuration( double startTimeInSeconds = 0, double durationInSeconds = DBL_MAX ) __NOTHROW__;
		virtual void APICALL GetTimeDuration( double & startTimeInSeconds, double & durationInSeconds ) const __NOTHROW__;
		virtual void APICALL GetFrameCountDuration( uint64 & startFrameCount, uint64 & countOfFrames ) const __NOTHROW__;
		virtual void APICALL GetFrameRateForStartFrameCount( uint64 & numerator, uint64 & denominator ) const __NOTHROW__;
		virtual void APICALL GetFrameRateForCountOfFrames( uint64 & numerator, uint64 & denominator ) const __NOTHROW__;
		virtual pIAssetPart_base APICALL Clone( pcIError_base & error ) const __NOTHROW__;

		virtual void APICALL SetFrameRateForStartFrameCount(uint64 numerator, uint64 denominator = 1);
		virtual void APICALL SetFrameRateForCountOfFrames(uint64 numerator, uint64 denominator = 1);
		virtual void APICALL SetBothFrameRates(uint64 numerator, uint64 denominator = 1);
		virtual void APICALL SetFrameRate(uint32 typeOfFrameRate, uint64 numerator, uint64 denominator, pcIError_base & error) __NOTHROW__;
		virtual spIAssetPart Clone() const;

		virtual ~IAssetTimePart() __NOTHROW__ {}

		pcIAssetTimePartBase GetRawPointer() const { return _sp.get(); }
		pIAssetTimePartBase GetRawPointer() { return _sp.get(); }

	protected:
		//virtual void AcquireSharedObject() const __NOTHROW__{};
		//virtual void ReleaseSharedObject() const __NOTHROW__{};

	private:
		spIAssetTimePartBase					_sp;
	//! \endcond
	};

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	*/
}
#endif  // IAssetTimePart_h__
