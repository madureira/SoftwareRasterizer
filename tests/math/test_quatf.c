#include "test.h"

#include "math/quatf.h"

#define EPS 1e-5f

/*
 * Construction
 */

static void test_create(TestContext* ctx)
{
    Quatf q = quatf(1.0f, 2.0f, 3.0f, 4.0f);
    TEST_ASSERT(ctx, q.x == 1.0f);
    TEST_ASSERT(ctx, q.y == 2.0f);
    TEST_ASSERT(ctx, q.z == 3.0f);
    TEST_ASSERT(ctx, q.w == 4.0f);
}

static void test_zero(TestContext* ctx)
{
    Quatf q = quatf_zero();
    TEST_ASSERT(ctx, q.x == 0.0f);
    TEST_ASSERT(ctx, q.y == 0.0f);
    TEST_ASSERT(ctx, q.z == 0.0f);
    TEST_ASSERT(ctx, q.w == 0.0f);
}

static void test_identity(TestContext* ctx)
{
    Quatf q = quatf_identity();
    TEST_ASSERT(ctx, q.x == 0.0f);
    TEST_ASSERT(ctx, q.y == 0.0f);
    TEST_ASSERT(ctx, q.z == 0.0f);
    TEST_ASSERT(ctx, q.w == 1.0f);
}

/*
 * Arithmetic
 */

static void test_add(TestContext* ctx)
{
    Quatf r = quatf_add(quatf(1.0f, 2.0f, 3.0f, 4.0f), quatf(5.0f, 6.0f, 7.0f, 8.0f));
    TEST_ASSERT(ctx, r.x == 6.0f);
    TEST_ASSERT(ctx, r.y == 8.0f);
    TEST_ASSERT(ctx, r.z == 10.0f);
    TEST_ASSERT(ctx, r.w == 12.0f);
}

static void test_subtract(TestContext* ctx)
{
    Quatf r = quatf_sub(quatf(5.0f, 6.0f, 7.0f, 8.0f), quatf(1.0f, 2.0f, 3.0f, 4.0f));
    TEST_ASSERT(ctx, r.x == 4.0f);
    TEST_ASSERT(ctx, r.y == 4.0f);
    TEST_ASSERT(ctx, r.z == 4.0f);
    TEST_ASSERT(ctx, r.w == 4.0f);
}

static void test_scale(TestContext* ctx)
{
    Quatf r = quatf_scale(quatf(1.0f, 2.0f, 3.0f, 4.0f), 2.0f);
    TEST_ASSERT(ctx, r.x == 2.0f);
    TEST_ASSERT(ctx, r.y == 4.0f);
    TEST_ASSERT(ctx, r.z == 6.0f);
    TEST_ASSERT(ctx, r.w == 8.0f);
}

static void test_negate(TestContext* ctx)
{
    Quatf r = quatf_neg(quatf(1.0f, -2.0f, 3.0f, -4.0f));
    TEST_ASSERT(ctx, r.x == -1.0f);
    TEST_ASSERT(ctx, r.y == 2.0f);
    TEST_ASSERT(ctx, r.z == -3.0f);
    TEST_ASSERT(ctx, r.w == 4.0f);
}

/*
 * Dot product / length
 */

static void test_dot_orthogonal(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(
        ctx, quatf_dot(quatf(1.0f, 0.0f, 0.0f, 0.0f), quatf(0.0f, 1.0f, 0.0f, 0.0f)), 0.0f, EPS);
}

static void test_dot_general(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(
        ctx, quatf_dot(quatf(1.0f, 2.0f, 3.0f, 4.0f), quatf(1.0f, 2.0f, 3.0f, 4.0f)), 30.0f, EPS);
}

static void test_length_squared(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len_sq(quatf(2.0f, 3.0f, 6.0f, 0.0f)), 49.0f, EPS);
}

static void test_length(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(quatf(2.0f, 3.0f, 6.0f, 0.0f)), 7.0f, EPS);
}

/*
 * Conjugate
 */

static void test_conjugate(TestContext* ctx)
{
    Quatf r = quatf_conjugate(quatf(1.0f, 2.0f, 3.0f, 4.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, -1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, -2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, -3.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.w, 4.0f, EPS);
}

/*
 * Multiplication (Hamilton product)
 */

static void test_multiply_i_times_i_equals_negative_one(TestContext* ctx)
{
    Quatf i = quatf(1.0f, 0.0f, 0.0f, 0.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_multiply(i, i), quatf(0.0f, 0.0f, 0.0f, -1.0f), EPS));
}

