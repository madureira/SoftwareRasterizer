#include "test.h"

#include "math/vec2.h"

#define EPS 1e-5f

/*
 * Construction
 */

static void test_create(TestContext* ctx)
{
    Vec2 v = vec2(3.0f, 4.0f);
    TEST_ASSERT(ctx, v.x == 3.0f);
    TEST_ASSERT(ctx, v.y == 4.0f);
}

static void test_zero(TestContext* ctx)
{
    Vec2 v = vec2_zero();
    TEST_ASSERT(ctx, v.x == 0.0f);
    TEST_ASSERT(ctx, v.y == 0.0f);
}

static void test_one(TestContext* ctx)
{
    Vec2 v = vec2_one();
    TEST_ASSERT(ctx, v.x == 1.0f);
    TEST_ASSERT(ctx, v.y == 1.0f);
}

/*
 * Arithmetic
 */

static void test_add(TestContext* ctx)
{
    Vec2 r = vec2_add(vec2(1.0f, 2.0f), vec2(3.0f, 4.0f));
    TEST_ASSERT(ctx, r.x == 4.0f);
    TEST_ASSERT(ctx, r.y == 6.0f);
}

static void test_subtract(TestContext* ctx)
{
    Vec2 r = vec2_subtract(vec2(5.0f, 7.0f), vec2(2.0f, 3.0f));
    TEST_ASSERT(ctx, r.x == 3.0f);
    TEST_ASSERT(ctx, r.y == 4.0f);
}

static void test_multiply(TestContext* ctx)
{
    Vec2 r = vec2_multiply(vec2(2.0f, 3.0f), 4.0f);
    TEST_ASSERT(ctx, r.x == 8.0f);
    TEST_ASSERT(ctx, r.y == 12.0f);
}

static void test_divide(TestContext* ctx)
{
    Vec2 r = vec2_divide(vec2(6.0f, 9.0f), 3.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, r.x, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, r.y, 3.0f, EPS);
}

static void test_negate(TestContext* ctx)
{
    Vec2 r = vec2_negate(vec2(1.0f, -2.0f));
    TEST_ASSERT(ctx, r.x == -1.0f);
    TEST_ASSERT(ctx, r.y == 2.0f);
}

/*
 * Vector operations
 */

static void test_dot_orthogonal(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2_dot(vec2(1.0f, 0.0f), vec2(0.0f, 1.0f)), 0.0f, EPS);
}

static void test_dot_parallel(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2_dot(vec2(1.0f, 0.0f), vec2(1.0f, 0.0f)), 1.0f, EPS);
}

static void test_length_squared(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2_length_squared(vec2(3.0f, 4.0f)), 25.0f, EPS);
}

static void test_length(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2_length(vec2(3.0f, 4.0f)), 5.0f, EPS);
}

/*
 * Distance
 */

static void test_distance_squared(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2_distance_squared(vec2_zero(), vec2(3.0f, 4.0f)), 25.0f, EPS);
}

static void test_distance(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2_distance(vec2_zero(), vec2(3.0f, 4.0f)), 5.0f, EPS);
}

static void test_distance_same_point(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, vec2_distance(vec2(2.0f, 3.0f), vec2(2.0f, 3.0f)), 0.0f, EPS);
}

/*
 * Normalization
 */

static void test_normalize_unit_length(TestContext* ctx)
{
    Vec2 n = vec2_normalize(vec2(3.0f, 4.0f));
    TEST_ASSERT_FLOAT_EQ(ctx, vec2_length(n), 1.0f, EPS);
}

static void test_normalize_near_zero_returns_zero(TestContext* ctx)
{
    Vec2 n = vec2_normalize(vec2_zero());
    TEST_ASSERT_TRUE(ctx, vec2_equals_exact(n, vec2_zero()));
}

static void test_try_normalize_success(TestContext* ctx)
{
    Vec2 result = vec2_zero();
    TEST_ASSERT_TRUE(ctx, vec2_try_normalize(vec2(3.0f, 4.0f), &result));
    TEST_ASSERT_FLOAT_EQ(ctx, vec2_length(result), 1.0f, EPS);
}

