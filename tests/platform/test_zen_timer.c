#include <stdint.h>

#include "test.h"

#include "platform/zen_timer.h"

extern uint64_t TimedCount;
extern uint64_t ReferenceCount;
extern uint8_t OverflowFlag;

static volatile uint64_t sink = 0;

static void spin(uint64_t iterations)
{
    for (uint64_t i = 0; i < iterations; ++i)
    {
        sink += i;
    }
}

/*
 * overflow_flag
 */

static void test_overflow_flag_is_zero_after_measurement(TestContext* ctx)
{
    ZTimerOn();
    ZTimerOff();
    TEST_ASSERT_INT_EQ(ctx, OverflowFlag, 0);
}

/*
 * elapsed_time
 */

static void test_timed_count_increases_with_more_work(TestContext* ctx)
{
    ZTimerOn();
    spin(1000);
    ZTimerOff();
    uint64_t small = TimedCount;

    ZTimerOn();
    spin(10000000);
    ZTimerOff();
    uint64_t large = TimedCount;

    TEST_ASSERT(ctx, large > small);
}

/*
 * reference_count
 */

static void test_reference_count_is_smaller_than_a_real_measurement(TestContext* ctx)
{
    ZTimerOn();
    spin(10000000);
    ZTimerOff();

    TEST_ASSERT(ctx, TimedCount > ReferenceCount);
}

/*
 * report
 */

static void test_report_does_not_crash(TestContext* ctx)
{
    (void)ctx;
    ZTimerOn();
    spin(1000);
    ZTimerOff();
    ZTimerReport();
}

static TestRegistry reg;

static void setup(void)
{
    describe("overflow_flag")
    {
        test(test_overflow_flag_is_zero_after_measurement);
    }
    describe("elapsed_time")
    {
        test(test_timed_count_increases_with_more_work);
    }
    describe("reference_count")
    {
        test(test_reference_count_is_smaller_than_a_real_measurement);
    }
    describe("report")
    {
        test(test_report_does_not_crash);
    }
}

const TestSuite zen_timer_suite = { "zen_timer", setup, &reg };
