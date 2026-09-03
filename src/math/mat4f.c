#include "math/mat4f.h"

#include <stdio.h>

Mat4f mat4f_rotation_x(f32 angle_radians)
{
    assert(isfinite(angle_radians));

    const f32 sine = sinf(angle_radians);
    const f32 cosine = cosf(angle_radians);

    return mat4f_rotation_x_sincos(sine, cosine);
}

Mat4f mat4f_rotation_y(f32 angle_radians)
{
    assert(isfinite(angle_radians));

    const f32 sine = sinf(angle_radians);
    const f32 cosine = cosf(angle_radians);

    return mat4f_rotation_y_sincos(sine, cosine);
}

Mat4f mat4f_rotation_z(f32 angle_radians)
{
    assert(isfinite(angle_radians));

    const f32 sine = sinf(angle_radians);
    const f32 cosine = cosf(angle_radians);

    return mat4f_rotation_z_sincos(sine, cosine);
}

Mat4f mat4f_rotation_axis(Vec3f axis, f32 angle_radians)
{
    assert(vec3f_is_finite(axis));
    assert(isfinite(angle_radians));

    return mat4f_rotation_quatf(quatf_from_axis_angle(axis, angle_radians));
}

Mat4f mat4f_perspective(f32 fovy_radians, f32 aspect, f32 near, f32 far)
{
    assert(isfinite(fovy_radians));
    assert(fovy_radians > 0.0f && fovy_radians < MATH_PI);
    assert(isfinite(aspect));
    assert(aspect > 0.0f);
    assert(isfinite(near));
    assert(isfinite(far));
    assert(0.0f < near && near < far);

    const f32 focal_length = 1.0f / tanf(fovy_radians * 0.5f);

    // clang-format off
    return mat4f(
        focal_length / aspect, 0.0f,          0.0f,                         0.0f,
        0.0f,                  focal_length,  0.0f,                         0.0f,
        0.0f,                  0.0f,         -(far + near) / (far - near), -2.0f * far * near / (far - near),
        0.0f,                  0.0f,         -1.0f,                         0.0f
    );
    // clang-format on
}

Quatf mat4f_to_quatf(Mat4f matrix)
{
    assert(mat4f_is_finite(matrix));

    Vec3f right = vec3f(matrix.m00, matrix.m10, matrix.m20);
    Vec3f up = vec3f(matrix.m01, matrix.m11, matrix.m21);
    Vec3f back = vec3f(matrix.m02, matrix.m12, matrix.m22);

    assert(vec3f_len_sq(right) > MATH_NORMALIZE_EPSILON_SQUARED);
    assert(vec3f_len_sq(up) > MATH_NORMALIZE_EPSILON_SQUARED);
    assert(vec3f_len_sq(back) > MATH_NORMALIZE_EPSILON_SQUARED);

    // Columns may carry scale (e.g. from mat4f_trs()); normalize them so the
    // extraction below can assume an orthonormal basis.
    right = vec3f_normalize(right);
    up = vec3f_normalize(up);
    back = vec3f_normalize(back);

    return quatf_from_orthonormal_basis(right, up, back);
}

bool mat4f_try_mul_vec3_perspective(Mat4f matrix, Vec3f point, Vec3f* result)
{
    if (result == NULL || !mat4f_is_finite(matrix) || !vec3f_is_finite(point))
    {
        return false;
    }

    const Vec4f transformed = mat4f_mul_vec4(matrix, vec4f_from_vec3f(point, 1.0f));

    Vec4f divided;

    if (!vec4f_try_perspective_divide(transformed, &divided))
    {
        return false;
    }

    *result = vec4f_xyz(divided);

    return true;
}

Mat4f mat4f_look_at(Vec3f eye, Vec3f target, Vec3f up)
{
    assert(vec3f_is_finite(eye));
    assert(vec3f_is_finite(target));
    assert(vec3f_is_finite(up));

    const Quatf rotation = quatf_look_rotation(vec3f_sub(target, eye), up);

    const Mat4f world = mat4f_trs(eye, rotation, vec3f_one());

    return mat4f_inverse(world);
}

