#include "test.h"

#include "math/vec2f.h"

#define EPS 1e-5f

/*
 * Construction
 */

static void test_create(TestContext* ctx)
{
    Vec2f v = vec2f(3.0f, 4.0f);
    TEST_ASSERT(ctx, v.x == 3.0f);
    TEST_ASSERT(ctx, v.y == 4.0f);
}

static void test_zero(TestContext* ctx)
{
    Vec2f v = vec2f_zero();
    TEST_ASSERT(ctx, v.x == 0.0f);
    TEST_ASSERT(ctx, v.y == 0.0f);
}

static void test_one(TestContext* ctx)
{
    Vec2f v = vec2f_one();
    TEST_ASSERT(ctx, v.x == 1.0f);
    TEST_ASSERT(ctx, v.y == 1.0f);
}

/*
 * Helpers
 */

static void test_is_finite_true_for_finite_vector(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec2f_is_finite(vec2f(1.0f, 2.0f)));
}

static void test_is_finite_false_for_nan_component(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2f_is_finite(vec2f(NAN, 0.0f)));
}

static void test_is_finite_false_for_infinite_component(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2f_is_finite(vec2f(1.0f, INFINITY)));
}

/*
 * Arithmetic
 */

static void test_add(TestContext* ctx)
{
    Vec2f r = vec2f_add(vec2f(1.0f, 2.0f), vec2f(3.0f, 4.0f));
    TEST_ASSERT(ctx, r.x == 4.0f);
    TEST_ASSERT(ctx, r.y == 6.0f);
}

static void test_subtract(TestContext* ctx)
{
    Vec2f r = vec2f_sub(vec2f(5.0f, 7.0f), vec2f(2.0f, 3.0f));
    TEST_ASSERT(ctx, r.x == 3.0f);
    TEST_ASSERT(ctx, r.y == 4.0f);
}

static void test_scale(TestContext* ctx)
{
    Vec2f r = vec2f_scale(vec2f(2.0f, 3.0f), 4.0f);
    TEST_ASSERT(ctx, r.x == 8.0f);
    TEST_ASSERT(ctx, r.y == 12.0f);
}

static void test_divide(TestContext* ctx)
{
    Vec2f r = vec2f_div(vec2f(6.0f, 9.0f), 3.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 3.0f, EPS);
}

static void test_negate(TestContext* ctx)
{
    Vec2f r = vec2f_neg(vec2f(1.0f, -2.0f));
    TEST_ASSERT(ctx, r.x == -1.0f);
    TEST_ASSERT(ctx, r.y == 2.0f);
}

/*
 * Dot product
 */

static void test_dot_orthogonal(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_dot(vec2f(1.0f, 0.0f), vec2f(0.0f, 1.0f)), 0.0f, EPS);
}

static void test_dot_parallel(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_dot(vec2f(1.0f, 0.0f), vec2f(1.0f, 0.0f)), 1.0f, EPS);
}

/*
 * Cross product
 */

static void test_cross_perpendicular_counter_clockwise(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_cross(vec2f(1.0f, 0.0f), vec2f(0.0f, 1.0f)), 1.0f, EPS);
}

static void test_cross_perpendicular_clockwise(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_cross(vec2f(0.0f, 1.0f), vec2f(1.0f, 0.0f)), -1.0f, EPS);
}

static void test_cross_parallel_same_direction(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_cross(vec2f(1.0f, 0.0f), vec2f(2.0f, 0.0f)), 0.0f, EPS);
}

static void test_cross_parallel_opposite_direction(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_cross(vec2f(1.0f, 0.0f), vec2f(-2.0f, 0.0f)), 0.0f, EPS);
}

static void test_cross_same_vector(TestContext* ctx)
{
    const Vec2f vector = vec2f(3.0f, 4.0f);

    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_cross(vector, vector), 0.0f, EPS);
}

static void test_cross_general_vectors(TestContext* ctx)
{
    /*
     * (2 * 4) - (3 * -1)
     * 8 + 3 = 11
     */
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_cross(vec2f(2.0f, 3.0f), vec2f(-1.0f, 4.0f)), 11.0f, EPS);
}

