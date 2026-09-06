/*
 * ============================================================================
 * Zen Timer - x86_64
 * ============================================================================
 *
 * Modern port of Michael Abrash's Zen Timer for Linux x86_64.
 *
 * The original implementation uses:
 *
 *     - Intel 8253 Programmable Interval Timer
 *     - Intel 8259 Programmable Interrupt Controller
 *     - CLI / STI
 *     - DOS INT 21h
 *
 * Those mechanisms are not available to a normal user-space application on
 * Linux.
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
 *     clock_gettime(CLOCK_MONOTONIC, &ts)
 *
 * as the monotonic high-resolution timer source. Unlike mach_absolute_time()
 * (used by the macOS port), clock_gettime() takes its arguments in registers
 * and writes its result through an output pointer (struct timespec), rather
 * than returning a single value in RAX
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

.text

/*
 * ============================================================================
 * External symbols
 * ============================================================================
 *
 * ZTimerStartCount/TimedCount/ReferenceCount/OverflowFlag are defined by
 * zen_timer.c. Unlike Mach-O, ELF does not decorate C symbols with a
 * leading underscore.
 */

.extern clock_gettime

.extern ZTimerStartCount
.extern TimedCount
.extern ReferenceCount
.extern OverflowFlag


/*
 * ============================================================================
 * zen_timer_read_clock (file-local helper, not part of the public API)
 * ============================================================================
 *
 * Reads CLOCK_MONOTONIC via clock_gettime() and returns the timestamp as a
 * single 64-bit nanosecond count in RAX:
 *
 *     nanoseconds = ts.tv_sec * 1000000000 + ts.tv_nsec
 *
 * clock_gettime(clockid_t clk_id, struct timespec *tp) uses the SysV ABI:
 * EDI = clk_id, RSI = tp, and writes { int64_t tv_sec; int64_t tv_nsec; }
 * (16 bytes on 64-bit Linux) through tp.
 *
 * CLOCK_MONOTONIC is 1 on Linux for every architecture (stable kernel UAPI
 * value), so it is used here as a plain immediate.
 *
 * Stack layout: on entry RSP is 8 mod 16 (the caller's CALL pushed an
 * 8-byte return address). subq $24 reserves 16 bytes for struct timespec
 * plus 8 bytes of padding, which restores 16-byte alignment before the
 * inner CALL, as required by the x86-64 ABI.
 *
 * ============================================================================
 */

.p2align 4, 0x90

zen_timer_read_clock:

    pushq %rbp
    movq %rsp, %rbp

    subq $24, %rsp

    movl $1, %edi          /* CLOCK_MONOTONIC */
    movq %rsp, %rsi        /* &ts */
    call clock_gettime

    movq (%rsp), %rax      /* ts.tv_sec */
    movq 8(%rsp), %rdx     /* ts.tv_nsec */

    imulq $1000000000, %rax, %rax
    addq %rdx, %rax        /* RAX = tv_sec * 1e9 + tv_nsec */

    addq $24, %rsp
    popq %rbp
    ret


/*
 * ============================================================================
 * ZTimerOn
 * ============================================================================
 *
 * Starts a timing measurement.
 *
 * Conceptually equivalent to:
 *
 *     ZTimerStartCount = zen_timer_read_clock();
 *
 * The original Abrash implementation programs the 8253 timer here.
 * The modern Linux implementation simply captures the current timestamp.
 *
 * ============================================================================
 */

.globl ZTimerOn
.p2align 4, 0x90

ZTimerOn:

    /*
     * Establish a minimal frame.
     *
     * The x86-64 ABI requires RSP to be 16-byte aligned immediately
     * before a CALL instruction. On entry, RSP is 8 mod 16 (the caller's
     * CALL pushed an 8-byte return address), so a single push is enough
     * to restore alignment before calling zen_timer_read_clock below.
     */
    pushq %rbp
    movq %rsp, %rbp

    call zen_timer_read_clock

    /*
     * Store the starting timestamp.
     *
     * Equivalent C:
     *
     *     ZTimerStartCount = value;
     */
    movq %rax, ZTimerStartCount(%rip)

    /*
     * The modern implementation does not use a 16-bit hardware counter,
     * so there is no equivalent overflow condition.
     *
     * Equivalent C:
     *
     *     OverflowFlag = 0;
     */
    movb $0, OverflowFlag(%rip)

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
 *     current = zen_timer_read_clock();
 *     elapsed = current - ZTimerStartCount;
 *     TimedCount = elapsed;
 *
 * After the main measurement, the overhead of the timer mechanism itself is
 * measured 16 times using ReferenceZTimerOn() and ReferenceZTimerOff().
 *
 * ============================================================================
 */

.globl ZTimerOff
.p2align 4, 0x90

ZTimerOff:

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
     * x86_64 ABI requirements.
     */
    pushq %r15


    /*
     * ------------------------------------------------------------------------
     * Capture the final timestamp
     * ------------------------------------------------------------------------
     */

    /*
     * Call zen_timer_read_clock().
     *
     * RAX = current timestamp
     */
    call zen_timer_read_clock

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
    movq ZTimerStartCount(%rip), %r12


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
    movq %r15, TimedCount(%rip)


    /*
     * clock_gettime(CLOCK_MONOTONIC) uses a 64-bit monotonic counter.
     *
     * Unlike the original 8253 implementation, we do not have a 16-bit
     * countdown timer that can wrap during the measurement.
     */
    movb $0, OverflowFlag(%rip)


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
    call ReferenceZTimerOn


    /*
     * Stop the reference measurement.
     *
     * ReferenceZTimerOff() returns the elapsed counter value in RAX.
     */
    call ReferenceZTimerOff


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
    movq %r13, ReferenceCount(%rip)


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
 *     ZTimerStartCount = zen_timer_read_clock();
 *
 * The main measurement has already been completed before this function is
 * called from ZTimerOff(), so reusing ZTimerStartCount here is safe.
 *
 * ============================================================================
 */

.globl ReferenceZTimerOn
.p2align 4, 0x90

ReferenceZTimerOn:

    /*
     * Establish a minimal frame to restore 16-byte stack alignment
     * before the CALL below (see the comment in ZTimerOn).
     */
    pushq %rbp
    movq %rsp, %rbp

    /*
     * Capture the current timestamp.
     *
     * RAX = zen_timer_read_clock()
     */
    call zen_timer_read_clock


    /*
     * Store the timestamp as the beginning of the reference measurement.
     */
    movq %rax, ZTimerStartCount(%rip)


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
 *     return zen_timer_read_clock() - ZTimerStartCount;
 *
 * ============================================================================
 */

.globl ReferenceZTimerOff
.p2align 4, 0x90

ReferenceZTimerOff:

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
    call zen_timer_read_clock


    /*
     * Save the current timestamp in RDX.
     *
     * RDX is caller-saved, so no preservation is required.
     */
    movq %rax, %rdx


    /*
     * Load the starting timestamp into RAX.
     */
    movq ZTimerStartCount(%rip), %rax


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
