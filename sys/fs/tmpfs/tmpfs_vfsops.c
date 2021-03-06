/*	$NetBSD: tmpfs_vfsops.c,v 1.44 2008/07/29 09:10:09 pooka Exp $	*/

/*
 * Copyright (c) 2005, 2006, 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Julio M. Merino Vidal, developed as part of Google's Summer of Code
 * 2005 program.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Efficient memory file system.
 *
 * tmpfs is a file system that uses NetBSD's virtual memory sub-system
 * (the well-known UVM) to store file data and metadata in an efficient
 * way.  This means that it does not follow the structure of an on-disk
 * file system because it simply does not need to.  Instead, it uses
 * memory-specific data structures and algorithms to automatically
 * allocate and release resources.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: tmpfs_vfsops.c,v 1.44 2008/07/29 09:10:09 pooka Exp $");

#include <sys/param.h>
#include <sys/types.h>
#include <sys/kmem.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/systm.h>
#include <sys/vnode.h>
#include <sys/proc.h>
#include <sys/module.h>

#include <miscfs/genfs/genfs.h>
#include <fs/tmpfs/tmpfs.h>
#include <fs/tmpfs/tmpfs_args.h>

MODULE(MODULE_CLASS_VFS, tmpfs, NULL);

/* --------------------------------------------------------------------- */

static int	tmpfs_mount(struct mount *, const char *, void *, size_t *);
static int	tmpfs_start(struct mount *, int);
static int	tmpfs_unmount(struct mount *, int);
static int	tmpfs_root(struct mount *, struct vnode **);
static int	tmpfs_vget(struct mount *, ino_t, struct vnode **);
static int	tmpfs_fhtovp(struct mount *, struct fid *, struct vnode **);
static int	tmpfs_vptofh(struct vnode *, struct fid *, size_t *);
static int	tmpfs_statvfs(struct mount *, struct statvfs *);
static int	tmpfs_sync(struct mount *, int, kauth_cred_t);
static void	tmpfs_init(void);
static void	tmpfs_done(void);
static int	tmpfs_snapshot(struct mount *, struct vnode *,
		    struct timespec *);

/* --------------------------------------------------------------------- */

static int
tmpfs_mount(struct mount *mp, const char *path, void *data, size_t *data_len)
{
	struct lwp *l = curlwp;
	int error;
	ino_t nodes;
	size_t pages;
	struct tmpfs_mount *tmp;
	struct tmpfs_node *root;
	struct tmpfs_args *args = data;

	if (*data_len < sizeof *args)
		return EINVAL;

	/* Handle retrieval of mount point arguments. */
	if (mp->mnt_flag & MNT_GETARGS) {
		if (mp->mnt_data == NULL)
			return EIO;
		tmp = VFS_TO_TMPFS(mp);

		args->ta_version = TMPFS_ARGS_VERSION;
		args->ta_nodes_max = tmp->tm_nodes_max;
		args->ta_size_max = tmp->tm_pages_max * PAGE_SIZE;

		root = tmp->tm_root;
		args->ta_root_uid = root->tn_uid;
		args->ta_root_gid = root->tn_gid;
		args->ta_root_mode = root->tn_mode;

		*data_len = sizeof *args;
		return 0;
	}

	if (mp->mnt_flag & MNT_UPDATE) {
		/* XXX: There is no support yet to update file system
		 * settings.  Should be added. */

		return EOPNOTSUPP;
	}

	if (args->ta_version != TMPFS_ARGS_VERSION)
		return EINVAL;

	/* Do not allow mounts if we do not have enough memory to preserve
	 * the minimum reserved pages. */
	if (tmpfs_mem_info(true) < TMPFS_PAGES_RESERVED)
		return EINVAL;

	/* Get the maximum number of memory pages this file system is
	 * allowed to use, based on the maximum size the user passed in
	 * the mount structure.  A value of zero is treated as if the
	 * maximum available space was requested. */
	if (args->ta_size_max < PAGE_SIZE || args->ta_size_max >= SIZE_MAX)
		pages = SIZE_MAX;
	else
		pages = args->ta_size_max / PAGE_SIZE +
		    (args->ta_size_max % PAGE_SIZE == 0 ? 0 : 1);
	if (pages > INT_MAX)
		pages = INT_MAX;
	KASSERT(pages > 0);

	if (args->ta_nodes_max <= 3)
		nodes = 3 + pages * PAGE_SIZE / 1024;
	else
		nodes = args->ta_nodes_max;
	if (nodes > INT_MAX)
		nodes = INT_MAX;
	KASSERT(nodes >= 3);

	/* Allocate the tmpfs mount structure and fill it. */
	tmp = kmem_alloc(sizeof(struct tmpfs_mount), KM_SLEEP);
	if (tmp == NULL)
		return ENOMEM;

	tmp->tm_nodes_max = nodes;
	tmp->tm_nodes_cnt = 0;
	LIST_INIT(&tmp->tm_nodes);

	mutex_init(&tmp->tm_lock, MUTEX_DEFAULT, IPL_NONE);

	tmp->tm_pages_max = pages;
	tmp->tm_pages_used = 0;
	tmpfs_pool_init(&tmp->tm_dirent_pool, sizeof(struct tmpfs_dirent),
	    "dirent", tmp);
	tmpfs_pool_init(&tmp->tm_node_pool, sizeof(struct tmpfs_node),
	    "node", tmp);
	tmpfs_str_pool_init(&tmp->tm_str_pool, tmp);

	/* Allocate the root node. */
	error = tmpfs_alloc_node(tmp, VDIR, args->ta_root_uid,
	    args->ta_root_gid, args->ta_root_mode & ALLPERMS, NULL, NULL,
	    VNOVAL, &root);
	KASSERT(error == 0 && root != NULL);
	root->tn_links++;
	tmp->tm_root = root;

	mp->mnt_data = tmp;
	mp->mnt_flag |= MNT_LOCAL;
	mp->mnt_stat.f_namemax = MAXNAMLEN;
	mp->mnt_fs_bshift = PAGE_SHIFT;
	mp->mnt_dev_bshift = DEV_BSHIFT;
	mp->mnt_iflag |= IMNT_MPSAFE;
	vfs_getnewfsid(mp);

	return set_statvfs_info(path, UIO_USERSPACE, "tmpfs", UIO_SYSSPACE,
	    mp->mnt_op->vfs_name, mp, l);
}

