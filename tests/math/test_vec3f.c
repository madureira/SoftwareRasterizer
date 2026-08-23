#include "test.h"

#include "math/vec3f.h"

#define EPS 1e-5f

/*
 * Construction
 */

static void test_create(TestContext* ctx)
{
    Vec3f v = vec3f(3.0f, 4.0f, 5.0f);
    TEST_ASSERT(ctx, v.x == 3.0f);
    TEST_ASSERT(ctx, v.y == 4.0f);
    TEST_ASSERT(ctx, v.z == 5.0f);
}

static void test_zero(TestContext* ctx)
{
    Vec3f v = vec3f_zero();
    TEST_ASSERT(ctx, v.x == 0.0f);
    TEST_ASSERT(ctx, v.y == 0.0f);
    TEST_ASSERT(ctx, v.z == 0.0f);
}

static void test_one(TestContext* ctx)
{
    Vec3f v = vec3f_one();
    TEST_ASSERT(ctx, v.x == 1.0f);
    TEST_ASSERT(ctx, v.y == 1.0f);
    TEST_ASSERT(ctx, v.z == 1.0f);
}

/*
 * Arithmetic
 */

static void test_add(TestContext* ctx)
{
    Vec3f r = vec3f_add(vec3f(1.0f, 2.0f, 3.0f), vec3f(4.0f, 5.0f, 6.0f));
    TEST_ASSERT(ctx, r.x == 5.0f);
    TEST_ASSERT(ctx, r.y == 7.0f);
    TEST_ASSERT(ctx, r.z == 9.0f);
}

static void test_subtract(TestContext* ctx)
{
    Vec3f r = vec3f_sub(vec3f(5.0f, 7.0f, 9.0f), vec3f(2.0f, 3.0f, 4.0f));
    TEST_ASSERT(ctx, r.x == 3.0f);
    TEST_ASSERT(ctx, r.y == 4.0f);
    TEST_ASSERT(ctx, r.z == 5.0f);
}

static void test_scale(TestContext* ctx)
{
    Vec3f r = vec3f_scale(vec3f(2.0f, 3.0f, 4.0f), 4.0f);
    TEST_ASSERT(ctx, r.x == 8.0f);
    TEST_ASSERT(ctx, r.y == 12.0f);
    TEST_ASSERT(ctx, r.z == 16.0f);
}

static void test_divide(TestContext* ctx)
{
    Vec3f r = vec3f_div(vec3f(6.0f, 9.0f, 12.0f), 3.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 3.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 4.0f, EPS);
}

static void test_negate(TestContext* ctx)
{
    Vec3f r = vec3f_neg(vec3f(1.0f, -2.0f, 3.0f));
    TEST_ASSERT(ctx, r.x == -1.0f);
    TEST_ASSERT(ctx, r.y == 2.0f);
    TEST_ASSERT(ctx, r.z == -3.0f);
}

static void test_add_inplace(TestContext* ctx)
{
    Vec3f v = vec3f(1.0f, 2.0f, 3.0f);
    vec3f_add_inplace(&v, vec3f(3.0f, 4.0f, 5.0f));
    TEST_ASSERT(ctx, v.x == 4.0f);
    TEST_ASSERT(ctx, v.y == 6.0f);
    TEST_ASSERT(ctx, v.z == 8.0f);
}

static void test_subtract_inplace(TestContext* ctx)
{
    Vec3f v = vec3f(5.0f, 7.0f, 9.0f);
    vec3f_sub_inplace(&v, vec3f(2.0f, 3.0f, 4.0f));
    TEST_ASSERT(ctx, v.x == 3.0f);
    TEST_ASSERT(ctx, v.y == 4.0f);
    TEST_ASSERT(ctx, v.z == 5.0f);
}

static void test_multiply_inplace(TestContext* ctx)
{
    Vec3f v = vec3f(2.0f, 3.0f, 4.0f);
    vec3f_scale_inplace(&v, 4.0f);
    TEST_ASSERT(ctx, v.x == 8.0f);
    TEST_ASSERT(ctx, v.y == 12.0f);
    TEST_ASSERT(ctx, v.z == 16.0f);
}

