;==============================================================================
; Zen Timer - x86_64 (Windows / MASM)
;==============================================================================
;
; Modern port of Michael Abrash's Zen Timer for Windows x86_64.
;
; The original implementation uses:
;
;     - Intel 8253 Programmable Interval Timer
;     - Intel 8259 Programmable Interrupt Controller
;     - CLI / STI
;     - DOS INT 21h
;
; Those mechanisms are not available to a normal user-space application on
; Windows.
;
; This version therefore preserves:
;
;     - the original function names;
;     - the original C interface;
;     - the ZTimerOn/ZTimerOff measurement model;
;     - reference measurements;
;     - the 16-sample overhead average;
;
; while using:
;
;     QueryPerformanceCounter(&counter)
;
; as the monotonic high-resolution timer source.
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
; To keep ZTimerOn/ZTimerOff/ReferenceZTimerOn/ReferenceZTimerOff
; identical in shape to the macOS/Linux ports, this file wraps the
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
; zen_timer_read_clock (internal helper, not part of the public C API)
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

    lea rcx, [rsp+32]           ; &counter, placed above the shadow space
    call QueryPerformanceCounter

    mov rax, [rsp+32]           ; raw QueryPerformanceCounter count

    add rsp, 40
    ret
zen_timer_read_clock ENDP


;==============================================================================
; ZTimerOn
;==============================================================================
;
; Starts a timing measurement.
;
; Conceptually equivalent to:
;
;     ZTimerStartCount = zen_timer_read_clock();
;
; The original Abrash implementation programs the 8253 timer here.
; The modern Windows implementation simply captures the current
; timestamp.
;
;==============================================================================

PUBLIC ZTimerOn
ZTimerOn PROC FRAME
    ; 32 bytes of shadow space for the CALL below (see the file header).
    sub rsp, 40
    .ALLOCSTACK 40
    .ENDPROLOG

    call zen_timer_read_clock

    ; Store the starting timestamp.
    ;
    ; Equivalent C:
    ;
    ;     ZTimerStartCount = value;
    mov ZTimerStartCount, rax

    ; The modern implementation does not use a 16-bit hardware counter,
    ; so there is no equivalent overflow condition.
    ;
    ; Equivalent C:
    ;
    ;     OverflowFlag = 0;
    mov OverflowFlag, 0

    add rsp, 40
    ret
ZTimerOn ENDP


;==============================================================================
; ZTimerOff
;==============================================================================
;
; Stops the timing measurement and calculates the elapsed counter value.
;
; Conceptually:
;
;     current = zen_timer_read_clock();
;     elapsed = current - ZTimerStartCount;
;     TimedCount = elapsed;
;
; After the main measurement, the overhead of the timer mechanism itself is
; measured 16 times using ReferenceZTimerOn() and ReferenceZTimerOff().
;
;==============================================================================

PUBLIC ZTimerOff
ZTimerOff PROC FRAME
    ; -------------------------------------------------------------------------
    ; Function prologue
    ; -------------------------------------------------------------------------
    ;
    ; R12-R15 are callee-saved registers in the Microsoft x64 ABI.
    ; We use them during the measurement, so they must be preserved.
    push r12
    .PUSHREG r12
    push r13
    .PUSHREG r13
    push r14
    .PUSHREG r14
    push r15
    .PUSHREG r15

    ; 32 bytes of shadow space, reused across all three CALLs below (the
    ; shadow space does not need to persist between calls, only to exist
    ; at the moment of each one).
    sub rsp, 40
    .ALLOCSTACK 40
    .ENDPROLOG


    ; -------------------------------------------------------------------------
    ; Capture the final timestamp
    ; -------------------------------------------------------------------------
    call zen_timer_read_clock
    mov r15, rax


    ; -------------------------------------------------------------------------
    ; Load the starting timestamp
    ; -------------------------------------------------------------------------
    mov r12, ZTimerStartCount

    ; Calculate:
    ;
    ;     current - start
    ;
    ; Equivalent C:
    ;
    ;     elapsed = current - ZTimerStartCount;
    sub r15, r12


    ; -------------------------------------------------------------------------
    ; Store the measured interval
    ; -------------------------------------------------------------------------
    mov TimedCount, r15

    ; QueryPerformanceCounter() uses a 64-bit monotonic counter.
    ;
    ; Unlike the original 8253 implementation, we do not have a 16-bit
    ; countdown timer that can wrap during the measurement.
    mov OverflowFlag, 0


    ; -------------------------------------------------------------------------
    ; Measure timer overhead
    ; -------------------------------------------------------------------------
    ;
    ; The original Zen Timer measures a nearly empty interval to determine
    ; the overhead introduced by the timer itself. We perform the
    ; measurement 16 times and accumulate the results.
    xor r13, r13            ; total = 0
    mov r14d, 16            ; loop counter

ZTimerReferenceLoop:
    call ReferenceZTimerOn
    call ReferenceZTimerOff     ; returns the elapsed counter value in RAX
    add r13, rax
    dec r14
    jnz ZTimerReferenceLoop


    ; -------------------------------------------------------------------------
    ; Calculate the average overhead
    ; -------------------------------------------------------------------------
    ;
    ;     average = (total + 8) / 16
    ;
    ; Since 16 is a power of two, division by 16 is a four-bit right
    ; shift.
    add r13, 8
    shr r13, 4
    mov ReferenceCount, r13


    ; -------------------------------------------------------------------------
    ; Function epilogue
    ; -------------------------------------------------------------------------
    add rsp, 40
    pop r15
    pop r14
    pop r13
    pop r12
    ret
ZTimerOff ENDP


;==============================================================================
; ReferenceZTimerOn
;==============================================================================
;
; Starts a reference measurement.
;
; The reference measurement is used to determine the overhead introduced
; by the timer mechanism itself.
;
; Equivalent C:
;
;     ZTimerStartCount = zen_timer_read_clock();
;
; The main measurement has already been completed before this function is
; called from ZTimerOff(), so reusing ZTimerStartCount here is safe.
;
;==============================================================================

PUBLIC ReferenceZTimerOn
ReferenceZTimerOn PROC FRAME
    sub rsp, 40
    .ALLOCSTACK 40
    .ENDPROLOG

    call zen_timer_read_clock
    mov ZTimerStartCount, rax

    add rsp, 40
    ret
ReferenceZTimerOn ENDP


;==============================================================================
; ReferenceZTimerOff
;==============================================================================
;
; Ends a reference measurement.
;
; Returns:
;
;     current - start
;
; in RAX.
;
; Equivalent C:
;
;     return zen_timer_read_clock() - ZTimerStartCount;
;
;==============================================================================

PUBLIC ReferenceZTimerOff
ReferenceZTimerOff PROC FRAME
    sub rsp, 40
    .ALLOCSTACK 40
    .ENDPROLOG

    call zen_timer_read_clock

    ; RDX is caller-saved, so no preservation is required.
    mov rdx, rax
    mov rax, ZTimerStartCount

    ; Calculate:
    ;
    ;     current - start
    ;
    ; RDX contains current, RAX contains start.
    sub rdx, rax

    ; RAX is the standard integer return register in the x86_64 ABI.
    mov rax, rdx

    add rsp, 40
    ret
ReferenceZTimerOff ENDP

END
