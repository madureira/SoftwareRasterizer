#ifndef CORE_SIMD_H
#define CORE_SIMD_H

#ifndef ENABLE_SIMD
#define ENABLE_SIMD 0
#endif

#if ENABLE_SIMD && defined(__SSE2__)
#include <emmintrin.h>
#define SIMD_SSE2  1
#define SIMD_NEON  0
#define SIMD_LANES 4
#elif ENABLE_SIMD && defined(__ARM_NEON)
#include <arm_neon.h>
#define SIMD_SSE2  0
#define SIMD_NEON  1
#define SIMD_LANES 4
#else
#define SIMD_SSE2  0
#define SIMD_NEON  0
#define SIMD_LANES 1
#endif

#endif // CORE_SIMD_H
