#ifndef MATH_MAT4F_H
#define MATH_MAT4F_H

#include "math/quatf.h"
#include "math/vec3f.h"
#include "math/vec4f.h"

// clang-format off
typedef struct Mat4f
{
    f32 m00, m01, m02, m03;
    f32 m10, m11, m12, m13;
    f32 m20, m21, m22, m23;
    f32 m30, m31, m32, m33;
} Mat4f;

/*
 * Construction
 */

#define mat4f(                                                  \
    m00_, m01_, m02_, m03_,                                     \
    m10_, m11_, m12_, m13_,                                     \
    m20_, m21_, m22_, m23_,                                     \
    m30_, m31_, m32_, m33_                                      \
)(Mat4f) {                                                      \
    .m00 = (m00_), .m01 = (m01_), .m02 = (m02_), .m03 = (m03_), \
    .m10 = (m10_), .m11 = (m11_), .m12 = (m12_), .m13 = (m13_), \
    .m20 = (m20_), .m21 = (m21_), .m22 = (m22_), .m23 = (m23_), \
    .m30 = (m30_), .m31 = (m31_), .m32 = (m32_), .m33 = (m33_)  \
}
// clang-format on

static inline Mat4f mat4f_zero(void)
{
    // clang-format off
    return mat4f(
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    );
    // clang-format on
}

