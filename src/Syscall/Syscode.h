#ifndef SYSCODE_DEF
#define SYSCODE_DEF

/* Linux Syscall Reference (64 bits)
- source : https://syscalls64.paolostivanin.com/
*/

#define SYS_Read 0x00
#define SYS_Write 0x01
#define SYS_Open 0x02
#define SYS_Close 0x03 
#define SYS_Newstat 0x04
#define SYS_Newfstat 0x05
#define SYS_Newlstat 0x06
#define SYS_Poll 0x07
#define SYS_Lseek 0x08
#define SYS_Mmap 0x09
#define SYS_Mprotect 0x0a
#define SYS_Munmap 0x0b
#define SYS_Brk 0x0c
#define SYS_RT_Sigaction 0x0d
#define SYS_RT_Sigprocmask 0x0e
#define SYS_Ioctl 0x10
#define SYS_Pread64 0x11
#define SYS_Pwrite64 0x12
#define SYS_Readv 0x13
#define SYS_Writev 0x14
#define SYS_Access 0x15
#define SYS_Pipe 0x16
#define SYS_Select 0x17
#define SYS_Sched_yield 0x18
#define SYS_Mremap 0x19
#define SYS_Msync 0x1a
#define SYS_Mincore 0x1b
#define SYS_Madvise 0x1c
#define SYS_Shmget 0x1d
#define SYS_Shmat 0x1e
#define SYS_Shmctl 0x1f
#define SYS_Dup 0x20

#define SYS_Dup2	0x21
#define SYS_Pause	0x22
#define SYS_Nanosleep	0x23
#define SYS_Getitimer	0x24
#define SYS_Alarm	0x25
#define SYS_Setitimer	0x26
#define SYS_Getpid	0x27
#define SYS_Sendfile64	0x28
#define SYS_Socket	0x29
#define SYS_Connect	0x2a
#define SYS_Accept	0x2b
#define SYS_Sendto	0x2c
#define SYS_Recvfrom	0x2d
#define SYS_Sendmsg	0x2e
#define SYS_Recvmsg	0x2f
#define SYS_Shutdown	0x30
#define SYS_Bind	0x31
#define SYS_Listen	0x32
#define SYS_Getsockname	0x33
#define SYS_Getpeername	0x34
#define SYS_Socketpair	0x35
#define SYS_Setsockopt	0x36
#define SYS_Getsockopt	0x37
#define SYS_Exit	0x3c
#define SYS_Wait4	0x3d
#define SYS_Kill	0x3e
#define SYS_Newuname	0x3f
#define SYS_Semget	0x40

#endif