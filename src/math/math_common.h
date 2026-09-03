#ifndef MATH_COMMON_H
#define MATH_COMMON_H

#include <assert.h>
#include <math.h>
#include <stdlib.h>

/*
 * Constants
 */

// PI defines
#define MATH_PI       3.14159265358979323846f
#define MATH_PI_2     6.28318530717958647692f
#define MATH_PI_DIV_2 1.57079632679489661923f
#define MATH_PI_DIV_4 0.78539816339744830962f
#define MATH_PI_INV   0.31830988618379067154f

// Fixed point mathematics constants
#define FIXP16_SHIFT    16
#define FIXP16_MAG      65536
#define FIXP16_DP_MASK  0x0000ffff
#define FIXP16_ROUND_UP 0x00008000

/*
 * Helpers
 */

// clang-format off
#define SWAP(a, b, t) { t=a; a=b; b=t; }
// clang-format on

static inline f32 math_clamp_f32(f32 value, f32 minimum, f32 maximum)
{
    assert(isfinite(value));
    assert(isfinite(minimum));
    assert(isfinite(maximum));
    assert(minimum <= maximum);

    if (value < minimum)
    {
        return minimum;
    }

    if (value > maximum)
    {
        return maximum;
    }

    return value;
}

static inline f32 math_lerp_f32(f32 from, f32 to, f32 amount)
{
    assert(isfinite(from));
    assert(isfinite(to));
    assert(isfinite(amount));

    return from + ((to - from) * amount);
}

static inline f32 math_degree_to_radians(f32 degrees)
{
    assert(isfinite(degrees));

    return degrees * (MATH_PI / 180.0f);
}

static inline f32 math_radians_to_degree(f32 radians)
{
    assert(isfinite(radians));

    return radians * (180.0f / MATH_PI);
}

static inline i32 math_rand_range(int x, int y)
{
    return (x + rand() % (y - x + 1)); // NOLINT(cert-msc30-c, cert-msc50-cpp)
}

/*
 * Fixed-point helpers
 */

static inline fixp16 math_i32_to_fixp16(i32 value)
{
    assert(value >= INT16_MIN && value <= INT16_MAX);

    return (fixp16)((u32)value << FIXP16_SHIFT);
}

static inline fixp16 math_f32_to_fixp16(f32 value)
{
    assert(isfinite(value));
    assert(value >= (f32)INT16_MIN && value <= (f32)INT16_MAX);

    return (fixp16)(lroundf(value * (f32)FIXP16_MAG));
}

static inline f32 math_fixp16_to_f32(fixp16 value)
{
    return (f32)value / (f32)FIXP16_MAG;
}

static inline i32 math_fixp16_int_part(fixp16 value)
{
    return value >> FIXP16_SHIFT; // arithmetic shift preserves the sign
}

static inline fixp16 math_fixp16_frac_part(fixp16 value)
{
    return value & FIXP16_DP_MASK;
}

static inline fixp16 math_fixp16_abs(fixp16 value)
{
    assert(value != INT32_MIN); // -INT32_MIN is not representable in fixp16

    return value < 0 ? -value : value;
}

static inline fixp16 math_fixp16_min(fixp16 a, fixp16 b)
{
    return a < b ? a : b;
}

static inline fixp16 math_fixp16_max(fixp16 a, fixp16 b)
{
    return a > b ? a : b;
}

static inline fixp16 math_fixp16_clamp(fixp16 value, fixp16 minimum, fixp16 maximum)
{
    assert(minimum <= maximum);

    if (value < minimum)
    {
        return minimum;
    }

    if (value > maximum)
    {
        return maximum;
    }

    return value;
}

static inline fixp16 math_fixp16_mul(fixp16 a, fixp16 b)
{
    i64 product = (i64)a * (i64)b;

    // Round to nearest (ties up) instead of truncating toward -inf.
    return (fixp16)((product + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
}

bool math_fixp16_try_mul(fixp16 a, fixp16 b, fixp16* result);

static inline fixp16 math_fixp16_lerp(fixp16 from, fixp16 to, fixp16 amount)
{
    return from + math_fixp16_mul(to - from, amount);
}

/**
 * Shared rounding-division core: computes numerator / denominator in a
 * 64-bit intermediate, rounded to nearest (ties away from zero), instead
 * of the truncation-toward-zero that plain integer division gives.
 *
 * FIXP16_ROUND_UP doesn't apply here: it's a bias sized for the fixed
 * >> FIXP16_SHIFT step used by mul, not for division by an arbitrary
 * runtime denominator.
 */
static inline i64 math_fixp16_rounded_div_i64(i64 numerator, i64 denominator)
{
    assert(denominator != 0);

    i64 quotient = numerator / denominator;
    i64 remainder = numerator % denominator;

    i64 abs_remainder = remainder < 0 ? -remainder : remainder;
    i64 abs_denominator = denominator < 0 ? -denominator : denominator;

    if (abs_remainder * 2 >= abs_denominator)
    {
        quotient += ((numerator < 0) == (denominator < 0)) ? 1 : -1;
    }

    return quotient;
}

static inline i64 math_fixp16_div_i64(fixp16 a, fixp16 b)
{
    return math_fixp16_rounded_div_i64((i64)a << FIXP16_SHIFT, (i64)b);
}

static inline fixp16 math_fixp16_div(fixp16 a, fixp16 b)
{
    return (fixp16)math_fixp16_div_i64(a, b);
}

bool math_fixp16_try_div(fixp16 a, fixp16 b, fixp16* result);

/**
 * Computes (a * b) / c in a single 64-bit intermediate instead of
 * chaining mul() then div(), which would round twice.
 *
 * Useful for perspective-correct interpolation.
 */
static inline fixp16 math_fixp16_muldiv(fixp16 a, fixp16 b, fixp16 c)
{
    return (fixp16)math_fixp16_rounded_div_i64((i64)a * (i64)b, (i64)c);
}

/*
 * Fast trigonometry
 */

/**
 * Fast sin()/cos() backed by a 1-degree-resolution lookup table with
 * linear interpolation between entries.
 *
 * Angles are in degrees, unlike the rest of the math library (which
 * works in radians), because the table is indexed by integer degree.
 *
 * math_build_sin_cos_tables() must be called once (e.g. during
 * platform/app startup) before the first call to math_fast_sin(),
 * math_fast_cos(), or math_fast_sincos(). The tables have a single
 * instance shared by every translation unit; they are defined in
 * math_common.c.
 */
void math_build_sin_cos_tables(void);

f32 math_fast_sin(f32 angle_degree);

f32 math_fast_cos(f32 angle_degree);

/**
 * Computes sin and cos of the same angle in one call.
 *
 * Prefer this over calling math_fast_sin() and math_fast_cos()
 * separately when both are needed (e.g. building a rotation): it
 * reduces the angle and looks up the table index only once instead
 * of twice.
 */
void math_fast_sincos(f32 angle_degree, f32* out_sin, f32* out_cos);

#endif // MATH_COMMON_H
