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

static void test_fast_sin_at_0_degrees(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(0.0f), 0.0f, 1e-6f);
}

static void test_fast_sin_at_90_degrees(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(90.0f), 1.0f, 1e-6f);
}

static void test_fast_sin_at_180_degrees(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(180.0f), 0.0f, 1e-6f);
}

static void test_fast_sin_at_270_degrees(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(270.0f), -1.0f, 1e-6f);
}

static void test_fast_sin_at_360_degrees_wraps_to_zero(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(360.0f), 0.0f, 1e-6f);
}

static void test_fast_sin_negative_angle_wraps(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(-90.0f), -1.0f, 1e-6f);
}

static void test_fast_sin_angle_beyond_360_wraps(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(450.0f), 1.0f, 1e-6f);
}

static void test_fast_sin_large_negative_angle_wraps(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(-450.0f), -1.0f, 1e-6f);
}

static void test_fast_sin_interpolates_between_table_entries(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(45.5f), sinf(math_degree_to_radians(45.5f)), 1e-4f);
}

static void test_fast_sin_interpolates_near_360_boundary(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_sin(359.5f), sinf(math_degree_to_radians(359.5f)), 1e-4f);
}

static void test_fast_cos_at_0_degrees(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_cos(0.0f), 1.0f, 1e-6f);
}

static void test_fast_cos_at_90_degrees(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_cos(90.0f), 0.0f, 1e-6f);
}

static void test_fast_cos_at_180_degrees(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_cos(180.0f), -1.0f, 1e-6f);
}

static void test_fast_cos_at_270_degrees(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_cos(270.0f), 0.0f, 1e-6f);
}

static void test_fast_cos_at_360_degrees_wraps_to_one(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_cos(360.0f), 1.0f, 1e-6f);
}

static void test_fast_cos_negative_angle_wraps(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_cos(-90.0f), 0.0f, 1e-6f);
}

static void test_fast_cos_angle_beyond_360_wraps(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_cos(450.0f), 0.0f, 1e-6f);
}

static void test_fast_cos_interpolates_between_table_entries(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_cos(45.5f), cosf(math_degree_to_radians(45.5f)), 1e-4f);
}

static void test_fast_cos_interpolates_near_360_boundary(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fast_cos(359.5f), cosf(math_degree_to_radians(359.5f)), 1e-4f);
}

static void test_fast_sincos_matches_fast_sin(TestContext* ctx)
{
    f32 sine;
    f32 cosine;
    math_fast_sincos(123.75f, &sine, &cosine);

    TEST_ASSERT_FLOAT_EQ(ctx, sine, math_fast_sin(123.75f), 0.0f);
}

static void test_fast_sincos_matches_fast_cos(TestContext* ctx)
{
    f32 sine;
    f32 cosine;
    math_fast_sincos(123.75f, &sine, &cosine);

    TEST_ASSERT_FLOAT_EQ(ctx, cosine, math_fast_cos(123.75f), 0.0f);
}

static void test_fast_sincos_pythagorean_identity(TestContext* ctx)
{
    f32 sine;
    f32 cosine;
    math_fast_sincos(37.3f, &sine, &cosine);

    TEST_ASSERT_FLOAT_EQ(ctx, (sine * sine) + (cosine * cosine), 1.0f, 2e-4f);
}

static TestRegistry reg;

static void setup(void)
{
    math_build_sin_cos_tables();

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
    describe("fast_sin")
    {
        test(test_fast_sin_at_0_degrees);
        test(test_fast_sin_at_90_degrees);
        test(test_fast_sin_at_180_degrees);
        test(test_fast_sin_at_270_degrees);
        test(test_fast_sin_at_360_degrees_wraps_to_zero);
        test(test_fast_sin_negative_angle_wraps);
        test(test_fast_sin_angle_beyond_360_wraps);
        test(test_fast_sin_large_negative_angle_wraps);
        test(test_fast_sin_interpolates_between_table_entries);
        test(test_fast_sin_interpolates_near_360_boundary);
    }
    describe("fast_cos")
    {
        test(test_fast_cos_at_0_degrees);
        test(test_fast_cos_at_90_degrees);
        test(test_fast_cos_at_180_degrees);
        test(test_fast_cos_at_270_degrees);
        test(test_fast_cos_at_360_degrees_wraps_to_one);
        test(test_fast_cos_negative_angle_wraps);
        test(test_fast_cos_angle_beyond_360_wraps);
        test(test_fast_cos_interpolates_between_table_entries);
        test(test_fast_cos_interpolates_near_360_boundary);
    }
    describe("fast_sincos")
    {
        test(test_fast_sincos_matches_fast_sin);
        test(test_fast_sincos_matches_fast_cos);
        test(test_fast_sincos_pythagorean_identity);
    }
}

const TestSuite math_common_suite = { "math_common", setup, &reg };
