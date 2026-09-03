#include "test.h"

#include "math/mat4f.h"

#define EPS       1e-5f
#define EPS_LOOSE 1e-4f

/*
 * Construction
 */

static void test_create(TestContext* ctx)
{
    // clang-format off
    Mat4f m = mat4f(
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    );
    // clang-format on

    TEST_ASSERT(ctx, m.m00 == 1.0f);
    TEST_ASSERT(ctx, m.m01 == 2.0f);
    TEST_ASSERT(ctx, m.m02 == 3.0f);
    TEST_ASSERT(ctx, m.m03 == 4.0f);
    TEST_ASSERT(ctx, m.m10 == 5.0f);
    TEST_ASSERT(ctx, m.m11 == 6.0f);
    TEST_ASSERT(ctx, m.m12 == 7.0f);
    TEST_ASSERT(ctx, m.m13 == 8.0f);
    TEST_ASSERT(ctx, m.m20 == 9.0f);
    TEST_ASSERT(ctx, m.m21 == 10.0f);
    TEST_ASSERT(ctx, m.m22 == 11.0f);
    TEST_ASSERT(ctx, m.m23 == 12.0f);
    TEST_ASSERT(ctx, m.m30 == 13.0f);
    TEST_ASSERT(ctx, m.m31 == 14.0f);
    TEST_ASSERT(ctx, m.m32 == 15.0f);
    TEST_ASSERT(ctx, m.m33 == 16.0f);
}

static void test_zero(TestContext* ctx)
{
    // clang-format off
    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_zero(), mat4f(
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    )));
    // clang-format on
}

static void test_identity(TestContext* ctx)
{
    // clang-format off
    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_identity(), mat4f(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    )));
    // clang-format on
}

/*
 * Helpers
 */

static void test_is_finite_true_for_finite_matrix(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, mat4f_is_finite(mat4f_identity()));
}

static void test_is_finite_false_for_nan_component(TestContext* ctx)
{
    Mat4f m = mat4f_identity();
    m.m21 = NAN;
    TEST_ASSERT_FALSE(ctx, mat4f_is_finite(m));
}

static void test_is_finite_false_for_infinite_component(TestContext* ctx)
{
    Mat4f m = mat4f_identity();
    m.m32 = INFINITY;
    TEST_ASSERT_FALSE(ctx, mat4f_is_finite(m));
}

/*
 * Arithmetic
 */

static void test_add(TestContext* ctx)
{
    // clang-format off
    Mat4f a = mat4f(
        1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f
    );
    Mat4f b = mat4f(
        4.0f, 3.0f, 2.0f, 1.0f,
        4.0f, 3.0f, 2.0f, 1.0f,
        4.0f, 3.0f, 2.0f, 1.0f,
        4.0f, 3.0f, 2.0f, 1.0f
    );
    Mat4f expected = mat4f(
        5.0f, 5.0f, 5.0f, 5.0f,
        5.0f, 5.0f, 5.0f, 5.0f,
        5.0f, 5.0f, 5.0f, 5.0f,
        5.0f, 5.0f, 5.0f, 5.0f
    );
    // clang-format on

    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_add(a, b), expected));
}

static void test_subtract(TestContext* ctx)
{
    // clang-format off
    Mat4f a = mat4f(
        5.0f, 5.0f, 5.0f, 5.0f,
        5.0f, 5.0f, 5.0f, 5.0f,
        5.0f, 5.0f, 5.0f, 5.0f,
        5.0f, 5.0f, 5.0f, 5.0f
    );
    Mat4f b = mat4f(
        1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f
    );
    Mat4f expected = mat4f(
        4.0f, 3.0f, 2.0f, 1.0f,
        4.0f, 3.0f, 2.0f, 1.0f,
        4.0f, 3.0f, 2.0f, 1.0f,
        4.0f, 3.0f, 2.0f, 1.0f
    );
    // clang-format on

    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_sub(a, b), expected));
}

static void test_scale(TestContext* ctx)
{
    Mat4f r = mat4f_scale(mat4f_identity(), 3.0f);

    // clang-format off
    TEST_ASSERT_TRUE(ctx, mat4f_eq(r, mat4f(
        3.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 3.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 3.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 3.0f
    )));
    // clang-format on
}