static void test_multiply_j_times_j_equals_negative_one(TestContext* ctx)
{
    Quatf j = quatf(0.0f, 1.0f, 0.0f, 0.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_multiply(j, j), quatf(0.0f, 0.0f, 0.0f, -1.0f), EPS));
}

static void test_multiply_k_times_k_equals_negative_one(TestContext* ctx)
{
    Quatf k = quatf(0.0f, 0.0f, 1.0f, 0.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_multiply(k, k), quatf(0.0f, 0.0f, 0.0f, -1.0f), EPS));
}

static void test_multiply_i_times_j_equals_k(TestContext* ctx)
{
    Quatf i = quatf(1.0f, 0.0f, 0.0f, 0.0f);
    Quatf j = quatf(0.0f, 1.0f, 0.0f, 0.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_multiply(i, j), quatf(0.0f, 0.0f, 1.0f, 0.0f), EPS));
}

static void test_multiply_j_times_k_equals_i(TestContext* ctx)
{
    Quatf j = quatf(0.0f, 1.0f, 0.0f, 0.0f);
    Quatf k = quatf(0.0f, 0.0f, 1.0f, 0.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_multiply(j, k), quatf(1.0f, 0.0f, 0.0f, 0.0f), EPS));
}

static void test_multiply_k_times_i_equals_j(TestContext* ctx)
{
    Quatf k = quatf(0.0f, 0.0f, 1.0f, 0.0f);
    Quatf i = quatf(1.0f, 0.0f, 0.0f, 0.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_multiply(k, i), quatf(0.0f, 1.0f, 0.0f, 0.0f), EPS));
}

static void test_multiply_is_not_commutative(TestContext* ctx)
{
    Quatf i = quatf(1.0f, 0.0f, 0.0f, 0.0f);
    Quatf k = quatf(0.0f, 0.0f, 1.0f, 0.0f);
    TEST_ASSERT_FALSE(ctx, quatf_eq_eps(quatf_multiply(i, k), quatf_multiply(k, i), EPS));
}

static void test_multiply_by_identity_is_unchanged(TestContext* ctx)
{
    Quatf q = quatf_normalize(quatf(1.0f, 2.0f, 3.0f, 4.0f));
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_multiply(q, quatf_identity()), q, EPS));
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_multiply(quatf_identity(), q), q, EPS));
}

/*
 * Normalization
 */

static void test_normalize_unit_length(TestContext* ctx)
{
    Quatf q = quatf_normalize(quatf(1.0f, 2.0f, 3.0f, 4.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(q), 1.0f, EPS);
}

static void test_try_normalize_success(TestContext* ctx)
{
    Quatf result = quatf_zero();
    TEST_ASSERT_TRUE(ctx, quatf_try_normalize(quatf(1.0f, 2.0f, 3.0f, 4.0f), &result));
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(result), 1.0f, EPS);
}

static void test_try_normalize_near_zero_returns_false(TestContext* ctx)
{
    Quatf result = quatf_identity();
    TEST_ASSERT_FALSE(ctx, quatf_try_normalize(quatf_zero(), &result));
    TEST_ASSERT_TRUE(ctx, quatf_eq(result, quatf_identity()));
}

static void test_try_normalize_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, quatf_try_normalize(quatf_identity(), NULL));
}

/*
 * Inverse
 */

static void test_inverse_of_unit_quaternion_equals_conjugate(TestContext* ctx)
{
    Quatf q = quatf_normalize(quatf(1.0f, 2.0f, 3.0f, 4.0f));
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_inverse(q), quatf_conjugate(q), EPS));
}

static void test_multiply_by_inverse_yields_identity(TestContext* ctx)
{
    Quatf q = quatf(1.0f, 2.0f, 3.0f, 4.0f);
    Quatf inverse = quatf_inverse(q);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_multiply(q, inverse), quatf_identity(), EPS));
}

static void test_try_inverse_near_zero_returns_false(TestContext* ctx)
{
    Quatf result = quatf_identity();
    TEST_ASSERT_FALSE(ctx, quatf_try_inverse(quatf_zero(), &result));
    TEST_ASSERT_TRUE(ctx, quatf_eq(result, quatf_identity()));
}

