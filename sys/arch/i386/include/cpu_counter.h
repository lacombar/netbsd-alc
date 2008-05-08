/*	$NetBSD: cpu_counter.h,v 1.8 2008/04/28 20:23:24 martin Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Bill Sommerfeld.
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

#ifndef _I386_CPU_COUNTER_H_
#define _I386_CPU_COUNTER_H_

/*
 * Machine-specific support for CPU counter.
 */

#include <machine/cpu.h>
#include <machine/cpufunc.h>
#include <machine/specialreg.h>

#ifdef _KERNEL

#include <x86/cpu_counter.h>

static __inline int
cpu_hascounter(void)
{

	/*
	 * Note that:
	 * 1) Intel documentation is very specific that code *must* test
	 * the CPU feature flag, even if you "know" that a particular
	 * rev of the hardware supports it.
	 * 2) We know that the TSC is busted on some Cyrix CPU in that if
	 * you execute "hlt" when in powersave mode, TSC stops counting,
	 * even though the CPU clock crystal is still ticking (it always has to).
	 */
	return (cpu_feature & CPUID_TSC) != 0;
}

#endif /* _KERNEL */

#endif /* !_I386_CPU_COUNTER_H_ */