static void test_negate(TestContext* ctx)
{
    // clang-format off
    Mat4f m = mat4f(
         1.0f, -2.0f,  3.0f, -4.0f,
        -1.0f,  2.0f, -3.0f,  4.0f,
         1.0f, -2.0f,  3.0f, -4.0f,
        -1.0f,  2.0f, -3.0f,  4.0f
    );
    Mat4f expected = mat4f(
        -1.0f,  2.0f, -3.0f,  4.0f,
         1.0f, -2.0f,  3.0f, -4.0f,
        -1.0f,  2.0f, -3.0f,  4.0f,
         1.0f, -2.0f,  3.0f, -4.0f
    );
    // clang-format on

    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_neg(m), expected));
}

static void test_add_inplace(TestContext* ctx)
{
    Mat4f a = mat4f_identity();
    mat4f_add_inplace(&a, mat4f_identity());

    TEST_ASSERT_TRUE(ctx, mat4f_eq(a, mat4f_scale(mat4f_identity(), 2.0f)));
}

static void test_subtract_inplace(TestContext* ctx)
{
    Mat4f a = mat4f_scale(mat4f_identity(), 2.0f);
    mat4f_sub_inplace(&a, mat4f_identity());

    TEST_ASSERT_TRUE(ctx, mat4f_eq(a, mat4f_identity()));
}

static void test_scale_inplace(TestContext* ctx)
{
    Mat4f a = mat4f_identity();
    mat4f_scale_inplace(&a, 5.0f);

    TEST_ASSERT_TRUE(ctx, mat4f_eq(a, mat4f_scale(mat4f_identity(), 5.0f)));
}

static void test_negate_inplace(TestContext* ctx)
{
    Mat4f a = mat4f_scale(mat4f_identity(), 2.0f);
    mat4f_neg_inplace(&a);

    TEST_ASSERT_TRUE(ctx, mat4f_eq(a, mat4f_scale(mat4f_identity(), -2.0f)));
}

/*
 * Matrix operations
 */

static void test_multiply_by_identity(TestContext* ctx)
{
    Mat4f m = mat4f_translation(vec3f(1.0f, 2.0f, 3.0f));

    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_mul(m, mat4f_identity()), m));
    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_mul(mat4f_identity(), m), m));
}

static void test_multiply_translation_and_scale(TestContext* ctx)
{
    Mat4f translation = mat4f_translation(vec3f(1.0f, 2.0f, 3.0f));
    Mat4f scale = mat4f_scale_xyz(vec3f(2.0f, 2.0f, 2.0f));

    // clang-format off
    Mat4f expected = mat4f(
        2.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 2.0f, 0.0f, 2.0f,
        0.0f, 0.0f, 2.0f, 3.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_mul(translation, scale), expected));
}

static void test_multiply_inplace(TestContext* ctx)
{
    Mat4f m = mat4f_translation(vec3f(1.0f, 2.0f, 3.0f));
    mat4f_mul_inplace(&m, mat4f_identity());

    TEST_ASSERT_TRUE(ctx, mat4f_eq(m, mat4f_translation(vec3f(1.0f, 2.0f, 3.0f))));
}

static void test_transpose(TestContext* ctx)
{
    // clang-format off
    Mat4f m = mat4f(
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    );
    Mat4f expected = mat4f(
        1.0f, 5.0f,  9.0f, 13.0f,
        2.0f, 6.0f, 10.0f, 14.0f,
        3.0f, 7.0f, 11.0f, 15.0f,
        4.0f, 8.0f, 12.0f, 16.0f
    );
    // clang-format on

    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_transpose(m), expected));
}

static void test_transpose_twice_returns_original(TestContext* ctx)
{
    Mat4f m = mat4f_translation(vec3f(1.0f, 2.0f, 3.0f));

    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_transpose(mat4f_transpose(m)), m));
}