static inline Mat4f mat4f_identity(void)
{
    // clang-format off
    return mat4f(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on
}

/*
 * Helpers
 */

static inline bool mat4f_is_finite(Mat4f m)
{
    // clang-format off
    return isfinite(m.m00) && isfinite(m.m01) && isfinite(m.m02) && isfinite(m.m03)
        && isfinite(m.m10) && isfinite(m.m11) && isfinite(m.m12) && isfinite(m.m13)
        && isfinite(m.m20) && isfinite(m.m21) && isfinite(m.m22) && isfinite(m.m23)
        && isfinite(m.m30) && isfinite(m.m31) && isfinite(m.m32) && isfinite(m.m33);
    // clang-format on
}

/*
 * Arithmetic
 */

static inline Mat4f mat4f_add(Mat4f a, Mat4f b)
{
    assert(mat4f_is_finite(a));
    assert(mat4f_is_finite(b));

    // clang-format off
    return mat4f(
        a.m00 + b.m00, a.m01 + b.m01, a.m02 + b.m02, a.m03 + b.m03,
        a.m10 + b.m10, a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13,
        a.m20 + b.m20, a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23,
        a.m30 + b.m30, a.m31 + b.m31, a.m32 + b.m32, a.m33 + b.m33
    );
    // clang-format on
}

static inline void mat4f_add_inplace(Mat4f* matrix, Mat4f value)
{
    assert(matrix != NULL);

    *matrix = mat4f_add(*matrix, value);
}

static inline Mat4f mat4f_sub(Mat4f a, Mat4f b)
{
    assert(mat4f_is_finite(a));
    assert(mat4f_is_finite(b));

    // clang-format off
    return mat4f(
        a.m00 - b.m00, a.m01 - b.m01, a.m02 - b.m02, a.m03 - b.m03,
        a.m10 - b.m10, a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13,
        a.m20 - b.m20, a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23,
        a.m30 - b.m30, a.m31 - b.m31, a.m32 - b.m32, a.m33 - b.m33
    );
    // clang-format on
}

static inline void mat4f_sub_inplace(Mat4f* matrix, Mat4f value)
{
    assert(matrix != NULL);

    *matrix = mat4f_sub(*matrix, value);
}

static inline Mat4f mat4f_scale(Mat4f matrix, f32 scalar)
{
    assert(mat4f_is_finite(matrix));
    assert(isfinite(scalar));

    // clang-format off
    return mat4f(
        matrix.m00 * scalar, matrix.m01 * scalar, matrix.m02 * scalar, matrix.m03 * scalar,
        matrix.m10 * scalar, matrix.m11 * scalar, matrix.m12 * scalar, matrix.m13 * scalar,
        matrix.m20 * scalar, matrix.m21 * scalar, matrix.m22 * scalar, matrix.m23 * scalar,
        matrix.m30 * scalar, matrix.m31 * scalar, matrix.m32 * scalar, matrix.m33 * scalar
    );
    // clang-format on
}

static inline void mat4f_scale_inplace(Mat4f* matrix, f32 scalar)
{
    assert(matrix != NULL);

    *matrix = mat4f_scale(*matrix, scalar);
}

static inline Mat4f mat4f_neg(Mat4f matrix)
{
    return mat4f_scale(matrix, -1.0f);
}

static inline void mat4f_neg_inplace(Mat4f* matrix)
{
    assert(matrix != NULL);

    *matrix = mat4f_neg(*matrix);
}

/*
 * Matrix operations
 */

static inline Mat4f mat4f_mul(Mat4f a, Mat4f b)
{
    assert(mat4f_is_finite(a));
    assert(mat4f_is_finite(b));

    // clang-format off
    return mat4f(
        a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30,
        a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31,
        a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32,
        a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33,

        a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30,
        a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31,
        a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32,
        a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33,

        a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30,
        a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31,
        a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32,
        a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33,

        a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30,
        a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31,
        a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32,
        a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33
    );
    // clang-format on
}

static inline void mat4f_mul_inplace(Mat4f* matrix, Mat4f value)
{
    assert(matrix != NULL);

    *matrix = mat4f_mul(*matrix, value);
}

static inline Mat4f mat4f_transpose(Mat4f matrix)
{
    assert(mat4f_is_finite(matrix));

    // clang-format off
    return mat4f(
        matrix.m00, matrix.m10, matrix.m20, matrix.m30,
        matrix.m01, matrix.m11, matrix.m21, matrix.m31,
        matrix.m02, matrix.m12, matrix.m22, matrix.m32,
        matrix.m03, matrix.m13, matrix.m23, matrix.m33
    );
    // clang-format on
}

static inline void mat4f_transpose_inplace(Mat4f* matrix)
{
    assert(matrix != NULL);

    *matrix = mat4f_transpose(*matrix);
}

/**
 * Computes the determinant of a matrix.
 *
 * A zero (or near-zero) determinant means the matrix is singular/too close to
 * singular to invert (see mat4f_try_inverse()). A negative determinant means
 * the matrix flips orientation (mirrored/negative scale) — the case
 * mat4f_get_scale() can't detect on its own.
 */
static inline f32 mat4f_determinant(Mat4f matrix)
{
    assert(mat4f_is_finite(matrix));

    const f32 b00 = matrix.m00 * matrix.m11 - matrix.m01 * matrix.m10;
    const f32 b01 = matrix.m00 * matrix.m12 - matrix.m02 * matrix.m10;
    const f32 b02 = matrix.m00 * matrix.m13 - matrix.m03 * matrix.m10;
    const f32 b03 = matrix.m01 * matrix.m12 - matrix.m02 * matrix.m11;
    const f32 b04 = matrix.m01 * matrix.m13 - matrix.m03 * matrix.m11;
    const f32 b05 = matrix.m02 * matrix.m13 - matrix.m03 * matrix.m12;

    const f32 b06 = matrix.m20 * matrix.m31 - matrix.m21 * matrix.m30;
    const f32 b07 = matrix.m20 * matrix.m32 - matrix.m22 * matrix.m30;
    const f32 b08 = matrix.m20 * matrix.m33 - matrix.m23 * matrix.m30;
    const f32 b09 = matrix.m21 * matrix.m32 - matrix.m22 * matrix.m31;
    const f32 b10 = matrix.m21 * matrix.m33 - matrix.m23 * matrix.m31;
    const f32 b11 = matrix.m22 * matrix.m33 - matrix.m23 * matrix.m32;

    // clang-format off
    return b00 * b11
        - b01 * b10
        + b02 * b09
        + b03 * b08
        - b04 * b07
        + b05 * b06;
    // clang-format on
}

/*
 * Vector transformation
 */

static inline Vec3f mat4f_mul_vec3(Mat4f matrix, Vec3f point)
{
    assert(mat4f_is_finite(matrix));
    assert(vec3f_is_finite(point));

    // clang-format off
    return vec3f(
        matrix.m00 * point.x +
        matrix.m01 * point.y +
        matrix.m02 * point.z +
        matrix.m03,

        matrix.m10 * point.x +
        matrix.m11 * point.y +
        matrix.m12 * point.z +
        matrix.m13,

        matrix.m20 * point.x +
        matrix.m21 * point.y +
        matrix.m22 * point.z +
        matrix.m23
    );
    // clang-format on
}

/**
 * Transforms a direction (e.g. a displacement, or a normal when
 * "matrix" has no non-uniform scale) by the 3x3 linear part of the matrix,
 * ignoring translation.
 */
static inline Vec3f mat4f_mul_vec3_direction(Mat4f matrix, Vec3f direction)
{
    assert(mat4f_is_finite(matrix));
    assert(vec3f_is_finite(direction));

    // clang-format off
    return vec3f(
        matrix.m00 * direction.x + matrix.m01 * direction.y + matrix.m02 * direction.z,
        matrix.m10 * direction.x + matrix.m11 * direction.y + matrix.m12 * direction.z,
        matrix.m20 * direction.x + matrix.m21 * direction.y + matrix.m22 * direction.z
    );
    // clang-format on
}

/**
 * Transforms a homogeneous vector by the full 4x4 matrix (including row 3),
 * without dividing by w.
 */
static inline Vec4f mat4f_mul_vec4(Mat4f matrix, Vec4f vector)
{
    assert(mat4f_is_finite(matrix));
    assert(vec4f_is_finite(vector));

    // clang-format off
    return vec4f(
        matrix.m00 * vector.x + matrix.m01 * vector.y + matrix.m02 * vector.z + matrix.m03 * vector.w,
        matrix.m10 * vector.x + matrix.m11 * vector.y + matrix.m12 * vector.z + matrix.m13 * vector.w,
        matrix.m20 * vector.x + matrix.m21 * vector.y + matrix.m22 * vector.z + matrix.m23 * vector.w,
        matrix.m30 * vector.x + matrix.m31 * vector.y + matrix.m32 * vector.z + matrix.m33 * vector.w
    );
    // clang-format on
}

/**
 * Transforms a point by the full 4x4 matrix (including row 3) and divides the
 * result by w, i.e. a perspective divide.
 */
static inline Vec3f mat4f_mul_vec3_perspective(Mat4f matrix, Vec3f point)
{
    assert(mat4f_is_finite(matrix));
    assert(vec3f_is_finite(point));

    const Vec4f transformed = mat4f_mul_vec4(matrix, vec4f_from_vec3f(point, 1.0f));

    assert(isfinite(transformed.w));
    assert(fabsf(transformed.w) > MATH_DIVISION_EPSILON);

    return vec4f_xyz(vec4f_scale(transformed, 1.0f / transformed.w));
}

/*
 * Comparison
 */

static inline bool mat4f_eq(Mat4f a, Mat4f b)
{
    // clang-format off
    return a.m00 == b.m00 && a.m01 == b.m01 && a.m02 == b.m02 && a.m03 == b.m03
        && a.m10 == b.m10 && a.m11 == b.m11 && a.m12 == b.m12 && a.m13 == b.m13
        && a.m20 == b.m20 && a.m21 == b.m21 && a.m22 == b.m22 && a.m23 == b.m23
        && a.m30 == b.m30 && a.m31 == b.m31 && a.m32 == b.m32 && a.m33 == b.m33;
    // clang-format on
}

static inline bool mat4f_eq_eps(Mat4f a, Mat4f b, f32 epsilon)
{
    assert(mat4f_is_finite(a));
    assert(mat4f_is_finite(b));
    assert(isfinite(epsilon));

    // clang-format off
    return fabsf(a.m00 - b.m00) <= epsilon && fabsf(a.m01 - b.m01) <= epsilon
        && fabsf(a.m02 - b.m02) <= epsilon && fabsf(a.m03 - b.m03) <= epsilon
        && fabsf(a.m10 - b.m10) <= epsilon && fabsf(a.m11 - b.m11) <= epsilon
        && fabsf(a.m12 - b.m12) <= epsilon && fabsf(a.m13 - b.m13) <= epsilon
        && fabsf(a.m20 - b.m20) <= epsilon && fabsf(a.m21 - b.m21) <= epsilon
        && fabsf(a.m22 - b.m22) <= epsilon && fabsf(a.m23 - b.m23) <= epsilon
        && fabsf(a.m30 - b.m30) <= epsilon && fabsf(a.m31 - b.m31) <= epsilon
        && fabsf(a.m32 - b.m32) <= epsilon && fabsf(a.m33 - b.m33) <= epsilon;
    // clang-format on
}

/*
 * Transformation matrices
 */

static inline Mat4f mat4f_translation(Vec3f translation)
{
    assert(vec3f_is_finite(translation));

    // clang-format off
    return mat4f(
        1.0f, 0.0f, 0.0f, translation.x,
        0.0f, 1.0f, 0.0f, translation.y,
        0.0f, 0.0f, 1.0f, translation.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on
}

static inline Mat4f mat4f_scale_xyz(Vec3f scale)
{
    assert(vec3f_is_finite(scale));

    // clang-format off
    return mat4f(
        scale.x, 0.0f,    0.0f,    0.0f,
        0.0f,    scale.y, 0.0f,    0.0f,
        0.0f,    0.0f,    scale.z, 0.0f,
        0.0f,    0.0f,    0.0f,    1.0f
    );
    // clang-format on
}

/**
 * Computes an orthographic projection matrix.
 *
 * Follows the same view-space convention as mat4f_perspective(): right-handed,
 * -Z forward, "near"/"far" are positive distances in front of the camera
 * (0 < near < far). NDC depth is OpenGL-style, i.e. z maps to [-1, 1].
 */
static inline Mat4f mat4f_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
    assert(isfinite(left));
    assert(isfinite(right));
    assert(isfinite(bottom));
    assert(isfinite(top));
    assert(isfinite(near));
    assert(isfinite(far));
    assert(right != left);
    assert(top != bottom);
    assert(0.0f < near && near < far);

    // clang-format off
    return mat4f(
        2.0f / (right - left), 0.0f,                  0.0f,                -(right + left) / (right - left),
        0.0f,                  2.0f / (top - bottom), 0.0f,                 (top + bottom) / (top - bottom),
        0.0f,                  0.0f,                 -2.0f / (far - near), -(far + near) / (far - near),
        0.0f,                  0.0f,                  0.0f,                 1.0f
    );
    // clang-format on
}

static inline Mat4f mat4f_rotation_x_sincos(f32 sine, f32 cosine)
{
    assert(isfinite(sine));
    assert(isfinite(cosine));

    // clang-format off
    return mat4f(
        1.0f, 0.0f,    0.0f,   0.0f,
        0.0f, cosine, -sine,   0.0f,
        0.0f, sine,    cosine, 0.0f,
        0.0f, 0.0f,    0.0f,   1.0f
    );
    // clang-format on
}

static inline Mat4f mat4f_rotation_y_sincos(f32 sine, f32 cosine)
{
    assert(isfinite(sine));
    assert(isfinite(cosine));

    // clang-format off
    return mat4f(
         cosine, 0.0f, sine,   0.0f,
         0.0f,   1.0f, 0.0f,   0.0f,
        -sine,   0.0f, cosine, 0.0f,
         0.0f,   0.0f, 0.0f,   1.0f
    );
    // clang-format on
}

static inline Mat4f mat4f_rotation_z_sincos(f32 sine, f32 cosine)
{
    assert(isfinite(sine));
    assert(isfinite(cosine));

    // clang-format off
    return mat4f(
        cosine, -sine,   0.0f, 0.0f,
        sine,    cosine, 0.0f, 0.0f,
        0.0f,    0.0f,   1.0f, 0.0f,
        0.0f,    0.0f,   0.0f, 1.0f
    );
    // clang-format on
}

Mat4f mat4f_rotation_x(f32 angle_radians);

Mat4f mat4f_rotation_y(f32 angle_radians);

Mat4f mat4f_rotation_z(f32 angle_radians);

/**
 * Computes a rotation matrix around an arbitrary axis.
 *
 * The axis is normalized internally.
 */
Mat4f mat4f_rotation_axis(Vec3f axis, f32 angle_radians);

/**
 * Computes a perspective projection matrix.
 *
 * Follows the same view-space convention as mat4f_look_at()/quatf_look_rotation():
 * right-handed, -Z forward. "fovy_radians" is the vertical field of view in
 * radians (0 < fovy_radians < PI), "aspect" is width/height (> 0), and
 * "near"/"far" are positive distances in front of the camera (0 < near < far).
 * NDC depth is OpenGL-style, i.e. z maps to [-1, 1].
 */
Mat4f mat4f_perspective(f32 fovy_radians, f32 aspect, f32 near, f32 far);

/**
 * Computes a rotation matrix from a unit quaternion.
 */
static inline Mat4f mat4f_rotation_quatf(Quatf quaternion)
{
    assert(quatf_is_finite(quaternion));
    assert(fabsf(quatf_len_sq(quaternion) - 1.0f) <= MATH_COMPARISON_EPSILON);

    const f32 x = quaternion.x;
    const f32 y = quaternion.y;
    const f32 z = quaternion.z;
    const f32 w = quaternion.w;

    const f32 xx = x * x;
    const f32 yy = y * y;
    const f32 zz = z * z;
    const f32 xy = x * y;
    const f32 xz = x * z;
    const f32 yz = y * z;
    const f32 wx = w * x;
    const f32 wy = w * y;
    const f32 wz = w * z;

    // clang-format off
    return mat4f(
        1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz),        2.0f * (xz + wy),        0.0f,
        2.0f * (xy + wz),        1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx),        0.0f,
        2.0f * (xz - wy),        2.0f * (yz + wx),        1.0f - 2.0f * (xx + yy), 0.0f,
        0.0f,                    0.0f,                    0.0f,                    1.0f
    );
    // clang-format on
}

