#include "platform/zen_timer.h"

#include <stdint.h>
#include <stdio.h>

#if defined(__APPLE__) && !defined(__aarch64__)
#include <mach/mach_time.h>
#elif defined(_WIN32)
#include <windows.h>
#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

/*
 * These variables are intentionally global because the Assembly
 * implementation access them directly.
 *
 * They correspond conceptually to:
 *  - OriginalFlags
 *  - TimedCount
 *  - ReferenceCount
 *  - OverflowFlag
 *
 *  from Michael Abrash's original implementation.
 */

/*
 * Timestamp captured by ZTimerOn()
 */
uint64_t ZTimerStartCount = 0;

/*
 * Raw elapsed ticks (platform-specific units, see zen_timer_read_clock()
 * in the OS-specific assembly) captured by ZTimerOff().
 *
 * This is analogous to TimedCount in the original code.
 */
uint64_t TimedCount = 0;

/*
 * Reference measurement for the overhead introduced by:
 *  - ZTimerOn()
 *  - ZTimerOff()
 *
 *  The original Zen Timer measueres this overhead 16 times ans
 *  subtracts the average from the measured interval.
 */
uint64_t ReferenceCount = 0;

/*
 * None of the monotonic clocks used on macOS, Linux, or Windows expose
 * the hardware timer overflow mechanism used by the original 8253
 * implementation.
 *
 * Therefore this remains zero.
 */
uint8_t OverflowFlag = 0;

#if defined(__APPLE__) && !defined(__aarch64__)

/*
 * mach_absolute_time() returns hardware-dependent time units.
 *
 * The timebase converts those units into nanoseconds:
 *  nanoseconds = ticks * numerator / denominator
 */
static mach_timebase_info_data_t ZTimerTimebase;

/*
 * Makes sure that the mach_absolute_time() conversion info has been initialized.
 */
static void ZTimerInitializeTimebase(void)
{
    if (ZTimerTimebase.denom == 0)
    {
        mach_timebase_info(&ZTimerTimebase);
    }
}

/*
 * Converts mach_absolute_time() units to nanoseconds.
 *
 * Use 128-bit arithmetic when available through Clang so that
 * the multiplication cannot overflow for large counter values.
 */
static uint64_t ZTimerTicksToNanoseconds(uint64_t ticks)
{
    __uint128_t nanoseconds;

    ZTimerInitializeTimebase();

    nanoseconds = ((__uint128_t)ticks * ZTimerTimebase.numer) / ZTimerTimebase.denom;

    return (uint64_t)nanoseconds;
}

#elif defined(__linux__) && !defined(__aarch64__)

/*
 * On Linux/x86_64, zen_timer_read_clock() (in the src/platform/linux/
 * assembly) already combines clock_gettime(CLOCK_MONOTONIC)'s tv_sec and
 * tv_nsec into a single nanosecond count, so no conversion is needed.
 * This is an identity function, kept only for symmetry with the other
 * platforms.
 */
static uint64_t ZTimerTicksToNanoseconds(uint64_t ticks)
{
    return ticks;
}

#elif defined(__aarch64__) && (defined(__APPLE__) || defined(__linux__))

/*
 * On Apple Silicon and Linux/ARM64 systems, the ZTimerOn()/ZTimerOff() functions
 * (implemented in platform-specific assembly) directly access the ARMv8 generic
 * timer through CNTVCT_EL0 rather than using mach_absolute_time()/clock_gettime().
 * On Apple Silicon, this approach cannot leverage mach_timebase_info()'s conversion
 * ratio since mach_absolute_time() operates on its own ~24 MHz clock source
 * (numer=125/denom=3) rather than the ARM timer.
 *
 * Since the counter's frequency isn't standardized, it's determined once via
 * zen_timer_read_frequency() (accessing CNTFRQ_EL0 from the same assembly file)
 * and stored for reuse, similar to how mach_timebase_info() and QueryPerformanceFrequency()
 * work.
 */

extern uint64_t zen_timer_read_frequency(void);

static uint64_t ZTimerFrequency = 0;

static void ZTimerInitializeFrequency(void)
{
    if (ZTimerFrequency == 0)
    {
        ZTimerFrequency = zen_timer_read_frequency();
    }
}

/*
 * Converts CNTVCT_EL0 ticks to nanoseconds.
 *
 * Use 128-bit arithmetic so that the multiplication cannot overflow for
 * large counter values.
 */
static uint64_t ZTimerTicksToNanoseconds(uint64_t ticks)
{
    __uint128_t nanoseconds;

    ZTimerInitializeFrequency();

    nanoseconds = ((__uint128_t)ticks * 1000000000ull) / ZTimerFrequency;

    return (uint64_t)nanoseconds;
}

#elif defined(_WIN32)

/*
 * QueryPerformanceCounter() returns platform-dependent "counts" whose
 * rate varies by machine. QueryPerformanceFrequency() reports how many
 * counts occur per second, analogous to mach_timebase_info() on macOS.
 */
static uint64_t ZTimerFrequency = 0;

/*
 * Makes sure that the QueryPerformanceFrequency() result has been cached.
 */
static void ZTimerInitializeFrequency(void)
{
    if (ZTimerFrequency == 0)
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        ZTimerFrequency = (uint64_t)frequency.QuadPart;
    }
}

