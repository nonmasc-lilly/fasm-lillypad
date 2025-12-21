format elf64 executable 3
entry _start

include "../flib.inc"

lillypad_definitions

segment readable executable


_start:
    cmp dword [rsp], 0x02
    jb .nargc
        push r12
        push r13
        mov r12, 0x18
        mov r13, [rsp + 0x10]
        shl r13, 3
        add r13, 0x18

        .loop:
            add r12, 0x08
            cmp r12, r13
            je .eloop
            call cstrlen, qword [rsp + r12]
            syscall SYS_WRITE, ~byte STDOUT, qword [rsp + r12], eax
            jmp .loop
        .eloop:

        pop r12
    .nargc:
    call print, qword msg
    syscall SYS_WRITE, ~byte STDOUT, lea [msg + 4], [msg]
    syscall SYS_EXIT, ~byte 0x45

print: ; (string)
    mov rsi, rdi
    lodsd
    mov edx, eax
    syscall SYS_WRITE, ~byte STDOUT, _, _
    ret

strdef msg, "Hello World!", 0x0A

