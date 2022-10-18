#ifndef __XMP_hpp__
#define __XMP_hpp__ 1

// =================================================================================================
// Copyright 2002 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
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
//  AWL Alan Lillich
//
// mm/dd/yy who Description of changes, most recent on top.
//
// 03-16-15 AJ  5.6-c048 Fixing build break due to XMPCore.def
// 03-02-15 AJ  5.6-c039 Marker Extensibility - Backward compatibility for extensions in GetBulkMarkers() and SetBulkMarkers()
// 05-19-09 AWL 5.0-c031-f039 First part of threading improvements. Force full rebuilds.
//
// 10-31-07 AWL 4.2-c023 Add new class XMPDocOps.
// 08-24-07 AWL 4.2-c019 Remove support for ancient CXMP_* init/term routines and non-public (AXE) expat.
// 08-02-07 AWL 4.2 Incorporate major revamp to doxygen comments.
//
// 03-24-06 AWL 4.0 Adapt for move to ham-perforce, integrate XMPFiles, bump version to 4.
//
// 04-19-05 AWL Improve Doxygen comments for SDK.
// 01-28-05 AWL Remove BIB.
// 01-17-04 AWL Move into new Perforce depot, cosmetic cleanup.
// 03-06-03 AWL Add conditional template class typedefs and Doxygen comments.
// 09-11-02 AWL Started first draft.
//
// =================================================================================================
#endif // AdobePrivate

//  ================================================================================================
/// \file XMP.hpp
/// \brief Overall header file for the XMP Toolkit
///
/// This is an overall header file, the only one that C++ clients should include.
///
/// The full client API is in the \c TXMPMeta.hpp, \c TXMPIterator.hpp, \c TXMPUtils.hpp headers.
/// Read these for information, but do not include them directly. The \c TXMP... classes are C++
/// template classes that must be instantiated with a string class such as \c std::string. The
/// string class is used to return text strings for property values, serialized XMP, and so on.
/// Clients must also compile \c XMP.incl_cpp to ensure that all client-side glue code is generated.
/// This should be done by including it in exactly one client source file.
///
/// There are two C preprocessor macros that simplify use of the templates:
///
/// \li \c TXMP_STRING_TYPE - Define this as the string class to use with the template. You will get
/// the template headers included and typedefs (\c SXMPMeta, and so on) to use in your code.
///
/// \li \c TXMP_EXPAND_INLINE - Define this as 1 if you want to have the template functions expanded
/// inline in your code. Leave it undefined, or defined as 0, to use out-of-line instantiations of
/// the template functions. Compiling \c XMP.incl_cpp generates explicit out-of-line
/// instantiations if \c TXMP_EXPAND_INLINE is off.
///
/// The template parameter, class \c tStringObj, must have the following member functions (which
/// match those for \c std::string):
///
/// <pre>
///  tStringObj& assign ( const char * str, size_t len )
///  size_t size() const
///  const char * c_str() const
/// </pre>
///
/// The string class must be suitable for at least UTF-8. This is the encoding used for all general
/// values, and is the default encoding for serialized XMP. The string type must also be suitable
/// for UTF-16 or UTF-32 if those serialization encodings are used. This mainly means tolerating
/// embedded 0 bytes, which \c std::string does.
//  ================================================================================================

/// /c XMP_Environment.h must be the first included header.
#include "XMP_Environment.h"

#include "XMP_Version.h"
#include "XMP_Const.h"

#if XMP_WinBuild
    #if XMP_DebugBuild
        #pragma warning ( push, 4 )
    #else
        #pragma warning ( push, 3 )
    #endif
    #pragma warning ( disable : 4702 ) // unreachable code
    #pragma warning ( disable : 4800 ) // forcing value to bool 'true' or 'false' (performance warning)
#endif

#if defined ( TXMP_STRING_TYPE )

    #include "TXMPMeta.hpp"
    #include "TXMPIterator.hpp"
    #include "TXMPUtils.hpp"
	#if AdobePrivate
    	#include "TXMPDocOps.hpp"
	#endif
	
    typedef class TXMPMeta <TXMP_STRING_TYPE>     SXMPMeta;       // For client convenience.
    typedef class TXMPIterator <TXMP_STRING_TYPE> SXMPIterator;
    typedef class TXMPUtils <TXMP_STRING_TYPE>    SXMPUtils;
    #if AdobePrivate
	    typedef class TXMPDocOps <TXMP_STRING_TYPE>   SXMPDocOps;
	#endif
	
    #if TXMP_EXPAND_INLINE
    	#error "TXMP_EXPAND_INLINE is not working at present. Please don't use it."
        #include "client-glue/TXMPMeta.incl_cpp"
        #include "client-glue/TXMPIterator.incl_cpp"
        #include "client-glue/TXMPUtils.incl_cpp"
        #if AdobePrivate
	        #include "client-glue/TXMPDocOps.incl_cpp"
        #endif
        #include "client-glue/TXMPFiles.incl_cpp"
    #endif

	#if XMP_INCLUDE_XMPFILES
#if AdobePrivate
		#include "TXMPAlbumArt.hpp"
		typedef class TXMPAlbumArt<TXMP_STRING_TYPE>    SXMPAlbumArt;
#endif
		#include "TXMPFiles.hpp"	// ! Needs typedef for SXMPMeta.
		typedef class TXMPFiles <TXMP_STRING_TYPE>    SXMPFiles;
		#if TXMP_EXPAND_INLINE
			#include "client-glue/TXMPFiles.incl_cpp"
		#endif
	#endif

#endif  // TXMP_STRING_TYPE

#if XMP_WinBuild
    #pragma warning ( pop )
#endif

// =================================================================================================

#endif  // __XMP_hpp__
