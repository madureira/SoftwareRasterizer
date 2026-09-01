#include "test.h"

#include "math/vec4f.h"

#define EPS 1e-5f

/*
 * Construction
 */

static void test_create(TestContext* ctx)
{
    Vec4f v = vec4f(1.0f, 2.0f, 3.0f, 4.0f);
    TEST_ASSERT(ctx, v.x == 1.0f);
    TEST_ASSERT(ctx, v.y == 2.0f);
    TEST_ASSERT(ctx, v.z == 3.0f);
    TEST_ASSERT(ctx, v.w == 4.0f);
}

static void test_zero(TestContext* ctx)
{
    Vec4f v = vec4f_zero();
    TEST_ASSERT(ctx, v.x == 0.0f);
    TEST_ASSERT(ctx, v.y == 0.0f);
    TEST_ASSERT(ctx, v.z == 0.0f);
    TEST_ASSERT(ctx, v.w == 0.0f);
}

static void test_one(TestContext* ctx)
{
    Vec4f v = vec4f_one();
    TEST_ASSERT(ctx, v.x == 1.0f);
    TEST_ASSERT(ctx, v.y == 1.0f);
    TEST_ASSERT(ctx, v.z == 1.0f);
    TEST_ASSERT(ctx, v.w == 1.0f);
}

/*
 * Homogeneous coordinates
 */

static void test_point_has_w_one(TestContext* ctx)
{
    Vec4f v = vec4f_point(1.0f, 2.0f, 3.0f);
    TEST_ASSERT(ctx, v.x == 1.0f);
    TEST_ASSERT(ctx, v.y == 2.0f);
    TEST_ASSERT(ctx, v.z == 3.0f);
    TEST_ASSERT(ctx, v.w == 1.0f);
}

static void test_vector_has_w_zero(TestContext* ctx)
{
    Vec4f v = vec4f_vector(1.0f, 2.0f, 3.0f);
    TEST_ASSERT(ctx, v.x == 1.0f);
    TEST_ASSERT(ctx, v.y == 2.0f);
    TEST_ASSERT(ctx, v.z == 3.0f);
    TEST_ASSERT(ctx, v.w == 0.0f);
}

static void test_is_point_true_for_point(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec4f_is_point(vec4f_point(1.0f, 2.0f, 3.0f)));
}

static void test_is_point_false_for_vector(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_is_point(vec4f_vector(1.0f, 2.0f, 3.0f)));
}

static void test_is_vector_true_for_vector(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec4f_is_vector(vec4f_vector(1.0f, 2.0f, 3.0f)));
}

static void test_is_vector_false_for_point(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_is_vector(vec4f_point(1.0f, 2.0f, 3.0f)));
}

/*
 * Helpers
 */

static void test_is_finite_true_for_finite_vector(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec4f_is_finite(vec4f(1.0f, 2.0f, 3.0f, 4.0f)));
}

static void test_is_finite_false_for_nan_component(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_is_finite(vec4f(NAN, 0.0f, 0.0f, 0.0f)));
}

static void test_is_finite_false_for_infinite_component(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_is_finite(vec4f(INFINITY, 0.0f, 0.0f, 0.0f)));
}

/*
 * Arithmetic
 */

static void test_add(TestContext* ctx)
{
    Vec4f r = vec4f_add(vec4f(1.0f, 2.0f, 3.0f, 4.0f), vec4f(5.0f, 6.0f, 7.0f, 8.0f));
    TEST_ASSERT(ctx, r.x == 6.0f);
    TEST_ASSERT(ctx, r.y == 8.0f);
    TEST_ASSERT(ctx, r.z == 10.0f);
    TEST_ASSERT(ctx, r.w == 12.0f);
}

static void test_subtract(TestContext* ctx)
{
    Vec4f r = vec4f_sub(vec4f(5.0f, 6.0f, 7.0f, 8.0f), vec4f(1.0f, 2.0f, 3.0f, 4.0f));
    TEST_ASSERT(ctx, r.x == 4.0f);
    TEST_ASSERT(ctx, r.y == 4.0f);
    TEST_ASSERT(ctx, r.z == 4.0f);
    TEST_ASSERT(ctx, r.w == 4.0f);
}