/**
 * Extracts the rotation of a matrix as a quaternion, from the 3x3 linear part.
 *
 * The 3 basis columns are normalized internally, so this tolerates matrices
 * carrying uniform or non-uniform scale (as produced by mat4f_trs()). It
 * cannot recover mirrored/negative-determinant scale.
 */
Quatf mat4f_to_quatf(Mat4f matrix);

/**
 * Builds a model matrix from a translation, rotation and scale (T * R * S).
 */
static inline Mat4f mat4f_trs(Vec3f translation, Quatf rotation, Vec3f scale)
{
    // clang-format off
    return mat4f_mul(
        mat4f_translation(translation),
        mat4f_mul(
            mat4f_rotation_quatf(rotation),
            mat4f_scale_xyz(scale)
        )
    );
    // clang-format on
}

/**
 * Extracts the translation component of a matrix, i.e. its last column.
 */
static inline Vec3f mat4f_get_translation(Mat4f matrix)
{
    assert(mat4f_is_finite(matrix));

    return vec3f(matrix.m03, matrix.m13, matrix.m23);
}

/**
 * Extracts the scale component of a matrix, as the length of each of its 3 basis columns.
 *
 * Assumes the matrix has no shear. Always returns positive components, so it
 * cannot detect mirrored/negative-determinant scale.
 */
