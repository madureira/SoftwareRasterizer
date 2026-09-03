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

static void test_rand_range_within_bounds(TestContext* ctx)
{
    for (int i = 0; i < 1000; ++i)
    {
        int value = math_rand_range(5, 10);
        TEST_ASSERT_TRUE(ctx, value >= 5 && value <= 10);
    }
}

static void test_rand_range_not_always_minimum(TestContext* ctx)
{
    bool saw_non_minimum = false;

    for (int i = 0; i < 1000; ++i)
    {
        if (math_rand_range(0, 100) != 0)
        {
            saw_non_minimum = true;
            break;
        }
    }

    TEST_ASSERT_TRUE(ctx, saw_non_minimum);
}

static void test_fixp16_i32_roundtrip_positive(TestContext* ctx)
{
    TEST_ASSERT_INT_EQ(ctx, math_fixp16_int_part(math_i32_to_fixp16(42)), 42);
}

static void test_fixp16_i32_roundtrip_negative(TestContext* ctx)
{
    TEST_ASSERT_INT_EQ(ctx, math_fixp16_int_part(math_i32_to_fixp16(-42)), -42);
}

static void test_fixp16_i32_roundtrip_zero(TestContext* ctx)
{
    TEST_ASSERT_INT_EQ(ctx, math_fixp16_int_part(math_i32_to_fixp16(0)), 0);
}

static void test_fixp16_f32_roundtrip_positive(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_f32_to_fixp16(3.5f)), 3.5f, 1e-4f);
}

static void test_fixp16_f32_roundtrip_negative(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_f32_to_fixp16(-3.5f)), -3.5f, 1e-4f);
}

static void test_fixp16_f32_roundtrip_zero(TestContext* ctx)
{
    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_f32_to_fixp16(0.0f)), 0.0f, 1e-4f);
}

static void test_fixp16_int_part_truncates_positive(TestContext* ctx)
{
    TEST_ASSERT_INT_EQ(ctx, math_fixp16_int_part(math_f32_to_fixp16(5.75f)), 5);
}

static void test_fixp16_int_and_frac_part_recombine_positive(TestContext* ctx)
{
    fixp16 value = math_f32_to_fixp16(5.5f);
    fixp16 recombined =
        (fixp16)(math_fixp16_int_part(value) * FIXP16_MAG) + math_fixp16_frac_part(value);

    TEST_ASSERT_INT_EQ(ctx, recombined, value);
}

static void test_fixp16_int_and_frac_part_recombine_negative(TestContext* ctx)
{
    fixp16 value = math_f32_to_fixp16(-5.5f);
    fixp16 recombined =
        (fixp16)(math_fixp16_int_part(value) * FIXP16_MAG) + math_fixp16_frac_part(value);

    TEST_ASSERT_INT_EQ(ctx, recombined, value);
}

static void test_fixp16_abs_positive(TestContext* ctx)
{
    TEST_ASSERT_INT_EQ(ctx, math_fixp16_abs(math_i32_to_fixp16(5)), math_i32_to_fixp16(5));
}

static void test_fixp16_abs_negative(TestContext* ctx)
{
    TEST_ASSERT_INT_EQ(ctx, math_fixp16_abs(math_i32_to_fixp16(-5)), math_i32_to_fixp16(5));
}

static void test_fixp16_abs_zero(TestContext* ctx)
{
    TEST_ASSERT_INT_EQ(ctx, math_fixp16_abs(0), 0);
}

static void test_fixp16_min_picks_smaller(TestContext* ctx)
{
    fixp16 a = math_i32_to_fixp16(2);
    fixp16 b = math_i32_to_fixp16(-3);

    TEST_ASSERT_INT_EQ(ctx, math_fixp16_min(a, b), b);
}

static void test_fixp16_max_picks_larger(TestContext* ctx)
{
    fixp16 a = math_i32_to_fixp16(2);
    fixp16 b = math_i32_to_fixp16(-3);

    TEST_ASSERT_INT_EQ(ctx, math_fixp16_max(a, b), a);
}

static void test_fixp16_clamp_below_minimum(TestContext* ctx)
{
    fixp16 value = math_i32_to_fixp16(-10);
    fixp16 minimum = math_i32_to_fixp16(0);
    fixp16 maximum = math_i32_to_fixp16(1);

    TEST_ASSERT_INT_EQ(ctx, math_fixp16_clamp(value, minimum, maximum), minimum);
}

