#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

typedef unsigned char uchar;

/*
 * Compile-time type-size validation
 */

// clang-format off
typedef char core_float32_must_be_32_bits[
    (sizeof(float32) * CHAR_BIT == 32) ? 1 : -1
];

typedef char core_float64_must_be_64_bits[
    (sizeof(float64) * CHAR_BIT == 64) ? 1 : -1
];
// clang-format on

#endif // CORE_TYPES_H
