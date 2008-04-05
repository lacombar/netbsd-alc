/*	$NetBSD: ucb1200reg.h,v 1.7 2001/09/15 12:47:08 uch Exp $ */

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
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
 * PHILIPS UCB1200 Advanced modem/audio analog front-end
 */

/* Internal register. access via SIB */
#define	UCB1200_IO_DATA_REG		0
#define	UCB1200_IO_DIR_REG		1 
#define	UCB1200_POSINTEN_REG		2 
#define	UCB1200_NEGINTEN_REG		3 
#define	UCB1200_INTSTAT_REG		4 
#define	UCB1200_TELECOMCTRLA_REG	5 
#define	UCB1200_TELECOMCTRLB_REG	6 
#define	UCB1200_AUDIOCTRLA_REG		7 
#define	UCB1200_AUDIOCTRLB_REG		8 
#define	UCB1200_TSCTRL_REG		9 
#define	UCB1200_ADCCTRL_REG		10
#define	UCB1200_ADCDATA_REG		11
#define	UCB1200_ID_REG			12
#define	UCB1200_MODE_REG		13
#define	UCB1200_RESERVED_REG		14
#define	UCB1200_NULL_REG		15 /* always returns 0xffff */

/*
 * I/O port data register
 */
#define UCB1200_IOPORT_MAX		10
#define UCB1200_IO_DATA_SPEAKER		0x100 /* XXX general? */

/*
 * Telecom control register A
 */
#define UCB1200_TELECOMCTRLA_DIV_MIN	16
#define UCB1200_TELECOMCTRLA_DIV_MAX	127
#define UCB1200_TELECOMCTRLA_DIV_SHIFT	0
#define UCB1200_TELECOMCTRLA_DIV_MASK	0x7f
#define UCB1200_TELECOMCTRLA_DIV(cr)					\
	(((cr) >> UCB1200_TELECOMCTRLA_DIV_SHIFT) &			\
	UCB1200_TELECOMCTRLA_DIV_MASK)
#define UCB1200_TELECOMCTRLA_DIV_SET(cr, val)				\
	((cr) | (((val) << UCB1200_TELECOMCTRLA_DIV_SHIFT) &		\
	(UCB1200_TELECOMCTRLA_DIV_MASK << UCB1200_TELECOMCTRLA_DIV_SHIFT)))

#define UCB1200_TELECOMCTRLA_LOOP	0x0080

/*
 * Telecom control register B
 */
#define UCB1200_TELECOMCTRLB_VBF		0x0008
#define UCB1200_TELECOMCTRLB_CLIPSTATCLR	0x0010
#define UCB1200_TELECOMCTRLB_ATT		0x0040
#define UCB1200_TELECOMCTRLB_STS		0x0800
#define UCB1200_TELECOMCTRLB_MUTE		0x2000
#define UCB1200_TELECOMCTRLB_INEN		0x4000
#define UCB1200_TELECOMCTRLB_OUTEN		0x8000

/*
 * Audio control register A
 */
#define UCB1200_AUDIOCTRLA_DIV_MIN	6
#define UCB1200_AUDIOCTRLA_DIV_MAX	127
#define UCB1200_AUDIOCTRLA_DIV_SHIFT	0
#define UCB1200_AUDIOCTRLA_DIV_MASK	0x7f
#define UCB1200_AUDIOCTRLA_DIV(cr)					\
	(((cr) >> UCB1200_AUDIOCTRLA_DIV_SHIFT) &			\
	UCB1200_AUDIOCTRLA_DIV_MASK)
#define UCB1200_AUDIOCTRLA_DIV_SET(cr, val)				\
	((cr) | (((val) << UCB1200_AUDIOCTRLA_DIV_SHIFT) &		\
	(UCB1200_AUDIOCTRLA_DIV_MASK << UCB1200_AUDIOCTRLA_DIV_SHIFT)))

