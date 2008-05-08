/* $NetBSD: syscalls.c,v 1.215 2008/04/24 11:51:47 ad Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.200 2008/04/24 11:51:19 ad Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: syscalls.c,v 1.215 2008/04/24 11:51:47 ad Exp $");

#if defined(_KERNEL_OPT)
#include "opt_nfsserver.h"
#include "opt_ntp.h"
#include "opt_compat_netbsd.h"
#include "opt_sysv.h"
#include "opt_compat_43.h"
#include "opt_posix.h"
#include "fs_lfs.h"
#include "fs_nfs.h"
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"exit",
	/*   2 */	"fork",
	/*   3 */	"read",
	/*   4 */	"write",
	/*   5 */	"open",
	/*   6 */	"close",
	/*   7 */	"wait4",
	/*   8 */	"compat_43_ocreat",
	/*   9 */	"link",
	/*  10 */	"unlink",
	/*  11 */	"#11 (obsolete execv)",
	/*  12 */	"chdir",
	/*  13 */	"fchdir",
	/*  14 */	"mknod",
	/*  15 */	"chmod",
	/*  16 */	"chown",
	/*  17 */	"break",
	/*  18 */	"compat_20_getfsstat",
	/*  19 */	"compat_43_olseek",
#ifdef COMPAT_43
	/*  20 */	"getpid",
#else
	/*  20 */	"getpid",
#endif
	/*  21 */	"compat_40_mount",
	/*  22 */	"unmount",
	/*  23 */	"setuid",
#ifdef COMPAT_43
	/*  24 */	"getuid",
#else
	/*  24 */	"getuid",
#endif
	/*  25 */	"geteuid",
	/*  26 */	"ptrace",
	/*  27 */	"recvmsg",
	/*  28 */	"sendmsg",
	/*  29 */	"recvfrom",
	/*  30 */	"accept",
	/*  31 */	"getpeername",
	/*  32 */	"getsockname",
	/*  33 */	"access",
	/*  34 */	"chflags",
	/*  35 */	"fchflags",
	/*  36 */	"sync",
	/*  37 */	"kill",
	/*  38 */	"compat_43_stat43",
	/*  39 */	"getppid",
	/*  40 */	"compat_43_lstat43",
	/*  41 */	"dup",
	/*  42 */	"pipe",
	/*  43 */	"getegid",
	/*  44 */	"profil",
	/*  45 */	"ktrace",
	/*  46 */	"compat_13_sigaction13",
#ifdef COMPAT_43
	/*  47 */	"getgid",
#else
	/*  47 */	"getgid",
