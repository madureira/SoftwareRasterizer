#ifndef MATH_COMMON_H
#define MATH_COMMON_H

#include <assert.h>
#include <math.h>

/*
 * Constants
 */

#define MATH_PI 3.14159265358979323846f

/*
 * Scalar helpers
 */

static inline float32 math_clamp_f32(float32 value, float32 minimum, float32 maximum)
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

static inline float32 math_lerp_f32(float32 from, float32 to, float32 amount)
{
    assert(isfinite(from));
    assert(isfinite(to));
    assert(isfinite(amount));

    return from + ((to - from) * amount);
}

static inline float32 math_degree_to_radians(float32 degrees)
{
    assert(isfinite(degrees));

    return degrees * (MATH_PI / 180.0f);
}

static inline float32 math_radians_to_degree(float32 radians)
{
    assert(isfinite(radians));

    return radians * (180.0f / MATH_PI);
}

#endif // MATH_COMMON_H
