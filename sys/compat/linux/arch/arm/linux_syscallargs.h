/* $NetBSD: linux_syscallargs.h,v 1.34 2008/01/15 22:40:14 njoly Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.33 2008/01/15 22:38:34 njoly Exp
 */

#ifndef _LINUX_SYS_SYSCALLARGS_H_
#define	_LINUX_SYS_SYSCALLARGS_H_

#define	LINUX_SYS_MAXSYSARGS	8

#undef	syscallarg
#define	syscallarg(x)							\
	union {								\
		register_t pad;						\
		struct { x datum; } le;					\
		struct { /* LINTED zero array dimension */		\
			int8_t pad[  /* CONSTCOND */			\
				(sizeof (register_t) < sizeof (x))	\
				? 0					\
				: sizeof (register_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

#undef check_syscall_args
#define check_syscall_args(call) \
	typedef char call##_check_args[sizeof (struct call##_args) \
		<= LINUX_SYS_MAXSYSARGS * sizeof (register_t) ? 1 : -1];

struct sys_exit_args;

struct sys_read_args;

struct sys_write_args;

struct linux_sys_open_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};
check_syscall_args(linux_sys_open)

struct sys_close_args;

struct linux_sys_waitpid_args {
	syscallarg(int) pid;
	syscallarg(int *) status;
	syscallarg(int) options;
};
check_syscall_args(linux_sys_waitpid)

struct linux_sys_creat_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
};
check_syscall_args(linux_sys_creat)

struct sys_link_args;

struct linux_sys_unlink_args {
	syscallarg(const char *) path;
};
check_syscall_args(linux_sys_unlink)

struct sys_execve_args;

struct sys_chdir_args;

struct linux_sys_time_args {
	syscallarg(linux_time_t *) t;
};
check_syscall_args(linux_sys_time)

struct linux_sys_mknod_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
	syscallarg(int) dev;
};
check_syscall_args(linux_sys_mknod)

struct sys_chmod_args;

struct linux_sys_lchown16_args {
	syscallarg(const char *) path;
	syscallarg(linux_uid16_t) uid;
	syscallarg(linux_gid16_t) gid;
};
check_syscall_args(linux_sys_lchown16)

struct compat_43_sys_lseek_args;

struct sys_setuid_args;

struct linux_sys_stime_args {
	syscallarg(linux_time_t *) t;
};
check_syscall_args(linux_sys_stime)

struct linux_sys_ptrace_args {
	syscallarg(int) request;
	syscallarg(int) pid;
	syscallarg(int) addr;
	syscallarg(int) data;
};
check_syscall_args(linux_sys_ptrace)

struct linux_sys_alarm_args {
	syscallarg(unsigned int) secs;
};
check_syscall_args(linux_sys_alarm)

struct linux_sys_utime_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_utimbuf *) times;
};
check_syscall_args(linux_sys_utime)

struct sys_access_args;

struct linux_sys_nice_args {
	syscallarg(int) incr;
};
check_syscall_args(linux_sys_nice)

struct linux_sys_kill_args {
	syscallarg(int) pid;
	syscallarg(int) signum;
};
check_syscall_args(linux_sys_kill)

struct sys___posix_rename_args;

struct sys_mkdir_args;

struct sys_rmdir_args;

struct sys_dup_args;

struct linux_sys_pipe_args {
	syscallarg(int *) pfds;
};
check_syscall_args(linux_sys_pipe)

struct linux_sys_times_args {
	syscallarg(struct times *) tms;
};
check_syscall_args(linux_sys_times)

struct linux_sys_brk_args {
	syscallarg(char *) nsize;
};
check_syscall_args(linux_sys_brk)

struct sys_setgid_args;

struct linux_sys_signal_args {
	syscallarg(int) signum;
	syscallarg(linux_handler_t) handler;
};
check_syscall_args(linux_sys_signal)

struct sys_acct_args;

struct linux_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(void *) data;
};
check_syscall_args(linux_sys_ioctl)

struct linux_sys_fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(void *) arg;
};
check_syscall_args(linux_sys_fcntl)

struct sys_setpgid_args;

struct linux_sys_oldolduname_args {
	syscallarg(struct linux_oldold_utsname *) up;
};
check_syscall_args(linux_sys_oldolduname)

struct sys_umask_args;

struct sys_chroot_args;

struct sys_dup2_args;

struct linux_sys_sigaction_args {
	syscallarg(int) signum;
	syscallarg(const struct linux_old_sigaction *) nsa;
	syscallarg(struct linux_old_sigaction *) osa;
};
check_syscall_args(linux_sys_sigaction)

struct linux_sys_sigsetmask_args {
	syscallarg(linux_old_sigset_t) mask;
};
check_syscall_args(linux_sys_sigsetmask)

struct linux_sys_setreuid16_args {
	syscallarg(linux_uid16_t) ruid;
	syscallarg(linux_uid16_t) euid;
};
check_syscall_args(linux_sys_setreuid16)

struct linux_sys_setregid16_args {
	syscallarg(linux_gid16_t) rgid;
	syscallarg(linux_gid16_t) egid;
};
check_syscall_args(linux_sys_setregid16)

struct linux_sys_sigsuspend_args {
	syscallarg(void *) restart;
	syscallarg(int) oldmask;
	syscallarg(int) mask;
};
check_syscall_args(linux_sys_sigsuspend)

struct linux_sys_sigpending_args {
	syscallarg(linux_old_sigset_t *) set;
};
check_syscall_args(linux_sys_sigpending)