static void test_divide_inplace(TestContext* ctx)
{
    Vec3f v = vec3f(6.0f, 9.0f, 12.0f);
    vec3f_div_inplace(&v, 3.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 3.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.z, 4.0f, EPS);
}

static void test_negate_inplace(TestContext* ctx)
{
    Vec3f v = vec3f(1.0f, -2.0f, 3.0f);
    vec3f_neg_inplace(&v);
    TEST_ASSERT(ctx, v.x == -1.0f);
    TEST_ASSERT(ctx, v.y == 2.0f);
    TEST_ASSERT(ctx, v.z == -3.0f);
}

/*
 * Dot product
 */

static void test_dot_orthogonal(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_dot(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f)), 0.0f,
                         EPS);
}

static void test_dot_parallel(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_dot(vec3f(1.0f, 0.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f)), 1.0f,
                         EPS);
}

/*
 * Cross product
 */

static void test_cross_x_cross_y_equals_z(TestContext* ctx)
{
    Vec3f r = vec3f_cross(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f));
    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(0.0f, 0.0f, 1.0f), EPS));
}

static void test_cross_y_cross_z_equals_x(TestContext* ctx)
{
    Vec3f r = vec3f_cross(vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f));
    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(1.0f, 0.0f, 0.0f), EPS));
}

static void test_cross_z_cross_x_equals_y(TestContext* ctx)
{
    Vec3f r = vec3f_cross(vec3f(0.0f, 0.0f, 1.0f), vec3f(1.0f, 0.0f, 0.0f));
    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(0.0f, 1.0f, 0.0f), EPS));
}

static void test_cross_same_vector_is_zero(TestContext* ctx)
{
    const Vec3f vector = vec3f(1.0f, 2.0f, 3.0f);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(vec3f_cross(vector, vector), vec3f_zero(), EPS));
}

static void test_cross_zero_vector(TestContext* ctx)
{
    TEST_ASSERT_TRUE(
        ctx, vec3f_eq_eps(vec3f_cross(vec3f_zero(), vec3f(3.0f, 4.0f, 5.0f)), vec3f_zero(), EPS));
}

static void test_cross_general_vectors(TestContext* ctx)
{
    /*
     * (2*6 - 3*5, 3*4 - 1*6, 1*5 - 2*4)
     * (12 - 15, 12 - 6, 5 - 8) = (-3, 6, -3)
     */
    Vec3f r = vec3f_cross(vec3f(1.0f, 2.0f, 3.0f), vec3f(4.0f, 5.0f, 6.0f));
    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(-3.0f, 6.0f, -3.0f), EPS));
}

static void test_cross_anti_commutative(TestContext* ctx)
{
    const Vec3f a = vec3f(1.0f, 2.0f, 3.0f);
    const Vec3f b = vec3f(4.0f, 5.0f, 6.0f);

    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(vec3f_cross(a, b), vec3f_neg(vec3f_cross(b, a)), EPS));
}

static void test_cross_result_orthogonal_to_operands(TestContext* ctx)
{
    const Vec3f a = vec3f(1.0f, 2.0f, 3.0f);
    const Vec3f b = vec3f(4.0f, 5.0f, 6.0f);
    const Vec3f cross = vec3f_cross(a, b);

    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_dot(cross, a), 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_dot(cross, b), 0.0f, EPS);
}

/*
 * Reflection
 */

