/*	$NetBSD: intr.h,v 1.23 2008/04/28 20:23:30 martin Exp $	*/

/*-
 * Copyright (c) 2000, 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MACHINE_INTR_H_
#define _MACHINE_INTR_H_

#define IPL_NONE	0	/* disable only this interrupt */
#define IPL_SOFTCLOCK	1	/* clock software interrupts (SI 0) */
#define IPL_SOFTBIO	1	/* bio software interrupts (SI 0) */
#define IPL_SOFTNET	2	/* network software interrupts (SI 1) */
#define IPL_SOFTSERIAL	2	/* serial software interrupts (SI 1) */
#define	IPL_VM		3
#define IPL_SCHED	4	/* disable clock interrupts */
#define IPL_HIGH	4	/* disable all interrupts */

#define _IPL_N		5

#define _IPL_SI0_FIRST	IPL_SOFTCLOCK
#define _IPL_SI0_LAST	IPL_SOFTBIO

#define _IPL_SI1_FIRST	IPL_SOFTNET
#define _IPL_SI1_LAST	IPL_SOFTSERIAL

#ifdef _KERNEL
#ifndef _LOCORE

#include <sys/device.h>
#include <mips/locore.h>

extern const uint32_t ipl_sr_bits[_IPL_N];

#define spl0()		(void)_spllower(0)
#define splx(s)		(void)_splset(s)

#define splsoft()	_splraise(ipl_sr_bits[IPL_SOFT])

typedef int ipl_t;
typedef struct {
	ipl_t _sr;
} ipl_cookie_t;

static inline ipl_cookie_t
makeiplcookie(ipl_t ipl)
{

	return (ipl_cookie_t){._sr = ipl_sr_bits[ipl]};
}

static inline int
splraiseipl(ipl_cookie_t icookie)
{

	return _splraise(icookie._sr);
}

#include <sys/spl.h>

struct newsmips_intrhand {
	LIST_ENTRY(newsmips_intrhand) ih_q;
	struct evcnt intr_count;
	int (*ih_func)(void *);
	void *ih_arg;
	u_int ih_level;
	u_int ih_mask;
	u_int ih_priority;
};

struct newsmips_intr {
	LIST_HEAD(,newsmips_intrhand) intr_q;
};

/*
 * Index into intrcnt[], which is defined in locore
 */
#define SERIAL0_INTR	0
#define SERIAL1_INTR	1
#define SERIAL2_INTR	2
#define LANCE_INTR	3
#define SCSI_INTR	4
#define ERROR_INTR	5
#define HARDCLOCK_INTR	6
#define FPU_INTR	7
#define SLOT1_INTR	8
#define SLOT2_INTR	9
#define SLOT3_INTR	10
#define FLOPPY_INTR	11
#define STRAY_INTR	12

extern u_int intrcnt[];

/* handle i/o device interrupts */
#ifdef news3400
void news3400_intr(uint32_t, uint32_t, uint32_t, uint32_t);
#endif
#ifdef news5000
void news5000_intr(uint32_t, uint32_t, uint32_t, uint32_t);
#endif
extern void (*hardware_intr)(uint32_t, uint32_t, uint32_t, uint32_t);

extern void (*enable_intr)(void);
extern void (*disable_intr)(void);
extern void (*enable_timer)(void);

#endif /* !_LOCORE */
#endif /* _KERNEL */
#endif /* _MACHINE_INTR_H_ */
