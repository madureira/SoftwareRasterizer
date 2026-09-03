#include "math/quatf.h"

#include <stdio.h>

#include "math/math_common.h"
#include "math/math_config.h"

Quatf quatf_normalize(Quatf quaternion)
{
    assert(quatf_is_finite(quaternion));

    const f32 length_squared = quatf_len_sq(quaternion);

    assert(length_squared > MATH_NORMALIZE_EPSILON_SQUARED);

    const f32 inverse_length = 1.0f / sqrtf(length_squared);

    const Quatf result = quatf_scale(quaternion, inverse_length);

    assert(quatf_is_finite(result));

    return result;
}

bool quatf_try_normalize(Quatf quaternion, Quatf* result)
{
    if (result == NULL)
    {
        return false;
    }

    if (!quatf_is_finite(quaternion))
    {
        return false;
    }

    const f32 length_squared = quatf_len_sq(quaternion);

    if (!isfinite(length_squared))
    {
        return false;
    }

    if (length_squared <= MATH_NORMALIZE_EPSILON_SQUARED)
    {
        return false;
    }

    const f32 inverse_length = 1.0f / sqrtf(length_squared);

    *result = quatf_scale(quaternion, inverse_length);

    return quatf_is_finite(*result);
}

Quatf quatf_inverse(Quatf quaternion)
{
    assert(quatf_is_finite(quaternion));

    const f32 length_squared = quatf_len_sq(quaternion);

    assert(length_squared > MATH_NORMALIZE_EPSILON_SQUARED);

    const f32 inverse_length_squared = 1.0f / length_squared;

    return quatf_scale(quatf_conjugate(quaternion), inverse_length_squared);
}

bool quatf_try_inverse(Quatf quaternion, Quatf* result)
{
    if (result == NULL)
    {
        return false;
    }

    if (!quatf_is_finite(quaternion))
    {
        return false;
    }

    const f32 length_squared = quatf_len_sq(quaternion);

    if (!isfinite(length_squared))
    {
        return false;
    }

    if (length_squared <= MATH_NORMALIZE_EPSILON_SQUARED)
    {
        return false;
    }

    *result = quatf_scale(quatf_conjugate(quaternion), 1.0f / length_squared);

    return quatf_is_finite(*result);
}

Quatf quatf_from_axis_angle(Vec3f axis, f32 angle_radians)
{
    assert(vec3f_is_finite(axis));
    assert(isfinite(angle_radians));

    axis = vec3f_normalize(axis);

    const f32 half_angle = angle_radians * 0.5f;

    const f32 sine = sinf(half_angle);
    const f32 cosine = cosf(half_angle);

    // clang-format off
    const Quatf result = quatf(
        axis.x * sine,
        axis.y * sine,
        axis.z * sine,
        cosine
    );
    // clang-format on

    assert(quatf_is_finite(result));

    return result;
}

Quatf quatf_from_axis_angle_sincos(Vec3f axis, f32 sine_half_angle, f32 cosine_half_angle)
{
    assert(vec3f_is_finite(axis));
    assert(isfinite(sine_half_angle));
    assert(isfinite(cosine_half_angle));

    axis = vec3f_normalize(axis);

    // clang-format off
    const Quatf result = quatf(
        axis.x * sine_half_angle,
        axis.y * sine_half_angle,
        axis.z * sine_half_angle,
        cosine_half_angle
    );
    // clang-format on

    assert(quatf_is_finite(result));

    return result;
}

Quatf quatf_from_euler(f32 pitch, f32 yaw, f32 roll)
{
    assert(isfinite(pitch));
    assert(isfinite(yaw));
    assert(isfinite(roll));

    const f32 half_pitch = pitch * 0.5f;
    const f32 half_yaw = yaw * 0.5f;
    const f32 half_roll = roll * 0.5f;

    const f32 sp = sinf(half_pitch);
    const f32 cp = cosf(half_pitch);

    const f32 sy = sinf(half_yaw);
    const f32 cy = cosf(half_yaw);

    const f32 sr = sinf(half_roll);
    const f32 cr = cosf(half_roll);

    // q = q_yaq * q_pitch * q_roll

    // clang-format off
    const Quatf result = quatf(
        cy * sp * cr + sy * cp * sr,
        sy * cp * cr - cy * sp * sr,
        cy * cp * sr - sy * sp * cr,
        cy * cp * cr + sy * sp * sr
    );
    // clang-format on

    assert(quatf_is_finite(result));

    return result;
}

