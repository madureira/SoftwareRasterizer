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

    *result = vec2_multiply(vector, inverse);

    return true;
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

bool vec2_try_normalize(Vec2 vector, Vec2* result)
{
    if (result == NULL || !vec2_is_finite(vector))
    {
        return false;
    }

    const float32 length_squared = vec2_length_squared(vector);

    if (length_squared <= MATH_NORMALIZE_EPSILON_SQUARED)
    {
        return false;
    }

    *result = vec2_multiply(vector, 1.0f / sqrtf(length_squared));

    return true;
}
