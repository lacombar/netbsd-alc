/* $NetBSD: linux_syscalls.c,v 1.32 2008/04/23 14:10:04 ad Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.29 2008/04/23 14:07:50 ad Exp  
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_syscalls.c,v 1.32 2008/04/23 14:10:04 ad Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_compat_netbsd.h"
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/common/linux_socketcall.h>
#include <compat/linux/linux_syscallargs.h>
#define linux_sys_mmap2_args linux_sys_mmap_args
#endif /* _KERNEL_OPT */

const char *const linux_syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"exit",
	/*   2 */	"fork",
	/*   3 */	"read",
	/*   4 */	"write",
	/*   5 */	"open",
	/*   6 */	"close",
	/*   7 */	"waitpid",
	/*   8 */	"creat",
	/*   9 */	"link",
	/*  10 */	"unlink",
	/*  11 */	"execve",
	/*  12 */	"chdir",
	/*  13 */	"time",
	/*  14 */	"mknod",
	/*  15 */	"chmod",
	/*  16 */	"__posix_lchown",
	/*  17 */	"#17 (unimplemented)",
	/*  18 */	"#18 (obsolete ostat)",
	/*  19 */	"lseek",
	/*  20 */	"getpid",
	/*  21 */	"#21 (unimplemented mount)",
	/*  22 */	"#22 (obsolete umount)",
	/*  23 */	"setuid",
	/*  24 */	"getuid",
	/*  25 */	"stime",
	/*  26 */	"ptrace",
	/*  27 */	"alarm",
	/*  28 */	"#28 (obsolete ofstat)",
	/*  29 */	"pause",
	/*  30 */	"utime",
	/*  31 */	"#31 (unimplemented)",
	/*  32 */	"#32 (unimplemented)",
	/*  33 */	"access",
	/*  34 */	"nice",
	/*  35 */	"#35 (unimplemented)",
	/*  36 */	"sync",
	/*  37 */	"kill",
	/*  38 */	"__posix_rename",
	/*  39 */	"mkdir",
	/*  40 */	"rmdir",
	/*  41 */	"dup",
	/*  42 */	"pipe",
	/*  43 */	"times",
	/*  44 */	"#44 (unimplemented)",
	/*  45 */	"brk",
	/*  46 */	"setgid",
	/*  47 */	"getgid",
	/*  48 */	"signal",
	/*  49 */	"geteuid",
	/*  50 */	"getegid",
	/*  51 */	"acct",
	/*  52 */	"#52 (unimplemented umount)",
	/*  53 */	"#53 (unimplemented)",
	/*  54 */	"ioctl",
	/*  55 */	"fcntl",
	/*  56 */	"#56 (obsolete mpx)",
	/*  57 */	"setpgid",
	/*  58 */	"#58 (unimplemented)",
	/*  59 */	"olduname",
	/*  60 */	"umask",
	/*  61 */	"chroot",
	/*  62 */	"#62 (unimplemented ustat)",
	/*  63 */	"dup2",
	/*  64 */	"getppid",
	/*  65 */	"getpgrp",
	/*  66 */	"setsid",
	/*  67 */	"sigaction",
	/*  68 */	"siggetmask",
	/*  69 */	"sigsetmask",
	/*  70 */	"setreuid",
	/*  71 */	"setregid",
	/*  72 */	"sigsuspend",
	/*  73 */	"sigpending",
	/*  74 */	"sethostname",
	/*  75 */	"setrlimit",
	/*  76 */	"getrlimit",
	/*  77 */	"getrusage",
	/*  78 */	"gettimeofday",
	/*  79 */	"settimeofday",
	/*  80 */	"getgroups",
	/*  81 */	"setgroups",
	/*  82 */	"#82 (unimplemented old_select)",
	/*  83 */	"symlink",
	/*  84 */	"oolstat",
	/*  85 */	"readlink",
	/*  86 */	"#86 (unimplemented uselib)",
	/*  87 */	"swapon",
	/*  88 */	"reboot",
	/*  89 */	"readdir",
	/*  90 */	"mmap",
	/*  91 */	"munmap",
	/*  92 */	"truncate",
	/*  93 */	"ftruncate",
	/*  94 */	"fchmod",
	/*  95 */	"__posix_fchown",
	/*  96 */	"getpriority",
	/*  97 */	"setpriority",
	/*  98 */	"#98 (unimplemented)",
	/*  99 */	"statfs",
	/* 100 */	"fstatfs",
	/* 101 */	"ioperm",
	/* 102 */	"socketcall",
	/* 103 */	"#103 (unimplemented syslog)",
	/* 104 */	"setitimer",
	/* 105 */	"getitimer",
	/* 106 */	"stat",
	/* 107 */	"lstat",
	/* 108 */	"fstat",
	/* 109 */	"uname",
	/* 110 */	"#110 (unimplemented iopl)",
	/* 111 */	"#111 (unimplemented vhangup)",
	/* 112 */	"#112 (unimplemented idle)",
	/* 113 */	"#113 (unimplemented vm86old)",
	/* 114 */	"wait4",
	/* 115 */	"swapoff",
	/* 116 */	"sysinfo",
	/* 117 */	"ipc",
	/* 118 */	"fsync",
	/* 119 */	"sigreturn",
	/* 120 */	"clone",
	/* 121 */	"setdomainname",
	/* 122 */	"new_uname",
	/* 123 */	"#123 (unimplemented modify_ldt)",
	/* 124 */	"#124 (unimplemented adjtimex)",
	/* 125 */	"mprotect",
	/* 126 */	"sigprocmask",
	/* 127 */	"#127 (unimplemented create_module)",
	/* 128 */	"#128 (unimplemented init_module)",
	/* 129 */	"#129 (unimplemented delete_module)",
	/* 130 */	"#130 (unimplemented get_kernel_syms)",
	/* 131 */	"#131 (unimplemented quotactl)",
	/* 132 */	"getpgid",
	/* 133 */	"fchdir",
	/* 134 */	"#134 (unimplemented bdflush)",
	/* 135 */	"#135 (unimplemented sysfs)",
	/* 136 */	"personality",
	/* 137 */	"#137 (unimplemented afs_syscall)",
	/* 138 */	"setfsuid",
	/* 139 */	"setfsgid",
	/* 140 */	"llseek",
	/* 141 */	"getdents",
	/* 142 */	"select",
	/* 143 */	"flock",
	/* 144 */	"__msync13",
	/* 145 */	"readv",
	/* 146 */	"writev",
	/* 147 */	"cacheflush",
	/* 148 */	"#148 (unimplemented cachectl)",
	/* 149 */	"sysmips",
	/* 150 */	"#150 (unimplemented)",
	/* 151 */	"getsid",
	/* 152 */	"fdatasync",
	/* 153 */	"__sysctl",
	/* 154 */	"mlock",
	/* 155 */	"munlock",
	/* 156 */	"mlockall",
	/* 157 */	"munlockall",
	/* 158 */	"sched_setparam",
	/* 159 */	"sched_getparam",
	/* 160 */	"sched_setscheduler",
	/* 161 */	"sched_getscheduler",
	/* 162 */	"sched_yield",
	/* 163 */	"sched_get_priority_max",
	/* 164 */	"sched_get_priority_min",
	/* 165 */	"#165 (unimplemented sched_rr_get_interval)",
	/* 166 */	"nanosleep",
	/* 167 */	"mremap",
	/* 168 */	"accept",
	/* 169 */	"bind",
	/* 170 */	"connect",
	/* 171 */	"getpeername",
	/* 172 */	"getsockname",
	/* 173 */	"getsockopt",
	/* 174 */	"listen",
	/* 175 */	"recv",
	/* 176 */	"recvfrom",
	/* 177 */	"recvmsg",
	/* 178 */	"send",
	/* 179 */	"sendmsg",
	/* 180 */	"sendto",
	/* 181 */	"setsockopt",
	/* 182 */	"#182 (unimplemented shutdown)",
	/* 183 */	"socket",
	/* 184 */	"socketpair",
	/* 185 */	"setresuid",
	/* 186 */	"getresuid",
	/* 187 */	"#187 (unimplemented query_module)",
	/* 188 */	"poll",
	/* 189 */	"#189 (unimplemented nfsservctl)",
	/* 190 */	"setresgid",
	/* 191 */	"getresgid",
	/* 192 */	"#192 (unimplemented prctl)",
	/* 193 */	"rt_sigreturn",
	/* 194 */	"rt_sigaction",
	/* 195 */	"rt_sigprocmask",
	/* 196 */	"rt_sigpending",
	/* 197 */	"#197 (unimplemented rt_sigtimedwait)",
	/* 198 */	"rt_queueinfo",
	/* 199 */	"rt_sigsuspend",
	/* 200 */	"pread",
	/* 201 */	"pwrite",
	/* 202 */	"__posix_chown",
	/* 203 */	"__getcwd",
	/* 204 */	"#204 (unimplemented capget)",
	/* 205 */	"#205 (unimplemented capset)",
	/* 206 */	"sigaltstack",
	/* 207 */	"#207 (unimplemented sendfile)",
	/* 208 */	"#208 (unimplemented)",
	/* 209 */	"#209 (unimplemented)",
	/* 210 */	"mmap2",
	/* 211 */	"truncate64",
	/* 212 */	"ftruncate64",
	/* 213 */	"stat64",
	/* 214 */	"lstat64",
	/* 215 */	"fstat64",
	/* 216 */	"#216 (unimplemented pivot_root)",
	/* 217 */	"mincore",
	/* 218 */	"madvise",
	/* 219 */	"getdents64",
	/* 220 */	"fcntl64",
	/* 221 */	"#221 (unimplemented / * reserved * /)",
	/* 222 */	"#222 (unimplemented gettid)",
	/* 223 */	"#223 (unimplemented readahead)",
	/* 224 */	"setxattr",
	/* 225 */	"lsetxattr",
	/* 226 */	"fsetxattr",
	/* 227 */	"getxattr",
	/* 228 */	"lgetxattr",
	/* 229 */	"fgetxattr",
	/* 230 */	"listxattr",
	/* 231 */	"llistxattr",
	/* 232 */	"flistxattr",
	/* 233 */	"removexattr",
	/* 234 */	"lremovexattr",
	/* 235 */	"fremovexattr",
	/* 236 */	"#236 (unimplemented tkill)",
	/* 237 */	"#237 (unimplemented sendfile64)",
	/* 238 */	"#238 (unimplemented futex)",
	/* 239 */	"#239 (unimplemented sched_setaffinity)",
	/* 240 */	"#240 (unimplemented sched_getaffinity)",
	/* 241 */	"#241 (unimplemented io_setup)",
	/* 242 */	"#242 (unimplemented io_destroy)",
	/* 243 */	"#243 (unimplemented io_getevents)",
	/* 244 */	"#244 (unimplemented io_submit)",
	/* 245 */	"#245 (unimplemented io_cancel)",
	/* 246 */	"exit_group",
	/* 247 */	"#247 (unimplemented lookup_dcookie)",
	/* 248 */	"#248 (unimplemented epoll_create)",
	/* 249 */	"#249 (unimplemented epoll_ctl)",
	/* 250 */	"#250 (unimplemented epoll_wait)",
	/* 251 */	"#251 (unimplemented remap_file_pages)",
	/* 252 */	"#252 (unimplemented set_tid_address)",
	/* 253 */	"#253 (unimplemented restart_syscall)",
	/* 254 */	"#254 (unimplemented fadvise64)",
	/* 255 */	"statfs64",
	/* 256 */	"fstatfs64",
	/* 257 */	"#257 (unimplemented timer_create)",
	/* 258 */	"#258 (unimplemented timer_settime)",
	/* 259 */	"#259 (unimplemented timer_gettime)",
	/* 260 */	"#260 (unimplemented timer_getoverrun)",
	/* 261 */	"#261 (unimplemented timer_delete)",
	/* 262 */	"clock_settime",
	/* 263 */	"clock_gettime",
	/* 264 */	"clock_getres",
	/* 265 */	"clock_nanosleep",
	/* 266 */	"#266 (unimplemented tgkill)",
	/* 267 */	"#267 (unimplemented utimes)",
	/* 268 */	"#268 (unimplemented mbind)",
	/* 269 */	"#269 (unimplemented get_mempolicy)",
	/* 270 */	"#270 (unimplemented set_mempolicy)",
	/* 271 */	"#271 (unimplemented mq_open)",
	/* 272 */	"#272 (unimplemented mq_unlink)",
	/* 273 */	"#273 (unimplemented mq_timedsend)",
	/* 274 */	"#274 (unimplemented mq_timedreceive)",
	/* 275 */	"#275 (unimplemented mq_notify)",
	/* 276 */	"#276 (unimplemented mq_getsetattr)",
};
