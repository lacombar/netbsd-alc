#	$NetBSD: Makefile,v 1.38 2008/07/02 07:44:13 dyoung Exp $
#	@(#)Makefile	8.1 (Berkeley) 6/5/93

# when making a change to this file, please check if the change is
# also needed for src/distrib/utils/x_ifconfig/Makefile.

.include <bsd.own.mk>

MAN=	ifconfig.8

WARNS=4
CFLAGS+=-g
SRCS= af_atalk.c af_iso.c af_link.c carp.c
.if (${USE_INET6} != "no")
CPPFLAGS+=	-DINET6
SRCS+= af_inet6.c
.endif

.include "Makefile.inc"

.include <bsd.prog.mk>
