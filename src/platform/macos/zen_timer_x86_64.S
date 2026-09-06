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
 * Starts a timing measurement:
 *
 *     ZTimerStartCount = mach_absolute_time();
 *
 * The original Abrash implementation programs the 8253 timer here.
 *
 * ============================================================================
 */

.globl _ZTimerOn
.p2align 4, 0x90

_ZTimerOn:

    /*
     * The x86-64 ABI requires RSP 16-byte aligned before a CALL. On
     * entry RSP is 8 mod 16 (the caller's CALL pushed a return
     * address), so an 8-byte reservation restores alignment. RBP is
     * never dereferenced here, so a plain SUB/ADD does this in one
     * instruction each instead of PUSH/MOV %rbp, without touching the
     * caller's RBP.
     */
    subq $8, %rsp

    call _mach_absolute_time
    movq %rax, _ZTimerStartCount(%rip)

    /*
     * No 16-bit hardware counter here, so no overflow condition
     * exists.
     */
    movb $0, _OverflowFlag(%rip)

    addq $8, %rsp
    ret


/*
 * ============================================================================
 * ZTimerOff
 * ============================================================================
 *
 * Stops the timing measurement:
 *
 *     elapsed = mach_absolute_time() - ZTimerStartCount;
 *     TimedCount = elapsed;
 *
 * then measures the ZTimerOn/ZTimerOff overhead 16 times and averages
 * the result.
 *
 * ============================================================================
 */

.globl _ZTimerOff
.p2align 4, 0x90

_ZTimerOff:

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

    call _mach_absolute_time
    movq %rax, %r15                     // current

    movq _ZTimerStartCount(%rip), %r12  // start
    subq %r12, %r15                     // elapsed = current - start

    movq %r15, _TimedCount(%rip)

    /*
     * mach_absolute_time() uses a 64-bit monotonic counter, so there's
     * no 16-bit countdown timer that can wrap during the measurement.
     */
    movb $0, _OverflowFlag(%rip)

    /*
     * Measure timer overhead: 16 back-to-back reads, averaged. R12 is
     * free to reuse as scratch here (its earlier value, ZTimerStartCount,
     * was already consumed above), holding each iteration's start
     * timestamp in a register instead of memory.
     */
    xorq %r13, %r13          // total = 0
    movl $16, %r14d          // loop counter

.LReferenceLoop:

    call _mach_absolute_time
    movq %rax, %r12                     // start

    call _mach_absolute_time
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
    movq %r13, _ReferenceCount(%rip)

    addq $8, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    ret
