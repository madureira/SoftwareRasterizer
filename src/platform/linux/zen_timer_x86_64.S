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
 * On entry RSP is 8 mod 16 (the caller's CALL pushed a return address).
 * RBP is never dereferenced here, so subq $24 alone -- 16 bytes for
 * struct timespec plus 8 bytes of padding -- restores 16-byte alignment
 * before the inner CALL, without needing a PUSH/MOV %rbp first.
 *
 * ============================================================================
 */

.p2align 4, 0x90

zen_timer_read_clock:

    subq $24, %rsp

    movl $1, %edi          /* CLOCK_MONOTONIC */
    movq %rsp, %rsi        /* &ts */
    call clock_gettime

    movq (%rsp), %rax      /* ts.tv_sec */
    movq 8(%rsp), %rdx     /* ts.tv_nsec */

    imulq $1000000000, %rax, %rax
    addq %rdx, %rax        /* RAX = tv_sec * 1e9 + tv_nsec */

    addq $24, %rsp
    ret


/*
 * ============================================================================
 * ZTimerOn
 * ============================================================================
 *
 * Starts a timing measurement:
 *
 *     ZTimerStartCount = zen_timer_read_clock();
 *
 * The original Abrash implementation programs the 8253 timer here.
 *
 * ============================================================================
 */

.globl ZTimerOn
.p2align 4, 0x90

ZTimerOn:

    /*
     * The x86-64 ABI requires RSP 16-byte aligned before a CALL. On
     * entry RSP is 8 mod 16 (the caller's CALL pushed a return
     * address), so an 8-byte reservation restores alignment. RBP is
     * never dereferenced here, so a plain SUB/ADD does this in one
     * instruction each instead of PUSH/MOV %rbp, without touching the
     * caller's RBP.
     */
    subq $8, %rsp

    call zen_timer_read_clock
    movq %rax, ZTimerStartCount(%rip)

    /*
     * No 16-bit hardware counter here, so no overflow condition
     * exists.
     */
    movb $0, OverflowFlag(%rip)

    addq $8, %rsp
    ret


/*
 * ============================================================================
 * ZTimerOff
 * ============================================================================
 *
 * Stops the timing measurement:
 *
 *     elapsed = zen_timer_read_clock() - ZTimerStartCount;
 *     TimedCount = elapsed;
 *
 * then measures the ZTimerOn/ZTimerOff overhead 16 times and averages
 * the result.
 *
 * ============================================================================
 */

.globl ZTimerOff
.p2align 4, 0x90

ZTimerOff:

    /*
     * R12-R15 are callee-saved and hold values live across the CALLs
     * below, so they must be preserved.
     */
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    /*
     * On entry RSP is 8 mod 16; the four pushes above move a multiple
     * of 16 bytes, so it's still 8 mod 16 here. This padding restores
     * alignment for the CALLs below.
     */
    subq $8, %rsp

    call zen_timer_read_clock
    movq %rax, %r15                     // current

    movq ZTimerStartCount(%rip), %r12   // start
    subq %r12, %r15                     // elapsed = current - start

    movq %r15, TimedCount(%rip)

    /*
     * clock_gettime(CLOCK_MONOTONIC) uses a 64-bit monotonic counter,
     * so there's no 16-bit countdown timer that can wrap during the
     * measurement.
     */
    movb $0, OverflowFlag(%rip)

    /*
     * Measure timer overhead: 16 back-to-back reads, averaged. R12 is
     * free to reuse as scratch here (its earlier value, ZTimerStartCount,
     * was already consumed above), holding each iteration's start
     * timestamp in a register instead of memory.
     */
    xorq %r13, %r13          // total = 0
    movl $16, %r14d          // loop counter

.LReferenceLoop:

    call zen_timer_read_clock
    movq %rax, %r12                     // start

    call zen_timer_read_clock
    subq %r12, %rax                     // elapsed = current - start

    addq %rax, %r13                     // total += elapsed

    decq %r14
    jne .LReferenceLoop

    /*
     * average = (total + 8) / 16, using a shift since 16 is a power
     * of two.
     */
    addq $8, %r13
    shrq $4, %r13
    movq %r13, ReferenceCount(%rip)

    addq $8, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    ret