static void test_try_inverse_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, quatf_try_inverse(quatf_identity(), NULL));
}

/*
 * From axis-angle
 */

static void test_from_axis_angle_result_is_unit_length(TestContext* ctx)
{
    Quatf q = quatf_from_axis_angle(vec3f(1.0f, 2.0f, 3.0f), 1.234f);
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(q), 1.0f, EPS);
}

static void test_from_axis_angle_normalizes_axis(TestContext* ctx)
{
    Quatf a = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    Quatf b = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 5.0f), MATH_PI / 2.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

static void test_from_axis_angle_sincos_matches_from_axis_angle(TestContext* ctx)
{
    const f32 angle = MATH_PI / 3.0f;
    const Vec3f axis = vec3f(0.0f, 1.0f, 0.0f);

    Quatf a = quatf_from_axis_angle(axis, angle);
    Quatf b = quatf_from_axis_angle_sincos(axis, sinf(angle * 0.5f), cosf(angle * 0.5f));

    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

/*
 * From Euler angles
 */

static void test_from_euler_pure_pitch_matches_axis_angle_x(TestContext* ctx)
{
    const f32 angle = MATH_PI / 4.0f;
    Quatf a = quatf_from_euler(angle, 0.0f, 0.0f);
    Quatf b = quatf_from_axis_angle(vec3f(1.0f, 0.0f, 0.0f), angle);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

static void test_from_euler_pure_yaw_matches_axis_angle_y(TestContext* ctx)
{
    const f32 angle = MATH_PI / 4.0f;
    Quatf a = quatf_from_euler(0.0f, angle, 0.0f);
    Quatf b = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), angle);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

static void test_from_euler_pure_roll_matches_axis_angle_z(TestContext* ctx)
{
    const f32 angle = MATH_PI / 4.0f;
    Quatf a = quatf_from_euler(0.0f, 0.0f, angle);
    Quatf b = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), angle);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

/*
 * To axis-angle
 */

static void test_to_axis_angle_round_trip(TestContext* ctx)
{
    const Vec3f axis_in = vec3f_normalize(vec3f(1.0f, 2.0f, 3.0f));
    const f32 angle_in = MATH_PI / 3.0f;

    Quatf q = quatf_from_axis_angle(axis_in, angle_in);

    Vec3f axis_out;
    f32 angle_out;
    quatf_to_axis_angle(q, &axis_out, &angle_out);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(axis_out, axis_in, EPS));
    TEST_ASSERT_FLOAT_EQ(ctx, angle_out, angle_in, EPS);
}

static void test_to_axis_angle_zero_rotation_defaults_to_x_axis(TestContext* ctx)
{
    Vec3f axis;
    f32 angle;
    quatf_to_axis_angle(quatf_identity(), &axis, &angle);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(axis, vec3f(1.0f, 0.0f, 0.0f), EPS));
    TEST_ASSERT_FLOAT_EQ(ctx, angle, 0.0f, EPS);
}

/*
 * From orthonormal basis
 */

static void test_from_orthonormal_basis_identity(TestContext* ctx)
{
    Quatf q = quatf_from_orthonormal_basis(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f),
                                           vec3f(0.0f, 0.0f, 1.0f));
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(q, quatf_identity(), EPS));
}

static void test_from_orthonormal_basis_quarter_turn_matches_axis_angle(TestContext* ctx)
{
    Quatf a = quatf_from_orthonormal_basis(vec3f(0.0f, 1.0f, 0.0f), vec3f(-1.0f, 0.0f, 0.0f),
                                           vec3f(0.0f, 0.0f, 1.0f));
    Quatf b = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

static void test_from_orthonormal_basis_round_trip_matches_source_rotation(TestContext* ctx)
{
    const Quatf q = quatf_from_axis_angle(vec3f_normalize(vec3f(1.0f, 2.0f, 3.0f)), 0.7f);

    const Vec3f right = quatf_rotate_vec3f(q, vec3f(1.0f, 0.0f, 0.0f));
    const Vec3f up = quatf_rotate_vec3f(q, vec3f(0.0f, 1.0f, 0.0f));
    const Vec3f back = quatf_rotate_vec3f(q, vec3f(0.0f, 0.0f, 1.0f));

    const Quatf r = quatf_from_orthonormal_basis(right, up, back);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(quatf_rotate_vec3f(r, vec3f(1.0f, 0.0f, 0.0f)), right, EPS));
    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(quatf_rotate_vec3f(r, vec3f(0.0f, 1.0f, 0.0f)), up, EPS));
}

