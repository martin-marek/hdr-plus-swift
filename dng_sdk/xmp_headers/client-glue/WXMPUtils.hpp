#if ! __WXMPUtils_hpp__
#define __WXMPUtils_hpp__ 1

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
// 03-16-15 AJ  5.6-c048 Fixing build break due to XMPCore.def
// 03-16-15 AJ  5.6-c047 Completing Rework of Get/SetBulkMarkers()
// 12-03-15 AJ  5.6-c045 Reworking glue code for Get/SetBulkMarkers()
// 03-02-15 AJ  5.6-c041 Fixed build break.
// 03-02-15 AJ  5.6-c039 Marker Extensibility - Backward compatibility for extensions in GetBulkMarkers() and SetBulkMarkers()
// 02-24-14 ADC 5.6-c001 XMPCommon Framework and XMPCore new APIs ported to Mac Environment.
//
// 07-21-09 AWL 5.0-c047 Add XMPUtils::ApplyTemplate.
// 05-21-09 AWL 5.0-c032 Revamp glue again to pass SetClientString with each function.
// 05-19-09 AWL 5.0-c031 First part of threading improvements, revamp the client glue.
// 01-23-09 AWL 5.0-c005 [2261961] Fix Get/SetBulkMarkers to tolerate varying size of cuePointParams vector.
//
// 10-13-08 AWL 4.4-c009 Add support for zone-less times. Get rid of old XMPUtils forms of DocOps.
//
// 05-05-08 AWL 4.2.2-c055 [1768777] Add private callbacks for CuePointParams storage in Get/SetBulkMarkers.
// 04-29-08 AWL 4.2.2-c052 [1768777,1768782] Add private performance enhancements for temporal metadata.
//
// 05-16-06 AWL 4.0-c006 Add SXMPUtils::PackageForJPEG and SXMPUtils::MergeFromJPEG.
// 03-24-06 AWL 4.0 Adapt for move to ham-perforce, integrate XMPFiles, bump version to 4.
//
// 02-21-06 AWL 3.3-015 Expose some of the "File Info" utils. The Adobe Labs build of XMPFiles needs
//				SXMPUtils::RemoveProperties and SXMPUtils::AppendProperties.
// 07-15-05 AWL 3.3-001 [1214033] Bump version to 3.3, the SDK is out. Put back missing multi-file utils.
//
// 05-16-05 AWL 3.2-100 Complete the deBIBification, integrate the internal and SDK source.
// 04-11-05 AWL 3.2-016 Add AdobePrivate conditionals where appropriate.
// 01-28-05 AWL 3.2-001 Remove BIB.
//
// 11-04-04 AWL 3.1.1-090 [1014853] Add XMPUtils::RemoveMultiValueInfo.
//
// 08-18-04 AWL 3.1-075 Add HasContainedDoc.
//
// 03-29-04 AWL Add Q&D versions of GetMainPacket and UpdateMainPacket.
// 02-17-04 AWL Add multi-file utilities. Rename CreateXyzDocument to InitializeXyzDocument.
// 02-14-04 AWL Remove all default parameters from the CXMP* classes.
// 02-09-04 AWL Start adding the new Document Operation utilities.
// 01-17-04 AWL Move into new Perforce depot, cosmetic cleanup.
// 10-15-02 AWL Add more functions, fix ConvertFrom strValue type, change into a class.
// 09-12-02 AWL Tweak to clean compile.
// 09-06-02 AWL First version for general review.
// 08-23-02 AWL Started first draft.
//
// =================================================================================================
#endif // AdobePrivate

