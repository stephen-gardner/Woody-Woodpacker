#!/bin/sh

rm -f hello_asm hello_asm.o;
nasm -f elf64 hello.s -o hello_asm.o;
gcc -s -no-pie -nostartfiles -nodefaultlibs -nostdlib hello_asm.o -o hello_asm

objcopy --remove-section     .gnu.hash             hello_asm
objcopy --remove-section     .note.gnu.build-id    hello_asm
objcopy --remove-section     .dynsym               hello_asm
objcopy --remove-section     .dynstr               hello_asm
objcopy --remove-section     .eh_frame             hello_asm



rm -f hello_c
gcc -no-pie hello.c -o hello_c