struct compat_43_sys_sethostname_args;

struct linux_sys_setrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct orlimit *) rlp;
};
check_syscall_args(linux_sys_setrlimit)

struct linux_sys_getrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct orlimit *) rlp;
};
check_syscall_args(linux_sys_getrlimit)

struct sys_getrusage_args;

struct linux_sys_gettimeofday_args {
	syscallarg(struct timeval *) tp;
	syscallarg(struct timezone *) tzp;
};
check_syscall_args(linux_sys_gettimeofday)

struct linux_sys_settimeofday_args {
	syscallarg(struct timeval *) tp;
	syscallarg(struct timezone *) tzp;
};
check_syscall_args(linux_sys_settimeofday)

struct linux_sys_getgroups16_args {
	syscallarg(int) gidsetsize;
	syscallarg(linux_gid16_t *) gidset;
};
check_syscall_args(linux_sys_getgroups16)

struct linux_sys_setgroups16_args {
	syscallarg(int) gidsetsize;
	syscallarg(linux_gid16_t *) gidset;
};
check_syscall_args(linux_sys_setgroups16)

struct linux_sys_oldselect_args {
	syscallarg(struct linux_oldselect *) lsp;
};
check_syscall_args(linux_sys_oldselect)

struct sys_symlink_args;

struct compat_43_sys_lstat_args;

struct sys_readlink_args;
#ifdef EXEC_AOUT

struct linux_sys_uselib_args {
	syscallarg(const char *) path;
};
check_syscall_args(linux_sys_uselib)
#else
#endif

struct linux_sys_swapon_args {
	syscallarg(char *) name;
};
check_syscall_args(linux_sys_swapon)

struct linux_sys_reboot_args {
	syscallarg(int) magic1;
	syscallarg(int) magic2;
	syscallarg(int) cmd;
	syscallarg(void *) arg;
};
check_syscall_args(linux_sys_reboot)

struct linux_sys_readdir_args {
	syscallarg(int) fd;
	syscallarg(void *) dent;
	syscallarg(unsigned int) count;
};
check_syscall_args(linux_sys_readdir)

struct linux_sys_old_mmap_args {
	syscallarg(struct linux_oldmmap *) lmp;
};
check_syscall_args(linux_sys_old_mmap)

struct sys_munmap_args;

struct compat_43_sys_truncate_args;

struct compat_43_sys_ftruncate_args;

struct sys_fchmod_args;

struct linux_sys_fchown16_args {
	syscallarg(int) fd;
	syscallarg(linux_uid16_t) uid;
	syscallarg(linux_gid16_t) gid;
};
check_syscall_args(linux_sys_fchown16)

struct linux_sys_getpriority_args {
	syscallarg(int) which;
	syscallarg(int) who;
};
check_syscall_args(linux_sys_getpriority)

struct sys_setpriority_args;

struct sys_profil_args;

struct linux_sys_statfs_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_statfs *) sp;
};
check_syscall_args(linux_sys_statfs)

struct linux_sys_fstatfs_args {
	syscallarg(int) fd;
	syscallarg(struct linux_statfs *) sp;
};
check_syscall_args(linux_sys_fstatfs)

struct linux_sys_socketcall_args {
	syscallarg(int) what;
	syscallarg(void *) args;
};
check_syscall_args(linux_sys_socketcall)

struct sys_setitimer_args;

struct sys_getitimer_args;

struct linux_sys_stat_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_stat *) sp;
};
check_syscall_args(linux_sys_stat)

struct linux_sys_lstat_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_stat *) sp;
};
check_syscall_args(linux_sys_lstat)

struct linux_sys_fstat_args {
	syscallarg(int) fd;
	syscallarg(struct linux_stat *) sp;
};
check_syscall_args(linux_sys_fstat)

struct linux_sys_olduname_args {
	syscallarg(struct linux_oldutsname *) up;
};
check_syscall_args(linux_sys_olduname)

struct linux_sys_wait4_args {
	syscallarg(int) pid;
	syscallarg(int *) status;
	syscallarg(int) options;
	syscallarg(struct rusage *) rusage;
};
check_syscall_args(linux_sys_wait4)

struct linux_sys_swapoff_args {
	syscallarg(const char *) path;
};
check_syscall_args(linux_sys_swapoff)

struct linux_sys_sysinfo_args {
	syscallarg(struct linux_sysinfo *) arg;
};
check_syscall_args(linux_sys_sysinfo)

struct linux_sys_ipc_args {
	syscallarg(int) what;
	syscallarg(int) a1;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(void *) ptr;
};
check_syscall_args(linux_sys_ipc)

struct sys_fsync_args;

struct linux_sys_sigreturn_args {
	syscallarg(struct linux_sigcontext *) scp;
};
check_syscall_args(linux_sys_sigreturn)

struct linux_sys_clone_args {
	syscallarg(int) flags;
	syscallarg(void *) stack;
};
check_syscall_args(linux_sys_clone)

struct linux_sys_setdomainname_args {
	syscallarg(char *) domainname;
	syscallarg(int) len;
};
check_syscall_args(linux_sys_setdomainname)

struct linux_sys_uname_args {
	syscallarg(struct linux_utsname *) up;
};
check_syscall_args(linux_sys_uname)

struct linux_sys_mprotect_args {
	syscallarg(const void *) start;
	syscallarg(unsigned long) len;
	syscallarg(int) prot;
};
check_syscall_args(linux_sys_mprotect)