/*
 * Translates QueryPerformanceCounter() measurements into nanosecond units.
 *
 * Since MSVC lacks support for 128-bit integers (unlike Clang/GCC which use
 * __uint128_t on other platforms), the multiplication followed by division
 * operation is broken down into two parts: handling whole seconds and
 * processing the fractional part separately. This approach computes:
 * ticks = seconds * freq + remainder, where remainder is less than freq.
 * Given this constraint, the product remainder * 1e9 will never exceed
 * 64 bits for any practical QueryPerformanceCounter() frequency values.
 * This method delivers mathematically precise results rather than approximations,
 * yielding identical truncated outputs as a single 128-bit calculation would produce.
 */
static uint64_t ZTimerTicksToNanoseconds(uint64_t ticks)
{
    uint64_t seconds;
    uint64_t remainder;

    ZTimerInitializeFrequency();

    seconds = ticks / ZTimerFrequency;
    remainder = ticks % ZTimerFrequency;

    return seconds * 1000000000ull + (remainder * 1000000000ull) / ZTimerFrequency;
}

#elif defined(__EMSCRIPTEN__)

/*
 * Since WebAssembly doesn't have platform-specific assembly code like
 * macOS/Linux/Windows versions do, the ZTimerOn()/ZTimerOff() functions
 * (declared in zen_timer.h) are implemented here using standard C code,
 * utilizing emscripten_get_now() - which accesses JavaScript's performance.now()
 * in milliseconds. This uses the same monotonic time source that
 * platform_get_perf_counter() employs for this platform in platform.c.
 *
 * ReferenceZTimerOn()/ReferenceZTimerOff() are local helper functions defined
 * here, which get inlined by the compiler at their sole usage site within
 * ZTimerOff() below - achieving the same result as the manual assembly
 * inlining used on other platforms, where reference measurements are
 * directly embedded into the ZTimerOff() implementation.
 */

static uint64_t ZTimerReadClockNanoseconds(void)
{
    return (uint64_t)(emscripten_get_now() * 1e6);
}

static void ReferenceZTimerOn(void)
{
    ZTimerStartCount = ZTimerReadClockNanoseconds();
}

static uint64_t ReferenceZTimerOff(void)
{
    uint64_t current = ZTimerReadClockNanoseconds();

    return current - ZTimerStartCount;
}

void ZTimerOn(void)
{
    ZTimerStartCount = ZTimerReadClockNanoseconds();
    OverflowFlag = 0;
}

void ZTimerOff(void)
{
    uint64_t current = ZTimerReadClockNanoseconds();
    uint64_t total = 0;

    TimedCount = current - ZTimerStartCount;
    OverflowFlag = 0;

    for (int i = 0; i < 16; ++i)
    {
        ReferenceZTimerOn();
        total += ReferenceZTimerOff();
    }

    ReferenceCount = (total + 8) / 16;
}

/*
 * The clock above already reports nanoseconds directly - there's no
 * coarser "ticks" unit to convert from, so this is an identity
 * function, exactly like the Linux path.
 */
static uint64_t ZTimerTicksToNanoseconds(uint64_t ticks)
{
    return ticks;
}

#endif

/*
 * Converts the raw timer interval into nanoseconds - the highest
 * precision common to all three platform clocks - and prints it at
 * three scales (nanoseconds, microseconds, milliseconds) so the value
 * reads naturally regardless of how long the interval was.
 *
 * This replaces the original DOS int 21h / AH=09 output mechanism.
 */
void ZTimerReport(void)
{
    uint64_t net_count;
    uint64_t nanoseconds;
    double microseconds;
    double milliseconds;

    /*
     * Check for timer overflow.
     *
     * This is always zero on macOS, Linux, and Windows, because each of
     * their monotonic clocks is a wide, monotonically increasing counter
     * rather than the 16-bit wrapping 8253 timer used by Abrash.
     */
    if (OverflowFlag != 0)
    {
        printf("\n"
               "****************************************************\n"
               "*                    Zen Timer                     *\n"
               "****************************************************\n"
               "* The timer overflowed, so the interval timed was  *\n"
               "* too long for the precision timer to measure.     *\n"
               "* Please perform the timing test again with the    *\n"
               "* long-period timer.                               *\n"
               "****************************************************\n");

        return;
    }

    /*
     * Equivalent to:
     *  - mov ax,[TimedCount]
     *  - sub ax,[ReferenceCount]
     *
     * in the original code.
     *
     * Here both values are 64-bit raw platform-specific tick counts.
     */
    if (TimedCount > ReferenceCount)
    {
        net_count = TimedCount - ReferenceCount;
    }
    else
    {
        net_count = 0;
    }

    // Convert the raw timer units into nanoseconds, then derive the
    // coarser scales from that single, highest-precision value.
    nanoseconds = ZTimerTicksToNanoseconds(net_count);
    microseconds = (double)nanoseconds / 1000.0;
    milliseconds = (double)nanoseconds / 1000000.0;

    // Equivalent in spirit to the original "Timed count: XXXXX microseconds",
    // extended to show the same interval at all three human-readable scales.
    printf("\n"
           "****************************************************\n"
           "*                    Zen Timer                     *\n"
           "****************************************************\n"
           "Timed count:\n"
           "  %10llu ns\n"
           "  %10.3f us\n"
           "  %10.3f ms\n",
           (unsigned long long)nanoseconds, microseconds, milliseconds);
}
