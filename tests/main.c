#include "test.h"

extern const TestSuite* suites[];
extern const int suite_count;

int main(void)
{
    int total_tests = 0;
    int total_failed = 0;

    for (int i = 0; i < suite_count; i++)
    {
        RunResult r = run_suite(suites[i]);
        total_tests += r.total;
        total_failed += r.failed;
    }

    print_summary(total_tests, total_failed);

    return total_failed > 0 ? 1 : 0;
}