static void test_cross_anti_commutative(TestContext* ctx)
{
    const Vec2f a = vec2f(2.0f, 3.0f);
    const Vec2f b = vec2f(-1.0f, 4.0f);

    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_cross(a, b), -vec2f_cross(b, a), EPS);
}

static void test_cross_zero_vector(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_cross(vec2f_zero(), vec2f(3.0f, 4.0f)), 0.0f, EPS);
}

/*
 * Reflection
 */

static void test_reflect_parallel_to_surface_unchanged(TestContext* ctx)
{
    Vec2f r = vec2f_reflect(vec2f(1.0f, 0.0f), vec2f(0.0f, 1.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
}

static void test_reflect_straight_incidence_bounces_back(TestContext* ctx)
{
    Vec2f r = vec2f_reflect(vec2f(0.0f, -1.0f), vec2f(0.0f, 1.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
}

static void test_reflect_general_angle(TestContext* ctx)
{
    /*
     * dot((1,-1), (0,1)) = -1
     * (1,-1) - 2 * -1 * (0,1) = (1,-1) + (0,2) = (1,1)
     */
    Vec2f r = vec2f_reflect(vec2f(1.0f, -1.0f), vec2f(0.0f, 1.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
}

static void test_reflect_preserves_length(TestContext* ctx)
{
    Vec2f v = vec2f(3.0f, 4.0f);
    Vec2f normal = vec2f_normalize(vec2f(1.0f, 1.0f));

    Vec2f r = vec2f_reflect(v, normal);

    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_len(r), vec2f_len(v), EPS);
}

/*
 * Length
 */

static void test_length_squared(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_len_sq(vec2f(3.0f, 4.0f)), 25.0f, EPS);
}

static void test_length(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_len(vec2f(3.0f, 4.0f)), 5.0f, EPS);
}

/*
 * Distance
 */

static void test_distance_squared(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_dist_sq(vec2f_zero(), vec2f(3.0f, 4.0f)), 25.0f, EPS);
}

static void test_distance(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_dist(vec2f_zero(), vec2f(3.0f, 4.0f)), 5.0f, EPS);
}

static void test_distance_same_point(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_dist(vec2f(2.0f, 3.0f), vec2f(2.0f, 3.0f)), 0.0f, EPS);
}

/*
 * Normalization
 */

static void test_normalize_unit_length(TestContext* ctx)
{
    Vec2f n = vec2f_normalize(vec2f(3.0f, 4.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_len(n), 1.0f, EPS);
}

static void test_normalize_near_zero_returns_zero(TestContext* ctx)
{
    Vec2f n = vec2f_normalize(vec2f_zero());
    TEST_ASSERT_TRUE(ctx, vec2f_eq(n, vec2f_zero()));
}

static void test_try_normalize_success(TestContext* ctx)
{
    Vec2f result = vec2f_zero();
    TEST_ASSERT_TRUE(ctx, vec2f_try_normalize(vec2f(3.0f, 4.0f), &result));
    TEST_ASSERT_FLOAT_EQ(ctx, vec2f_len(result), 1.0f, EPS);
}

static void test_try_normalize_near_zero_returns_false(TestContext* ctx)
{
    Vec2f result = vec2f(1.0f, 0.0f);
    TEST_ASSERT_FALSE(ctx, vec2f_try_normalize(vec2f_zero(), &result));
    TEST_ASSERT_TRUE(ctx, vec2f_eq(result, vec2f(1.0f, 0.0f)));
}

static void test_try_normalize_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2f_try_normalize(vec2f(1.0f, 0.0f), NULL));
}

/*
 * Safe division
 */

static void test_try_divide_success(TestContext* ctx)
{
    Vec2f result = vec2f_zero();
    TEST_ASSERT_TRUE(ctx, vec2f_try_div(vec2f(6.0f, 9.0f), 3.0f, &result));
    TEST_ASSERT_FLOAT_EQ(ctx, result.x, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.y, 3.0f, EPS);
}

