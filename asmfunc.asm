bits   64
default rel
global compute_acceleration

section .data
kmh_to_ms  dd 0.277777791        ; constant for converting kmh to ms

section .text
; rcx = rows
; rdx = ptr to {vi,vf,t} floats (3*rows)
;  r8 = ptr to int results
compute_acceleration:
    push rbx
    push rsi
    push rdi

    mov  rbx, rcx        ; rows remaining
    mov  rsi, rdx        ; input ptr
    mov  rdi, r8         ; output ptr

.loop:
    test rbx, rbx
    jz   .done

    movss xmm0, [rsi]        ; vi
    movss xmm1, [rsi+4]      ; vf
    movss xmm2, [rsi+8]      ; t

    mulss xmm0, [rel kmh_to_ms]
    mulss xmm1, [rel kmh_to_ms]

    subss xmm1, xmm0         ; delta v
    divss xmm1, xmm2         ; a = delta v / t

    cvtss2si eax, xmm1      ; round to integer
    mov   [rdi], eax

    lea  rsi, [rsi+12]       ; next car (3 floats)
    add  rdi, 4              ; next output
    dec  rbx
    jmp  .loop

.done:
    pop  rdi
    pop  rsi
    pop  rbx
    ret