static void test_scale(TestContext* ctx)
{
    Vec4f r = vec4f_scale(vec4f(1.0f, 2.0f, 3.0f, 4.0f), 2.0f);
    TEST_ASSERT(ctx, r.x == 2.0f);
    TEST_ASSERT(ctx, r.y == 4.0f);
    TEST_ASSERT(ctx, r.z == 6.0f);
    TEST_ASSERT(ctx, r.w == 8.0f);
}

static void test_divide(TestContext* ctx)
{
    Vec4f r = vec4f_div(vec4f(2.0f, 4.0f, 6.0f, 8.0f), 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.z, 3.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.w, 4.0f, EPS);
}

static void test_negate(TestContext* ctx)
{
    Vec4f r = vec4f_neg(vec4f(1.0f, -2.0f, 3.0f, -4.0f));
    TEST_ASSERT(ctx, r.x == -1.0f);
    TEST_ASSERT(ctx, r.y == 2.0f);
    TEST_ASSERT(ctx, r.z == -3.0f);
    TEST_ASSERT(ctx, r.w == 4.0f);
}

static void test_add_inplace(TestContext* ctx)
{
    Vec4f v = vec4f(1.0f, 2.0f, 3.0f, 4.0f);
    vec4f_add_inplace(&v, vec4f(5.0f, 6.0f, 7.0f, 8.0f));
    TEST_ASSERT(ctx, v.x == 6.0f);
    TEST_ASSERT(ctx, v.y == 8.0f);
    TEST_ASSERT(ctx, v.z == 10.0f);
    TEST_ASSERT(ctx, v.w == 12.0f);
}

static void test_subtract_inplace(TestContext* ctx)
{
    Vec4f v = vec4f(5.0f, 6.0f, 7.0f, 8.0f);
    vec4f_sub_inplace(&v, vec4f(1.0f, 2.0f, 3.0f, 4.0f));
    TEST_ASSERT(ctx, v.x == 4.0f);
    TEST_ASSERT(ctx, v.y == 4.0f);
    TEST_ASSERT(ctx, v.z == 4.0f);
    TEST_ASSERT(ctx, v.w == 4.0f);
}

static void test_scale_inplace(TestContext* ctx)
{
    Vec4f v = vec4f(1.0f, 2.0f, 3.0f, 4.0f);
    vec4f_scale_inplace(&v, 2.0f);
    TEST_ASSERT(ctx, v.x == 2.0f);
    TEST_ASSERT(ctx, v.y == 4.0f);
    TEST_ASSERT(ctx, v.z == 6.0f);
    TEST_ASSERT(ctx, v.w == 8.0f);
}

static void test_divide_inplace(TestContext* ctx)
{
    Vec4f v = vec4f(2.0f, 4.0f, 6.0f, 8.0f);
    vec4f_div_inplace(&v, 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.z, 3.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.w, 4.0f, EPS);
}

static void test_negate_inplace(TestContext* ctx)
{
    Vec4f v = vec4f(1.0f, -2.0f, 3.0f, -4.0f);
    vec4f_neg_inplace(&v);
    TEST_ASSERT(ctx, v.x == -1.0f);
    TEST_ASSERT(ctx, v.y == 2.0f);
    TEST_ASSERT(ctx, v.z == -3.0f);
    TEST_ASSERT(ctx, v.w == 4.0f);
}

/*
 * Dot product
 */

static void test_dot_orthogonal(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(
        ctx, vec4f_dot(vec4f(1.0f, 0.0f, 0.0f, 0.0f), vec4f(0.0f, 1.0f, 0.0f, 0.0f)), 0.0f, EPS);
}

static void test_dot_general(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(
        ctx, vec4f_dot(vec4f(1.0f, 2.0f, 3.0f, 4.0f), vec4f(1.0f, 2.0f, 3.0f, 4.0f)), 30.0f, EPS);
}

/*
 * Length
 */

static void test_length_squared(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec4f_len_sq(vec4f(1.0f, 2.0f, 2.0f, 4.0f)), 25.0f, EPS);
}

static void test_length(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec4f_len(vec4f(1.0f, 2.0f, 2.0f, 4.0f)), 5.0f, EPS);
}

/*
 * Distance
 */

static void test_distance_squared(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec4f_dist_sq(vec4f_zero(), vec4f(1.0f, 2.0f, 2.0f, 4.0f)), 25.0f,
                         EPS);
}

static void test_distance(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec4f_dist(vec4f_zero(), vec4f(1.0f, 2.0f, 2.0f, 4.0f)), 5.0f, EPS);
}

