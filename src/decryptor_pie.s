bits 64
default rel

.d:
    pushf
    push    rdi
    push    rsi
    push    rdx
    push    rcx
    push    rax

    mov     rax, 1
    mov     rdi, 1
    lea     rsi, [rel msg]
    mov     rdx, entry-msg
    syscall

    call    .c
.c  pop     rcx
    sub     rcx, .c-.d
    sub     rcx, [rel pos]

    mov     rdi, [rel address]
    add     rdi, rcx
    mov     rsi, [rel size]
    mov     rdx, [rel key]
    add     rsi, rdi


.a: cmp     rdi, rsi
    jge     .b
    xor     byte [rdi], dl
    inc     rdi
    ror     rdx, 8
    jmp     .a

.b: add [rel entry], rcx
    pop     rax
    pop     rcx
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
    pos     dq 0x99999999aaaaaaaa
