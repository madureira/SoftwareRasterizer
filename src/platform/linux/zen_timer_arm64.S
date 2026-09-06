//
// ============================================================================
// Zen Timer - Linux ARM64 implementation
// ============================================================================
//
// Original concept:
//    Michael Abrash
//    Graphics Programming Black Book
//
// Original implementation:
//
//    8253 programmable inverval timer
//    8259 programmable interrupt controller
//    DOS
//    x86
//
// This implementation:
//
//    ARM64 (AArch64)
//    Linux
//    CNTVCT_EL0 (ARMv8 generic timer, read directly -- see below)
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
// None of the functions below perform a BL, so none need to save
// x29/x30 or maintain a frame -- AAPCS64 guarantees SP arrives already
// 16-byte aligned, and it's only touched where a function has its own
// callee-saved registers to preserve (ZTimerOff).
//
// ----------------------------------------------------------------------------

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
// Starts timing:
//
//    ZTimerStartCount = <read CNTVCT_EL0>;
//
// ============================================================================
//

.globl ZTimerOn
.p2align 2
ZTimerOn:

    isb
    mrs     x0, CNTVCT_EL0

    adrp    x1, ZTimerStartCount
    add     x1, x1, :lo12:ZTimerStartCount
    str     x0, [x1]                        // ZTimerStartCount = timestamp

    // No 16-bit hardware counter here, so no overflow condition exists.
    adrp    x1, OverflowFlag
    add     x1, x1, :lo12:OverflowFlag
    mov     w0, wzr
    strb    w0, [x1]

    ret


//
// ============================================================================
// ZTimerOff
// ============================================================================
//
// Stops timing, then measures the ZTimerOn/ZTimerOff overhead 16 times
// and averages the result. The reference measurement loop keeps its
// start/current timestamps in registers rather than round-tripping
// through memory, since there's no separate procedure call boundary to
// cross here.
//
// ============================================================================
//

.globl ZTimerOff
.p2align 2
ZTimerOff:

    // x19-x22 are the only callee-saved registers this function uses;
    // no BL happens here, so x29/x30 need no saving.
    stp     x19, x20, [sp, #-32]!
    stp     x21, x22, [sp, #16]

    isb
    mrs     x19, CNTVCT_EL0                 // current

    adrp    x1, ZTimerStartCount
    add     x1, x1, :lo12:ZTimerStartCount
    ldr     x20, [x1]                       // start

    sub     x19, x19, x20                   // elapsed = current - start

    adrp    x1, TimedCount
    add     x1, x1, :lo12:TimedCount
    str     x19, [x1]

    // No 8259-style overflow detection exists here.
    adrp    x1, OverflowFlag
    add     x1, x1, :lo12:OverflowFlag
    mov     w0, wzr
    strb    w0, [x1]

    // Measure timer overhead: 16 back-to-back reads, averaged.
    mov     x21, xzr                        // total = 0
    mov     w22, #16                        // loop counter

ZTimerReferenceLoop:

    isb
    mrs     x0, CNTVCT_EL0                  // start
    isb
    mrs     x1, CNTVCT_EL0                  // current

    sub     x0, x1, x0                      // elapsed = current - start
    add     x21, x21, x0                    // total += elapsed

    subs    w22, w22, #1
    b.ne    ZTimerReferenceLoop

    // average = (total + 8) / 16
    add     x21, x21, #8
    lsr     x21, x21, #4

    adrp    x1, ReferenceCount
    add     x1, x1, :lo12:ReferenceCount
    str     x21, [x1]

    ldp     x21, x22, [sp, #16]
    ldp     x19, x20, [sp], #32

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

    mrs     x0, CNTFRQ_EL0
    ret
