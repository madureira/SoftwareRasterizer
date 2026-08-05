#include <string.h>

#include "test.h"

#include "core/utils.h"

/*
 * str_trim_left
 */

static void test_trim_left_no_whitespace(TestContext* ctx)
{
    const char* result = str_trim_left("hello");
    TEST_ASSERT(ctx, strcmp(result, "hello") == 0);
}

static void test_trim_left_leading_spaces(TestContext* ctx)
{
    const char* result = str_trim_left("   hello");
    TEST_ASSERT(ctx, strcmp(result, "hello") == 0);
}

static void test_trim_left_leading_tabs(TestContext* ctx)
{
    const char* result = str_trim_left("\t\thello");
    TEST_ASSERT(ctx, strcmp(result, "hello") == 0);
}

static void test_trim_left_mixed_whitespace(TestContext* ctx)
{
    const char* result = str_trim_left(" \t \nhello");
    TEST_ASSERT(ctx, strcmp(result, "hello") == 0);
}

static void test_trim_left_trailing_whitespace_preserved(TestContext* ctx)
{
    const char* result = str_trim_left("hello   ");
    TEST_ASSERT(ctx, strcmp(result, "hello   ") == 0);
}

static void test_trim_left_empty_string(TestContext* ctx)
{
    const char* result = str_trim_left("");
    TEST_ASSERT(ctx, strcmp(result, "") == 0);
}

static void test_trim_left_all_whitespace(TestContext* ctx)
{
    const char* result = str_trim_left("   ");
    TEST_ASSERT(ctx, strcmp(result, "") == 0);
}

static void test_trim_left_returns_pointer_into_original(TestContext* ctx)
{
    const char* input = "  hello";
    const char* result = str_trim_left(input);
    TEST_ASSERT(ctx, result == input + 2);
}

/*
 * str_trim_right
 */

static void test_trim_right_no_whitespace(TestContext* ctx)
{
    char text[] = "hello";
    str_trim_right(text);
    TEST_ASSERT(ctx, strcmp(text, "hello") == 0);
}

static void test_trim_right_trailing_spaces(TestContext* ctx)
{
    char text[] = "hello   ";
    str_trim_right(text);
    TEST_ASSERT(ctx, strcmp(text, "hello") == 0);
}

static void test_trim_right_trailing_tabs(TestContext* ctx)
{
    char text[] = "hello\t\t";
    str_trim_right(text);
    TEST_ASSERT(ctx, strcmp(text, "hello") == 0);
}

static void test_trim_right_mixed_whitespace(TestContext* ctx)
{
    char text[] = "hello \t\n";
    str_trim_right(text);
    TEST_ASSERT(ctx, strcmp(text, "hello") == 0);
}

static void test_trim_right_leading_whitespace_preserved(TestContext* ctx)
{
    char text[] = "   hello";
    str_trim_right(text);
    TEST_ASSERT(ctx, strcmp(text, "   hello") == 0);
}

static void test_trim_right_empty_string(TestContext* ctx)
{
    char text[] = "";
    str_trim_right(text);
    TEST_ASSERT(ctx, strcmp(text, "") == 0);
}

static void test_trim_right_all_whitespace(TestContext* ctx)
{
    char text[] = "   ";
    str_trim_right(text);
    TEST_ASSERT(ctx, strcmp(text, "") == 0);
}

/*
 * str_trim
 */

static void test_trim_no_whitespace(TestContext* ctx)
{
    char text[] = "hello";
    char* result = str_trim(text);
    TEST_ASSERT(ctx, strcmp(result, "hello") == 0);
}

static void test_trim_leading_only(TestContext* ctx)
{
    char text[] = "   hello";
    char* result = str_trim(text);
    TEST_ASSERT(ctx, strcmp(result, "hello") == 0);
}

static void test_trim_trailing_only(TestContext* ctx)
{
    char text[] = "hello   ";
    char* result = str_trim(text);
    TEST_ASSERT(ctx, strcmp(result, "hello") == 0);
}

static void test_trim_both_sides(TestContext* ctx)
{
    char text[] = "   hello   ";
    char* result = str_trim(text);
    TEST_ASSERT(ctx, strcmp(result, "hello") == 0);
}

static void test_trim_empty_string(TestContext* ctx)
{
    char text[] = "";
    char* result = str_trim(text);
    TEST_ASSERT(ctx, strcmp(result, "") == 0);
}

static void test_trim_all_whitespace(TestContext* ctx)
{
    char text[] = "   ";
    char* result = str_trim(text);
    TEST_ASSERT(ctx, strcmp(result, "") == 0);
}

static void test_trim_preserves_inner_whitespace(TestContext* ctx)
{
    char text[] = "  hello world  ";
    char* result = str_trim(text);
    TEST_ASSERT(ctx, strcmp(result, "hello world") == 0);
}

static void test_trim_mixed_whitespace_both_sides(TestContext* ctx)
{
    char text[] = "\t hello \n";
    char* result = str_trim(text);
    TEST_ASSERT(ctx, strcmp(result, "hello") == 0);
}

/*
 * str_equals
 */

static void test_equals_identical_strings(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, str_equals("hello", "hello"));
}

static void test_equals_different_strings(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, str_equals("hello", "world"));
}

static void test_equals_empty_strings(TestContext* ctx)
{
    TEST_ASSERT_TRUE(ctx, str_equals("", ""));
}

static void test_equals_empty_vs_nonempty(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, str_equals("", "hello"));
}

static void test_equals_case_sensitive(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, str_equals("Hello", "hello"));
}

static void test_equals_prefix_not_equal(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, str_equals("hello", "hello world"));
}

/*
 * Suite registration
 */

static TestRegistry reg;

static void setup(void)
{
    describe("str_trim_left")
    {
        test(test_trim_left_no_whitespace);
        test(test_trim_left_leading_spaces);
        test(test_trim_left_leading_tabs);
        test(test_trim_left_mixed_whitespace);
        test(test_trim_left_trailing_whitespace_preserved);
        test(test_trim_left_empty_string);
        test(test_trim_left_all_whitespace);
        test(test_trim_left_returns_pointer_into_original);
    }
    describe("str_trim_right")
    {
        test(test_trim_right_no_whitespace);
        test(test_trim_right_trailing_spaces);
        test(test_trim_right_trailing_tabs);
        test(test_trim_right_mixed_whitespace);
        test(test_trim_right_leading_whitespace_preserved);
        test(test_trim_right_empty_string);
        test(test_trim_right_all_whitespace);
    }
    describe("str_trim")
    {
        test(test_trim_no_whitespace);
        test(test_trim_leading_only);
        test(test_trim_trailing_only);
        test(test_trim_both_sides);
        test(test_trim_empty_string);
        test(test_trim_all_whitespace);
        test(test_trim_preserves_inner_whitespace);
        test(test_trim_mixed_whitespace_both_sides);
    }
    describe("str_equals")
    {
        test(test_equals_identical_strings);
        test(test_equals_different_strings);
        test(test_equals_empty_strings);
        test(test_equals_empty_vs_nonempty);
        test(test_equals_case_sensitive);
        test(test_equals_prefix_not_equal);
    }
}

const TestSuite utils_suite = { "utils", setup, &reg };