#endif
	/*  48 */	"compat_13_sigprocmask13",
	/*  49 */	"__getlogin",
	/*  50 */	"__setlogin",
	/*  51 */	"acct",
	/*  52 */	"compat_13_sigpending13",
	/*  53 */	"compat_13_sigaltstack13",
	/*  54 */	"ioctl",
	/*  55 */	"compat_12_oreboot",
	/*  56 */	"revoke",
	/*  57 */	"symlink",
	/*  58 */	"readlink",
	/*  59 */	"execve",
	/*  60 */	"umask",
	/*  61 */	"chroot",
	/*  62 */	"compat_43_fstat43",
	/*  63 */	"compat_43_ogetkerninfo",
	/*  64 */	"compat_43_ogetpagesize",
	/*  65 */	"compat_12_msync",
	/*  66 */	"vfork",
	/*  67 */	"#67 (obsolete vread)",
	/*  68 */	"#68 (obsolete vwrite)",
	/*  69 */	"sbrk",
	/*  70 */	"sstk",
	/*  71 */	"compat_43_ommap",
	/*  72 */	"vadvise",
	/*  73 */	"munmap",
	/*  74 */	"mprotect",
	/*  75 */	"madvise",
	/*  76 */	"#76 (obsolete vhangup)",
	/*  77 */	"#77 (obsolete vlimit)",
	/*  78 */	"mincore",
	/*  79 */	"getgroups",
	/*  80 */	"setgroups",
	/*  81 */	"getpgrp",
	/*  82 */	"setpgid",
	/*  83 */	"setitimer",
	/*  84 */	"compat_43_owait",
	/*  85 */	"compat_12_oswapon",
	/*  86 */	"getitimer",
	/*  87 */	"compat_43_ogethostname",
	/*  88 */	"compat_43_osethostname",
	/*  89 */	"compat_43_ogetdtablesize",
	/*  90 */	"dup2",
	/*  91 */	"#91 (unimplemented getdopt)",
	/*  92 */	"fcntl",
	/*  93 */	"select",
	/*  94 */	"#94 (unimplemented setdopt)",
	/*  95 */	"fsync",
	/*  96 */	"setpriority",
	/*  97 */	"compat_30_socket",
	/*  98 */	"connect",
	/*  99 */	"compat_43_oaccept",
	/* 100 */	"getpriority",
	/* 101 */	"compat_43_osend",
	/* 102 */	"compat_43_orecv",
	/* 103 */	"compat_13_sigreturn13",
	/* 104 */	"bind",
	/* 105 */	"setsockopt",
	/* 106 */	"listen",
	/* 107 */	"#107 (obsolete vtimes)",
	/* 108 */	"compat_43_osigvec",
	/* 109 */	"compat_43_osigblock",
	/* 110 */	"compat_43_osigsetmask",
	/* 111 */	"compat_13_sigsuspend13",
	/* 112 */	"compat_43_osigstack",
	/* 113 */	"compat_43_orecvmsg",
	/* 114 */	"compat_43_osendmsg",
	/* 115 */	"#115 (obsolete vtrace)",
	/* 116 */	"gettimeofday",
	/* 117 */	"getrusage",
	/* 118 */	"getsockopt",
	/* 119 */	"#119 (obsolete resuba)",
	/* 120 */	"readv",
	/* 121 */	"writev",
	/* 122 */	"settimeofday",
	/* 123 */	"fchown",
	/* 124 */	"fchmod",
	/* 125 */	"compat_43_orecvfrom",
	/* 126 */	"setreuid",
	/* 127 */	"setregid",
	/* 128 */	"rename",
	/* 129 */	"compat_43_otruncate",
	/* 130 */	"compat_43_oftruncate",
	/* 131 */	"flock",
	/* 132 */	"mkfifo",
	/* 133 */	"sendto",
	/* 134 */	"shutdown",
	/* 135 */	"socketpair",
	/* 136 */	"mkdir",
	/* 137 */	"rmdir",
	/* 138 */	"utimes",
	/* 139 */	"#139 (obsolete 4.2 sigreturn)",
	/* 140 */	"adjtime",
	/* 141 */	"compat_43_ogetpeername",
	/* 142 */	"compat_43_ogethostid",
	/* 143 */	"compat_43_osethostid",
	/* 144 */	"compat_43_ogetrlimit",
	/* 145 */	"compat_43_osetrlimit",
	/* 146 */	"compat_43_okillpg",
	/* 147 */	"setsid",
	/* 148 */	"quotactl",
	/* 149 */	"compat_43_oquota",
	/* 150 */	"compat_43_ogetsockname",
	/* 151 */	"#151 (unimplemented)",
	/* 152 */	"#152 (unimplemented)",
	/* 153 */	"#153 (unimplemented)",
	/* 154 */	"#154 (unimplemented)",
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	/* 155 */	"nfssvc",
#else
	/* 155 */	"#155 (excluded nfssvc)",
#endif
	/* 156 */	"compat_43_ogetdirentries",
	/* 157 */	"compat_20_statfs",
	/* 158 */	"compat_20_fstatfs",
	/* 159 */	"#159 (unimplemented)",
	/* 160 */	"#160 (unimplemented)",
	/* 161 */	"compat_30_getfh",
	/* 162 */	"compat_09_ogetdomainname",
	/* 163 */	"compat_09_osetdomainname",
	/* 164 */	"compat_09_ouname",
	/* 165 */	"sysarch",
	/* 166 */	"#166 (unimplemented)",
	/* 167 */	"#167 (unimplemented)",
	/* 168 */	"#168 (unimplemented)",
#if (defined(SYSVSEM) || !defined(_KERNEL)) && !defined(_LP64)
	/* 169 */	"compat_10_osemsys",
#else
	/* 169 */	"#169 (excluded 1.0 semsys)",
#endif
#if (defined(SYSVMSG) || !defined(_KERNEL)) && !defined(_LP64)
	/* 170 */	"compat_10_omsgsys",
#else
	/* 170 */	"#170 (excluded 1.0 msgsys)",
#endif
#if (defined(SYSVSHM) || !defined(_KERNEL)) && !defined(_LP64)
	/* 171 */	"compat_10_oshmsys",
#else
	/* 171 */	"#171 (excluded 1.0 shmsys)",
#endif
	/* 172 */	"#172 (unimplemented)",
	/* 173 */	"pread",
	/* 174 */	"pwrite",
	/* 175 */	"compat_30_ntp_gettime",
#if defined(NTP) || !defined(_KERNEL)
	/* 176 */	"ntp_adjtime",
