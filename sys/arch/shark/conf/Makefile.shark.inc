#	$NetBSD: Makefile.shark.inc,v 1.11 2008/06/07 18:47:41 he Exp $

MACHINE_ARCH=		arm
CPPFLAGS+=		-D${MACHINE}
GENASSYM_EXTRAS+=	${THISARM}/shark/genassym.cf

.if (${OBJECT_FMT} == "ELF")
# Need to convert the kernel from ELF to a.out so that OpenFirmware
# can load it.

LINKFLAGS=	-T ldscript

SYSTEM_LD_HEAD_EXTRA+=; \
	( cat ${ARM}/conf/kern.ldscript.head ; \
	  OBJDUMP=${OBJDUMP} ${HOST_SH} $S/conf/mkldscript.sh \
		${SYSTEM_OBJ} ; \
	  cat ${ARM}/conf/kern.ldscript.tail ) > ldscript

SYSTEM_LD_TAIL_EXTRA+=; \
	echo \
	 "${OBJCOPY} -O a.out-arm-netbsd \
		-R .ident -R .arm.atpcs -R .comment -R .note.netbsd.ident \
		-R .debug_abbrev -R .debug_info -R .debug_line -R .debug_str \
		-R .debug_frame -R .debug_loc -R .debug_pubnames \
		-R .debug_aranges \
		$@ $@.aout"; \
	${OBJCOPY} -O a.out-arm-netbsd \
		-R .ident -R .arm.atpcs -R .comment -R .note.netbsd.ident \
		-R .debug_abbrev -R .debug_info -R .debug_line -R .debug_str \
		-R .debug_frame -R .debug_loc -R .debug_pubnames \
		-R .debug_aranges \
		$@ $@.aout

KERNIMAGES=	netbsd netbsd.aout

.endif