void quatf_to_axis_angle(Quatf quaternion, Vec3f* axis, f32* angle_radians)
{
    assert(axis != NULL);
    assert(angle_radians != NULL);
    assert(quatf_is_finite(quaternion));

    /*
     *  q.w    = cos(angle / 2) * |q|
     * |q.xyz| = sin(angle / 2) * |q|
     */

    // clang-format off
    const f32 axis_length_squared =
        quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z;
    // clang-format on

    const f32 length_squared = axis_length_squared + quaternion.w * quaternion.w;

    /*
     * For a zero rotation, the axis is arbitrary.
     * Use +X as the conventional default.
     */
    if (axis_length_squared <= MATH_NORMALIZE_EPSILON_SQUARED * length_squared)
    {
        *axis = vec3f(1.0f, 0.0f, 0.0f);
        *angle_radians = 0.0f;
        return;
    }

    const f32 axis_length = sqrtf(axis_length_squared);
    const f32 inverse = 1.0f / axis_length;

    *axis = vec3f(quaternion.x * inverse, quaternion.y * inverse, quaternion.z * inverse);

    *angle_radians = 2.0f * atan2f(axis_length, quaternion.w);
}

Quatf quatf_from_orthonormal_basis(Vec3f right, Vec3f up, Vec3f back)
{
    assert(vec3f_is_finite(right));
    assert(vec3f_is_finite(up));
    assert(vec3f_is_finite(back));
    assert(fabsf(vec3f_len_sq(right) - 1.0f) <= MATH_COMPARISON_EPSILON);
    assert(fabsf(vec3f_len_sq(up) - 1.0f) <= MATH_COMPARISON_EPSILON);
    assert(fabsf(vec3f_len_sq(back) - 1.0f) <= MATH_COMPARISON_EPSILON);

    // Rotation matrix entries r{row}{col}, read from the (right, up, back) basis columns.
    const f32 r00 = right.x;
    const f32 r01 = up.x;
    const f32 r02 = back.x;

    const f32 r10 = right.y;
    const f32 r11 = up.y;
    const f32 r12 = back.y;

    const f32 r20 = right.z;
    const f32 r21 = up.z;
    const f32 r22 = back.z;

    // Sum of the diagonal, picks which branch below is numerically stable.
    const f32 trace = r00 + r11 + r22;

    Quatf result;

    if (trace > 0.0f)
    {
        const f32 four_w = sqrtf(trace + 1.0f) * 2.0f; // 4 * w
        const f32 inverse_four_w = 1.0f / four_w;

        // clang-format off
        result = quatf(
            (r21 - r12) * inverse_four_w,
            (r02 - r20) * inverse_four_w,
            (r10 - r01) * inverse_four_w,
            0.25f * four_w
        );
        // clang-format on
    }
    else if (r00 > r11 && r00 > r22)
    {
        const f32 four_x = sqrtf(1.0f + r00 - r11 - r22) * 2.0f; // 4 * x
        const f32 inverse_four_x = 1.0f / four_x;

        // clang-format off
        result = quatf(
            0.25f * four_x,
            (r01 + r10) * inverse_four_x,
            (r02 + r20) * inverse_four_x,
            (r21 - r12) * inverse_four_x
        );
        // clang-format on
    }
    else if (r11 > r22)
    {
        const f32 four_y = sqrtf(1.0f + r11 - r00 - r22) * 2.0f; // 4 * y
        const f32 inverse_four_y = 1.0f / four_y;

        // clang-format off
        result = quatf(
            (r01 + r10) * inverse_four_y,
            0.25f * four_y,
            (r12 + r21) * inverse_four_y,
            (r02 - r20) * inverse_four_y
        );
        // clang-format on
    }
    else
    {
        const f32 four_z = sqrtf(1.0f + r22 - r00 - r11) * 2.0f; // 4 * z
        const f32 inverse_four_z = 1.0f / four_z;

        // clang-format off
        result = quatf(
            (r02 + r20) * inverse_four_z,
            (r12 + r21) * inverse_four_z,
            0.25f * four_z,
            (r10 - r01) * inverse_four_z
        );
        // clang-format on
    }

    assert(quatf_is_finite(result));

    return result;
}

