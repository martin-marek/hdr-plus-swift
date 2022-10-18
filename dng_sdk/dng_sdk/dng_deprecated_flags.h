/*****************************************************************************/

#ifndef __dng_deprecated_flags__
#define __dng_deprecated_flags__

/*****************************************************************************/

/// \def qDNGDepthSupport
/// 1 to add support for depth maps in DNG format.
/// Deprecated 2018-09-19.

#ifdef __cplusplus
#define qDNGDepthSupport #error
#endif

/*****************************************************************************/

/// \def qDNGPreserveBlackPoint
/// 1 to add support for non-zero black point in early raw pipeline.
/// Deprecated 2018-09-19.

#ifdef __cplusplus
#define qDNGPreserveBlackPoint #error
#endif

/// \def qDNGEdgeWrap
/// 1 to add support for "edge wrap" options for dng_image::Get.
/// Deprecated 2019-11-1

#ifdef __cplusplus
#define qDNGEdgeWrap (1)
#endif

/*****************************************************************************/

#endif	// __dng_deprecated_flags__

/*****************************************************************************/
