#include "test.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static const char* relative_path(const char* file)
{
    const char* prefix = PROJECT_SOURCE_DIR "/";
    usize prefix_len = strlen(prefix);
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
    fprintf(stdout, "    FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "        %s:%d\n", relative_path(file), line);
    fprintf(stdout, "        %s\n", expr);
    ctx->failures++;
}

void test_bool_fail(TestContext* ctx, const char* file, int line, const char* expr, int actual,
                    int expected_true)
{
    fprintf(stdout, "    FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "        %s:%d\n", relative_path(file), line);
    fprintf(stdout, "        expected: %s\n", expected_true ? "true" : "false");
    fprintf(stdout, "        actual:   %s  (%s)\n", actual ? "true" : "false", expr);
    ctx->failures++;
}

void test_float_eq_fail(TestContext* ctx, const char* file, int line, const char* actual_expr,
                        float actual, const char* expected_expr, float expected)
{
    fprintf(stdout, "    FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "        %s:%d\n", relative_path(file), line);
    fprintf(stdout, "        expected: %g  (%s)\n", expected, expected_expr);
    fprintf(stdout, "        actual:   %g  (%s)\n", actual, actual_expr);
    ctx->failures++;
}

void test_double_eq_fail(TestContext* ctx, const char* file, int line, const char* actual_expr,
                         double actual, const char* expected_expr, double expected)
{
    fprintf(stdout, "    FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "        %s:%d\n", relative_path(file), line);
    fprintf(stdout, "        expected: %g  (%s)\n", expected, expected_expr);
    fprintf(stdout, "        actual:   %g  (%s)\n", actual, actual_expr);
    ctx->failures++;
}

void test_int_eq_fail(TestContext* ctx, const char* file, int line, const char* actual_expr,
                      int actual, const char* expected_expr, int expected)
{
    fprintf(stdout, "    FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "        %s:%d\n", relative_path(file), line);
    fprintf(stdout, "        expected: %d  (%s)\n", expected, expected_expr);
    fprintf(stdout, "        actual:   %d  (%s)\n", actual, actual_expr);
    ctx->failures++;
}

void test_ptr_fail(TestContext* ctx, const char* file, int line, const char* expr, const void* ptr,
                   int expect_null)
{
    fprintf(stdout, "    FAIL  %s\n", ctx->test_name);
    fprintf(stdout, "        %s:%d\n", relative_path(file), line);
    if (expect_null)
    {
        fprintf(stdout, "        expected: NULL\n");
    }
    else
    {
        fprintf(stdout, "        expected: non-NULL\n");
    }
    fprintf(stdout, "        actual:   %p  (%s)\n", ptr, expr);
    ctx->failures++;
}

static TestRegistry* active_registry = NULL;

void test_describe_begin(const char* name)
{
    if (active_registry->group_count >= TEST_MAX_GROUPS)
    {
        return;
    }

    int g = active_registry->group_count++;
    active_registry->groups[g].name = name;
    active_registry->groups[g].test_names = active_registry->test_names[g];
    active_registry->groups[g].tests = active_registry->test_fns[g];
    active_registry->groups[g].count = 0;
    active_registry->current_group = g;
}

void test_describe_end(void)
{
}

void test_register(const char* name, TestFn fn)
{
    TestGroup* g = &active_registry->groups[active_registry->current_group];
    if (g->count >= TEST_MAX_PER_GROUP)
    {
        return;
    }

    int i = g->count++;
    active_registry->test_names[active_registry->current_group][i] = name;
    active_registry->test_fns[active_registry->current_group][i] = fn;
}

RunResult run_suite(const TestSuite* suite)
{
    memset(suite->registry, 0, sizeof(TestRegistry));
    active_registry = suite->registry;
    suite->setup();
    active_registry = NULL;

    TestRegistry* reg = suite->registry;

    int total = 0;
    for (int g = 0; g < reg->group_count; g++)
    {
        total += reg->groups[g].count;
    }

    fprintf(stdout, "\nRunning suite: %s (%d tests) (%d groups)\n", suite->name, total,
            reg->group_count);

    int tests_failed = 0;

    for (int g = 0; g < reg->group_count; g++)
    {
        const TestGroup* group = &reg->groups[g];
        fprintf(stdout, "  Group: %s\n", group->name);

        for (int i = 0; i < group->count; i++)
        {
            TestContext ctx = { group->test_names[i], 0 };
            group->tests[i](&ctx);

            if (ctx.failures == 0)
            {
                fprintf(stdout, "    PASS  %s\n", ctx.test_name);
            }
            else
            {
                tests_failed++;
            }
        }
    }

    return (RunResult){ total, tests_failed };
}

void print_summary(int total_tests, int tests_failed)
{
    fprintf(stdout, "\nResults: %d passed, %d failed\n", total_tests - tests_failed, tests_failed);
}
