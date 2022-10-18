#if AdobePrivate	// The whole class is private for now.

#ifndef __TXMPDocOps_hpp__
#define __TXMPDocOps_hpp__ 1

#if ( ! __XMP_hpp__ )
    #error "Do not directly include, use XMP.hpp"
#endif

// =================================================================================================
// Copyright Adobe
// Copyright 2002-2008 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#if AdobePrivate
// =================================================================================================
// Change history
// ==============
//
// Writers:
// AWL Alan Lillich
//
// mm-dd-yy who Description of changes, most recent on top
//
// 07-22-09 AWL 5.0-c048 [2381643] Properly fix memory leaks from XMPDocOps::Clone.
// 07-17-09 AWL [2381643] Fix memory leaks from XMPDocOps::Clone (client-only fix).
// 05-21-09 AWL 5.0-c032 Revamp glue again to pass SetClientString with each function.
//
// 04-24-08 AWL 4.2.2-c051 [1765983] Add SXMPDocOps::IsDirty overload to return reason mask.
//
// 03-04-08 AWL Add Doxygen API comments.
// 02-22-08 AWL 4.2-c041 Add routines to SXMPDocOps - EnsureIDsExist, IsDirty, Clone, and GetMetaRef.
// 12-10-07 AWL Change name of fileType params to mimeType.
// 12-03-07 AWL 4.2-c028 Change file type parameters in XMPDocOps from XMP_FileFormat enum to a MIME string.
// 11-12-07 AWL 4.2-c026 More progress implementing XMPDocOps.
// 11-07-07 AWL 4.2-c025 More progress implementing XMPDocOps.
// 10-31-07 AWL 4.2-c023 Add new class XMPDocOps.
//
// =================================================================================================
#endif // AdobePrivate

// =================================================================================================
/// \file TXMPDocOps.hpp
/// \brief Services to support application "document operations".
///
/// @details\c TXMPDocOps is the template class providing services in support of common operations to
/// application documents. These services implement XMP policy that needs to be consistent across
/// applications. The template must be instantiated with a string class such as \c std::string. See
/// the instructions in XMP.hpp, and the Overview for a discussion of the overall architecture of
/// the XMP API.
// =================================================================================================

// =================================================================================================
/// \class TXMPDocOps TXMPDocOps.hpp
/// @brief Services to support application "document operations".
///
/// \c TXMPDocOps is a template class which must be instantiated with a string class such as
/// \c std::string. See the instructions in XMP.hpp, and the Overview for a discussion of the overall
/// architecture of the XMP API.
///
/// @details This class defines helper functions that support common behavior for high level operations on
/// application documents. Common examples are the user menu actions \c File/New, \c File/Open,
/// \c File/Save, \c File/SaveAs, etc. Applications must keep a per-document \c SXMPDocOps object
/// alongside their \c SXMPMeta object, document level state is kept by the \c SXMPDocOps object.
// =================================================================================================

#include <vector>
#if XMP_AndroidBuild
    #include "jni.h"
#endif