static inline Vec3f mat4f_get_scale(Mat4f matrix)
{
    assert(mat4f_is_finite(matrix));

    // clang-format off
    return vec3f(
        vec3f_len(vec3f(matrix.m00, matrix.m10, matrix.m20)),
        vec3f_len(vec3f(matrix.m01, matrix.m11, matrix.m21)),
        vec3f_len(vec3f(matrix.m02, matrix.m12, matrix.m22))
    );
    // clang-format on
}

/**
 * Attempts to invert a matrix.
 *
 * Return false when result is NULL, the matrix is not finite,
 * or the matrix is singular/too close to singular.
 *
 * On failure, the value pointed to by result is not modified.
 */
bool mat4f_try_inverse(Mat4f matrix, Mat4f* result);

Mat4f mat4f_inverse(Mat4f matrix);

/**
 * Computes the matrix that correctly transforms normals under "matrix",
 * even when "matrix" carries non-uniform scale: the inverse-transpose of its
 * 3x3 linear part (the translation column doesn't leak into that block for
 * an affine matrix, so transposing the full inverse is enough).
 *
 * Use with mat4f_mul_vec3_direction(), e.g.:
 * mat4f_mul_vec3_direction(mat4f_normal_matrix(model), normal)
 *
 * Asserts that "matrix" is invertible; use mat4f_try_normal_matrix() when
 * that can't be guaranteed.
 */
