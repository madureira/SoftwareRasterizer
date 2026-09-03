#ifndef MATH_VEC4F_H
#define MATH_VEC4F_H

#include "math/math_common.h"
#include "math/math_config.h"
#include "math/vec3f.h"

typedef struct Vec4f
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vec4f;

/*
 * Construction
 */

// clang-format off
#define vec4f(x_, y_, z_, w_) (Vec4f){.x = (x_), .y = (y_), .z = (z_), .w = (w_)}
// clang-format on

static inline Vec4f vec4f_zero(void)
{
    return vec4f(0.0f, 0.0f, 0.0f, 0.0f);
}

static inline Vec4f vec4f_one(void)
{
    return vec4f(1.0f, 1.0f, 1.0f, 1.0f);
}

/*
 * Homogeneous coordinates
 */

/**
 * Creates a homogeneous point
 *
 * The resulting vector has w = 1
 */
static inline Vec4f vec4f_point(f32 x, f32 y, f32 z)
{
    return vec4f(x, y, z, 1.0f);
}

/**
 * Creates a homogeneous vector
 *
 * The resulting vector has w = 0
 */
static inline Vec4f vec4f_vector(f32 x, f32 y, f32 z)
{
    return vec4f(x, y, z, 0.0f);
}

/*
 * Helpers
 */

static inline bool vec4f_is_finite(Vec4f vector)
{
    return isfinite(vector.x) && isfinite(vector.y) && isfinite(vector.z) && isfinite(vector.w);
}

/*
 * Arithmetic
 */

