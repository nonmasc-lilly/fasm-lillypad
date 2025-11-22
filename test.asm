format elf64 executable 3
entry _start

include "flib.inc"

segment .text

_start:
    call print, qword msg
    syscall SYS_EXIT, ~byte 0x45

print: ; (string)
    mov rsi, rdi
    lodsd
    mov edx, eax
    syscall SYS_WRITE, ~byte STDOUT, _, _
    ret

strdef msg, "Hello World!", 0x0A
