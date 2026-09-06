/*
 * ============================================================================
 * Zen Timer - Linux x86_64
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
 *     Linux
 *     clock_gettime(CLOCK_MONOTONIC, &ts)
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
 * original interrupts flag with POPF. Normal Linux user-space apps cannot
 * manipulate the CPU interrupt mask this way, so this implementation
 * preserves the API and timing model but does not disable interrupts.
 *
 * ----------------------------------------------------------------------------
 *
 * Unlike mach_absolute_time() (used by the macOS port), clock_gettime()
 * takes its arguments in registers and writes its result through an
 * output pointer (struct timespec), rather than returning a single value
 * in RAX.
 *
 * ----------------------------------------------------------------------------
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

    subq $24, %rsp                     // struct timespec + padding

    movl $1, %edi                      // CLOCK_MONOTONIC
    movq %rsp, %rsi                    // &ts
    call clock_gettime

    movq (%rsp), %rax                  // ts.tv_sec
    movq 8(%rsp), %rdx                 // ts.tv_nsec

    imulq $1000000000, %rax, %rax      // rax *= 1e9
    addq %rdx, %rax                    // rax += tv_nsec

    addq $24, %rsp                     // release struct timespec + padding
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
     * On entry RSP is 8 mod 16; an 8-byte reservation restores the
     * 16-byte alignment a CALL requires, without a PUSH/MOV %rbp (RBP
     * is never dereferenced here).
     */
    subq $8, %rsp

    call zen_timer_read_clock          // rax = zen_timer_read_clock()
    movq %rax, ZTimerStartCount(%rip)  // ZTimerStartCount = rax

    // No 16-bit hardware counter here, so no overflow condition exists.
    movb $0, OverflowFlag(%rip)        // OverflowFlag = 0

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

    // Save R12-R14 (callee-saved, used below); three pushes (24 bytes)
    // already restore 16-byte alignment from entry's 8-mod-16 state.
    pushq %r12
    pushq %r13
    pushq %r14

    call zen_timer_read_clock          // rax = zen_timer_read_clock()
    movq ZTimerStartCount(%rip), %r12  // r12 = start
    subq %r12, %rax                    // rax = elapsed = current - start

    movq %rax, TimedCount(%rip)        // TimedCount = elapsed

    // No 16-bit hardware counter here, so no overflow condition exists.
    movb $0, OverflowFlag(%rip)        // OverflowFlag = 0

    // Measure timer overhead: 16 back-to-back reads, averaged. R12 is
    // free to reuse as scratch (its earlier value was already consumed).
    xorq %r13, %r13                    // total = 0
    movl $16, %r14d                    // loop counter

.LReferenceLoop:

    call zen_timer_read_clock          // rax = start
    movq %rax, %r12                    // r12 = start

    call zen_timer_read_clock          // rax = current
    subq %r12, %rax                    // rax = elapsed = current - start

    addq %rax, %r13                    // total += elapsed

    decq %r14                          // r14--
    jne .LReferenceLoop                // while (r14 != 0)

    // average = (total + 8) / 16
    addq $8, %r13
    shrq $4, %r13
    movq %r13, ReferenceCount(%rip)    // ReferenceCount = average

    popq %r14
    popq %r13
    popq %r12
    ret

/*
 * Marks the stack as non-executable. Hand-written .S files don't get
 * this section automatically the way compiler-generated object files
 * do, and its absence makes the linker conservatively mark the whole
 * executable's stack as executable -- undesirable, and the source of
 * the "missing .note.GNU-stack section" linker warning.
 */
.section .note.GNU-stack,"",@progbits
