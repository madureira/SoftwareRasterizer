#include "vec2.h"

#include "math/math_config.h"

bool vec2_try_divide(Vec2 vector, float32 scalar, Vec2* result)
{
    // clang-format off
    if (
        result == NULL
        || !vec2_is_finite(vector)
        || !isfinite(scalar)
        || fabsf(scalar) <= MATH_DIVISION_EPSILON)
    {
        return false;
    }
    // clang-format on

    const float32 inverse = 1.0f / scalar;
    const Vec2 division_result = vec2_multiply(vector, inverse);

    if (!vec2_is_finite(division_result))
    {
        return false;
    }

    *result = division_result;

    return true;
}

float32 vec2_length(Vec2 vector)
{
    assert(vec2_is_finite(vector));

    const float32 length_squared = vec2_length_squared(vector);

    assert(isfinite(length_squared));

    return sqrtf(length_squared);
}

float32 vec2_distance(Vec2 a, Vec2 b)
{
    assert(vec2_is_finite(a));
    assert(vec2_is_finite(b));

    const float32 distance_squared = vec2_distance_squared(a, b);

    assert(isfinite(distance_squared));

    return sqrtf(distance_squared);
}

Vec2 vec2_normalize(Vec2 vector)
{
    assert(vec2_is_finite(vector));

    const float32 length_squared = vec2_length_squared(vector);

    assert(isfinite(length_squared));

    // Avoid normalizing zero or extremely small vectors.
    if (length_squared <= MATH_NORMALIZE_EPSILON_SQUARED)
    {
        return vec2_zero();
    }

    const float32 inverse_length = 1.0f / sqrtf(length_squared);

    return vec2_multiply(vector, inverse_length);
}

bool vec2_equals_epsilon(Vec2 a, Vec2 b, float32 epsilon)
{
    assert(vec2_is_finite(a));
    assert(vec2_is_finite(b));
    assert(isfinite(epsilon));
    assert(epsilon >= 0.0f);

    // clang-format off
    return (
        fabsf(a.x - b.x) <= epsilon
        && fabsf(a.y - b.y) <= epsilon
    );
    // clang-format on
}