static void test_transpose_inplace(TestContext* ctx)
{
    // clang-format off
    Mat4f m = mat4f(
        1.0f, 2.0f, 0.0f, 0.0f,
        3.0f, 4.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    Mat4f expected = mat4f(
        1.0f, 3.0f, 0.0f, 0.0f,
        2.0f, 4.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    mat4f_transpose_inplace(&m);

    TEST_ASSERT_TRUE(ctx, mat4f_eq(m, expected));
}

static void test_determinant_identity(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, mat4f_determinant(mat4f_identity()), 1.0f, EPS);
}

static void test_determinant_scale_matrix(TestContext* ctx)
{
    Mat4f m = mat4f_scale_xyz(vec3f(2.0f, 3.0f, 4.0f));

    TEST_ASSERT_FLOAT_EQ(ctx, mat4f_determinant(m), 24.0f, EPS);
}

static void test_determinant_singular_matrix_is_zero(TestContext* ctx)
{
    // clang-format off
    Mat4f m = mat4f(
        1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    TEST_ASSERT_FLOAT_EQ(ctx, mat4f_determinant(m), 0.0f, EPS);
}

/*
 * Vector transformation
 */

static void test_mul_vec3_translates_point(TestContext* ctx)
{
    Mat4f m = mat4f_translation(vec3f(1.0f, 2.0f, 3.0f));
    Vec3f r = mat4f_mul_vec3(m, vec3f(0.0f, 0.0f, 0.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(1.0f, 2.0f, 3.0f), EPS));
}

static void test_mul_vec3_direction_ignores_translation(TestContext* ctx)
{
    Mat4f m = mat4f_translation(vec3f(1.0f, 2.0f, 3.0f));
    Vec3f r = mat4f_mul_vec3_direction(m, vec3f(5.0f, 6.0f, 7.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(5.0f, 6.0f, 7.0f), EPS));
}

static void test_mul_vec3_direction_applies_rotation(TestContext* ctx)
{
    Mat4f m = mat4f_rotation_z(MATH_PI / 2.0f);
    Vec3f r = mat4f_mul_vec3_direction(m, vec3f(1.0f, 0.0f, 0.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(0.0f, 1.0f, 0.0f), EPS));
}

static void test_mul_vec4_with_homogeneous_component(TestContext* ctx)
{
    Mat4f m = mat4f_translation(vec3f(1.0f, 2.0f, 3.0f));
    Vec4f r = mat4f_mul_vec4(m, vec4f(0.0f, 0.0f, 0.0f, 2.0f));

    TEST_ASSERT_TRUE(ctx, vec4f_eq_eps(r, vec4f(2.0f, 4.0f, 6.0f, 2.0f), EPS));
}

static void test_mul_vec3_perspective_divides_by_w(TestContext* ctx)
{
    Mat4f m = mat4f_perspective(MATH_PI / 2.0f, 1.0f, 1.0f, 100.0f);
    Vec3f r = mat4f_mul_vec3_perspective(m, vec3f(0.0f, 0.0f, -1.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(0.0f, 0.0f, -1.0f), EPS));
}

/*
 * Comparison
 */

static void test_equals_exact_equal(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_identity(), mat4f_identity()));
}

static void test_equals_exact_different(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, mat4f_eq(mat4f_identity(), mat4f_scale(mat4f_identity(), 1.0001f)));
}

static void test_equals_epsilon_within(TestContext* ctx)
{
    Mat4f a = mat4f_identity();
    Mat4f b = a;
    b.m00 += 1e-7f;

    TEST_ASSERT_TRUE(ctx, mat4f_eq_eps(a, b, 1e-6f));
}

static void test_equals_epsilon_outside(TestContext* ctx)
{
    Mat4f a = mat4f_identity();
    Mat4f b = a;
    b.m00 += 1e-4f;

    TEST_ASSERT_FALSE(ctx, mat4f_eq_eps(a, b, 1e-6f));
}

/*
 * Transformation matrices
 */

static void test_translation_matrix(TestContext* ctx)
{
    // clang-format off
    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_translation(vec3f(1.0f, 2.0f, 3.0f)), mat4f(
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 2.0f,
        0.0f, 0.0f, 1.0f, 3.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    )));
    // clang-format on
}

static void test_scale_xyz_matrix(TestContext* ctx)
{
    // clang-format off
    TEST_ASSERT_TRUE(ctx, mat4f_eq(mat4f_scale_xyz(vec3f(2.0f, 3.0f, 4.0f)), mat4f(
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 3.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 4.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    )));
    // clang-format on
}

