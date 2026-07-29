#include "math/vec2f.h"

#include "math/math_config.h"

bool vec2f_try_div(Vec2f vector, float32 scalar, Vec2f* result)
{
    // clang-format off
    if (
        result == NULL
        || !vec2f_is_finite(vector)
        || !isfinite(scalar)
        || fabsf(scalar) <= MATH_DIVISION_EPSILON)
    {
        return false;
    }
    // clang-format on

    const float32 inverse = 1.0f / scalar;

    *result = vec2f_scale(vector, inverse);

    return true;
}

Vec2f vec2f_normalize(Vec2f vector)
{
    assert(vec2f_is_finite(vector));

    const float32 length_squared = vec2f_len_sq(vector);

    assert(isfinite(length_squared));

    // Avoid normalizing zero or extremely small vectors.
    if (length_squared <= MATH_NORMALIZE_EPSILON_SQUARED)
    {
        return vec2f_zero();
    }

    const float32 inverse_length = 1.0f / sqrtf(length_squared);

    return vec2f_scale(vector, inverse_length);
}

bool vec2f_try_normalize(Vec2f vector, Vec2f* result)
{
    if (result == NULL || !vec2f_is_finite(vector))
    {
        return false;
    }

    const float32 length_squared = vec2f_len_sq(vector);

    if (length_squared <= MATH_NORMALIZE_EPSILON_SQUARED)
    {
        return false;
    }

    *result = vec2f_scale(vector, 1.0f / sqrtf(length_squared));

    return true;
}

Vec2f vec2f_rotate(Vec2f vector, float32 angle_radians)
{
    assert(vec2f_is_finite(vector));
    assert(isfinite(angle_radians));

    const float32 sine = sinf(angle_radians);
    const float32 cosine = cosf(angle_radians);

    return vec2f_rotate_sincos(vector, sine, cosine);
}

void vec2f_rotate_inplace(Vec2f* vector, float32 angle_radians)
{
    assert(vector != NULL);
    assert(vec2f_is_finite(*vector));
    assert(isfinite(angle_radians));

    const float32 sine = sinf(angle_radians);
    const float32 cosine = cosf(angle_radians);

    const float32 original_x = vector->x;
    const float32 original_y = vector->y;

    vector->x = (original_x * cosine) - (original_y * sine);
    vector->y = (original_x * sine) + (original_y * cosine);
}

Vec2f vec2f_rotate_around(Vec2f vector, Vec2f pivot, float32 angle_radians)
{
    assert(vec2f_is_finite(vector));
    assert(vec2f_is_finite(pivot));
    assert(isfinite(angle_radians));

    const Vec2f translated = vec2f_sub(vector, pivot);
    const Vec2f rotated = vec2f_rotate(translated, angle_radians);

    return vec2f_add(rotated, pivot);
}
