/*	$NetBSD: kern_exit_43.c,v 1.21 2007/12/20 23:02:44 dsl Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)kern_exit.c	8.7 (Berkeley) 2/12/94
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: kern_exit_43.c,v 1.21 2007/12/20 23:02:44 dsl Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/ioctl.h>
#include <sys/proc.h>
#include <sys/tty.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/kernel.h>
#include <sys/buf.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/syslog.h>
#include <sys/resourcevar.h>
#include <sys/ptrace.h>
#include <sys/acct.h>

#include <sys/mount.h>
#include <sys/syscallargs.h>

#include <sys/cpu.h>
#include <machine/reg.h>
#include <compat/common/compat_util.h>

#ifdef m68k
#include <machine/psl.h>		/* only m68k ports use PSL_ALLCC */
#include <machine/frame.h>
#define GETPS(rp)	((struct frame *)(rp))->f_sr
#else
#define GETPS(rp)	(rp)[PS]
#endif

int
compat_43_sys_wait(struct lwp *l, const void *v, register_t *retval)
{
	int error, status, was_zombie;
	int child_pid = WAIT_ANY;


#ifdef PSL_ALLCC
#else
#endif

#ifdef PSL_ALLCC
	if ((GETPS(l->l_md.md_regs) & PSL_ALLCC) != PSL_ALLCC) {
		error = do_sys_wait(l, &child_pid, &status, 0, NULL, &was_zombie);
	} else {
		error = do_sys_wait(l, &child_pid, &status,
		    l->l_md.md_regs[R0], (struct rusage *)l->l_md.md_regs[R1],
		    &was_zombie);
	}
#else
	error = do_sys_wait(l, &child_pid, &status, 0, NULL, &was_zombie);
#endif
	retval[0] = child_pid;
	retval[1] = status;
	return error;
}