struct linux_sys_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(const linux_old_sigset_t *) set;
	syscallarg(linux_old_sigset_t *) oset;
};
check_syscall_args(linux_sys_sigprocmask)

struct sys_getpgid_args;

struct sys_fchdir_args;

struct linux_sys_personality_args {
	syscallarg(int) per;
};
check_syscall_args(linux_sys_personality)

struct linux_sys_setfsuid_args {
	syscallarg(uid_t) uid;
};
check_syscall_args(linux_sys_setfsuid)

struct linux_sys_setfsgid_args {
	syscallarg(gid_t) gid;
};
check_syscall_args(linux_sys_setfsgid)

struct linux_sys_llseek_args {
	syscallarg(int) fd;
	syscallarg(u_int32_t) ohigh;
	syscallarg(u_int32_t) olow;
	syscallarg(void *) res;
	syscallarg(int) whence;
};
check_syscall_args(linux_sys_llseek)

struct linux_sys_getdents_args {
	syscallarg(int) fd;
	syscallarg(struct linux_dirent *) dent;
	syscallarg(unsigned int) count;
};
check_syscall_args(linux_sys_getdents)

struct linux_sys_select_args {
	syscallarg(int) nfds;
	syscallarg(fd_set *) readfds;
	syscallarg(fd_set *) writefds;
	syscallarg(fd_set *) exceptfds;
	syscallarg(struct timeval *) timeout;
};
check_syscall_args(linux_sys_select)

struct sys_flock_args;

struct linux_sys_msync_args {
	syscallarg(void *) addr;
	syscallarg(int) len;
	syscallarg(int) fl;
};
check_syscall_args(linux_sys_msync)

struct sys_readv_args;

struct sys_writev_args;

struct sys_getsid_args;

struct linux_sys_fdatasync_args {
	syscallarg(int) fd;
};
check_syscall_args(linux_sys_fdatasync)

struct linux_sys___sysctl_args {
	syscallarg(struct linux___sysctl *) lsp;
};
check_syscall_args(linux_sys___sysctl)

struct sys_mlock_args;

struct sys_munlock_args;

struct sys_mlockall_args;

struct linux_sys_sched_setparam_args {
	syscallarg(pid_t) pid;
	syscallarg(const struct linux_sched_param *) sp;
};
check_syscall_args(linux_sys_sched_setparam)

struct linux_sys_sched_getparam_args {
	syscallarg(pid_t) pid;
	syscallarg(struct linux_sched_param *) sp;
};
check_syscall_args(linux_sys_sched_getparam)

struct linux_sys_sched_setscheduler_args {
	syscallarg(pid_t) pid;
	syscallarg(int) policy;
	syscallarg(const struct linux_sched_param *) sp;
};
check_syscall_args(linux_sys_sched_setscheduler)

struct linux_sys_sched_getscheduler_args {
	syscallarg(pid_t) pid;
};
check_syscall_args(linux_sys_sched_getscheduler)

struct linux_sys_sched_get_priority_max_args {
	syscallarg(int) policy;
};
check_syscall_args(linux_sys_sched_get_priority_max)

struct linux_sys_sched_get_priority_min_args {
	syscallarg(int) policy;
};
check_syscall_args(linux_sys_sched_get_priority_min)

struct sys_nanosleep_args;

struct linux_sys_mremap_args {
	syscallarg(void *) old_address;
	syscallarg(size_t) old_size;
	syscallarg(size_t) new_size;
	syscallarg(u_long) flags;
};
check_syscall_args(linux_sys_mremap)

struct linux_sys_setresuid16_args {
	syscallarg(linux_uid16_t) ruid;
	syscallarg(linux_uid16_t) euid;
	syscallarg(linux_uid16_t) suid;
};
check_syscall_args(linux_sys_setresuid16)

struct linux_sys_getresuid16_args {
	syscallarg(linux_uid16_t *) ruid;
	syscallarg(linux_uid16_t *) euid;
	syscallarg(linux_uid16_t *) suid;
};
check_syscall_args(linux_sys_getresuid16)

struct sys_poll_args;

struct linux_sys_setresgid16_args {
	syscallarg(linux_gid16_t) rgid;
	syscallarg(linux_gid16_t) egid;
	syscallarg(linux_gid16_t) sgid;
};
check_syscall_args(linux_sys_setresgid16)

struct linux_sys_getresgid16_args {
	syscallarg(linux_gid16_t *) rgid;
	syscallarg(linux_gid16_t *) egid;
	syscallarg(linux_gid16_t *) sgid;
};
check_syscall_args(linux_sys_getresgid16)

struct linux_sys_rt_sigaction_args {
	syscallarg(int) signum;
	syscallarg(const struct linux_sigaction *) nsa;
	syscallarg(struct linux_sigaction *) osa;
	syscallarg(size_t) sigsetsize;
};
check_syscall_args(linux_sys_rt_sigaction)

struct linux_sys_rt_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(const linux_sigset_t *) set;
	syscallarg(linux_sigset_t *) oset;
	syscallarg(size_t) sigsetsize;
};
check_syscall_args(linux_sys_rt_sigprocmask)

struct linux_sys_rt_sigpending_args {
	syscallarg(linux_sigset_t *) set;
	syscallarg(size_t) sigsetsize;
};
check_syscall_args(linux_sys_rt_sigpending)

struct linux_sys_rt_queueinfo_args {
	syscallarg(int) pid;
	syscallarg(int) signum;
	syscallarg(void *) uinfo;
};
check_syscall_args(linux_sys_rt_queueinfo)

