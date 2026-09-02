#ifndef MATH_COMMON_H
#define MATH_COMMON_H

#include <assert.h>
#include <math.h>

/*
 * Constants
 */

#define MATH_PI 3.14159265358979323846f

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

/*
 * Scalar helpers
 */

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

#endif // MATH_COMMON_H
