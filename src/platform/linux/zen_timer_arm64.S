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
//    clock_gettime(CLOCK_MONOTONIC, &ts)
//
// IMPORTANT:
//
// The original Zen Timer disabled interrupts with CLI and restored
// the original interrupts flag with POPF.
//
// Normal Linux user-space apps cannot manipulate the CPU interrupt mask
// in this way.
//
// Hence this implementation preserves the API and timing model,
// but does not disable interrupts.
//
// This file also uses ELF addressing conventions (adrp + :lo12:) instead
// of the Mach-O-specific @PAGE/@PAGEOFF used by the macOS port, and does
// not decorate C symbols with a leading underscore.
//
// ----------------------------------------------------------------------------
//
// ARM64 ABI:
//
//    x0-x7   = argument / return registers
//    x19-x28 = callee-saved registers
//    x29     = frame pointer
//    x30     = link register
//
// A function call is made with:
//
//    bl function
//
// Return is performed with:
//
//    ret
//
// ----------------------------------------------------------------------------

.text

//
// ============================================================================
// External symbols
// ============================================================================
//

.extern clock_gettime

//
// ============================================================================
// Global variables defined in zen_timer.c
// ============================================================================
//

.extern ZTimerStartCount
.extern TimedCount
.extern ReferenceCount
.extern OverflowFlag

//
// ============================================================================
// zen_timer_read_clock (file-local helper, not part of the public API)
// ============================================================================
//
// Reads CLOCK_MONOTONIC via clock_gettime() and returns the timestamp
// as a single 64-bit nanosecond count in X0:
//
//     nanoseconds = ts.tv_sec * 1000000000 + ts.tv_nsec
//
// clock_gettime(clockid_t clk_id, struct timespec *tp) uses the AAPCS64
// convention: W0 = clk_id, X1 = tp, and writes
// { int64_t tv_sec; int64_t tv_nsec; } (16 bytes on 64-bit Linux)
// through tp.
//
// CLOCK_MONOTONIC is 1 on Linux for every architecture (stable kernel
// UAPI value), so it is used here as a plain immediate.
//
// ============================================================================
//

.p2align 2

