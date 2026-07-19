#ifndef MATH_VEC2_H
#define MATH_VEC2_H

#include "math/math_common.h"

typedef struct Vec2
{
    float32 x;
    float32 y;
} Vec2;

/**
 * Construction
 */

static inline Vec2 vec2_create(float32 x, float32 y)
{
    // clang-format off
    return (Vec2){
        .x = x,
        .y = y
    };
    // clang-format on
}

static inline Vec2 vec2_zero(void)
{
    return vec2_create(0.0f, 0.0f);
}

static inline Vec2 vec2_one(void)
{
    return vec2_create(1.0f, 1.0f);
}

/**
 * Helpers
 */

static inline bool vec2_is_finite(Vec2 vector)
{
    return isfinite(vector.x) && isfinite(vector.y);
}

/**
 * Arithmetic
 */

static inline Vec2 vec2_add(Vec2 a, Vec2 b)
{
    // clang-format off
    return vec2_create(
        a.x + b.x,
        a.y + b.y
    );
    // clang-format on
}

static inline Vec2 vec2_subtract(Vec2 a, Vec2 b)
{
    // clang-format off
    return vec2_create(
        a.x - b.x,
        a.y - b.y
    );
    // clang-format on
}

static inline Vec2 vec2_multiply(Vec2 vector, float32 scalar)
{
    // clang-format off
    return vec2_create(
        vector.x * scalar,
        vector.y * scalar
    );
    // clang-format on
}

/**
 * Divides a vector by a scalar.
 *
 * Preconditions:
 * - vector must have finite components;
 * - scalar must be finite and non-zero.
 *
 * Use vec2_try_divide() for untrusted or dynamically computed values.
 */
static inline Vec2 vec2_divide(Vec2 vector, float32 scalar)
{
    assert(vec2_is_finite(vector));
    assert(isfinite(scalar));
    assert(scalar != 0.0f);

    const float32 inverse = 1.0f / scalar;

    return vec2_multiply(vector, inverse);
}

static inline Vec2 vec2_negate(Vec2 vector)
{
    // clang-format off
    return vec2_create(
        -vector.x,
        -vector.y
    );
    // clang-format on
}

/**
 * Vector operations
 */

static inline float32 vec2_dot(Vec2 a, Vec2 b)
{
    return (a.x * b.x) + (a.y * b.y);
}

static inline float32 vec2_length_squared(Vec2 vector)
{
    return vec2_dot(vector, vector);
}

static inline float32 vec2_distance_squared(Vec2 a, Vec2 b)
{
    return vec2_length_squared(vec2_subtract(b, a));
}

/**
 * Performs an unclamped linear interpolation.
 *
 * amount = 0 returns from.
 * amount = 1 returns to.
 * Values outside [0, 1] extrapolate.
 */
static inline Vec2 vec2_lerp(Vec2 from, Vec2 to, float32 amount)
{
    assert(vec2_is_finite(from));
    assert(vec2_is_finite(to));
    assert(isfinite(amount));

    // clang-format off
    return vec2_create(
        math_lerp_float32(from.x, to.x, amount),
        math_lerp_float32(from.y, to.y, amount)
    );
    // clang-format on
}

static inline Vec2 vec2_lerp_clamped(Vec2 from, Vec2 to, float32 amount)
{
    // clang-format off
    return vec2_lerp(
        from,
        to,
        math_clamp_float32(amount, 0.0f, 1.0f)
    );
    // clang-format on
}

/**
 * In-place operations
 */

static inline void vec2_add_inplace(Vec2* vector, Vec2 value)
{
    assert(vector != NULL);

    vector->x += value.x;
    vector->y += value.y;
}

static inline void vec2_subtract_inplace(Vec2* vector, Vec2 value)
{
    assert(vector != NULL);

    vector->x -= value.x;
    vector->y -= value.y;
}

static inline void vec2_multiply_inplace(Vec2* vector, float32 scalar)
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
 * Use vec2_try_divide() for untrusted or dynamically computed values.
 */
static inline void vec2_divide_inplace(Vec2* vector, float32 scalar)
{
    assert(vector != NULL);
    assert(vec2_is_finite(*vector));
    assert(isfinite(scalar));
    assert(scalar != 0.0f);

    const float32 inverse = 1.0f / scalar;

    vector->x *= inverse;
    vector->y *= inverse;
}

static inline void vec2_negate_inplace(Vec2* vector)
{
    assert(vector != NULL);

    vector->x = -vector->x;
    vector->y = -vector->y;
}

/**
 * Comparison
 */

static inline bool vec2_equals_exact(Vec2 a, Vec2 b)
{
    return a.x == b.x && a.y == b.y;
}

static inline bool vec2_near(Vec2 a, Vec2 b, float32 distance)
{
    assert(vec2_is_finite(a));
    assert(vec2_is_finite(b));
    assert(isfinite(distance));
    assert(distance >= 0.0f);

    const float32 maximum_distance_squared = distance * distance;

    assert(isfinite(maximum_distance_squared));

    const float32 actual_distance_squared = vec2_distance_squared(a, b);

    assert(isfinite(actual_distance_squared));

    return actual_distance_squared <= maximum_distance_squared;
}

/**
 * Returns true when the distance between a and b is less than
 * or equal to the provided squared distance.
 */
static inline bool vec2_near_squared(Vec2 a, Vec2 b, float32 maximum_distance_squared)
{
    assert(vec2_is_finite(a));
    assert(vec2_is_finite(b));
    assert(isfinite(maximum_distance_squared));
    assert(maximum_distance_squared >= 0.0f);

    const float32 actual_distance_squared = vec2_distance_squared(a, b);

    assert(isfinite(actual_distance_squared));

    return actual_distance_squared <= maximum_distance_squared;
}

/**
 * Operations implemented in vec2.c
 */

/**
 * Attempts to divide a vector by a scalar.
 *
 * Returns false when result is NULL, the vector or scalar is not
 * finite, the scalar is too close to zero, or the result overflows.
 *
 * On failure, the value pointed to by result is not modified.
 */
bool vec2_try_divide(Vec2 vector, float32 scalar, Vec2* result);

float32 vec2_length(Vec2 vector);

float32 vec2_distance(Vec2 a, Vec2 b);

Vec2 vec2_normalize(Vec2 vector);

/**
 * Returns true when the absolute difference of each corresponding
 * component is less than or equal to epsilon.
 */
bool vec2_equals_epsilon(Vec2 a, Vec2 b, float32 epsilon);

#endif // MATH_VEC2_H
