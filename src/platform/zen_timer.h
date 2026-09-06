#ifndef ZEN_TIMER_H
#define ZEN_TIMER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Starts the Zen timer.
     *
     * Equivalent public API to Abrash's original ZTimerOn().
     *
     * On the original DOS implementation, interrupts were manipulated
     * to guarantee a precise measurement.
     *
     * Normal user-space code cannot do that on macOS, Linux, or Windows,
     * so ZTimerOn() is implemented in dedicated assembly per platform
     * (src/platform/<os>/zen_timer_<arch>.S or .asm), using each OS's
     * own monotonic clock API instead: mach_absolute_time() on macOS,
     * clock_gettime(CLOCK_MONOTONIC) on Linux, and
     * QueryPerformanceCounter() on Windows (via MASM, since MSVC's
     * cl.exe cannot assemble the GAS-syntax .S files used elsewhere).
     *
     * WebAssembly has no equivalent hand-written assembly, so the
     * Emscripten build implements ZTimerOn() in plain C instead,
     * directly in zen_timer.c, using emscripten_get_now().
     */
    void ZTimerOn(void);

    /*
     * Stops the Zen timer and stores the measured interval.
     *
     * Equivalent public API to Abrash's original ZTimerOff().
     *
     * Implemented in the same OS-specific assembly files as ZTimerOn()
     * (and, on Emscripten, the same plain-C fallback in zen_timer.c).
     */
    void ZTimerOff(void);

    /*
     * Reports the measured time in nanoseconds, microseconds, and
     * milliseconds.
     *
     * Equivalent public API to Abrash's original ZTimerReport().
     */
    void ZTimerReport(void);

#ifdef __cplusplus
}
#endif

#endif // ZEN_TIMER_H
