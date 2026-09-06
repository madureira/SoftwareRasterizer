;==============================================================================
; Zen Timer - Windows x86_64
;==============================================================================
;
; Original concept:
;    Michael Abrash
;    Graphics Programming Black Book
;
; Original implementation:
;
;     - Intel 8253 Programmable Interval Timer
;     - Intel 8259 Programmable Interrupt Controller
;     - CLI / STI
;     - DOS INT 21h
;
; This implementation:
;
;     x86_64
;     Windows
;     QueryPerformanceCounter(&counter)
;
; This version therefore preserves:
;
;     - the original function names;
;     - the original C interface;
;     - the ZTimerOn/ZTimerOff measurement model;
;     - reference measurements;
;     - the 16-sample overhead average;
;
; IMPORTANT:
;
; The original Zen Timer disabled interrupts with CLI and restored the
; original interrupts flag with POPF. Normal Windows user-space apps
; cannot manipulate the CPU interrupt mask this way, so this
; implementation preserves the API and timing model but does not
; disable interrupts.
;
; ----------------------------------------------------------------------------
;
; Two things make this file different from the macOS/Linux ports:
;
;   1. QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount) follows
;      the Microsoft x64 calling convention: the pointer argument goes in
;      RCX, and LARGE_INTEGER is effectively a plain 64-bit integer
;      written through that pointer, rather than a value returned
;      directly in RAX.
;
;   2. The Microsoft x64 ABI requires the CALLER to reserve 32 bytes of
;      "shadow space" on the stack immediately before every CALL, in
;      addition to the usual 16-byte stack alignment. The SysV ABI used
;      by macOS/Linux has no equivalent requirement.
;
; To keep ZTimerOn/ZTimerOff's calls to the underlying clock identical in
; shape to the macOS/Linux ports, this file wraps the
; QueryPerformanceCounter() call in a small file-local helper,
; zen_timer_read_clock, which marshals the output pointer and shadow
; space and returns a single 64-bit raw counter value in RAX, just like
; mach_absolute_time()/clock_gettime() do on the other platforms. Note
; that this raw value is in QueryPerformanceCounter "counts", not
; nanoseconds; converting counts to microseconds requires
; QueryPerformanceFrequency(), which is handled on the C side in
; zen_timer.c (the same way the macOS port converts mach_absolute_time()
; ticks using mach_timebase_info there).
;
; ----------------------------------------------------------------------------
;
; Architecture:
;
;     x86_64
;
; Syntax:
;
;     MASM (Intel)
;
;==============================================================================

.CODE

;==============================================================================
; External symbols
;==============================================================================
;
; ZTimerStartCount/TimedCount/ReferenceCount/OverflowFlag are defined by
; zen_timer.c.

EXTERN QueryPerformanceCounter:PROC

EXTERN ZTimerStartCount:QWORD
EXTERN TimedCount:QWORD
EXTERN ReferenceCount:QWORD
EXTERN OverflowFlag:BYTE


;==============================================================================
; zen_timer_read_clock (file-local helper, not part of the public API)
;==============================================================================
;
; Not declared PUBLIC, and not called from anywhere outside this file.
;
; Reads the performance counter via QueryPerformanceCounter() and returns
; the raw count in RAX.
;
; QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount) uses the
; Microsoft x64 ABI: RCX = lpPerformanceCount. The caller must also
; reserve 32 bytes of shadow space immediately before the CALL.
;
; Stack layout: on entry RSP is 8 mod 16 (the caller's CALL pushed an
; 8-byte return address). This procedure pushes no registers, so
; SUB RSP, 40 reserves 32 bytes of shadow space plus 8 bytes for our own
; LARGE_INTEGER storage, which also happens to restore 16-byte alignment
; (8 - 40 = -32, a multiple of 16).
;
;==============================================================================

zen_timer_read_clock PROC FRAME
    sub rsp, 40
    .ALLOCSTACK 40
    .ENDPROLOG

    lea rcx, [rsp+32]             ; rcx = &counter
    call QueryPerformanceCounter  ; writes the count through rcx

    mov rax, [rsp+32]             ; rax = counter

    add rsp, 40
    ret
zen_timer_read_clock ENDP


;==============================================================================
; ZTimerOn
;==============================================================================
;
; Starts a timing measurement:
;
;     ZTimerStartCount = zen_timer_read_clock();
;
; The original Abrash implementation programs the 8253 timer here.
;
;==============================================================================

PUBLIC ZTimerOn
ZTimerOn PROC FRAME
    ; 32 bytes of shadow space for the CALL below (see the file header).
    sub rsp, 40
    .ALLOCSTACK 40
    .ENDPROLOG

    call zen_timer_read_clock     ; rax = zen_timer_read_clock()
    mov ZTimerStartCount, rax     ; ZTimerStartCount = rax

    ; No 16-bit hardware counter here, so no overflow condition exists.
    mov OverflowFlag, 0           ; OverflowFlag = 0

    add rsp, 40
    ret
ZTimerOn ENDP


;==============================================================================
; ZTimerOff
;==============================================================================
;
; Stops the timing measurement:
;
;     elapsed = zen_timer_read_clock() - ZTimerStartCount;
;     TimedCount = elapsed;
;
; then measures the ZTimerOn/ZTimerOff overhead 16 times and averages
; the result.
;
;==============================================================================

PUBLIC ZTimerOff
ZTimerOff PROC FRAME
    ; R12-R14 are callee-saved registers in the Microsoft x64 ABI.
    ; We use them during the measurement, so they must be preserved.
    push r12
    .PUSHREG r12
    push r13
    .PUSHREG r13
    push r14
    .PUSHREG r14

    ; 32 bytes of mandatory shadow space, reused across all three CALLs
    ; below. Three pushes (24 bytes) plus 32 keeps RSP 16-byte aligned,
    ; so no extra padding is needed.
    sub rsp, 32
    .ALLOCSTACK 32
    .ENDPROLOG

    call zen_timer_read_clock     ; rax = zen_timer_read_clock()
    mov r12, ZTimerStartCount     ; r12 = start
    sub rax, r12                  ; rax = elapsed = current - start

    mov TimedCount, rax           ; TimedCount = elapsed

    ; No 16-bit hardware counter here, so no overflow condition exists.
    mov OverflowFlag, 0           ; OverflowFlag = 0

    ; Measure timer overhead: 16 back-to-back reads, averaged. R12 is
    ; free to reuse as scratch (its earlier value was already consumed).
    xor r13, r13                  ; total = 0
    mov r14d, 16                  ; loop counter

ZTimerReferenceLoop:
    call zen_timer_read_clock     ; rax = start
    mov r12, rax                  ; r12 = start

    call zen_timer_read_clock     ; rax = current
    sub rax, r12                  ; rax = elapsed = current - start

    add r13, rax                  ; total += elapsed
    dec r14                       ; r14--
    jnz ZTimerReferenceLoop       ; while (r14 != 0)

    ; average = (total + 8) / 16
    add r13, 8
    shr r13, 4
    mov ReferenceCount, r13       ; ReferenceCount = average

    add rsp, 32
    pop r14
    pop r13
    pop r12
    ret
ZTimerOff ENDP

END