static void test_distance_same_point(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(
        ctx, vec4f_dist(vec4f(1.0f, 2.0f, 3.0f, 4.0f), vec4f(1.0f, 2.0f, 3.0f, 4.0f)), 0.0f, EPS);
}

/*
 * Normalization
 */

static void test_normalize_unit_length(TestContext* ctx)
{
    Vec4f n = vec4f_normalize(vec4f(1.0f, 2.0f, 2.0f, 4.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, vec4f_len(n), 1.0f, EPS);
}

static void test_try_normalize_success(TestContext* ctx)
{
    Vec4f result = vec4f_zero();
    TEST_ASSERT_TRUE(ctx, vec4f_try_normalize(vec4f(1.0f, 2.0f, 2.0f, 4.0f), &result));
    TEST_ASSERT_FLOAT_EQ(ctx, vec4f_len(result), 1.0f, EPS);
}

static void test_try_normalize_near_zero_returns_false(TestContext* ctx)
{
    Vec4f result = vec4f(1.0f, 0.0f, 0.0f, 0.0f);
    TEST_ASSERT_FALSE(ctx, vec4f_try_normalize(vec4f_zero(), &result));
    TEST_ASSERT_TRUE(ctx, vec4f_eq(result, vec4f(1.0f, 0.0f, 0.0f, 0.0f)));
}

static void test_try_normalize_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_try_normalize(vec4f(1.0f, 0.0f, 0.0f, 0.0f), NULL));
}

/*
 * Safe division
 */

static void test_try_divide_success(TestContext* ctx)
{
    Vec4f result = vec4f_zero();
    TEST_ASSERT_TRUE(ctx, vec4f_try_div(vec4f(2.0f, 4.0f, 6.0f, 8.0f), 2.0f, &result));
    TEST_ASSERT_FLOAT_EQ(ctx, result.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.y, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.z, 3.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.w, 4.0f, EPS);
}

static void test_try_divide_near_zero_returns_false(TestContext* ctx)
{
    Vec4f result = vec4f_zero();
    TEST_ASSERT_FALSE(ctx, vec4f_try_div(vec4f(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, &result));
}

static void test_try_divide_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_try_div(vec4f(1.0f, 0.0f, 0.0f, 0.0f), 2.0f, NULL));
}

/*
 * Perspective divide
 */

static void test_try_perspective_divide_success(TestContext* ctx)
{
    Vec4f result = vec4f_zero();
    TEST_ASSERT_TRUE(ctx, vec4f_try_perspective_divide(vec4f(2.0f, 4.0f, 6.0f, 2.0f), &result));
    TEST_ASSERT_FLOAT_EQ(ctx, result.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.y, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.z, 3.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.w, 1.0f, EPS);
}

static void test_try_perspective_divide_near_zero_w_returns_false(TestContext* ctx)
{
    Vec4f result = vec4f_zero();
    TEST_ASSERT_FALSE(ctx, vec4f_try_perspective_divide(vec4f(1.0f, 2.0f, 3.0f, 0.0f), &result));
}

static void test_try_perspective_divide_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_try_perspective_divide(vec4f(1.0f, 2.0f, 3.0f, 1.0f), NULL));
}

/*
 * Interpolation
 */

static void test_lerp_at_zero(TestContext* ctx)
{
    Vec4f from = vec4f_zero();
    Vec4f to = vec4f(10.0f, 10.0f, 10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec4f_eq(vec4f_lerp(from, to, 0.0f), from));
}

static void test_lerp_at_one(TestContext* ctx)
{
    Vec4f from = vec4f_zero();
    Vec4f to = vec4f(10.0f, 10.0f, 10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec4f_eq(vec4f_lerp(from, to, 1.0f), to));
}

static void test_lerp_at_midpoint(TestContext* ctx)
{
    Vec4f r = vec4f_lerp(vec4f_zero(), vec4f(10.0f, 10.0f, 10.0f, 10.0f), 0.5f);
    TEST_ASSERT_TRUE(ctx, vec4f_eq_eps(r, vec4f(5.0f, 5.0f, 5.0f, 5.0f), EPS));
}

static void test_lerp_clamped_above_one(TestContext* ctx)
{
    Vec4f to = vec4f(10.0f, 10.0f, 10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec4f_eq(vec4f_lerp_clamp(vec4f_zero(), to, 2.0f), to));
}

