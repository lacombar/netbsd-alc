/*	$NetBSD: sysarch.h,v 1.6 2005/12/11 12:16:47 christos Exp $	*/

/*
 * Copyright (c) 1996-1997 Mark Brinicombe.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Mark Brinicombe.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _ARM_SYSARCH_H_
#define _ARM_SYSARCH_H_

#include <sys/cdefs.h>

/*
 * Pickup definition of uintptr_t
 */
#include <sys/stdint.h>

/*
 * Architecture specific syscalls (arm)
 */

#define ARM_SYNC_ICACHE		0
#define ARM_DRAIN_WRITEBUF	1

struct arm_sync_icache_args {
	uintptr_t	addr;		/* Virtual start address */
	size_t		len;		/* Region size */
};

#ifndef _KERNEL
__BEGIN_DECLS
int	arm_sync_icache __P((u_int addr, int len));
int	arm_drain_writebuf __P((void));
int	sysarch __P((int, void *));
__END_DECLS
#endif

#endif /* !_ARM_SYSARCH_H_ */