static void test_reflect_parallel_to_surface_unchanged(TestContext* ctx)
{
    Vec3f r = vec3f_reflect(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_reflect_straight_incidence_bounces_back(TestContext* ctx)
{
    Vec3f r = vec3f_reflect(vec3f(0.0f, -1.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_reflect_general_angle(TestContext* ctx)
{
    /*
     * dot((1,-1,0), (0,1,0)) = -1
     * (1,-1,0) - 2 * -1 * (0,1,0) = (1,-1,0) + (0,2,0) = (1,1,0)
     */
    Vec3f r = vec3f_reflect(vec3f(1.0f, -1.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_reflect_preserves_length(TestContext* ctx)
{
    Vec3f v = vec3f(2.0f, 3.0f, 6.0f);
    Vec3f normal = vec3f_normalize(vec3f(1.0f, 1.0f, 1.0f));

    Vec3f r = vec3f_reflect(v, normal);

    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_len(r), vec3f_len(v), EPS);
}

/*
 * Length
 */

static void test_length_squared(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_len_sq(vec3f(2.0f, 3.0f, 6.0f)), 49.0f, EPS);
}

static void test_length(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_len(vec3f(2.0f, 3.0f, 6.0f)), 7.0f, EPS);
}

/*
 * Distance
 */

static void test_distance_squared(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_dist_sq(vec3f_zero(), vec3f(2.0f, 3.0f, 6.0f)), 49.0f, EPS);
}

static void test_distance(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_dist(vec3f_zero(), vec3f(2.0f, 3.0f, 6.0f)), 7.0f, EPS);
}

static void test_distance_same_point(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_dist(vec3f(2.0f, 3.0f, 4.0f), vec3f(2.0f, 3.0f, 4.0f)), 0.0f,
                         EPS);
}

/*
 * Normalization
 */

static void test_normalize_unit_length(TestContext* ctx)
{
    Vec3f n = vec3f_normalize(vec3f(2.0f, 3.0f, 6.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_len(n), 1.0f, EPS);
}

static void test_normalize_near_zero_returns_zero(TestContext* ctx)
{
    Vec3f n = vec3f_normalize(vec3f_zero());
    TEST_ASSERT_TRUE(ctx, vec3f_eq(n, vec3f_zero()));
}

static void test_try_normalize_success(TestContext* ctx)
{
    Vec3f result = vec3f_zero();
    TEST_ASSERT_TRUE(ctx, vec3f_try_normalize(vec3f(2.0f, 3.0f, 6.0f), &result));
    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_len(result), 1.0f, EPS);
}

static void test_try_normalize_near_zero_returns_false(TestContext* ctx)
{
    Vec3f result = vec3f(1.0f, 0.0f, 0.0f);
    TEST_ASSERT_FALSE(ctx, vec3f_try_normalize(vec3f_zero(), &result));
    TEST_ASSERT_TRUE(ctx, vec3f_eq(result, vec3f(1.0f, 0.0f, 0.0f)));
}

static void test_try_normalize_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec3f_try_normalize(vec3f(1.0f, 0.0f, 0.0f), NULL));
}

/*
 * Safe division
 */

static void test_try_divide_success(TestContext* ctx)
{
    Vec3f result = vec3f_zero();
    TEST_ASSERT_TRUE(ctx, vec3f_try_div(vec3f(6.0f, 9.0f, 12.0f), 3.0f, &result));
    TEST_ASSERT_FLOAT_EQ(ctx, result.x, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.y, 3.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.z, 4.0f, EPS);
}

static void test_try_divide_near_zero_returns_false(TestContext* ctx)
{
    Vec3f result = vec3f_zero();
    TEST_ASSERT_FALSE(ctx, vec3f_try_div(vec3f(1.0f, 1.0f, 1.0f), 0.0f, &result));
}

static void test_try_divide_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec3f_try_div(vec3f(1.0f, 0.0f, 0.0f), 2.0f, NULL));
}

/*
 * Interpolation
 */

static void test_lerp_at_zero(TestContext* ctx)
{
    Vec3f from = vec3f_zero();
    Vec3f to = vec3f(10.0f, 10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec3f_eq(vec3f_lerp(from, to, 0.0f), from));
}

static void test_lerp_at_one(TestContext* ctx)
{
    Vec3f from = vec3f_zero();
    Vec3f to = vec3f(10.0f, 10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec3f_eq(vec3f_lerp(from, to, 1.0f), to));
}

static void test_lerp_at_midpoint(TestContext* ctx)
{
    Vec3f r = vec3f_lerp(vec3f_zero(), vec3f(10.0f, 10.0f, 10.0f), 0.5f);
    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(r, vec3f(5.0f, 5.0f, 5.0f), EPS));
}

static void test_lerp_clamped_above_one(TestContext* ctx)
{
    Vec3f to = vec3f(10.0f, 10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec3f_eq(vec3f_lerp_clamp(vec3f_zero(), to, 2.0f), to));
}