#else
	/* 176 */	"#176 (excluded ntp_adjtime)",
#endif
	/* 177 */	"#177 (unimplemented)",
	/* 178 */	"#178 (unimplemented)",
	/* 179 */	"#179 (unimplemented)",
	/* 180 */	"#180 (unimplemented)",
	/* 181 */	"setgid",
	/* 182 */	"setegid",
	/* 183 */	"seteuid",
#if defined(LFS) || !defined(_KERNEL)
	/* 184 */	"lfs_bmapv",
	/* 185 */	"lfs_markv",
	/* 186 */	"lfs_segclean",
	/* 187 */	"lfs_segwait",
#else
	/* 184 */	"#184 (excluded lfs_bmapv)",
	/* 185 */	"#185 (excluded lfs_markv)",
	/* 186 */	"#186 (excluded lfs_segclean)",
	/* 187 */	"#187 (excluded lfs_segwait)",
#endif
	/* 188 */	"compat_12_stat12",
	/* 189 */	"compat_12_fstat12",
	/* 190 */	"compat_12_lstat12",
	/* 191 */	"pathconf",
	/* 192 */	"fpathconf",
	/* 193 */	"#193 (unimplemented)",
	/* 194 */	"getrlimit",
	/* 195 */	"setrlimit",
	/* 196 */	"compat_12_getdirentries",
	/* 197 */	"mmap",
	/* 198 */	"__syscall",
	/* 199 */	"lseek",
	/* 200 */	"truncate",
	/* 201 */	"ftruncate",
	/* 202 */	"__sysctl",
	/* 203 */	"mlock",
	/* 204 */	"munlock",
	/* 205 */	"undelete",
	/* 206 */	"futimes",
	/* 207 */	"getpgid",
	/* 208 */	"reboot",
	/* 209 */	"poll",
#if defined(LKM) || !defined(_KERNEL)
	/* 210 */	"lkmnosys",
	/* 211 */	"lkmnosys",
	/* 212 */	"lkmnosys",
	/* 213 */	"lkmnosys",
	/* 214 */	"lkmnosys",
	/* 215 */	"lkmnosys",
	/* 216 */	"lkmnosys",
	/* 217 */	"lkmnosys",
	/* 218 */	"lkmnosys",
	/* 219 */	"lkmnosys",
#else	/* !LKM */
	/* 210 */	"#210 (excluded lkmnosys)",
	/* 211 */	"#211 (excluded lkmnosys)",
	/* 212 */	"#212 (excluded lkmnosys)",
	/* 213 */	"#213 (excluded lkmnosys)",
	/* 214 */	"#214 (excluded lkmnosys)",
	/* 215 */	"#215 (excluded lkmnosys)",
	/* 216 */	"#216 (excluded lkmnosys)",
	/* 217 */	"#217 (excluded lkmnosys)",
	/* 218 */	"#218 (excluded lkmnosys)",
	/* 219 */	"#219 (excluded lkmnosys)",
#endif	/* !LKM */
#if defined(SYSVSEM) || !defined(_KERNEL)
	/* 220 */	"compat_14___semctl",
	/* 221 */	"semget",
	/* 222 */	"semop",
	/* 223 */	"semconfig",
#else
	/* 220 */	"#220 (excluded compat_14_semctl)",
	/* 221 */	"#221 (excluded semget)",
	/* 222 */	"#222 (excluded semop)",
	/* 223 */	"#223 (excluded semconfig)",
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
	/* 224 */	"compat_14_msgctl",
	/* 225 */	"msgget",
	/* 226 */	"msgsnd",
	/* 227 */	"msgrcv",
#else
	/* 224 */	"#224 (excluded compat_14_msgctl)",
	/* 225 */	"#225 (excluded msgget)",
	/* 226 */	"#226 (excluded msgsnd)",
	/* 227 */	"#227 (excluded msgrcv)",
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
	/* 228 */	"shmat",
	/* 229 */	"compat_14_shmctl",
	/* 230 */	"shmdt",
	/* 231 */	"shmget",
#else
	/* 228 */	"#228 (excluded shmat)",
	/* 229 */	"#229 (excluded compat_14_shmctl)",
	/* 230 */	"#230 (excluded shmdt)",
	/* 231 */	"#231 (excluded shmget)",
