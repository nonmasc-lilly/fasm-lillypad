
macro ldr_proto kind, expr, rgb, rgw, rgd, rgq
    match =lea, kind
        kind rgq, expr
    else
        match =byte e, expr
            kind rgb, byte e
        else match =word e, expr
            kind rgw, word e
        else match =dword e, expr
            kind rgd, dworgd e
        else match =qword e, expr
            kind rgq, qword e
        else
            kind rgd, dword expr
        end match
    end match
end macro

macro ldr_typed expr, rgb, rgw, rgd, rgq
    match =lea e, expr
        ldr_proto lea, e, rgb, rgw, rgd, rgq
    else match =mov e, expr
        ldr_proto mov, e, rgb, rgw, rgd, rgq
    else
        ldr_proto mov, expr, rgb, rgw, rgd, rgq
    end match
end macro

macro ldr_nullcheck expr, rgb, rgw, rgd, rgq
    match =~, expr
        xor rgd, rgd
    else match =~ e, expr
        xor rgd, rgd
        ldr_typed e, rgb, rgw, rgd, rgq
    else
        ldr_typed expr, rgb, rgw, rgd, rgq
    end match
end macro

macro ldr reg, expr
    match =rax, reg
        ldr_nullcheck expr, al, ax, eax, rax
    else match =rbx, reg
        ldr_nullcheck expr, bl, bx, ebx, rbx
    else match =rcx, reg
        ldr_nullcheck expr, cl, cx, ecx, rcx
    else match =rdx, reg
        ldr_nullcheck expr, dl, dx, edx, rdx
    else match =rsi, reg
        ldr_nullcheck expr, sil, si, esi, rsi
    else match =rdi, reg
        ldr_nullcheck expr, dil, di, edi, rdi
    else match =r8, reg
        ldr_nullcheck expr, r8b, r8w, r8d, r8
    else match =r9, reg 
        ldr_nullcheck expr, r9b, r9w, r9d, r9
    else match =r10, reg 
        ldr_nullcheck expr, r10b, r10w, r10d, r10
    else match =r11, reg 
        ldr_nullcheck expr, r11b, r11w, r11d, r11
    else match =r12, reg 
        ldr_nullcheck expr, r12b, r12w, r12d, r12
    else match =r13, reg 
        ldr_nullcheck expr, r13b, r13w, r13d, r13
    else match =r14, reg 
        ldr_nullcheck expr, r14b, r14w, r14d, r14
    else match =r15, reg 
        ldr_nullcheck expr, r15b, r15w, r15d, r15
    end match
end macro

macro call_varg a0*, a1*, a2*, a3*, a4*, a5*, arguments*&
    local rg
    rg = 0
    iterate arg, arguments
        match =_ =:== expr, arg
            rg = expr
        else match reg =:== expr, arg
                ldr reg, expr
        else match =_, arg
            rg = rg + 1
        else
            if rg = 0
                ldr a0, arg
            else if rg = 1
                ldr a1, arg
            else if rg = 2
                ldr a2, arg
            else if rg = 3
                ldr a3, arg
            else if rg = 4
                ldr a4, arg
            else if rg = 5
                ldr a5, arg
            end if
            rg = rg + 1
        end match
    end iterate
end macro

macro syscall syscode*, arguments*&
    call_varg rdi, rsi, rdx, r10, r8, r9, arguments
    ldr rax, ~byte syscode
    syscall
end macro

macro call name*, arguments*&
    call_varg rdi, rsi, rdx, rcx, r8, r9, arguments
    call name
end macro

macro enter
    push rbp
    mov rbp, rsp
end macro

macro lods size*, register
    lods size [ uses_register ]
    match any, register
        ldr regster, size rax
    end match
end macro

macro lodstr _string
    match any, _string
        ldr lea rsi, _string
    end match
    lods dword rcx
end macro

macro strdef_typed name_expr*, dat*&
    local kind
    match type name =[ size =], name_expr
        match =byte, type
            kind equ db
        else match =word, type
            kind equ dw
        else match =dword, type
            kind equ dd
        else match =qword, type
            kind equ dq
        else
            err
        end match

        label name
        match =byte, size
            db @f-$-1
        else match =word, size
            dw @f-$-2
        else match =dword, size
            dd @f-$-4
        else match =qword, size
            dq @f-$-8
        else
            err
        end match
    else
        err
    end match
    match dedication, kind
        dedication dat
    else
        err
    end match
    @@:
