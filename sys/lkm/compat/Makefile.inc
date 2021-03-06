#	$NetBSD: Makefile.inc,v 1.20 2007/04/22 08:30:01 dsl Exp $

.if !target(assym.h)
S!=	cd ${.CURDIR}/../../..;pwd

NOMAN=	# defined

.include <bsd.own.mk>

DPSRCS+=	assym.h
CLEANFILES+=	assym.h

.if (${MACHINE_ARCH} == "powerpc")
.if (${MACHINE} == "macppc" || ${MACHINE} == "ofppc" || ${MACHINE} == "prep")
# use oea
GENASSYM_DIR= $S/arch/${MACHINE_ARCH}/oea
CPPFLAGS+=	-DPPC_OEA
.endif
.endif

.if exists($S/arch/${MACHINE}/${MACHINE}/genassym.cf)
GENASSYM_DIR?= $S/arch/${MACHINE}/${MACHINE}
.elif exists($S/arch/${MACHINE_CPU}/${MACHINE_CPU}/genassym.cf)
GENASSYM_DIR?= $S/arch/${MACHINE_CPU}/${MACHINE_CPU}
.else
GENASSYM_DIR?=
.endif

assym.h: ${GENASSYM_DIR}/genassym.cf
	${TOOL_GENASSYM} -- ${CC} ${CFLAGS} ${CPPFLAGS} ${PROF} \
		< ${GENASSYM_DIR}/genassym.cf > assym.h.tmp && \
	mv -f assym.h.tmp assym.h

.include "../Makefile.inc"
.endif