struct linux_sys_rt_sigsuspend_args {
	syscallarg(linux_sigset_t *) unewset;
	syscallarg(size_t) sigsetsize;
};
check_syscall_args(linux_sys_rt_sigsuspend)

struct linux_sys_pread_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(size_t) nbyte;
	syscallarg(linux_off_t) offset;
};
check_syscall_args(linux_sys_pread)

struct linux_sys_pwrite_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(size_t) nbyte;
	syscallarg(linux_off_t) offset;
};
check_syscall_args(linux_sys_pwrite)

struct linux_sys_chown16_args {
	syscallarg(const char *) path;
	syscallarg(linux_uid16_t) uid;
	syscallarg(linux_gid16_t) gid;
};
check_syscall_args(linux_sys_chown16)

struct sys___getcwd_args;

struct linux_sys_sigaltstack_args {
	syscallarg(const struct linux_sigaltstack *) ss;
	syscallarg(struct linux_sigaltstack *) oss;
};
check_syscall_args(linux_sys_sigaltstack)

struct linux_sys_ugetrlimit_args {
	syscallarg(int) which;
	syscallarg(struct rlimit *) rlp;
};
check_syscall_args(linux_sys_ugetrlimit)
#define linux_sys_mmap2_args linux_sys_mmap_args

struct linux_sys_mmap2_args;

struct linux_sys_truncate64_args {
	syscallarg(const char *) path;
	syscallarg(off_t) length;
};
check_syscall_args(linux_sys_truncate64)

struct linux_sys_ftruncate64_args {
	syscallarg(unsigned int) fd;
	syscallarg(off_t) length;
};
check_syscall_args(linux_sys_ftruncate64)

struct linux_sys_stat64_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_stat64 *) sp;
};
check_syscall_args(linux_sys_stat64)

struct linux_sys_lstat64_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_stat64 *) sp;
};
check_syscall_args(linux_sys_lstat64)

struct linux_sys_fstat64_args {
	syscallarg(int) fd;
	syscallarg(struct linux_stat64 *) sp;
};
check_syscall_args(linux_sys_fstat64)

struct sys___posix_lchown_args;

struct sys_setreuid_args;

struct sys_setregid_args;

struct sys_getgroups_args;

struct sys_setgroups_args;

struct sys___posix_fchown_args;

struct linux_sys_setresuid_args {
	syscallarg(uid_t) ruid;
	syscallarg(uid_t) euid;
	syscallarg(uid_t) suid;
};
check_syscall_args(linux_sys_setresuid)

struct linux_sys_getresuid_args {
	syscallarg(uid_t *) ruid;
	syscallarg(uid_t *) euid;
	syscallarg(uid_t *) suid;
};
check_syscall_args(linux_sys_getresuid)

struct linux_sys_setresgid_args {
	syscallarg(gid_t) rgid;
	syscallarg(gid_t) egid;
	syscallarg(gid_t) sgid;
};
check_syscall_args(linux_sys_setresgid)

struct linux_sys_getresgid_args {
	syscallarg(gid_t *) rgid;
	syscallarg(gid_t *) egid;
	syscallarg(gid_t *) sgid;
};
check_syscall_args(linux_sys_getresgid)

struct sys___posix_chown_args;

struct sys_setuid_args;

struct sys_setgid_args;

struct linux_sys_setfsuid_args;

struct linux_sys_setfsgid_args;

struct linux_sys_getdents64_args {
	syscallarg(int) fd;
	syscallarg(struct linux_dirent64 *) dent;
	syscallarg(unsigned int) count;
};
check_syscall_args(linux_sys_getdents64)

struct sys_mincore_args;

struct sys_madvise_args;

struct linux_sys_fcntl64_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(void *) arg;
};
check_syscall_args(linux_sys_fcntl64)

struct linux_sys_setxattr_args {
	syscallarg(char *) path;
	syscallarg(char *) name;
	syscallarg(void *) value;
	syscallarg(size_t) size;
	syscallarg(int) flags;
};
check_syscall_args(linux_sys_setxattr)

struct linux_sys_lsetxattr_args {
	syscallarg(char *) path;
	syscallarg(char *) name;
	syscallarg(void *) value;
	syscallarg(size_t) size;
	syscallarg(int) flags;
};
check_syscall_args(linux_sys_lsetxattr)

struct linux_sys_fsetxattr_args {
	syscallarg(int) fd;
	syscallarg(char *) name;
	syscallarg(void *) value;
	syscallarg(size_t) size;
	syscallarg(int) flags;
};
check_syscall_args(linux_sys_fsetxattr)

struct linux_sys_getxattr_args {
	syscallarg(char *) path;
	syscallarg(char *) name;
	syscallarg(void *) value;
	syscallarg(size_t) size;
};
check_syscall_args(linux_sys_getxattr)

struct linux_sys_lgetxattr_args {
	syscallarg(char *) path;
	syscallarg(char *) name;
	syscallarg(void *) value;
	syscallarg(size_t) size;
};
check_syscall_args(linux_sys_lgetxattr)

struct linux_sys_fgetxattr_args {
	syscallarg(int) fd;
	syscallarg(char *) name;
	syscallarg(void *) value;
	syscallarg(size_t) size;
};
check_syscall_args(linux_sys_fgetxattr)

struct linux_sys_listxattr_args {
	syscallarg(char *) path;
	syscallarg(char *) list;
	syscallarg(size_t) size;
};
check_syscall_args(linux_sys_listxattr)