static void test_lerp_clamped_below_zero(TestContext* ctx)
{
    Vec3f from = vec3f_zero();
    TEST_ASSERT_TRUE(ctx,
                     vec3f_eq(vec3f_lerp_clamp(from, vec3f(10.0f, 10.0f, 10.0f), -1.0f), from));
}

/*
 * Comparison
 */

static void test_equals_exact_equal(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec3f_eq(vec3f(1.0f, 2.0f, 3.0f), vec3f(1.0f, 2.0f, 3.0f)));
}

static void test_equals_exact_different(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec3f_eq(vec3f(1.0f, 2.0f, 3.0f), vec3f(1.0f, 2.0f, 3.1f)));
}

static void test_equals_epsilon_within(TestContext* ctx)
{
    Vec3f a = vec3f(1.0f, 2.0f, 3.0f);
    Vec3f b = vec3f(1.0f + 1e-7f, 2.0f, 3.0f);
    TEST_ASSERT_TRUE(ctx, vec3f_eq_eps(a, b, 1e-6f));
}

static void test_equals_epsilon_outside(TestContext* ctx)
{
    Vec3f a = vec3f(1.0f, 2.0f, 3.0f);
    Vec3f b = vec3f(1.0f + 1e-4f, 2.0f, 3.0f);
    TEST_ASSERT_FALSE(ctx, vec3f_eq_eps(a, b, 1e-6f));
}

static void test_near_within_distance(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec3f_near(vec3f_zero(), vec3f(2.0f, 3.0f, 6.0f), 7.0f));
}

static void test_near_outside_distance(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec3f_near(vec3f_zero(), vec3f(2.0f, 3.0f, 6.0f), 6.0f));
}

/*
 * Rotation
 */

