/*	$NetBSD: cons.c,v 1.8 2008/03/23 17:19:57 tsutsui Exp $	*/

/*
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
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
 * from: Utah Hdr: cons.c 1.7 92/02/28
 *
 *	@(#)cons.c	8.1 (Berkeley) 6/10/93
 */
/*
 * Copyright (c) 1988 University of Utah.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
 * from: Utah Hdr: cons.c 1.7 92/02/28
 *
 *	@(#)cons.c	8.1 (Berkeley) 6/10/93
 */

#include <lib/libsa/stand.h>

#include <machine/cpu.h>

#include "boot.h"
#include "cons.h"

#ifdef CONS_SERIAL
void siocnprobe(struct consdev *);
void siocninit(struct consdev *);
void siocnputchar(void *, int);
int siocngetchar(void *);
int siocnscan(void *);
# include "ns16550.h"
# ifndef COMPORT
#  define COMPORT COM1
# endif
# ifndef COMSPEED
#  define COMSPEED 9600
# endif
#endif

#ifdef CONS_ZS
void zscnprobe(struct consdev *);
void zscninit(struct consdev *);
void zscnputchar(void *, int);
int zscngetchar(void *);
int zscnscan(void *);
#include "zs.h"
#ifndef ZSCHAN
#define ZSCHAN ZS_CHAN_A
#endif
#ifndef ZSSPEED
#define ZSSPEED 115200
#endif
#endif

struct consdev constab[] = {
#ifdef CONS_SERIAL
	{ "com", COMPORT, COMSPEED,
	    siocnprobe, siocninit, siocngetchar, siocnputchar, siocnscan },
#endif
#ifdef CONS_ZS
	{ "zs", ZSCHAN, ZSSPEED,
	    zscnprobe, zscninit, zscngetchar, zscnputchar, zscnscan },
#endif
	{ 0 }
};

struct consdev *cn_tab;

char *
cninit(int *addr, int *speed)
{
	register struct consdev *cp;

	cn_tab = NULL;
	for (cp = constab; cp->cn_probe; cp++) {
		(*cp->cn_probe)(cp);
		if (cp->cn_pri > CN_DEAD &&
		    (cn_tab == NULL || cp->cn_pri > cn_tab->cn_pri))
			cn_tab = cp;
	}
	if (cn_tab) {
		(*cn_tab->cn_init)(cn_tab);
		*addr = cn_tab->address;
		*speed = cn_tab->speed;
		return cn_tab->cn_name;
	}

	return NULL;
}

int
cngetc(void)
{

	if (cn_tab)
		return (*cn_tab->cn_getc)(cn_tab->cn_dev);
	return 0;
}

void
cnputc(int c)
{

	if (cn_tab)
		(*cn_tab->cn_putc)(cn_tab->cn_dev, c);
}

int
cnscan(void)
{

	if (cn_tab)
		return (*cn_tab->cn_scan)(cn_tab->cn_dev);
	return -1;
}

#ifdef CONS_FB
/*
 * frame buffer console
 */
void
fbcnprobe(struct consdev *cp)
{

	cp->cn_pri = CN_INTERNAL;
}

void
fbcninit(struct consdev *cp)
{

	video_init((uint8_t *)cp->address);
	kbdreset();
}

int
fbcngetchar(void *dev)
{

	return kbd_getc();
}

void
fbcnputchar(void *dev, int c)
{

	video_putc(c);
}

int
fbcnscan(void *dev)
{

	return kbd(1);
}
#endif /* CONS_FB */

#ifdef CONS_VGA
/*
 * VGA console
 */
void
vgacnprobe(struct consdev *cp)
{

	cp->cn_pri = CN_NORMAL;
}

void
vgacninit(struct consdev *cp)
{

	vga_reset((uint8_t *)cp->address);
	vga_init((uint8_t *)cp->address);
	kbdreset();
}

int
vgacngetchar(void *dev)
{

	return kbd_getc();
}

void
vgacnputchar(void *dev, int c)
{

	vga_putc(c);
}

int
vgacnscan(void *dev)
{

	return kbd(1);
}
#endif /* CONS_VGA */

#ifdef CONS_SERIAL
/*
 * serial console
 */
void
siocnprobe(struct consdev *cp)
{

	if (*((uint32_t *)COMPROBE) != 0 &&
	    cobalt_id != COBALT_ID_QUBE2700)
		cp->cn_pri = CN_REMOTE;
}

void
siocninit(struct consdev *cp)
{

	cp->cn_dev = (void *)NS16550_init(cp->address, cp->speed);
}

int
siocngetchar(void *dev)
{

	return NS16550_getc((struct NS16550 *)dev);
}

void
siocnputchar(void *dev, int c)
{

	if (c == '\n')
		NS16550_putc((struct NS16550 *)dev, '\r');
	NS16550_putc((struct NS16550 *)dev, c);
}

int
siocnscan(void *dev)
{

	return NS16550_scankbd((struct NS16550 *)dev);
}
#endif /* CONS_SERIAL */

#ifdef CONS_ZS
/*
 * optional z85c30 serial console on Qube2700
 */
void
zscnprobe(struct consdev *cp)
{

	if (*((uint32_t *)ZSPROBE) != 0 &&
	    cobalt_id == COBALT_ID_QUBE2700)
		cp->cn_pri = CN_REMOTE;
}

void
zscninit(struct consdev *cp)
{

	cp->cn_dev = zs_init(cp->address, cp->speed);
}

int
zscngetchar(void *dev)
{

	return zs_getc(dev);
}

void
zscnputchar(void *dev, int c)
{

	if (c == '\n')
		zs_putc(dev, '\r');
	zs_putc(dev, c);
}

int
zscnscan(void *dev)
{

	return zs_scan(dev);
}
#endif	/* CONS_ZS */
