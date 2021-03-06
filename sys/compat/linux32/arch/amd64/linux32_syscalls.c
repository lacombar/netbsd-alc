/* $NetBSD: linux32_syscalls.c,v 1.41 2008/10/06 14:53:58 njoly Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.38 2008/10/06 14:53:01 njoly Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux32_syscalls.c,v 1.41 2008/10/06 14:53:58 njoly Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <machine/netbsd32_machdep.h>
#include <compat/netbsd32/netbsd32.h>
#include <compat/netbsd32/netbsd32_syscallargs.h>
#include <compat/linux32/common/linux32_types.h>
#include <compat/linux32/common/linux32_signal.h>
#include <compat/linux32/arch/amd64/linux32_missing.h>
#include <compat/linux32/linux32_syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/linux_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const linux32_syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"netbsd32_exit",
	/*   2 */	"fork",
	/*   3 */	"netbsd32_read",
	/*   4 */	"netbsd32_write",
	/*   5 */	"open",
	/*   6 */	"netbsd32_close",
	/*   7 */	"waitpid",
	/*   8 */	"creat",
	/*   9 */	"netbsd32_link",
	/*  10 */	"unlink",
	/*  11 */	"netbsd32_execve",
	/*  12 */	"netbsd32_chdir",
	/*  13 */	"time",
	/*  14 */	"mknod",
	/*  15 */	"netbsd32_chmod",
	/*  16 */	"lchown16",
	/*  17 */	"break",
	/*  18 */	"#18 (obsolete ostat)",
	/*  19 */	"compat_43_netbsd32_olseek",
	/*  20 */	"getpid",
	/*  21 */	"#21 (unimplemented mount)",
	/*  22 */	"#22 (unimplemented umount)",
	/*  23 */	"linux_setuid16",
	/*  24 */	"linux_getuid16",
	/*  25 */	"stime",
	/*  26 */	"ptrace",
	/*  27 */	"alarm",
	/*  28 */	"#28 (obsolete ofstat)",
	/*  29 */	"pause",
	/*  30 */	"utime",
	/*  31 */	"#31 (obsolete stty)",
	/*  32 */	"#32 (obsolete gtty)",
	/*  33 */	"netbsd32_access",
	/*  34 */	"nice",
	/*  35 */	"#35 (obsolete ftime)",
	/*  36 */	"sync",
	/*  37 */	"kill",
	/*  38 */	"netbsd32___posix_rename",
	/*  39 */	"netbsd32_mkdir",
	/*  40 */	"netbsd32_rmdir",
	/*  41 */	"netbsd32_dup",
	/*  42 */	"pipe",
	/*  43 */	"times",
	/*  44 */	"#44 (obsolete prof)",
	/*  45 */	"brk",
	/*  46 */	"linux_setgid16",
	/*  47 */	"linux_getgid16",
	/*  48 */	"signal",
	/*  49 */	"linux_geteuid16",
	/*  50 */	"linux_getegid16",
	/*  51 */	"netbsd32_acct",
	/*  52 */	"#52 (obsolete phys)",
	/*  53 */	"#53 (obsolete lock)",
	/*  54 */	"ioctl",
	/*  55 */	"fcntl",
	/*  56 */	"#56 (obsolete mpx)",
	/*  57 */	"netbsd32_setpgid",
	/*  58 */	"#58 (obsolete ulimit)",
	/*  59 */	"oldolduname",
	/*  60 */	"netbsd32_umask",
	/*  61 */	"netbsd32_chroot",
	/*  62 */	"#62 (unimplemented ustat)",
	/*  63 */	"netbsd32_dup2",
	/*  64 */	"getppid",
	/*  65 */	"getpgrp",
	/*  66 */	"setsid",
	/*  67 */	"#67 (unimplemented sigaction)",
	/*  68 */	"#68 (unimplemented siggetmask)",
	/*  69 */	"#69 (unimplemented sigsetmask)",
	/*  70 */	"setreuid16",
	/*  71 */	"setregid16",
	/*  72 */	"#72 (unimplemented sigsuspend)",
	/*  73 */	"#73 (unimplemented sigpending)",
	/*  74 */	"compat_43_netbsd32_osethostname",
	/*  75 */	"setrlimit",
	/*  76 */	"getrlimit",
	/*  77 */	"netbsd32_getrusage",
	/*  78 */	"gettimeofday",
	/*  79 */	"settimeofday",
	/*  80 */	"getgroups16",
	/*  81 */	"setgroups16",
	/*  82 */	"oldselect",
	/*  83 */	"netbsd32_symlink",
	/*  84 */	"compat_43_netbsd32_lstat43",
	/*  85 */	"netbsd32_readlink",
	/*  86 */	"#86 (unimplemented uselib)",
	/*  87 */	"swapon",
	/*  88 */	"reboot",
	/*  89 */	"readdir",
	/*  90 */	"old_mmap",
	/*  91 */	"netbsd32_munmap",
	/*  92 */	"compat_43_netbsd32_otruncate",
	/*  93 */	"compat_43_netbsd32_oftruncate",
	/*  94 */	"netbsd32_fchmod",
	/*  95 */	"fchown16",
	/*  96 */	"getpriority",
	/*  97 */	"netbsd32_setpriority",
	/*  98 */	"#98 (unimplemented profil)",
	/*  99 */	"statfs",
	/* 100 */	"#100 (unimplemented fstatfs)",
	/* 101 */	"#101 (unimplemented ioperm)",
	/* 102 */	"socketcall",
	/* 103 */	"#103 (unimplemented syslog)",
	/* 104 */	"netbsd32_setitimer",
	/* 105 */	"netbsd32_getitimer",
	/* 106 */	"#106 (unimplemented stat)",
	/* 107 */	"#107 (unimplemented lstat)",
	/* 108 */	"#108 (unimplemented fstat)",
	/* 109 */	"olduname",
	/* 110 */	"#110 (unimplemented iopl)",
	/* 111 */	"#111 (unimplemented vhangup)",
	/* 112 */	"#112 (unimplemented idle)",
	/* 113 */	"#113 (unimplemented vm86old)",
	/* 114 */	"wait4",
	/* 115 */	"swapoff",
	/* 116 */	"sysinfo",
	/* 117 */	"ipc",
	/* 118 */	"netbsd32_fsync",
	/* 119 */	"sigreturn",
	/* 120 */	"clone",
	/* 121 */	"#121 (unimplemented setdomainname)",
	/* 122 */	"uname",
	/* 123 */	"#123 (unimplemented modify_ldt)",
	/* 124 */	"#124 (unimplemented adjtimex)",
	/* 125 */	"mprotect",
	/* 126 */	"#126 (unimplemented sigprocmask)",
	/* 127 */	"#127 (unimplemented create_module)",
	/* 128 */	"#128 (unimplemented init_module)",
	/* 129 */	"#129 (unimplemented delete_module)",
	/* 130 */	"#130 (unimplemented get_kernel_syms)",
	/* 131 */	"#131 (unimplemented quotactl)",
	/* 132 */	"netbsd32_getpgid",
	/* 133 */	"netbsd32_fchdir",
	/* 134 */	"#134 (unimplemented bdflush)",
	/* 135 */	"#135 (unimplemented sysfs)",
	/* 136 */	"#136 (unimplemented personality)",
	/* 137 */	"#137 (unimplemented afs_syscall)",
	/* 138 */	"setfsuid16",
	/* 139 */	"setfsgid16",
	/* 140 */	"llseek",
	/* 141 */	"getdents",
	/* 142 */	"select",
	/* 143 */	"netbsd32_flock",
	/* 144 */	"netbsd32___msync13",
	/* 145 */	"netbsd32_readv",
	/* 146 */	"netbsd32_writev",
	/* 147 */	"netbsd32_getsid",
	/* 148 */	"fdatasync",
	/* 149 */	"__sysctl",
	/* 150 */	"netbsd32_mlock",
	/* 151 */	"netbsd32_munlock",
	/* 152 */	"netbsd32_mlockall",
	/* 153 */	"munlockall",
	/* 154 */	"#154 (unimplemented sched_setparam)",
	/* 155 */	"sched_getparam",
	/* 156 */	"sched_setscheduler",
	/* 157 */	"sched_getscheduler",
	/* 158 */	"sched_yield",
	/* 159 */	"#159 (unimplemented sched_get_priority_max)",
	/* 160 */	"#160 (unimplemented sched_get_priority_min)",
	/* 161 */	"#161 (unimplemented sched_rr_get_interval)",
	/* 162 */	"netbsd32_nanosleep",
	/* 163 */	"mremap",
	/* 164 */	"setresuid16",
	/* 165 */	"getresuid16",
	/* 166 */	"#166 (unimplemented vm86)",
	/* 167 */	"#167 (unimplemented query_module)",
	/* 168 */	"netbsd32_poll",
	/* 169 */	"#169 (unimplemented nfsservctl)",
	/* 170 */	"setresgid16",
	/* 171 */	"getresgid16",
	/* 172 */	"#172 (unimplemented prctl)",
	/* 173 */	"rt_sigreturn",
	/* 174 */	"rt_sigaction",
	/* 175 */	"rt_sigprocmask",
	/* 176 */	"rt_sigpending",
	/* 177 */	"#177 (unimplemented rt_sigtimedwait)",
	/* 178 */	"#178 (unimplemented rt_queueinfo)",
	/* 179 */	"rt_sigsuspend",
	/* 180 */	"pread",
	/* 181 */	"pwrite",
	/* 182 */	"chown16",
	/* 183 */	"netbsd32___getcwd",
	/* 184 */	"#184 (unimplemented capget)",
	/* 185 */	"#185 (unimplemented capset)",
	/* 186 */	"#186 (unimplemented sigaltstack)",
	/* 187 */	"#187 (unimplemented sendfile)",
	/* 188 */	"#188 (unimplemented getpmsg)",
	/* 189 */	"#189 (unimplemented putpmsg)",
	/* 190 */	"__vfork14",
	/* 191 */	"ugetrlimit",
	/* 192 */	"mmap2",
	/* 193 */	"#193 (unimplemented truncate64)",
	/* 194 */	"#194 (unimplemented ftruncate64)",
	/* 195 */	"stat64",
	/* 196 */	"lstat64",
	/* 197 */	"fstat64",
	/* 198 */	"netbsd32___posix_lchown",
	/* 199 */	"getuid",
	/* 200 */	"getgid",
	/* 201 */	"geteuid",
	/* 202 */	"getegid",
	/* 203 */	"netbsd32_setreuid",
	/* 204 */	"netbsd32_setregid",
	/* 205 */	"netbsd32_getgroups",
	/* 206 */	"netbsd32_setgroups",
	/* 207 */	"netbsd32___posix_fchown",
	/* 208 */	"setresuid",
	/* 209 */	"#209 (unimplemented getresuid)",
	/* 210 */	"setresgid",
	/* 211 */	"#211 (unimplemented getresgid)",
	/* 212 */	"netbsd32___posix_chown",
	/* 213 */	"netbsd32_setuid",
	/* 214 */	"netbsd32_setgid",
	/* 215 */	"setfsuid",
	/* 216 */	"setfsgid",
	/* 217 */	"#217 (unimplemented pivot_root)",
	/* 218 */	"#218 (unimplemented mincore)",
	/* 219 */	"netbsd32_madvise",
	/* 220 */	"getdents64",
