#ifndef __TXMPAlbumArt_hpp__
#define __TXMPAlbumArt_hpp__ 1

#if ( ! __XMP_hpp__ )
    #error "Do not directly include, use XMP.hpp"
#endif

// =================================================================================================
// Copyright Adobe
// Copyright 2016 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

// =================================================================================================
/// \file TXMPAlbumArt.hpp
/// \brief API for access to the Album Artworks .
///
/// \c TXMPAlbumArt is the template class providing access services of Album Art in the XMP Toolkit. It must
/// be instantiated with a string class such as \c std::string. See the instructions in XMP.hpp, and
/// the Overview for a discussion of the overall architecture of the XMP API.
// =================================================================================================

// =================================================================================================
/// \class TXMPAlbumArt TXMPAlbumArt.hpp
/// @brief API for access to the Album Artworks.
///
/// \c TXMPAlbumArt is the template class providing the access services of Album Art in the XMP Toolkit.
/// It should be instantiated with a string class such as \c std::string. Read the Toolkit Overview for
/// information about the overall architecture of the XMP API, and the documentation for \c XMP.hpp
/// for specific instantiation instructions.
///
/// Access these functions through the concrete class, \c SXMPAlbumArt.
///
// =================================================================================================

template <class tStringObj> class TXMPAlbumArt {

public:

	///	@brief Specifies the usage of the image in the file.
	/// Valid values are started from "kXMPFiles_AlbumArt_Use_" string.
	/// Only one usage can exist for one album art object.
	XMP_Uns8 usageType;

	/// @brief Specifies the format of the image.
	/// Supported types are : JPEG, PNG, TIFF, GIF and URL of image.
	/// Corresponding constants are :
	///		kXMPFiles_AlbumArt_NoFormat, ( No format information )
	///		kXMPFiles_AlbumArt_JPEG, ( JPEG )
	///		kXMPFiles_AlbumArt_PNG,	( PNG )
	///		kXMPFiles_AlbumArt_TIFF, ( TIFF )
	///		kXMPFiles_AlbumArt_GIF,	( GIF )
	///		kXMPFiles_AlbumArt_URLImage, ( Image is not directly embedded but URL of the image resource exists in imageData.
	///										Used for external resource dependency )
	///		kXMPFiles_AlbumArt_UnSupportedFormat ( format information exists but not among the specified types )
	XMP_Uns8 formatType;

	/// @brief Specifies the short description (UTF-8) of the image.
	/// This string must exists in UTF-8 encoding.
	/// The string can be converted in different encodings using variable encodingType.
	/// So, User will get description in UTF-8 encoding and original encoding will be present in variable encodingType.
	/// While setting description, User needs to set required value in UTF-8 and required final encoding using varaible encodingType.
	tStringObj description;

	/// @brief Specifies encoding type of the description in the file.
	/// Valid values are :
	///	kXMPFiles_AlbumArt_Enc_UTF8 , Use UTF8 encoding
	///	kXMPFiles_AlbumArt_Enc_UTF16Big , Use UTF16 big-endian encoding
	///	kXMPFiles_AlbumArt_Enc_UTF16Little , Use UTF16 little-endian encoding
	///	kXMPFiles_AlbumArt_Enc_UTF32Big , Use UTF32 big-endian encoding
	///	kXMPFiles_AlbumArt_Enc_UTF32Little , Use UTF32 little-endian encoding
	///	kXMPFiles_AlbumArt_Enc_Latin1 , Use Latin1 encoding
	///	kXMPFiles_AlbumArt_Enc_Unknown , None of the defined encodings
	/// Note: While updating album arts in a file, we will try to write description according to provided encoding type if that file format supports provided encoding type,
	///		  Else, value will be written in any supported type of encoding of that format.
	XMP_Uns8 encodingType;

	// ---------------------------------------------------------------------------------------------
	/// @brief Constructs an object
	///
	/// This constructor creates a new \c TXMPAlbumArt objects and initializes its member variables with 
	/// the values given in the arguments. Its sets the imageData to NULL and imageLength to 0. 
	/// All the arguments are optional here.
	///
	/// @param usageType Specifies the usage of the image in the file.
	///
	/// @param formatType Specifies the format of the image.
	///
	/// @param description Specifies the short description (UTF-8) of the image.
	///
	/// @param encodingType Specifies encoding type of the description in the file.


	TXMPAlbumArt ( XMP_Uns8 usageType = kXMPFiles_AlbumArt_Use_None,
		XMP_Uns8 formatType = kXMPFiles_AlbumArt_NoFormat,
		tStringObj description = tStringObj(),
		XMP_Uns8 encodingType = kXMPFiles_AlbumArt_Enc_UTF8 );

	// ---------------------------------------------------------------------------------------------
	/// @brief Constructs an object 
	/// 
	/// This constructor creates a new \c TXMPAlbumArt objects and initializes its member variables with 
	/// the values given in the arguments.
	///
	/// @param imageLength Length of image data.
	///
	/// @param imageData Binary data of the image. 
	///
	/// @param createCopy Specifies whether to deep copy the imageData or shallow copy. 
	///             If it is true then deep copy of imageData is performed. 
	///
	/// @param usageType Specifies the usage of the image in the file.
	///
	/// @param formatType Specifies the format of the image.
	///
	/// @param description Specifies the short description (UTF-8) of the image.
	///
	/// @param encodingType Specifies encoding type of the description in the file.

	TXMPAlbumArt ( XMP_Uns32 imageLength, 
		XMP_BinaryData imageData, 
		XMP_Bool createCopy = false, 
		XMP_Uns8 usageType = kXMPFiles_AlbumArt_Use_None,
		XMP_Uns8 formatType = kXMPFiles_AlbumArt_NoFormat,
		tStringObj description = tStringObj(),
		XMP_Uns8 encodingType = kXMPFiles_AlbumArt_Enc_UTF8 );

	// ---------------------------------------------------------------------------------------------
	/// @brief Destructor of the TXMPAlbumArt
	///
	/// This will free memory allocated for the imageData if its local copy was created.
	///

	virtual ~TXMPAlbumArt();
	
	// ---------------------------------------------------------------------------------------------
	/// @brief Copy Constructor 
	///
	/// This will copy (deep copy) data of provided TXMPAlbumArt object into the current TXMPAlbumArt.
	/// Memory for the current imagedata will be allocated on heap using imageLength.
	///
	/// @param original An existing AlbumArt object to copy.

	TXMPAlbumArt( const TXMPAlbumArt & original );
	
	// ---------------------------------------------------------------------------------------------
	/// @brief Assignment operator
	///
	/// Assigns the TXMPAlbumArt data from an existing object to current TXMPAlbumArt.
	/// This will also allocate memory for the imageData which will be deallocated by the destructor.
	/// 
	/// @param rhs An existing AlbumArt object. 

	void operator= ( const TXMPAlbumArt & rhs );

	// ---------------------------------------------------------------------------------------------
	/// @brief \c GetImageData() obtains the imageData and imageLength of TXMPAlbumArt object.
	/// 
	/// @param imageData [out] The XMPBinaryData in which to return the imageData of this object.
	///
	/// @return Length of image data.

	XMP_Uns32 GetImageData ( XMP_BinaryData * imageData = NULL );

	// ---------------------------------------------------------------------------------------------
	/// @brief \c SetImageData() sets the imageData and imageLength of TXMPAlbumArt object.
	/// 
	/// It deallocates the existing memory allocated for image data if local copy exists.
	///
	/// @param imageLength Length of image data.
	///
	/// @param imageData The XMPBinaryData in which to return the imageData of this object.
	///
	/// @param createCopy Specifies whether to use the memory provided by the client or to create its local copy for image data .
	///
	

	void SetImageData ( const XMP_Uns32 & imageLength , XMP_BinaryData imageData , bool createCopy = false );

private:
	/// Specifies unprocessed image data
	/// Format of this unprocessed image will be specified by formatType
	XMP_BinaryData imageData;

	/// Specifies the length of the unprocessed image
	XMP_Uns32 imageLength;

	/// Specifies whether to use the memory provided by the client or to create its local copy for image data.
	XMP_Bool isLocalCopy;
};  // class TXMPAlbumArt

// =================================================================================================

#endif // __TXMPAlbumArt_hpp__
