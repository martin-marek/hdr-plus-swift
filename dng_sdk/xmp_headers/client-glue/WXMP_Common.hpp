#if ! __WXMP_Common_hpp__
#define __WXMP_Common_hpp__ 1

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
// AWL Alan Lillich
//
// mm-dd-yy who Description of changes, most recent on top.
//
// 09-28-12 AWL 5.5-f039 Add LocateMetadataFiles.
//
// 06-11-09 AWL 5.0-c034-f043 Finish threading revamp, implement friendly reader/writer locking.
// 05-19-09 AWL 5.0-c031-f039 First part of threading improvements, revamp the client glue.
// 05-14-09 AWL 5.0-c030-f038 Improve the call tracing mechanism.
//
// 01-28-08 AWL 4.2-f062 Add public CheckFileFormat and CheckPackageFormat functions.
// 10-31-07 AWL 4.2-c023 Add new class XMPDocOps.
// 08-24-07 AWL 4.2-c019 Remove support for ancient CXMP_* init/term routines and non-public (AXE) expat.
//
// 03-24-06 AWL 4.0 Adapt for move to ham-perforce, integrate XMPFiles, bump version to 4.
//
// 05-16-05 AWL Initial creation.
//
// =================================================================================================
#endif // AdobePrivate

#ifndef XMP_Inline
	#if TXMP_EXPAND_INLINE
		#define XMP_Inline inline
	#else
		#define XMP_Inline /* not inline */
	#endif
#endif

#define XMP_CTorDTorIntro(Class) template <class tStringObj> XMP_Inline Class<tStringObj>
#define XMP_MethodIntro(Class,ResultType) template <class tStringObj> XMP_Inline ResultType Class<tStringObj>

typedef void (* SetClientStringProc) ( void * clientPtr, XMP_StringPtr valuePtr, XMP_StringLen valueLen );
typedef void (* SetClientStringVectorProc) ( void * clientPtr, XMP_StringPtr * arrayPtr, XMP_Uns32 stringCount );

#if AdobePrivate
typedef void (* PushArtInfoProc1 ) ( void * clientPtr, XMP_Uns32 imageLength, XMP_StringPtr description,
				XMP_StringLen descLen, XMP_Uns8 usageType, XMP_Uns8 formatType, XMP_Uns8 encodingType,
				XMP_BinaryData imageData );
typedef void (* GetArtInfoProc1 ) ( void * clientPtr, XMP_Uns32 index, XMP_Uns32 * imageLength,
				XMP_StringPtr * description,	XMP_StringLen * descLen, XMP_Uns8 * usageType,
				XMP_Uns8 * formatType, XMP_Uns8 * encodingType,	XMP_BinaryData * imageData );
#endif

struct WXMP_Result {
    XMP_StringPtr errMessage;
    void *        ptrResult;
    double        floatResult;
    XMP_Uns64     int64Result;
    XMP_Uns32     int32Result;
	WXMP_Result() : errMessage(0),ptrResult(NULL),floatResult(0),int64Result(0),int32Result(0){};
};

#if __cplusplus
extern "C" {
#endif

#define PropagateException(res)	\
	if ( res.errMessage != 0 ) throw XMP_Error ( res.int32Result, res.errMessage );

#ifndef XMP_TraceClientCalls
	#define XMP_TraceClientCalls		0
	#define XMP_TraceClientCallsToFile	0
#endif

#if ! XMP_TraceClientCalls
	#define InvokeCheck(WCallProto) \
    	WXMP_Result wResult;        \
		WCallProto;                 \
		PropagateException ( wResult )
#else
	extern FILE * xmpClientLog;
	#define InvokeCheck(WCallProto)                                                                          \
    	WXMP_Result wResult;                                                                                 \
    	fprintf ( xmpClientLog, "WXMP calling: %s\n", #WCallProto ); fflush ( xmpClientLog );                \
    	WCallProto;                                                                                          \
    	if ( wResult.errMessage == 0 ) {                                                                     \
			fprintf ( xmpClientLog, "WXMP back, no error\n" ); fflush ( xmpClientLog );                      \
    	} else {                                                                                             \
			fprintf ( xmpClientLog, "WXMP back, error: %s\n", wResult.errMessage ); fflush ( xmpClientLog ); \
    	}                                                                                                    \
		PropagateException ( wResult )
#endif

// =================================================================================================

#define WrapNoCheckVoid(WCallProto) \
	WCallProto;

#define WrapCheckVoid(WCallProto) \
    InvokeCheck(WCallProto);

#define WrapCheckMetaRef(result,WCallProto) \
    InvokeCheck(WCallProto);                \
    XMPMetaRef result = XMPMetaRef(wResult.ptrResult)

#define WrapCheckIterRef(result,WCallProto) \
    InvokeCheck(WCallProto);                \
    XMPIteratorRef result = XMPIteratorRef(wResult.ptrResult)

#define WrapCheckDocOpsRef(result,WCallProto) \
    InvokeCheck(WCallProto);                  \
    XMPDocOpsRef result = XMPDocOpsRef(wResult.ptrResult)

#define  WrapCheckNewMetadata(result,WCallProto) \
    InvokeCheck(WCallProto);                  \
    void * result = wResult.ptrResult

#define WrapCheckBool(result,WCallProto) \
    InvokeCheck(WCallProto);             \
    bool result = bool(wResult.int32Result)

#define WrapCheckTriState(result,WCallProto) \
    InvokeCheck(WCallProto);                 \
    XMP_TriState result = XMP_TriState(wResult.int32Result)

#define WrapCheckOptions(result,WCallProto) \
    InvokeCheck(WCallProto);                \
    XMP_OptionBits result = XMP_OptionBits(wResult.int32Result)

#define WrapCheckStatus(result,WCallProto) \
    InvokeCheck(WCallProto);               \
    XMP_Status result = XMP_Status(wResult.int32Result)

#define WrapCheckIndex(result,WCallProto) \
    InvokeCheck(WCallProto);              \
    XMP_Index result = XMP_Index(wResult.int32Result)

#define WrapCheckInt32(result,WCallProto) \
    InvokeCheck(WCallProto);              \
    XMP_Int32 result = wResult.int32Result

#define WrapCheckInt64(result,WCallProto) \
    InvokeCheck(WCallProto);              \
    XMP_Int64 result = wResult.int64Result

#define WrapCheckFloat(result,WCallProto) \
    InvokeCheck(WCallProto);              \
    double result = wResult.floatResult

#define WrapCheckFormat(result,WCallProto) \
    InvokeCheck(WCallProto);               \
    XMP_FileFormat result = wResult.int32Result

// =================================================================================================

#if __cplusplus
}	// extern "C"
#endif

#endif  // __WXMP_Common_hpp__