static void test_fixp16_clamp_above_maximum(TestContext* ctx)
{
    fixp16 value = math_i32_to_fixp16(10);
    fixp16 minimum = math_i32_to_fixp16(0);
    fixp16 maximum = math_i32_to_fixp16(1);

    TEST_ASSERT_INT_EQ(ctx, math_fixp16_clamp(value, minimum, maximum), maximum);
}

static void test_fixp16_clamp_in_range(TestContext* ctx)
{
    fixp16 value = math_f32_to_fixp16(0.5f);
    fixp16 minimum = math_i32_to_fixp16(0);
    fixp16 maximum = math_i32_to_fixp16(1);

    TEST_ASSERT_INT_EQ(ctx, math_fixp16_clamp(value, minimum, maximum), value);
}

static void test_fixp16_mul_identity(TestContext* ctx)
{
    fixp16 one = math_i32_to_fixp16(1);
    fixp16 value = math_f32_to_fixp16(3.25f);

    TEST_ASSERT_INT_EQ(ctx, math_fixp16_mul(one, value), value);
}

static void test_fixp16_mul_known_values(TestContext* ctx)
{
    fixp16 a = math_i32_to_fixp16(2);
    fixp16 b = math_i32_to_fixp16(3);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_mul(a, b)), 6.0f, 1e-4f);
}

static void test_fixp16_mul_negative_operand(TestContext* ctx)
{
    fixp16 a = math_i32_to_fixp16(-2);
    fixp16 b = math_i32_to_fixp16(3);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_mul(a, b)), -6.0f, 1e-4f);
}

static void test_fixp16_mul_by_zero(TestContext* ctx)
{
    fixp16 a = math_f32_to_fixp16(123.45f);

    TEST_ASSERT_INT_EQ(ctx, math_fixp16_mul(a, 0), 0);
}

static void test_fixp16_try_mul_null_result_fails(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, math_fixp16_try_mul(math_i32_to_fixp16(1), math_i32_to_fixp16(1), NULL));
}

static void test_fixp16_try_mul_within_range_succeeds(TestContext* ctx)
{
    fixp16 result;
    bool ok = math_fixp16_try_mul(math_i32_to_fixp16(100), math_i32_to_fixp16(100), &result);

    TEST_ASSERT_TRUE(ctx, ok);
    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(result), 10000.0f, 1e-2f);
}

static void test_fixp16_try_mul_overflow_fails(TestContext* ctx)
{
    fixp16 result;
    bool ok = math_fixp16_try_mul(math_i32_to_fixp16(30000), math_i32_to_fixp16(30000), &result);

    TEST_ASSERT_FALSE(ctx, ok);
}

static void test_fixp16_div_known_values(TestContext* ctx)
{
    fixp16 a = math_i32_to_fixp16(6);
    fixp16 b = math_i32_to_fixp16(2);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_div(a, b)), 3.0f, 1e-4f);
}

static void test_fixp16_div_fraction_result(TestContext* ctx)
{
    fixp16 a = math_i32_to_fixp16(1);
    fixp16 b = math_i32_to_fixp16(4);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_div(a, b)), 0.25f, 1e-4f);
}

static void test_fixp16_div_identity(TestContext* ctx)
{
    fixp16 value = math_f32_to_fixp16(7.5f);
    fixp16 one = math_i32_to_fixp16(1);

    TEST_ASSERT_INT_EQ(ctx, math_fixp16_div(value, one), value);
}

static void test_fixp16_div_negative_operand(TestContext* ctx)
{
    fixp16 a = math_i32_to_fixp16(-6);
    fixp16 b = math_i32_to_fixp16(2);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_div(a, b)), -3.0f, 1e-4f);
}

static void test_fixp16_div_rounds_to_nearest(TestContext* ctx)
{
    fixp16 a = math_i32_to_fixp16(2);
    fixp16 b = math_i32_to_fixp16(3);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_div(a, b)), 0.6667f, 1e-3f);
}

static void test_fixp16_try_div_null_result_fails(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, math_fixp16_try_div(math_i32_to_fixp16(1), math_i32_to_fixp16(1), NULL));
}

static void test_fixp16_try_div_by_zero_fails(TestContext* ctx)
{
    fixp16 result;
    TEST_ASSERT_FALSE(ctx, math_fixp16_try_div(math_i32_to_fixp16(1), 0, &result));
}

static void test_fixp16_try_div_within_range_succeeds(TestContext* ctx)
{
    fixp16 result;
    bool ok = math_fixp16_try_div(math_i32_to_fixp16(10), math_i32_to_fixp16(2), &result);

    TEST_ASSERT_TRUE(ctx, ok);
    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(result), 5.0f, 1e-4f);
}

