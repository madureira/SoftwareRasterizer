//
// ============================================================================
// Zen Timer - Linux ARM64
// ============================================================================
//
// Original concept:
//    Michael Abrash
//    Graphics Programming Black Book
//
// Original implementation:
//
//     - Intel 8253 Programmable Interval Timer
//     - Intel 8259 Programmable Interrupt Controller
//     - CLI / STI
//     - DOS INT 21h
//
// This implementation:
//
//    ARM64 (AArch64)
//    Linux
//    CNTVCT_EL0 (ARMv8 generic timer, read directly -- see below)
//
// This version therefore preserves:
//
//     - the original function names;
//     - the original C interface;
//     - the ZTimerOn/ZTimerOff measurement model;
//     - reference measurements;
//     - the 16-sample overhead average;
//
// IMPORTANT:
//
// The original Zen Timer disabled interrupts with CLI and restored the
// original interrupts flag with POPF. Normal Linux user-space apps
// cannot manipulate the CPU interrupt mask this way, so this
// implementation preserves the API and timing model but does not
// disable interrupts.
//
// This file also uses ELF addressing conventions (adrp + :lo12:) instead
// of the Mach-O-specific @PAGE/@PAGEOFF used by the macOS port, and does
// not decorate C symbols with a leading underscore.
//
// ----------------------------------------------------------------------------
//
// CNTVCT_EL0 is the ARMv8 generic timer's count register, readable
// directly from EL0 (user space) with a single MRS -- no function call,
// no output pointer, no per-read arithmetic like clock_gettime()'s
// tv_sec * 1e9 + tv_nsec. The Linux kernel enables EL0 access to this
// register by default.
//
// An ISB precedes each MRS to prevent the CPU from speculatively
// reordering the counter read against surrounding instructions, the
// same practice production vDSOs use.
//
// Unlike nanoseconds, the counter's tick rate is not architecturally
// fixed (it varies across ARM64 implementations), and is reported by
// CNTFRQ_EL0 (also EL0-readable). Converting raw ticks to nanoseconds
// using that frequency is handled in zen_timer.c, not here.
//
// None of the functions below perform a BL, so none of them touch SP
// or any callee-saved register (x19-x28) at all -- everything runs in
// temporary registers, which AAPCS64 lets a callee clobber freely
// without saving, since nothing is ever called out to that could
// observe them.
//
// ----------------------------------------------------------------------------
//
// Architecture:
//
//    ARM64 (AArch64)
//
// Syntax:
//
//    GAS (ARM64)
//
// ============================================================================

.text

.extern ZTimerStartCount
.extern TimedCount
.extern ReferenceCount
.extern OverflowFlag

//
// ============================================================================
// ZTimerOn
// ============================================================================
//
// Starts a timing measurement:
//
//    ZTimerStartCount = <read CNTVCT_EL0>;
//
// The original Abrash implementation programs the 8253 timer here.
//
// ============================================================================
//

.globl ZTimerOn
.p2align 2
ZTimerOn:

    isb
    mrs     x0, CNTVCT_EL0                  // x0 = CNTVCT_EL0

    adrp    x1, ZTimerStartCount            // x1 = page(ZTimerStartCount)
    add     x1, x1, :lo12:ZTimerStartCount  // x1 = &ZTimerStartCount
    str     x0, [x1]                        // ZTimerStartCount = x0

    // No 16-bit hardware counter here, so no overflow condition exists.
    adrp    x1, OverflowFlag
    add     x1, x1, :lo12:OverflowFlag
    mov     w0, wzr                         // w0 = 0
    strb    w0, [x1]                        // OverflowFlag = 0

    ret


//
// ============================================================================
// ZTimerOff
// ============================================================================
//
// Stops the timing measurement:
//
//    elapsed = <read CNTVCT_EL0> - ZTimerStartCount;
//    TimedCount = elapsed;
//
// then measures the ZTimerOn/ZTimerOff overhead 16 times and averages
// the result. The reference measurement loop keeps its start/current
// timestamps in registers rather than round-tripping through memory,
// since there's no separate procedure call boundary to cross here.
//
// ============================================================================
//

.globl ZTimerOff
.p2align 2
ZTimerOff:

    // No stack frame: nothing here is called out to, so x9-x12 need no saving.
    isb
    mrs     x9, CNTVCT_EL0                  // current

    adrp    x1, ZTimerStartCount
    add     x1, x1, :lo12:ZTimerStartCount
    ldr     x10, [x1]                       // start

    sub     x9, x9, x10                     // elapsed = current - start

    adrp    x1, TimedCount
    add     x1, x1, :lo12:TimedCount
    str     x9, [x1]                        // TimedCount = elapsed

    // No 16-bit hardware counter here, so no overflow condition exists.
    adrp    x1, OverflowFlag
    add     x1, x1, :lo12:OverflowFlag
    mov     w0, wzr                         // w0 = 0
    strb    w0, [x1]                        // OverflowFlag = 0

    // Measure timer overhead: 16 back-to-back reads, averaged. x9/x10 are
    // free to reuse as scratch (their earlier values were already consumed).
    mov     x11, xzr                        // total = 0
    mov     w12, #16                        // loop counter

ZTimerReferenceLoop:

    isb
    mrs     x9, CNTVCT_EL0                  // start
    isb
    mrs     x10, CNTVCT_EL0                 // current

    sub     x9, x10, x9                     // elapsed = current - start
    add     x11, x11, x9                    // total += elapsed

    subs    w12, w12, #1                    // w12--
    b.ne    ZTimerReferenceLoop             // while (w12 != 0)

    // average = (total + 8) / 16
    add     x11, x11, #8
    lsr     x11, x11, #4

    adrp    x1, ReferenceCount
    add     x1, x1, :lo12:ReferenceCount
    str     x11, [x1]                       // ReferenceCount = average

    ret


//
// ============================================================================
// zen_timer_read_frequency
// ============================================================================
//
// Returns the ARMv8 generic timer's tick rate in Hz (CNTFRQ_EL0), so
// zen_timer.c can convert the raw CNTVCT_EL0 ticks above into
// nanoseconds. Fixed at boot, so the C side calls this once and caches
// the result.
//
// ============================================================================
//

.globl zen_timer_read_frequency
.p2align 2
zen_timer_read_frequency:

    mrs     x0, CNTFRQ_EL0                  // x0 = CNTFRQ_EL0
    ret
