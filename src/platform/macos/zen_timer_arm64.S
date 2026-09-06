//
// ============================================================================
// Zen Timer - Apple Silicon implementation
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
//    Apple Silicon / ARM64
//    macOS
//    mach_absolute_time()
//
// IMPORTANT:
//
// The original Zen Timer disabled interrupts with CLI and restored
// the original interrupts flag with POPF.
//
// Normal macOS user-space apps cannot manipulate the CPU
// interrupt mask in this way.
//
// Hence this implementation preserves the API and timing model,
// but does not disable interrupts.
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

.extern _mach_absolute_time

//
// ============================================================================
// Global variables defined in zen_timer.c
// ============================================================================
//

.extern _ZTimerStartCount
.extern _TimedCount
.extern _ReferenceCount
.extern _OverflowFlag

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
// ARM64/macOS:
//
//    call mach_absolute_time()
//    store returned timestamp
//
// mach_absolute_time() returns:
//
//    x0 = current absolute timestamp
//
// ============================================================================
//

.globl _ZTimerOn
.p2align 2
_ZTimerOn:

    //
    // Save the link register and frame pointer.
    //
    // On ARM64, BL stores the return address in X30.
    //
    stp     x29, x30, [sp, #-16]!

    //
    // Estabilsh the standard ARM64 stack frame.
    //
    mov     x29, sp

    //
    // Call macOS's mach_absolute_time().
    //
    // Equivalent conceptual operation to reading the 8253 timer
    // in the original implementation.
    //
    bl      _mach_absolute_time

    //
    // mach_absolute_time() returned the current timestamp in X0.
    //
    // Store that timestamp as the begining of the measurement.
    //
    adrp    x1, _ZTimerStartCount@PAGE
    //
    // Add the page-relative offset of ZTimerStartCount.
    //
    add     x1, x1, _ZTimerStartCount@PAGEOFF

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
// ARM64/macOS:
//
//     read mach_absolute_time()
//     subtract start timestamp
//     measure ZTimerOn/ZTimerOff overhead
//     store result
//
// ============================================================================
//

.globl _ZTimerOff
.p2align 2
_ZTimerOff:

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
    // mach_absolute_time() returns:
    //
    //     x0 = current timestamp
    //
    bl      _mach_absolute_time

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

    adrp    x1, _ZTimerStartCount@PAGE

    //
    // Calculate the address of ZTimerStartCount.
    //
    add     x1, x1, _ZTimerStartCount@PAGEOFF

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

    adrp    x1, _TimedCount@PAGE

    //
    // Calculate the address of TimedCount.
    //
    add     x1, x1, _TimedCount@PAGEOFF

    //
    // Store the elapsed raw mach_absolute_time() count.
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
    // mach_absolute_time() does not use that mechanism.
    //
    // Therefore:
    //
    //     OverflowFlag = 0
    //
    //

    adrp    x1, _OverflowFlag@PAGE

    //
    // Calculate address of OverflowFlag.
    //
    add     x1, x1, _OverflowFlag@PAGEOFF

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

_ZTimerReferenceLoop:

    //
    // ------------------------------------------------------------------------
    // ReferenceZTimerOn
    // ------------------------------------------------------------------------
    //
    // This is equivalent to starting the hardware timer for an
    // empty code fragment.
    //
    bl      _ReferenceZTimerOn

    //
    // ------------------------------------------------------------------------
    // ReferenceZTimerOff
    // ------------------------------------------------------------------------
    //
    // This captures the elapsed time of the empty fragment.
    //
    bl      _ReferenceZTimerOff

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
    b.ne    _ZTimerReferenceLoop

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
    adrp    x1, _ReferenceCount@PAGE

    //
    // Calculate address of ReferenceCount.
    //
    add     x1, x1, _ReferenceCount@PAGEOFF

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
//     mach_absolute_time()
//
// ============================================================================
//

.globl _ReferenceZTimerOn
.p2align 2
_ReferenceZTimerOn:

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
    bl      _mach_absolute_time

    //
    // Store timestamp in ZTimerStartCount.
    //
    adrp    x1, _ZTimerStartCount@PAGE

    //
    // Calculate address.
    //
    add     x1, x1, _ZTimerStartCount@PAGEOFF

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
//     x0 = elapsed mach_absolute_time() units
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

.globl _ReferenceZTimerOff
.p2align 2
_ReferenceZTimerOff:

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
    bl      _mach_absolute_time

    //
    // Current timestamp is in X0.
    //
    mov     x1, x0

    //
    // Load the timestamp captured by ReferenceZTimerOn().
    //
    adrp    x2, _ZTimerStartCount@PAGE

    //
    // Calculate address.
    //
    add     x2, x2, _ZTimerStartCount@PAGEOFF

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
