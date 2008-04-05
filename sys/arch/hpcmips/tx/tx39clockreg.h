/*	$NetBSD: tx39clockreg.h,v 1.2 2000/09/28 16:18:26 uch Exp $ */

/*-
 * Copyright (c) 1999, 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by UCHIYAMA Yasushi.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

/*
 * Toshiba TX3912/3922 Clock module
 */

#define TX39_CLOCKCTRL_REG		0x1c0

/*
 *	Clock Control Register
 */
/* R/W */
#define TX39_CLOCK_CHICLKDIV_SHIFT	24
#define TX39_CLOCK_CHICLKDIV_MASK	0xff
#define TX39_CLOCK_CHICLKDIV(cr)				\
	(((cr) >> TX39_CLOCK_CHICLKDIV_SHIFT) &			\
	TX39_CLOCK_CHICLKDIV_MASK)
#define TX39_CLOCK_CHICLKDIV_SET(cr, val)			\
	((cr) | (((val) << TX39_CLOCK_CHICLKDIV_SHIFT) &	\
	(TX39_CLOCK_CHICLKDIV_MASK << TX39_CLOCK_CHICLKDIV_SHIFT)))

#define TX39_CLOCK_ENCLKTEST		0x00800000
#define TX39_CLOCK_CCLKTESTSELSIB	0x00400000
#define TX39_CLOCK_CHIMCLKSEL		0x00200000
#define TX39_CLOCK_CHICLKDIR		0x00100000
#define TX39_CLOCK_ENCHIMCLK		0x00080000
#ifdef TX391X
#define TX39_CLOCK_ENVIDCLK		0x00040000
#define TX39_CLOCK_ENMBUSCLK		0x00020000
#endif /* TX391X */
#ifdef TX392X
#define TX39_CLOCK_SPICLKDIR		0x00040000
#define TX39_CLOCK_ENIRDACLK		0x00020000
#endif /* TX392X */
#define TX39_CLOCK_ENSPICLK		0x00010000
#define TX39_CLOCK_ENTIMERCLK		0x00008000
#define TX39_CLOCK_ENFASTTIMERCLK	0x00004000
#define TX39_CLOCK_SIBCLKDIR		0x00002000
#ifdef TX392X
#define TX39_CLOCK_ENC48MOUTCLK		0x00001000
#endif /* TX392X */
#define TX39_CLOCK_ENSIBMCLK		0x00000800

#define TX39_CLOCK_SIBMCLKDIV_SHIFT	8
#define TX39_CLOCK_SIBMCLKDIV_MASK	0x7
#define TX39_CLOCK_SIBMCLKDIV(cr)				\
	(((cr) >> TX39_CLOCK_SIBMCLKDIV_SHIFT) &		\
	TX39_CLOCK_SIBMCLKDIV_MASK)
#define TX39_CLOCK_SIBMCLKDIV_SET(cr, val)			\
	((cr) | (((val) << TX39_CLOCK_SIBMCLKDIV_SHIFT) &	\
	(TX39_CLOCK_SIBMCLKDIV_MASK << TX39_CLOCK_SIBMCLKDIV_SHIFT)))

#define TX39_CLOCK_CSERSEL	    0x00000080

#define TX39_CLOCK_CSERDIV_SHIFT	4
#define TX39_CLOCK_CSERDIV_MASK		0x7
#define TX39_CLOCK_CSERDIV(cr)					\
	(((cr) >> TX39_CLOCK_CSERDIV_SHIFT) &			\
	TX39_CLOCK_CSERDIV_MASK)
#define TX39_CLOCK_CSERDIV_SET(cr, val)				\
	((cr) | (((val) << TX39_CLOCK_CSERDIV_SHIFT) &		\
	(TX39_CLOCK_CSERDIV_MASK << TX39_CLOCK_CSERDIV_SHIFT)))

#define TX39_CLOCK_ENCSERCLK		0x00000008
#define TX39_CLOCK_ENIRCLK		0x00000004
#define TX39_CLOCK_ENUARTACLK		0x00000002
#define TX39_CLOCK_ENUARTBCLK		0x00000001
