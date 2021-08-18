/*****************************************************************************/
// Copyright 2017-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#ifndef __dng_simd_type__
#define __dng_simd_type__

/*****************************************************************************/

#include "dng_flags.h"

/*****************************************************************************/

#if qDNGIntelCompiler
#include <immintrin.h>
#endif	// qDNGIntelCompiler

/*****************************************************************************/

enum SIMDType
	{

	Scalar,

	SSE2,       // Pentium 4
	AVX,        // Sandy Bridge
	AVX2,       // Haswell
	F16C = AVX2, //Ivy bridge
	AVX512_SKX, // Sky Lake Server

	SIMD_Sentinel

	};

/*****************************************************************************/

template <int SIMDType>
class SIMDTraits
{
public:
	static const int kVecSizeFloat = 1;
	static const int kVecSizeInt32 = 1;
};

template <>
class SIMDTraits<SSE2>
{
public:
	static const int kVecSizeFloat = 4;
	static const int kVecSizeInt32 = 4;
};

template <>
class SIMDTraits<AVX>
{
public:
	static const int kVecSizeFloat = 8;
	static const int kVecSizeInt32 = 4;
};

template <>
class SIMDTraits<AVX2>
{
public:
	static const int kVecSizeFloat = 8;
	static const int kVecSizeInt32 = 8;
};

template <>
class SIMDTraits<AVX512_SKX>
{
public:
	static const int kVecSizeFloat = 16;
	static const int kVecSizeInt32 = 16;
};

const SIMDType SIMDTypeMaxValue = SIMDType(SIMD_Sentinel - 1);

extern SIMDType	gDNGMaxSIMD;

/*****************************************************************************/

#if qDNGIntelCompiler

// Intel compiler.

// Macros are preferred for "#pragma simd" because at some point these will
// all change to OpenMP 4.x compliant "#pragma omp simd" directives (no longer
// Intel-specific). 
//
// Note that _Pragma(x) requires C99 or C++11 support.

// Pre-defined feature levels.

#define CR_SIMD_MIN_FEATURE   (_FEATURE_SSE2)
#define CR_AVX_FEATURE        (_FEATURE_AVX)
#define CR_AVX2_FEATURE       (_FEATURE_AVX|_FEATURE_FMA|_FEATURE_AVX2)
#define CR_F16C_FEATURE			CR_AVX2_FEATURE
#define CR_AVX512_SKX_FEATURE (_FEATURE_AVX512F|_FEATURE_AVX512CD|_FEATURE_AVX512BW|_FEATURE_AVX512DQ|_FEATURE_AVX512VL)
#define CR_COMPILER_USING_AVX512_SKX (__AVX512F__ && __AVX512VL__ && __AVX512BW__ && __AVX512DQ__ && __AVX512CD__)

#define __SIMDTYPE_TFY(x) #x
#define _SIMDTYPE_TFY(x) __SIMDTYPE_TFY(x)

#if qDNGDebug

// Debug build.

//#define INTEL_PRAGMA_SIMD_ASSERT_C(clause) _Pragma(PM2__STR1__(simd clause))
#define INTEL_PRAGMA_SIMD_ASSERT _Pragma("simd")
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_FLOAT(s) _Pragma(_SIMDTYPE_TFY(simd vectorlength( SIMDTraits<s>::kVecSizeFloat ) ))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT32(s) _Pragma(_SIMDTYPE_TFY(simd vectorlength( SIMDTraits<s>::kVecSizeInt32 ) ))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT16(s) _Pragma(_SIMDTYPE_TFY(simd vectorlength( SIMDTraits<s>::kVecSizeInt32*2 ) ))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT8(s) _Pragma(_SIMDTYPE_TFY(simd vectorlength( SIMDTraits<s>::kVecSizeInt32*4 ) ))

#else

// Release build.