static void test_try_divide_near_zero_returns_false(TestContext* ctx)
{
    Vec2f result = vec2f_zero();
    TEST_ASSERT_FALSE(ctx, vec2f_try_div(vec2f(1.0f, 1.0f), 0.0f, &result));
}

static void test_try_divide_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2f_try_div(vec2f(1.0f, 0.0f), 2.0f, NULL));
}

/*
 * Interpolation
 */

static void test_lerp_at_zero(TestContext* ctx)
{
    Vec2f from = vec2f_zero();
    Vec2f to = vec2f(10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec2f_eq(vec2f_lerp(from, to, 0.0f), from));
}

static void test_lerp_at_one(TestContext* ctx)
{
    Vec2f from = vec2f_zero();
    Vec2f to = vec2f(10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec2f_eq(vec2f_lerp(from, to, 1.0f), to));
}

static void test_lerp_at_midpoint(TestContext* ctx)
{
    Vec2f r = vec2f_lerp(vec2f_zero(), vec2f(10.0f, 10.0f), 0.5f);
    TEST_ASSERT_TRUE(ctx, vec2f_eq_eps(r, vec2f(5.0f, 5.0f), EPS));
}

static void test_lerp_clamped_above_one(TestContext* ctx)
{
    Vec2f to = vec2f(10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec2f_eq(vec2f_lerp_clamp(vec2f_zero(), to, 2.0f), to));
}

static void test_lerp_clamped_below_zero(TestContext* ctx)
{
    Vec2f from = vec2f_zero();
    TEST_ASSERT_TRUE(ctx, vec2f_eq(vec2f_lerp_clamp(from, vec2f(10.0f, 10.0f), -1.0f), from));
}

/*
 * Comparison
 */

static void test_equals_exact_equal(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec2f_eq(vec2f(1.0f, 2.0f), vec2f(1.0f, 2.0f)));
}

static void test_equals_exact_different(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2f_eq(vec2f(1.0f, 2.0f), vec2f(1.0f, 2.1f)));
}

static void test_equals_epsilon_within(TestContext* ctx)
{
    Vec2f a = vec2f(1.0f, 2.0f);
    Vec2f b = vec2f(1.0f + 1e-7f, 2.0f);
    TEST_ASSERT_TRUE(ctx, vec2f_eq_eps(a, b, 1e-6f));
}

static void test_equals_epsilon_outside(TestContext* ctx)
{
    Vec2f a = vec2f(1.0f, 2.0f);
    Vec2f b = vec2f(1.0f + 1e-4f, 2.0f);
    TEST_ASSERT_FALSE(ctx, vec2f_eq_eps(a, b, 1e-6f));
}

static void test_near_within_distance(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec2f_near(vec2f_zero(), vec2f(3.0f, 4.0f), 5.0f));
}

static void test_near_outside_distance(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2f_near(vec2f_zero(), vec2f(3.0f, 4.0f), 4.0f));
}

static void test_near_sq_within_distance(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec2f_near_sq(vec2f_zero(), vec2f(3.0f, 4.0f), 25.0f));
}

static void test_near_sq_outside_distance(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2f_near_sq(vec2f_zero(), vec2f(3.0f, 4.0f), 24.0f));
}

/*
 * In-place operations
 */

static void test_add_inplace(TestContext* ctx)
{
    Vec2f v = vec2f(1.0f, 2.0f);
    vec2f_add_inplace(&v, vec2f(3.0f, 4.0f));
    TEST_ASSERT(ctx, v.x == 4.0f);
    TEST_ASSERT(ctx, v.y == 6.0f);
}

static void test_subtract_inplace(TestContext* ctx)
{
    Vec2f v = vec2f(5.0f, 7.0f);
    vec2f_sub_inplace(&v, vec2f(2.0f, 3.0f));
    TEST_ASSERT(ctx, v.x == 3.0f);
    TEST_ASSERT(ctx, v.y == 4.0f);
}

