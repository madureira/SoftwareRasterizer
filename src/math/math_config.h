#ifndef MATH_CONFIG_H
#define MATH_CONFIG_H

#include <float.h>

/*
 * Floating-point compilation requirements
 */

#if defined(__FAST_MATH__)
#error "The math library requires strict floating-point semantics. Do not compile with -ffast-math."
#endif

#if defined(__FINITE_MATH_ONLY__) && (__FINITE_MATH_ONLY__ > 0)
#error "The math library does not support finite-math-only optimizations."
#endif

/*
 * Floating-point representation requirements
 */

#if FLT_RADIX != 2
#error "The math library requires binary floating-point types."
#endif

#if FLT_MANT_DIG != 24 || FLT_MAX_EXP != 128
#error "float32 must use an IEEE-754 binary32-compatible representation."
#endif

#if DBL_MANT_DIG != 53 || DBL_MAX_EXP != 1024
#error "float64 must use an IEEE-754 binary64-compatible representation."
#endif

/*
 * Default numerical tolerances
 */

#ifndef MATH_DIVISION_EPSILON
#define MATH_DIVISION_EPSILON 1.0e-8f
#endif

#ifndef MATH_NORMALIZE_EPSILON
#define MATH_NORMALIZE_EPSILON 1.0e-6f
#endif

// clang-format off
#define MATH_NORMALIZE_EPSILON_SQUARED \
    ((MATH_NORMALIZE_EPSILON) * \
     (MATH_NORMALIZE_EPSILON))

#ifndef MATH_COMPARISON_EPSILON
#define MATH_COMPARISON_EPSILON 1.0e-6f
#endif
// clang-format on

#endif // MATH_CONFIG_H
