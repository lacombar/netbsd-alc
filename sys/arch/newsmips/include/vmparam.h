/*	$NetBSD: vmparam.h,v 1.4 2000/12/01 17:57:44 tsutsui Exp $	*/

#include <mips/vmparam.h>

/*
 * NEWS has one physical memory segment.
 */
#define	VM_PHYSSEG_MAX		1

#define	VM_NFREELIST		1
#define	VM_FREELIST_DEFAULT	0