static void test_orthographic_maps_corners_to_ndc(TestContext* ctx)
{
    Mat4f m = mat4f_orthographic(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 3.0f);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(mat4f_mul_vec3(m, vec3f(-1.0f, -1.0f, -1.0f)),
                                       vec3f(-1.0f, -1.0f, -1.0f), EPS));
    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(mat4f_mul_vec3(m, vec3f(1.0f, 1.0f, -3.0f)),
                                       vec3f(1.0f, 1.0f, 1.0f), EPS));
}

static void test_rotation_x_sincos_quarter_turn(TestContext* ctx)
{
    Mat4f m = mat4f_rotation_x_sincos(1.0f, 0.0f);
    Vec3f r = mat4f_mul_vec3(m, vec3f(0.0f, 1.0f, 0.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(0.0f, 0.0f, 1.0f), EPS));
}

static void test_rotation_y_sincos_quarter_turn(TestContext* ctx)
{
    Mat4f m = mat4f_rotation_y_sincos(1.0f, 0.0f);
    Vec3f r = mat4f_mul_vec3(m, vec3f(0.0f, 0.0f, 1.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(1.0f, 0.0f, 0.0f), EPS));
}

static void test_rotation_z_sincos_quarter_turn(TestContext* ctx)
{
    Mat4f m = mat4f_rotation_z_sincos(1.0f, 0.0f);
    Vec3f r = mat4f_mul_vec3(m, vec3f(1.0f, 0.0f, 0.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(0.0f, 1.0f, 0.0f), EPS));
}

static void test_rotation_x_matches_sincos(TestContext* ctx)
{
    Mat4f a = mat4f_rotation_x(MATH_PI / 2.0f);
    Mat4f b = mat4f_rotation_x_sincos(1.0f, 0.0f);

    TEST_ASSERT_TRUE(ctx, mat4f_eq_eps(a, b, EPS));
}

static void test_rotation_y_matches_sincos(TestContext* ctx)
{
    Mat4f a = mat4f_rotation_y(MATH_PI / 2.0f);
    Mat4f b = mat4f_rotation_y_sincos(1.0f, 0.0f);

    TEST_ASSERT_TRUE(ctx, mat4f_eq_eps(a, b, EPS));
}

static void test_rotation_z_matches_sincos(TestContext* ctx)
{
    Mat4f a = mat4f_rotation_z(MATH_PI / 2.0f);
    Mat4f b = mat4f_rotation_z_sincos(1.0f, 0.0f);

    TEST_ASSERT_TRUE(ctx, mat4f_eq_eps(a, b, EPS));
}

static void test_rotation_axis_z_matches_rotation_z(TestContext* ctx)
{
    Mat4f a = mat4f_rotation_axis(vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    Mat4f b = mat4f_rotation_z(MATH_PI / 2.0f);

    TEST_ASSERT_TRUE(ctx, mat4f_eq_eps(a, b, EPS));
}

static void test_rotation_axis_matches_quatf_rotation(TestContext* ctx)
{
    Vec3f axis = vec3f_normalize(vec3f(1.0f, 2.0f, 3.0f));
    const f32 angle = 0.7f;

    Mat4f m = mat4f_rotation_axis(axis, angle);
    Quatf q = quatf_from_axis_angle(axis, angle);

    Vec3f v = vec3f(1.0f, 0.0f, 0.0f);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(mat4f_mul_vec3(m, v), quatf_rotate_vec3f(q, v), EPS));
}

static void test_perspective_matrix_values(TestContext* ctx)
{
    Mat4f m = mat4f_perspective(MATH_PI / 2.0f, 2.0f, 1.0f, 3.0f);

    TEST_ASSERT_FLOAT_EQ(ctx, m.m00, 0.5f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, m.m11, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, m.m22, -2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, m.m23, -3.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, m.m32, -1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, m.m33, 0.0f, EPS);
}

static void test_rotation_quatf_identity_is_identity_matrix(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx,
                     mat4f_eq_eps(mat4f_rotation_quatf(quatf_identity()), mat4f_identity(), EPS));
}

static void test_rotation_quatf_quarter_turn_matches_rotation_z(TestContext* ctx)
{
    Quatf q = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);

    TEST_ASSERT_TRUE(ctx,
                     mat4f_eq_eps(mat4f_rotation_quatf(q), mat4f_rotation_z(MATH_PI / 2.0f), EPS));
}

