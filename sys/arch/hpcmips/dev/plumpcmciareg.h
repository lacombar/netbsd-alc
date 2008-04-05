/*	$NetBSD: plumpcmciareg.h,v 1.4 2001/09/15 12:47:07 uch Exp $ */

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

/* (CS3) */
#define	PLUM_PCMCIA_REGBASE		0x5000
#define	PLUM_PCMCIA_REGSIZE		0x1000

/* (MCS0) */
/* 1MByte */
#define PLUM_PCMCIA_IOBASE1		0x00600000
#define PLUM_PCMCIA_IOSIZE1		0x00100000
/* 1MByte */
#define PLUM_PCMCIA_IOBASE2		0x00700000
#define PLUM_PCMCIA_IOSIZE2		0x00100000
/* 8Mbyte */
#define PLUM_PCMCIA_MEMBASE1		0x00800000
#define PLUM_PCMCIA_MEMSIZE1		0x00800000
/* 16MByte */
#define PLUM_PCMCIA_MEMBASE2		0x01000000
#define PLUM_PCMCIA_MEMSIZE2		0x01000000
/* 32MByte */
#define PLUM_PCMCIA_MEMBASE3		0x02000000
#define PLUM_PCMCIA_MEMSIZE3		0x02000000
 /* (MCS1) */
/* 64MByte */
#define PLUM_PCMCIA_MEMBASE4		0x04000000
#define PLUM_PCMCIA_MEMSIZE4		0x04000000

/*
 * # of slots
 */
#define PLUMPCMCIA_NSLOTS 2
/*
 * Control registers.
 */
#define PLUM_PCMCIA_REGSPACE_SLOT0	0
#define PLUM_PCMCIA_REGSPACE_SLOT1	0x800
#define PLUM_PCMCIA_REGSPACE_SIZE	0x800

#define PLUM_PCMCIA_IDENT		0x000
#define PLUM_PCMCIA_STATUS		0x004
#define PLUM_PCMCIA_STATUS_BVD1				0x01
#define PLUM_PCMCIA_STATUS_BVD2				0x02
#define PLUM_PCMCIA_STATUS_CD1				0x04
#define PLUM_PCMCIA_STATUS_CD2				0x08
#define PLUM_PCMCIA_STATUS_WRITEPROTECT			0x10
/* really READY/!BUSY */
#define	PLUM_PCMCIA_STATUS_READY			0x20
#define PLUM_PCMCIA_STATUS_PWROK			0x40

#define PLUM_PCMCIA_PWRCTRL		0x008
/* output enable */
#define	PLUM_PCMCIA_PWRCTRL_OE				0x80
#define	PLUM_PCMCIA_PWRCTRL_DISABLE_RESETDRV		0x40
#define	PLUM_PCMCIA_PWRCTRL_AUTOSWITCH_ENABLE		0x20
#define	PLUM_PCMCIA_PWRCTRL_PWR_ENABLE			0x10
#define PLUM_PCMCIA_PWRCTRL_VCC_CTRLBIT1		0x08
#define PLUM_PCMCIA_PWRCTRL_VCC_CTRLBIT0		0x04
#define PLUM_PCMCIA_PWRCTRL_VPP1_CTRLBIT1		0x02
#define PLUM_PCMCIA_PWRCTRL_VPP1_CTRLBIT0		0x01

#define PLUM_PCMCIA_GENCTRL				0x00c
/* active low (zero) */
#define	PLUM_PCMCIA_GENCTRL_RESET			0x40
#define	PLUM_PCMCIA_GENCTRL_CARDTYPE_MASK		0x20
#define	PLUM_PCMCIA_GENCTRL_CARDTYPE_IO			0x20
#define	PLUM_PCMCIA_GENCTRL_CARDTYPE_MEM		0x00

#define PLUM_PCMCIA_CSCINT_STAT				0x010
#define PLUM_PCMCIA_CSCINT				0x014
#define PLUM_PCMCIA_CSCINT_BATTERY_DEAD			0x01
#define PLUM_PCMCIA_CSCINT_BATTERY_WARNING		0x02
#define PLUM_PCMCIA_CSCINT_READY			0x04
#define PLUM_PCMCIA_CSCINT_CARD_DETECT			0x08

#define PLUM_PCMCIA_WINEN				0x018
#define PLUM_PCMCIA_WINEN_IO1				0x00000080
#define PLUM_PCMCIA_WINEN_IO0				0x00000040
#define PLUM_PCMCIA_WINEN_MEM4				0x00000010
#define PLUM_PCMCIA_WINEN_MEM3				0x00000008
#define PLUM_PCMCIA_WINEN_MEM2				0x00000004
#define PLUM_PCMCIA_WINEN_MEM1				0x00000002
#define PLUM_PCMCIA_WINEN_MEM0				0x00000001
#define PLUM_PCMCIA_WINEN_MEM(x)	(1 << (x))