/* --------------------------------------------------------------------- */

static int
tmpfs_start(struct mount *mp, int flags)
{

	return 0;
}

/* --------------------------------------------------------------------- */

/* ARGSUSED2 */
static int
tmpfs_unmount(struct mount *mp, int mntflags)
{
	int error;
	int flags = 0;
	struct tmpfs_mount *tmp;
	struct tmpfs_node *node;

	/* Handle forced unmounts. */
	if (mntflags & MNT_FORCE)
		flags |= FORCECLOSE;

	/* Finalize all pending I/O. */
	error = vflush(mp, NULL, flags);
	if (error != 0)
		return error;

	tmp = VFS_TO_TMPFS(mp);

	/* Free all associated data.  The loop iterates over the linked list
	 * we have containing all used nodes.  For each of them that is
	 * a directory, we free all its directory entries.  Note that after
	 * freeing a node, it will automatically go to the available list,
	 * so we will later have to iterate over it to release its items. */
	node = LIST_FIRST(&tmp->tm_nodes);
	while (node != NULL) {
		struct tmpfs_node *next;

		if (node->tn_type == VDIR) {
			struct tmpfs_dirent *de;

			de = TAILQ_FIRST(&node->tn_spec.tn_dir.tn_dir);
			while (de != NULL) {
				struct tmpfs_dirent *nde;

				nde = TAILQ_NEXT(de, td_entries);
				tmpfs_free_dirent(tmp, de, false);
				de = nde;
				node->tn_size -= sizeof(struct tmpfs_dirent);
			}
		}

		next = LIST_NEXT(node, tn_entries);
		tmpfs_free_node(tmp, node);
		node = next;
	}

	tmpfs_pool_destroy(&tmp->tm_dirent_pool);
	tmpfs_pool_destroy(&tmp->tm_node_pool);
	tmpfs_str_pool_destroy(&tmp->tm_str_pool);

	KASSERT(tmp->tm_pages_used == 0);

	/* Throw away the tmpfs_mount structure. */
	mutex_destroy(&tmp->tm_lock);
	kmem_free(tmp, sizeof(*tmp));
	mp->mnt_data = NULL;

	return 0;
}

/* --------------------------------------------------------------------- */

static int
tmpfs_root(struct mount *mp, struct vnode **vpp)
{

	return tmpfs_alloc_vp(mp, VFS_TO_TMPFS(mp)->tm_root, vpp);
}

/* --------------------------------------------------------------------- */

static int
tmpfs_vget(struct mount *mp, ino_t ino,
    struct vnode **vpp)
{

	printf("tmpfs_vget called; need for it unknown yet\n");
	return EOPNOTSUPP;
}

/* --------------------------------------------------------------------- */

static int
tmpfs_fhtovp(struct mount *mp, struct fid *fhp, struct vnode **vpp)
{
	bool found;
	struct tmpfs_fid tfh;
	struct tmpfs_mount *tmp;
	struct tmpfs_node *node;

	tmp = VFS_TO_TMPFS(mp);

	if (fhp->fid_len != sizeof(struct tmpfs_fid))
		return EINVAL;

	memcpy(&tfh, fhp, sizeof(struct tmpfs_fid));

	if (tfh.tf_id >= tmp->tm_nodes_max)
		return EINVAL;

	found = false;
	mutex_enter(&tmp->tm_lock);
	LIST_FOREACH(node, &tmp->tm_nodes, tn_entries) {
		if (node->tn_id == tfh.tf_id &&
		    node->tn_gen == tfh.tf_gen) {
			found = true;
			break;
		}
	}
	mutex_exit(&tmp->tm_lock);

	/* XXXAD nothing to prevent 'node' from being removed. */
	return found ? tmpfs_alloc_vp(mp, node, vpp) : EINVAL;
}