struct linux_sys_llistxattr_args {
	syscallarg(char *) path;
	syscallarg(char *) list;
	syscallarg(size_t) size;
};
check_syscall_args(linux_sys_llistxattr)

struct linux_sys_flistxattr_args {
	syscallarg(int) fd;
	syscallarg(char *) list;
	syscallarg(size_t) size;
};
check_syscall_args(linux_sys_flistxattr)

struct linux_sys_removexattr_args {
	syscallarg(char *) path;
	syscallarg(char *) name;
};
check_syscall_args(linux_sys_removexattr)

struct linux_sys_lremovexattr_args {
	syscallarg(char *) path;
	syscallarg(char *) name;
};
check_syscall_args(linux_sys_lremovexattr)

struct linux_sys_fremovexattr_args {
	syscallarg(int) fd;
	syscallarg(char *) name;
};
check_syscall_args(linux_sys_fremovexattr)

struct linux_sys_exit_group_args {
	syscallarg(int) error_code;
};
check_syscall_args(linux_sys_exit_group)

struct linux_sys_clock_settime_args {
	syscallarg(clockid_t) which;
	syscallarg(struct linux_timespec *) tp;
};
check_syscall_args(linux_sys_clock_settime)

struct linux_sys_clock_gettime_args {
	syscallarg(clockid_t) which;
	syscallarg(struct linux_timespec *) tp;
};
check_syscall_args(linux_sys_clock_gettime)

struct linux_sys_clock_getres_args {
	syscallarg(clockid_t) which;
	syscallarg(struct linux_timespec *) tp;
};
check_syscall_args(linux_sys_clock_getres)

struct linux_sys_clock_nanosleep_args {
	syscallarg(clockid_t) which;
	syscallarg(int) flags;
	syscallarg(struct linux_timespec *) rqtp;
	syscallarg(struct linux_timespec *) rmtp;
};
check_syscall_args(linux_sys_clock_nanosleep)

struct linux_sys_statfs64_args {
	syscallarg(const char *) path;
	syscallarg(size_t) sz;
	syscallarg(struct linux_statfs64 *) sp;
};
check_syscall_args(linux_sys_statfs64)

struct linux_sys_fstatfs64_args {
	syscallarg(int) fd;
	syscallarg(size_t) sz;
	syscallarg(struct linux_statfs64 *) sp;
};
check_syscall_args(linux_sys_fstatfs64)

struct linux_sys_cacheflush_args {
	syscallarg(uintptr_t) from;
	syscallarg(intptr_t) to;
};
check_syscall_args(linux_sys_cacheflush)

/*
 * System call prototypes.
 */

int	linux_sys_nosys(struct lwp *, const void *, register_t *);

int	sys_exit(struct lwp *, const struct sys_exit_args *, register_t *);

int	sys_fork(struct lwp *, const void *, register_t *);

int	sys_read(struct lwp *, const struct sys_read_args *, register_t *);

int	sys_write(struct lwp *, const struct sys_write_args *, register_t *);

int	linux_sys_open(struct lwp *, const struct linux_sys_open_args *, register_t *);

int	sys_close(struct lwp *, const struct sys_close_args *, register_t *);

int	linux_sys_waitpid(struct lwp *, const struct linux_sys_waitpid_args *, register_t *);

int	linux_sys_creat(struct lwp *, const struct linux_sys_creat_args *, register_t *);

int	sys_link(struct lwp *, const struct sys_link_args *, register_t *);

int	linux_sys_unlink(struct lwp *, const struct linux_sys_unlink_args *, register_t *);

int	sys_execve(struct lwp *, const struct sys_execve_args *, register_t *);

int	sys_chdir(struct lwp *, const struct sys_chdir_args *, register_t *);

int	linux_sys_time(struct lwp *, const struct linux_sys_time_args *, register_t *);

int	linux_sys_mknod(struct lwp *, const struct linux_sys_mknod_args *, register_t *);

int	sys_chmod(struct lwp *, const struct sys_chmod_args *, register_t *);

int	linux_sys_lchown16(struct lwp *, const struct linux_sys_lchown16_args *, register_t *);

int	compat_43_sys_lseek(struct lwp *, const struct compat_43_sys_lseek_args *, register_t *);

int	sys_getpid(struct lwp *, const void *, register_t *);

int	sys_setuid(struct lwp *, const struct sys_setuid_args *, register_t *);

int	sys_getuid(struct lwp *, const void *, register_t *);

int	linux_sys_stime(struct lwp *, const struct linux_sys_stime_args *, register_t *);

int	linux_sys_ptrace(struct lwp *, const struct linux_sys_ptrace_args *, register_t *);

int	linux_sys_alarm(struct lwp *, const struct linux_sys_alarm_args *, register_t *);

int	linux_sys_pause(struct lwp *, const void *, register_t *);

int	linux_sys_utime(struct lwp *, const struct linux_sys_utime_args *, register_t *);

int	sys_access(struct lwp *, const struct sys_access_args *, register_t *);

int	linux_sys_nice(struct lwp *, const struct linux_sys_nice_args *, register_t *);

int	sys_sync(struct lwp *, const void *, register_t *);

int	linux_sys_kill(struct lwp *, const struct linux_sys_kill_args *, register_t *);

int	sys___posix_rename(struct lwp *, const struct sys___posix_rename_args *, register_t *);

int	sys_mkdir(struct lwp *, const struct sys_mkdir_args *, register_t *);

int	sys_rmdir(struct lwp *, const struct sys_rmdir_args *, register_t *);

int	sys_dup(struct lwp *, const struct sys_dup_args *, register_t *);

