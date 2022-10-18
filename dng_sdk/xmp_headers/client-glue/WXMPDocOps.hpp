#if ! __WXMPDocOps_hpp__
#define __WXMPDocOps_hpp__ 1

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
// 04-24-08 AWL 4.2.2-c051 [1765983] Add SXMPDocOps::IsDirty overload to return reason mask.
//
// 02-22-08 AWL 4.2-c041 Add routines to SXMPDocOps - EnsureIDsExist, IsDirty, Clone, and GetMetaRef.
// 12-10-07 AWL Change name of fileType params to mimeType.
// 12-03-07 AWL 4.2-c028 Change file type parameters in XMPDocOps from XMP_FileFormat enum to a MIME string.
// 11-12-07 AWL 4.2-c026 More progress implementing XMPDocOps.
// 11-07-07 AWL 4.2-c025 More progress implementing XMPDocOps.
// 10-31-07 AWL 4.2-c023 Add new class XMPDocOps.
//
// =================================================================================================
#endif // AdobePrivate

#include "client-glue/WXMP_Common.hpp"

#if XMP_AndroidBuild
#include "jni.h"
#endif

#if __cplusplus
extern "C" {
#endif

// =================================================================================================

#define zXMPDocOps_SetAppName_1(appName) \
	WXMPDocOps_SetAppName_1 ( appName, &wResult );

#define zXMPDocOps_CreateID_1(prefix,clientID,SetClientString) \
	WXMPDocOps_CreateID_1 ( prefix, clientID, SetClientString, &wResult );

#define zXMPDocOps_CTor_1() \
	WXMPDocOps_CTor_1 ( &wResult )

#define zXMPDocOps_NewXMP_1(metaRef,mimeType,options) \
	WXMPDocOps_NewXMP_1 ( this->xmpDocRef, metaRef, mimeType, options, &wResult );

#define zXMPDocOps_OpenXMP_1(metaRef,mimeType,filePath,fileModTime,options) \
	WXMPDocOps_OpenXMP_1 ( this->xmpDocRef, metaRef, mimeType, filePath, fileModTime, options, &wResult );

#define zXMPDocOps_BranchXMP_1(derivedDocRef,derivedMetaRef,mimeType,options) \
	WXMPDocOps_BranchXMP_1 ( this->xmpDocRef, derivedDocRef, derivedMetaRef, mimeType, options, &wResult );

#define zXMPDocOps_PrepareForSave_1(mimeType,filePath,fileModTime,options) \
	WXMPDocOps_PrepareForSave_1 ( this->xmpDocRef, mimeType, filePath, fileModTime, options, &wResult );

#define zXMPDocOps_NoteChange_1(partsPtr,partsCount) \
	WXMPDocOps_NoteChange_1 ( this->xmpDocRef, partsPtr, partsCount, &wResult );

#define zXMPDocOps_HasPartChanged_1(prevInstanceID,partsPtr,partsCount) \
	WXMPDocOps_HasPartChanged_1 ( this->xmpDocRef, prevInstanceID, partsPtr, partsCount, &wResult );

#define zXMPDocOps_GetPartChangeID_1(partsPtr,partsCount,clientID,SetClientString) \
	WXMPDocOps_GetPartChangeID_1 ( this->xmpDocRef, partsPtr, partsCount, clientID, SetClientString, &wResult );

#define zXMPDocOps_EnsureIDsExist_1(options) \
	WXMPDocOps_EnsureIDsExist_1 ( this->xmpDocRef, options, &wResult );

#define zXMPDocOps_IsDirty_1(reasons) \
	WXMPDocOps_IsDirty_1 ( this->xmpDocRef, reasons, &wResult );

#define zXMPDocOps_Clone_1(options) \
	WXMPDocOps_Clone_1 ( this->xmpDocRef, options, &wResult );

#define zXMPDocOps_GetMetaRef_1() \
	WXMPDocOps_GetMetaRef_1 ( this->xmpDocRef, &wResult );
    
    
#if XMP_AndroidBuild
    #define zXMPDocOps_SetJVM_1(jvm) \
        WXMPDocOps_SetJVM_1 ( jvm, &wResult );
#endif

// =================================================================================================

extern void
XMP_PUBLIC WXMPDocOps_SetAppName_1 ( XMP_StringPtr appName,
									  WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPDocOps_CreateID_1 ( XMP_StringPtr prefix,
									void *        clientID,
									SetClientStringProc SetClientString,
									WXMP_Result * wResult );
#if XMP_AndroidBuild
extern void
XMP_PUBLIC WXMPDocOps_SetJVM_1 ( JavaVM* jvm,
                                WXMP_Result * wResult );
#endif

// -------------------------------------------------------------------------------------------------

extern void
XMP_PUBLIC WXMPDocOps_CTor_1 ( WXMP_Result * result );

extern void
XMP_PUBLIC WXMPDocOps_IncrementRefCount_1 ( XMPDocOpsRef xmpDocRef );

extern void
XMP_PUBLIC WXMPDocOps_DecrementRefCount_1 ( XMPDocOpsRef xmpDocRef );

// -------------------------------------------------------------------------------------------------

extern void
XMP_PUBLIC WXMPDocOps_NewXMP_1 ( XMPDocOpsRef   xmpDocRef,
								  XMPMetaRef     metaRef,
								  XMP_StringPtr  mimeType,
								  XMP_OptionBits options,
								  WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPDocOps_OpenXMP_1 ( XMPDocOpsRef   xmpDocRef,
								   XMPMetaRef     metaRef,
								   XMP_StringPtr  mimeType,
								   XMP_StringPtr  filePath,
								   XMP_DateTime * fileModTime,
								   XMP_OptionBits options,
								   WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPDocOps_BranchXMP_1 ( XMPDocOpsRef   xmpDocRef,
									 XMPDocOpsRef   derivedDocRef,
									 XMPMetaRef     derivedMetaRef,
									 XMP_StringPtr  mimeType,
									 XMP_OptionBits options,
									 WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPDocOps_PrepareForSave_1 ( XMPDocOpsRef   xmpDocRef,
										  XMP_StringPtr  mimeType,
										  XMP_StringPtr  filePath,
										  XMP_DateTime * fileModTime,
										  XMP_OptionBits options,
										  WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPDocOps_NoteChange_1 ( XMPDocOpsRef    xmpDocRef,
									  XMP_StringPtr * partsPtr,
									  XMP_Index       partsCount,
									  WXMP_Result *   wResult );

extern void
XMP_PUBLIC WXMPDocOps_HasPartChanged_1 ( XMPDocOpsRef    xmpDocRef,
										  XMP_StringPtr   prevInstanceID,
										  XMP_StringPtr * partsPtr,
										  XMP_Index       partsCount,
										  WXMP_Result *   wResult ) /* const */ ;

extern void
XMP_PUBLIC WXMPDocOps_GetPartChangeID_1 ( XMPDocOpsRef    xmpDocRef,
										   XMP_StringPtr * partsPtr,
										   XMP_Index       partsCount,
										   void *          clientID,
										   SetClientStringProc SetClientString,
										   WXMP_Result *   wResult ) /* const */ ;

extern void
XMP_PUBLIC WXMPDocOps_EnsureIDsExist_1 ( XMPDocOpsRef   xmpDocRef,
									  	  XMP_OptionBits options,
									  	  WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPDocOps_IsDirty_1 ( XMPDocOpsRef     xmpDocRef,
								   XMP_OptionBits * reasons,
								   WXMP_Result *    wResult ) /* const */ ;

extern void
XMP_PUBLIC WXMPDocOps_Clone_1 ( XMPDocOpsRef   xmpDocRef,
								 XMP_OptionBits options,
								 WXMP_Result *  wResult ) /* const */ ;

extern void
XMP_PUBLIC WXMPDocOps_GetMetaRef_1 ( XMPDocOpsRef  xmpDocRef,
									  WXMP_Result * wResult ) /* const */ ;

// =================================================================================================

#if __cplusplus
} /* extern "C" */
#endif

#endif  // __WXMPDocOps_hpp__
