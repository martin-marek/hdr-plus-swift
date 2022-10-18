#if ! __WXMPIterator_hpp__
#define __WXMPIterator_hpp__ 1

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
//  ADC	Amandeep Chawla
//
// mm-dd-yy who Description of changes, most recent on top.
//
// 02-24-14 ADC 5.6-c001 XMPCommon Framework and XMPCore new APIs ported to Mac Environment.
//
// 05-21-09 AWL 5.0-c032 Revamp glue again to pass SetClientString with each function.
// 05-19-09 AWL 5.0-c031 First part of threading improvements, revamp the client glue.
//
// 03-24-06 AWL 4.0 Adapt for move to ham-perforce, integrate XMPFiles, bump version to 4.
//
// 05-16-05 AWL 3.3-100 Complete the deBIBification, integrate the internal and SDK source. Bump the
//              version to 3.3 and build to 100, well ahead of main's latest 3.3-009.
//
// 02-11-05 AWL 3.2-002 Add client reference counting.
// 01-28-05 AWL Remove BIB.
//
// 04-16-04 AWL Add forward declaration of class WXMPIterator for AIX.
// 02-14-04 AWL Remove all default parameters from the WXMP* classes.
// 01-17-04 AWL Move into new Perforce depot, cosmetic cleanup.
// 05-09-03 AWL Remove HasMore, make Next return a found/not-found status.
// 10-16-02 AWL Simplify member function names.
// 09-12-02 AWL Tweak to clean compile.
// 09-06-02 AWL First version for general review.
// 08-15-02 AWL Started first draft.
//
// =================================================================================================
#endif // AdobePrivate

#include "client-glue/WXMP_Common.hpp"

#if __cplusplus
extern "C" {
#endif

// =================================================================================================

#define zXMPIterator_PropCTor_1(xmpRef,schemaNS,propName,options) \
    WXMPIterator_PropCTor_1 ( xmpRef, schemaNS, propName, options, &wResult );

#define zXMPIterator_TableCTor_1(schemaNS,propName,options) \
    WXMPIterator_TableCTor_1 ( schemaNS, propName, options, &wResult );
    

#define zXMPIterator_Next_1(schemaNS,propPath,propValue,options,SetClientString) \
    WXMPIterator_Next_1 ( this->iterRef, schemaNS, propPath, propValue, options, SetClientString, &wResult );

#define zXMPIterator_Skip_1(options) \
    WXMPIterator_Skip_1 ( this->iterRef, options, &wResult );

// -------------------------------------------------------------------------------------------------

extern void
XMP_PUBLIC WXMPIterator_PropCTor_1 ( XMPMetaRef     xmpRef,
                          XMP_StringPtr  schemaNS,
                          XMP_StringPtr  propName,
                          XMP_OptionBits options,
                          WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPIterator_TableCTor_1 ( XMP_StringPtr  schemaNS,
                           XMP_StringPtr  propName,
                           XMP_OptionBits options,
                           WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPIterator_IncrementRefCount_1 ( XMPIteratorRef iterRef );

extern void
XMP_PUBLIC WXMPIterator_DecrementRefCount_1 ( XMPIteratorRef iterRef );

extern void
XMP_PUBLIC WXMPIterator_Next_1 ( XMPIteratorRef   iterRef,
                      void *           schemaNS,
                      void *           propPath,
                      void *           propValue,
                      XMP_OptionBits * options,
                      SetClientStringProc SetClientString,
                      WXMP_Result *    wResult );

extern void
XMP_PUBLIC WXMPIterator_Skip_1 ( XMPIteratorRef iterRef,
                      XMP_OptionBits options,
                      WXMP_Result *  wResult );

// =================================================================================================

#if __cplusplus
} /* extern "C" */
#endif

#endif  // __WXMPIterator_hpp__