static void test_fixp16_try_div_overflow_fails(TestContext* ctx)
{
    fixp16 result;
    bool ok = math_fixp16_try_div(math_i32_to_fixp16(30000), 1, &result);

    TEST_ASSERT_FALSE(ctx, ok);
}

static void test_fixp16_lerp_at_zero(TestContext* ctx)
{
    fixp16 from = math_i32_to_fixp16(2);
    fixp16 to = math_i32_to_fixp16(8);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_lerp(from, to, 0)), 2.0f, 1e-4f);
}

static void test_fixp16_lerp_at_one(TestContext* ctx)
{
    fixp16 from = math_i32_to_fixp16(2);
    fixp16 to = math_i32_to_fixp16(8);
    fixp16 one = math_i32_to_fixp16(1);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_lerp(from, to, one)), 8.0f, 1e-4f);
}

static void test_fixp16_lerp_at_midpoint(TestContext* ctx)
{
    fixp16 from = math_i32_to_fixp16(2);
    fixp16 to = math_i32_to_fixp16(8);
    fixp16 half = math_f32_to_fixp16(0.5f);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_lerp(from, to, half)), 5.0f, 1e-4f);
}

static void test_fixp16_muldiv_known_values(TestContext* ctx)
{
    fixp16 a = math_i32_to_fixp16(2);
    fixp16 b = math_i32_to_fixp16(3);
    fixp16 c = math_i32_to_fixp16(4);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(math_fixp16_muldiv(a, b, c)), 1.5f, 1e-4f);
}

static void test_fixp16_muldiv_matches_separate_mul_then_div(TestContext* ctx)
{
    fixp16 a = math_f32_to_fixp16(7.0f);
    fixp16 b = math_f32_to_fixp16(3.0f);
    fixp16 c = math_f32_to_fixp16(5.0f);

    fixp16 combined = math_fixp16_muldiv(a, b, c);
    fixp16 separate = math_fixp16_div(math_fixp16_mul(a, b), c);

    TEST_ASSERT_FLOAT_EQ(ctx, math_fixp16_to_f32(combined), math_fixp16_to_f32(separate), 1e-3f);
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
    describe("rand_range")
    {
        test(test_rand_range_within_bounds);
        test(test_rand_range_not_always_minimum);
    }
    describe("fixp16_conversions")
    {
        test(test_fixp16_i32_roundtrip_positive);
        test(test_fixp16_i32_roundtrip_negative);
        test(test_fixp16_i32_roundtrip_zero);
        test(test_fixp16_f32_roundtrip_positive);
        test(test_fixp16_f32_roundtrip_negative);
        test(test_fixp16_f32_roundtrip_zero);
        test(test_fixp16_int_part_truncates_positive);
        test(test_fixp16_int_and_frac_part_recombine_positive);
        test(test_fixp16_int_and_frac_part_recombine_negative);
    }
    describe("fixp16_abs_min_max_clamp")
    {
        test(test_fixp16_abs_positive);
        test(test_fixp16_abs_negative);
        test(test_fixp16_abs_zero);
        test(test_fixp16_min_picks_smaller);
        test(test_fixp16_max_picks_larger);
        test(test_fixp16_clamp_below_minimum);
        test(test_fixp16_clamp_above_maximum);
        test(test_fixp16_clamp_in_range);
    }
    describe("fixp16_mul")
    {
        test(test_fixp16_mul_identity);
        test(test_fixp16_mul_known_values);
        test(test_fixp16_mul_negative_operand);
        test(test_fixp16_mul_by_zero);
        test(test_fixp16_try_mul_null_result_fails);
        test(test_fixp16_try_mul_within_range_succeeds);
        test(test_fixp16_try_mul_overflow_fails);
    }
    describe("fixp16_div")
    {
        test(test_fixp16_div_known_values);
        test(test_fixp16_div_fraction_result);
        test(test_fixp16_div_identity);
        test(test_fixp16_div_negative_operand);
        test(test_fixp16_div_rounds_to_nearest);
        test(test_fixp16_try_div_null_result_fails);
        test(test_fixp16_try_div_by_zero_fails);
        test(test_fixp16_try_div_within_range_succeeds);
        test(test_fixp16_try_div_overflow_fails);
    }
    describe("fixp16_lerp")
    {
        test(test_fixp16_lerp_at_zero);
        test(test_fixp16_lerp_at_one);
        test(test_fixp16_lerp_at_midpoint);
    }
    describe("fixp16_muldiv")
    {
        test(test_fixp16_muldiv_known_values);
        test(test_fixp16_muldiv_matches_separate_mul_then_div);
    }
}

const TestSuite math_common_suite = { "math_common", setup, &reg };
