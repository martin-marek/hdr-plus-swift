#ifndef IAssetManager_h__
#define IAssetManager_h__ 1

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
// 11-27-14 ADC 1.0-a016 Submitting changes related to iOS build.
// 11-24-14 ADC 1.0-a008 Support for ICompositionRelationship interface.
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

	class XMP_PUBLIC IAssetManager_v1
		: public virtual ISharedObject
		, public virtual IThreadSafe
		, public virtual IVersionable
	{
	public:
		// all unsafe functions

		// Factories to create the specific segments
		// all static functions
		spIAssetManager CreateAssetManager( const spIMetadata & metadata );
		/*virtual pIAssetManager APICALL GetActualIAssetManager() __NOTHROW__ = 0;
		XMP_PRIVATE pcIAssetManager GetActualIAssetManager() const __NOTHROW__{
			return const_cast<IAssetManager_v1 *>(this)->GetActualIAssetManager();
		}

		virtual AdobeXMPAM_Int::pIAssetManager_I APICALL GetIAssetManager_I() __NOTHROW__ = 0;
		XMP_PRIVATE AdobeXMPAM_Int::pcIAssetManager_I GetIAssetManager_I() const __NOTHROW__{
			return const_cast< IAssetManager_v1 * >(this)->GetIAssetManager_I();
		}*/

		XMP_PRIVATE static spIAssetManager MakeShared(pIAssetManager_base ptr);
		XMP_PRIVATE static spcIAssetManager MakeShared(pcIAssetManager_base ptr) {
			return MakeShared(const_cast<pIAssetManager_base>(ptr));
		}

		XMP_PRIVATE static uint64 GetInterfaceID() { return kIAssetManagerID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }
	
	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		// all safe functions
		//! \endcond

	protected:
		//!
		//! protected Virtual Destructor
		//!
		virtual ~IAssetManager_v1() __NOTHROW__ = 0;
	#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
	#endif
			REQ_FRIEND_CLASS_DECLARATION();

	};

	inline IAssetManager_v1::~IAssetManager_v1() __NOTHROW__ { }

/*#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	//typedef BASE_CLASS( IAssetManager, CLIENT_IASSETMANAGER_VERSION ) IAssetManagerBase;

	class IAssetManagerProxy : public virtual IAssetManager {
	//! \cond XMP_INTERNAL_DOCUMENTATION
	public:
		typedef IAssetManagerBase *											pIAssetManagerBase;
		typedef const IAssetManagerBase *									pcIAssetManagerBase;
		typedef shared_ptr< IAssetManagerBase >								spIAssetManagerBase;
		typedef shared_ptr< const IAssetManagerBase >						spcIAssetManagerBase;

		public:
		IAssetManager( const spIAssetManagerBase & sp ) : _sp( sp ) {}
		IAssetManager( const spcIAssetManagerBase & csp ) : _sp( const_pointer_cast< IAssetManagerBase > ( csp ) ) {}
		
		//virtual void AcquireSharedOwnership() const __NOTHROW__ {};
		//virtual void ReleaseSharedOwnership() const __NOTHROW__ {};

		//REMOVE: declare all virtual functions

		virtual ~IAssetManager() __NOTHROW__ {}

		pcIAssetManagerBase GetRawPointer() const { return _sp.get(); }
		pIAssetManagerBase GetRawPointer() { return _sp.get(); }

	private:
		spIAssetManagerBase					_sp;
	//! \endcond
	};

#endif */ // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	class IAssetManagerProxy : public virtual IAssetManager{
		//! \cond XMP_INTERNAL_DOCUMENTATION
	public:
		/*typedef IAssetManagerBase *											pIAssetManagerBase;
		typedef const IAssetManagerBase *										pcIAssetManagerBase;
		typedef shared_ptr< IAssetManagerBase >								spIAssetManagerBase;
		typedef shared_ptr< const IAssetManagerBase >							spcIAssetManagerBase;*/



		IAssetManagerProxy(pIAssetManager ptr);
		virtual ~IAssetManagerProxy();

		pIAssetManager APICALL GetActualIAssetManager() __NOTHROW__;
		AdobeXMPAM_Int::pIAssetManager_I APICALL GetIAssetManager_I() __NOTHROW__;

		void APICALL Acquire() const __NOTHROW__;
		void APICALL Release() const __NOTHROW__;
		AdobeXMPCommon_Int::pISharedObject_I APICALL GetISharedObject_I() __NOTHROW__;
		virtual void APICALL EnableThreadSafety() const __NOTHROW__;
		virtual void APICALL DisableThreadSafety() const __NOTHROW__;
		virtual bool APICALL IsThreadSafe() const;
		virtual AdobeXMPCommon_Int::pIThreadSafe_I APICALL GetIThreadSafe_I() __NOTHROW__;

		pvoid APICALL GetInterfacePointer(uint64 interfaceID, uint32 interfaceVersion);

	protected:
		virtual uint32 APICALL isThreadSafe() const __NOTHROW__;
		pvoid APICALL getInterfacePointer(uint64 interfaceID, uint32 interfaceVersion, pcIError_base & error) __NOTHROW__;


		//	pcIAssetManagerBase GetRawPointer() const { return _sp.get(); }
		//	pIAssetManagerBase GetRawPointer() { return _sp.get(); }



	protected:
		//virtual void AcquireSharedOwnership() const __NOTHROW__ {};
		//virtual void ReleaseSharedOwnership() const __NOTHROW__ {};

	private:
		pIAssetManager								mRawPtr;
		//! \endcond
	};

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

}

#endif  // IAssetManager_h__
