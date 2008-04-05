/*	$NetBSD: zxvar.h,v 1.1 2002/09/13 14:03:53 ad Exp $	*/

/*
 *  Copyright (c) 2002 The NetBSD Foundation, Inc.
 *  All rights reserved.
 *
 *  This code is derived from software contributed to The NetBSD Foundation
 *  by Andrew Doran.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. All advertising materials mentioning features or use of this software
 *     must display the following acknowledgement:
 *         This product includes software developed by the NetBSD
 *         Foundation, Inc. and its contributors.
 *  4. Neither the name of The NetBSD Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 *  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (C) 1999, 2000 Jakub Jelinek (jakub@redhat.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * JAKUB JELINEK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _DEV_SBUS_ZXVAR_H_
#define _DEV_SBUS_ZXVAR_H_

/*
 * Sun (and Linux) compatible offsets for mmap().
 */
#define ZX_FB0_VOFF		0x00000000
#define ZX_LC0_VOFF		0x00800000
#define ZX_LD0_VOFF		0x00801000
#define ZX_LX0_CURSOR_VOFF	0x00802000
#define ZX_FB1_VOFF		0x00803000
#define ZX_LC1_VOFF		0x01003000
#define ZX_LD1_VOFF		0x01004000
#define ZX_LX0_VERT_VOFF	0x01005000
#define ZX_LX_KRN_VOFF		0x01006000
#define ZX_LC0_KRN_VOFF		0x01007000
#define ZX_LC1_KRN_VOFF		0x01008000
#define ZX_LD_GBL_VOFF		0x01009000

#define	ZX_WID_SHARED_8	0
#define	ZX_WID_SHARED_24	1
#define	ZX_WID_DBL_8		2
#define	ZX_WID_DBL_24		3

/*
 * Per-instance data.
 */
struct zx_softc {
	struct device	sc_dv;
	struct sbusdev	sc_sd;
	struct fbdevice	sc_fb;
	bus_space_tag_t	sc_bt;

	int		sc_flags;
	int		sc_fontw;
	int		sc_fonth;
	u_int8_t	*sc_cmap;
	u_int32_t	*sc_pixels;
	bus_addr_t	sc_paddr;
	int		sc_shiftx;
	int		sc_shifty;

	struct fbcurpos	sc_curpos;
	struct fbcurpos	sc_curhot;
	struct fbcurpos sc_cursize;
	u_int8_t	sc_curcmap[8];
	u_int32_t	sc_curbits[2][32];

	volatile struct zx_command *sc_zc;
	volatile struct zx_cross *sc_zx;
	volatile struct zx_draw *sc_zd_ss0;
	volatile struct zx_draw_ss1 *sc_zd_ss1;
	volatile struct zx_cursor *sc_zcu;
};
#define	ZX_BLANKED	0x01
#define	ZX_CURSOR	0x02

#endif	/* !_DEV_SBUS_ZXVAR_H_ */
