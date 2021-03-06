#	$NetBSD: Makefile.inc,v 1.15 2008/06/24 13:02:25 matt Exp $

BINDIR=			/usr/mdec

release:

S?=		${.CURDIR}/../../../..

.PHONY: machine-links
beforedepend: machine-links
# ${MACHINE} then ${MACHINE_ARCH}
machine-links:
	-rm -f machine && \
		ln -s $S/arch/hp300/include machine
	-rm -f hp300 && \
		ln -s $S/arch/hp300/include hp300
	-rm -f m68k && \
		ln -s $S/arch/m68k/include m68k
CLEANFILES+=machine hp300 m68k

realall: machine-links .WAIT ${PROG}

