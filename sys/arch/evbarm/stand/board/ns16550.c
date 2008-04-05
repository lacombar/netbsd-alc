/*	$NetBSD: ns16550.c,v 1.3 2005/12/24 20:07:03 perry Exp $	*/

/*
 * Copyright (c) 2002 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
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
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* 
 * This file provides the cons_init() function and console I/O routines
 * for boards that use 16550-compatible UARTs.
 */

#include <sys/types.h>
#include <dev/ic/comreg.h>
#include <lib/libsa/stand.h>

#include "board.h"

#define	INB(x)		*((volatile uint8_t *) (CONADDR + (x)))
#define	OUTB(x, v)	*((volatile uint8_t *) (CONADDR + (x))) = (v)

#define	ISSET(t,f)	((t) & (f))

#ifndef NS16550_FREQ
#define	NS16550_FREQ	COM_FREQ
#endif

static int
comspeed(int speed)
{
#define	divrnd(n, q)	(((n)*2/(q)+1)/2)	/* divide and round off */

	int x, err;

	if (speed <= 0)  
		return (-1);
	x = divrnd((NS16550_FREQ / 16), speed);
	if (x <= 0)
		return (-1);
	err = divrnd((((quad_t)NS16550_FREQ) / 16) * 1000, speed * x) - 1000;
	if (err < 0)
		err = -err; 
	if (err > COM_TOLERANCE)
		return (-1);
	return (x);
#undef divrnd
}

void
cons_init(void)
{
	int rate;

	OUTB(com_cfcr, LCR_DLAB);
	rate = comspeed(CONSPEED);
	OUTB(com_dlbl, rate);
	OUTB(com_dlbh, rate >> 8);
	OUTB(com_cfcr, LCR_8BITS);
	OUTB(com_mcr, MCR_DTR | MCR_RTS); 
	OUTB(com_fifo,
	    FIFO_ENABLE | FIFO_RCV_RST | FIFO_XMT_RST | FIFO_TRIGGER_1);
	OUTB(com_ier, 0);
}

int
getchar(void)
{
	uint8_t stat;

	while (!ISSET(stat = INB(com_lsr), LSR_RXRDY))
		/* spin */ ;
	return (INB(com_data));
}

static void
iputchar(int c)
{
	uint8_t stat;
	int timo;

	/* Wait for any pending transmission to finish. */
	timo = 50000;   
	while (!ISSET(stat = INB(com_lsr), LSR_TXRDY) && --timo)
		/* spin */ ;

	OUTB(com_data, c);

	/* Wait for this transmission to complete. */
	timo = 1500000;
	while (!ISSET(stat = INB(com_lsr), LSR_TXRDY) && --timo)
		/* spin */ ;

	/* Clear any interrupts generated by this transmission. */
	(void) INB(com_iir);
}

void
putchar(int c)
{

	if (c == '\n')
		iputchar('\r');
	iputchar(c);
}