//#define INTEL_PRAGMA_SIMD_ASSERT_C(clause) _Pragma(PM2__STR1__(simd assert clause))
#define INTEL_PRAGMA_SIMD_ASSERT _Pragma("simd assert")
#if 1
#if (__INTEL_COMPILER < 1800)
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_FLOAT(s) _Pragma(_SIMDTYPE_TFY(simd assert vectorlength( SIMDTraits<s>::kVecSizeFloat ) ))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT32(s) _Pragma(_SIMDTYPE_TFY(simd assert vectorlength( SIMDTraits<s>::kVecSizeInt32 ) ))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT16(s) _Pragma(_SIMDTYPE_TFY(simd assert vectorlength( SIMDTraits<s>::kVecSizeInt32*2 ) ))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT8(s) _Pragma(_SIMDTYPE_TFY(simd assert vectorlength( SIMDTraits<s>::kVecSizeInt32*4 ) ))
#else
// FIX_ME_ERIC_CHAN: I removed the assert to fix compile time error when using Intel compiler version 18.
// Need to figure out correct fix when we switch to newer version. - tknoll 10/30/2017.
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_FLOAT(s) _Pragma(_SIMDTYPE_TFY(simd vectorlength( SIMDTraits<s>::kVecSizeFloat ) ))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT32(s) _Pragma(_SIMDTYPE_TFY(simd vectorlength( SIMDTraits<s>::kVecSizeInt32 ) ))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT16(s) _Pragma(_SIMDTYPE_TFY(simd vectorlength( SIMDTraits<s>::kVecSizeInt32*2 ) ))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT8(s) _Pragma(_SIMDTYPE_TFY(simd vectorlength( SIMDTraits<s>::kVecSizeInt32*4 ) ))
#endif
#else
// Don't force
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_FLOAT(s) _Pragma(_SIMDTYPE_TFY(simd assert))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT32(s) _Pragma(_SIMDTYPE_TFY(simd assert))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT16(s) _Pragma(_SIMDTYPE_TFY(simd assert))
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT8(s) _Pragma(_SIMDTYPE_TFY(simd assert))
#endif
#endif

#define SET_CPU_FEATURE(simd) _allow_cpu_features( (simd >= AVX512_SKX) ? CR_AVX512_SKX_FEATURE : (simd >= AVX2) ? CR_AVX2_FEATURE : ((simd >= AVX) ? CR_AVX_FEATURE : CR_SIMD_MIN_FEATURE) )
//#define SET_CPU_FEATURE_NOFMA(simd) _allow_cpu_features( ((simd >= AVX512_SKX) ? CR_AVX512_SKX_FEATURE : (simd >= AVX2) ? CR_AVX2_FEATURE : ((simd >= AVX) ? CR_AVX_FEATURE : CR_SIMD_MIN_FEATURE)) & ~_FEATURE_FMA )
#define SET_CPU_FEATURE_NOFMA(simd) _allow_cpu_features( (simd >= AVX) ? CR_AVX_FEATURE : CR_SIMD_MIN_FEATURE)
#define INTEL_PRAGMA_NOVECTOR _Pragma("novector")
#define INTEL_COMPILER_NEEDED_NOTE

#else

// Non-Intel compiler. Use empty definitions for the macros.
// Credit: http://www.highprogrammer.com/alan/windev/visualstudio.html, but avoid using $ character
#define Stringize( L )			#L
#define MakeString( M, L )		M(L)
#define _x_Line					MakeString( Stringize, __LINE__ )

#if qDNGValidateTarget
// Do not warn about Intel compiler if building dng_validate.
#define INTEL_COMPILER_NEEDED_NOTE
#else
#if !(defined (IOS_ENV) || defined(ANDROID_ENV)) && (defined(__x86_64__) || defined(__i386__))
#ifndef _MSC_VER
#define INTEL_COMPILER_NEEDED_NOTE _Pragma("message(\"NOTE: Intel Compiler needed for optimizations in \" __FILE__ \":\" _x_Line )")
#else
// Intel compiler understands C99 _Pragma, but not Microsoft, so use MS-specific __pragma instead
#define INTEL_COMPILER_NEEDED_NOTE __pragma(message("NOTE: Intel Compiler needed for optimizations in " __FILE__ ":" _x_Line " in " __FUNCTION__))
#endif
#else
#define INTEL_COMPILER_NEEDED_NOTE
#endif
#endif

#define INTEL_PRAGMA_SIMD_ASSERT
//#define INTEL_PRAGMA_SIMD_ASSERT_C(clause)
#define SET_CPU_FEATURE(simd)
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_FLOAT(simd)
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT16(simd)
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT32(simd)
#define INTEL_PRAGMA_SIMD_ASSERT_VECLEN_INT8(simd)
#define INTEL_PRAGMA_NOVECTOR

#endif	// qDNGIntelCompiler

/*****************************************************************************/

#endif	// __dng_simd_type__

/*****************************************************************************/