#endif
	/* 232 */	"clock_gettime",
	/* 233 */	"clock_settime",
	/* 234 */	"clock_getres",
	/* 235 */	"timer_create",
	/* 236 */	"timer_delete",
	/* 237 */	"timer_settime",
	/* 238 */	"timer_gettime",
	/* 239 */	"timer_getoverrun",
	/* 240 */	"nanosleep",
	/* 241 */	"fdatasync",
	/* 242 */	"mlockall",
	/* 243 */	"munlockall",
	/* 244 */	"__sigtimedwait",
	/* 245 */	"#245 (unimplemented sys_sigqueue)",
	/* 246 */	"modctl",
#if defined(P1003_1B_SEMAPHORE) || (!defined(_KERNEL) && defined(_LIBC))
	/* 247 */	"_ksem_init",
	/* 248 */	"_ksem_open",
	/* 249 */	"_ksem_unlink",
	/* 250 */	"_ksem_close",
	/* 251 */	"_ksem_post",
	/* 252 */	"_ksem_wait",
	/* 253 */	"_ksem_trywait",
	/* 254 */	"_ksem_getvalue",
	/* 255 */	"_ksem_destroy",
	/* 256 */	"#256 (unimplemented sys__ksem_timedwait)",
#else
	/* 247 */	"#247 (excluded sys__ksem_init)",
	/* 248 */	"#248 (excluded sys__ksem_open)",
	/* 249 */	"#249 (excluded sys__ksem_unlink)",
	/* 250 */	"#250 (excluded sys__ksem_close)",
	/* 251 */	"#251 (excluded sys__ksem_post)",
	/* 252 */	"#252 (excluded sys__ksem_wait)",
	/* 253 */	"#253 (excluded sys__ksem_trywait)",
	/* 254 */	"#254 (excluded sys__ksem_getvalue)",
	/* 255 */	"#255 (excluded sys__ksem_destroy)",
	/* 256 */	"#256 (unimplemented sys__ksem_timedwait)",
#endif
	/* 257 */	"mq_open",
	/* 258 */	"mq_close",
	/* 259 */	"mq_unlink",
	/* 260 */	"mq_getattr",
	/* 261 */	"mq_setattr",
	/* 262 */	"mq_notify",
	/* 263 */	"mq_send",
	/* 264 */	"mq_receive",
	/* 265 */	"mq_timedsend",
	/* 266 */	"mq_timedreceive",
	/* 267 */	"#267 (unimplemented)",
	/* 268 */	"#268 (unimplemented)",
	/* 269 */	"#269 (unimplemented)",
	/* 270 */	"__posix_rename",
	/* 271 */	"swapctl",
	/* 272 */	"compat_30_getdents",
	/* 273 */	"minherit",
	/* 274 */	"lchmod",
	/* 275 */	"lchown",
	/* 276 */	"lutimes",
	/* 277 */	"__msync13",
	/* 278 */	"compat_30___stat13",
	/* 279 */	"compat_30___fstat13",
	/* 280 */	"compat_30___lstat13",
	/* 281 */	"__sigaltstack14",
	/* 282 */	"__vfork14",
	/* 283 */	"__posix_chown",
	/* 284 */	"__posix_fchown",
	/* 285 */	"__posix_lchown",
	/* 286 */	"getsid",
	/* 287 */	"__clone",
	/* 288 */	"fktrace",
	/* 289 */	"preadv",
	/* 290 */	"pwritev",
	/* 291 */	"compat_16___sigaction14",
	/* 292 */	"__sigpending14",
	/* 293 */	"__sigprocmask14",
	/* 294 */	"__sigsuspend14",
	/* 295 */	"compat_16___sigreturn14",
	/* 296 */	"__getcwd",
	/* 297 */	"fchroot",
	/* 298 */	"compat_30_fhopen",
	/* 299 */	"compat_30_fhstat",
	/* 300 */	"compat_20_fhstatfs",
#if defined(SYSVSEM) || !defined(_KERNEL)
	/* 301 */	"____semctl13",
#else
	/* 301 */	"#301 (excluded ____semctl13)",
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
	/* 302 */	"__msgctl13",
#else
	/* 302 */	"#302 (excluded __msgctl13)",
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
	/* 303 */	"__shmctl13",
#else
	/* 303 */	"#303 (excluded __shmctl13)",
