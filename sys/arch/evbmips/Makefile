#	$NetBSD: Makefile,v 1.7 2008/10/25 22:27:37 apb Exp $

# Makefile for evbmips tags file and boot blocks

# Find where mips source files are for inclusion in tags
.include <../mips/Makefile.inc>

# XXX !  fix this
TEVBMIPS=	${SYSDIR}/arch/evbmips/tags
SEVBMIPS=	${SYSDIR}/arch/evbmips/evbmips/*.[ch] \
		${SYSDIR}/arch/evbmips/include/*.h \
		${SYSDIR}/arch/evbmips/isa/*.[ch]
AEVBMIPS=	${SYSDIR}/arch/evbmips/evbmips/*.S

# Directories in which to place tags links
DEVBMIPS= evbmips include isa pci

.include "../../kern/Make.tags.inc"

tags:
	-rm -f ${TEVBMIPS}
	-echo ${SEVBMIPS} ${SMIPS} | xargs ctags -wadtf ${TEVBMIPS}
	-${FINDCOMM} | xargs ctags -wadtf ${TEVBMIPS}
	egrep "^LEAF(.*)|^[AN]LEAF(.*)|^NON_LEAF(.*)" ${AEVBMIPS} ${AMIPS} | \
	    ${TOOL_SED} -e \
		"s;\([^:]*\):\([^(]*\)(\([^, )]*\)\(.*\);\3 \1 /^\2(\3\4$$/;" \
	    >> ${TEVBMIPS}
	sort -o ${TEVBMIPS} ${TEVBMIPS}

links:
	-for i in ${DEVBMIPS}; do \
	    cd $$i && rm -f tags; ln -s ../tags tags; done


SUBDIR=	compile include

.include <bsd.subdir.mk>
