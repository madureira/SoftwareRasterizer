/*
 * ============================================================================
 * Zen Timer - x86_64
 * ============================================================================
 *
 * Modern port of Michael Abrash's Zen Timer for macOS x86_64.
 *
 * The original implementation uses:
 *
 *     - Intel 8253 Programmable Interval Timer
 *     - Intel 8259 Programmable Interrupt Controller
 *     - CLI / STI
 *     - DOS INT 21h
 *
 * Those mechanisms are not available to a normal user-space application on
 * macOS.
 *
 * This version therefore preserves:
 *
 *     - the original function names;
 *     - the original C interface;
 *     - the ZTimerOn/ZTimerOff measurement model;
 *     - reference measurements;
 *     - the 16-sample overhead average;
 *
 * while using:
 *
 *     mach_absolute_time()
 *
 * as the monotonic high-resolution timer source.
 *
 * Architecture:
 *
 *     x86_64
 *
 * Syntax:
 *
 *     AT&T
 *
 * ============================================================================
 */

.section __TEXT,__text

/*
 * ============================================================================
 * External symbols
 * ============================================================================
 *
 * These variables are defined by zen_timer.c.
 *
 * C symbols on macOS are referenced from assembly with a leading underscore.
 */

.extern _mach_absolute_time

.extern _ZTimerStartCount
.extern _TimedCount
.extern _ReferenceCount
.extern _OverflowFlag


/*
 * ============================================================================
 * ZTimerOn
 * ============================================================================
 *
 * Starts a timing measurement.
 *
 * Conceptually equivalent to:
 *
 *     ZTimerStartCount = mach_absolute_time();
 *
 * The original Abrash implementation programs the 8253 timer here.
 * The modern macOS implementation simply captures the current timestamp.
 *
 * ============================================================================
 */

.globl _ZTimerOn
.p2align 4, 0x90

_ZTimerOn:

    /*
     * Establish a minimal frame.
     *
     * The x86-64 ABI (both SysV and Microsoft) requires RSP to be
     * 16-byte aligned immediately before a CALL instruction. On entry,
     * RSP is 8 mod 16 (the caller's CALL pushed an 8-byte return
     * address), so a single push is enough to restore alignment before
     * calling mach_absolute_time() below.
     */
    pushq %rbp
    movq %rsp, %rbp

    /*
     * Call:
     *
     *     uint64_t mach_absolute_time(void);
     *
     * The return value is placed in RAX.
     */
    call _mach_absolute_time

    /*
     * Store the starting timestamp.
     *
     * Equivalent C:
     *
     *     ZTimerStartCount = value;
     */
    movq %rax, _ZTimerStartCount(%rip)

    /*
     * The modern implementation does not use a 16-bit hardware counter,
     * so there is no equivalent overflow condition.
     *
     * Equivalent C:
     *
     *     OverflowFlag = 0;
     */
    movb $0, _OverflowFlag(%rip)

    /*
     * Restore the frame and return to the caller.
     */
    popq %rbp
    ret


/*
 * ============================================================================
 * ZTimerOff
 * ============================================================================
 *
 * Stops the timing measurement and calculates the elapsed counter value.
 *
 * Conceptually:
 *
 *     current = mach_absolute_time();
 *     elapsed = current - ZTimerStartCount;
 *     TimedCount = elapsed;
 *
 * After the main measurement, the overhead of the timer mechanism itself is
 * measured 16 times using ReferenceZTimerOn() and ReferenceZTimerOff().
 *
 * ============================================================================
 */

.globl _ZTimerOff
.p2align 4, 0x90

_ZTimerOff:

    /*
     * ------------------------------------------------------------------------
     * Function prologue
     * ------------------------------------------------------------------------
     *
     * R12-R15 are callee-saved registers in the x86_64 ABI.
     * We use them during the measurement, so they must be preserved.
     */

    /*
     * Save RBP.
     */
    pushq %rbp

    /*
     * Save R12.
     */
    pushq %r12

    /*
     * Save R13.
     */
    pushq %r13

    /*
     * Save R14.
     */
    pushq %r14

    /*
     * Save R15.
     *
     * The stack remains 16-byte aligned before calls according to the
     * macOS x86_64 ABI requirements.
     */
    pushq %r15


    /*
     * ------------------------------------------------------------------------
     * Capture the final timestamp
     * ------------------------------------------------------------------------
     */

    /*
     * Call mach_absolute_time().
     *
     * RAX = current timestamp
     */
    call _mach_absolute_time

    /*
     * Move the current timestamp into R15.
     */
    movq %rax, %r15


    /*
     * ------------------------------------------------------------------------
     * Load the starting timestamp
     * ------------------------------------------------------------------------
     */

    /*
     * Load:
     *
     *     ZTimerStartCount
     *
     * into R12.
     */
    movq _ZTimerStartCount(%rip), %r12


    /*
     * Calculate:
     *
     *     current - start
     *
     * Equivalent C:
     *
     *     elapsed = current - ZTimerStartCount;
     */
    subq %r12, %r15


    /*
     * ------------------------------------------------------------------------
     * Store the measured interval
     * ------------------------------------------------------------------------
     */

    /*
     * Store the elapsed counter value in TimedCount.
     *
     * Equivalent C:
     *
     *     TimedCount = elapsed;
     */
    movq %r15, _TimedCount(%rip)


    /*
     * mach_absolute_time() uses a 64-bit monotonic counter.
     *
     * Unlike the original 8253 implementation, we do not have a 16-bit
     * countdown timer that can wrap during the measurement.
     */
    movb $0, _OverflowFlag(%rip)


    /*
     * ------------------------------------------------------------------------
     * Measure timer overhead
     * ------------------------------------------------------------------------
     *
     * The original Zen Timer measures a nearly empty interval to determine
     * the overhead introduced by the timer itself.
     *
     * We perform the measurement 16 times and accumulate the results.
     */

    /*
     * Clear R13.
     *
     * R13 will hold:
     *
     *     total = 0;
     */
    xorq %r13, %r13


    /*
     * Set the loop counter to 16.
     *
     * R14 = 16
     */
    movl $16, %r14d