#define PLUM_PCMCIA_MEM_WINS				5
#define	PLUM_PCMCIA_MEM_SHIFT				12
#define	PLUM_PCMCIA_MEM_PAGESIZE		(1<<PLUM_PCMCIA_MEM_SHIFT)

#define PLUM_PCMCIA_IO_WINS				2

#define PLUM_PCMCIA_IOWINCTRL				0x01c
#define PLUM_PCMCIA_IOWINCTRL_WINMASK			0x0000000f
#define PLUM_PCMCIA_IOWINCTRL_WIN0SHIFT			0
#define PLUM_PCMCIA_IOWINCTRL_WIN1SHIFT			4
#define PLUM_PCMCIA_IOWINCTRL_DATASIZE16		0x00000001
#define PLUM_PCMCIA_IOWINCTRL_IOCS16SRC			0x00000002
#define PLUM_PCMCIA_IOWINCTRL_ZEROWAIT			0x00000004
#define PLUM_PCMCIA_IOWINCTRL_WAITSTATE			0x00000008

#define PLUM_PCMCIA_IOWIN0STARTADDR	0x020
#define PLUM_PCMCIA_IOWIN1STARTADDR	0x030
#define PLUM_PCMCIA_IOWINSTARTADDR(x)					\
	((x) * 0x10 + PLUM_PCMCIA_IOWIN0STARTADDR)

#define PLUM_PCMCIA_IOWIN0STOPADDR	0x024
#define PLUM_PCMCIA_IOWIN1STOPADDR	0x034
#define PLUM_PCMCIA_IOWINSTOPADDR(x)					\
	((x) * 0x10 + PLUM_PCMCIA_IOWIN0STOPADDR)

#define PLUM_PCMCIA_IOWIN0ADDRCTRL	0x028
#define PLUM_PCMCIA_IOWIN1ADDRCTRL	0x038
#define PLUM_PCMCIA_IOWINADDRCTRL(x)					\
	((x) * 0x10 + PLUM_PCMCIA_IOWIN0ADDRCTRL)

#define PLUM_PCMCIA_IOWINADDRCTRL_AREA1	0x00000000
#define PLUM_PCMCIA_IOWINADDRCTRL_AREA2	0x00000001

#define PLUM_PCMCIA_MEMWIN0STARTADDR	0x040
#define PLUM_PCMCIA_MEMWIN1STARTADDR	0x060
#define PLUM_PCMCIA_MEMWIN2STARTADDR	0x080
#define PLUM_PCMCIA_MEMWIN3STARTADDR	0x0a0
#define PLUM_PCMCIA_MEMWIN4STARTADDR	0x0c0
#define PLUM_PCMCIA_MEMWINSTARTADDR(x)					\
	((x) * 0x20 + PLUM_PCMCIA_MEMWIN0STARTADDR)

#define PLUM_PCMCIA_MEMWIN0STOPADDR	0x044
#define PLUM_PCMCIA_MEMWIN1STOPADDR	0x064
#define PLUM_PCMCIA_MEMWIN2STOPADDR	0x084
#define PLUM_PCMCIA_MEMWIN3STOPADDR	0x0a4
#define PLUM_PCMCIA_MEMWIN4STOPADDR	0x0c4
#define PLUM_PCMCIA_MEMWINSTOPADDR(x)					\
	((x) * 0x20 + PLUM_PCMCIA_MEMWIN0STOPADDR)

#define PLUM_PCMCIA_MEMWIN0OFSADDR	0x048
#define PLUM_PCMCIA_MEMWIN1OFSADDR	0x068
#define PLUM_PCMCIA_MEMWIN2OFSADDR	0x088
#define PLUM_PCMCIA_MEMWIN3OFSADDR	0x0a8
#define PLUM_PCMCIA_MEMWIN4OFSADDR	0x0c8
#define PLUM_PCMCIA_MEMWINOFSADDR(x)					\
	((x) * 0x20 + PLUM_PCMCIA_MEMWIN0OFSADDR)

#define PLUM_PCMCIA_MEMWIN0CTRL		0x04c
#define PLUM_PCMCIA_MEMWIN1CTRL		0x06c
#define PLUM_PCMCIA_MEMWIN2CTRL		0x08c
#define PLUM_PCMCIA_MEMWIN3CTRL		0x0ac
#define PLUM_PCMCIA_MEMWIN4CTRL		0x0cc
#define PLUM_PCMCIA_MEMWINCTRL(x)					\
	((x) * 0x20 + PLUM_PCMCIA_MEMWIN0CTRL)