static void test_from_orthonormal_basis_result_is_unit_length(TestContext* ctx)
{
    Quatf q = quatf_from_orthonormal_basis(vec3f(0.0f, 1.0f, 0.0f), vec3f(-1.0f, 0.0f, 0.0f),
                                           vec3f(0.0f, 0.0f, 1.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(q), 1.0f, EPS);
}

/*
 * Look rotation
 */

static void test_look_rotation_default_forward_and_up_is_identity(TestContext* ctx)
{
    Quatf q = quatf_look_rotation(vec3f(0.0f, 0.0f, -1.0f), vec3f(0.0f, 1.0f, 0.0f));
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(q, quatf_identity(), EPS));
}

static void test_look_rotation_result_is_unit_length(TestContext* ctx)
{
    Quatf q = quatf_look_rotation(vec3f(1.0f, 2.0f, 3.0f), vec3f(0.0f, 1.0f, 0.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(q), 1.0f, EPS);
}

static void test_look_rotation_rotates_local_forward_to_target(TestContext* ctx)
{
    const Vec3f forward = vec3f_normalize(vec3f(1.0f, 2.0f, 3.0f));
    Quatf q = quatf_look_rotation(forward, vec3f(0.0f, 1.0f, 0.0f));

    Vec3f rotated_forward = quatf_rotate_vec3f(q, vec3f(0.0f, 0.0f, -1.0f));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(rotated_forward, forward, EPS));
}

static void test_look_rotation_rotated_up_is_orthogonal_to_forward(TestContext* ctx)
{
    const Vec3f forward = vec3f_normalize(vec3f(1.0f, 2.0f, 3.0f));
    Quatf q = quatf_look_rotation(forward, vec3f(0.0f, 1.0f, 0.0f));

    Vec3f rotated_up = quatf_rotate_vec3f(q, vec3f(0.0f, 1.0f, 0.0f));

    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_dot(rotated_up, forward), 0.0f, EPS);
}

static void test_look_rotation_ignores_up_magnitude(TestContext* ctx)
{
    Quatf a = quatf_look_rotation(vec3f(1.0f, 2.0f, 3.0f), vec3f(0.0f, 1.0f, 0.0f));
    Quatf b = quatf_look_rotation(vec3f(1.0f, 2.0f, 3.0f), vec3f(0.0f, 5.0f, 0.0f));
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

static void test_look_rotation_quarter_turn_matches_axis_angle(TestContext* ctx)
{
    Quatf a = quatf_look_rotation(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f));
    Quatf b = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), -MATH_PI / 2.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

/*
 * From two vectors
 */

static void test_from_two_vectors_identical_is_identity(TestContext* ctx)
{
    Quatf q = quatf_from_two_vectors(vec3f(1.0f, 2.0f, 3.0f), vec3f(2.0f, 4.0f, 6.0f));
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(q, quatf_identity(), EPS));
}

static void test_from_two_vectors_quarter_turn_matches_axis_angle(TestContext* ctx)
{
    Quatf a = quatf_from_two_vectors(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f));
    Quatf b = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

static void test_from_two_vectors_rotates_from_onto_to(TestContext* ctx)
{
    const Vec3f from = vec3f(1.0f, 2.0f, 3.0f);
    const Vec3f to = vec3f(4.0f, -1.0f, 2.0f);

    Quatf q = quatf_from_two_vectors(from, to);

    Vec3f rotated = quatf_rotate_vec3f(q, vec3f_normalize(from));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(rotated, vec3f_normalize(to), EPS));
}

static void test_from_two_vectors_result_is_unit_length(TestContext* ctx)
{
    Quatf q = quatf_from_two_vectors(vec3f(1.0f, 2.0f, 3.0f), vec3f(4.0f, -1.0f, 2.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(q), 1.0f, EPS);
}

static void test_from_two_vectors_antiparallel_rotates_from_onto_to(TestContext* ctx)
{
    const Vec3f from = vec3f(1.0f, 0.0f, 0.0f);
    const Vec3f to = vec3f(-1.0f, 0.0f, 0.0f);

    Quatf q = quatf_from_two_vectors(from, to);

    Vec3f rotated = quatf_rotate_vec3f(q, vec3f_normalize(from));

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(rotated, vec3f_normalize(to), EPS));
}

