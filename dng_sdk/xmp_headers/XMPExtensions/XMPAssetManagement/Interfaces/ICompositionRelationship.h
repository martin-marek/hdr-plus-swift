#ifndef ICompositionRelationship_h__
#define ICompositionRelationship_h__ 1

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

#include "XMPCore/XMPCoreFwdDeclarations.h"

#include "XMPCommon/Interfaces/BaseInterfaces/ISharedObject.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IVersionable.h"
#include "XMPCommon/Interfaces/BaseInterfaces/IThreadSafe.h"

namespace AdobeXMPAM {
	using namespace AdobeXMPCommon;
	using namespace AdobeXMPCore;

	//!
	//! \brief Version1 of the interface that represents a relationship between a composite
	//! document and its components.
	//! \details Provides all the functions to
	//!		-# get the required information of a relationship.
	//!		-# set various other options of a relationship.
	//! \attention Support multi threading through locks but can be disabled by the client.
	//!
	class XMP_PUBLIC ICompositionRelationship_v1
		: public virtual ISharedObject
		, public virtual IVersionable
		, public virtual IThreadSafe
	{
	public:

		//!
		//! \brief Indicates how the markers of the components ( and/or their components ) be used.
		//!
		typedef enum {
			//! Ignore markers in the component and all its components.
			kCompositionMaskMarkersAll			= 0,
			//! Process markers in the component and all its components.
			kCompositionMaskMarkersNone			= 1,
			//! Not specified by the client.
			kCompositionMaskMarkersNotSpecified	= 2,
			// Add more values here

			//! Maximum value this enum can hold, should be treated as invalid value.
			kCompositionMaskMardersMaxValue		= kMaxEnumValue
		} eCompositionMaskMarkers;

		//!
		//! \brief Get the metadata of the component asset.
		//! \return a shared pointer of const \#NS_XMPCORE::IStructureNode object containing the metadata of
		//! the component asset.
		//!
		virtual spcIStructureNode APICALL GetComponentMetadata() const = 0;

		//!
		//! \brief Get the details about the part used from the component asset.
		//! \return a shared pointer of const \#AdobeXMPAM::IAssetPart object containing all the details
		//! about the part used from the component asset.
		//!
		virtual spcIAssetPart APICALL GetComponentPart() const = 0;

		//!
		//! \brief Get the metadata of the composed asset.
		//! \return a shared pointer of const \#NS_XMPCORE::IStructureNode object containing the metadata of
		//! the composed asset.
		virtual spcIStructureNode APICALL GetComposedMetadata() const = 0;

		//!
		//! \brief Get the details about the part of the composed asset which uses the component part.
		//! \return a shared pointer of const \#AdobeXMPAM::IAssetPart object containing all the details
		//! about the part of the composed asset which uses the component part.
		virtual spcIAssetPart APICALL GetComposedPart() const = 0;

		//!
		//! \brief Get the name of a mapping function used to map the part from the component asset to the part
		//! in composed asset.
		//! \return a shared pointer of const \#NS_XMPCOMMON::IUTF8String containing the name of a mapping
		//! function used to map the part from the component asset to the part in composed asset.
		//! \note return an invalid shared pointer in case no name is provided by the client.
		//!
		virtual spcIUTF8String APICALL GetMappingFunctionName() const = 0;

		//!
		//! \brief Set\\Change\\Delete the name of a mapping function used to map the part from the component
		//! asset to the part in composed asset.
		//! \param[in] name a pointer to const NULL terminated char buffer consisting of a name of a
		//! mapping function used to map the part from the component asset to the part in composed asset.
		//! \param[in] nameLength Length of char buffer consisting of a name of a
		//! mapping function used to map the part from the component asset to the part in composed asset.
		//! \note setting the name to NULL means client wants to remove the name.
		//!
		virtual void APICALL SetMappingFunctionName(const char * name, sizet nameLength) = 0;

		//!
		//! \brief Gets whether markers in the component should be ignored( masked ) or processed normally.
		//! \return a value of type \#eCompositionMaskMarkers indicating whether markers in the component
		//! should be ignored or not.
		//! \note default value is kCompositionMaskMarkersNotSpecified if not set by the client.
		//!
		virtual eCompositionMaskMarkers APICALL GetMaskMarkersFlag() const = 0;

		//!
		//! \brief Sets whether markers in the component should be ignored( masked ) or processed normally.
		//! \param[in] flag a value of type \#eCompositionMaskMarkers indicating whether markers in
		//! the component should be ignored or not.
		//! \attention Error is thrown in case value of flag is not the one supported by the library.
		//!
		virtual void APICALL SetMaskMarkersFlag(eCompositionMaskMarkers flag) = 0;

		//!
		//! \brief Get the document ID of the component asset.
		//! \return a shared pointer of const NS_XMPCOMMON::IUTF8String object containing the document
		//! ID of the component asset.
		//!
		virtual spcIUTF8String APICALL GetComponentDocumentID() const = 0;

		//!
		//! \brief Get the instance ID of the component asset.
		//! \return a shared pointer of const NS_XMPCOMMON::IUTF8String object containing the instance
		//! ID of the component asset.
		//!
		virtual spcIUTF8String APICALL GetComponentInstanceID() const = 0;

		//!
		//! \brief Get the document ID of the composed asset.
		//! \return a shared pointer of const NS_XMPCOMMON::IUTF8String object containing the document
		//! ID of the composed asset.
		//!
		virtual spcIUTF8String APICALL GetComposedDocumentID() const = 0;

		//!
		//! \brief Get the instance ID of the composed asset.
		//! \return a shared pointer of const NS_XMPCOMMON::IUTF8String object containing the instance
		//! ID of the composed asset.
		//!
		virtual spcIUTF8String APICALL GetComposedInstanceID() const = 0;

		//!
		//! \brief Set/Change/Delete the file path of the component asset.
		//! \param[in] filePath a pointer to const NULL terminated char buffer containing the file path
		//! of the component asset.
		//! \param[in] filePathLength Length of char buffer containing the
		//! file path to the component asset.
		//! \note setting the filePath to NULL means client wants to remove the filePath.
		//!
		virtual void APICALL SetComponentFilePath(const char * filePath, sizet filePathLength) = 0;

		//!
		//! \brief Get the file path of the component asset.
		//! \return a shared pointer of const \#NS_XMPCOMMON::IUTF8String object containing the file path
		//! of the component asset.
		//! \note an invalid shared pointer is returned in case no information regarding file path of the
		//! component asset is available or has been explicitly removed by the client.
		//!
		virtual spcIUTF8String APICALL GetComponentFilePath() const = 0;

		//!
		//! \brief Appends an alternate path of the component asset to the list of alternate paths to be used
		//! for locating the asset.
		//! \param[in] alternatePath a pointer to const NULL terminated char buffer containing an
		//! alternate path to the component asset.
		//! \param[in] altPathLength Length of char buffer containing an
		//! alternate path to the component asset.
		//! \attention Error is thrown in case alternatePath is a NULL pointer or its contents are empty.
		//!
		virtual void APICALL AppendComponentAlternatePath(const char * alternatePath, sizet altPathLength) = 0;

		//!
		//! \brief Get the list of alternate paths to be used for locating the component asset.
		//! \return a vector of shared pointers of const \#NS_XMPCOMMON::IUTF8String objects containing
		//! the alternate path to be used for locating the component asset.
		//!
		virtual spcIUTF8Strings APICALL GetComponentAlternatePaths() const = 0;

		//!
		//! \brief Clears all the alternate paths to be used for locating the component asset.
		//!
		virtual void APICALL ClearComponentAlternatePaths() = 0;

		XMP_PRIVATE static spICompositionRelationship MakeShared(pICompositionRelationship_base ptr);
		XMP_PRIVATE static spcICompositionRelationship MakeShared(pcICompositionRelationship_base ptr) {
			return MakeShared(const_cast< pICompositionRelationship_base >(ptr));
		}

		virtual pICompositionRelationship APICALL GetActualICompositionRelationship() __NOTHROW__ = 0;
		XMP_PRIVATE pcICompositionRelationship GetActualICompositionRelationship() const __NOTHROW__{
			return const_cast< ICompositionRelationship_v1 * >(this)->GetActualICompositionRelationship();
		}

		virtual AdobeXMPAM_Int::pICompositionRelationship_I APICALL GetICompositionRelationship_I() __NOTHROW__ = 0;

		XMP_PRIVATE AdobeXMPAM_Int::pcICompositionRelationship_I GetICompositionRelationship_I() const __NOTHROW__{
			return const_cast< ICompositionRelationship_v1 * >(this)->GetICompositionRelationship_I();
		}

		XMP_PRIVATE static uint64 GetInterfaceID() { return kICompositionRelationshipID; }
		XMP_PRIVATE static uint32 GetInterfaceVersion() { return 1; }

	protected:
		//! \cond XMP_INTERNAL_DOCUMENTATION
		virtual pcIStructureNode_base APICALL getComponentMetadata(pcIError_base & error) const __NOTHROW__ = 0;
		virtual pcIAssetPart_base APICALL getComponentPart(pcIError_base & error) const __NOTHROW__ = 0;

		virtual pcIStructureNode_base APICALL getComposedMetadata(pcIError_base & error) const __NOTHROW__ = 0;
		virtual pcIAssetPart_base APICALL getComposedPart(pcIError_base & error) const __NOTHROW__ = 0;

		virtual pcIUTF8String_base APICALL getMappingFunctionName(pcIError_base & error) const __NOTHROW__ = 0;

		virtual uint32 APICALL getMaskMarkersFlag(pcIError_base & error) const __NOTHROW__ = 0;
		virtual void APICALL setMaskMarkersFlag(uint32 flag, pcIError_base & error) __NOTHROW__ = 0;

		virtual pcIUTF8String_base APICALL getComponentDocumentID(pcIError_base & error) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getComponentInstanceID(pcIError_base & error) const __NOTHROW__ = 0;

		virtual pcIUTF8String_base APICALL getComposedDocumentID(pcIError_base & error) const __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getComposedInstanceID(pcIError_base & error) const __NOTHROW__ = 0;

		virtual pcIUTF8String_base APICALL getComponentFilePath( pcIError_base & error ) const __NOTHROW__ = 0;

		virtual void APICALL appendComponentAlternatePath(const char * alternatePath, sizet altPathLength, pcIError_base & error) __NOTHROW__ = 0;
		virtual sizet  APICALL getComponentAlternatePathsSize() __NOTHROW__ = 0;
		virtual pcIUTF8String_base APICALL getComponentAlternatePathAtIndex(sizet index, pcIError_base & error) __NOTHROW__ = 0;
		//! \endcond

	protected:
		//!
		//! protected Virtual Destructor
		//!
		virtual ~ICompositionRelationship_v1() {};

	#ifdef FRIEND_CLASS_DECLARATION
			FRIEND_CLASS_DECLARATION();
	#endif
			REQ_FRIEND_CLASS_DECLARATION();
	
	};

	
/*#if GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE

	typedef BASE_CLASS( ICompositionRelationship, CLIENT_ICOMPOSITIONRELATIONSHIP_VERSION ) ICompositionRelationshipBase;

	class ICompositionRelationship : public virtual ICompositionRelationshipBase {
	//! \cond XMP_INTERNAL_DOCUMENTATION
	public:
		typedef ICompositionRelationshipBase *											pICompositionRelationshipBase;
		typedef const ICompositionRelationshipBase *									pcICompositionRelationshipBase;
		typedef shared_ptr< ICompositionRelationshipBase >								spICompositionRelationshipBase;
		typedef shared_ptr< const ICompositionRelationshipBase >						spcICompositionRelationshipBase;

		ICompositionRelationship( const spICompositionRelationshipBase & sp ) : _sp( sp ) {}
		ICompositionRelationship( const spcICompositionRelationshipBase & csp ) : _sp( const_pointer_cast< ICompositionRelationshipBase > ( csp ) ) {}
		
		virtual void AcquireSharedObject() const __NOTHROW__ {};
		virtual void ReleaseSharedObject() const __NOTHROW__ {};

		virtual ~ICompositionRelationship() __NOTHROW__ {}

		pcICompositionRelationshipBase GetRawPointer() const { return _sp.get(); }
		pICompositionRelationshipBase GetRawPointer() { return _sp.get(); }

		virtual spcIStructureNode GetComponentMetadata() const;
		virtual pcIStructureNode_base GetComponentMetadata( pcIError_base & error ) const __NOTHROW__;
		virtual spcIAssetPart GetComponentPart() const;
		virtual pcIAssetPart_base GetComponentPart( pcIError_base & error ) const __NOTHROW__;
		virtual spcIStructureNode GetComposedMetadata() const;
		virtual pcIStructureNode_base GetComposedMetadata( pcIError_base & error ) const __NOTHROW__;
		virtual spcIAssetPart GetComposedPart() const;
		virtual pcIAssetPart_base GetComposedPart( pcIError_base & error ) const __NOTHROW__;
		virtual spcIUTF8String GetMappingFunctionName() const;
		virtual pcIUTF8String_base GetMappingFunctionName( pcIError_base & error ) const __NOTHROW__;
		virtual void SetMappingFunctionName( const char * name );
		virtual eCompositionMaskMarkers GetMaskMarkersFlag() const;
		virtual uint32 GetMaskMarkersFlag( pcIError_base & error ) const __NOTHROW__;
		virtual void SetMaskMarkersFlag( eCompositionMaskMarkers flag );
		virtual void SetMaskMarkersFlag( uint32 flag, pcIError_base & error ) __NOTHROW__;
		virtual spcIUTF8String GetComponentDocumentID() const;
		virtual pcIUTF8String_base GetComponentDocumentID( pcIError_base & error ) const __NOTHROW__;
		virtual spcIUTF8String GetComposedDocumentID() const;
		virtual pcIUTF8String_base GetComposedDocumentID( pcIError_base & error ) const __NOTHROW__;
		virtual spcIUTF8String GetComponentInstanceID() const;
		virtual pcIUTF8String_base GetComponentInstanceID( pcIError_base & error ) const __NOTHROW__;
		virtual spcIUTF8String GetComposedInstanceID() const;
		virtual pcIUTF8String_base GetComposedInstanceID( pcIError_base & error ) const __NOTHROW__;
		virtual void SetComponentFilePath( const char * filePath );
		virtual spcIUTF8String GetComponentFilePath() const;
		virtual pcIUTF8String_base GetComponentFilePath( pcIError_base & error ) const __NOTHROW__;
		virtual void AppendComponentAlternatePath( const char * alternatePath );
		virtual void AppendComponentAlternatePath( const char * alternatePath, pcIError_base & error ) __NOTHROW__;
		virtual spcIUTF8StringList GetComponentAlternatePaths() const;
		virtual void ClearComponentAlternatePaths();
		virtual sizet GetComponentAlternatePathsSize() __NOTHROW__;
		virtual pcIUTF8String_base GetComponentAlternatePathAtIndex( sizet index, pcIError_base & error ) __NOTHROW__;

	private:
		spICompositionRelationshipBase					_sp;
	//! \endcond
	};

#endif  // GENERATE_XMPEXTENSIONS_CLIENT_LAYER_CODE
	*/
}

#endif  // ICompositionRelationship_h__
