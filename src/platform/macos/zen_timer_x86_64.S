/*
 * ============================================================================
 * Zen Timer - macOS x86_64
 * ============================================================================
 *
 * Original concept:
 *    Michael Abrash
 *    Graphics Programming Black Book
 *
 * Original implementation:
 *
 *     - Intel 8253 Programmable Interval Timer
 *     - Intel 8259 Programmable Interrupt Controller
 *     - CLI / STI
 *     - DOS INT 21h
 *
 * This implementation:
 *
 *     x86_64
 *     macOS
 *     mach_absolute_time()
 *
 * This version therefore preserves:
 *
 *     - the original function names;
 *     - the original C interface;
 *     - the ZTimerOn/ZTimerOff measurement model;
 *     - reference measurements;
 *     - the 16-sample overhead average;
 *
 * IMPORTANT:
 *
 * The original Zen Timer disabled interrupts with CLI and restored the
 * original interrupts flag with POPF. Normal macOS user-space apps cannot
 * manipulate the CPU interrupt mask this way, so this implementation
 * preserves the API and timing model but does not disable interrupts.
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
     * On entry RSP is 8 mod 16; an 8-byte reservation restores the
     * 16-byte alignment a CALL requires, without a PUSH/MOV %rbp (RBP
     * is never dereferenced here).
     */
    subq $8, %rsp

    call _mach_absolute_time            // rax = mach_absolute_time()
    movq %rax, _ZTimerStartCount(%rip)  // ZTimerStartCount = rax

    // No 16-bit hardware counter here, so no overflow condition exists.
    movb $0, _OverflowFlag(%rip)        // OverflowFlag = 0

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

    // Save R12-R14 (callee-saved, used below); three pushes (24 bytes)
    // already restore 16-byte alignment from entry's 8-mod-16 state.
    pushq %r12
    pushq %r13
    pushq %r14

    call _mach_absolute_time            // rax = mach_absolute_time()
    movq _ZTimerStartCount(%rip), %r12  // r12 = start
    subq %r12, %rax                     // rax = elapsed = current - start

    movq %rax, _TimedCount(%rip)        // TimedCount = elapsed

    // No 16-bit hardware counter here, so no overflow condition exists.
    movb $0, _OverflowFlag(%rip)        // OverflowFlag = 0

    // Measure timer overhead: 16 back-to-back reads, averaged. R12 is
    // free to reuse as scratch (its earlier value was already consumed).
    xorq %r13, %r13                     // total = 0
    movl $16, %r14d                     // loop counter

.LReferenceLoop:

    call _mach_absolute_time            // rax = start
    movq %rax, %r12                     // r12 = start

    call _mach_absolute_time            // rax = current
    subq %r12, %rax                     // rax = elapsed = current - start

    addq %rax, %r13                     // total += elapsed

    decq %r14                           // r14--
    jne .LReferenceLoop                 // while (r14 != 0)

    // average = (total + 8) / 16
    addq $8, %r13
    shrq $4, %r13
    movq %r13, _ReferenceCount(%rip)    // ReferenceCount = average

    popq %r14
    popq %r13
    popq %r12
    ret
