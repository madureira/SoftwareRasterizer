#ifndef MATH_QUATF_H
#define MATH_QUATF_H

#include "math/vec3f.h"

typedef struct Quatf
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quatf;

/**
 * Contruction
 */

// clang-format off
#define quatf(x_, y_, z_, w_) (Quatf){.x = (x_), .y = (y_), .z = (z_), .w = (w_)}
// clang-format on

static inline Quatf quatf_zero(void)
{
    return quatf(0.0f, 0.0f, 0.0f, 0.0f);
}

static inline Quatf quatf_identity(void)
{
    return quatf(0.0f, 0.0f, 0.0f, 1.0f);
}

/**
 * Helpers
 */

static inline bool quatf_is_finite(Quatf quaternion)
{
    return isfinite(quaternion.x) && isfinite(quaternion.y) && isfinite(quaternion.z)
           && isfinite(quaternion.w);
}

/**
 * Arithmetic
 */

static inline Quatf quatf_add(Quatf a, Quatf b)
{
    return quatf(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static inline Quatf quatf_sub(Quatf a, Quatf b)
{
    return quatf(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static inline Quatf quatf_scale(Quatf quaternion, f32 scalar)
{
    return quatf(quaternion.x * scalar, quaternion.y * scalar, quaternion.z * scalar,
                 quaternion.w * scalar);
}

static inline Quatf quatf_neg(Quatf quaternion)
{
    return quatf(-quaternion.x, -quaternion.y, -quaternion.z, -quaternion.w);
}

/**
 * Quaternion operations
 */

static inline f32 quatf_dot(Quatf a, Quatf b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

static inline f32 quatf_len_sq(Quatf quaternion)
{
    return quatf_dot(quaternion, quaternion);
}

static inline f32 quatf_len(Quatf quaternion)
{
    assert(quatf_is_finite(quaternion));

    const f32 length_squared = quatf_len_sq(quaternion);

    assert(isfinite(length_squared));

    return sqrtf(length_squared);
}

static inline Quatf quatf_conjugate(Quatf quaternion)
{
    return quatf(-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w);
}

/**
 * Quaternion multiplication.
 *
 * The order of multiplication matters:
 *
 * quatf_multiply(a, b) != quatf_multiply(b, a)
 */
static inline Quatf quatf_multiply(Quatf a, Quatf b)
{
    // clang-format off
    return quatf(
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    );
    // clang-format on
}

/**
 * Comparison
 */
static inline bool quatf_eq(Quatf a, Quatf b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

/**
 * Returns true when the absolute difference of each corresponding
 * component is less than or equal to epsilon.
 */
static inline bool quatf_eq_eps(Quatf a, Quatf b, f32 epsilon)
{
    assert(quatf_is_finite(a));
    assert(quatf_is_finite(b));
    assert(isfinite(epsilon));
    assert(epsilon >= 0.0f);

    // clang-format off
    return fabsf(a.x - b.x) <= epsilon
        && fabsf(a.y - b.y) <= epsilon
        && fabsf(a.z - b.z) <= epsilon
        && fabsf(a.w - b.w) <= epsilon;
    // clang-format on
}

Quatf quatf_normalize(Quatf quaternion);

static inline void quatf_normalize_inplace(Quatf* quaternion)
{
    assert(quaternion != NULL);
    *quaternion = quatf_normalize(*quaternion);
}

bool quatf_try_normalize(Quatf quaternion, Quatf* result);

Quatf quatf_inverse(Quatf quaternion);

bool quatf_try_inverse(Quatf quaternion, Quatf* result);

/**
 * Creates a quaternion representing a rotation around an axis.
 *
 * The axis does not need to be normalized, it is normalized internaly.
 */
Quatf quatf_from_axis_angle(Vec3f axis, f32 angle_radians);

/**
 * Creates a quaternion representing a rotation around an axis,
 * given a precomputed sine and cosine of the rotation angle
 */
Quatf quatf_from_axis_angle_sincos(Vec3f axis, f32 sine_half_angle, f32 cosine_half_angle);

/**
 * Euler angles
 *
 * pitch = rotation around x
 * yaw   = rotation around y
 * roll  = rotation around z
 *
 * Rotation order: Yaw * Pitch * Roll
 */
Quatf quatf_from_euler(f32 pitch, f32 yaw, f32 roll);

void quatf_to_axis_angle(Quatf quaternion, Vec3f* axis, f32* angle_radians);

/**
 * Creates a quaternion from an orthonormal basis given as its (right, up, back)
 * column vectors.
 *
 * "right", "up" and "back" must each be unit length and mutually orthogonal.
 */
Quatf quatf_from_orthonormal_basis(Vec3f right, Vec3f up, Vec3f back);

/**
 * Creates a rotation that orients an object to face "forward",
 * using "up" as a reference for the up direction.
 *
 * Follow the axis convention: right-handed, +Y up, -Z forward.
 * An unrotated object faces (0, 0, -1).
 *
 * "forward" and "up" don't need to be normalized, and "up" doesn't
 * need to be orthogonal to "forward".
 *
 * "forward" must be non-zero and must not be parallel to "up".
 */
Quatf quatf_look_rotation(Vec3f forward, Vec3f up);

/**
 * Creates the shortest-arc rotation that maps direction "from" onto direction "to".
 *
 * "from" and "to" don't need to be normalized, but must be non-zero.
 */
Quatf quatf_from_two_vectors(Vec3f from, Vec3f to);

/**
 * Rotates a vector using this quaternion.
 *
 * The quaternion must be normalized.
 */
Vec3f quatf_rotate_vec3f(Quatf quaternion, Vec3f vector);

Quatf quatf_rotate_x(Quatf quaternion, f32 angle_radians);

Quatf quatf_rotate_y(Quatf quaternion, f32 angle_radians);

Quatf quatf_rotate_z(Quatf quaternion, f32 angle_radians);

static inline void quatf_rotate_x_inplace(Quatf* quaternion, f32 angle_radians)
{
    assert(quaternion != NULL);
    *quaternion = quatf_rotate_x(*quaternion, angle_radians);
}

static inline void quatf_rotate_y_inplace(Quatf* quaternion, f32 angle_radians)
{
    assert(quaternion != NULL);
    *quaternion = quatf_rotate_y(*quaternion, angle_radians);
}

static inline void quatf_rotate_z_inplace(Quatf* quaternion, f32 angle_radians)
{
    assert(quaternion != NULL);
    *quaternion = quatf_rotate_z(*quaternion, angle_radians);
}

/**
 * Interpolates between two quaternions without clamping.
 */
Quatf quatf_lerp(Quatf from, Quatf to, f32 amount);

/**
 * Interpolates between two quaternions, clamping amount to [0,1].
 */
Quatf quatf_lerp_clamp(Quatf from, Quatf to, f32 amount);

/**
 * Spherical linear interpolation
 */
Quatf quatf_slerp(Quatf from, Quatf to, f32 amount);

/**
 * Normalized linear interpolation (lerp followed by normalize).
 *
 * Cheaper than quatf_slerp
 */
Quatf quatf_nlerp(Quatf from, Quatf to, f32 amount);

/**
 * Normalized linear interpolation, clamping amount to [0,1].
 */
Quatf quatf_nlerp_clamp(Quatf from, Quatf to, f32 amount);

#endif // MATH_QUATF_H