#endif
	/* 304 */	"lchflags",
	/* 305 */	"issetugid",
	/* 306 */	"utrace",
	/* 307 */	"getcontext",
	/* 308 */	"setcontext",
	/* 309 */	"_lwp_create",
	/* 310 */	"_lwp_exit",
	/* 311 */	"_lwp_self",
	/* 312 */	"_lwp_wait",
	/* 313 */	"_lwp_suspend",
	/* 314 */	"_lwp_continue",
	/* 315 */	"_lwp_wakeup",
	/* 316 */	"_lwp_getprivate",
	/* 317 */	"_lwp_setprivate",
	/* 318 */	"_lwp_kill",
	/* 319 */	"_lwp_detach",
	/* 320 */	"_lwp_park",
	/* 321 */	"_lwp_unpark",
	/* 322 */	"_lwp_unpark_all",
	/* 323 */	"_lwp_setname",
	/* 324 */	"_lwp_getname",
	/* 325 */	"_lwp_ctl",
	/* 326 */	"#326 (unimplemented)",
	/* 327 */	"#327 (unimplemented)",
	/* 328 */	"#328 (unimplemented)",
	/* 329 */	"#329 (unimplemented)",
	/* 330 */	"sa_register",
	/* 331 */	"sa_stacks",
	/* 332 */	"sa_enable",
	/* 333 */	"sa_setconcurrency",
	/* 334 */	"sa_yield",
	/* 335 */	"sa_preempt",
	/* 336 */	"sa_unblockyield",
	/* 337 */	"#337 (unimplemented)",
	/* 338 */	"#338 (unimplemented)",
	/* 339 */	"#339 (unimplemented)",
	/* 340 */	"__sigaction_sigtramp",
	/* 341 */	"pmc_get_info",
	/* 342 */	"pmc_control",
	/* 343 */	"rasctl",
	/* 344 */	"kqueue",
	/* 345 */	"kevent",
	/* 346 */	"_sched_setparam",
	/* 347 */	"_sched_getparam",
	/* 348 */	"_sched_setaffinity",
	/* 349 */	"_sched_getaffinity",
	/* 350 */	"sched_yield",
	/* 351 */	"#351 (unimplemented)",
	/* 352 */	"#352 (unimplemented)",
	/* 353 */	"#353 (unimplemented)",
	/* 354 */	"fsync_range",
	/* 355 */	"uuidgen",
	/* 356 */	"getvfsstat",
	/* 357 */	"statvfs1",
	/* 358 */	"fstatvfs1",
	/* 359 */	"compat_30_fhstatvfs1",
	/* 360 */	"extattrctl",
	/* 361 */	"extattr_set_file",
	/* 362 */	"extattr_get_file",
	/* 363 */	"extattr_delete_file",
	/* 364 */	"extattr_set_fd",
	/* 365 */	"extattr_get_fd",
	/* 366 */	"extattr_delete_fd",
	/* 367 */	"extattr_set_link",
	/* 368 */	"extattr_get_link",
	/* 369 */	"extattr_delete_link",
	/* 370 */	"extattr_list_fd",
	/* 371 */	"extattr_list_file",
	/* 372 */	"extattr_list_link",
	/* 373 */	"pselect",
	/* 374 */	"pollts",
	/* 375 */	"setxattr",
	/* 376 */	"lsetxattr",
	/* 377 */	"fsetxattr",
	/* 378 */	"getxattr",
	/* 379 */	"lgetxattr",
	/* 380 */	"fgetxattr",
	/* 381 */	"listxattr",
	/* 382 */	"llistxattr",
	/* 383 */	"flistxattr",
	/* 384 */	"removexattr",
	/* 385 */	"lremovexattr",
	/* 386 */	"fremovexattr",
	/* 387 */	"__stat30",
	/* 388 */	"__fstat30",
	/* 389 */	"__lstat30",
	/* 390 */	"__getdents30",
	/* 391 */	"#391 (ignored old posix_fadvise)",
	/* 392 */	"compat_30___fhstat30",
	/* 393 */	"__ntp_gettime30",
	/* 394 */	"__socket30",
	/* 395 */	"__getfh30",
	/* 396 */	"__fhopen40",
	/* 397 */	"__fhstatvfs140",
	/* 398 */	"__fhstat40",
	/* 399 */	"aio_cancel",
	/* 400 */	"aio_error",
	/* 401 */	"aio_fsync",
	/* 402 */	"aio_read",
	/* 403 */	"aio_return",
	/* 404 */	"aio_suspend",
	/* 405 */	"aio_write",
	/* 406 */	"lio_listio",
	/* 407 */	"#407 (unimplemented)",
	/* 408 */	"#408 (unimplemented)",
	/* 409 */	"#409 (unimplemented)",
	/* 410 */	"__mount50",
	/* 411 */	"mremap",
	/* 412 */	"pset_create",
	/* 413 */	"pset_destroy",
	/* 414 */	"pset_assign",
	/* 415 */	"_pset_bind",
	/* 416 */	"__posix_fadvise50",
};
