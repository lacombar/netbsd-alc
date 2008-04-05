/*	$NetBSD: ieee.h,v 1.10 2007/02/04 00:39:19 christos Exp $	*/

/*	$OpenBSD: ieee.h,v 1.1 1999/04/20 19:44:04 mickey Exp $	*/

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
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
 *	@(#)ieee.h	8.1 (Berkeley) 6/11/93
 */

/*
 * ieee.h defines the machine-dependent layout of the machine's IEEE
 * floating point.  It does *not* define (yet?) any of the rounding
 * mode bits, exceptions, and so forth.
 */

#include <sys/ieee754.h>

#ifdef _LP64
#define	EXT_EXPBITS	15
#define EXT_FRACHBITS	16
#define	EXT_FRACHMBITS	32
#define	EXT_FRACLMBITS	32
#define	EXT_FRACLBITS	32
#define	EXT_FRACBITS	(EXT_FRACLBITS + EXT_FRACLMBITS + EXT_FRACHMBITS + EXT_FRACHBITS)

#define	EXT_TO_ARRAY32(u, a) do {			\
	(a)[0] = (uint32_t)(u).extu_ext.ext_fracl;	\
	(a)[1] = (uint32_t)(u).extu_ext.ext_fraclm;	\
	(a)[2] = (uint32_t)(u).extu_ext.ext_frachm;	\
	(a)[3] = (uint32_t)(u).extu_ext.ext_frach;	\
} while(/*CONSTCOND*/0)

struct ieee_ext {
	u_int	ext_sign:1;
	u_int	ext_exp:EXT_EXPBITS;
	u_int	ext_frach:EXT_FRACHBITS;
	u_int	ext_frachm;
	u_int	ext_fraclm;
	u_int	ext_fracl;
};

/*
 * Floats whose exponent is in [1..INFNAN) (of whatever type) are
 * `normal'.  Floats whose exponent is INFNAN are either Inf or NaN.
 * Floats whose exponent is zero are either zero (iff all fraction
 * bits are zero) or subnormal values.
 *
 * A NaN is a `signalling NaN' if its QUIETNAN bit is set in its
 * high fraction; if the bit is clear, it is a `quiet NaN'.
 */
#define	EXT_EXP_INFNAN	32767

#if 0
#define	SNG_QUIETNAN	(1 << 22)
#define	DBL_QUIETNAN	(1 << 19)
#define	EXT_QUIETNAN	(1 << 15)
#endif

/*
 * Exponent biases.
 */
#define	EXT_EXP_BIAS	16383

/*
 * Convenience data structures.
 */
union ieee_ext_u {
	long double		extu_ld;
	struct ieee_ext		extu_ext;
};
#endif /* _LP64 */