static void test_from_two_vectors_antiparallel_result_is_unit_length(TestContext* ctx)
{
    Quatf q = quatf_from_two_vectors(vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, -1.0f, 0.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(q), 1.0f, EPS);
}

/*
 * Rotating vectors
 */

static void test_rotate_vec3f_identity_unchanged(TestContext* ctx)
{
    Vec3f v = vec3f(1.0f, 2.0f, 3.0f);
    Vec3f r = quatf_rotate_vec3f(quatf_identity(), v);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, v.x, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, v.y, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, v.z, EPS);
}

static void test_rotate_vec3f_quarter_turn_around_z(TestContext* ctx)
{
    Quatf q = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    Vec3f r = quatf_rotate_vec3f(q, vec3f(1.0f, 0.0f, 0.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_rotate_vec3f_quarter_turn_around_x(TestContext* ctx)
{
    Quatf q = quatf_from_axis_angle(vec3f(1.0f, 0.0f, 0.0f), MATH_PI / 2.0f);
    Vec3f r = quatf_rotate_vec3f(q, vec3f(0.0f, 1.0f, 0.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 1.0f, EPS);
}

static void test_rotate_vec3f_quarter_turn_around_y(TestContext* ctx)
{
    Quatf q = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), MATH_PI / 2.0f);
    Vec3f r = quatf_rotate_vec3f(q, vec3f(0.0f, 0.0f, 1.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

/*
 * Per-axis rotation helpers
 */

static void test_rotate_x_from_identity_matches_axis_angle(TestContext* ctx)
{
    const f32 angle = MATH_PI / 5.0f;
    Quatf a = quatf_rotate_x(quatf_identity(), angle);
    Quatf b = quatf_from_axis_angle(vec3f(1.0f, 0.0f, 0.0f), angle);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

static void test_rotate_y_from_identity_matches_axis_angle(TestContext* ctx)
{
    const f32 angle = MATH_PI / 5.0f;
    Quatf a = quatf_rotate_y(quatf_identity(), angle);
    Quatf b = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), angle);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

static void test_rotate_z_from_identity_matches_axis_angle(TestContext* ctx)
{
    const f32 angle = MATH_PI / 5.0f;
    Quatf a = quatf_rotate_z(quatf_identity(), angle);
    Quatf b = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), angle);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, EPS));
}

// Cross-checked against the independently implemented vec3f_rotate.
static void test_rotate_x_then_z_matches_vec3f_composition(TestContext* ctx)
{
    const f32 angle_x = MATH_PI / 2.0f;
    const f32 angle_z = MATH_PI / 3.0f;
    const Vec3f v = vec3f(0.3f, -0.7f, 1.1f);

    Quatf q = quatf_identity();
    q = quatf_rotate_x(q, angle_x);
    q = quatf_rotate_z(q, angle_z);

    Vec3f actual = quatf_rotate_vec3f(q, v);

    Vec3f expected = vec3f_rotate(vec3f_rotate(v, vec3f(0.0f, 0.0f, 1.0f), angle_z),
                                  vec3f(1.0f, 0.0f, 0.0f), angle_x);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(actual, expected, EPS));
}

static void test_rotate_x_inplace_matches_rotate_x(TestContext* ctx)
{
    const f32 angle = 0.9f;
    Quatf expected = quatf_rotate_x(quatf_identity(), angle);

    Quatf q = quatf_identity();
    quatf_rotate_x_inplace(&q, angle);

    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(q, expected, EPS));
}

/*
 * Interpolation
 */

static void test_lerp_at_zero(TestContext* ctx)
{
    Quatf from = quatf(1.0f, 2.0f, 3.0f, 4.0f);
    Quatf to = quatf(5.0f, 6.0f, 7.0f, 8.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq(quatf_lerp(from, to, 0.0f), from));
}

static void test_lerp_at_one(TestContext* ctx)
{
    Quatf from = quatf(1.0f, 2.0f, 3.0f, 4.0f);
    Quatf to = quatf(5.0f, 6.0f, 7.0f, 8.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq(quatf_lerp(from, to, 1.0f), to));
}

