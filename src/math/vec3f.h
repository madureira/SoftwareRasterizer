#ifndef MATH_VEC3F_H
#define MATH_VEC3F_H

#include "math/math_common.h"
#include "math/math_config.h"
#include "math/vec2f.h"

typedef struct Vec3f
{
    f32 x;
    f32 y;
    f32 z;
} Vec3f;

/**
 * Construction
 */

// clang-format off
#define vec3f(x_, y_, z_) (Vec3f){.x = (x_), .y = (y_), .z = (z_)}
// clang-format on

static inline Vec3f vec3f_zero(void)
{
    return vec3f(0.0f, 0.0f, 0.0f);
}

static inline Vec3f vec3f_one(void)
{
    return vec3f(1.0f, 1.0f, 1.0f);
}

/**
 * Helpers
 */

static inline bool vec3f_is_finite(Vec3f vector)
{
    return isfinite(vector.x) && isfinite(vector.y) && isfinite(vector.z);
}

/**
 * Arithmetic
 */

static inline Vec3f vec3f_add(Vec3f a, Vec3f b)
{
    return vec3f(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline Vec3f vec3f_sub(Vec3f a, Vec3f b)
{
    return vec3f(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline Vec3f vec3f_scale(Vec3f vector, f32 scalar)
{
    return vec3f(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

/**
 * Divides a vector by a scalar.
 *
 * Preconditions:
 * - vector must have finite components;
 * - scalar must be finite and non-zero;
 *
 * Use vec3f_try_div() for untrusted or dynamically computed values.
 */
static inline Vec3f vec3f_div(Vec3f vector, f32 scalar)
{
    assert(vec3f_is_finite(vector));
    assert(isfinite(scalar));
    assert(scalar != 0.0f);

    const f32 inverse = 1.0f / scalar;

    return vec3f_scale(vector, inverse);
}

static inline Vec3f vec3f_neg(Vec3f vector)
{
    return vec3f(-vector.x, -vector.y, -vector.z);
}

/**
 * Vector operations
 */

static inline f32 vec3f_dot(Vec3f a, Vec3f b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static inline Vec3f vec3f_cross(Vec3f a, Vec3f b)
{
    assert(vec3f_is_finite(a));
    assert(vec3f_is_finite(b));

    const Vec3f cross = vec3f(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);

    assert(vec3f_is_finite(cross));

    return cross;
}

static inline f32 vec3f_len_sq(Vec3f vector)
{
    return vec3f_dot(vector, vector);
}

static inline f32 vec3f_dist_sq(Vec3f a, Vec3f b)
{
    return vec3f_len_sq(vec3f_sub(b, a));
}

/**
 * Reflects a vector off a surface with the given normal.
 *
 * Preconditions:
 * - vector and normal must have finite components;
 * - normal must be a unit vector.
 */
static inline Vec3f vec3f_reflect(Vec3f vector, Vec3f normal)
{
    assert(vec3f_is_finite(vector));
    assert(vec3f_is_finite(normal));
    assert(fabsf(vec3f_len_sq(normal) - 1.0f) <= MATH_COMPARISON_EPSILON);

    return vec3f_sub(vector, vec3f_scale(normal, 2.0f * vec3f_dot(vector, normal)));
}

/**
 * Performs an unclamped linear interpolation.
 *
 * amount = 0 returns from.
 * amount = 1 returns to.
 * Values outside [0, 1] extrapolate.
 */
static inline Vec3f vec3f_lerp(Vec3f from, Vec3f to, f32 amount)
{
    assert(vec3f_is_finite(from));
    assert(vec3f_is_finite(to));
    assert(isfinite(amount));

    return vec3f(math_lerp_f32(from.x, to.x, amount), math_lerp_f32(from.y, to.y, amount),
                 math_lerp_f32(from.z, to.z, amount));
}

static inline Vec3f vec3f_lerp_clamp(Vec3f from, Vec3f to, f32 amount)
{
    return vec3f_lerp(from, to, math_clamp_f32(amount, 0.0f, 1.0f));
}

/**
 * In-place operations
 */

static inline void vec3f_add_inplace(Vec3f* vector, Vec3f value)
{
    assert(vector != NULL);

    vector->x += value.x;
    vector->y += value.y;
    vector->z += value.z;
}

static inline void vec3f_sub_inplace(Vec3f* vector, Vec3f value)
{
    assert(vector != NULL);

    vector->x -= value.x;
    vector->y -= value.y;
    vector->z -= value.z;
}

static inline void vec3f_scale_inplace(Vec3f* vector, f32 scalar)
{
    assert(vector != NULL);

    vector->x *= scalar;
    vector->y *= scalar;
    vector->z *= scalar;
}

/**
 * Divides a vector by a scalar in place.
 *
 * Preconditions:
 * - vector must have finite components;
 * - scalar must be finite and non-zero;
 *
 * Use vec3f_try_div() for untrusted or dynamically computed values.
 */
static inline void vec3f_div_inplace(Vec3f* vector, f32 scalar)
{
    assert(vector != NULL);
    assert(vec3f_is_finite(*vector));
    assert(isfinite(scalar));
    assert(scalar != 0.0f);

    const f32 inverse = 1.0f / scalar;

    vector->x *= inverse;
    vector->y *= inverse;
    vector->z *= inverse;
}

static inline void vec3f_neg_inplace(Vec3f* vector)
{
    assert(vector != NULL);

    vector->x = -vector->x;
    vector->y = -vector->y;
    vector->z = -vector->z;
}

/**
 * Comparison
 */
static inline bool vec3f_eq(Vec3f a, Vec3f b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static inline bool vec3f_near(Vec3f a, Vec3f b, f32 distance)
{
    assert(vec3f_is_finite(a));
    assert(vec3f_is_finite(b));
    assert(isfinite(distance));
    assert(distance >= 0.0f);

    const f32 maximum_distance_squared = distance * distance;

    assert(isfinite(maximum_distance_squared));

    const f32 actual_distance_squared = vec3f_dist_sq(a, b);

    assert(isfinite(actual_distance_squared));

    return actual_distance_squared <= maximum_distance_squared;
}

/**
 * Returns true when the squared distance between a and b is less than
 * or equal to the provided maximum squared distance.
 */
static inline bool vec3f_near_sq(Vec3f a, Vec3f b, f32 maximum_distance_squared)
{
    assert(vec3f_is_finite(a));
    assert(vec3f_is_finite(b));
    assert(isfinite(maximum_distance_squared));
    assert(maximum_distance_squared >= 0.0f);

    const f32 actual_distance_squared = vec3f_dist_sq(a, b);

    assert(isfinite(actual_distance_squared));

    return actual_distance_squared <= maximum_distance_squared;
}

static inline f32 vec3f_len(Vec3f vector)
{
    assert(vec3f_is_finite(vector));

    const f32 length_squared = vec3f_len_sq(vector);

    assert(isfinite(length_squared));

    return sqrtf(length_squared);
}

static inline f32 vec3f_dist(Vec3f a, Vec3f b)
{
    assert(vec3f_is_finite(a));
    assert(vec3f_is_finite(b));

    const f32 distance_squared = vec3f_dist_sq(a, b);

    assert(isfinite(distance_squared));

    return sqrtf(distance_squared);
}

/**
 * Returns true when the absolute difference of each corresponding
 * component is less than or equal to epsilon.
 */
static inline bool vec3f_eq_eps(Vec3f a, Vec3f b, f32 epsilon)
{
    assert(vec3f_is_finite(a));
    assert(vec3f_is_finite(b));
    assert(isfinite(epsilon));
    assert(epsilon >= 0.0f);

    // clang-format off
    return fabsf(a.x - b.x) <= epsilon
        && fabsf(a.y - b.y) <= epsilon
        && fabsf(a.z - b.z) <= epsilon;
    // clang-format on
}

/**
 * Conversions
 */

static inline Vec3f vec3f_from_vec2f(Vec2f xy, f32 z)
{
    assert(vec2f_is_finite(xy));
    assert(isfinite(z));

    return vec3f(xy.x, xy.y, z);
}

static inline Vec2f vec3f_xy(Vec3f vector)
{
    assert(vec3f_is_finite(vector));

    return vec2f(vector.x, vector.y);
}

/**
 * Operations implemented in vec3f.c
 */

/**
 * Attempts to divide a vector by a scalar.
 *
 * Returns false when result is NULL, the vector or scalar is not finite,
 * or the scalar is too close to zero.
 *
 * On failure, the value pointed to by result is not modified.
 */
bool vec3f_try_div(Vec3f vector, f32 scalar, Vec3f* result);

Vec3f vec3f_normalize(Vec3f vector);

/**
 * Attempts to normalize a vector.
 *
 * Returns false when result is NULL, the vector is not finite,
 * or the vector is too close to zero to normalize safely.
 *
 * On failure, the value pointed to by result is not modified.
 */
bool vec3f_try_normalize(Vec3f vector, Vec3f* result);

/**
 * Rotates a vector around an axis through the origin, given a precomputed
 * sine and cosine of the rotation angle.
 *
 * The axis does not need to be normalized; it is normalized internally.
 */
Vec3f vec3f_rotate_sincos(Vec3f vector, Vec3f axis, f32 sine, f32 cosine);

/**
 * Rotates a vector around an axis through the origin.
 */
Vec3f vec3f_rotate(Vec3f vector, Vec3f axis, f32 angle_radians);

void vec3f_rotate_inplace(Vec3f* vector, Vec3f axis, f32 angle_radians);

/**
 * Rotates a vector around an axis passing through the given pivot point.
 */
Vec3f vec3f_rotate_around(Vec3f vector, Vec3f pivot, Vec3f axis, f32 angle_radians);

void vec3f_rotate_around_inplace(Vec3f* vector, Vec3f pivot, Vec3f axis, f32 angle_radians);

#endif // MATH_VEC3F_H
