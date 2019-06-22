bits 64
default rel
_start:
    pushf
    push    rdi
    push    rsi
    push    rdx
    push    rax

    mov     rax, 1
    mov     rdi, 1
    lea     rsi, [rel msg]
    mov     rdx, entry-msg
    syscall

    mov     rdi, [rel address]
    mov     rsi, [rel size]    
    mov     rdx, [rel key]
    add     rsi, rdi

.b: cmp     rdi, rsi
    jge     .c
    xor     byte [rdi], dl
    inc     rdi
    ror     rdx, 8
    jmp     .b

.c: pop     rax
    pop     rdx
    pop     rsi
    pop     rdi
    popf
    jmp     [rel entry]

    msg     db ".....WOODY.....",10
    entry   dq 0x1111111122222222
    address dq 0x3333333344444444
    size    dq 0x5555555566666666
    key     dq 0x7777777788888888
