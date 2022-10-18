/*****************************************************************************/
// Copyright 2006-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:	Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

/** \file
 * Conditional compilation flags for DNG SDK.
 *
 * All conditional compilation macros for the DNG SDK begin with a lowercase 'q'.
 */

/*****************************************************************************/

#ifndef __dng_flags__
#define __dng_flags__

/*****************************************************************************/

/// \def qMacOS 
/// 1 if compiling for Mac OS X.

/// \def qWinOS 
/// 1 if compiling for Windows.

// Make sure a platform is defined

#if !(defined(qMacOS) || defined(qWinOS) || defined(qAndroid) || defined(qiPhone) || defined(qLinux) || defined(qWeb))
#include "RawEnvironment.h"
#endif

// This requires a force include or compiler define.  These are the unique platforms.

#if !(defined(qMacOS) || defined(qWinOS) || defined(qAndroid) || defined(qiPhone) || defined(qLinux) || defined(qWeb))
#error Unable to figure out platform
#endif

/*****************************************************************************/

// Platforms.
// Zeros out any undefined platforms, so that #if can be used in place of #ifdef.

#ifndef qMacOS
#define qMacOS 0
#endif

#ifndef qiPhone
#define qiPhone 0
#endif

#ifndef qiPhoneSimulator
#define qiPhoneSimulator 0
#endif

#ifndef qAndroid
#define qAndroid 0
#endif

#ifndef qWinOS
#define qWinOS 0
#endif

#ifndef qWinRT
#define qWinRT 0
#endif

#ifndef qLinux
#define qLinux 0
#endif

#ifndef qWeb
#define qWeb 0
#endif

/*****************************************************************************/

#if qiPhoneSimulator
#if !qiPhone
#error "qiPhoneSimulator set and not qiPhone"
#endif
#endif

#if qWinRT
#if !qWinOS
#error "qWinRT set but not qWinOS"
#endif
#endif

/*****************************************************************************/

// arm and arm64 support

// arm detect (apple vs. win)
#if defined(__arm__) || defined(__arm64__) || defined(_M_ARM) || defined(_M_ARM64) || defined(__aarch64__)
#define qARM 1
#endif

#if defined(__arm64__) || defined(_M_ARM64) || defined(__aarch64__)
#define qARM64 1
#endif

#ifndef qARM 
#define qARM 0
#endif

#ifndef qARM64 
#define qARM64 0
#endif

/*****************************************************************************/

/// \def qX86_64
/// 1 if and only if this target platform is 64-bit x86 architecture

#if defined(__x86_64__)
#define qX86_64 1
#endif

#ifndef qX86_64
#define qX86_64 0
#endif

/*****************************************************************************/

// Establish WIN32 and WIN64 definitions.

#if defined(_WIN32) && !defined(WIN32)
#define WIN32 1
#endif

#if defined(_WIN64) && !defined(WIN64)
#define WIN64 1
#endif

/*****************************************************************************/

/// \def qDNGDebug 
/// 1 if debug code is compiled in, 0 otherwise. Enables assertions and other debug
/// checks in exchange for slower processing.

// Figure out if debug build or not.

#ifndef qDNGDebug

#if defined(Debug)
#define qDNGDebug Debug

#elif defined(_DEBUG)
#define qDNGDebug _DEBUG

#else
#define qDNGDebug 0

#endif
#endif

/*****************************************************************************/
// Support Intel Thread Building Blocks (TBB)?
// 
// This flag needs to be configured via the project, because there are sources
// outside the cr_sdk (such as the CTJPEG and ACE libs) that need to use the
// same flag to determine whether to use TBB or not.
// 
// By default, configure to 0 (disabled).

#ifndef qCRSupportTBB
#define qCRSupportTBB 0
#endif

#if qCRSupportTBB
#ifndef TBB_DEPRECATED
#define TBB_DEPRECATED 0
#endif
#endif

// This is not really a switch, but rather a shorthand for determining whether
// or not we're building a particular translation unit (source file) using the
// Intel Compiler.

#ifndef qDNGIntelCompiler
#if defined(__INTEL_COMPILER)
#define qDNGIntelCompiler (__INTEL_COMPILER >= 1700)
#else
#define qDNGIntelCompiler 0
#endif
#endif

/*****************************************************************************/

// Figure out byte order.

/// \def qDNGBigEndian 
/// 1 if this target platform is big endian (e.g. PowerPC Macintosh), else 0.
///
/// \def qDNGLittleEndian 
/// 1 if this target platform is little endian (e.g. x86 processors), else 0.

#ifndef qDNGBigEndian

#if defined(qDNGLittleEndian)
#define qDNGBigEndian !qDNGLittleEndian

#elif defined(__POWERPC__)
#define qDNGBigEndian 1

#elif defined(__INTEL__)
#define qDNGBigEndian 0

#elif defined(_M_IX86)
#define qDNGBigEndian 0

#elif defined(_M_X64) || defined(__amd64__)
#define qDNGBigEndian 0

#elif defined(__LITTLE_ENDIAN__)
#define qDNGBigEndian 0

#elif defined(__BIG_ENDIAN__)
#define qDNGBigEndian 1

#elif defined(_ARM_) || defined(__ARM_NEON) || defined(__mips__)
#define qDNGBigEndian 0

#elif defined(_M_ARM64)
// See https://docs.microsoft.com/en-us/cpp/build/arm64-windows-abi-conventions?view=vs-2019
#define qDNGBigEndian 0

