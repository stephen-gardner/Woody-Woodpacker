bits 64
default rel
global _start

_start:     pushf                               ; save registers
            push    rdi                         ;
            push    rsi                         ;
            push    rdx                         ;
            push    rax                         ;

            mov     rax, 1                      ; print msg
            mov     rdi, 1                      ;
            lea     rsi, [rel msg]              ;
            mov     rdx, entry - msg            ;
            syscall

            mov     rax, [rel pos]              ; is pie ?
            test    rax, rax                    ;
            jz      decrypt                     ;

            call    delta                       ; calc delta
delta:      pop     rax                         ;
            sub     rax, delta - _start         ;
            sub     rax, [rel pos]              ;
            add     [rel address], rax          ; fix addresses
            add     [rel entry], rax            ;

decrypt:    mov     rdi, [rel address]          ; decrypt
            mov     rsi, [rel size]             ;
            mov     rdx, [rel key]              ;
            add     rsi, rdi                    ;
loop:       cmp     rdi, rsi                    ;
            jge     done                        ;
            xor     byte [rdi], dl              ;
            inc     rdi                         ;
            ror     rdx, 8                      ;
            jmp     loop                        ;

done:       pop     rax                         ; restore registers
            pop     rdx                         ;
            pop     rsi                         ;
            pop     rdi                         ;
            popf
            jmp     [rel entry]                 ; return control

msg         db ".....WOODY.....",10             ; 
entry       dq 0x1111111122222222               ; vars
address     dq 0x3333333344444444               ;
size        dq 0x5555555566666666               ;
key         dq 0x7777777788888888               ;
pos         dq 0x0000000000000000               ; if pie: vaddr of decryptor