int	linux_sys_pipe(struct lwp *, const struct linux_sys_pipe_args *, register_t *);

int	linux_sys_times(struct lwp *, const struct linux_sys_times_args *, register_t *);

int	linux_sys_brk(struct lwp *, const struct linux_sys_brk_args *, register_t *);

int	sys_setgid(struct lwp *, const struct sys_setgid_args *, register_t *);

int	sys_getgid(struct lwp *, const void *, register_t *);

int	linux_sys_signal(struct lwp *, const struct linux_sys_signal_args *, register_t *);

int	sys_geteuid(struct lwp *, const void *, register_t *);

int	sys_getegid(struct lwp *, const void *, register_t *);

int	sys_acct(struct lwp *, const struct sys_acct_args *, register_t *);

int	linux_sys_ioctl(struct lwp *, const struct linux_sys_ioctl_args *, register_t *);

int	linux_sys_fcntl(struct lwp *, const struct linux_sys_fcntl_args *, register_t *);

int	sys_setpgid(struct lwp *, const struct sys_setpgid_args *, register_t *);

int	linux_sys_oldolduname(struct lwp *, const struct linux_sys_oldolduname_args *, register_t *);

int	sys_umask(struct lwp *, const struct sys_umask_args *, register_t *);

int	sys_chroot(struct lwp *, const struct sys_chroot_args *, register_t *);

int	sys_dup2(struct lwp *, const struct sys_dup2_args *, register_t *);

int	sys_getppid(struct lwp *, const void *, register_t *);

int	sys_getpgrp(struct lwp *, const void *, register_t *);

int	sys_setsid(struct lwp *, const void *, register_t *);

int	linux_sys_sigaction(struct lwp *, const struct linux_sys_sigaction_args *, register_t *);

int	linux_sys_siggetmask(struct lwp *, const void *, register_t *);

int	linux_sys_sigsetmask(struct lwp *, const struct linux_sys_sigsetmask_args *, register_t *);

int	linux_sys_setreuid16(struct lwp *, const struct linux_sys_setreuid16_args *, register_t *);

int	linux_sys_setregid16(struct lwp *, const struct linux_sys_setregid16_args *, register_t *);

int	linux_sys_sigsuspend(struct lwp *, const struct linux_sys_sigsuspend_args *, register_t *);

int	linux_sys_sigpending(struct lwp *, const struct linux_sys_sigpending_args *, register_t *);

int	compat_43_sys_sethostname(struct lwp *, const struct compat_43_sys_sethostname_args *, register_t *);

int	linux_sys_setrlimit(struct lwp *, const struct linux_sys_setrlimit_args *, register_t *);

int	linux_sys_getrlimit(struct lwp *, const struct linux_sys_getrlimit_args *, register_t *);

int	sys_getrusage(struct lwp *, const struct sys_getrusage_args *, register_t *);

int	linux_sys_gettimeofday(struct lwp *, const struct linux_sys_gettimeofday_args *, register_t *);

int	linux_sys_settimeofday(struct lwp *, const struct linux_sys_settimeofday_args *, register_t *);

int	linux_sys_getgroups16(struct lwp *, const struct linux_sys_getgroups16_args *, register_t *);

int	linux_sys_setgroups16(struct lwp *, const struct linux_sys_setgroups16_args *, register_t *);

int	linux_sys_oldselect(struct lwp *, const struct linux_sys_oldselect_args *, register_t *);

int	sys_symlink(struct lwp *, const struct sys_symlink_args *, register_t *);

int	compat_43_sys_lstat(struct lwp *, const struct compat_43_sys_lstat_args *, register_t *);

int	sys_readlink(struct lwp *, const struct sys_readlink_args *, register_t *);

#ifdef EXEC_AOUT
int	linux_sys_uselib(struct lwp *, const struct linux_sys_uselib_args *, register_t *);

#else
#endif
int	linux_sys_swapon(struct lwp *, const struct linux_sys_swapon_args *, register_t *);

int	linux_sys_reboot(struct lwp *, const struct linux_sys_reboot_args *, register_t *);

int	linux_sys_readdir(struct lwp *, const struct linux_sys_readdir_args *, register_t *);

int	linux_sys_old_mmap(struct lwp *, const struct linux_sys_old_mmap_args *, register_t *);

int	sys_munmap(struct lwp *, const struct sys_munmap_args *, register_t *);

int	compat_43_sys_truncate(struct lwp *, const struct compat_43_sys_truncate_args *, register_t *);

int	compat_43_sys_ftruncate(struct lwp *, const struct compat_43_sys_ftruncate_args *, register_t *);

int	sys_fchmod(struct lwp *, const struct sys_fchmod_args *, register_t *);

int	linux_sys_fchown16(struct lwp *, const struct linux_sys_fchown16_args *, register_t *);

int	linux_sys_getpriority(struct lwp *, const struct linux_sys_getpriority_args *, register_t *);

int	sys_setpriority(struct lwp *, const struct sys_setpriority_args *, register_t *);

int	sys_profil(struct lwp *, const struct sys_profil_args *, register_t *);

int	linux_sys_statfs(struct lwp *, const struct linux_sys_statfs_args *, register_t *);

int	linux_sys_fstatfs(struct lwp *, const struct linux_sys_fstatfs_args *, register_t *);

int	linux_sys_socketcall(struct lwp *, const struct linux_sys_socketcall_args *, register_t *);

int	sys_setitimer(struct lwp *, const struct sys_setitimer_args *, register_t *);