static void test_rotate_zero_angle(TestContext* ctx)
{
    Vec3f r = vec3f_rotate(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f), 0.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_rotate_quarter_turn_around_z(TestContext* ctx)
{
    Vec3f r = vec3f_rotate(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_rotate_half_turn_around_z(TestContext* ctx)
{
    Vec3f r = vec3f_rotate(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f), MATH_PI);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, -1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_rotate_negative_quarter_turn_around_z(TestContext* ctx)
{
    Vec3f r = vec3f_rotate(vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f), -MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_rotate_quarter_turn_around_x(TestContext* ctx)
{
    Vec3f r = vec3f_rotate(vec3f(0.0f, 1.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f), MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 1.0f, EPS);
}

static void test_rotate_quarter_turn_around_y(TestContext* ctx)
{
    Vec3f r = vec3f_rotate(vec3f(0.0f, 0.0f, 1.0f), vec3f(0.0f, 1.0f, 0.0f), MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_rotate_preserves_length(TestContext* ctx)
{
    Vec3f v = vec3f(2.0f, 3.0f, 6.0f);
    f32 length_before = vec3f_len(v);

    Vec3f r = vec3f_rotate(v, vec3f(1.0f, 1.0f, 1.0f), 0.7f);

    TEST_ASSERT_FLOAT_EQ(ctx, vec3f_len(r), length_before, EPS);
}

static void test_rotate_inplace_quarter_turn(TestContext* ctx)
{
    Vec3f v = vec3f(1.0f, 0.0f, 0.0f);
    vec3f_rotate_inplace(&v, vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.z, 0.0f, EPS);
}

static void test_rotate_inplace_half_turn(TestContext* ctx)
{
    Vec3f v = vec3f(1.0f, 0.0f, 0.0f);
    vec3f_rotate_inplace(&v, vec3f(0.0f, 0.0f, 1.0f), MATH_PI);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, -1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.z, 0.0f, EPS);
}

static void test_rotate_around_pivot(TestContext* ctx)
{
    Vec3f r = vec3f_rotate_around(vec3f(2.0f, 0.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f),
                                  vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_rotate_around_self_returns_same(TestContext* ctx)
{
    Vec3f v = vec3f(1.0f, 0.0f, 0.0f);
    Vec3f r = vec3f_rotate_around(v, v, vec3f(0.0f, 0.0f, 1.0f), MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, v.x, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, v.y, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, v.z, EPS);
}

static void test_rotate_around_inplace_pivot(TestContext* ctx)
{
    Vec3f v = vec3f(2.0f, 0.0f, 0.0f);
    vec3f_rotate_around_inplace(&v, vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f),
                                MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.z, 0.0f, EPS);
}

static void test_rotate_sincos_quarter_turn_around_z(TestContext* ctx)
{
    Vec3f r = vec3f_rotate_sincos(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, 1.0f), 1.0f, 0.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
}

static void test_rotate_sincos_normalizes_axis(TestContext* ctx)
{
    Vec3f r = vec3f_rotate_sincos(vec3f(1.0f, 0.0f, 0.0f), vec3f(0.0f, 0.0f, 2.0f), 1.0f, 0.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 0.0f, EPS);
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
        test(test_one);
    }
    describe("arithmetic")
    {
        test(test_add);
        test(test_subtract);
        test(test_scale);
        test(test_divide);
        test(test_negate);
        test(test_add_inplace);
        test(test_subtract_inplace);
        test(test_multiply_inplace);
        test(test_divide_inplace);
        test(test_negate_inplace);
    }
    describe("dot")
    {
        test(test_dot_orthogonal);
        test(test_dot_parallel);
    }
    describe("cross")
    {
        test(test_cross_x_cross_y_equals_z);
        test(test_cross_y_cross_z_equals_x);
        test(test_cross_z_cross_x_equals_y);
        test(test_cross_same_vector_is_zero);
        test(test_cross_zero_vector);
        test(test_cross_general_vectors);
        test(test_cross_anti_commutative);
        test(test_cross_result_orthogonal_to_operands);
    }
    describe("reflect")
    {
        test(test_reflect_parallel_to_surface_unchanged);
        test(test_reflect_straight_incidence_bounces_back);
        test(test_reflect_general_angle);
        test(test_reflect_preserves_length);
    }
    describe("length")
    {
        test(test_length_squared);
        test(test_length);
    }
    describe("distance")
    {
        test(test_distance_squared);
        test(test_distance);
        test(test_distance_same_point);
    }
    describe("normalize")
    {
        test(test_normalize_unit_length);
        test(test_normalize_near_zero_returns_zero);
        test(test_try_normalize_success);
        test(test_try_normalize_near_zero_returns_false);
        test(test_try_normalize_null_returns_false);
    }
    describe("try_div")
    {
        test(test_try_divide_success);
        test(test_try_divide_near_zero_returns_false);
        test(test_try_divide_null_returns_false);
    }
    describe("lerp")
    {
        test(test_lerp_at_zero);
        test(test_lerp_at_one);
        test(test_lerp_at_midpoint);
        test(test_lerp_clamped_above_one);
        test(test_lerp_clamped_below_zero);
    }
    describe("eq")
    {
        test(test_equals_exact_equal);
        test(test_equals_exact_different);
        test(test_equals_epsilon_within);
        test(test_equals_epsilon_outside);
        test(test_near_within_distance);
        test(test_near_outside_distance);
    }
    describe("rotate")
    {
        test(test_rotate_zero_angle);
        test(test_rotate_quarter_turn_around_z);
        test(test_rotate_half_turn_around_z);
        test(test_rotate_negative_quarter_turn_around_z);
        test(test_rotate_quarter_turn_around_x);
        test(test_rotate_quarter_turn_around_y);
        test(test_rotate_preserves_length);
    }
    describe("rotate_inplace")
    {
        test(test_rotate_inplace_quarter_turn);
        test(test_rotate_inplace_half_turn);
    }
    describe("rotate_around")
    {
        test(test_rotate_around_pivot);
        test(test_rotate_around_self_returns_same);
    }
    describe("rotate_around_inplace")
    {
        test(test_rotate_around_inplace_pivot);
    }
    describe("rotate_sincos")
    {
        test(test_rotate_sincos_quarter_turn_around_z);
        test(test_rotate_sincos_normalizes_axis);
    }
}

const TestSuite vec3f_suite = { "vec3f", setup, &reg };
