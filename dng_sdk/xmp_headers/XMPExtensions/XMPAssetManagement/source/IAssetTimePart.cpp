// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2014 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

namespace AdobeXMPAM {
	class IAssetTimePartProxy;
}

#define FRIEND_CLASS_DECLARATION() friend class AdobeXMPAM::IAssetTimePartProxy;

#include "XMPAssetManagement/Interfaces/IAssetTimePart.h"

#if  GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include "XMPCommon/Utilities/TWrapperFunctions.h"
#include "XMPCommon/Interfaces/IError.h"
#include "XMPCommon/Interfaces/IUTF8String.h"
#include "XMPExtensions/include/IExtensionsObjectFactory.h"

namespace AdobeXMPAM {

#if XMP_WinBuild
#pragma warning( push )
#pragma warning( disable : 4250 )
#endif

	class IAssetTimePartProxy 
		: public virtual IAssetTimePart
		, public virtual IAssetPartProxy 
	{
	private:
		pIAssetTimePart mRawPtr;
	public:
		IAssetTimePartProxy( pIAssetTimePart ptr )
			: mRawPtr( ptr )
			, IAssetPartProxy( ptr ) {}

		~IAssetTimePartProxy() {}

		pIAssetTimePart APICALL GetActualIAssetTimePart() __NOTHROW__ { return mRawPtr; }

		AdobeXMPAM_Int::pIAssetTimePart_I APICALL GetIAssetTimePart_I() __NOTHROW__ {
			return mRawPtr->GetIAssetTimePart_I();
		}

		void APICALL SetFrameCountDuration( uint64 startFrameCount /*= 0*/, uint64 countOfFrames /*= Max_XMP_Uns64 */ ) __NOTHROW__ {
			mRawPtr->SetFrameCountDuration( startFrameCount, countOfFrames );
		}

		bool APICALL SetTimeDuration(double startTimeInSeconds /*= 0*/, double durationInSeconds /*= DBL_MAX */) __NOTHROW__{
			return mRawPtr->SetTimeDuration( startTimeInSeconds, durationInSeconds );
		}


		void APICALL GetTimeDuration(double & startTimeInSeconds, double & durationInSeconds) const __NOTHROW__{
			mRawPtr->GetTimeDuration( startTimeInSeconds, durationInSeconds );
		}

		void APICALL GetFrameCountDuration(uint64 & startFrameCount, uint64 & countOfFrames) const __NOTHROW__{
			mRawPtr->GetFrameCountDuration( startFrameCount, countOfFrames );
		}

		void APICALL GetFrameRateForStartFrameCount(uint64 & numerator, uint64 & denominator) const __NOTHROW__{
			mRawPtr->GetFrameRateForStartFrameCount( numerator, denominator );
		}

		void APICALL GetFrameRateForCountOfFrames(uint64 & numerator, uint64 & denominator) const __NOTHROW__{
			mRawPtr->GetFrameRateForCountOfFrames( numerator, denominator );
		}

		void APICALL SetFrameRateForStartFrameCount(uint64 numerator, uint64 denominator /*= 1 */) {
			CallSafeFunctionReturningVoid< IAssetTimePart, uint32, uint64, uint64 >(
				mRawPtr, &IAssetTimePart::setFrameRate, 1, numerator, denominator );
		}

		void APICALL SetFrameRateForCountOfFrames(uint64 numerator, uint64 denominator /*= 1 */) {
			CallSafeFunctionReturningVoid< IAssetTimePart, uint32, uint64, uint64 >(
				mRawPtr, &IAssetTimePart::setFrameRate, 2, numerator, denominator );
		}

		void APICALL SetBothFrameRates(uint64 numerator, uint64 denominator /*= 1 */) {
			CallSafeFunctionReturningVoid< IAssetTimePart, uint32, uint64, uint64 >(
				mRawPtr, &IAssetTimePart::setFrameRate, 3, numerator, denominator );
		}

		void APICALL setFrameRate(uint32 typeOfFrameRate, uint64 numerator, uint64 denominator, pcIError_base & error) __NOTHROW__{
			mRawPtr->setFrameRate( typeOfFrameRate, numerator, denominator, error );
		}

		spIAssetPart APICALL Clone() const {
			return CallConstSafeFunctionReturningPointer<IAssetTimePart, pIAssetPart_base, IAssetTimePart>(
				mRawPtr, &IAssetTimePart::clone);
		}

		pIAssetPart_base APICALL clone(pcIError_base & error) const __NOTHROW__{
			return mRawPtr->clone( error );
		}
	};