static void test_trs_with_identity_rotation_and_scale_is_translation(TestContext* ctx)
{
    Mat4f m = mat4f_trs(vec3f(1.0f, 2.0f, 3.0f), quatf_identity(), vec3f_one());

    TEST_ASSERT_TRUE(ctx, mat4f_eq_eps(m, mat4f_translation(vec3f(1.0f, 2.0f, 3.0f)), EPS));
}

static void test_get_translation(TestContext* ctx)
{
    Mat4f m = mat4f_trs(vec3f(1.0f, 2.0f, 3.0f), quatf_identity(), vec3f(2.0f, 3.0f, 4.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(mat4f_get_translation(m), vec3f(1.0f, 2.0f, 3.0f), EPS));
}

static void test_get_scale(TestContext* ctx)
{
    Mat4f m = mat4f_trs(vec3f(1.0f, 2.0f, 3.0f), quatf_identity(), vec3f(2.0f, 3.0f, 4.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(mat4f_get_scale(m), vec3f(2.0f, 3.0f, 4.0f), EPS_LOOSE));
}

/*
 * Inverse
 */

static void test_try_inverse_success_round_trip(TestContext* ctx)
{
    Mat4f m =
        mat4f_trs(vec3f(1.0f, 2.0f, 3.0f), quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), 0.6f),
                  vec3f(2.0f, 2.0f, 2.0f));

    Mat4f inverse;
    TEST_ASSERT_TRUE(ctx, mat4f_try_inverse(m, &inverse));
    TEST_ASSERT_TRUE(ctx, mat4f_eq_eps(mat4f_mul(m, inverse), mat4f_identity(), EPS_LOOSE));
}

static void test_try_inverse_singular_returns_false(TestContext* ctx)
{
    // clang-format off
    Mat4f m = mat4f(
        1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    Mat4f result = mat4f_identity();
    TEST_ASSERT_FALSE(ctx, mat4f_try_inverse(m, &result));
    TEST_ASSERT_TRUE(ctx, mat4f_eq(result, mat4f_identity()));
}

static void test_try_inverse_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, mat4f_try_inverse(mat4f_identity(), NULL));
}

static void test_try_inverse_non_finite_returns_false(TestContext* ctx)
{
    Mat4f m = mat4f_identity();
    m.m00 = NAN;

    Mat4f result;
    TEST_ASSERT_FALSE(ctx, mat4f_try_inverse(m, &result));
}

static void test_inverse_matches_try_inverse(TestContext* ctx)
{
    Mat4f m = mat4f_translation(vec3f(1.0f, 2.0f, 3.0f));

    Mat4f expected;
    mat4f_try_inverse(m, &expected);

    TEST_ASSERT_TRUE(ctx, mat4f_eq_eps(mat4f_inverse(m), expected, EPS));
}

/*
 * Normal matrix
 */

static void test_try_normal_matrix_success(TestContext* ctx)
{
    Mat4f m = mat4f_scale_xyz(vec3f(2.0f, 4.0f, 8.0f));

    Mat4f normal_matrix;
    TEST_ASSERT_TRUE(ctx, mat4f_try_normal_matrix(m, &normal_matrix));
    TEST_ASSERT_TRUE(ctx, mat4f_eq_eps(normal_matrix, mat4f_normal_matrix(m), EPS));
}

static void test_try_normal_matrix_singular_returns_false(TestContext* ctx)
{
    Mat4f m = mat4f_scale_xyz(vec3f(0.0f, 1.0f, 1.0f));

    Mat4f result;
    TEST_ASSERT_FALSE(ctx, mat4f_try_normal_matrix(m, &result));
}

static void test_try_normal_matrix_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, mat4f_try_normal_matrix(mat4f_identity(), NULL));
}