int	sys_getitimer(struct lwp *, const struct sys_getitimer_args *, register_t *);

int	linux_sys_stat(struct lwp *, const struct linux_sys_stat_args *, register_t *);

int	linux_sys_lstat(struct lwp *, const struct linux_sys_lstat_args *, register_t *);

int	linux_sys_fstat(struct lwp *, const struct linux_sys_fstat_args *, register_t *);

int	linux_sys_olduname(struct lwp *, const struct linux_sys_olduname_args *, register_t *);

int	linux_sys_wait4(struct lwp *, const struct linux_sys_wait4_args *, register_t *);

int	linux_sys_swapoff(struct lwp *, const struct linux_sys_swapoff_args *, register_t *);

int	linux_sys_sysinfo(struct lwp *, const struct linux_sys_sysinfo_args *, register_t *);

int	linux_sys_ipc(struct lwp *, const struct linux_sys_ipc_args *, register_t *);

int	sys_fsync(struct lwp *, const struct sys_fsync_args *, register_t *);

int	linux_sys_sigreturn(struct lwp *, const struct linux_sys_sigreturn_args *, register_t *);

int	linux_sys_clone(struct lwp *, const struct linux_sys_clone_args *, register_t *);

int	linux_sys_setdomainname(struct lwp *, const struct linux_sys_setdomainname_args *, register_t *);

int	linux_sys_uname(struct lwp *, const struct linux_sys_uname_args *, register_t *);

int	linux_sys_mprotect(struct lwp *, const struct linux_sys_mprotect_args *, register_t *);

int	linux_sys_sigprocmask(struct lwp *, const struct linux_sys_sigprocmask_args *, register_t *);

int	sys_getpgid(struct lwp *, const struct sys_getpgid_args *, register_t *);

int	sys_fchdir(struct lwp *, const struct sys_fchdir_args *, register_t *);

int	linux_sys_personality(struct lwp *, const struct linux_sys_personality_args *, register_t *);

int	linux_sys_setfsuid(struct lwp *, const struct linux_sys_setfsuid_args *, register_t *);

int	linux_sys_setfsgid(struct lwp *, const struct linux_sys_setfsgid_args *, register_t *);

int	linux_sys_llseek(struct lwp *, const struct linux_sys_llseek_args *, register_t *);

int	linux_sys_getdents(struct lwp *, const struct linux_sys_getdents_args *, register_t *);

int	linux_sys_select(struct lwp *, const struct linux_sys_select_args *, register_t *);

int	sys_flock(struct lwp *, const struct sys_flock_args *, register_t *);

int	linux_sys_msync(struct lwp *, const struct linux_sys_msync_args *, register_t *);

int	sys_readv(struct lwp *, const struct sys_readv_args *, register_t *);

int	sys_writev(struct lwp *, const struct sys_writev_args *, register_t *);

int	sys_getsid(struct lwp *, const struct sys_getsid_args *, register_t *);

int	linux_sys_fdatasync(struct lwp *, const struct linux_sys_fdatasync_args *, register_t *);

int	linux_sys___sysctl(struct lwp *, const struct linux_sys___sysctl_args *, register_t *);

int	sys_mlock(struct lwp *, const struct sys_mlock_args *, register_t *);

int	sys_munlock(struct lwp *, const struct sys_munlock_args *, register_t *);

int	sys_mlockall(struct lwp *, const struct sys_mlockall_args *, register_t *);

int	sys_munlockall(struct lwp *, const void *, register_t *);

int	linux_sys_sched_setparam(struct lwp *, const struct linux_sys_sched_setparam_args *, register_t *);

int	linux_sys_sched_getparam(struct lwp *, const struct linux_sys_sched_getparam_args *, register_t *);

int	linux_sys_sched_setscheduler(struct lwp *, const struct linux_sys_sched_setscheduler_args *, register_t *);

int	linux_sys_sched_getscheduler(struct lwp *, const struct linux_sys_sched_getscheduler_args *, register_t *);

int	linux_sys_sched_yield(struct lwp *, const void *, register_t *);

int	linux_sys_sched_get_priority_max(struct lwp *, const struct linux_sys_sched_get_priority_max_args *, register_t *);

int	linux_sys_sched_get_priority_min(struct lwp *, const struct linux_sys_sched_get_priority_min_args *, register_t *);

int	sys_nanosleep(struct lwp *, const struct sys_nanosleep_args *, register_t *);

int	linux_sys_mremap(struct lwp *, const struct linux_sys_mremap_args *, register_t *);

int	linux_sys_setresuid16(struct lwp *, const struct linux_sys_setresuid16_args *, register_t *);

int	linux_sys_getresuid16(struct lwp *, const struct linux_sys_getresuid16_args *, register_t *);

int	sys_poll(struct lwp *, const struct sys_poll_args *, register_t *);

int	linux_sys_setresgid16(struct lwp *, const struct linux_sys_setresgid16_args *, register_t *);

int	linux_sys_getresgid16(struct lwp *, const struct linux_sys_getresgid16_args *, register_t *);

int	linux_sys_rt_sigaction(struct lwp *, const struct linux_sys_rt_sigaction_args *, register_t *);

int	linux_sys_rt_sigprocmask(struct lwp *, const struct linux_sys_rt_sigprocmask_args *, register_t *);

int	linux_sys_rt_sigpending(struct lwp *, const struct linux_sys_rt_sigpending_args *, register_t *);

int	linux_sys_rt_queueinfo(struct lwp *, const struct linux_sys_rt_queueinfo_args *, register_t *);