	spIAssetTimePart IAssetTimePart_v1::CreateStandardTimePart( eAssetPartComponent component ) {
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetTimePart_base, IAssetTimePart, uint32>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateStandardTimePart,
			static_cast< uint32 >( component ) );
	}

	spIAssetTimePart IAssetTimePart_v1::CreateUserDefinedTimePart( const char * component, sizet componentLength ) {
		return CallSafeFunctionReturningPointer<IExtensionsObjectFactory, pIAssetTimePart_base, IAssetTimePart, const char *, sizet>(
			IExtensionsObjectFactory::GetExtensionsObjectFactory(), &IExtensionsObjectFactory::CreateUserDefinedTimePart,
			component, componentLength );
	}

	spIAssetTimePart IAssetTimePart_v1::MakeShared(pIAssetPart_base ptr) {
		if ( !ptr ) return spIAssetTimePart();
		pIAssetPart p = IAssetPart::GetInterfaceVersion() > 1 ? ptr->GetInterfacePointer< IAssetPart >() : ptr;
		// need to downcast the pointer when raw pointer is kept
		pIAssetTimePart pt = dynamic_cast<pIAssetTimePart>(p);
		return shared_ptr< IAssetTimePart >(new IAssetTimePartProxy(pt));
	}
#if XMP_WinBuild
	#pragma warning( pop )
#endif
}

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#include <math.h>
namespace AdobeXMPAM {

	bool APICALL IAssetTimePart_v1::SetTimeDuration(double startTimeInSeconds /*= 0*/, double durationInSeconds /*= DBL_MAX */) __NOTHROW__{
		if ( startTimeInSeconds <= 0.0 || durationInSeconds <= 0.0 )
			return false;
		static const double kDoubleEpsilon = 0.00000000001;

		uint64 numerator = 1, denominator = 1;
		uint64 startFrameCount = 0;
		if ( fabs( startTimeInSeconds ) >= kDoubleEpsilon ) {
			GetFrameRateForStartFrameCount( numerator, denominator );
			double dStartFrameCount = startTimeInSeconds * numerator / denominator;
			startFrameCount = static_cast< uint64 >( floor( dStartFrameCount ) );
		}

		uint64 countOfFrames = Max_XMP_Uns64;
		if ( fabs( durationInSeconds - DBL_MAX ) >= kDoubleEpsilon ) {
			GetFrameRateForCountOfFrames( numerator, denominator );
			double dCountOfFrames = durationInSeconds * numerator / denominator;
			countOfFrames = static_cast< uint64 >( ceil( dCountOfFrames ) );
		}

		SetFrameCountDuration( startFrameCount, countOfFrames );
		return true;
	}

	void APICALL IAssetTimePart_v1::GetTimeDuration(double & startTimeInSeconds, double & durationInSeconds) const __NOTHROW__{
		uint64 numerator = 1, denominator = 1, startFrameNumber = 0, countOfFrames = 1;
		GetFrameCountDuration( startFrameNumber, countOfFrames );
		GetFrameRateForStartFrameCount( numerator, denominator );
		double dStartFrameNumber = static_cast< double >( startFrameNumber ), dCountOfFrames = static_cast< double >( countOfFrames );
		startTimeInSeconds = dStartFrameNumber * denominator / numerator;
		if ( countOfFrames == Max_XMP_Uns64 ) {
			durationInSeconds = DBL_MAX;
		} else {
			GetFrameRateForCountOfFrames( numerator, denominator );
			durationInSeconds = dCountOfFrames * denominator / numerator;
		}
	}

}
