#	$NetBSD: Makefile,v 1.23 2006/08/26 15:26:02 matt Exp $
#	@(#)Makefile	8.1 (Berkeley) 6/5/93

.include <bsd.own.mk>

PROG=	route
MAN=	route.8
SRCS=	route.c show.c keywords.c

.if (${USE_INET6} != "no")
CPPFLAGS+=-DINET6
.endif

# The Makefile over in ../../distrib/utils/x_route
# would like keywords.[ch] to always exist here, so
# they are now checked in as sources.
#
# CPPFLAGS+=-I.
# CLEANFILES+= keywords.c keywords.h
# keywords.c keywords.h : keywords.sh
# 	${HOST_SH} keywords.sh

.include <bsd.prog.mk>