static void test_try_normalize_near_zero_returns_false(TestContext* ctx)
{
    Vec2 result = vec2(1.0f, 0.0f);
    TEST_ASSERT_FALSE(ctx, vec2_try_normalize(vec2_zero(), &result));
    TEST_ASSERT_TRUE(ctx, vec2_equals_exact(result, vec2(1.0f, 0.0f)));
}

static void test_try_normalize_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2_try_normalize(vec2(1.0f, 0.0f), NULL));
}

/*
 * Safe division
 */

static void test_try_divide_success(TestContext* ctx)
{
    Vec2 result = vec2_zero();
    TEST_ASSERT_TRUE(ctx, vec2_try_divide(vec2(6.0f, 9.0f), 3.0f, &result));
    TEST_ASSERT_FLOAT_EQ(ctx, result.x, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, result.y, 3.0f, EPS);
}

static void test_try_divide_near_zero_returns_false(TestContext* ctx)
{
    Vec2 result = vec2_zero();
    TEST_ASSERT_FALSE(ctx, vec2_try_divide(vec2(1.0f, 1.0f), 0.0f, &result));
}

static void test_try_divide_null_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2_try_divide(vec2(1.0f, 0.0f), 2.0f, NULL));
}

/*
 * Interpolation
 */

static void test_lerp_at_zero(TestContext* ctx)
{
    Vec2 from = vec2_zero();
    Vec2 to = vec2(10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec2_equals_exact(vec2_lerp(from, to, 0.0f), from));
}

static void test_lerp_at_one(TestContext* ctx)
{
    Vec2 from = vec2_zero();
    Vec2 to = vec2(10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec2_equals_exact(vec2_lerp(from, to, 1.0f), to));
}

static void test_lerp_at_midpoint(TestContext* ctx)
{
    Vec2 r = vec2_lerp(vec2_zero(), vec2(10.0f, 10.0f), 0.5f);
    TEST_ASSERT_TRUE(ctx, vec2_equals_epsilon(r, vec2(5.0f, 5.0f), EPS));
}

static void test_lerp_clamped_above_one(TestContext* ctx)
{
    Vec2 to = vec2(10.0f, 10.0f);
    TEST_ASSERT_TRUE(ctx, vec2_equals_exact(vec2_lerp_clamped(vec2_zero(), to, 2.0f), to));
}

static void test_lerp_clamped_below_zero(TestContext* ctx)
{
    Vec2 from = vec2_zero();
    TEST_ASSERT_TRUE(ctx, vec2_equals_exact(vec2_lerp_clamped(from, vec2(10.0f, 10.0f), -1.0f), from));
}

/*
 * Comparison
 */

static void test_equals_exact_equal(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec2_equals_exact(vec2(1.0f, 2.0f), vec2(1.0f, 2.0f)));
}

static void test_equals_exact_different(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2_equals_exact(vec2(1.0f, 2.0f), vec2(1.0f, 2.1f)));
}

static void test_equals_epsilon_within(TestContext* ctx)
{
    Vec2 a = vec2(1.0f, 2.0f);
    Vec2 b = vec2(1.0f + 1e-7f, 2.0f);
    TEST_ASSERT_TRUE(ctx, vec2_equals_epsilon(a, b, 1e-6f));
}

static void test_equals_epsilon_outside(TestContext* ctx)
{
    Vec2 a = vec2(1.0f, 2.0f);
    Vec2 b = vec2(1.0f + 1e-4f, 2.0f);
    TEST_ASSERT_FALSE(ctx, vec2_equals_epsilon(a, b, 1e-6f));
}

static void test_near_within_distance(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, vec2_near(vec2_zero(), vec2(3.0f, 4.0f), 5.0f));
}

static void test_near_outside_distance(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, vec2_near(vec2_zero(), vec2(3.0f, 4.0f), 4.0f));
}

/*
 * In-place operations
 */

static void test_add_inplace(TestContext* ctx)
{
    Vec2 v = vec2(1.0f, 2.0f);
    vec2_add_inplace(&v, vec2(3.0f, 4.0f));
    TEST_ASSERT(ctx, v.x == 4.0f);
    TEST_ASSERT(ctx, v.y == 6.0f);
}

