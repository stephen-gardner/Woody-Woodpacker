bits 64
default rel
global _start
section .text
_start:
    push    rdi
    push    rsi
    push    rdx
    mov     rdi, [rel address]
    mov     rsi, [rel size]    
    mov     rdx, [rel key]
    add     rsi, rdi
.loop:    
    cmp     rdi, rsi
    jge     .done
    xor     byte [rdi], dl
    inc     rdi
    ror     rdx, 8
    jmp     .loop
.done:
    pop     rdx
    pop     rsi
    pop     rdi
    jmp     [rel ep]

    ep      dq 0x1111111122222222
    address dq 0x3333333344444444
    size    dq 0x5555555566666666
    key     dq 0x7777777788888888