.LReferenceLoop:

    /*
     * Start a reference measurement.
     */
    call _ReferenceZTimerOn


    /*
     * Stop the reference measurement.
     *
     * ReferenceZTimerOff() returns the elapsed counter value in RAX.
     */
    call _ReferenceZTimerOff


    /*
     * Add the measured overhead to the accumulated total.
     *
     * Equivalent C:
     *
     *     total += reference;
     */
    addq %rax, %r13


    /*
     * Decrement the loop counter.
     *
     * Equivalent C:
     *
     *     i--;
     */
    decq %r14


    /*
     * Repeat while the counter is not zero.
     *
     * DEC updates the Zero Flag:
     *
     *     i == 0 -> ZF = 1
     *     i != 0 -> ZF = 0
     */
    jne .LReferenceLoop


    /*
     * ------------------------------------------------------------------------
     * Calculate the average overhead
     * ------------------------------------------------------------------------
     *
     * We have:
     *
     *     total = sum of 16 measurements
     *
     * To calculate a rounded average:
     *
     *     average = (total + 8) / 16;
     *
     * Since 16 is a power of two, division by 16 can be performed with
     * a four-bit right shift.
     */

    /*
     * Add 8 for rounding before division.
     */
    addq $8, %r13


    /*
     * Divide by 16.
     *
     * Equivalent:
     *
     *     total >> 4
     *
     * which is equivalent to:
     *
     *     total / 16
     */
    shrq $4, %r13


    /*
     * Store the averaged overhead.
     *
     * Equivalent C:
     *
     *     ReferenceCount = average;
     */
    movq %r13, _ReferenceCount(%rip)


    /*
     * ------------------------------------------------------------------------
     * Function epilogue
     * ------------------------------------------------------------------------
     *
     * Restore the callee-saved registers in reverse order.
     */

    /*
     * Restore R15.
     */
    popq %r15

    /*
     * Restore R14.
     */
    popq %r14

    /*
     * Restore R13.
     */
    popq %r13

    /*
     * Restore R12.
     */
    popq %r12

    /*
     * Restore RBP.
     */
    popq %rbp

    /*
     * Return to the caller.
     */
    ret


/*
 * ============================================================================
 * ReferenceZTimerOn
 * ============================================================================
 *
 * Starts a reference measurement.
 *
 * The reference measurement is used to determine the overhead introduced
 * by the timer mechanism itself.
 *
 * Equivalent C:
 *
 *     ZTimerStartCount = mach_absolute_time();
 *
 * The main measurement has already been completed before this function is
 * called from ZTimerOff(), so reusing ZTimerStartCount here is safe.
 *
 * ============================================================================
 */

.globl _ReferenceZTimerOn
.p2align 4, 0x90

_ReferenceZTimerOn:

    /*
     * Establish a minimal frame to restore 16-byte stack alignment
     * before the CALL below (see the comment in ZTimerOn).
     */
    pushq %rbp
    movq %rsp, %rbp

    /*
     * Capture the current timestamp.
     *
     * RAX = mach_absolute_time()
     */
    call _mach_absolute_time


    /*
     * Store the timestamp as the beginning of the reference measurement.
     */
    movq %rax, _ZTimerStartCount(%rip)


    /*
     * Restore the frame and return to the caller.
     *
     * RAX still contains the timestamp, although the caller does not need
     * this value.
     */
    popq %rbp
    ret


/*
 * ============================================================================
 * ReferenceZTimerOff
 * ============================================================================
 *
 * Ends a reference measurement.
 *
 * Returns:
 *
 *     current - start
 *
 * in RAX.
 *
 * Equivalent C:
 *
 *     return mach_absolute_time() - ZTimerStartCount;
 *
 * ============================================================================
 */

.globl _ReferenceZTimerOff
.p2align 4, 0x90

_ReferenceZTimerOff:

    /*
     * Establish a minimal frame to restore 16-byte stack alignment
     * before the CALL below (see the comment in ZTimerOn).
     */
    pushq %rbp
    movq %rsp, %rbp

    /*
     * Capture the final timestamp.
     *
     * RAX = current timestamp
     */
    call _mach_absolute_time


    /*
     * Save the current timestamp in RDX.
     *
     * RDX is caller-saved, so no preservation is required.
     */
    movq %rax, %rdx


    /*
     * Load the starting timestamp into RAX.
     */
    movq _ZTimerStartCount(%rip), %rax


    /*
     * Calculate:
     *
     *     current - start
     *
     * RDX contains current.
     * RAX contains start.
     */
    subq %rax, %rdx


    /*
     * Move the elapsed value into RAX.
     *
     * RAX is the standard integer return register in the x86_64 ABI.
     */
    movq %rdx, %rax


    /*
     * Restore the frame and return.
     *
     *     RAX = elapsed counter value
     */
    popq %rbp
    ret
