#	$NetBSD: bsd.subdir.mk,v 1.49 2008/10/25 14:58:00 apb Exp $
#	@(#)bsd.subdir.mk	8.1 (Berkeley) 6/8/93

.include <bsd.init.mk>

.if !defined(NOSUBDIR)					# {

.for dir in ${SUBDIR}
.if exists(${dir}.${MACHINE})
__REALSUBDIR+=${dir}.${MACHINE}
.else
__REALSUBDIR+=${dir}
.endif
.endfor

__recurse: .USE
	@${MAKEDIRTARGET} ${.TARGET:C/^[^-]*-//} ${.TARGET:C/-.*$//}

.if make(cleandir)
__RECURSETARG=	${TARGETS:Nclean}
clean:
.else
__RECURSETARG=	${TARGETS}
.endif

# for obscure reasons, we can't do a simple .if ${dir} == ".WAIT"
# but have to assign to __TARGDIR first.
.for targ in ${__RECURSETARG}
.for dir in ${__REALSUBDIR}
__TARGDIR := ${dir}
.if ${__TARGDIR} == ".WAIT"
SUBDIR_${targ} += .WAIT
.elif !commands(${targ}-${dir})
${targ}-${dir}: .PHONY .MAKE __recurse
SUBDIR_${targ} += ${targ}-${dir}
.endif
.endfor
.if defined(__REALSUBDIR)
subdir-${targ}: .PHONY ${SUBDIR_${targ}}
${targ}: subdir-${targ}
.endif
.endfor

.endif	# ! NOSUBDIR					# }

${TARGETS}:	# ensure existence