static void test_multiply_inplace(TestContext* ctx)
{
    Vec2f v = vec2f(2.0f, 3.0f);
    vec2f_scale_inplace(&v, 4.0f);
    TEST_ASSERT(ctx, v.x == 8.0f);
    TEST_ASSERT(ctx, v.y == 12.0f);
}

static void test_divide_inplace(TestContext* ctx)
{
    Vec2f v = vec2f(6.0f, 9.0f);
    vec2f_div_inplace(&v, 3.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 3.0f, EPS);
}

static void test_negate_inplace(TestContext* ctx)
{
    Vec2f v = vec2f(1.0f, -2.0f);
    vec2f_neg_inplace(&v);
    TEST_ASSERT(ctx, v.x == -1.0f);
    TEST_ASSERT(ctx, v.y == 2.0f);
}

/*
 * Rotation
 */

static void test_rotate_sincos_quarter_turn(TestContext* ctx)
{
    Vec2f r = vec2f_rotate_sincos(vec2f(1.0f, 0.0f), 1.0f, 0.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
}

static void test_rotate_sincos_rotates_y_axis(TestContext* ctx)
{
    Vec2f r = vec2f_rotate_sincos(vec2f(0.0f, 1.0f), 1.0f, 0.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, -1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
}

static void test_rotate_zero_angle(TestContext* ctx)
{
    Vec2f r = vec2f_rotate(vec2f(1.0f, 0.0f), 0.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
}

static void test_rotate_quarter_turn(TestContext* ctx)
{
    Vec2f r = vec2f_rotate(vec2f(1.0f, 0.0f), MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
}

static void test_rotate_half_turn(TestContext* ctx)
{
    Vec2f r = vec2f_rotate(vec2f(1.0f, 0.0f), MATH_PI);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, -1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
}

static void test_rotate_negative_quarter_turn(TestContext* ctx)
{
    Vec2f r = vec2f_rotate(vec2f(0.0f, 1.0f), -MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 0.0f, EPS);
}

static void test_rotate_inplace_quarter_turn(TestContext* ctx)
{
    Vec2f v = vec2f(1.0f, 0.0f);
    vec2f_rotate_inplace(&v, MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, 0.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 1.0f, EPS);
}

static void test_rotate_inplace_half_turn(TestContext* ctx)
{
    Vec2f v = vec2f(1.0f, 0.0f);
    vec2f_rotate_inplace(&v, MATH_PI);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, -1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 0.0f, EPS);
}

static void test_rotate_around_pivot(TestContext* ctx)
{
    Vec2f r = vec2f_rotate_around(vec2f(2.0f, 0.0f), vec2f(1.0f, 0.0f), MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 1.0f, EPS);
}

static void test_rotate_around_self_returns_same(TestContext* ctx)
{
    Vec2f v = vec2f(1.0f, 0.0f);
    Vec2f r = vec2f_rotate_around(v, v, MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, v.x, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, v.y, EPS);
}

static void test_rotate_around_inplace_pivot(TestContext* ctx)
{
    Vec2f v = vec2f(2.0f, 0.0f);
    vec2f_rotate_around_inplace(&v, vec2f(1.0f, 0.0f), MATH_PI / 2.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, 1.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 1.0f, EPS);
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
        test(test_cross_perpendicular_counter_clockwise);
        test(test_cross_perpendicular_clockwise);
        test(test_cross_parallel_same_direction);
        test(test_cross_parallel_opposite_direction);
        test(test_cross_same_vector);
        test(test_cross_general_vectors);
        test(test_cross_anti_commutative);
        test(test_cross_zero_vector);
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
        test(test_near_sq_within_distance);
        test(test_near_sq_outside_distance);
    }
    describe("rotate")
    {
        test(test_rotate_zero_angle);
        test(test_rotate_quarter_turn);
        test(test_rotate_half_turn);
        test(test_rotate_negative_quarter_turn);
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
        test(test_rotate_sincos_quarter_turn);
        test(test_rotate_sincos_rotates_y_axis);
    }
}

const TestSuite vec2f_suite = { "vec2f", setup, &reg };
