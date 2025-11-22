
macro ldr_proto kind, expr, rb, rw, rd, rq {
    local matched
    matched equ FALSE
    match =lea, kind \{
        matched equ TRUE
        kind rq, expr
    \}
    match =FALSE, matched \{
        match =byte e, expr \\{
            matched equ TRUE
            kind rb, byte e
        \\}
        match =word e, expr \\{
            matched equ TRUE
            kind rw, word e
        \\}
        match =dword e, expr \\{
            matched equ TRUE
            kind rd, dword e
        \\}
        match =qword e, expr \\{
            matched equ TRUE
            kind rq, qword e
        \\}
        match =FALSE, matched \\{
            kind rd, dword expr
        \\}
    \}
}

macro ldr_typed expr, rb, rw, rd, rq {
common
    local matched
    matched equ FALSE
    match =lea e, expr \{
        matched equ TRUE
        ldr_proto lea, e, rb, rw, rd, rq
    \}
    match =mov e, expr \{
        matched equ TRUE
        ldr_proto mov, e, rb, rw, rd, rq
    \}
    match =FALSE, matched \{
        ldr_proto mov, expr, rb, rw, rd, rq
    \}
}

macro ldr_nullcheck expr, rb, rw, rd, rq {
common
    local matched
    matched equ FALSE
    match =~, expr \{
        matched equ TRUE
        xor rd, rd
    \}
    match =~ e, expr \{
        matched equ TRUE
        xor rd, rd
        ldr_typed e, rb, rw, rd, rq
    \}
    match =FALSE, matched \{
        ldr_typed expr, rb, rw, rd, rq
    \}
}

macro ldr reg, expr {
common
    match =rax, reg \{
        ldr_nullcheck expr, al, ax, eax, rax
    \}
    match =rbx, reg \{
        ldr_nullcheck expr, bl, bx, ebx, rbx
    \}
    match =rcx, reg \{
        ldr_nullcheck expr, cl, cx, ecx, rcx
    \}
    match =rdx, reg \{
        ldr_nullcheck expr, dl, dx, edx, rdx
    \}
    match =rsi, reg \{
        ldr_nullcheck expr, sil, si, esi, rsi
    \}
    match =rdi, reg \{
        ldr_nullcheck expr, dil, di, edi, rdi
    \}
    match =r8, reg \{
        ldr_nullcheck expr, r8b, r8w, r8d, r8
    \}
    match =r9, reg \{
        ldr_nullcheck expr, r9b, r9w, r9d, r9
    \}
    match =r10, reg \{
        ldr_nullcheck expr, r10b, r10w, r10d, r10
    \}
    match =r11, reg \{
        ldr_nullcheck expr, r11b, r11w, r11d, r11
    \}
    match =r12, reg \{
        ldr_nullcheck expr, r12b, r12w, r12d, r12
    \}
    match =r13, reg \{
        ldr_nullcheck expr, r13b, r13w, r13d, r13
    \}
    match =r14, reg \{
        ldr_nullcheck expr, r14b, r14w, r14d, r14
    \}
    match =r15, reg \{
        ldr_nullcheck expr, r15b, r15w, r15d, r15
    \}
}

macro syscall syscode*, [arg] {
common
    local rg, matched
    rg = 0
forward
match any, arg \{
    matched equ FALSE
    match =_ =:== expr, arg \\{
        matched equ TRUE
        rg = expr
    \\}
    match =FALSE, matched \\{
        match reg =:== expr, arg \\\{
            matched equ TRUE
            ldr reg, expr
        \\\}
    \\}
    match =_, arg \\{
        matched equ TRUE
        rg = rg + 1
    \\}
    match =FALSE, matched \\{
        if rg = 0
            ldr rdi, arg
        end if
        if rg = 1
            ldr rsi, arg
        end if
        if rg = 2
            ldr rdx, arg
        end if
        if rg = 3
            ldr r10, arg
        end if
        if rg = 4
            ldr r8, arg
        end if
        if rg = 5
            ldr r9, arg
        end if
        rg = rg + 1
    \\}
\}
common
    ldr rax, ~byte syscode
    syscall
}

macro call name*, [arg] {
common
    local rg, matched
    rg = 0
forward
match any, arg \{
    matched equ FALSE
    match =_ =:== expr, arg \\{
        matched equ TRUE
        rg = expr
    \\}
    match =FALSE, matched \\{
        match reg =:== expr, arg \\\{
            matched equ TRUE
            ldr reg, expr
        \\\}
    \\}
    match =_, arg \\{
        matched equ TRUE
        rg = rg + 1
    \\}
    match =FALSE, matched \\{
        if rg = 0
            ldr rdi, arg
        end if
        if rg = 1
            ldr rsi, arg
        end if
        if rg = 2
            ldr rdx, arg
        end if
        if rg = 3
            ldr rcx, arg
        end if
        if rg = 4
            ldr r8, arg
        end if
        if rg = 5
            ldr r9, arg
        end if
        rg = rg + 1
    \\}
\}
common
    call name
}

macro strdef_typed name_expr*, [dat] {
common
    local matched, kind
    matched equ FALSE
    match type name =[ size =], name_expr \{
        match =byte, type \\{
            matched equ TRUE
            kind equ db
        \\}
        match =word, type \\{
            matched equ TRUE
            kind equ dw
        \\}
        match =dword, type \\{
            matched equ TRUE
            kind equ dd
        \\}
        match =qword, type \\{
            matched equ TRUE
            kind equ dq
        \\}
        match =FALSE, matched \\{
            err
        \\}
        matched equ FALSE

        label name
        match =byte, size \\{
            matched equ TRUE
            db @f-$-1
        \\}
        match =word, size \\{
            matched equ TRUE
            dw @f-$-2
        \\}
        match =dword, size \\{
            matched equ TRUE
            dd @f-$-4
        \\}
        match =qword, size \\{
            matched equ TRUE
            dq @f-$-8
        \\}
        match =FALSE, matched \\{
            err
        \\}
    \}
    match =FALSE, matched \{
        err
    \}
forward
    kind dat
common
    @@:
}

segment fix seg
macro seg first {
common
    local matched
    matched equ FALSE
    segment fix segment
    match =.text, first \{
        matched equ TRUE
        segment readable executable
    \}
    match =.data, first \{
        matched equ TRUE
        segment readable writeable
    \}
    match =.rodata, first \{
        matched equ TRUE
        segment readable
    \}
    match =FALSE, matched \{
        segment first
    \}
    segment fix seg
}

macro strdef name*, [dat] {
common
    strdef_typed byte name [ dword ], dat
}

macro enum [arg] {
common
    local x, matched
    x = 0
forward
    matched equ FALSE
    match a =:== b, arg \{
        matched equ TRUE
        a = b
        x = b
    \}
    match =FALSE, matched \{
        arg = x
    \}
    x = x + 1
}

; STD FILDES
enum\
    STDIN, STDOUT, STDERR
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
