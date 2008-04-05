/*	$NetBSD: procfs_machdep.c,v 1.5 2005/12/11 12:19:15 christos Exp $	*/

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: procfs_machdep.c,v 1.5 2005/12/11 12:19:15 christos Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <miscfs/procfs/procfs.h>


/*
 * Linux-style /proc/cpuinfo.
 * Only used when procfs is mounted with -o linux.
 */
int
procfs_getcpuinfstr(char *sbuf, int *len)
{
	*len = 0;

	return 0;
}
