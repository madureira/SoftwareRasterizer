#include "math/vec3f.h"

bool vec3f_try_div(Vec3f vector, f32 scalar, Vec3f* result)
{
    if (result == NULL || !vec3f_is_finite(vector) || !isfinite(scalar)
        || fabsf(scalar) <= MATH_DIVISION_EPSILON)
    {
        return false;
    }

    const f32 inverse = 1.0f / scalar;

    *result = vec3f_scale(vector, inverse);

    return true;
}

Vec3f vec3f_normalize(Vec3f vector)
{
    assert(vec3f_is_finite(vector));

    const f32 length_squared = vec3f_len_sq(vector);

    assert(isfinite(length_squared));

    if (length_squared <= MATH_NORMALIZE_EPSILON_SQUARED)
    {
        return vec3f_zero();
    }

    const f32 inverse_length = 1.0f / sqrtf(length_squared);

    return vec3f_scale(vector, inverse_length);
}

bool vec3f_try_normalize(Vec3f vector, Vec3f* result)
{
    if (result == NULL || !vec3f_is_finite(vector))
    {
        return false;
    }

    const f32 length_squared = vec3f_len_sq(vector);

    if (length_squared <= MATH_NORMALIZE_EPSILON_SQUARED)
    {
        return false;
    }

    *result = vec3f_scale(vector, 1.0f / sqrtf(length_squared));

    return true;
}

Vec3f vec3f_rotate_sincos(Vec3f vector, Vec3f axis, f32 sine, f32 cosine)
{
    assert(vec3f_is_finite(vector));
    assert(vec3f_is_finite(axis));
    assert(isfinite(sine));
    assert(isfinite(cosine));

    axis = vec3f_normalize(axis);

    const Vec3f cross = vec3f_cross(axis, vector);
    const f32 dot = vec3f_dot(axis, vector);

    return vec3f_add(vec3f_add(vec3f_scale(vector, cosine), vec3f_scale(cross, sine)),
                     vec3f_scale(axis, dot * (1.0f - cosine)));
}

Vec3f vec3f_rotate(Vec3f vector, Vec3f axis, f32 angle_radians)
{
    assert(vec3f_is_finite(vector));
    assert(vec3f_is_finite(axis));
    assert(isfinite(angle_radians));

    const f32 sine = sinf(angle_radians);
    const f32 cosine = cosf(angle_radians);

    return vec3f_rotate_sincos(vector, axis, sine, cosine);
}

void vec3f_rotate_inplace(Vec3f* vector, Vec3f axis, f32 angle_radians)
{
    assert(vector != NULL);
    assert(vec3f_is_finite(*vector));
    assert(vec3f_is_finite(axis));
    assert(isfinite(angle_radians));

    const f32 sine = sinf(angle_radians);
    const f32 cosine = cosf(angle_radians);

    *vector = vec3f_rotate_sincos(*vector, axis, sine, cosine);
}

Vec3f vec3f_rotate_around(Vec3f vector, Vec3f pivot, Vec3f axis, f32 angle_radians)
{
    assert(vec3f_is_finite(vector));
    assert(vec3f_is_finite(pivot));
    assert(vec3f_is_finite(axis));
    assert(isfinite(angle_radians));

    const Vec3f translated = vec3f_sub(vector, pivot);
    const Vec3f rotated = vec3f_rotate(translated, axis, angle_radians);

    return vec3f_add(rotated, pivot);
}

void vec3f_rotate_around_inplace(Vec3f* vector, Vec3f pivot, Vec3f axis, f32 angle_radians)
{
    assert(vector != NULL);

    *vector = vec3f_rotate_around(*vector, pivot, axis, angle_radians);
}
