#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Fixed-width signed integers.
 */
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/*
 * Fixed-width unsigned integers.
 */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/*
 * Floating-point types.
 *
 * Their expected representation is validated below.
 */
typedef float f32;
typedef double f64;

/*
 * Size and pointer-related integers.
 */
typedef size_t usize;
typedef ptrdiff_t isize;

typedef uintptr_t uptr;
typedef intptr_t iptr;

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

/*
 * C99-compatible compile-time assertion.
 */
#define CORE_JOIN_IMPL(a, b) a##b
#define CORE_JOIN(a, b)      CORE_JOIN_IMPL(a, b)

#define CORE_STATIC_ASSERT(condition, name)                                                        \
    typedef char CORE_JOIN(core_static_assert_, name)[(condition) ? 1 : -1]

/*
 * Floating-point representation requirements.
 */

CORE_STATIC_ASSERT(CHAR_BIT == 8, char_must_have_8_bits);

CORE_STATIC_ASSERT(sizeof(f32) * CHAR_BIT == 32, f32_must_have_32_bits);

CORE_STATIC_ASSERT(FLT_RADIX == 2, f32_must_use_binary_radix);

CORE_STATIC_ASSERT(FLT_MANT_DIG == 24, f32_must_have_24_bit_mantissa);

CORE_STATIC_ASSERT(FLT_MAX_EXP == 128, f32_must_have_expected_max_exponent);

CORE_STATIC_ASSERT(sizeof(f64) * CHAR_BIT == 64, f64_must_have_64_bits);

CORE_STATIC_ASSERT(DBL_MANT_DIG == 53, f64_must_have_53_bit_mantissa);

CORE_STATIC_ASSERT(DBL_MAX_EXP == 1024, f64_must_have_expected_max_exponent);

#endif // CORE_TYPES_H