/* --------------------------------------------------------------------- */

static int
tmpfs_vptofh(struct vnode *vp, struct fid *fhp, size_t *fh_size)
{
	struct tmpfs_fid tfh;
	struct tmpfs_node *node;

	if (*fh_size < sizeof(struct tmpfs_fid)) {
		*fh_size = sizeof(struct tmpfs_fid);
		return E2BIG;
	}
	*fh_size = sizeof(struct tmpfs_fid);
	node = VP_TO_TMPFS_NODE(vp);

	memset(&tfh, 0, sizeof(tfh));
	tfh.tf_len = sizeof(struct tmpfs_fid);
	tfh.tf_gen = node->tn_gen;
	tfh.tf_id = node->tn_id;
	memcpy(fhp, &tfh, sizeof(tfh));

	return 0;
}

/* --------------------------------------------------------------------- */

/* ARGSUSED2 */
static int
tmpfs_statvfs(struct mount *mp, struct statvfs *sbp)
{
	fsfilcnt_t freenodes;
	struct tmpfs_mount *tmp;

	tmp = VFS_TO_TMPFS(mp);

	sbp->f_iosize = sbp->f_frsize = sbp->f_bsize = PAGE_SIZE;

	sbp->f_blocks = TMPFS_PAGES_MAX(tmp);
	sbp->f_bavail = sbp->f_bfree = TMPFS_PAGES_AVAIL(tmp);
	sbp->f_bresvd = 0;

	freenodes = MIN(tmp->tm_nodes_max - tmp->tm_nodes_cnt,
	    TMPFS_PAGES_AVAIL(tmp) * PAGE_SIZE / sizeof(struct tmpfs_node));

	sbp->f_files = tmp->tm_nodes_cnt + freenodes;
	sbp->f_favail = sbp->f_ffree = freenodes;
	sbp->f_fresvd = 0;

	copy_statvfs_info(sbp, mp);

	return 0;
}

/* --------------------------------------------------------------------- */

/* ARGSUSED0 */
static int
tmpfs_sync(struct mount *mp, int waitfor,
    kauth_cred_t uc)
{

	return 0;
}

/* --------------------------------------------------------------------- */

static void
tmpfs_init(void)
{

}

/* --------------------------------------------------------------------- */

static void
tmpfs_done(void)
{

}

/* --------------------------------------------------------------------- */

static int
tmpfs_snapshot(struct mount *mp, struct vnode *vp,
    struct timespec *ctime)
{

	return EOPNOTSUPP;
}

/* --------------------------------------------------------------------- */

/*
 * tmpfs vfs operations.
 */

extern const struct vnodeopv_desc tmpfs_fifoop_opv_desc;
extern const struct vnodeopv_desc tmpfs_specop_opv_desc;
extern const struct vnodeopv_desc tmpfs_vnodeop_opv_desc;

const struct vnodeopv_desc * const tmpfs_vnodeopv_descs[] = {
	&tmpfs_fifoop_opv_desc,
	&tmpfs_specop_opv_desc,
	&tmpfs_vnodeop_opv_desc,
	NULL,
};

struct vfsops tmpfs_vfsops = {
	MOUNT_TMPFS,			/* vfs_name */
	sizeof (struct tmpfs_args),
	tmpfs_mount,			/* vfs_mount */
	tmpfs_start,			/* vfs_start */
	tmpfs_unmount,			/* vfs_unmount */
	tmpfs_root,			/* vfs_root */
	(void *)eopnotsupp,		/* vfs_quotactl */
	tmpfs_statvfs,			/* vfs_statvfs */
	tmpfs_sync,			/* vfs_sync */
	tmpfs_vget,			/* vfs_vget */
	tmpfs_fhtovp,			/* vfs_fhtovp */
	tmpfs_vptofh,			/* vfs_vptofh */
	tmpfs_init,			/* vfs_init */
	NULL,				/* vfs_reinit */
	tmpfs_done,			/* vfs_done */
	NULL,				/* vfs_mountroot */
	tmpfs_snapshot,			/* vfs_snapshot */
	vfs_stdextattrctl,		/* vfs_extattrctl */
	(void *)eopnotsupp,		/* vfs_suspendctl */
	genfs_renamelock_enter,
	genfs_renamelock_exit,
	(void *)eopnotsupp,
	tmpfs_vnodeopv_descs,
	0,				/* vfs_refcount */
	{ NULL, NULL },
};

static int
tmpfs_modcmd(modcmd_t cmd, void *arg)
{

	switch (cmd) {
	case MODULE_CMD_INIT:
		return vfs_attach(&tmpfs_vfsops);
	case MODULE_CMD_FINI:
		return vfs_detach(&tmpfs_vfsops);
	default:
		return ENOTTY;
	}
}