Quatf quatf_look_rotation(Vec3f forward, Vec3f up)
{
    assert(vec3f_is_finite(forward));
    assert(vec3f_is_finite(up));
    assert(vec3f_len_sq(forward) > MATH_NORMALIZE_EPSILON_SQUARED);

    const Vec3f forward_axis = vec3f_normalize(forward);
    const Vec3f right_unnormalized = vec3f_cross(forward_axis, up);

    // forward and up must not be parallel: there is no unique right axis
    assert(vec3f_len_sq(right_unnormalized) > MATH_NORMALIZE_EPSILON_SQUARED);

    const Vec3f right_axis = vec3f_normalize(right_unnormalized);
    const Vec3f up_axis = vec3f_cross(right_axis, forward_axis);
    const Vec3f back_axis = vec3f_neg(forward_axis);

    return quatf_from_orthonormal_basis(right_axis, up_axis, back_axis);
}

Quatf quatf_from_two_vectors(Vec3f from, Vec3f to)
{
    assert(vec3f_is_finite(from));
    assert(vec3f_is_finite(to));
    assert(vec3f_len_sq(from) > MATH_NORMALIZE_EPSILON_SQUARED);
    assert(vec3f_len_sq(to) > MATH_NORMALIZE_EPSILON_SQUARED);

    const Vec3f from_axis = vec3f_normalize(from);
    const Vec3f to_axis = vec3f_normalize(to);

    const f32 dot = vec3f_dot(from_axis, to_axis);

    if (dot > 1.0f - MATH_COMPARISON_EPSILON)
    {
        return quatf_identity();
    }

    if (dot < -1.0f + MATH_COMPARISON_EPSILON)
    {
        /*
         * from_axis and to_axis are antiparallel: their cross product degenerates
         * to zero, so pick an arbitrary axis orthogonal to from_axis instead.
         * The result is a 180 degree rotation around that axis.
         */
        const Vec3f fallback_axis =
            fabsf(from_axis.x) < 0.9f ? vec3f(1.0f, 0.0f, 0.0f) : vec3f(0.0f, 1.0f, 0.0f);
        const Vec3f axis = vec3f_normalize(vec3f_cross(from_axis, fallback_axis));

        const Quatf result = quatf(axis.x, axis.y, axis.z, 0.0f);

        assert(quatf_is_finite(result));

        return result;
    }

    const Vec3f axis = vec3f_cross(from_axis, to_axis);

    const Quatf result = quatf_normalize(quatf(axis.x, axis.y, axis.z, 1.0f + dot));

    assert(quatf_is_finite(result));

    return result;
}

Vec3f quatf_rotate_vec3f(Quatf quaternion, Vec3f vector)
{
    assert(quatf_is_finite(quaternion));
    assert(vec3f_is_finite(vector));

    // This optimized implementation assumes a unit quaternion
    assert(fabsf(quatf_len_sq(quaternion) - 1.0f) <= MATH_COMPARISON_EPSILON);

    const Vec3f q = vec3f(quaternion.x, quaternion.y, quaternion.z);

    const Vec3f t = vec3f_scale(vec3f_cross(q, vector), 2.0f);

    // clang-format off
    const Vec3f result = vec3f_add(
        vec3f_add(
            vector,
            vec3f_scale(t, quaternion.w)
        ),
        vec3f_cross(q, t)
    );
    // clang-format on

    assert(vec3f_is_finite(result));

    return result;
}

Quatf quatf_rotate_x(Quatf quaternion, f32 angle_radians)
{
    assert(quatf_is_finite(quaternion));
    assert(isfinite(angle_radians));

    const f32 half_angle = angle_radians * 0.5f;

    const f32 s = sinf(half_angle);
    const f32 c = cosf(half_angle);

    // clang-format off
    const Quatf result = quatf(
        quaternion.x * c + quaternion.w * s,
        quaternion.y * c + quaternion.z * s,
        quaternion.z * c - quaternion.y * s,
        quaternion.w * c - quaternion.x * s
    );
    // clang-format on

    assert(quatf_is_finite(result));

    return result;
}

