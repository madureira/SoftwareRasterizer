#ifndef MATH_VEC2F_H
#define MATH_VEC2F_H

#include "math/math_common.h"
#include "math/math_config.h"

typedef struct Vec2f
{
    f32 x;
    f32 y;
} Vec2f;

/**
 * Construction
 */

// clang-format off
#define vec2f(x_, y_) (Vec2f){.x = (x_), .y = (y_)}
// clang-format on

static inline Vec2f vec2f_zero(void)
{
    return vec2f(0.0f, 0.0f);
}

static inline Vec2f vec2f_one(void)
{
    return vec2f(1.0f, 1.0f);
}

/**
 * Helpers
 */

static inline bool vec2f_is_finite(Vec2f vector)
{
    return isfinite(vector.x) && isfinite(vector.y);
}

/**
 * Arithmetic
 */

static inline Vec2f vec2f_add(Vec2f a, Vec2f b)
{
    return vec2f(a.x + b.x, a.y + b.y);
}

static inline Vec2f vec2f_sub(Vec2f a, Vec2f b)
{
    return vec2f(a.x - b.x, a.y - b.y);
}

static inline Vec2f vec2f_scale(Vec2f vector, f32 scalar)
{
    return vec2f(vector.x * scalar, vector.y * scalar);
}

static inline Vec2f vec2f_rotate_sincos(Vec2f vector, f32 sine, f32 cosine)
{
    assert(vec2f_is_finite(vector));
    assert(isfinite(sine));
    assert(isfinite(cosine));

    return vec2f((vector.x * cosine) - (vector.y * sine), (vector.x * sine) + (vector.y * cosine));
}

/**
 * Divides a vector by a scalar.
 *
 * Preconditions:
 * - vector must have finite components;
 * - scalar must be finite and non-zero.
 *
 * Use vec2f_try_div() for untrusted or dynamically computed values.
 */
static inline Vec2f vec2f_div(Vec2f vector, f32 scalar)
{
    assert(vec2f_is_finite(vector));
    assert(isfinite(scalar));
    assert(scalar != 0.0f);

    const f32 inverse = 1.0f / scalar;

    return vec2f_scale(vector, inverse);
}

static inline Vec2f vec2f_neg(Vec2f vector)
{
    return vec2f(-vector.x, -vector.y);
}

/**
 * Vector operations
 */

static inline f32 vec2f_dot(Vec2f a, Vec2f b)
{
    return (a.x * b.x) + (a.y * b.y);
}

static inline f32 vec2f_cross(Vec2f a, Vec2f b)
{
    assert(vec2f_is_finite(a));
    assert(vec2f_is_finite(b));

    const f32 result = (a.x * b.y) - (a.y * b.x);

    assert(isfinite(result));

    return result;
}

static inline f32 vec2f_len_sq(Vec2f vector)
{
    return vec2f_dot(vector, vector);
}

static inline f32 vec2f_dist_sq(Vec2f a, Vec2f b)
{
    return vec2f_len_sq(vec2f_sub(b, a));
}

/**
 * Reflects a vector off a surface with the given normal.
 *
 * Preconditions:
 * - vector and normal must have finite components;
 * - normal must be a unit vector.
 */
static inline Vec2f vec2f_reflect(Vec2f vector, Vec2f normal)
{
    assert(vec2f_is_finite(vector));
    assert(vec2f_is_finite(normal));
    assert(fabsf(vec2f_len_sq(normal) - 1.0f) <= MATH_COMPARISON_EPSILON);

    return vec2f_sub(vector, vec2f_scale(normal, 2.0f * vec2f_dot(vector, normal)));
}

/**
 * Performs an unclamped linear interpolation.
 *
 * amount = 0 returns from.
 * amount = 1 returns to.
 * Values outside [0, 1] extrapolate.
 */
static inline Vec2f vec2f_lerp(Vec2f from, Vec2f to, f32 amount)
{
    assert(vec2f_is_finite(from));
    assert(vec2f_is_finite(to));
    assert(isfinite(amount));

    return vec2f(math_lerp_f32(from.x, to.x, amount), math_lerp_f32(from.y, to.y, amount));
}

static inline Vec2f vec2f_lerp_clamp(Vec2f from, Vec2f to, f32 amount)
{
    return vec2f_lerp(from, to, math_clamp_f32(amount, 0.0f, 1.0f));
}

/**
 * In-place operations
 */

static inline void vec2f_add_inplace(Vec2f* vector, Vec2f value)
{
    assert(vector != NULL);

    vector->x += value.x;
    vector->y += value.y;
}

