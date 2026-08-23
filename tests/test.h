#ifndef TESTS_TEST_H
#define TESTS_TEST_H

typedef struct TestContext
{
    const char* test_name;
    int failures;
} TestContext;

typedef void (*TestFn)(TestContext* ctx);

typedef struct TestGroup
{
    const char* name;
    const char** test_names;
    TestFn* tests;
    int count;
} TestGroup;

#define TEST_MAX_GROUPS    32
#define TEST_MAX_PER_GROUP 32

typedef struct TestRegistry
{
    TestGroup groups[TEST_MAX_GROUPS];
    const char* test_names[TEST_MAX_GROUPS][TEST_MAX_PER_GROUP];
    TestFn test_fns[TEST_MAX_GROUPS][TEST_MAX_PER_GROUP];
    int group_count;
    int current_group;
} TestRegistry;

typedef struct TestSuite
{
    const char* name;
    void (*setup)(void);
    TestRegistry* registry;
} TestSuite;

typedef struct RunResult
{
    int total;
    int failed;
} RunResult;

void test_context_fail(TestContext* ctx, const char* file, int line, const char* expr);

int test_floats_equal(float a, float b, float eps);

void test_float_eq_fail(TestContext* ctx, const char* file, int line, const char* actual_expr,
                        float actual, const char* expected_expr, float expected);

void test_bool_fail(TestContext* ctx, const char* file, int line, const char* expr, int actual,
                    int expected_true);

int test_doubles_equal(double a, double b, double eps);

void test_double_eq_fail(TestContext* ctx, const char* file, int line, const char* actual_expr,
                         double actual, const char* expected_expr, double expected);

void test_int_eq_fail(TestContext* ctx, const char* file, int line, const char* actual_expr,
                      int actual, const char* expected_expr, int expected);

void test_ptr_fail(TestContext* ctx, const char* file, int line, const char* expr, const void* ptr,
                   int expect_null);

void test_describe_begin(const char* name);
void test_describe_end(void);
void test_register(const char* name, TestFn fn);

RunResult run_suite(const TestSuite* suite);

void print_summary(int total_tests, int tests_failed);

#define describe(name)                                                                             \
    for (int _d = (test_describe_begin(name), 1); _d; _d = (test_describe_end(), 0))

#define test(fn) test_register(#fn, fn)

#define TEST_ASSERT(ctx, expr)                                                                     \
    do                                                                                             \
    {                                                                                              \
        if (!(expr))                                                                               \
        {                                                                                          \
            test_context_fail((ctx), __FILE__, __LINE__, #expr);                                   \
        }                                                                                          \
    } while (0)

#define TEST_ASSERT_TRUE(ctx, expr)                                                                \
    do                                                                                             \
    {                                                                                              \
        int _v = !!(expr);                                                                         \
        if (!_v)                                                                                   \
        {                                                                                          \
            test_bool_fail((ctx), __FILE__, __LINE__, #expr, _v, 1);                               \
        }                                                                                          \
    } while (0)

#define TEST_ASSERT_FALSE(ctx, expr)                                                               \
    do                                                                                             \
    {                                                                                              \
        int _v = !!(expr);                                                                         \
        if (_v)                                                                                    \
        {                                                                                          \
            test_bool_fail((ctx), __FILE__, __LINE__, #expr, _v, 0);                               \
        }                                                                                          \
    } while (0)

#define TEST_ASSERT_FLOAT_EQ(ctx, a, b, eps)                                                       \
    do                                                                                             \
    {                                                                                              \
        float _a = (a);                                                                            \
        float _b = (b);                                                                            \
        if (!test_floats_equal(_a, _b, (eps)))                                                     \
        {                                                                                          \
            test_float_eq_fail((ctx), __FILE__, __LINE__, #a, _a, #b, _b);                         \
        }                                                                                          \
    } while (0)

#define TEST_ASSERT_DOUBLE_EQ(ctx, a, b, eps)                                                      \
    do                                                                                             \
    {                                                                                              \
        double _a = (a);                                                                           \
        double _b = (b);                                                                           \
        if (!test_doubles_equal(_a, _b, (eps)))                                                    \
        {                                                                                          \
            test_double_eq_fail((ctx), __FILE__, __LINE__, #a, _a, #b, _b);                        \
        }                                                                                          \
    } while (0)

#define TEST_ASSERT_INT_EQ(ctx, a, b)                                                              \
    do                                                                                             \
    {                                                                                              \
        int _a = (a);                                                                              \
        int _b = (b);                                                                              \
        if (_a != _b)                                                                              \
        {                                                                                          \
            test_int_eq_fail((ctx), __FILE__, __LINE__, #a, _a, #b, _b);                           \
        }                                                                                          \
    } while (0)

#define TEST_ASSERT_NULL(ctx, ptr)                                                                 \
    do                                                                                             \
    {                                                                                              \
        const void* _p = (const void*)(ptr);                                                       \
        if (_p != NULL)                                                                            \
        {                                                                                          \
            test_ptr_fail((ctx), __FILE__, __LINE__, #ptr, _p, 1);                                 \
        }                                                                                          \
    } while (0)

#define TEST_ASSERT_NOT_NULL(ctx, ptr)                                                             \
    do                                                                                             \
    {                                                                                              \
        const void* _p = (const void*)(ptr);                                                       \
        if (_p == NULL)                                                                            \
        {                                                                                          \
            test_ptr_fail((ctx), __FILE__, __LINE__, #ptr, _p, 0);                                 \
        }                                                                                          \
    } while (0)

#endif // TESTS_TEST_H