template <class tStringObj> class TXMPDocOps {

public:

    //  ============================================================================================
    /// \name Constructors, destructor, assignment
    /// @{
    ///
    /// ...

    // -----------------------------------------------------------------
    /// @brief Default constructor initializes an empty document object.

    TXMPDocOps();

    // ---------------------------------------------------------------------------------------------
    /// @brief Copy constructor
    ///
    /// Increments an internal reference count but does not perform a deep copy.
    ///
    /// @param original The existing \c TXMPDocOps object to copy. The new \c TXMPDocOps object returned via original.
    ///
    TXMPDocOps ( const TXMPDocOps<tStringObj> & original );

    // ---------------------------------------------------------------------------------------------
    /// @brief Reconstruct a \c TXMPDocOps object from an internal reference.
    ///
    /// @details This constructor creates a new \c TXMPDocOps object that refers to the underlying reference
    /// object of an existing \c TXMPDocOps object. Use to safely pass \c SXMPDocOps references across
    /// DLL boundaries.
    ///
    /// @param _xmpDocOpsRef The underlying reference object, obtained from some other XMP object
    /// with \c TXMPDocOps::GetInternalRef().
    ///

    TXMPDocOps ( XMPDocOpsRef _xmpDocOpsRef );

    // ---------------------------------------------------------------------------------------------
    /// @brief Destructor; typical virtual destructor.

    virtual ~TXMPDocOps() throw();

    // ---------------------------------------------------------------------------------------------
    /// @brief Assignment operator
    ///
    /// @details Increments an internal reference count but does not perform a deep copy. The two client
    /// objects will both refer to the same internal object.
    ///
    /// @param rhs The existing \c TXMPDocOps object.

	void operator= ( const TXMPDocOps<tStringObj> & rhs );

    // ---------------------------------------------------------------------------------------------
    /// @brief GetInternalRef() retrieves an internal reference that can be safely passed across DLL
    /// boundaries and reconstructed.
    ///
    /// @details Use with the reconstruction constructor to safely pass \c SXMPDocOps references across DLL
    /// boundaries where the clients might have used different string types when instantiating
    /// \c TXMPDocOps.
	///
    /// @return The internal reference.
    ///
    /// @see \c TXMPMeta::GetInternalRef() for usage.

    XMPDocOpsRef GetInternalRef();

    /// @}

    //  ============================================================================================
    /// \name Static functions
    /// @{
    ///
    /// Static utility functions.

    // ---------------------------------------------------------------------------------------------
    /// @brief \c SetAppName() sets the application's name.
    ///
    /// @details Use \c SetAppName to provide the application name which is used to set \c xmp:CreatorTool
    /// property and the \c stEvt:softwareAgent field of \c xmpMM:History entries.
    ///
    /// @param in_appName The application's name for use in the \c xmp:CreatorTool property.

    static void SetAppName ( XMP_StringPtr in_appName );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c CreateID() creates a new GUID string.
    ///
    /// @details Generates a new GUID and formats it as a string with the given prefix. It is recommended
    /// that the prefix end with a colon and follow W3C URI scheme syntax.
    ///
    /// @param out_xmpID The string to received the value of the generated GUID.
	/// @param prefix The prefix for the generated GUID.

    static void CreateID ( tStringObj * out_xmpID, XMP_StringPtr prefix = "xmp.id:" );

    /// @}

    //  ============================================================================================
    /// \name General document manipulation
    /// @{
    ///
    /// @details These are general operations intended to help applications maintain standard XMP properties.
    /// The basic model is that an application's document object would contain both an \c SXMPMeta
    /// object and an \c SXMPDocOps object. These functions define the association of those, and
    /// perform standard operations on the \c SXMPMeta for typical user actions like New, Open,
    /// Save, or SaveAs.

    // ---------------------------------------------------------------------------------------------
    /// @brief \c NewXMP() initializes the \c SXMPDocOps and \c SXMPMeta objects for a new document.
    ///
    /// @details An application should call \c NewXMP when it creates a new document to perform standard XMP
    /// initialization. This includes setting \c dc:format to the MIME type (if provided), setting
    /// \c xmp:CreatorTool to the application name (if provided via \c SetAppName), setting
    /// \c xmp:CreateDate to the current local date-time, and noting that the entire document has
    ///  changed (see \c NoteChange).
    ///
    /// @param inout_docXMP The SXMPMeta object associated with the application document. This should
    /// be an empty SXMPMeta object. The application can set other properties after calling \c NewXMP.
    ///
    /// @param in_mimeType Optional MIME type string, used to set \c dc:format.
    ///
    /// @param in_options There are no currently defined options.

    void NewXMP ( TXMPMeta<tStringObj> * inout_docXMP,
                  XMP_StringPtr  in_mimeType = "",
                  XMP_OptionBits in_options  = kXMP_NoOptions );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c OpenXMP() performs standard normalization when opening existing documents.
    ///
    /// @details An application should call \c OpenXMP when opening an existing document, after reading any
    /// existing XMP. The primary role of \c OpenXMP is to detect changes by other "ignorant"
    /// applications, and to prepare the \c SXMPDocOps object for editing operations.
    ///
    /// @param inout_docXMP The SXMPMeta object associated with the existing document. This should
    /// be initialized with the XMP from the document, if any.
    ///
    /// @param in_mimeType Optional MIME type string, used to detect changes and to set \c dc:format.
    /// If provided and different from an existing dc:format value, an xmpMM:History entry is
    /// appended to report the change in format.
    ///
    /// @param in_options There are no currently defined options.

    void OpenXMP ( TXMPMeta<tStringObj> * inout_docXMP,
                   XMP_StringPtr  in_mimeType,
                   XMP_OptionBits in_options = kXMP_NoOptions );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c OpenXMP() performs standard normalization when opening existing documents.
    ///
    /// @details This overload of \c OpenXMP adds an optional file path parameter.
    ///
    /// @param inout_docXMP The SXMPMeta object associated with the existing document. This should
    /// be initialized with the XMP from the document, if any.
    ///
    /// @param in_mimeType Optional MIME type string, used to detect changes and to set \c dc:format.
    /// If provided and different from an existing dc:format value, an xmpMM:History entry is
    /// appended to report the change in format.
    ///
    /// @param in_filePath An optional disk path of the file, should be an absolute path. Saved for
    /// use in \c PrepareForSave to detect SaveAs operations to a new location.
    ///
    /// @param in_options There are no currently defined options.

    void OpenXMP ( TXMPMeta<tStringObj> * inout_docXMP,
                   XMP_StringPtr  in_mimeType,
                   XMP_StringPtr  in_filePath,
                   XMP_OptionBits in_options = kXMP_NoOptions );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c OpenXMP() performs standard normalization when opening existing documents.
    ///
    /// @details This overload of \c OpenXMP adds an optional file modification time parameter.
    ///
    /// @param inout_docXMP The SXMPMeta object associated with the existing document. This should
    /// be initialized with the XMP from the document, if any.
    ///
    /// @param in_mimeType Optional MIME type string, used to detect changes and to set \c dc:format.
    /// If provided and different from an existing dc:format value, an xmpMM:History entry is
    /// appended to report the change in format.
    ///
    /// @param in_filePath An optional disk path of the file, should be an absolute path. Saved for
    ///  use in \c PrepareForSave to detect SaveAs operations to a new location.
    ///
    /// @param in_fileModTime An optional file modification time, presumably from the file system.
    ///  This is compared with the timestamp in the most recent "created" or "saved" \c xmpMM:History
    ///  entry. If this time is newer, presumably the file has been modified by an "ignorant"
    ///  application and a "modified" \c xmpMM:History entry is appended. See \c PrepareForSave.
    ///
    /// @param in_options There are no currently defined options.

    void OpenXMP ( TXMPMeta<tStringObj> * inout_docXMP,
                   XMP_StringPtr  in_mimeType,
                   XMP_StringPtr  in_filePath,
                   XMP_DateTime & in_fileModTime,
                   XMP_OptionBits in_options = kXMP_NoOptions );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c BranchXMP() is used to initialize a derived document's XMP.
    ///
    /// @details \c BranchXMP() is used to initialize a derived document's XMP. It should be called when
    /// deriving one document from another, for example when doing a Save As operation. It should be
    /// called on the source document:
    /// {code}
    /// sourceDoc.BranchXMP ( &derivedDoc, &derivedXMP, ... );
    /// {code}
    ///
    /// @param inout_derivedDoc The SXMPDocOps object for the derived document. Can be the same as
    /// the source document, in which case inout_derivedXMP must be null or match the source. 
    ///
    /// @param inout_derivedXMP The XMP to be associated with the derived document. Must not be null
    /// unless inout_derivedDoc matches the source document ("this").
    ///
    /// @param in_mimeType Optional MIME type string, used to detect changes and to set \c dc:format.
    /// If provided and different from an existing dc:format value, an xmpMM:History entry is
    /// appended to report the change in format.
    ///
    /// @param in_options There are no currently defined options.

    void BranchXMP ( TXMPDocOps<tStringObj> * inout_derivedDoc,
                     TXMPMeta<tStringObj> * inout_derivedXMP,
                     XMP_StringPtr  in_mimeType,
                     XMP_OptionBits in_options = kXMP_NoOptions );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c PrepareForSave() is used to finalize the XMP in preparation for saving the document.
    ///
    /// @details \c PrepareForSave() is used to finalize the XMP in preparation for saving the document. It
    /// makes sure that standard IDs exist, sets various date properties, appends appropriate
    /// history, etc. It does nothing though if the document is unchanged (not dirty).
    ///
    /// @param in_mimeType Optional MIME type string, used to detect changes and to set \c dc:format.
    /// If provided and different from an existing dc:format value, an xmpMM:History entry is
    /// appended to report the change in format.
    ///
    /// @param in_options There are no currently defined options.

    void PrepareForSave ( XMP_StringPtr  in_mimeType,
                          XMP_OptionBits in_options = kXMP_NoOptions );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c PrepareForSave() is used to finalize the XMP in preparation for saving the document.
    ///
    /// @details This overload of \c PrepareForSave adds an optional file path parameter.
    ///
    /// @param in_mimeType Optional MIME type string, used to detect changes and to set \c dc:format.
    /// If provided and different from an existing dc:format value, an xmpMM:History entry is
    /// appended to report the change in format.
    ///
    /// @param in_filePath An optional disk path of the file, should be an absolute path. Used to
    /// detect SaveAs operations to a new location.
    ///
    /// @param in_options There are no currently defined options.

    void PrepareForSave ( XMP_StringPtr  in_mimeType,
                          XMP_StringPtr  in_filePath,
                          XMP_OptionBits in_options = kXMP_NoOptions );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c PrepareForSave() is used to finalize the XMP in preparation for saving the document.
    ///
    /// @details This overload of \c PrepareForSave adds an optional file modification time parameter.
    ///
    /// @param in_mimeType Optional MIME type string, used to detect changes and to set \c dc:format.
    /// If provided and different from an existing dc:format value, an xmpMM:History entry is
    /// appended to report the change in format.
    ///
    /// @param in_filePath An optional disk path of the file, should be an absolute path. Used to
    /// detect SaveAs operations to a new location.
    ///
    /// @param in_fileModTime Optional file modification time. Saved in the history for later use to
    /// detect changes by XMP-unaware applications.
    ///
    /// @param in_options There are no currently defined options.

    void PrepareForSave ( XMP_StringPtr  in_mimeType,
                          XMP_StringPtr  in_filePath,
                          XMP_DateTime & in_fileModTime,
                          XMP_OptionBits in_options = kXMP_NoOptions );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c NoteChange() is used to report changes to some portion of a document.
    ///
    /// @details \c NoteChange() is used to report changes to some portion of a document. This information is
    /// later recorded in the history by \c PrepareForSave.
    ///
    /// @param in_part A string denoting the changed portion of the file. The syntax is extensible,
    /// consisting of a '/' separated hierarchy with a leading '/'. A set of standard parts is
    /// defined in XMP_Const.h:
	/// \li c kXMP_Part_All           Indicates "any" or "all"
	/// \li c kXMP_Part_Metadata      Portions of the metadata changed
	/// \li c kXMP_Part_Content       Any/all of the content (non-metadata)
	/// \li c kXMP_Part_Audio         Any/all sound
	/// \li c kXMP_Part_Visual        Any/all image data (video or still)
	/// \li c kXMP_Part_Video         Video or animation (moving image)
	/// \li c kXMP_Part_Raster        Static raster image
	/// \li c kXMP_Part_Vector        Static vector image
	/// \li c kXMP_Part_FormData      Form field data
	/// \li c kXMP_Part_FormTemplate  Form template
	/// \li c kXMP_Part_Annots        Applied annotations (comments)

    void NoteChange ( XMP_StringPtr in_part );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c NoteChange() is used to report changes to some portion of a document.
    ///
    /// @details This overload takes a vector of changed parts. It is equivalent to calling the single part
    /// form on each of the vector elements.
    ///
    /// @param in_parts A vector of strings, each following the part name syntax.

    void NoteChange ( std::vector<XMP_StringPtr> & in_parts );

    /// @}

    //  ============================================================================================
    /// \name File change detection
    /// @{
    ///
    /// Utilities that help applications determine what has changed in a file since last seen. A
    /// typical use might be in an application that creates a compound document from portions of a
    /// file, and wants to detect when other applications change those portions.

    // ---------------------------------------------------------------------------------------------
    /// @brief \c HasPartChanged() is used to determine if a portion of a file has changed.
    ///
    /// @details \c HasPartChanged is used to determine if a portion of a file has changed since some
    /// previous save with a given \c xmpMM:InstanceID property.
    ///
    /// @param in_prevInstanceID The \c xmpMM:InstanceID of the file when last seen.
    ///
    /// @param in_partName The part name of interest.
    ///
    /// @return Returns kXMPTS_Yes if the named part, or any ancestor, or any descendant, has changed.
    /// Returns kXMPTS_No if it has not changed. Returns kXMPTS_Maybe if yes or no cannot be determined,
    /// for example if the history has no entry for the given instance ID.

    XMP_TriState HasPartChanged ( XMP_StringPtr in_prevInstanceID,
     	                          XMP_StringPtr in_partName ) const;

    // ---------------------------------------------------------------------------------------------
    /// @brief \c HasPartChanged() is used to determine if a portion of a file has changed.
    ///
    /// This overload takes a vector of part names. 
    ///
    /// @param in_prevInstanceID The \c xmpMM:InstanceID of the file when last seen.
    ///
    /// @param in_partNames The part names of interest.
    ///
    /// @return Returns kXMPTS_Yes if any of the named parts, or any ancestor, or any descendant,
    /// have changed. Returns kXMPTS_No if none have changed. Returns kXMPTS_Maybe if yes or no
    /// cannot be determined, for example if the history has no entry for the given instance ID.

    XMP_TriState HasPartChanged ( XMP_StringPtr in_prevInstanceID,
								  std::vector<XMP_StringPtr> & in_partNames ) const;

    // ---------------------------------------------------------------------------------------------
    /// @brief \c GetPartChangeID() is used to determine the instance ID when a portion of a file changed.
    ///
    /// \c GetPartChangeID is used to determine the instance ID when a portion of a file most
    /// recently changed.
    ///
    /// @param in_partName The part name of interest.
    ///
    /// @param out_instanceID The instance ID of the file at the last save that changed the part.
    ///
    /// @return Returns true if history about the given part changing is found.

    bool GetPartChangeID ( XMP_StringPtr in_partName,
                           tStringObj *  out_instanceID ) const;

    // ---------------------------------------------------------------------------------------------
    /// @brief \c GetPartChangeID() is used to determine the instance ID when a portion of a file changed.
    ///
    /// This overload accepts a vector of part names. It returns information about the most recent
    /// change that affected any of the named parts.
    ///
    /// @param in_partNames The part names of interest.
    ///
    /// @param out_instanceID The instance ID of the file at the last save that changed any part.
    ///
    /// @return Returns true if history about any of the given parts changing is found.

    bool GetPartChangeID ( std::vector<XMP_StringPtr> & in_partNames,
                           tStringObj *  out_instanceID ) const;

    // ---------------------------------------------------------------------------------------------
    /// @brief \c EnsureIDsExist() is a simple utility to make sure standard GUIDs exist.
    ///
    /// \c EnsureIDsExist can be used to make sure that the \c xmpMM:InstanceID, \c xmpMM:DocumentID,
    /// and \c xmpMM:OriginalDocumentID properties exist. The options parameter can be used to limit
    /// which IDs are ensured.
    ///
    /// @param options Flags to control which IDs are ensured. The available values are:
    /// \li \c kXMPDocOps_IgnoreDocumentID - Don't create \c xmpMM:DocumentID or \c xmpMM:OriginalDocumentID.
    /// \li \c kXMPDocOps_IgnoreOriginalDocumentID - Don't create \c xmpMM:OriginalDocumentID.
    ///
    /// @return Returns true if any new IDs were created.

    /// @}

    //  ============================================================================================
    /// \name Miscellaneous utilities
    /// @{

    bool EnsureIDsExist ( XMP_OptionBits options = 0 );

    // ---------------------------------------------------------------------------------------------
    /// @brief \c IsDirty() is used to tell if the document has been changed.
    ///
    /// @param reasons Optional bit mask returning reasons why the document is dirty.
    ///
    /// @return Returns true if the document has been changed. See also NoteChange.

    bool IsDirty ( XMP_OptionBits * reasons ) const;
    bool IsDirty() const;

    // ---------------------------------------------------------------------------------------------
    /// @brief \c Clone() creates a deep copy of an SXMPDocOps object.
    ///
    /// Use this function to create an entirely separate copy of an SXMPDocOps object. Associated
    /// XMP is also cloned.  Assignment and copy constructors only increment a reference count, they
    /// do not do a deep copy. 
    ///
    /// @param options There are no currently defined options.
    ///
    /// @return An SXMPDocOps object cloned from the original.

    TXMPDocOps Clone ( XMP_OptionBits options = 0 ) const;

    // ---------------------------------------------------------------------------------------------
    /// @brief \c GetMetaRef() returns the internal reference for the associated XMP.
    ///
    /// \c GetMetaRef() returns the internal reference for the associated XMP. See \c SXMPMeta::GetInternalRef
    /// for additional details about why and how to use the internal reference.
    ///
    /// @return Returns the internal reference for the associated XMP, 0 if there is none.

    XMPMetaRef GetMetaRef() const;
    
#if XMP_AndroidBuild

	// ---------------------------------------------------------------------------------------------
    /// @brief \c SetJVM() allows client to Set JavaVM for android platform
    ///
	///
    static void SetJVM(JavaVM* vm);
#endif
    /// @}

    // =============================================================================================

private:

	XMPDocOpsRef xmpDocRef;

	static void SetClientString ( void * clientPtr, XMP_StringPtr valuePtr, XMP_StringLen valueLen );

};  // class TXMPDocOps

// =================================================================================================

#endif // __TXMPDocOps_hpp__

#endif	// AdobePrivate