static inline void vec2f_sub_inplace(Vec2f* vector, Vec2f value)
{
    assert(vector != NULL);

    vector->x -= value.x;
    vector->y -= value.y;
}

static inline void vec2f_scale_inplace(Vec2f* vector, f32 scalar)
{
    assert(vector != NULL);

    vector->x *= scalar;
    vector->y *= scalar;
}

/**
 * Divides a vector by a scalar in place.
 *
 * Preconditions:
 * - vector must have finite components;
 * - scalar must be finite and non-zero.
 *
 * Use vec2f_try_div() for untrusted or dynamically computed values.
 */
static inline void vec2f_div_inplace(Vec2f* vector, f32 scalar)
{
    assert(vector != NULL);
    assert(vec2f_is_finite(*vector));
    assert(isfinite(scalar));
    assert(scalar != 0.0f);

    const f32 inverse = 1.0f / scalar;

    vector->x *= inverse;
    vector->y *= inverse;
}

static inline void vec2f_neg_inplace(Vec2f* vector)
{
    assert(vector != NULL);

    vector->x = -vector->x;
    vector->y = -vector->y;
}

/**
 * Comparison
 */

static inline bool vec2f_eq(Vec2f a, Vec2f b)
{
    return a.x == b.x && a.y == b.y;
}

static inline bool vec2f_near(Vec2f a, Vec2f b, f32 distance)
{
    assert(vec2f_is_finite(a));
    assert(vec2f_is_finite(b));
    assert(isfinite(distance));
    assert(distance >= 0.0f);

    const f32 maximum_distance_squared = distance * distance;

    assert(isfinite(maximum_distance_squared));

    const f32 actual_distance_squared = vec2f_dist_sq(a, b);

    assert(isfinite(actual_distance_squared));

    return actual_distance_squared <= maximum_distance_squared;
}

/**
 * Returns true when the squared distance between a and b is less than
 * or equal to the provided maximum squared distance.
 */
static inline bool vec2f_near_sq(Vec2f a, Vec2f b, f32 maximum_distance_squared)
{
    assert(vec2f_is_finite(a));
    assert(vec2f_is_finite(b));
    assert(isfinite(maximum_distance_squared));
    assert(maximum_distance_squared >= 0.0f);

    const f32 actual_distance_squared = vec2f_dist_sq(a, b);

    assert(isfinite(actual_distance_squared));

    return actual_distance_squared <= maximum_distance_squared;
}

static inline f32 vec2f_len(Vec2f vector)
{
    assert(vec2f_is_finite(vector));

    const f32 length_squared = vec2f_len_sq(vector);

    assert(isfinite(length_squared));

    return sqrtf(length_squared);
}

static inline f32 vec2f_dist(Vec2f a, Vec2f b)
{
    assert(vec2f_is_finite(a));
    assert(vec2f_is_finite(b));

    const f32 distance_squared = vec2f_dist_sq(a, b);

    assert(isfinite(distance_squared));

    return sqrtf(distance_squared);
}

/**
 * Returns true when the absolute difference of each corresponding
 * component is less than or equal to epsilon.
 */
static inline bool vec2f_eq_eps(Vec2f a, Vec2f b, f32 epsilon)
{
    assert(vec2f_is_finite(a));
    assert(vec2f_is_finite(b));
    assert(isfinite(epsilon));
    assert(epsilon >= 0.0f);

    return (fabsf(a.x - b.x) <= epsilon && fabsf(a.y - b.y) <= epsilon);
}

/**
 * Operations implemented in vec2f.c
 */

/**
 * Attempts to divide a vector by a scalar.
 *
 * Returns false when result is NULL, the vector or scalar is not
 * finite, or the scalar is too close to zero.
 *
 * On failure, the value pointed to by result is not modified.
 */
bool vec2f_try_div(Vec2f vector, f32 scalar, Vec2f* result);

Vec2f vec2f_normalize(Vec2f vector);

/**
 * Attempts to normalize a vector.
 *
 * Returns false when result is NULL, the vector is not finite,
 * or the vector is too close to zero to normalize safely.
 *
 * On failure, the value pointed to by result is not modified.
 */
bool vec2f_try_normalize(Vec2f vector, Vec2f* result);

Vec2f vec2f_rotate(Vec2f vector, f32 angle_radians);

void vec2f_rotate_inplace(Vec2f* vector, f32 angle_radians);

Vec2f vec2f_rotate_around(Vec2f vector, Vec2f pivot, f32 angle_radians);

void vec2f_rotate_around_inplace(Vec2f* vector, Vec2f pivot, f32 angle_radians);

#endif // MATH_VEC2F_H
