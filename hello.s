bits 64
default rel
global _start
section .text
_start:
    mov rdx, 12
    lea rsi, [rel msg]
    mov rdi, 1
    mov rax, 1
    syscall

    xor rdi, rdi
    mov rax, 60
    syscall
    
msg db "hello world",10
