#	$NetBSD: Makefile,v 1.2 2008/07/29 13:17:42 pooka Exp $
#

.PATH:  ${.CURDIR}/../../../../ufs/ext2fs

LIB=	rumpfs_ext2fs

SRCS=	ext2fs_alloc.c ext2fs_balloc.c ext2fs_bmap.c ext2fs_bswap.c	\
	ext2fs_inode.c ext2fs_lookup.c ext2fs_readwrite.c ext2fs_subr.c	\
	ext2fs_vfsops.c ext2fs_vnops.c

CFLAGS+=-Wno-pointer-sign

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