#define linux32_sys_fcntl64 linux32_sys_fcntl
#define linux32_sys_fcntl64_args linux32_sys_fcntl_args
	/* 221 */	"fcntl64",
	/* 222 */	"#222 (unimplemented / * unused * /)",
	/* 223 */	"#223 (unimplemented / * unused * /)",
	/* 224 */	"gettid",
	/* 225 */	"#225 (unimplemented readahead)",
	/* 226 */	"#226 (unimplemented setxattr)",
	/* 227 */	"#227 (unimplemented lsetxattr)",
	/* 228 */	"#228 (unimplemented fsetxattr)",
	/* 229 */	"#229 (unimplemented getxattr)",
	/* 230 */	"#230 (unimplemented lgetxattr)",
	/* 231 */	"#231 (unimplemented fgetxattr)",
	/* 232 */	"#232 (unimplemented listxattr)",
	/* 233 */	"#233 (unimplemented llistxattr)",
	/* 234 */	"#234 (unimplemented flistxattr)",
	/* 235 */	"#235 (unimplemented removexattr)",
	/* 236 */	"#236 (unimplemented lremovexattr)",
	/* 237 */	"#237 (unimplemented fremovexattr)",
	/* 238 */	"#238 (unimplemented tkill)",
	/* 239 */	"#239 (unimplemented sendfile64)",
	/* 240 */	"#240 (unimplemented futex)",
	/* 241 */	"#241 (unimplemented sched_setaffinity)",
	/* 242 */	"#242 (unimplemented sched_getaffinity)",
	/* 243 */	"#243 (unimplemented set_thread_area)",
	/* 244 */	"#244 (unimplemented get_thread_area)",
	/* 245 */	"#245 (unimplemented io_setup)",
	/* 246 */	"#246 (unimplemented io_destroy)",
	/* 247 */	"#247 (unimplemented io_getevents)",
	/* 248 */	"#248 (unimplemented io_submit)",
	/* 249 */	"#249 (unimplemented io_cancel)",
	/* 250 */	"#250 (unimplemented fadvise64)",
	/* 251 */	"#251 (unimplemented / * unused * /)",
	/* 252 */	"exit_group",
	/* 253 */	"#253 (unimplemented lookup_dcookie)",
	/* 254 */	"#254 (unimplemented epoll_create)",
	/* 255 */	"#255 (unimplemented epoll_ctl)",
	/* 256 */	"#256 (unimplemented epoll_wait)",
	/* 257 */	"#257 (unimplemented remap_file_pages)",
	/* 258 */	"#258 (unimplemented set_tid_address)",
	/* 259 */	"#259 (unimplemented timer_create)",
	/* 260 */	"#260 (unimplemented timer_settime)",
	/* 261 */	"#261 (unimplemented timer_gettime)",
	/* 262 */	"#262 (unimplemented timer_getoverrun)",
	/* 263 */	"#263 (unimplemented timer_delete)",
	/* 264 */	"clock_settime",
	/* 265 */	"clock_gettime",
	/* 266 */	"clock_getres",
	/* 267 */	"#267 (unimplemented clock_nanosleep)",
	/* 268 */	"#268 (unimplemented statfs64)",
	/* 269 */	"#269 (unimplemented fstatfs64)",
	/* 270 */	"#270 (unimplemented tgkill)",
	/* 271 */	"#271 (unimplemented utimes)",
	/* 272 */	"#272 (unimplemented fadvise64_64)",
	/* 273 */	"#273 (unimplemented vserver)",
	/* 274 */	"#274 (unimplemented mbind)",
	/* 275 */	"#275 (unimplemented get_mempolicy)",
	/* 276 */	"#276 (unimplemented set_mempolicy)",
	/* 277 */	"#277 (unimplemented mq_open)",
	/* 278 */	"#278 (unimplemented mq_unlink)",
	/* 279 */	"#279 (unimplemented mq_timedsend)",
	/* 280 */	"#280 (unimplemented mq_timedreceive)",
	/* 281 */	"#281 (unimplemented mq_notify)",
	/* 282 */	"#282 (unimplemented mq_getsetattr)",
	/* 283 */	"#283 (unimplemented kexec_load)",
};
