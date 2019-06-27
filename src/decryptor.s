bits 64
default rel
global _start

_start:
            xor     eax, eax
            cdq
            mov     dl, 10
            inc     eax
            mov     edi, eax
            lea     rsi, [rel msg]
            syscall

            lea     rax, [rsi - (msg-_start)]
            sub     rax, qword [rsi + (pos-msg)]
            mov     rdi, rax
            add     rax, qword [rsi + (entry-msg)]
            push    rax
            add     rdi, qword [rsi + (address-msg)]
            mov     rcx, qword [rsi + (size-msg)]
            mov     rax, qword [rsi + (key-msg)]

decrypt:    xor     byte [rdi], al
            ror     rax, 1
            inc     rdi
            loop    decrypt
            mul     ecx
            ret

msg         db "..WOODY..",10
entry       dq 0x1111111122222222
address     dq 0x3333333344444444
size        dq 0x5555555566666666
key         dq 0x7777777788888888
pos         dq 0xaaaaaaaabbbbbbbb
