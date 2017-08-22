#ifndef SYSCODE_DEF
#define SYSCODE_DEF

#define SYS_RESTART_SYSCALL 0x00
#define SYS_EXIT 0x01
#define SYS_FORK 0x02
#define SYS_READ 0x03 
#define SYS_WRITE 0x04
#define SYS_OPEN 0x05
#define SYS_CLOSE 0x06
#define SYS_WAITPID 0x07
#define SYS_CREAT 0x08
#define SYS_LINK 0x09
#define SYS_UNLINK 0x0a
#define SYS_EXECVE 0x0b
#define SYS_CHDIR 0x0c
#define SYS_TIME 0x0d
#define SYS_MKNOD 0x0e
#define SYS_CHMOD 0x0f
#define SYS_LCHOWN16 0x10
// 0x11 -> not implemented!
#define SYS_STAT 0x12
#define SYS_LSEEK 0x13
#define SYS_GETPID 0x14
#define SYS_MOUNT 0x15
#define SYS_OLDUMOUNT 0x16
#define SYS_SETUID16 0x17
#define SYS_GETUID16 0x18
#define SYS_STIME 0x19
#define SYS_PTRACE 0x1a
#define SYS_ALARM 0x1b
#define SYS_FSTAT 0x1c
#define SYS_PAUSE 0x1d
#define SYS_UTIME 0x1e
// 0x1f, 0x20 -> not implemented
#define SYS_ACCESS 0x21
#define SYS_NICE 0x22
// 0x23 -> not implemented 
#define SYS_SYNC 0x24
#define SYS_KILL 0x25
#define SYS_RENAME 0x26
#define SYS_MKDIR 0x27
#define SYS_RMDIR 0x28
#define SYS_DUP 0x29
#define SYS_PIPE 0x2a
#define SYS_TIMES 0x2b
// 0x2c -> not implemented
#define SYS_BRK 0x2d
#define SYS_SETGID16 0x2e
#define SYS_GETGID16 0x2f
#define SYS_SIGNAL 0x30
#define SYS_GETEUID16 0x31
#define SYS_GETEGID16 0x32
#define SYS_ACCT 0x33
#define SYS_UMOUNT 0x34
// 0x35 -> not implemented
#define SYS_IOCTL 0x36
#define SYS_FCNTL 0x37 
// 0x38 -> not implemented
#define SYS_SETPGID 0x39
// 0x3a -> not implemented 
#define SYS_OLDUNAME 0x3b
#define SYS_UMASK 0x3c
#define SYS_CHROOT 0x3d
#define SYS_USTAT 0x3e
#define SYS_DUP2 0x3f
#define SYS_GETPPID 0x40
#define SYS_GETPGRP 0x41
#define SYS_SETSID 0x42
