#ifndef __WXMPFiles_hpp__
#define __WXMPFiles_hpp__	1

// =================================================================================================
// Copyright Adobe
// Copyright 2002 Adobe
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
//	AWL Alan Lillich
//	ADC Amandeep Chawla
//	IJS Inder Jeet Singh
//
// mm-dd-yy who Description of changes, most recent on top
//
// 05-13-13 ADC 5.6-f060 Removing usage of bool in APIs exposed at DLL boundaries.
// 01-30-13 IJS 5.6-f029 Removed LocateMetadataFiles API from XMPFiles
// 01-07-13 IJS 5.6-f018 Changed name of API IsWritable to IsMetadataWritable.
//                       Added root Path folder as associated Resource for folder based Formats.
// 12-21-12 IJS 5.6-f008 Added GetAssociatedResources and IsWritable APIs 
//
// 09-28-12 AWL 5.5-f039 Add LocateMetadataFiles.
// 09-28-12 ADC 5.5-f038 Implement public API and glue layer for XMPFiles error notifications.
// 06-22-12 AWL 5.5-f019 Add file update progress tracking to the MPEG-4 handler.
// 06-20-12 AWL 5.5-f018 Add outer layers for XMPFiles progress notifications, no handlers use it yet.
//
// 10-27-11 AWL 5.4-f030 [3007461] Improve performance of GetFileModDate.
// 09-23-11 AWL 5.4-f012 Add GetFileModDate.
//
// 08-17-10 AWL 5.3-f001 Integrate I/O revamp to main.
//
// 05-21-09 AWL 5.0-f040 Revamp glue again to pass SetClientString with each function.
// 05-19-09 AWL 5.0-f039 First part of threading improvements, revamp the client glue.
//
// 01-28-08 AWL 4.2-f062 Add public CheckFileFormat and CheckPackageFormat functions.
// 08-24-07 AWL 4.2-c019 Remove support for ancient CXMP_* init/term routines and non-public (AXE) expat.
// 01-11-07 AWL 4.2-f007 [1454747] Change QuickTime init/term calls to handle main and background thread issues.
//
// 04-07-06 AWL 4.0-f002 Add XMPFiles::GetThumbnail. Change XMPFiles::OpenFile to close the disk file
//				early for read-only access. Change XMPFileHandler::ExtractXMP to CacheFileData.
// 03-24-06 AWL 4.0 Adapt for move to ham-perforce, integrate XMPFiles, bump version to 4.
//
// 07-21-05 AWL 1.3-001 Remove BIB, same as XMP. Create Xcode project. Bump version to 1.3.
//
// 10-07-04 AWL 1.0-008 Add missing SetAbortProc in WXMPFiles.cpp and export lists.
// 09-23-04 AWL 1.0-007 Use the XMP toolkit's build number and timestamp. Add current position
//				result to LFA_Seek. Add SetAbortProc and abort checking.
// 08-20-04 AWL Add handlerFlags output to GetFileInfo.
// 08-13-04 AWL Checkpoint, almost full implementation, compiles.
// 06-25-04 AWL First draft.
//
// =================================================================================================
#endif // AdobePrivate

#include "client-glue/WXMP_Common.hpp"

#if XMP_StaticBuild	// ! Client XMP_IO objects can only be used in static builds.
	#include "XMP_IO.hpp"
#endif

