/*
 * ============================================================================
 * Zen Timer
 * ============================================================================
 *
 * High-resolution interval timer for code performance measurement.
 * Wraps routines with ZTimerOn()/ZTimerOff(), then call ZTimerReport()
 * to print elapsed time. Used to benchmark and compare rasterizer routines.
 *
 * Based on Michael Abrash's Zen Timer from "Zen of Assembly Language"
 * and "Graphics Programming Black Book". Original version ran on DOS,
 * using Intel 8253 PIT directly since no OS clock API existed yet.
 *
 * ============================================================================
 * Why hand-written assembly?
 * ============================================================================
 *
 * ARM64 (Linux, macOS): Read CNTVCT_EL0 directly with MRS instruction.
 * No C99 equivalent exists; alternatives like clock_gettime() add overhead.
 *
 * x86_64 (Linux, macOS, Windows): Same OS API calls used across platforms.
 * Assembly ensures fixed instruction count around clock reads, avoiding
 * instability in timing overhead measurements.
 *
 * Keeps all platforms consistent with Abrash's original DOS implementation,
 * which had no choice but to use assembly.
 * ============================================================================
 */

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
     * Public API equivalent to Abrash's original ZTimerOn().
     *
     * On the original DOS implementation, interrupts were manipulated
     * to guarantee a precise measurement.
     *
     * User-space code cannot do this on modern OSes, so platform-specific
     * assembly implementations exist in:
     *   src/platform/<os>/zen_timer_<arch>.S or .asm
     *
     * Uses each OS's monotonic clock API:
     *   macOS: mach_absolute_time()
     *   Linux: clock_gettime(CLOCK_MONOTONIC)
     *   Windows: QueryPerformanceCounter() (via MASM, as MSVC cannot
     *            process GAS-syntax .S files)
     *
     * WebAssembly version implemented in C using emscripten_get_now().
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