#else

#ifndef qXCodeRez
#error Unable to figure out byte order.
#endif

#endif
#endif

#ifndef qXCodeRez

#ifndef qDNGLittleEndian
#define qDNGLittleEndian !qDNGBigEndian
#endif

#endif

/*****************************************************************************/

/// \def qDNG64Bit 
/// 1 if this target platform uses 64-bit addresses, 0 otherwise.

#ifndef qDNG64Bit

#if qMacOS

#ifdef __LP64__
#if	   __LP64__
#define qDNG64Bit 1
#endif
#endif

#elif qWinOS

#ifdef WIN64
#if	   WIN64
#define qDNG64Bit 1
#endif
#endif

#elif qLinux

#ifdef __LP64__
#if	   __LP64__
#define qDNG64Bit 1
#endif
#endif

#elif qAndroid

#ifdef __LP64__
#if	   __LP64__
#define qDNG64Bit 1
#endif
#endif

#endif

#ifndef qDNG64Bit
#ifdef qXCodeRez
#define qDNG64Bit qXCodeRez
#else
#define qDNG64Bit 0
#endif
#endif

#endif

/*****************************************************************************/

#ifdef __cplusplus
#if defined(__clang__)
	#define DNG_RESTRICT __restrict__
	#define DNG_ALWAYS_INLINE __attribute((__always_inline__)) inline
#else
	#define DNG_RESTRICT 
	#define DNG_ALWAYS_INLINE inline
#endif
#endif	// __cplusplus

/*****************************************************************************/

/// \def qDNGThreadSafe 
/// 1 if target platform has thread support and threadsafe libraries, 0 otherwise.

#ifndef qDNGThreadSafe
#define qDNGThreadSafe (qMacOS || qWinOS)
#endif

/*****************************************************************************/

/// \def qDNGValidateTarget 
/// 1 if dng_validate command line tool is being built, 0 otherwise.

#ifndef qDNGValidateTarget
#define qDNGValidateTarget 0
#endif

/*****************************************************************************/

/// \def qDNGValidate 
/// 1 if DNG validation code is enabled, 0 otherwise.

#ifndef qDNGValidate
#define qDNGValidate qDNGValidateTarget
#endif

/*****************************************************************************/

/// \def qDNGPrintMessages 
/// 1 if dng_show_message should use fprintf to stderr. 0 if it should use a platform
/// specific interrupt mechanism.

#ifndef qDNGPrintMessages
#define qDNGPrintMessages qDNGValidate
#endif

/*****************************************************************************/

// Experimental features -- work in progress for Lightroom and Camera Raw
// major releases. Turn this off for Lightroom & Camera Raw dot releases.

#ifndef qDNGExperimental
#define qDNGExperimental 1
#endif

/*****************************************************************************/

/// \def qDNGXMPFiles 
/// 1 to use XMPFiles.

#ifndef qDNGXMPFiles
#define qDNGXMPFiles 1
#endif

/*****************************************************************************/

/// \def qDNGXMPDocOps 
/// 1 to use XMPDocOps.

#ifndef qDNGXMPDocOps
#define qDNGXMPDocOps (!qDNGValidateTarget)
#endif

/*****************************************************************************/

/// \def qDNGUseLibJPEG
/// 1 to use open-source libjpeg for lossy jpeg processing.

#ifndef qDNGUseLibJPEG
#define qDNGUseLibJPEG qDNGValidateTarget
#endif

/*****************************************************************************/

#ifndef qDNGAVXSupport
#define qDNGAVXSupport ((qMacOS || qWinOS) && qDNG64Bit && !qARM && 1)
#endif

#if qDNGAVXSupport && !(qDNG64Bit && !qARM)
#error AVX support is enabled when 64-bit support is not or ARM is
#endif

/*****************************************************************************/

#ifndef qDNGSupportVC5
#define qDNGSupportVC5 (1)
#endif

/*****************************************************************************/

/// \def qDNGUsingSanitizer
/// Set to 1 when using a Sanitizer tool.

#ifndef qDNGUsingSanitizer
#define qDNGUsingSanitizer (0)
#endif

/*****************************************************************************/

#ifndef DNG_ATTRIB_NO_SANITIZE
#if qDNGUsingSanitizer && defined(__clang__)
#define DNG_ATTRIB_NO_SANITIZE(type) __attribute__((no_sanitize(type)))
#else
#define DNG_ATTRIB_NO_SANITIZE(type)
#endif
#endif

/*****************************************************************************/

// Big image support?
//
// When set to true:
// - maximum linear image dimensions is 300000 pixels
// - maximum total number of pixels is 10 gigapixels (10 * 1000 * 1000 * 1000 pixels)
//
// When set to false:
// - maximum linear image dimensions is 65000 pixels
// - maximum total number of pixels is 512 megapixels (512 * 1024 * 1024 pixels)

#ifndef qDNGBigImage
#define qDNGBigImage (qDNGExperimental && 1)
#endif

/*****************************************************************************/

// Enable XMP support in the DNG SDK?

#ifndef qDNGUseXMP
#define qDNGUseXMP 1
#endif

/*****************************************************************************/

// Use custom integral types?

#ifndef qDNGUseCustomIntegralTypes
#define qDNGUseCustomIntegralTypes 0
#endif

/*****************************************************************************/

// Place deprecated flags into this file.

#include "dng_deprecated_flags.h"

/*****************************************************************************/

#endif
	
/*****************************************************************************/
