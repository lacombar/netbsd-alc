#	$NetBSD: Makefile,v 1.7 2005/12/11 12:24:49 christos Exp $

.include "../Makefile.inc"

.PATH.c: $S/coda ${.CURDIR}/../coda

KMOD=	coda5

SRCS=	lkminit_vfs.c
SRCS+=	coda_namecache.c coda_psdev.c coda_subr.c coda_venus.c
SRCS+=	coda_vfsops.c coda_vnops.c

CPPFLAGS+=	-DCODA_COMPAT_5

.include <bsd.kmod.mk>