zen_timer_read_clock:

    //
    // Reserve 32 bytes: 16 for the saved frame pointer/link register,
    // 16 for struct timespec. SP must stay 16-byte aligned at all times
    // on AArch64.
    //
    stp     x29, x30, [sp, #-32]!
    mov     x29, sp

    mov     w0, #1              // CLOCK_MONOTONIC
    add     x1, sp, #16         // &ts, stored above the saved fp/lr
    bl      clock_gettime

    ldr     x0, [sp, #16]       // ts.tv_sec
    ldr     x1, [sp, #24]       // ts.tv_nsec

    // 1000000000 (0x3B9ACA00) does not fit a single MOVZ 16-bit
    // immediate, so it is built with MOVZ+MOVK instead of the `mov`
    // pseudo-instruction (some assemblers, e.g. GNU as, do not expand
    // `mov` into a multi-instruction immediate load automatically).
    movz    x2, #0xCA00
    movk    x2, #0x3B9A, lsl #16
    mul     x0, x0, x2
    add     x0, x0, x1          // X0 = tv_sec * 1e9 + tv_nsec

    ldp     x29, x30, [sp], #32
    ret

//
// ============================================================================
// ZTimerOn
// ============================================================================
//
// Starts timing.
//
// Original:
//
//    ZTimerOn
//
// Original sequence:
//
//    save context
//    save FLAGS
//    enable interrupts
//    program 8253
//    load timer
//    wait
//    disable interrupts
//    start timer
//    restore context
//
// ARM64/Linux:
//
//    call zen_timer_read_clock()
//    store returned timestamp
//
// zen_timer_read_clock() returns:
//
//    x0 = current timestamp (nanoseconds)
//
// ============================================================================
//

.globl ZTimerOn
.p2align 2
ZTimerOn:

    //
    // Save the link register and frame pointer.
    //
    // On ARM64, BL stores the return address in X30.
    //
    stp     x29, x30, [sp, #-16]!

    //
    // Establish the standard ARM64 stack frame.
    //
    mov     x29, sp

    //
    // Read the monotonic clock.
    //
    // Equivalent conceptual operation to reading the 8253 timer
    // in the original implementation.
    //
    bl      zen_timer_read_clock

    //
    // zen_timer_read_clock() returned the current timestamp in X0.
    //
    // Store that timestamp as the begining of the measurement.
    //
    adrp    x1, ZTimerStartCount
    //
    // Add the page-relative offset of ZTimerStartCount.
    //
    add     x1, x1, :lo12:ZTimerStartCount

    //
    // Store the 64-bit timestamp.
    //
    str     x0, [x1]

    //
    // Restore the previous frame pointer and link register.
    //
    ldp     x29, x30, [sp], #16

    //
    // Return to the caller.
    //
    ret


//
// ============================================================================
// ZTimerOff
// ============================================================================
//
// Stops timing.
//
// Original:
//
//     ZTimerOff
//
// Original sequence:
//
//     save registers
//     latch 8253
//     check PIC for overflow
//     enable interrupts
//     read timer
//     calculate elapsed count
//     measure overhead
//     restore interrupt state
//
// ARM64/Linux:
//
//     read zen_timer_read_clock()
//     subtract start timestamp
//     measure ZTimerOn/ZTimerOff overhead
//     store result
//
// ============================================================================
//

.globl ZTimerOff
.p2align 2
ZTimerOff:

    //
    // Save frame pointer and link register.
    //
    stp     x29, x30, [sp, #-48]!

    //
    // Establish frame pointer.
    //
    mov     x29, sp

    //
    // Save callee-saved registers x19-x22.
    //
    // Per the ARM64 AAPCS64 calling convention, x19-x28 must be
    // preserved across calls. This function uses x19-x22 as scratch
    // registers below, so their original values must be restored
    // before returning to the caller.
    //
    stp     x19, x20, [sp, #16]
    stp     x21, x22, [sp, #32]

    //
    // ------------------------------------------------------------------------
    // Read current timestamp.
    // ------------------------------------------------------------------------
    //
    // zen_timer_read_clock() returns:
    //
    //     x0 = current timestamp (nanoseconds)
    //
    bl      zen_timer_read_clock

    //
    // Preserve the current timestamp in x19.
    //
    // x19 is callee-saved according to the ARM64 ABI.
    //
    mov     x19, x0

    //
    // ------------------------------------------------------------------------
    // Load ZTimerStartCount.
    // ------------------------------------------------------------------------
    //

    adrp    x1, ZTimerStartCount

    //
    // Calculate the address of ZTimerStartCount.
    //
    add     x1, x1, :lo12:ZTimerStartCount

    //
    // Load the timestamp captured by ZTimerOn().
    //
    ldr     x20, [x1]

    //
    // ------------------------------------------------------------------------
    // Calculate elapsed time.
    // ------------------------------------------------------------------------
    //
    //     elapsed = current - start
    //
    // This replaces the original:
    //
    //     neg ax
    //
    // because the 8253 counted downward.
    //
    sub     x19, x19, x20

    //
    // ------------------------------------------------------------------------
    // Store TimedCount.
    // ------------------------------------------------------------------------
    //

    adrp    x1, TimedCount

    //
    // Calculate the address of TimedCount.
    //
    add     x1, x1, :lo12:TimedCount

    //
    // Store the elapsed raw clock count.
    //
    str     x19, [x1]

    //
    // ------------------------------------------------------------------------
    // No 8259 overflow detection exists here.
    // ------------------------------------------------------------------------
    //
    // The original code checked IRQ0:
    //
    //     and al, 1
    //
    // and stored the result in OverflowFlag.
    //
    // clock_gettime() does not use that mechanism.
    //
    // Therefore:
    //
    //     OverflowFlag = 0
    //
    //

    adrp    x1, OverflowFlag

    //
    // Calculate address of OverflowFlag.
    //
    add     x1, x1, :lo12:OverflowFlag

    //
    // W0 = zero.
    //
    mov     w0, wzr

    //
    // Store zero as the overflow state.
    //
    strb    w0, [x1]

    //
    // ------------------------------------------------------------------------
    // Measure timer overhead.
    // ------------------------------------------------------------------------
    //
    // The original Zen Timer performs:
    //
    //     ReferenceZTimerOn()
    //     ReferenceZTimerOff()
    //
    // sixteen times and averages the result.
    //
    // We preserve that idea.
    //

    //
    // X21 = accumulated reference time.
    //
    mov     x21, xzr

    //
    // X22 = loop counter.
    //
    mov     w22, #16


//
// ----------------------------------------------------------------------------
// Reference measurement loop
// ----------------------------------------------------------------------------
//

ZTimerReferenceLoop:

    //
    // ------------------------------------------------------------------------
    // ReferenceZTimerOn
    // ------------------------------------------------------------------------
    //
    // This is equivalent to starting the hardware timer for an
    // empty code fragment.
    //
    bl      ReferenceZTimerOn

    //
    // ------------------------------------------------------------------------
    // ReferenceZTimerOff
    // ------------------------------------------------------------------------
    //
    // This captures the elapsed time of the empty fragment.
    //
    bl      ReferenceZTimerOff

    //
    // Add the measured reference interval.
    //
    add     x21, x21, x0

    //
    // Decrement loop counter.
    //
    subs    w22, w22, #1

    //
    // Continue until all 16 measurements have been performed.
    //
    b.ne    ZTimerReferenceLoop

    //
    // ------------------------------------------------------------------------
    // Average the reference measurements.
    // ------------------------------------------------------------------------
    //
    // Original:
    //
    //     add ReferenceCount, 8
    //     shr ReferenceCount, 4
    //
    // This effectively rounds:
    //
    //     total / 16
    //
    // ARM64:
    //
    //     add 8
    //     logical shift right by 4
    //

    add     x21, x21, #8

    //
    // Divide by 16.
    //
    lsr     x21, x21, #4

    //
    // Store ReferenceCount.
    //
    adrp    x1, ReferenceCount

    //
    // Calculate address of ReferenceCount.
    //
    add     x1, x1, :lo12:ReferenceCount

    //
    // Store averaged reference overhead.
    //
    str     x21, [x1]

    //
    // Restore callee-saved registers x19-x22.
    //
    ldp     x19, x20, [sp, #16]
    ldp     x21, x22, [sp, #32]

    //
    // Restore frame pointer and link register.
    //
    ldp     x29, x30, [sp], #48

    //
    // Return to caller.
    //
    ret


//
// ============================================================================
// ReferenceZTimerOn
// ============================================================================
//
// Starts a timer measurement used exclusively to determine the
// overhead introduced by the Zen Timer itself.
//
// Original:
//
//     ReferenceZTimerOn
//
// Original hardware operation:
//
//     program 8253
//     load timer count
//
// ARM64 operation:
//
//     zen_timer_read_clock()
//
// ============================================================================
//

.globl ReferenceZTimerOn
.p2align 2
ReferenceZTimerOn:

    //
    // Save frame pointer and link register.
    //
    stp     x29, x30, [sp, #-16]!

    //
    // Establish frame pointer.
    //
    mov     x29, sp

    //
    // Read current absolute timestamp.
    //
    bl      zen_timer_read_clock

    //
    // Store timestamp in ZTimerStartCount.
    //
    adrp    x1, ZTimerStartCount

    //
    // Calculate address.
    //
    add     x1, x1, :lo12:ZTimerStartCount

    //
    // Store timestamp.
    //
    str     x0, [x1]

    //
    // Restore registers.
    //
    ldp     x29, x30, [sp], #16

    //
    // Return.
    //
    ret


//
// ============================================================================
// ReferenceZTimerOff
// ============================================================================
//
// Stops a reference measurement.
//
// Returns:
//
//     x0 = elapsed clock count (nanoseconds)
//
// Original:
//
//     read 8253
//     calculate count
//     add count to ReferenceCount
//
// In this implementation we return the elapsed count to the
// caller. ZTimerOff() performs the accumulation.
//
// ============================================================================
//

.globl ReferenceZTimerOff
.p2align 2
ReferenceZTimerOff:

    //
    // Save frame pointer and link register.
    //
    stp     x29, x30, [sp, #-16]!

    //
    // Establish frame pointer.
    //
    mov     x29, sp

    //
    // Read current timestamp.
    //
    bl      zen_timer_read_clock

    //
    // Current timestamp is in X0.
    //
    mov     x1, x0

    //
    // Load the timestamp captured by ReferenceZTimerOn().
    //
    adrp    x2, ZTimerStartCount

    //
    // Calculate address.
    //
    add     x2, x2, :lo12:ZTimerStartCount

    //
    // Load starting timestamp.
    //
    ldr     x3, [x2]

    //
    // Calculate elapsed time:
    //
    //     elapsed = current - start
    //
    sub     x0, x1, x3

    //
    // X0 now contains the reference measurement.
    //
    // This is also the return value of the function.
    //

    //
    // Restore frame pointer and link register.
    //
    ldp     x29, x30, [sp], #16

    //
    // Return with elapsed count in X0.
    //
    ret