Quatf quatf_rotate_y(Quatf quaternion, f32 angle_radians)
{
    assert(quatf_is_finite(quaternion));
    assert(isfinite(angle_radians));

    const f32 half_angle = angle_radians * 0.5f;

    const f32 s = sinf(half_angle);
    const f32 c = cosf(half_angle);

    // clang-format off
    const Quatf result = quatf(
        quaternion.x * c - quaternion.z * s,
        quaternion.y * c + quaternion.w * s,
        quaternion.z * c + quaternion.x * s,
        quaternion.w * c - quaternion.y * s
    );
    // clang-format on

    assert(quatf_is_finite(result));

    return result;
}

Quatf quatf_rotate_z(Quatf quaternion, f32 angle_radians)
{
    assert(quatf_is_finite(quaternion));
    assert(isfinite(angle_radians));

    const f32 half_angle = angle_radians * 0.5f;

    const f32 s = sinf(half_angle);
    const f32 c = cosf(half_angle);

    // clang-format off
    const Quatf result = quatf(
        quaternion.x * c + quaternion.y * s,
        quaternion.y * c - quaternion.x * s,
        quaternion.z * c + quaternion.w * s,
        quaternion.w * c - quaternion.z * s
    );
    // clang-format on

    assert(quatf_is_finite(result));

    return result;
}

Quatf quatf_lerp(Quatf from, Quatf to, f32 amount)
{
    assert(quatf_is_finite(from));
    assert(quatf_is_finite(to));
    assert(isfinite(amount));

    // clang-format off
    return quatf_add(
        from,
        quatf_scale(
            quatf_sub(to, from),
            amount
        )
    );
    // clang-format on
}

Quatf quatf_lerp_clamp(Quatf from, Quatf to, f32 amount)
{
    // clang-format off
    return quatf_lerp(
        from,
        to,
        math_clamp_f32(amount, 0.0f, 1.0f)
    );
    // clang-format on
}

Quatf quatf_slerp(Quatf from, Quatf to, f32 amount)
{
    assert(quatf_is_finite(from));
    assert(quatf_is_finite(to));
    assert(isfinite(amount));

    from = quatf_normalize(from);
    to = quatf_normalize(to);

    f32 dot = quatf_dot(from, to);

    /*
     * q ahd -q represent the same rotation.
     * Use the shortest interpolation path.
     */
    if (dot < 0.0f)
    {
        to = quatf_neg(to);
        dot = -dot;
    }

    /*
     * For very small angles, lerp avoids
     * numerical instability in sin(theta).
     */
    if (dot > 1.0f - MATH_COMPARISON_EPSILON)
    {
        return quatf_normalize(quatf_lerp(from, to, amount));
    }

    const f32 angle = acosf(dot);
    const f32 sine_angle = sinf(angle);
    const f32 inverse_sine_angle = 1.0f / sine_angle;

    const f32 from_weight = sinf((1.0f - amount) * angle) * inverse_sine_angle;

    const f32 to_weight = sinf(amount * angle) * inverse_sine_angle;

    // clang-format off
    return quatf_add(
        quatf_scale(from, from_weight),
        quatf_scale(to, to_weight)
    );
    // clang-format on
}

Quatf quatf_nlerp(Quatf from, Quatf to, f32 amount)
{
    assert(quatf_is_finite(from));
    assert(quatf_is_finite(to));
    assert(isfinite(amount));

    /*
     * q and -q represent the same rotation.
     * Use the shortest interpolation path.
     */
    if (quatf_dot(from, to) < 0.0f)
    {
        to = quatf_neg(to);
    }

    return quatf_normalize(quatf_lerp(from, to, amount));
}

Quatf quatf_nlerp_clamp(Quatf from, Quatf to, f32 amount)
{
    // clang-format off
    return quatf_nlerp(
        from,
        to,
        math_clamp_f32(amount, 0.0f, 1.0f)
    );
    // clang-format on
}

void quatf_print(Quatf quaternion)
{
    // clang-format off
    printf(
        "Quatf(x: %.6f, y: %.6f, z: %.6f, w: %.6f)\n",
        quaternion.x, quaternion.y, quaternion.z, quaternion.w
    );
    // clang-format on
}