bool mat4f_try_inverse(Mat4f matrix, Mat4f* result)
{
    if (result == NULL)
    {
        return false;
    }

    if (!mat4f_is_finite(matrix))
    {
        return false;
    }

    // Calculate the cofactors required by the adjugate matrix

    const f32 a00 = matrix.m00;
    const f32 a01 = matrix.m01;
    const f32 a02 = matrix.m02;
    const f32 a03 = matrix.m03;

    const f32 a10 = matrix.m10;
    const f32 a11 = matrix.m11;
    const f32 a12 = matrix.m12;
    const f32 a13 = matrix.m13;

    const f32 a20 = matrix.m20;
    const f32 a21 = matrix.m21;
    const f32 a22 = matrix.m22;
    const f32 a23 = matrix.m23;

    const f32 a30 = matrix.m30;
    const f32 a31 = matrix.m31;
    const f32 a32 = matrix.m32;
    const f32 a33 = matrix.m33;

    const f32 b00 = a00 * a11 - a01 * a10;
    const f32 b01 = a00 * a12 - a02 * a10;
    const f32 b02 = a00 * a13 - a03 * a10;
    const f32 b03 = a01 * a12 - a02 * a11;
    const f32 b04 = a01 * a13 - a03 * a11;
    const f32 b05 = a02 * a13 - a03 * a12;

    const f32 b06 = a20 * a31 - a21 * a30;
    const f32 b07 = a20 * a32 - a22 * a30;
    const f32 b08 = a20 * a33 - a23 * a30;
    const f32 b09 = a21 * a32 - a22 * a31;
    const f32 b10 = a21 * a33 - a23 * a31;
    const f32 b11 = a22 * a33 - a23 * a32;

    const f32 determinant = mat4f_determinant(matrix);

    if (!isfinite(determinant))
    {
        return false;
    }

    if (fabsf(determinant) <= MATH_DIVISION_EPSILON)
    {
        return false;
    }

    const f32 inverse_determinant = 1.0f / determinant;

    // clang-format off
    const Mat4f inverse = mat4f(
        ( a11 * b11 - a12 * b10 + a13 * b09) * inverse_determinant,
        (-a01 * b11 + a02 * b10 - a03 * b09) * inverse_determinant,
        ( a31 * b05 - a32 * b04 + a33 * b03) * inverse_determinant,
        (-a21 * b05 + a22 * b04 - a23 * b03) * inverse_determinant,

        (-a10 * b11 + a12 * b08 - a13 * b07) * inverse_determinant,
        ( a00 * b11 - a02 * b08 + a03 * b07) * inverse_determinant,
        (-a30 * b05 + a32 * b02 - a33 * b01) * inverse_determinant,
        ( a20 * b05 - a22 * b02 + a23 * b01) * inverse_determinant,

        ( a10 * b10 - a11 * b08 + a13 * b06) * inverse_determinant,
        (-a00 * b10 + a01 * b08 - a03 * b06) * inverse_determinant,
        ( a30 * b04 - a31 * b02 + a33 * b00) * inverse_determinant,
        (-a20 * b04 + a21 * b02 - a23 * b00) * inverse_determinant,

        (-a10 * b09 + a11 * b07 - a12 * b06) * inverse_determinant,
        ( a00 * b09 - a01 * b07 + a02 * b06) * inverse_determinant,
        (-a30 * b03 + a31 * b01 - a32 * b00) * inverse_determinant,
        ( a20 * b03 - a21 * b01 + a22 * b00) * inverse_determinant
    );
    // clang-format on

    if (!mat4f_is_finite(inverse))
    {
        return false;
    }

    *result = inverse;

    return true;
}

Mat4f mat4f_inverse(Mat4f matrix)
{
    assert(mat4f_is_finite(matrix));

    Mat4f result;

    const bool success = mat4f_try_inverse(matrix, &result);
    (void)success;

    assert(success);

    return result;
}

bool mat4f_try_normal_matrix(Mat4f matrix, Mat4f* result)
{
    if (result == NULL)
    {
        return false;
    }

    Mat4f inverse;

    if (!mat4f_try_inverse(matrix, &inverse))
    {
        return false;
    }

    *result = mat4f_transpose(inverse);

    return true;
}

void mat4f_print(Mat4f matrix)
{
    // clang-format off
    printf(
        "Mat4f(\n"
        "  [%.6f, %.6f, %.6f, %.6f]\n"
        "  [%.6f, %.6f, %.6f, %.6f]\n"
        "  [%.6f, %.6f, %.6f, %.6f]\n"
        "  [%.6f, %.6f, %.6f, %.6f]\n"
        ")\n",
        matrix.m00, matrix.m01, matrix.m02, matrix.m03,
        matrix.m10, matrix.m11, matrix.m12, matrix.m13,
        matrix.m20, matrix.m21, matrix.m22, matrix.m23,
        matrix.m30, matrix.m31, matrix.m32, matrix.m33
    );
    // clang-format on
}