static inline Mat4f mat4f_normal_matrix(Mat4f matrix)
{
    return mat4f_transpose(mat4f_inverse(matrix));
}

/**
 * Attempts to compute the matrix that correctly transforms normals under
 * "matrix" (see mat4f_normal_matrix()).
 *
 * Returns false when result is NULL, the matrix is not finite, or the
 * matrix is singular/too close to singular.
 *
 * On failure, the value pointed to by result is not modified.
 */
bool mat4f_try_normal_matrix(Mat4f matrix, Mat4f* result);

/**
 * Attempts to transform a point by the full 4x4 matrix (including row 3),
 * dividing the result by w.
 *
 * Returns false when result is NULL, the matrix or point is not finite, or
 * w is not finite or too close to zero (for instance, a point behind the
 * camera under a perspective projection).
 *
 * On failure, the value pointed to by result is not modified.
 */
bool mat4f_try_mul_vec3_perspective(Mat4f matrix, Vec3f point, Vec3f* result);

/**
 * Computes a view matrix that positions the camera at "eye" and orients it to
 * face "target", using "up" as a reference for the up direction.
 *
 * "target" and "up" follow the same preconditions as quatf_look_rotation():
 * "eye" must not equal "target", and "up" must not be parallel to the
 * eye-to-target direction.
 */
Mat4f mat4f_look_at(Vec3f eye, Vec3f target, Vec3f up);

void mat4f_print(Mat4f matrix);

#endif // MATH_MAT4F_H