int	linux_sys_rt_sigsuspend(struct lwp *, const struct linux_sys_rt_sigsuspend_args *, register_t *);

int	linux_sys_pread(struct lwp *, const struct linux_sys_pread_args *, register_t *);

int	linux_sys_pwrite(struct lwp *, const struct linux_sys_pwrite_args *, register_t *);

int	linux_sys_chown16(struct lwp *, const struct linux_sys_chown16_args *, register_t *);

int	sys___getcwd(struct lwp *, const struct sys___getcwd_args *, register_t *);

int	linux_sys_sigaltstack(struct lwp *, const struct linux_sys_sigaltstack_args *, register_t *);

int	sys___vfork14(struct lwp *, const void *, register_t *);

int	linux_sys_ugetrlimit(struct lwp *, const struct linux_sys_ugetrlimit_args *, register_t *);

#define linux_sys_mmap2_args linux_sys_mmap_args
int	linux_sys_mmap2(struct lwp *, const struct linux_sys_mmap2_args *, register_t *);

int	linux_sys_truncate64(struct lwp *, const struct linux_sys_truncate64_args *, register_t *);

int	linux_sys_ftruncate64(struct lwp *, const struct linux_sys_ftruncate64_args *, register_t *);

int	linux_sys_stat64(struct lwp *, const struct linux_sys_stat64_args *, register_t *);

int	linux_sys_lstat64(struct lwp *, const struct linux_sys_lstat64_args *, register_t *);

int	linux_sys_fstat64(struct lwp *, const struct linux_sys_fstat64_args *, register_t *);

int	sys___posix_lchown(struct lwp *, const struct sys___posix_lchown_args *, register_t *);

int	sys_setreuid(struct lwp *, const struct sys_setreuid_args *, register_t *);

int	sys_setregid(struct lwp *, const struct sys_setregid_args *, register_t *);

int	sys_getgroups(struct lwp *, const struct sys_getgroups_args *, register_t *);

int	sys_setgroups(struct lwp *, const struct sys_setgroups_args *, register_t *);

int	sys___posix_fchown(struct lwp *, const struct sys___posix_fchown_args *, register_t *);

int	linux_sys_setresuid(struct lwp *, const struct linux_sys_setresuid_args *, register_t *);

int	linux_sys_getresuid(struct lwp *, const struct linux_sys_getresuid_args *, register_t *);

int	linux_sys_setresgid(struct lwp *, const struct linux_sys_setresgid_args *, register_t *);

int	linux_sys_getresgid(struct lwp *, const struct linux_sys_getresgid_args *, register_t *);

int	sys___posix_chown(struct lwp *, const struct sys___posix_chown_args *, register_t *);

int	linux_sys_getdents64(struct lwp *, const struct linux_sys_getdents64_args *, register_t *);

int	sys_mincore(struct lwp *, const struct sys_mincore_args *, register_t *);

int	sys_madvise(struct lwp *, const struct sys_madvise_args *, register_t *);

int	linux_sys_fcntl64(struct lwp *, const struct linux_sys_fcntl64_args *, register_t *);

int	linux_sys_setxattr(struct lwp *, const struct linux_sys_setxattr_args *, register_t *);

int	linux_sys_lsetxattr(struct lwp *, const struct linux_sys_lsetxattr_args *, register_t *);

int	linux_sys_fsetxattr(struct lwp *, const struct linux_sys_fsetxattr_args *, register_t *);

int	linux_sys_getxattr(struct lwp *, const struct linux_sys_getxattr_args *, register_t *);

int	linux_sys_lgetxattr(struct lwp *, const struct linux_sys_lgetxattr_args *, register_t *);

int	linux_sys_fgetxattr(struct lwp *, const struct linux_sys_fgetxattr_args *, register_t *);

int	linux_sys_listxattr(struct lwp *, const struct linux_sys_listxattr_args *, register_t *);

int	linux_sys_llistxattr(struct lwp *, const struct linux_sys_llistxattr_args *, register_t *);

int	linux_sys_flistxattr(struct lwp *, const struct linux_sys_flistxattr_args *, register_t *);

int	linux_sys_removexattr(struct lwp *, const struct linux_sys_removexattr_args *, register_t *);

int	linux_sys_lremovexattr(struct lwp *, const struct linux_sys_lremovexattr_args *, register_t *);

int	linux_sys_fremovexattr(struct lwp *, const struct linux_sys_fremovexattr_args *, register_t *);

int	linux_sys_exit_group(struct lwp *, const struct linux_sys_exit_group_args *, register_t *);

int	linux_sys_clock_settime(struct lwp *, const struct linux_sys_clock_settime_args *, register_t *);

int	linux_sys_clock_gettime(struct lwp *, const struct linux_sys_clock_gettime_args *, register_t *);

int	linux_sys_clock_getres(struct lwp *, const struct linux_sys_clock_getres_args *, register_t *);

int	linux_sys_clock_nanosleep(struct lwp *, const struct linux_sys_clock_nanosleep_args *, register_t *);

int	linux_sys_statfs64(struct lwp *, const struct linux_sys_statfs64_args *, register_t *);

int	linux_sys_fstatfs64(struct lwp *, const struct linux_sys_fstatfs64_args *, register_t *);

int	linux_sys_breakpoint(struct lwp *, const void *, register_t *);

int	linux_sys_cacheflush(struct lwp *, const struct linux_sys_cacheflush_args *, register_t *);

#endif /* _LINUX_SYS_SYSCALLARGS_H_ */
