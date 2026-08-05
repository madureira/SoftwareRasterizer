#include "test.h"

#include "math/math_common.h"

static void test_clamp_below_minimum(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_clamp_f32(-5.0f, 0.0f, 1.0f), 0.0f, 0.0f);
}

static void test_clamp_above_maximum(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_clamp_f32(5.0f, 0.0f, 1.0f), 1.0f, 0.0f);
}

static void test_clamp_at_minimum(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_clamp_f32(0.0f, 0.0f, 1.0f), 0.0f, 0.0f);
}

static void test_clamp_at_maximum(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_clamp_f32(1.0f, 0.0f, 1.0f), 1.0f, 0.0f);
}

static void test_clamp_in_range(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_clamp_f32(0.5f, 0.0f, 1.0f), 0.5f, 0.0f);
}

static void test_lerp_at_zero(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_lerp_f32(2.0f, 8.0f, 0.0f), 2.0f, 0.0f);
}

static void test_lerp_at_one(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_lerp_f32(2.0f, 8.0f, 1.0f), 8.0f, 0.0f);
}

static void test_lerp_at_midpoint(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_lerp_f32(2.0f, 8.0f, 0.5f), 5.0f, 1e-6f);
}

static void test_lerp_extrapolates(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_lerp_f32(0.0f, 10.0f, 2.0f), 20.0f, 1e-6f);
}

static void test_degree_to_radians_zero(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_degree_to_radians(0.0f), 0.0f, 0.0f);
}

static void test_degree_to_radians_180(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_degree_to_radians(180.0f), MATH_PI, 1e-6f);
}

static void test_degree_to_radians_90(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_degree_to_radians(90.0f), MATH_PI / 2.0f, 1e-6f);
}

static void test_degree_to_radians_360(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_degree_to_radians(360.0f), 2.0f * MATH_PI, 1e-6f);
}

static void test_radians_to_degree_zero(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_radians_to_degree(0.0f), 0.0f, 0.0f);
}

static void test_radians_to_degree_pi(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_radians_to_degree(MATH_PI), 180.0f, 1e-4f);
}

static void test_radians_to_degree_half_pi(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_radians_to_degree(MATH_PI / 2.0f), 90.0f, 1e-4f);
}

static void test_degree_radians_roundtrip(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_radians_to_degree(math_degree_to_radians(45.0f)), 45.0f, 1e-4f);
}

static TestRegistry reg;

static void setup(void)
{
    describe("clamp")
    {
        test(test_clamp_below_minimum);
        test(test_clamp_above_maximum);
        test(test_clamp_at_minimum);
        test(test_clamp_at_maximum);
        test(test_clamp_in_range);
    }
    describe("lerp")
    {
        test(test_lerp_at_zero);
        test(test_lerp_at_one);
        test(test_lerp_at_midpoint);
        test(test_lerp_extrapolates);
    }
    describe("degree_to_radians")
    {
        test(test_degree_to_radians_zero);
        test(test_degree_to_radians_180);
        test(test_degree_to_radians_90);
        test(test_degree_to_radians_360);
    }
    describe("radians_to_degree")
    {
        test(test_radians_to_degree_zero);
        test(test_radians_to_degree_pi);
        test(test_radians_to_degree_half_pi);
        test(test_degree_radians_roundtrip);
    }
}

const TestSuite math_common_suite = { "math_common", setup, &reg };