#if __cplusplus
extern "C" {
#endif

// =================================================================================================
/// \file WXMPFiles.hpp
/// \brief High level support to access metadata in files of interest to Adobe applications.
///
/// This header ...
///
// =================================================================================================

// =================================================================================================

#define WrapCheckXMPFilesRef(result,WCallProto) \
    WXMP_Result wResult;                        \
    WCallProto;                                 \
    PropagateException ( wResult );             \
    XMPFilesRef result = XMPFilesRef(wResult.ptrResult)

static XMP_Bool WrapProgressReport ( XMP_ProgressReportProc proc, void * context,
								 float elapsedTime, float fractionDone, float secondsToGo )
{
	bool ok;
	try {
		ok = (*proc) ( context, elapsedTime, fractionDone, secondsToGo );
	} catch ( ... ) {
		ok = false;
	}
	return ConvertBoolToXMP_Bool( ok );
}

// =================================================================================================

static XMP_Bool WrapFilesErrorNotify ( XMPFiles_ErrorCallbackProc proc, void * context,
	XMP_StringPtr filePath, XMP_ErrorSeverity severity, XMP_Int32 cause, XMP_StringPtr message )
{
	bool ok;
	try {
		ok = (*proc) ( context, filePath, severity, cause, message );
	} catch ( ... ) {
		ok = false;
	}
	return ConvertBoolToXMP_Bool( ok );
}

// =================================================================================================

#define zXMPFiles_GetVersionInfo_1(versionInfo) \
	WXMPFiles_GetVersionInfo_1 ( versionInfo /* no wResult */ )

#define zXMPFiles_Initialize_1(options) \
	WXMPFiles_Initialize_1 ( options, &wResult )

#define zXMPFiles_Initialize_2(options,pluginFolder,plugins) \
	WXMPFiles_Initialize_2 ( options, pluginFolder, plugins, &wResult )

#define zXMPFiles_Terminate_1() \
	WXMPFiles_Terminate_1 ( /* no wResult */ )

#define zXMPFiles_CTor_1() \
	WXMPFiles_CTor_1 ( &wResult )

#define zXMPFiles_GetFormatInfo_1(format,flags) \
	WXMPFiles_GetFormatInfo_1 ( format, flags, &wResult )

#define zXMPFiles_CheckFileFormat_1(filePath) \
	WXMPFiles_CheckFileFormat_1 ( filePath, &wResult )

#define zXMPFiles_CheckPackageFormat_1(folderPath) \
	WXMPFiles_CheckPackageFormat_1 ( folderPath, &wResult )

#define zXMPFiles_GetFileModDate_1(filePath,modDate,format,options) \
	WXMPFiles_GetFileModDate_1 ( filePath, modDate, format, options, &wResult )

#define zXMPFiles_GetAssociatedResources_1( filePath, resourceList, format, options, SetClientStringVector ) \
	WXMPFiles_GetAssociatedResources_1 ( filePath, resourceList, format, options, SetClientStringVector, &wResult )

#define zXMPFiles_IsMetadataWritable_1( filePath, writable, format, options ) \
	WXMPFiles_IsMetadataWritable_1 ( filePath, writable, format, options, &wResult )

#define zXMPFiles_OpenFile_1(filePath,format,openFlags) \
	WXMPFiles_OpenFile_1 ( this->xmpFilesRef, filePath, format, openFlags, &wResult )

#if AdobePrivate
#define zXMPFiles_GetAlbumArts_1( albumArtVector, PushArtInfo1 ) \
	WXMPFiles_GetAlbumArts_1 ( this->xmpFilesRef, albumArtVector, PushArtInfo1, &wResult )

#define zXMPFiles_PutAlbumArts_1( albumArtVector, albumArtCount, GetArtInfo1 ) \
	WXMPFiles_PutAlbumArts_1 ( this->xmpFilesRef, albumArtVector, albumArtCount, GetArtInfo1, &wResult )
#endif

#if XMP_StaticBuild	// ! Client XMP_IO objects can only be used in static builds.
#define zXMPFiles_OpenFile_2(clientIO,format,openFlags) \
	WXMPFiles_OpenFile_2 ( this->xmpFilesRef, clientIO, format, openFlags, &wResult )
#endif

#define zXMPFiles_CloseFile_1(closeFlags) \
	WXMPFiles_CloseFile_1 ( this->xmpFilesRef, closeFlags, &wResult )

#define zXMPFiles_GetFileInfo_1(clientPath,openFlags,format,handlerFlags,SetClientString) \
	WXMPFiles_GetFileInfo_1 ( this->xmpFilesRef, clientPath, openFlags, format, handlerFlags, SetClientString, &wResult )

#define zXMPFiles_SetAbortProc_1(abortProc,abortArg) \
	WXMPFiles_SetAbortProc_1 ( this->xmpFilesRef, abortProc, abortArg, &wResult )

#define zXMPFiles_GetXMP_1(xmpRef,clientPacket,packetInfo,SetClientString) \
	WXMPFiles_GetXMP_1 ( this->xmpFilesRef, xmpRef, clientPacket, packetInfo, SetClientString, &wResult )

#define zXMPFiles_PutXMP_1(xmpRef,xmpPacket,xmpPacketLen) \
	WXMPFiles_PutXMP_1 ( this->xmpFilesRef, xmpRef, xmpPacket, xmpPacketLen, &wResult )

#define zXMPFiles_CanPutXMP_1(xmpRef,xmpPacket,xmpPacketLen) \
	WXMPFiles_CanPutXMP_1 ( this->xmpFilesRef, xmpRef, xmpPacket, xmpPacketLen, &wResult )

#define zXMPFiles_SetDefaultProgressCallback_1(proc,context,interval,sendStartStop) \
	WXMPFiles_SetDefaultProgressCallback_1 ( WrapProgressReport, proc, context, interval, sendStartStop, &wResult )

#define zXMPFiles_SetProgressCallback_1(proc,context,interval,sendStartStop) \
	WXMPFiles_SetProgressCallback_1 ( this->xmpFilesRef, WrapProgressReport, proc, context, interval, sendStartStop, &wResult )

#define zXMPFiles_SetDefaultErrorCallback_1(proc,context,limit) \
	WXMPFiles_SetDefaultErrorCallback_1 ( WrapFilesErrorNotify, proc, context, limit, &wResult )

#define zXMPFiles_SetErrorCallback_1(proc,context,limit) \
	WXMPFiles_SetErrorCallback_1 ( this->xmpFilesRef, WrapFilesErrorNotify, proc, context, limit, &wResult )

#define zXMPFiles_ResetErrorCallbackLimit_1(limit) \
	WXMPFiles_ResetErrorCallbackLimit_1 ( this->xmpFilesRef, limit, &wResult )

// =================================================================================================

extern void WXMPFiles_GetVersionInfo_1 ( XMP_VersionInfo * versionInfo );

extern void WXMPFiles_Initialize_1 ( XMP_OptionBits      options,
                                     WXMP_Result *       result );

extern void WXMPFiles_Initialize_2 ( XMP_OptionBits      options,
									 const char*         pluginFolder,
									 const char*         plugins,
									 WXMP_Result *       result );

extern void WXMPFiles_Terminate_1();

extern void WXMPFiles_CTor_1 ( WXMP_Result * result );

extern void WXMPFiles_IncrementRefCount_1 ( XMPFilesRef xmpFilesRef );

extern void WXMPFiles_DecrementRefCount_1 ( XMPFilesRef xmpFilesRef );

extern void WXMPFiles_GetFormatInfo_1 ( XMP_FileFormat   format,
                                        XMP_OptionBits * flags,	// ! Can be null.
                                        WXMP_Result *    result );

extern void WXMPFiles_CheckFileFormat_1 ( XMP_StringPtr filePath,
                               			  WXMP_Result * result );

extern void WXMPFiles_CheckPackageFormat_1 ( XMP_StringPtr folderPath,
                      						 WXMP_Result * result );

extern void WXMPFiles_GetFileModDate_1 ( XMP_StringPtr    filePath,
                                         XMP_DateTime *   modDate,
					                     XMP_FileFormat * format,	// ! Can be null.
					                     XMP_OptionBits   options,
                      					 WXMP_Result *    result );


extern void WXMPFiles_GetAssociatedResources_1 ( XMP_StringPtr             filePath,
												 void *                    resourceList,
												 XMP_FileFormat            format, 
												 XMP_OptionBits            options, 
					                             SetClientStringVectorProc SetClientStringVector,
												 WXMP_Result *             result );

extern void WXMPFiles_IsMetadataWritable_1 ( XMP_StringPtr    filePath,
									         XMP_Bool *       writable, 
									         XMP_FileFormat   format,
									         XMP_OptionBits   options, 
									         WXMP_Result *    result );

extern void WXMPFiles_OpenFile_1 ( XMPFilesRef    xmpFilesRef,
                                   XMP_StringPtr  filePath,
					               XMP_FileFormat format,
					               XMP_OptionBits openFlags,
                                   WXMP_Result *  result );

#if AdobePrivate
extern void WXMPFiles_GetAlbumArts_1( XMPFilesRef		xmpFilesRef, 
									  void *			clientVecAlbumArt,
									  PushArtInfoProc1	pushArtOnClientVecProc,
									  WXMP_Result *		result );

extern void WXMPFiles_PutAlbumArts_1( XMPFilesRef		xmpFilesRef,
									  void *			clientVecAlbumArt,
									  XMP_Uns32			albumArtCount,
									  GetArtInfoProc1	getArtInfoFromClientVecProc,
									  WXMP_Result *		result );
#endif

#if XMP_StaticBuild	// ! Client XMP_IO objects can only be used in static builds.
extern void WXMPFiles_OpenFile_2 ( XMPFilesRef    xmpFilesRef,
                                   XMP_IO *       clientIO,
					               XMP_FileFormat format,
					               XMP_OptionBits openFlags,
                                   WXMP_Result *  result );
#endif

extern void WXMPFiles_CloseFile_1 ( XMPFilesRef    xmpFilesRef,
                                    XMP_OptionBits closeFlags,
                                    WXMP_Result *  result );

extern void WXMPFiles_GetFileInfo_1 ( XMPFilesRef      xmpFilesRef,
                                      void *           clientPath,
					                  XMP_OptionBits * openFlags,		// ! Can be null.
					                  XMP_FileFormat * format,		// ! Can be null.
					                  XMP_OptionBits * handlerFlags,	// ! Can be null.
					                  SetClientStringProc SetClientString,
                                      WXMP_Result *    result );

extern void WXMPFiles_SetAbortProc_1 ( XMPFilesRef   xmpFilesRef,
                                       XMP_AbortProc abortProc,
									   void *        abortArg,
                                       WXMP_Result * result );

extern void WXMPFiles_GetXMP_1 ( XMPFilesRef      xmpFilesRef,
                                 XMPMetaRef       xmpRef,		// ! Can be null.
    			                 void *           clientPacket,
    			                 XMP_PacketInfo * packetInfo,	// ! Can be null.
    			                 SetClientStringProc SetClientString,
                                 WXMP_Result *    result );

extern void WXMPFiles_PutXMP_1 ( XMPFilesRef   xmpFilesRef,
                                 XMPMetaRef    xmpRef,		// ! Only one of the XMP object or packet are passed.
                                 XMP_StringPtr xmpPacket,
                                 XMP_StringLen xmpPacketLen,
                                 WXMP_Result * result );

extern void WXMPFiles_CanPutXMP_1 ( XMPFilesRef   xmpFilesRef,
                                    XMPMetaRef	  xmpRef,		// ! Only one of the XMP object or packet are passed.
                                    XMP_StringPtr xmpPacket,
                                    XMP_StringLen xmpPacketLen,
                                    WXMP_Result * result );

extern void WXMPFiles_SetDefaultProgressCallback_1 ( XMP_ProgressReportWrapper wrapperproc,
													 XMP_ProgressReportProc    clientProc,
													 void *        context,
													 float         interval,
													 XMP_Bool      sendStartStop,
													 WXMP_Result * result );

extern void WXMPFiles_SetProgressCallback_1 ( XMPFilesRef   xmpFilesRef,
											  XMP_ProgressReportWrapper wrapperproc,
											  XMP_ProgressReportProc    clientProc,
											  void *        context,
											  float         interval,
											  XMP_Bool      sendStartStop,
											  WXMP_Result * result );

// -------------------------------------------------------------------------------------------------

extern void WXMPFiles_SetDefaultErrorCallback_1 ( XMPFiles_ErrorCallbackWrapper	wrapperProc,
												  XMPFiles_ErrorCallbackProc	clientProc,
												  void *						context,
												  XMP_Uns32						limit,
												  WXMP_Result *					wResult );

extern void WXMPFiles_SetErrorCallback_1 ( XMPFilesRef						xmpRef,
										   XMPFiles_ErrorCallbackWrapper	wrapperProc,
										   XMPFiles_ErrorCallbackProc		clientProc,
										   void *							context,
										   XMP_Uns32						limit,
										   WXMP_Result *					wResult );

extern void WXMPFiles_ResetErrorCallbackLimit_1 ( XMPFilesRef				xmpRef,
												  XMP_Uns32					limit,
												  WXMP_Result *				wResult );

// =================================================================================================

#if __cplusplus
}
#endif

#endif // __WXMPFiles_hpp__