static void test_normal_matrix_transforms_normal_correctly(TestContext* ctx)
{
    Mat4f model = mat4f_scale_xyz(vec3f(1.0f, 2.0f, 1.0f));
    Mat4f normal_matrix = mat4f_normal_matrix(model);

    // A normal along the non-scaled axis should remain unit length and axis-aligned.
    Vec3f transformed = mat4f_mul_vec3_direction(normal_matrix, vec3f(1.0f, 0.0f, 0.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(vec3f_normalize(transformed), vec3f(1.0f, 0.0f, 0.0f), EPS));
}

/*
 * Perspective divide (try)
 */

static void test_try_mul_vec3_perspective_success(TestContext* ctx)
{
    Mat4f m = mat4f_perspective(MATH_PI / 2.0f, 1.0f, 1.0f, 100.0f);

    Vec3f result;
    TEST_ASSERT_TRUE(ctx, mat4f_try_mul_vec3_perspective(m, vec3f(0.0f, 0.0f, -1.0f), &result));
    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(result, vec3f(0.0f, 0.0f, -1.0f), EPS));
}

static void test_try_mul_vec3_perspective_null_returns_false(TestContext* ctx)
{
    Mat4f m = mat4f_perspective(MATH_PI / 2.0f, 1.0f, 1.0f, 100.0f);

    TEST_ASSERT_FALSE(ctx, mat4f_try_mul_vec3_perspective(m, vec3f(0.0f, 0.0f, -1.0f), NULL));
}

static void test_try_mul_vec3_perspective_non_finite_matrix_returns_false(TestContext* ctx)
{
    Mat4f m = mat4f_identity();
    m.m00 = NAN;

    Vec3f result;
    TEST_ASSERT_FALSE(ctx, mat4f_try_mul_vec3_perspective(m, vec3f(0.0f, 0.0f, -1.0f), &result));
}

static void test_try_mul_vec3_perspective_near_zero_w_returns_false(TestContext* ctx)
{
    // clang-format off
    Mat4f m = mat4f(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    );
    // clang-format on

    Vec3f result;
    TEST_ASSERT_FALSE(ctx, mat4f_try_mul_vec3_perspective(m, vec3f(0.0f, 0.0f, 0.0f), &result));
}

/*
 * Quaternion conversion
 */

static void test_to_quatf_identity_matrix_is_identity_quaternion(TestContext* ctx)
{
    Quatf q = mat4f_to_quatf(mat4f_identity());

    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(q, quatf_identity(), EPS));
}

static void test_to_quatf_round_trip_from_rotation_matrix(TestContext* ctx)
{
    Quatf original = quatf_from_axis_angle(vec3f_normalize(vec3f(1.0f, 2.0f, 3.0f)), 0.8f);
    Mat4f m = mat4f_rotation_quatf(original);

    Quatf recovered = mat4f_to_quatf(m);

    Vec3f v = vec3f(0.0f, 1.0f, 0.0f);

    TEST_ASSERT_TRUE(
        ctx, vec3f_eq_eps(quatf_rotate_vec3f(original, v), quatf_rotate_vec3f(recovered, v), EPS));
}

static void test_to_quatf_ignores_non_uniform_scale(TestContext* ctx)
{
    Quatf original = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), 0.5f);
    Mat4f m = mat4f_trs(vec3f_zero(), original, vec3f(2.0f, 3.0f, 4.0f));

    Quatf recovered = mat4f_to_quatf(m);

    Vec3f v = vec3f(1.0f, 0.0f, 0.0f);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(quatf_rotate_vec3f(original, v),
                                       quatf_rotate_vec3f(recovered, v), EPS_LOOSE));
}

/*
 * Look at
 */

static void test_look_at_maps_eye_to_origin(TestContext* ctx)
{
    Vec3f eye = vec3f(0.0f, 0.0f, 5.0f);
    Vec3f target = vec3f(0.0f, 0.0f, 0.0f);
    Vec3f up = vec3f(0.0f, 1.0f, 0.0f);

    Mat4f view = mat4f_look_at(eye, target, up);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(mat4f_mul_vec3(view, eye), vec3f_zero(), EPS_LOOSE));
}

static void test_look_at_maps_target_onto_negative_z(TestContext* ctx)
{
    Vec3f eye = vec3f(0.0f, 0.0f, 5.0f);
    Vec3f target = vec3f(0.0f, 0.0f, 0.0f);
    Vec3f up = vec3f(0.0f, 1.0f, 0.0f);

    Mat4f view = mat4f_look_at(eye, target, up);
    Vec3f transformed_target = mat4f_mul_vec3(view, target);

    TEST_ASSERT_FLOAT_EQ(ctx, transformed_target.x, 0.0f, EPS_LOOSE);
    TEST_ASSERT_FLOAT_EQ(ctx, transformed_target.y, 0.0f, EPS_LOOSE);
    TEST_ASSERT_FLOAT_EQ(ctx, transformed_target.z, -vec3f_dist(eye, target), EPS_LOOSE);
}