#define UCB1200_AUDIOCTRLA_GAIN_SHIFT	7
#define UCB1200_AUDIOCTRLA_GAIN_MASK	0x1f
#define UCB1200_AUDIOCTRLA_GAIN(cr)					\
	(((cr) >> UCB1200_AUDIOCTRLA_GAIN_SHIFT) &			\
	UCB1200_AUDIOCTRLA_GAIN_MASK)
#define UCB1200_AUDIOCTRLA_GAIN_SET(cr, val)				\
	((cr) | (((val) << UCB1200_AUDIOCTRLA_GAIN_SHIFT) &		\
	(UCB1200_AUDIOCTRLA_GAIN_MASK << UCB1200_AUDIOCTRLA_GAIN_SHIFT)))

/*
 * Audio control register B
 */
#define UCB1200_AUDIOCTRLB_ATT_MIN	0
#define UCB1200_AUDIOCTRLB_ATT_MAX	0x1f
#define UCB1200_AUDIOCTRLB_ATT_SHIFT	0
#define UCB1200_AUDIOCTRLB_ATT_MASK	0x1f
#define UCB1200_AUDIOCTRLB_ATT(cr)					\
	(((cr) >> UCB1200_AUDIOCTRLB_ATT_SHIFT) &			\
	UCB1200_AUDIOCTRLB_ATT_MASK)
#define UCB1200_AUDIOCTRLB_ATT_CLR(cr)					\
	((cr) & ~(UCB1200_AUDIOCTRLB_ATT_MASK <<			\
		  UCB1200_AUDIOCTRLB_ATT_SHIFT))
#define UCB1200_AUDIOCTRLB_ATT_SET(cr, val)				\
	((cr) | (((val) << UCB1200_AUDIOCTRLB_ATT_SHIFT) &		\
	(UCB1200_AUDIOCTRLB_ATT_MASK << UCB1200_AUDIOCTRLB_ATT_SHIFT)))

#define	UCB1200_AUDIOCTRLB_CLIPSTATCLR	0x0040
#define	UCB1200_AUDIOCTRLB_LOOP		0x0100
#define	UCB1200_AUDIOCTRLB_MUTE		0x2000
#define	UCB1200_AUDIOCTRLB_INEN		0x4000
#define	UCB1200_AUDIOCTRLB_OUTEN	0x8000

/*
 * Touch screen control register
 */
#define	UCB1200_TSCTRL_MXLOW	0x00002000
#define	UCB1200_TSCTRL_PXLOW	0x00001000
#define	UCB1200_TSCTRL_BIAS	0x00000800

#define UCB1200_TSCTRL_MODE_SHIFT	8
#define UCB1200_TSCTRL_MODE_MASK	0x7f
#define UCB1200_TSCTRL_MODE(cr)						\
	(((cr) >> UCB1200_TSCTRL_MODE_SHIFT) &				\
	UCB1200_TSCTRL_MODE_MASK)
#define UCB1200_TSCTRL_MODE_INTERRUPT	0
#define UCB1200_TSCTRL_MODE_PRESSURE	(1 << UCB1200_TSCTRL_MODE_SHIFT)
#define UCB1200_TSCTRL_MODE_POSITION0	(2 << UCB1200_TSCTRL_MODE_SHIFT)
#define UCB1200_TSCTRL_MODE_POSITION1	(3 << UCB1200_TSCTRL_MODE_SHIFT)

#define	UCB1200_TSCTRL_PYGND	0x00000080
#define	UCB1200_TSCTRL_MYGND	0x00000040
#define	UCB1200_TSCTRL_PXGND	0x00000020
#define	UCB1200_TSCTRL_MXGND	0x00000010
#define	UCB1200_TSCTRL_PYPWR	0x00000008
#define	UCB1200_TSCTRL_MYPWR	0x00000004
#define	UCB1200_TSCTRL_PXPWR	0x00000002
#define	UCB1200_TSCTRL_MXPWR	0x00000001