static void test_lerp_clamped_above_one(TestContext* ctx)
{
    Quatf to = quatf(5.0f, 6.0f, 7.0f, 8.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq(quatf_lerp_clamp(quatf(1.0f, 2.0f, 3.0f, 4.0f), to, 2.0f), to));
}

static void test_lerp_clamped_below_zero(TestContext* ctx)
{
    Quatf from = quatf(1.0f, 2.0f, 3.0f, 4.0f);
    Quatf to = quatf(5.0f, 6.0f, 7.0f, 8.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq(quatf_lerp_clamp(from, to, -1.0f), from));
}

static void test_slerp_at_zero_returns_from(TestContext* ctx)
{
    Quatf from = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), 0.0f);
    Quatf to = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_slerp(from, to, 0.0f), from, EPS));
}

static void test_slerp_at_one_returns_to(TestContext* ctx)
{
    Quatf from = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), 0.0f);
    Quatf to = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_slerp(from, to, 1.0f), to, EPS));
}

static void test_slerp_result_is_unit_length(TestContext* ctx)
{
    Quatf from = quatf_from_axis_angle(vec3f(1.0f, 0.0f, 0.0f), 0.0f);
    Quatf to = quatf_from_axis_angle(vec3f(1.0f, 0.0f, 0.0f), MATH_PI);
    Quatf r = quatf_slerp(from, to, 0.3f);
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(r), 1.0f, EPS);
}

static void test_slerp_takes_shortest_path(TestContext* ctx)
{
    Quatf from = quatf_identity();
    Quatf to = quatf_neg(quatf_identity());
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_slerp(from, to, 0.5f), quatf_identity(), EPS));
}

static void test_nlerp_at_zero_matches_from(TestContext* ctx)
{
    Quatf from = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), 0.1f);
    Quatf to = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), 1.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_nlerp(from, to, 0.0f), from, EPS));
}

static void test_nlerp_at_one_matches_to(TestContext* ctx)
{
    Quatf from = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), 0.1f);
    Quatf to = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), 1.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_nlerp(from, to, 1.0f), to, EPS));
}

static void test_nlerp_result_is_unit_length(TestContext* ctx)
{
    Quatf from = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), 0.0f);
    Quatf to = quatf_from_axis_angle(vec3f(0.0f, 1.0f, 0.0f), MATH_PI / 2.0f);
    Quatf r = quatf_nlerp(from, to, 0.4f);
    TEST_ASSERT_FLOAT_EQ(ctx, quatf_len(r), 1.0f, EPS);
}

static void test_nlerp_clamp_clamps_above_one(TestContext* ctx)
{
    Quatf from = quatf_identity();
    Quatf to = quatf_from_axis_angle(vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(quatf_nlerp_clamp(from, to, 2.0f), to, EPS));
}

/*
 * Comparison
 */

static void test_eq_exact_equal(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, quatf_eq(quatf(1.0f, 2.0f, 3.0f, 4.0f), quatf(1.0f, 2.0f, 3.0f, 4.0f)));
}

static void test_eq_exact_different(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, quatf_eq(quatf(1.0f, 2.0f, 3.0f, 4.0f), quatf(1.0f, 2.0f, 3.0f, 4.1f)));
}

static void test_eq_eps_within(TestContext* ctx)
{
    Quatf a = quatf(1.0f, 2.0f, 3.0f, 4.0f);
    Quatf b = quatf(1.0f + 1e-7f, 2.0f, 3.0f, 4.0f);
    TEST_ASSERT_TRUE(ctx, quatf_eq_eps(a, b, 1e-6f));
}