/*
 * Suite registration
 */

static TestRegistry reg;

static void setup(void)
{
    describe("construction")
    {
        test(test_create);
        test(test_zero);
        test(test_identity);
    }
    describe("helpers")
    {
        test(test_is_finite_true_for_finite_matrix);
        test(test_is_finite_false_for_nan_component);
        test(test_is_finite_false_for_infinite_component);
    }
    describe("arithmetic")
    {
        test(test_add);
        test(test_subtract);
        test(test_scale);
        test(test_negate);
        test(test_add_inplace);
        test(test_subtract_inplace);
        test(test_scale_inplace);
        test(test_negate_inplace);
    }
    describe("matrix operations")
    {
        test(test_multiply_by_identity);
        test(test_multiply_translation_and_scale);
        test(test_multiply_inplace);
        test(test_transpose);
        test(test_transpose_twice_returns_original);
        test(test_transpose_inplace);
        test(test_determinant_identity);
        test(test_determinant_scale_matrix);
        test(test_determinant_singular_matrix_is_zero);
    }
    describe("vector transformation")
    {
        test(test_mul_vec3_translates_point);
        test(test_mul_vec3_direction_ignores_translation);
        test(test_mul_vec3_direction_applies_rotation);
        test(test_mul_vec4_with_homogeneous_component);
        test(test_mul_vec3_perspective_divides_by_w);
    }
    describe("eq")
    {
        test(test_equals_exact_equal);
        test(test_equals_exact_different);
        test(test_equals_epsilon_within);
        test(test_equals_epsilon_outside);
    }
    describe("transformation matrices")
    {
        test(test_translation_matrix);
        test(test_scale_xyz_matrix);
        test(test_orthographic_maps_corners_to_ndc);
        test(test_rotation_x_sincos_quarter_turn);
        test(test_rotation_y_sincos_quarter_turn);
        test(test_rotation_z_sincos_quarter_turn);
        test(test_rotation_x_matches_sincos);
        test(test_rotation_y_matches_sincos);
        test(test_rotation_z_matches_sincos);
        test(test_rotation_axis_z_matches_rotation_z);
        test(test_rotation_axis_matches_quatf_rotation);
        test(test_perspective_matrix_values);
        test(test_rotation_quatf_identity_is_identity_matrix);
        test(test_rotation_quatf_quarter_turn_matches_rotation_z);
        test(test_trs_with_identity_rotation_and_scale_is_translation);
        test(test_get_translation);
        test(test_get_scale);
    }
    describe("inverse")
    {
        test(test_try_inverse_success_round_trip);
        test(test_try_inverse_singular_returns_false);
        test(test_try_inverse_null_returns_false);
        test(test_try_inverse_non_finite_returns_false);
        test(test_inverse_matches_try_inverse);
    }
    describe("normal matrix")
    {
        test(test_try_normal_matrix_success);
        test(test_try_normal_matrix_singular_returns_false);
        test(test_try_normal_matrix_null_returns_false);
        test(test_normal_matrix_transforms_normal_correctly);
    }
    describe("try_mul_vec3_perspective")
    {
        test(test_try_mul_vec3_perspective_success);
        test(test_try_mul_vec3_perspective_null_returns_false);
        test(test_try_mul_vec3_perspective_non_finite_matrix_returns_false);
        test(test_try_mul_vec3_perspective_near_zero_w_returns_false);
    }
    describe("quaternion conversion")
    {
        test(test_to_quatf_identity_matrix_is_identity_quaternion);
        test(test_to_quatf_round_trip_from_rotation_matrix);
        test(test_to_quatf_ignores_non_uniform_scale);
    }
    describe("look_at")
    {
        test(test_look_at_maps_eye_to_origin);
        test(test_look_at_maps_target_onto_negative_z);
    }
}

const TestSuite mat4f_suite = { "mat4f", setup, &reg };