#define PLUM_PCMCIA_MEMWINCTRL_MAP_SHIFT	24
#define PLUM_PCMCIA_MEMWINCTRL_MAP_MASK		0x3
#define PLUM_PCMCIA_MEMWINCTRL_MAP(cr)					\
	(((cr) >> PLUM_PCMCIA_MEMWINCTRL_MAP_SHIFT) &			\
	PLUM_PCMCIA_MEMWINCTRL_MAP_MASK)
#define PLUM_PCMCIA_MEMWINCTRL_MAP_SET(cr, val)				\
	((cr) | (((val) << PLUM_PCMCIA_MEMWINCTRL_MAP_SHIFT) &		\
	(PLUM_PCMCIA_MEMWINCTRL_MAP_MASK << PLUM_PCMCIA_MEMWINCTRL_MAP_SHIFT)))
#define PLUM_PCMCIA_MEMWINCTRL_MAP_CLEAR(cr)				\
	((cr) &= ~(PLUM_PCMCIA_MEMWINCTRL_MAP_MASK << PLUM_PCMCIA_MEMWINCTRL_MAP_SHIFT))
#define PLUM_PCMCIA_MEMWINCTRL_MAP_AREA1	0x0
#define PLUM_PCMCIA_MEMWINCTRL_MAP_AREA2	0x1
#define PLUM_PCMCIA_MEMWINCTRL_MAP_AREA3	0x2
#define PLUM_PCMCIA_MEMWINCTRL_MAP_AREA4	0x3

#define PLUM_PCMCIA_MEMWINCTRL_WRPROTECT	0x00800000
#define PLUM_PCMCIA_MEMWINCTRL_REGACTIVE	0x00400000
#define PLUM_PCMCIA_MEMWINCTRL_DATASIZE16	0x00000080
#define PLUM_PCMCIA_MEMWINCTRL_ZERO_WS		0x00000040

#define PLUM_PCMCIA_MEMWINCTRL_TIMING_SHIFT	14
#define PLUM_PCMCIA_MEMWINCTRL_TIMING_MASK	0x3
#define PLUM_PCMCIA_MEMWINCTRL_TIMING(cr)				\
	(((cr) >> PLUM_PCMCIA_MEMWINCTRL_TIMING_SHIFT) &		\
	PLUM_PCMCIA_MEMWINCTRL_TIMING_MASK)
#define PLUM_PCMCIA_MEMWINCTRL_TIMING_SET(cr, val)			\
	((cr) | (((val) << PLUM_PCMCIA_MEMWINCTRL_TIMING_SHIFT) &	\
	(PLUM_PCMCIA_MEMWINCTRL_TIMING_MASK <<				\
	PLUM_PCMCIA_MEMWINCTRL_TIMING_SHIFT)))
#define PLUM_PCMCIA_MEMWINCTRL_TIMING_STD	0x0
#define PLUM_PCMCIA_MEMWINCTRL_TIMING_1WAIT	0x1
#define PLUM_PCMCIA_MEMWINCTRL_TIMING_2WAIT	0x2
#define PLUM_PCMCIA_MEMWINCTRL_TIMING_3WAIT	0x3

#define PLUM_PCMCIA_MEMWINCTRL_DATASIZE_16BIT	0x00000080 /* else 8bit */
#define PLUM_PCMCIA_MEMWINCTRL_ZEROWS		0x00000040

#define PLUM_PCMCIA_GENCTRL2		0x058
#define PLUM_PCMCIA_GENCTRL2_VCC5V	0x000000c0
#define PLUM_PCMCIA_GENCTRL2_VCC3V	0x00000080

#define PLUM_PCMCIA_GLOBALCTRL		0x078
#define PLUM_PCMCIA_GLOBALCTRL_EXPLICIT_WB_CSC_INT	0x04

#define PLUM_PCMCIA_TIMING		0x0ec

#define PLUM_PCMCIA_FUNCCTRL		0x0f8
#define PLUM_PCMCIA_FUNCCTRL_3VSUPPORT	0x00000001
#define PLUM_PCMCIA_FUNCCTRL_VSSEN	0x00000002

#define PLUM_PCMCIA_SPECIALMODE		0x0fc

#define PLUM_PCMCIA_SLOTCTRL		0x100
#define PLUM_PCMCIA_SLOTCTRL_ENABLE	0x00000080

#define PLUM_PCMCIA_BUFOFF		0x104
#define PLUM_PCMCIA_CARDDETECTMODE	0x108
#define PLUM_PCMCIA_CARDPWRCTRL		0x10c
#define PLUM_PCMCIA_CARDPWRCTRL_OFF	1
#define PLUM_PCMCIA_CARDPWRCTRL_ON	0