#include "client-glue/WXMP_Common.hpp"
#if __cplusplus
extern "C" {
#endif

// =================================================================================================

#define zXMPUtils_ComposeArrayItemPath_1(schemaNS,arrayName,itemIndex,itemPath,SetClientString) \
    WXMPUtils_ComposeArrayItemPath_1 ( schemaNS, arrayName, itemIndex, itemPath, SetClientString, &wResult );

#define zXMPUtils_ComposeStructFieldPath_1(schemaNS,structName,fieldNS,fieldName,fieldPath,SetClientString) \
    WXMPUtils_ComposeStructFieldPath_1 ( schemaNS, structName, fieldNS, fieldName, fieldPath, SetClientString, &wResult );

#define zXMPUtils_ComposeQualifierPath_1(schemaNS,propName,qualNS,qualName,qualPath,SetClientString) \
    WXMPUtils_ComposeQualifierPath_1 ( schemaNS, propName, qualNS, qualName, qualPath, SetClientString, &wResult );

#define zXMPUtils_ComposeLangSelector_1(schemaNS,arrayName,langName,selPath,SetClientString) \
    WXMPUtils_ComposeLangSelector_1 ( schemaNS, arrayName, langName, selPath, SetClientString, &wResult );

#define zXMPUtils_ComposeFieldSelector_1(schemaNS,arrayName,fieldNS,fieldName,fieldValue,selPath,SetClientString) \
    WXMPUtils_ComposeFieldSelector_1 ( schemaNS, arrayName, fieldNS, fieldName, fieldValue, selPath, SetClientString, &wResult );

#define zXMPUtils_ConvertFromBool_1(binValue,strValue,SetClientString) \
    WXMPUtils_ConvertFromBool_1 ( binValue, strValue, SetClientString, &wResult );

#define zXMPUtils_ConvertFromInt_1(binValue,format,strValue,SetClientString) \
    WXMPUtils_ConvertFromInt_1 ( binValue, format, strValue, SetClientString, &wResult );

#define zXMPUtils_ConvertFromInt64_1(binValue,format,strValue,SetClientString) \
    WXMPUtils_ConvertFromInt64_1 ( binValue, format, strValue, SetClientString, &wResult );

#define zXMPUtils_ConvertFromFloat_1(binValue,format,strValue,SetClientString) \
    WXMPUtils_ConvertFromFloat_1 ( binValue, format, strValue, SetClientString, &wResult );

#define zXMPUtils_ConvertFromDate_1(binValue,strValue,SetClientString) \
    WXMPUtils_ConvertFromDate_1 ( binValue, strValue, SetClientString, &wResult );

#define zXMPUtils_ConvertToBool_1(strValue) \
    WXMPUtils_ConvertToBool_1 ( strValue, &wResult );

#define zXMPUtils_ConvertToInt_1(strValue) \
    WXMPUtils_ConvertToInt_1 ( strValue, &wResult );

#define zXMPUtils_ConvertToInt64_1(strValue) \
    WXMPUtils_ConvertToInt64_1 ( strValue, &wResult );

#define zXMPUtils_ConvertToFloat_1(strValue) \
    WXMPUtils_ConvertToFloat_1 ( strValue, &wResult );

#define zXMPUtils_ConvertToDate_1(strValue,binValue) \
    WXMPUtils_ConvertToDate_1 ( strValue, binValue, &wResult );

#define zXMPUtils_CurrentDateTime_1(time) \
    WXMPUtils_CurrentDateTime_1 ( time, &wResult );

#define zXMPUtils_SetTimeZone_1(time) \
    WXMPUtils_SetTimeZone_1 ( time, &wResult );

#define zXMPUtils_ConvertToUTCTime_1(time) \
    WXMPUtils_ConvertToUTCTime_1 ( time, &wResult );

#define zXMPUtils_ConvertToLocalTime_1(time) \
    WXMPUtils_ConvertToLocalTime_1 ( time, &wResult );

#define zXMPUtils_CompareDateTime_1(left,right) \
    WXMPUtils_CompareDateTime_1 ( left, right, &wResult );

#define zXMPUtils_EncodeToBase64_1(rawStr,rawLen,encodedStr,SetClientString) \
    WXMPUtils_EncodeToBase64_1 ( rawStr, rawLen, encodedStr, SetClientString, &wResult );

#define zXMPUtils_DecodeFromBase64_1(encodedStr,encodedLen,rawStr,SetClientString) \
    WXMPUtils_DecodeFromBase64_1 ( encodedStr, encodedLen, rawStr, SetClientString, &wResult );

#define zXMPUtils_PackageForJPEG_1(xmpObj,stdStr,extStr,digestStr,SetClientString) \
    WXMPUtils_PackageForJPEG_1 ( xmpObj, stdStr, extStr, digestStr, SetClientString, &wResult );

#define zXMPUtils_MergeFromJPEG_1(fullXMP,extendedXMP) \
    WXMPUtils_MergeFromJPEG_1 ( fullXMP, extendedXMP, &wResult );

#define zXMPUtils_CatenateArrayItems_1(xmpObj,schemaNS,arrayName,separator,quotes,options,catedStr,SetClientString) \
    WXMPUtils_CatenateArrayItems_1 ( xmpObj, schemaNS, arrayName, separator, quotes, options, catedStr, SetClientString, &wResult );

#define zXMPUtils_SeparateArrayItems_1(xmpObj,schemaNS,arrayName,options,catedStr) \
    WXMPUtils_SeparateArrayItems_1 ( xmpObj, schemaNS, arrayName, options, catedStr, &wResult );

#define zXMPUtils_ApplyTemplate_1(workingXMP,templateXMP,actions) \
    WXMPUtils_ApplyTemplate_1 ( workingXMP, templateXMP, actions, &wResult );

#define zXMPUtils_RemoveProperties_1(xmpObj,schemaNS,propName,options) \
    WXMPUtils_RemoveProperties_1 ( xmpObj, schemaNS, propName, options, &wResult );

#if AdobePrivate
#define zXMPUtils_AppendProperties_1(source,dest,options) \
    WXMPUtils_AppendProperties_1 ( source, dest, options, &wResult );
#endif

#define zXMPUtils_DuplicateSubtree_1(source,dest,sourceNS,sourceRoot,destNS,destRoot,options) \
    WXMPUtils_DuplicateSubtree_1 ( source, dest, sourceNS, sourceRoot, destNS, destRoot, options, &wResult );

#if AdobePrivate

#define zCollectMultiFileXMP_1(inputXMP,multiXMP,options) \
    WXMPUtils_CollectMultiFileXMP_1 ( inputXMP, multiXMP, options, &wResult );

#define zDistributeMultiFileXMP_1(multiXMP,outputXMP,options) \
    WXMPUtils_DistributeMultiFileXMP_1 ( multiXMP, outputXMP, options, &wResult );

#define zIsPropertyMultiValued_1(multiXMP,propNS,propName) \
    WXMPUtils_IsPropertyMultiValued_1 ( multiXMP, propNS, propName, &wResult );

#define zGetDateRange_1(multiXMP,propNS,propName,oldest,newest) \
    WXMPUtils_GetDateRange_1 ( multiXMP, propNS, propName, oldest, newest, &wResult );

#define zGetMergedListPath_1(multiXMP,propNS,propName,pathPtr,SetClientString) \
    WXMPUtils_GetMergedListPath_1 ( multiXMP, propNS, propName, pathPtr, SetClientString, &wResult );

#define zRemoveMultiValueInfo_1(multiXMP,propNS,propName) \
    WXMPUtils_RemoveMultiValueInfo_1 ( multiXMP, propNS, propName, &wResult );

#endif	// AdobePrivate

#if AdobePrivate

#define zXMPUtils_GetBulkMarkers_1(xmp,ns,path,array,elemSize,allocMarker,allocCue) \
    WXMPUtils_GetBulkMarkers_1 ( xmp, ns, path, array, elemSize, allocMarker,allocCue, &wResult );

#define zXMPUtils_SetBulkMarkers_1(xmp,ns,path,count,array,elemSize,getProc) \
    WXMPUtils_SetBulkMarkers_1 ( xmp, ns, path, count, array, elemSize, getProc, &wResult );

#define zXMPUtils_GetBulkMarkers_2(xmp,ns,path,array,elemSize,allocMarker,allocCue) \
    WXMPUtils_GetBulkMarkers_2 ( xmp, ns, path, array, elemSize, allocMarker,allocCue, &wResult );

#define zXMPUtils_SetBulkMarkers_2(xmp,ns,path,count,array,elemSize,getProc) \
    WXMPUtils_SetBulkMarkers_2 ( xmp, ns, path, count, array, elemSize, getProc, &wResult );

#endif	// AdobePrivate

// =================================================================================================

extern void
XMP_PUBLIC WXMPUtils_ComposeArrayItemPath_1 ( XMP_StringPtr   schemaNS,
                                   XMP_StringPtr   arrayName,
                                   XMP_Index       itemIndex,
                                   void *          itemPath,
                                   SetClientStringProc SetClientString,
                                   WXMP_Result *   wResult );

extern void
XMP_PUBLIC WXMPUtils_ComposeStructFieldPath_1 ( XMP_StringPtr   schemaNS,
                                     XMP_StringPtr   structName,
                                     XMP_StringPtr   fieldNS,
                                     XMP_StringPtr   fieldName,
                                     void *          fieldPath,
                                     SetClientStringProc SetClientString,
                                     WXMP_Result *   wResult );

extern void
XMP_PUBLIC WXMPUtils_ComposeQualifierPath_1 ( XMP_StringPtr   schemaNS,
                                   XMP_StringPtr   propName,
                                   XMP_StringPtr   qualNS,
                                   XMP_StringPtr   qualName,
                                   void *          qualPath,
                                   SetClientStringProc SetClientString,
                                   WXMP_Result *   wResult );

extern void
XMP_PUBLIC WXMPUtils_ComposeLangSelector_1 ( XMP_StringPtr   schemaNS,
                                  XMP_StringPtr   arrayName,
                                  XMP_StringPtr   langName,
                                  void *          selPath,
                                  SetClientStringProc SetClientString,
                                  WXMP_Result *   wResult );

extern void
XMP_PUBLIC WXMPUtils_ComposeFieldSelector_1 ( XMP_StringPtr   schemaNS,
                                   XMP_StringPtr   arrayName,
                                   XMP_StringPtr   fieldNS,
                                   XMP_StringPtr   fieldName,
                                   XMP_StringPtr   fieldValue,
                                   void *          selPath,
                                   SetClientStringProc SetClientString,
                                   WXMP_Result *   wResult );

// -------------------------------------------------------------------------------------------------

extern void
XMP_PUBLIC WXMPUtils_ConvertFromBool_1 ( XMP_Bool      binValue,
                              void *        strValue,
                              SetClientStringProc SetClientString,
                              WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertFromInt_1 ( XMP_Int32     binValue,
                             XMP_StringPtr format,
                             void *        strValue,
                             SetClientStringProc SetClientString,
                             WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertFromInt64_1 ( XMP_Int64     binValue,
                               XMP_StringPtr format,
                               void *        strValue,
                               SetClientStringProc SetClientString,
                               WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertFromFloat_1 ( double        binValue,
                               XMP_StringPtr format,
                               void *        strValue,
                               SetClientStringProc SetClientString,
                               WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertFromDate_1 ( const XMP_DateTime & binValue,
                              void *               strValue,
                              SetClientStringProc SetClientString,
                              WXMP_Result *        wResult );

// -------------------------------------------------------------------------------------------------

extern void
XMP_PUBLIC WXMPUtils_ConvertToBool_1 ( XMP_StringPtr strValue,
                            WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertToInt_1 ( XMP_StringPtr strValue,
                           WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertToInt64_1 ( XMP_StringPtr strValue,
                             WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertToFloat_1 ( XMP_StringPtr strValue,
                             WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertToDate_1 ( XMP_StringPtr  strValue,
                            XMP_DateTime * binValue,
                            WXMP_Result *  wResult );

// -------------------------------------------------------------------------------------------------

extern void
XMP_PUBLIC WXMPUtils_CurrentDateTime_1 ( XMP_DateTime * time,
                              WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_SetTimeZone_1 ( XMP_DateTime * time,
                          WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertToUTCTime_1 ( XMP_DateTime * time,
                               WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_ConvertToLocalTime_1 ( XMP_DateTime * time,
                                 WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_CompareDateTime_1 ( const XMP_DateTime & left,
                              const XMP_DateTime & right,
                              WXMP_Result *        wResult );

// -------------------------------------------------------------------------------------------------

extern void
XMP_PUBLIC WXMPUtils_EncodeToBase64_1 ( XMP_StringPtr rawStr,
                             XMP_StringLen rawLen,
                             void *        encodedStr,
                             SetClientStringProc SetClientString,
                             WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_DecodeFromBase64_1 ( XMP_StringPtr encodedStr,
                               XMP_StringLen encodedLen,
                               void *        rawStr,
                               SetClientStringProc SetClientString,
                               WXMP_Result * wResult );

// -------------------------------------------------------------------------------------------------

extern void
XMP_PUBLIC WXMPUtils_PackageForJPEG_1 ( XMPMetaRef    xmpObj,
                             void *        stdStr,
                             void *        extStr,
                             void *        digestStr,
                             SetClientStringProc SetClientString,
                             WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_MergeFromJPEG_1 ( XMPMetaRef    fullXMP,
                            XMPMetaRef    extendedXMP,
                            WXMP_Result * wResult );

// -------------------------------------------------------------------------------------------------

extern void
XMP_PUBLIC WXMPUtils_CatenateArrayItems_1 ( XMPMetaRef     xmpObj,
                                 XMP_StringPtr  schemaNS,
                                 XMP_StringPtr  arrayName,
                                 XMP_StringPtr  separator,
                                 XMP_StringPtr  quotes,
                                 XMP_OptionBits options,
                                 void *         catedStr,
                                 SetClientStringProc SetClientString,
                                 WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_SeparateArrayItems_1 ( XMPMetaRef     xmpObj,
                                 XMP_StringPtr  schemaNS,
                                 XMP_StringPtr  arrayName,
                                 XMP_OptionBits options,
                                 XMP_StringPtr  catedStr,
                                 WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_ApplyTemplate_1 ( XMPMetaRef     workingXMP,
							XMPMetaRef     templateXMP,
							XMP_OptionBits options,
							WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_RemoveProperties_1 ( XMPMetaRef     xmpObj,
                               XMP_StringPtr  schemaNS,
                               XMP_StringPtr  propName,
                               XMP_OptionBits options,
                               WXMP_Result *  wResult );

#if AdobePrivate
extern void
XMP_PUBLIC WXMPUtils_AppendProperties_1 ( XMPMetaRef     source,
                               XMPMetaRef     dest,
                               XMP_OptionBits options,
                               WXMP_Result *  wResult );
#endif

extern void
XMP_PUBLIC WXMPUtils_DuplicateSubtree_1 ( XMPMetaRef     source,
                               XMPMetaRef     dest,
                               XMP_StringPtr  sourceNS,
                               XMP_StringPtr  sourceRoot,
                               XMP_StringPtr  destNS,
                               XMP_StringPtr  destRoot,
                               XMP_OptionBits options,
                               WXMP_Result *  wResult );

#if AdobePrivate

extern void
XMP_PUBLIC WXMPUtils_CollectMultiFileXMP_1 ( XMPMetaRef     inputXMP,
                                  XMPMetaRef     multiXMP,
                                  XMP_OptionBits options,
                                  WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_DistributeMultiFileXMP_1 ( XMPMetaRef     multiXMP,
                                     XMPMetaRef     outputXMP,
                                     XMP_OptionBits options,
                                     WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_IsPropertyMultiValued_1 ( XMPMetaRef    multiXMP,
                                    XMP_StringPtr propNS,
                                    XMP_StringPtr propName,
                                    WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_GetDateRange_1 ( XMPMetaRef    multiXMP,
                           XMP_StringPtr propNS,
                           XMP_StringPtr propName,
                           XMP_DateTime * oldest,
                           XMP_DateTime * newest,
                           WXMP_Result *  wResult );

extern void
XMP_PUBLIC WXMPUtils_GetMergedListPath_1 ( XMPMetaRef    multiXMP,
                                XMP_StringPtr propNS,
                                XMP_StringPtr propName,
                                void *        pathStr,
                                SetClientStringProc SetClientString,
                                WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_RemoveMultiValueInfo_1 ( XMPMetaRef    multiXMP,
                                   XMP_StringPtr propNS,
                                   XMP_StringPtr propName,
                                   WXMP_Result * wResult );

#endif	// AdobePrivate

#if AdobePrivate

// -------------------------------------------------------------------------------------------------


typedef void * (*XMPDMO_AllocClientMarkersProc) (void * clientPtr, XMP_Index count);
typedef void * (* XMPDMO_AllocClientCuePointsProc) ( XMPDMO_MarkerInfo * clientMarker, XMP_Index count );
typedef void (* XMPDMO_GetCuePointParamsProc) ( const XMPDMO_MarkerInfo * marker,
												const XMPDMO_CuePointInfo ** cpInfo,
												XMP_Index * cpCount );

extern void
XMP_PUBLIC WXMPUtils_GetBulkMarkers_1 ( XMPMetaRef    wXMP,
							 XMP_StringPtr ns,
							 XMP_StringPtr path,
							 void *        clientPtr,
							 XMP_Uns32     clientElemSize,
							 XMPDMO_AllocClientMarkersProc AllocClientMarkers,
							 XMPDMO_AllocClientCuePointsProc  AllocClientCuePoints,
							 WXMP_Result * wResult );

extern void
XMP_PUBLIC WXMPUtils_SetBulkMarkers_1 ( XMPMetaRef    wXMP,
							 XMP_StringPtr ns,
							 XMP_StringPtr path,
							 XMP_Index     count,
							 const void *  clientArray,
							 XMP_Uns32     clientElemSize,
							 XMPDMO_GetCuePointParamsProc GetCuePointParams,
							 WXMP_Result * wResult );


#endif	// AdobePrivate

// =================================================================================================

#if __cplusplus
} /* extern "C" */
#endif

#endif  // __WXMPUtils_hpp__
