#include "test.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static const char* relative_path(const char* file)
{
    const char* prefix = PROJECT_SOURCE_DIR "/";
    size_t prefix_len = strlen(prefix);
    if (strncmp(file, prefix, prefix_len) == 0)
    {
        return file + prefix_len;
    }
    return file;
}

int test_floats_equal(float a, float b, float eps)
{
    return fabsf(a - b) <= eps;
}

int test_doubles_equal(double a, double b, double eps)
{
    return fabs(a - b) <= eps;
}

void test_context_fail(TestContext* ctx, const char* file, int line, const char* expr)
{
    fprintf(stdout, "  FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "          %s:%d\n", relative_path(file), line);
    fprintf(stdout, "          %s\n", expr);
    ctx->failures++;
}

void test_bool_fail(TestContext* ctx, const char* file, int line, const char* expr, int actual,
                    int expected_true)
{
    fprintf(stdout, "  FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "          %s:%d\n", relative_path(file), line);
    fprintf(stdout, "          expected: %s\n", expected_true ? "true" : "false");
    fprintf(stdout, "          actual:   %s  (%s)\n", actual ? "true" : "false", expr);
    ctx->failures++;
}

void test_float_eq_fail(TestContext* ctx, const char* file, int line, const char* actual_expr,
                        float actual, const char* expected_expr, float expected)
{
    fprintf(stdout, "  FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "          %s:%d\n", relative_path(file), line);
    fprintf(stdout, "          expected: %g  (%s)\n", expected, expected_expr);
    fprintf(stdout, "          actual:   %g  (%s)\n", actual, actual_expr);
    ctx->failures++;
}

void test_double_eq_fail(TestContext* ctx, const char* file, int line, const char* actual_expr,
                         double actual, const char* expected_expr, double expected)
{
    fprintf(stdout, "  FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "          %s:%d\n", relative_path(file), line);
    fprintf(stdout, "          expected: %g  (%s)\n", expected, expected_expr);
    fprintf(stdout, "          actual:   %g  (%s)\n", actual, actual_expr);
    ctx->failures++;
}

void test_int_eq_fail(TestContext* ctx, const char* file, int line, const char* actual_expr,
                      int actual, const char* expected_expr, int expected)
{
    fprintf(stdout, "  FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "          %s:%d\n", relative_path(file), line);
    fprintf(stdout, "          expected: %d  (%s)\n", expected, expected_expr);
    fprintf(stdout, "          actual:   %d  (%s)\n", actual, actual_expr);
    ctx->failures++;
}

void test_ptr_fail(TestContext* ctx, const char* file, int line, const char* expr, const void* ptr,
                   int expect_null)
{
    fprintf(stdout, "  FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "          %s:%d\n", relative_path(file), line);
    if (expect_null)
    {
        fprintf(stdout, "          expected: NULL\n");
    }
    else
    {
        fprintf(stdout, "          expected: non-NULL\n");
    }
    fprintf(stdout, "          actual:   %p  (%s)\n", ptr, expr);
    ctx->failures++;
}

int run_suite(const TestSuite* suite)
{
    fprintf(stdout, "\nRunning suite: %s (%d tests)\n", suite->name, suite->count);

    int tests_failed = 0;

    for (int i = 0; i < suite->count; i++)
    {
        TestContext ctx = {suite->test_names[i], 0};
        suite->tests[i](&ctx);

        if (ctx.failures == 0)
        {
            fprintf(stdout, "  PASS  %s\n", ctx.test_name);
        }
        else
        {
            tests_failed++;
        }
    }

    return tests_failed;
}

void print_summary(int total_tests, int tests_failed)
{
    fprintf(stdout, "\nResults: %d passed, %d failed\n", total_tests - tests_failed, tests_failed);
}