static void test_eq_eps_outside(TestContext* ctx)
{
    Quatf a = quatf(1.0f, 2.0f, 3.0f, 4.0f);
    Quatf b = quatf(1.0f + 1e-4f, 2.0f, 3.0f, 4.0f);
    TEST_ASSERT_FALSE(ctx, quatf_eq_eps(a, b, 1e-6f));
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
    describe("arithmetic")
    {
        test(test_add);
        test(test_subtract);
        test(test_scale);
        test(test_negate);
    }
    describe("dot_length")
    {
        test(test_dot_orthogonal);
        test(test_dot_general);
        test(test_length_squared);
        test(test_length);
    }
    describe("conjugate")
    {
        test(test_conjugate);
    }
    describe("multiply")
    {
        test(test_multiply_i_times_i_equals_negative_one);
        test(test_multiply_j_times_j_equals_negative_one);
        test(test_multiply_k_times_k_equals_negative_one);
        test(test_multiply_i_times_j_equals_k);
        test(test_multiply_j_times_k_equals_i);
        test(test_multiply_k_times_i_equals_j);
        test(test_multiply_is_not_commutative);
        test(test_multiply_by_identity_is_unchanged);
    }
    describe("normalize")
    {
        test(test_normalize_unit_length);
        test(test_try_normalize_success);
        test(test_try_normalize_near_zero_returns_false);
        test(test_try_normalize_null_returns_false);
    }
    describe("inverse")
    {
        test(test_inverse_of_unit_quaternion_equals_conjugate);
        test(test_multiply_by_inverse_yields_identity);
        test(test_try_inverse_near_zero_returns_false);
        test(test_try_inverse_null_returns_false);
    }
    describe("from_axis_angle")
    {
        test(test_from_axis_angle_result_is_unit_length);
        test(test_from_axis_angle_normalizes_axis);
        test(test_from_axis_angle_sincos_matches_from_axis_angle);
    }
    describe("from_euler")
    {
        test(test_from_euler_pure_pitch_matches_axis_angle_x);
        test(test_from_euler_pure_yaw_matches_axis_angle_y);
        test(test_from_euler_pure_roll_matches_axis_angle_z);
    }
    describe("to_axis_angle")
    {
        test(test_to_axis_angle_round_trip);
        test(test_to_axis_angle_zero_rotation_defaults_to_x_axis);
    }
    describe("from_orthonormal_basis")
    {
        test(test_from_orthonormal_basis_identity);
        test(test_from_orthonormal_basis_quarter_turn_matches_axis_angle);
        test(test_from_orthonormal_basis_round_trip_matches_source_rotation);
        test(test_from_orthonormal_basis_result_is_unit_length);
    }
    describe("look_rotation")
    {
        test(test_look_rotation_default_forward_and_up_is_identity);
        test(test_look_rotation_result_is_unit_length);
        test(test_look_rotation_rotates_local_forward_to_target);
        test(test_look_rotation_rotated_up_is_orthogonal_to_forward);
        test(test_look_rotation_ignores_up_magnitude);
        test(test_look_rotation_quarter_turn_matches_axis_angle);
    }
    describe("from_two_vectors")
    {
        test(test_from_two_vectors_identical_is_identity);
        test(test_from_two_vectors_quarter_turn_matches_axis_angle);
        test(test_from_two_vectors_rotates_from_onto_to);
        test(test_from_two_vectors_result_is_unit_length);
        test(test_from_two_vectors_antiparallel_rotates_from_onto_to);
        test(test_from_two_vectors_antiparallel_result_is_unit_length);
    }
    describe("rotate_vec3f")
    {
        test(test_rotate_vec3f_identity_unchanged);
        test(test_rotate_vec3f_quarter_turn_around_z);
        test(test_rotate_vec3f_quarter_turn_around_x);
        test(test_rotate_vec3f_quarter_turn_around_y);
    }
    describe("rotate_axis")
    {
        test(test_rotate_x_from_identity_matches_axis_angle);
        test(test_rotate_y_from_identity_matches_axis_angle);
        test(test_rotate_z_from_identity_matches_axis_angle);
        test(test_rotate_x_then_z_matches_vec3f_composition);
        test(test_rotate_x_inplace_matches_rotate_x);
    }
    describe("lerp")
    {
        test(test_lerp_at_zero);
        test(test_lerp_at_one);
        test(test_lerp_clamped_above_one);
        test(test_lerp_clamped_below_zero);
    }
    describe("slerp")
    {
        test(test_slerp_at_zero_returns_from);
        test(test_slerp_at_one_returns_to);
        test(test_slerp_result_is_unit_length);
        test(test_slerp_takes_shortest_path);
    }
    describe("nlerp")
    {
        test(test_nlerp_at_zero_matches_from);
        test(test_nlerp_at_one_matches_to);
        test(test_nlerp_result_is_unit_length);
        test(test_nlerp_clamp_clamps_above_one);
    }
    describe("eq")
    {
        test(test_eq_exact_equal);
        test(test_eq_exact_different);
        test(test_eq_eps_within);
        test(test_eq_eps_outside);
    }
}

const TestSuite quatf_suite = { "quatf", setup, &reg };