static inline Vec4f vec4f_add(Vec4f a, Vec4f b)
{
    return vec4f(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static inline void vec4f_add_inplace(Vec4f* vector, Vec4f value)
{
    assert(vector != NULL);

    vector->x += value.x;
    vector->y += value.y;
    vector->z += value.z;
    vector->w += value.w;
}

static inline Vec4f vec4f_sub(Vec4f a, Vec4f b)
{
    return vec4f(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static inline void vec4f_sub_inplace(Vec4f* vector, Vec4f value)
{
    assert(vector != NULL);

    vector->x -= value.x;
    vector->y -= value.y;
    vector->z -= value.z;
    vector->w -= value.w;
}

static inline Vec4f vec4f_scale(Vec4f vector, f32 scalar)
{
    return vec4f(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar);
}

static inline void vec4f_scale_inplace(Vec4f* vector, f32 scalar)
{
    assert(vector != NULL);

    vector->x *= scalar;
    vector->y *= scalar;
    vector->z *= scalar;
    vector->w *= scalar;
}

/**
 * Divides a vector by a scalar.
 *
 * Preconditions:
 * - vector must have finite components;
 * - scalar must be finite and non-zero;
 *
 * Use vec4f_try_div() for untrusted or dynamically computed values.
 */
static inline Vec4f vec4f_div(Vec4f vector, f32 scalar)
{
    assert(vec4f_is_finite(vector));
    assert(isfinite(scalar));
    assert(scalar != 0.0f);

    const f32 inverse = 1.0f / scalar;

    return vec4f_scale(vector, inverse);
}

/**
 * Divides a vector by a scalar inplace.
 *
 * Preconditions:
 * - vector must have finite components;
 * - scalar must be finite and non-zero;
 *
 * Use vec4f_try_div() for untrusted or dynamically computed values.
 */
static inline void vec4f_div_inplace(Vec4f* vector, f32 scalar)
{
    assert(vector != NULL);
    assert(vec4f_is_finite(*vector));
    assert(isfinite(scalar));
    assert(scalar != 0.0f);

    const f32 inverse = 1.0f / scalar;

    vector->x *= inverse;
    vector->y *= inverse;
    vector->z *= inverse;
    vector->w *= inverse;
}

static inline Vec4f vec4f_neg(Vec4f vector)
{
    return vec4f(-vector.x, -vector.y, -vector.z, -vector.w);
}

static inline void vec4f_neg_inplace(Vec4f* vector)
{
    assert(vector != NULL);

    vector->x = -vector->x;
    vector->y = -vector->y;
    vector->z = -vector->z;
    vector->w = -vector->w;
}

/*
 * Vector operations
 */

static inline f32 vec4f_dot(Vec4f a, Vec4f b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

static inline f32 vec4f_len_sq(Vec4f vector)
{
    return vec4f_dot(vector, vector);
}

static inline f32 vec4f_dist_sq(Vec4f a, Vec4f b)
{
    return vec4f_len_sq(vec4f_sub(b, a));
}

/**
 * Performs an unclamped linear interpolation.
 *
 * amount = 0 returns from.
 * amount = 1 returns to.
 * Values outside [0, 1] extrapolate.
 */
static inline Vec4f vec4f_lerp(Vec4f from, Vec4f to, f32 amount)
{
    assert(vec4f_is_finite(from));
    assert(vec4f_is_finite(to));
    assert(isfinite(amount));

    // clang-format off
    return vec4f(
        math_lerp_f32(from.x, to.x, amount),
        math_lerp_f32(from.y, to.y, amount),
        math_lerp_f32(from.z, to.z, amount),
        math_lerp_f32(from.w, to.w, amount)
    );
    // clang-format on
}

static inline Vec4f vec4f_lerp_clamp(Vec4f from, Vec4f to, f32 amount)
{
    return vec4f_lerp(from, to, math_clamp_f32(amount, 0.0f, 1.0f));
}

/*
 * Homogeneous operations
 */

/**
 * Returns true when the vector represents a point.
 *
 * A homogeneous point has w = 1
 */
static inline bool vec4f_is_point(Vec4f vector)
{
    assert(vec4f_is_finite(vector));

    return fabsf(vector.w - 1.0f) <= MATH_COMPARISON_EPSILON;
}

/**
 * Returns true when the vector represents a direction vector.
 *
 * A homogeneous vector has w = 0
 */
static inline bool vec4f_is_vector(Vec4f vector)
{
    assert(vec4f_is_finite(vector));

    return fabsf(vector.w) <= MATH_COMPARISON_EPSILON;
}

/*
 * Comparison
 */

static inline bool vec4f_eq(Vec4f a, Vec4f b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

/**
 * Returns true when the absolute difference of each corresponding
 * component is less than or equal to epsilon.
 */
static inline bool vec4f_eq_eps(Vec4f a, Vec4f b, f32 epsilon)
{
    assert(vec4f_is_finite(a));
    assert(vec4f_is_finite(b));
    assert(isfinite(epsilon));
    assert(epsilon >= 0.0f);

    // clang-format off
    return fabsf(a.x - b.x) <= epsilon
        && fabsf(a.y - b.y) <= epsilon
        && fabsf(a.z - b.z) <= epsilon
        && fabsf(a.w - b.w) <= epsilon;
    // clang-format on
}

static inline bool vec4f_near(Vec4f a, Vec4f b, f32 distance)
{
    assert(vec4f_is_finite(a));
    assert(vec4f_is_finite(b));
    assert(isfinite(distance));
    assert(distance >= 0.0f);

    const f32 maximum_distance_squared = distance * distance;

    assert(isfinite(maximum_distance_squared));

    const f32 actual_distance_squared = vec4f_dist_sq(a, b);

    assert(isfinite(actual_distance_squared));

    return actual_distance_squared <= maximum_distance_squared;
}

/**
 * Returns true when the squared distance between a and b is less than
 * or equal to the provided maximum squared distance.
 */
static inline bool vec4f_near_sq(Vec4f a, Vec4f b, f32 maximum_distance_squared)
{
    assert(vec4f_is_finite(a));
    assert(vec4f_is_finite(b));
    assert(isfinite(maximum_distance_squared));
    assert(maximum_distance_squared >= 0.0f);

    const f32 actual_distance_squared = vec4f_dist_sq(a, b);

    assert(isfinite(actual_distance_squared));

    return actual_distance_squared <= maximum_distance_squared;
}

/*
 * Length
 */

static inline f32 vec4f_len(Vec4f vector)
{
    assert(vec4f_is_finite(vector));

    const f32 length_squared = vec4f_len_sq(vector);

    assert(isfinite(length_squared));

    return sqrtf(length_squared);
}

static inline f32 vec4f_dist(Vec4f a, Vec4f b)
{
    assert(vec4f_is_finite(a));
    assert(vec4f_is_finite(b));

    const f32 distance_squared = vec4f_dist_sq(a, b);

    assert(isfinite(distance_squared));

    return sqrtf(distance_squared);
}

/*
 * Conversions
 */

static inline Vec4f vec4f_from_vec3f(Vec3f xyz, f32 w)
{
    assert(vec3f_is_finite(xyz));
    assert(isfinite(w));

    return vec4f(xyz.x, xyz.y, xyz.z, w);
}

static inline Vec3f vec4f_xyz(Vec4f vector)
{
    assert(vec4f_is_finite(vector));

    return vec3f(vector.x, vector.y, vector.z);
}

/*
 * Operations implemented in vec4f.c
 */

/**
 * Attempts to divide a vector by a scalar.
 *
 * Returns false when the result is NULL, the vector or scalar is not finite,
 * or the scalar is too close to zero.
 *
 * On failure, the value pointed to by results is not modified
 */
bool vec4f_try_div(Vec4f vector, f32 scalar, Vec4f* result);

/**
 * Normalizes a four-dimension vector.
 *
 * Unlike vec2f_normalize()/vec3f_normalize(), which silently return a zero
 * vector for near-zero input, this asserts instead. A near-zero Vec4f
 * usually signals a degenerate homogeneous coordinate rather than a
 * meaningful "no direction" case, so failing loudly is intentional here.
 * Use vec4f_try_normalize() when the input may legitimately be near-zero.
 */
Vec4f vec4f_normalize(Vec4f vector);

/**
 * Attempts to normalize a four-dimension vector.
 *
 * Returns false when result is NULL, the vector is not finite,
 * or the vector is too close to zero to normalize safely.
 *
 * On failure, the value pointed to by result is not modified.
 */
bool vec4f_try_normalize(Vec4f vector, Vec4f* result);

/**
 * Performs the homogeneous perspective divide.
 *
 * Converts:
 * (x, y, z, w)
 *
 * into:
 * (x / w, y / w, z / w, 1)
 *
 * Returns false when result is NULL, the vector is not finite,
 * or w is too close to zero.
 */
bool vec4f_try_perspective_divide(Vec4f vector, Vec4f* result);

#endif // MATH_VEC4F_H