/* touch screen modes */
#define UCB1200_TSCTRL_YPOSITION					\
	(UCB1200_TSCTRL_PXPWR | UCB1200_TSCTRL_MXGND |			\
	UCB1200_TSCTRL_MODE_POSITION0 | UCB1200_TSCTRL_BIAS)
#define UCB1200_TSCTRL_XPOSITION					\
	(UCB1200_TSCTRL_PYPWR | UCB1200_TSCTRL_MYGND |			\
	UCB1200_TSCTRL_MODE_POSITION0 | UCB1200_TSCTRL_BIAS)
#define UCB1200_TSCTRL_PRESSURE						\
	(UCB1200_TSCTRL_PXPWR | UCB1200_TSCTRL_MXPWR |			\
	UCB1200_TSCTRL_PYGND | UCB1200_TSCTRL_MYGND |			\
	UCB1200_TSCTRL_MODE_PRESSURE | UCB1200_TSCTRL_BIAS)

#define UCB1200_TSCTRL_INTERRUPT					\
	(UCB1200_TSCTRL_PXPWR | UCB1200_TSCTRL_MXPWR |			\
	UCB1200_TSCTRL_PYGND | UCB1200_TSCTRL_MYGND |			\
	UCB1200_TSCTRL_MODE_INTERRUPT)

#define UCB1200_TSCTRL_PRESSURE1
#define UCB1200_TSCTRL_PRESSURE2
#define UCB1200_TSCTRL_PRESSURE3
#define UCB1200_TSCTRL_PRESSURE4
#define UCB1200_TSCTRL_PRESSURE5
#define UCB1200_TSCTRL_XRESISTANCE
#define UCB1200_TSCTRL_YRESISTANCE

/*
 * ADC control register
 */
#define UCB1200_ADCCTRL_ENABLE		0x8000
#define UCB1200_ADCCTRL_START		0x0080
#define UCB1200_ADCCTRL_EXTREF		0x0020

#define UCB1200_ADCCTRL_INPUT_SHIFT	2
#define UCB1200_ADCCTRL_INPUT_MASK	0x7
#define UCB1200_ADCCTRL_INPUT_SET(cr, val)				\
	((cr) | (((val) << UCB1200_ADCCTRL_INPUT_SHIFT) &		\
	(UCB1200_ADCCTRL_INPUT_MASK << UCB1200_ADCCTRL_INPUT_SHIFT)))
#define UCB1200_ADCCTRL_INPUT_TSPX	0x0
#define UCB1200_ADCCTRL_INPUT_TSMX	0x1
#define UCB1200_ADCCTRL_INPUT_TSPY	0x2
#define UCB1200_ADCCTRL_INPUT_TSMY	0x3
#define UCB1200_ADCCTRL_INPUT_AD0	0x4
#define UCB1200_ADCCTRL_INPUT_AD1	0x5
#define UCB1200_ADCCTRL_INPUT_AD2	0x6
#define UCB1200_ADCCTRL_INPUT_AD3	0x7

#define UCB1200_ADCCTRL_VREFBYP		0x0002
#define UCB1200_ADCCTRL_SYNCMODE	0x0001

/*
 * ADC data register
 */
#define UCB1200_ADCDATA_INPROGRESS	0x8000

#define UCB1200_ADCDATA_SHIFT	5
#define UCB1200_ADCDATA_MASK	0x3ff
#define UCB1200_ADCDATA(cr)						\
	(((cr) >> UCB1200_ADCDATA_SHIFT) &				\
	UCB1200_ADCDATA_MASK)

/*
 * ID register
 */
/* PHILIPS products */
/* Version 3, Device 0, Supplier 1 */
#define UCB1100_ID	0x1003
/* Version 4, Device 0, Supplier 1 */
#define UCB1200_ID	0x1004
/* Version 10, Device 0, Supplier 1 */
#define UCB1300_ID	0x100a
/* TOSHIBA TC35413F */
#define TC35413F_ID	0x9712