static void test_subtract_inplace(TestContext* ctx)
{
    Vec2 v = vec2(5.0f, 7.0f);
    vec2_subtract_inplace(&v, vec2(2.0f, 3.0f));
    TEST_ASSERT(ctx, v.x == 3.0f);
    TEST_ASSERT(ctx, v.y == 4.0f);
}

static void test_multiply_inplace(TestContext* ctx)
{
    Vec2 v = vec2(2.0f, 3.0f);
    vec2_multiply_inplace(&v, 4.0f);
    TEST_ASSERT(ctx, v.x == 8.0f);
    TEST_ASSERT(ctx, v.y == 12.0f);
}

static void test_divide_inplace(TestContext* ctx)
{
    Vec2 v = vec2(6.0f, 9.0f);
    vec2_divide_inplace(&v, 3.0f);
    TEST_ASSERT_FLOAT_EQ(ctx, v.x, 2.0f, EPS);
    TEST_ASSERT_FLOAT_EQ(ctx, v.y, 3.0f, EPS);
}

static void test_negate_inplace(TestContext* ctx)
{
    Vec2 v = vec2(1.0f, -2.0f);
    vec2_negate_inplace(&v);
    TEST_ASSERT(ctx, v.x == -1.0f);
    TEST_ASSERT(ctx, v.y == 2.0f);
}

/*
 * Suite registration
 */

// clang-format off
static const char *test_names[] = {
    "test_create",
    "test_zero",
    "test_one",
    "test_add",
    "test_subtract",
    "test_multiply",
    "test_divide",
    "test_negate",
    "test_dot_orthogonal",
    "test_dot_parallel",
    "test_length_squared",
    "test_length",
    "test_distance_squared",
    "test_distance",
    "test_distance_same_point",
    "test_normalize_unit_length",
    "test_normalize_near_zero_returns_zero",
    "test_try_normalize_success",
    "test_try_normalize_near_zero_returns_false",
    "test_try_normalize_null_returns_false",
    "test_try_divide_success",
    "test_try_divide_near_zero_returns_false",
    "test_try_divide_null_returns_false",
    "test_lerp_at_zero",
    "test_lerp_at_one",
    "test_lerp_at_midpoint",
    "test_lerp_clamped_above_one",
    "test_lerp_clamped_below_zero",
    "test_equals_exact_equal",
    "test_equals_exact_different",
    "test_equals_epsilon_within",
    "test_equals_epsilon_outside",
    "test_near_within_distance",
    "test_near_outside_distance",
    "test_add_inplace",
    "test_subtract_inplace",
    "test_multiply_inplace",
    "test_divide_inplace",
    "test_negate_inplace",
};

static TestFn tests[] = {
    test_create,
    test_zero,
    test_one,
    test_add,
    test_subtract,
    test_multiply,
    test_divide,
    test_negate,
    test_dot_orthogonal,
    test_dot_parallel,
    test_length_squared,
    test_length,
    test_distance_squared,
    test_distance,
    test_distance_same_point,
    test_normalize_unit_length,
    test_normalize_near_zero_returns_zero,
    test_try_normalize_success,
    test_try_normalize_near_zero_returns_false,
    test_try_normalize_null_returns_false,
    test_try_divide_success,
    test_try_divide_near_zero_returns_false,
    test_try_divide_null_returns_false,
    test_lerp_at_zero,
    test_lerp_at_one,
    test_lerp_at_midpoint,
    test_lerp_clamped_above_one,
    test_lerp_clamped_below_zero,
    test_equals_exact_equal,
    test_equals_exact_different,
    test_equals_epsilon_within,
    test_equals_epsilon_outside,
    test_near_within_distance,
    test_near_outside_distance,
    test_add_inplace,
    test_subtract_inplace,
    test_multiply_inplace,
    test_divide_inplace,
    test_negate_inplace,
};

const TestSuite vec2_suite = {
    "vec2",
    test_names,
    tests,
    (int)(sizeof(tests) / sizeof(tests[0])),
};
// clang-format on
