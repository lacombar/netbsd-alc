#	$NetBSD: bsd.hostlib.mk,v 1.14 2004/06/10 00:29:58 lukem Exp $

.include <bsd.init.mk>
.include <bsd.sys.mk>

##### Basic targets
clean:		cleanlib

##### Default values
CFLAGS+=	${COPTS}
HOST_MKDEP?=	CC=${HOST_CC:Q} mkdep
MKDEP_SUFFIXES?=	.o .lo

# Override these:
MKDEP:=		${HOST_MKDEP}

.if ${TOOLCHAIN_MISSING} == "no" || defined(EXTERNAL_TOOLCHAIN)
OBJHOSTMACHINE=	# set
.endif

##### Build rules
.if defined(HOSTLIB)
_YHLSRCS=	${SRCS:M*.[ly]:C/\..$/.c/} ${YHEADER:D${SRCS:M*.y:.y=.h}}
DPSRCS+=	${_YHLSRCS}
CLEANFILES+=	${_YHLSRCS}
.endif	# defined(HOSTLIB)

.if !empty(SRCS:N*.h:N*.sh)
OBJS+=		${SRCS:N*.h:N*.sh:R:S/$/.lo/g}
.endif

.if defined(OBJS) && !empty(OBJS)
.NOPATH: ${OBJS} ${HOSTPROG} ${_YHLSRCS}

${OBJS}: ${DPSRCS}

lib${HOSTLIB}.a: ${OBJS} ${DPADD}
	${_MKTARGET_BUILD}
	rm -f ${.TARGET}
	${HOST_AR} cq ${.TARGET} ${OBJS}
	${HOST_RANLIB} ${.TARGET}

.endif	# defined(OBJS) && !empty(OBJS)

realall: lib${HOSTLIB}.a

cleanlib: .PHONY
	rm -f a.out [Ee]rrs mklog core *.core \
	    lib${HOSTLIB}.a ${OBJS} ${CLEANFILES}

beforedepend:
CFLAGS:=	${HOST_CFLAGS}
CPPFLAGS:=	${HOST_CPPFLAGS}

##### Pull in related .mk logic
.include <bsd.obj.mk>
.include <bsd.dep.mk>

${TARGETS}:	# ensure existence