end macro

macro strdef name*, dat*&
    strdef_typed byte name [ dword ], dat
end macro

macro segment flags&
    match =.text, flags
        segment readable executable
    else match =.data, flags
        segment readable writeable
    else match =.rodata, flags
        segment readable
    else
        segment flags
    end match
end macro

macro enum arguments*&
    local x, matched
    x = 0
    iterate arg, arguments
        match a =:== b, arg
            a = b
            x = b
        else
            arg = x
        end match
        x = x + 1
    end iterate
end macro

macro lillypad_definitions
    cstrlen: ; (string)
        mov rsi, rdi
        xor eax, eax
        xor ecx, ecx
        dec rcx
        repne scasb
        mov rax, rdi
        sub rax, rsi
        dec rax
        ret
end macro

; STD FILDES
enum\
    STDIN, STDOUT, STDERR
; FCNTL O_
enum\
    O_RDONLY, O_WRONLY, O_RDWR
; MMAN MAP_
enum\
    MAP_SHARED := 0x01, MAP_PRIVATE, MAP_FIXED := 0x10, MAP_ANONYMOUS := 0x20
; SYSCALLS as stated in: <https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/>
enum\
    SYS_READ,                   SYS_WRITE,                  SYS_OPEN,\
    SYS_CLOSE,                  SYS_STAT,                   SYS_FSTAT,\
    SYS_LSTAT,                  SYS_POLL,                   SYS_LSEEK,\
    SYS_MMAP,                   SYS_MPROTECT,               SYS_MUNMAP,\
    SYS_BRK,                    SYS_RT_SIGACTION,           SYS_RT_SIGPROC_MASK,\
    SYS_RT_SIGRETURN,           SYS_IOCTL,                  SYS_PREAD64,\
    SYS_PWRITE64,               SYS_READV,                  SYS_WRITEV,\
    SYS_ACCESS,                 SYS_PIPE,                   SYS_SELECT,\
    SYS_SCHED_YIELD,            SYS_MREMAP,                 SYS_MSYNC,\
    SYS_MINCORE,                SYS_MADVISE,                SYS_SHMGET,\
    SYS_SHMAT,                  SYS_SHMCTL,                 SYS_DUP,\
    SYS_DUP2,                   SYS_PAUSE,                  SYS_NANOSLEEP,\
    SYS_GETTIMER,               SYS_ALARM,                  SYS_SETITIMER,\
    SYS_GETPID,                 SYS_SENDFILE,               SYS_SOCKET,\
    SYS_CONNECT,                SYS_ACCEPT,                 SYS_SENDTO,\
    SYS_RECVFROM,               SYS_SENDMSG,                SYS_RECVMSG,\
    SYS_SHUTDOWN,               SYS_BIND,                   SYS_LISTEN,\
    SYS_GETSOCKNAME,            SYS_GETPEERNAME,            SYS_SOCKETPAIR,\
    SYS_SETSOCKOPT,             SYS_GETSOCKOPT,             SYS_CLONE,\
    SYS_FORK,                   SYS_VFORK,                  SYS_EXECVE,\
    SYS_EXIT,                   SYS_WAIT4,                  SYS_KILL,\
    SYS_UNAME,                  SYS_SEMGET,                 SYS_SEMOP,\
    SYS_SEMCTL,                 SYS_SHMDT,                  SYS_MSGGET,\
    SYS_MSGSND,                 SYS_MSGRCV,                 SYS_MSGCTL,\
    SYS_FCNTL,                  SYS_FLOCK,                  SYS_FSYNC,\
    SYS_FDATASYNC,              SYS_TRUNCATE,               SYS_FTRUNCATE,\
    SYS_GETDENTS,               SYS_GETCWD,                 SYS_CHDIR,\
    SYS_FCHDIR,                 SYS_RENAME,                 SYS_MKDIR,\
    SYS_RMDIR,                  SYS_CREAT,                  SYS_LINK,\
    SYS_UNLINK,                 SYS_SYMLINK,                SYS_READLINK,\
    SYS_CHMOD,                  SYS_FCHMOD,                 SYS_CHOWN,\
    SYS_FCHOWN,                 SYS_LCHOWN,                 SYS_UMASK,\
    SYS_GETTIMEOFDAY,           SYS_GETRLIMIT,              SYS_GETRUSAGE,\
    SYS_SYSINFO,                SYS_TIMES,                  SYS_PTRACE,\
    SYS_GETUID,                 SYS_SYSLOG,                 SYS_GETGID,\
    SYS_SETUID,                 SYS_SETGID,                 SYS_GETEUID,\
    SYS_GETEGID,                SYS_SETPGID,                SYS_GETPPID,\
    SYS_GETPGRP,                SYS_SETSID,                 SYS_SETREUID,\
    SYS_SETREGID,               SYS_GETGROUPS,              SYS_SETGROUPS,\
    SYS_SETRESUID,              SYS_GETRESUID,              SYS_SETRESGID,\
    SYS_GETRESGID,              SYS_GETPGID,                SYS_SETFSUID,\
    SYS_SETFSGID,               SYS_GETSID,                 SYS_CAPGET,\
    SYS_RT_SIGPENDING,          SYS_RT_SIGTIMEDWAIT,        SYS_RT_SIGQUEUEINFO,\
    SYS_RT_SIGSUSPEND,          SYS_SIGALTSTACK,            SYS_UTIME,\
    SYS_MKNOD,                  SYS_USELIB,                 SYS_PERSONALITY,\
    SYS_USTAT,                  SYS_STATFS,                 SYS_FSTATFS,\
    SYS_SYSFS,                  SYS_GETPRIORITY,            SYS_SETPRIORITY,\
    SYS_SCHED_SETPARAM,         SYS_SCHED_GETPARAM,         SYS_SCHED_SETSCHEDULER,\
    SYS_SCHED_GETSCHEDULER,     SYS_SCHED_GET_PRIORITY_MAX, SYS_SCHED_GET_PRIORITY_MIN,\
    SYS_SCHED_RR_GET_INTERVAL,  SYS_MLOCK,                  SYS_MUNLOCK,\
    SYS_MLOCKALL,               SYS_MUNLOCKALL,             SYS_VHANGUP,\
    SYS_MODIFY_LDT,             SYS_PIVOT_ROOT,             SYS__SYSCTL,\
    SYS_PRCTL,                  SYS_ARCH_PRCTL,             SYS_ADJTIMEX,\
    SYS_SETRLIMIT,              SYS_CHROOT,                 SYS_CHROOT,\
    SYS_SYNC,                   SYS_ACCT,                   SYS_SETTIMEOFDAY,\
    SYS_MOUNT,                  SYS_UMOUNT2,                SYS_SWAPON,\
    SYS_SWAPOFF,                SYS_REBOOT,                 SYS_SETHOSTNAME,\
    SYS_SETDOMAINNAME,          SYS_IOPL,                   SYS_IOPERM,\
    SYS_CREATE_MODULE,          SYS_INIT_MODULE,            SYS_DELETE_MODULE,\
    SYS_GET_KERNEL_SYMS,        SYS_QUERY_MODULE,           SYS_QUOTACTL,\
    SYS_NFSSERVCTL,             SYS_GETPMSG,                SYS_PUTPMSG,\
    SYS_AFS_SYSCALL,            SYS_TUXCALL,                SYS_SECURITY,\
    SYS_GETTID,                 SYS_READAHEAD,              SYS_SETXATTR,\
    SYS_LSETXATTR,              SYS_FSETXATTR,              SYS_GETXATTR,\
    SYS_LGETXATTR,              SYS_FGETXATTR,              SYS_LISTXATTR,\
    SYS_LLISTXATTR,             SYS_FLISTXATTR,             SYS_REMOVEXATTR,\
    SYS_LREMOVEXATTR,           SYS_FREMOVEXATTR,           SYS_TKILL,\
    SYS_TIME,                   SYS_FUTEX,                  SYS_SCHED_SETAFFINITY,\
    SYS_SCHED_GETAFFINITY,      SYS_SET_THREAD_AREA,        SYS_IO_SETUP,\
    SYS_IO_DESTROY,             SYS_IO_GETEVENTS,           SYS_IO_SUBMIT,\
    SYS_IO_CANCEL,              SYS_GET_THREAD_AREA,        SYS_LOOKUP_DCOOKIE,\
    SYS_EPOLL_CREATE,           SYS_EPOLL_CTL_OLD,          SYS_EPOLL_WAIT_OLD,\
    SYS_REMAP_FILE_PAGES,       SYS_GETDENTS64,             SYS_SET_TID_ADDRESS,\
    SYS_RESTART_SYSCALL,        SYS_SEMTIMEDOP,             SYS_FADVISE64,\
    SYS_TIMER_CREATE,           SYS_TIMER_SETTIME,          SYS_TIMER_GETTIME,\
    SYS_TIMER_GETOVERRUN,       SYS_TIMER_DELETE,           SYS_CLOCK_SETTIME,\
    SYS_CLOCK_GETTIME,          SYS_CLOCK_GETRES,           SYS_CLOCK_NANOSLEEP,\
    SYS_EXIT_GROUP,             SYS_EPOLL_WAIT,             SYS_EPOLL_CTL,\
    SYS_TGKILL,                 SYS_UTIMES,                 SYS_VSERVER,\
    SYS_MBIND,                  SYS_SET_MEMPOLICY,          SYS_GET_MEMPOLICY,\
    SYS_MQ_OPEN,                SYS_MQ_UNLINK,              SYS_MQTIMEDSEND,\
    SYS_MQ_TIMEDRECEIVE,        SYS_MQ_NOTIFY,              SYS_MQ_GETSETATTR,\
    SYS_KEXEC_LOAD,             SYS_WAITID,                 SYS_ADD_KEY,\
    SYS_REQUEST_KEY,            SYS_KEYCTL,                 SYS_IOPRIO_SET,\
    SYS_IOPRIO_GET,             SYS_INOTIFY_INIT,           SYS_INOTIFY_ADD_WATCH,\
    SYS_INOTIFY_RM_WATCH,       SYS_MIGRATE_PAGES,          SYS_OPNEAT,\
    SYS_MKDIRAT,                SYS_MKNODAT,                SYS_FCHOWNAT,\
    SYS_FUTIMESAT,              SYS_NEWFSTATAT,             SYS_UNLINK_AT,\
    SYS_RENAME_AT,              SYS_LINKAT,                 SYS_SYMLINKAT,\
    SYS_READLINKAT,             SYS_FCHMODAT,               SYS_FACCESSAT,\
    SYS_PSELECT6,               SYS_PPOLL,                  SYS_UNSHARE,\
    SYS_SET_ROBUST_LIST,        SYS_GET_ROBUST_LIST,        SYS_SPLICE,\
    SYS_TEE,                    SYS_SYNC_FILE_RANGE,        SYS_VMSPLICE,\
    SYS_MOVE_PAGES,             SYS_UTIMENSAT,              SYS_EPOLL_WAIT,\
    SYS_SIGNALFD,               SYS_TIMERFD_CREATE,         SYS_EVENTFD,\
    SYS_FALLOCATE,              SYS_TIMERFD_SETTIME,        SYS_TIMERFD_GETTIME,\
    SYS_ACCEPT4,                SYS_SIGNALFD4,              SYS_EVENTFD2,\
    SYS_EPOLL_CREATE1,          SYS_DUP3,                   SYS_PIPE2,\
    SYS_INOTIFY_INIT1,          SYS_PREADV,                 SYS_PWRITEV,\
    SYS_RT_TGSIGQUEUEINFO,      SYS_PERF_EVENT_OPEN,        SYS_RECVMMSG,\
    SYS_FANOTIFY_INIT,          SYS_FANOTIFY_MARK,          SYS_PRLIMIT64,\
    SYS_NAME_TO_HANDLE_AT,      SYS_OPEN_BY_HANDLE_AT,      SYS_CLOCK_ADJTIME,\
    SYS_SYNCFS,                 SYS_SENDMMSG,               SYS_SETNS,\
    SYS_GETCPU,                 SYS_PROCESS_VM_READV,       SYS_PROCESS_VM_WRITEV,\
    SYS_KCMP,                   SYS_FINIT_MODULE,           SYS_SCHED_SETATTR,\
    SYS_SCHED_GETATTR,          SYS_RENAMEAT2,              SYS_SECCOMP,\
    SYS_GETRANDOM,              SYS_MEMFD_CREATE,           SYS_KEXEC_FILE_LOAD,\
    SYS_BPF,                    STUB_EXECVE_AT,             USERFAULTFD,\
    MEMBARRIER,                 MLOCK2,                     COPY_FILE_RANGE,\
    PREADV2,                    PWRITEV2,                   PKEY_MPROTECT,\
    PKEY_ALLOC,                 PKEY_FREE,                  STATX,\
    IO_PGETEVENTS,              RSEQ,                       PKEY_MPROTECT