static void test_lerp_clamped_below_zero(TestContext* ctx)
{
    Vec4f from = vec4f_zero();
    TEST_ASSERT_TRUE(
        ctx, vec4f_eq(vec4f_lerp_clamp(from, vec4f(10.0f, 10.0f, 10.0f, 10.0f), -1.0f), from));
}

/*
 * Comparison
 */

static void test_equals_exact_equal(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec4f_eq(vec4f(1.0f, 2.0f, 3.0f, 4.0f), vec4f(1.0f, 2.0f, 3.0f, 4.0f)));
}

static void test_equals_exact_different(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_eq(vec4f(1.0f, 2.0f, 3.0f, 4.0f), vec4f(1.0f, 2.0f, 3.0f, 4.1f)));
}

static void test_equals_epsilon_within(TestContext* ctx)
{
    Vec4f a = vec4f(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4f b = vec4f(1.0f + 1e-7f, 2.0f, 3.0f, 4.0f);
    TEST_ASSERT_TRUE(ctx, vec4f_eq_eps(a, b, 1e-6f));
}

static void test_equals_epsilon_outside(TestContext* ctx)
{
    Vec4f a = vec4f(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4f b = vec4f(1.0f + 1e-4f, 2.0f, 3.0f, 4.0f);
    TEST_ASSERT_FALSE(ctx, vec4f_eq_eps(a, b, 1e-6f));
}

static void test_near_within_distance(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec4f_near(vec4f_zero(), vec4f(1.0f, 2.0f, 2.0f, 4.0f), 5.0f));
}

static void test_near_outside_distance(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_near(vec4f_zero(), vec4f(1.0f, 2.0f, 2.0f, 4.0f), 4.0f));
}

static void test_near_sq_within_distance(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec4f_near_sq(vec4f_zero(), vec4f(1.0f, 2.0f, 2.0f, 4.0f), 25.0f));
}

static void test_near_sq_outside_distance(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec4f_near_sq(vec4f_zero(), vec4f(1.0f, 2.0f, 2.0f, 4.0f), 24.0f));
}

/*
 * Conversions
 */

static void test_from_vec3f(TestContext* ctx)
{
    Vec4f v = vec4f_from_vec3f(vec3f(1.0f, 2.0f, 3.0f), 4.0f);
    TEST_ASSERT(ctx, v.x == 1.0f);
    TEST_ASSERT(ctx, v.y == 2.0f);
    TEST_ASSERT(ctx, v.z == 3.0f);
    TEST_ASSERT(ctx, v.w == 4.0f);
}

static void test_xyz(TestContext* ctx)
{
    Vec3f xyz = vec4f_xyz(vec4f(1.0f, 2.0f, 3.0f, 4.0f));
    TEST_ASSERT(ctx, xyz.x == 1.0f);
    TEST_ASSERT(ctx, xyz.y == 2.0f);
    TEST_ASSERT(ctx, xyz.z == 3.0f);
}

static void test_xyz_from_vec3f_round_trip(TestContext* ctx)
{
    Vec3f xyz = vec3f(4.0f, 5.0f, 6.0f);
    TEST_ASSERT_TRUE(ctx, vec3f_eq(vec4f_xyz(vec4f_from_vec3f(xyz, 7.0f)), xyz));
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
    describe("homogeneous")
    {
        test(test_point_has_w_one);
        test(test_vector_has_w_zero);
        test(test_is_point_true_for_point);
        test(test_is_point_false_for_vector);
        test(test_is_vector_true_for_vector);
        test(test_is_vector_false_for_point);
    }
    describe("helpers")
    {
        test(test_is_finite_true_for_finite_vector);
        test(test_is_finite_false_for_nan_component);
        test(test_is_finite_false_for_infinite_component);
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
        test(test_scale_inplace);
        test(test_divide_inplace);
        test(test_negate_inplace);
    }
    describe("dot")
    {
        test(test_dot_orthogonal);
        test(test_dot_general);
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
    describe("try_perspective_divide")
    {
        test(test_try_perspective_divide_success);
        test(test_try_perspective_divide_near_zero_w_returns_false);
        test(test_try_perspective_divide_null_returns_false);
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
        test(test_near_sq_within_distance);
        test(test_near_sq_outside_distance);
    }
    describe("conversions")
    {
        test(test_from_vec3f);
        test(test_xyz);
        test(test_xyz_from_vec3f_round_trip);
    }
}

const TestSuite vec4f_suite = { "vec4f", setup, &reg };
