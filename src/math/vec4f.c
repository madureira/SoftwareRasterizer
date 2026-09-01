#include "math/vec4f.h"

bool vec4f_try_div(Vec4f vector, f32 scalar, Vec4f* result)
{
    if (result == NULL)
    {
        return false;
    }

    if (!vec4f_is_finite(vector) || !isfinite(scalar))
    {
        return false;
    }

    if (fabsf(scalar) <= MATH_DIVISION_EPSILON)
    {
        return false;
    }

    const f32 inverse = 1.0f / scalar;

    const Vec4f value = vec4f_scale(vector, inverse);

    if (!vec4f_is_finite(value))
    {
        return false;
    }

    *result = value;

    return true;
}

Vec4f vec4f_normalize(Vec4f vector)
{
    assert(vec4f_is_finite(vector));

    const f32 length_squared = vec4f_len_sq(vector);

    assert(isfinite(length_squared));
    assert(length_squared > MATH_NORMALIZE_EPSILON_SQUARED);

    const f32 inverse_length = 1.0f / sqrtf(length_squared);

    const Vec4f result = vec4f_scale(vector, inverse_length);

    assert(vec4f_is_finite(result));

    return result;
}

bool vec4f_try_normalize(Vec4f vector, Vec4f* result)
{
    if (result == NULL)
    {
        return false;
    }

    if (!vec4f_is_finite(vector))
    {
        return false;
    }

    const f32 length_squared = vec4f_len_sq(vector);

    if (!isfinite(length_squared))
    {
        return false;
    }

    if (length_squared <= MATH_NORMALIZE_EPSILON_SQUARED)
    {
        return false;
    }

    const f32 inverse_length = 1.0f / sqrtf(length_squared);

    const Vec4f normalized = vec4f_scale(vector, inverse_length);

    if (!vec4f_is_finite(normalized))
    {
        return false;
    }

    *result = normalized;

    return true;
}

bool vec4f_try_perspective_divide(Vec4f vector, Vec4f* result)
{
    if (result == NULL)
    {
        return false;
    }

    if (!vec4f_is_finite(vector))
    {
        return false;
    }

    if (fabsf(vector.w) <= MATH_DIVISION_EPSILON)
    {
        return false;
    }

    const f32 inverse_w = 1.0f / vector.w;

    // clang-format off
    const Vec4f divide = vec4f(
        vector.x * inverse_w,
        vector.y * inverse_w,
        vector.z * inverse_w,
        1.0f
    );
    // clang-format on

    if (!vec4f_is_finite(divide))
    {
        return false;
    }

    *result = divide;

    return true;
}
