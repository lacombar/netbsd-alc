/* $NetBSD: udf.h,v 1.18 2008/07/17 19:10:22 reinoud Exp $ */

/*
 * Copyright (c) 2006, 2008 Reinoud Zandijk
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef _FS_UDF_UDF_H_
#define _FS_UDF_UDF_H_

#include <sys/queue.h>
#include <sys/uio.h>
#include <sys/mutex.h>

#include "udf_osta.h"
#include "ecma167-udf.h"
#include <sys/cdio.h>
#include <sys/bufq.h>
#include <sys/disk.h>
#include <sys/kthread.h>
#include <miscfs/genfs/genfs_node.h>

/* debug section */
extern int udf_verbose;

/* undefine UDF_COMPLETE_DELETE to need `purge'; but purge is not implemented */
#define UDF_COMPLETE_DELETE

/* debug categories */
#define UDF_DEBUG_VOLUMES	0x000001
#define UDF_DEBUG_LOCKING	0x000002
#define UDF_DEBUG_NODE		0x000004
#define UDF_DEBUG_LOOKUP	0x000008
#define UDF_DEBUG_READDIR	0x000010
#define UDF_DEBUG_FIDS		0x000020
#define UDF_DEBUG_DESCRIPTOR	0x000040
#define UDF_DEBUG_TRANSLATE	0x000080
#define UDF_DEBUG_STRATEGY	0x000100
#define UDF_DEBUG_READ		0x000200
#define UDF_DEBUG_WRITE		0x000400
#define UDF_DEBUG_CALL		0x000800
#define UDF_DEBUG_ATTR		0x001000
#define UDF_DEBUG_EXTATTR	0x002000
#define UDF_DEBUG_ALLOC		0x004000
#define UDF_DEBUG_ADWLK		0x008000
#define UDF_DEBUG_DIRHASH	0x010000
#define UDF_DEBUG_NOTIMPL	0x020000
#define UDF_DEBUG_SHEDULE	0x040000
#define UDF_DEBUG_ECCLINE	0x080000
#define UDF_DEBUG_SYNC		0x100000
#define UDF_DEBUG_PARANOIA	0x200000
#define UDF_DEBUG_PARANOIDADWLK	0x400000
#define UDF_DEBUG_NODEDUMP	0x800000

/* initial value of udf_verbose */
#define UDF_DEBUGGING		0

#ifdef DEBUG
#define DPRINTF(name, arg) { \
		if (udf_verbose & UDF_DEBUG_##name) {\
			printf arg;\
		};\
	}
#define DPRINTFIF(name, cond, arg) { \
		if (udf_verbose & UDF_DEBUG_##name) { \
			if (cond) printf arg;\
		};\
	}
#else
#define DPRINTF(name, arg) {}
#define DPRINTFIF(name, cond, arg) {}
#endif


/* constants to identify what kind of identifier we are dealing with */
#define UDF_REGID_DOMAIN		 1
#define UDF_REGID_UDF			 2
#define UDF_REGID_IMPLEMENTATION	 3
#define UDF_REGID_APPLICATION		 4
#define UDF_REGID_NAME			99


/* DON'T change these: they identify 13thmonkey's UDF implementation */
#define APP_NAME		"*NetBSD UDF"
#define APP_VERSION_MAIN	0
#define APP_VERSION_SUB		4
#define IMPL_NAME		"*NetBSD kernel UDF"


/* Configuration values */
#define UDF_INODE_HASHBITS 	10
#define UDF_INODE_HASHSIZE	(1<<UDF_INODE_HASHBITS)
#define UDF_INODE_HASHMASK	(UDF_INODE_HASHSIZE - 1)
#define UDF_ECCBUF_HASHBITS	10
#define UDF_ECCBUF_HASHSIZE	(1<<UDF_ECCBUF_HASHBITS)
#define UDF_ECCBUF_HASHMASK	(UDF_ECCBUF_HASHSIZE -1)

#define UDF_DIRHASH_HASHBITS	5
#define UDF_DIRHASH_HASHSIZE	(1<<UDF_DIRHASH_HASHBITS)
#define UDF_DIRHASH_HASHMASK	(UDF_DIRHASH_HASHSIZE -1)

#define UDF_ECCLINE_MAXFREE	10			/* picked */
#define UDF_ECCLINE_MAXBUSY	100			/* picked */

#define UDF_MAX_MAPPINGS	(MAXPHYS/DEV_BSIZE)	/* 128 */
#define UDF_VAT_CHUNKSIZE	(64*1024)		/* picked */
#define UDF_SYMLINKBUFLEN	(64*1024)		/* picked */


/* structure space */
#define UDF_ANCHORS		4	/* 256, 512, N-256, N */
#define UDF_PARTITIONS		4	/* overkill */
#define UDF_PMAPS		5	/* overkill */
#define UDF_LVDINT_SEGMENTS	100	/* big overkill */
#define UDF_LVINT_LOSSAGE	4	/* lose 2 openings */
#define UDF_MAX_ALLOC_EXTENTS	50	/* overkill */


/* constants */
#define UDF_MAX_NAMELEN		255			/* as per SPEC */
#define UDF_TRANS_ZERO		((uint64_t) -1)
#define UDF_TRANS_UNMAPPED	((uint64_t) -2)
#define UDF_TRANS_INTERN	((uint64_t) -3)
#define UDF_MAX_SECTOR		((uint64_t) -10)	/* high water mark */


/* RW content hint for allocation and other purposes */
#define UDF_C_PROCESSED	 0	/* not relevant */
#define UDF_C_USERDATA	 1	/* all but userdata is metadata */
//#define UDF_C_METADATA	 2	/* unspecified metadata */
#define UDF_C_DSCR	 3	/* update sectornr and CRC */
#define UDF_C_NODE	 4	/* file/dir node, update sectornr and CRC */
#define UDF_C_EXTATTRS	 5	/* dunno what to do yet */
#define UDF_C_FIDS	 6	/* update all contained fids */


/* use unused b_freelistindex for our UDF_C_TYPE */
#define b_udf_c_type	b_freelistindex


/* virtual to physical mapping types */
#define UDF_VTOP_RAWPART UDF_PMAPS	/* [0..UDF_PMAPS> are normal     */

#define UDF_VTOP_TYPE_RAW            0
#define UDF_VTOP_TYPE_UNKNOWN        0
#define UDF_VTOP_TYPE_PHYS           1
#define UDF_VTOP_TYPE_VIRT           2
#define UDF_VTOP_TYPE_SPARABLE       3
#define UDF_VTOP_TYPE_META           4


/* allocation strategies */
#define UDF_ALLOC_SEQUENTIAL         1  /* linear on NWA                 */
#define UDF_ALLOC_VAT                2  /* VAT handling                  */
#define UDF_ALLOC_SPACEMAP           3  /* spacemaps                     */
#define UDF_ALLOC_METABITMAP         4  /* metadata bitmap               */
#define UDF_ALLOC_METASEQUENTIAL     5  /* in chunks seq., nodes not seq */
#define UDF_ALLOC_RELAXEDSEQUENTIAL  6  /* only nodes not seq.           */


/* logical volume open/close actions */
#define UDF_OPEN_SESSION	  0x01  /* if needed writeout VRS + VDS	     */
#define UDF_CLOSE_SESSION	  0x02	/* close session after writing VAT   */
#define UDF_WRITE_VAT		  0x04	/* sequential VAT filesystem         */
#define UDF_WRITE_LVINT		  0x08	/* write out open lvint              */
#define UDF_WRITE_PART_BITMAPS	  0x10	/* write out partition space bitmaps */
#define UDF_APPENDONLY_LVINT	  0x20	/* no shifting, only appending       */
#define UDFLOGVOL_BITS "\20\1OPENSESSION\2CLOSESESSION\3WRITEVAT\4WRITELVINT"\
			  "\5APPENDONLY"

/* logical volume error handling actions */
#define UDF_UPDATE_TRACKINFO	  0x01	/* update trackinfo and re-shedule   */
#define UDF_REMAP_BLOCK		  0x02	/* remap the failing block length    */
#define UDFONERROR_BITS "\20\1UPDATE_TRACKINFO\2REMAP_BLOCK"


/* readdir cookies */
#define UDF_DIRCOOKIE_DOT 1


/* malloc pools */
MALLOC_DECLARE(M_UDFMNT);
MALLOC_DECLARE(M_UDFVOLD);
MALLOC_DECLARE(M_UDFTEMP);

struct pool udf_node_pool;
struct pool udf_dirhash_pool;
struct pool udf_dirhash_entry_pool;
struct udf_node;
struct udf_strategy;


struct udf_lvintq {
	uint32_t		start;
	uint32_t		end;
	uint32_t		pos;
	uint32_t		wpos;
};


struct udf_bitmap {
	uint8_t			*blob;			/* allocated         */
	uint8_t			*bits;			/* bits themselves   */
	uint8_t			*pages;			/* dirty pages       */
	uint32_t		 max_offset;		/* in bits           */
	uint32_t		 data_pos;		/* position in data  */
	uint32_t		 metadata_pos;		/* .. in metadata    */
};


struct udf_strat_args {
	struct udf_mount *ump;
	struct udf_node  *udf_node;
	struct long_ad   *icb;
	union dscrptr    *dscr;
	struct buf       *nestbuf;
	kauth_cred_t	  cred;
	int waitfor;
};

struct udf_strategy {
	int  (*create_logvol_dscr)  (struct udf_strat_args *args);
	void (*free_logvol_dscr)    (struct udf_strat_args *args);
	int  (*read_logvol_dscr)    (struct udf_strat_args *args);
	int  (*write_logvol_dscr)   (struct udf_strat_args *args);
	void (*queuebuf)	    (struct udf_strat_args *args);
	void (*discstrat_init)      (struct udf_strat_args *args);
	void (*discstrat_finish)    (struct udf_strat_args *args);
};

extern struct udf_strategy udf_strat_bootstrap;
extern struct udf_strategy udf_strat_sequential;
extern struct udf_strategy udf_strat_direct;
extern struct udf_strategy udf_strat_rmw;


/* pre cleanup */
struct udf_mount {
	struct mount		*vfs_mountp;
	struct vnode		*devvp;	
	struct mmc_discinfo	 discinfo;
	struct udf_args		 mount_args;

	/* format descriptors */
	kmutex_t		 logvol_mutex;
	struct anchor_vdp	*anchors[UDF_ANCHORS];	/* anchors to VDS    */
	struct pri_vol_desc	*primary_vol;		/* identification    */
	struct logvol_desc	*logical_vol;		/* main mapping v->p */
	struct unalloc_sp_desc	*unallocated;		/* free UDF space    */
	struct impvol_desc	*implementation;	/* likely reduntant  */
	struct logvol_int_desc	*logvol_integrity;	/* current integrity */
	struct part_desc	*partitions[UDF_PARTITIONS]; /* partitions   */
	/* logvol_info is derived; points *into* other structures */
	struct udf_logvol_info	*logvol_info;		/* integrity descr.  */

	/* fileset and root directories */
	struct fileset_desc	*fileset_desc;		/* normally one      */

	/* tracing logvol integrity history */
	struct udf_lvintq	 lvint_trace[UDF_LVDINT_SEGMENTS];
	int			 lvopen;		/* logvol actions    */
	int			 lvclose;		/* logvol actions    */

	/* disc allocation / writing method */
	int			 lvreadwrite;		/* bits */
	int			 data_alloc;		/* all userdata */
	int			 meta_alloc;		/* all metadata */
	int			 data_part;
	int			 metadata_part;
	kmutex_t		 allocate_mutex;

	/* logical to physical translations */
	int 			 vtop[UDF_PMAPS+1];	/* vpartnr trans     */
	int			 vtop_tp[UDF_PMAPS+1];	/* type of trans     */

	/* sequential track info */
	struct mmc_trackinfo	 data_track;
	struct mmc_trackinfo	 metadata_track;

	/* VAT */
	uint32_t		 first_possible_vat_location;
	uint32_t		 last_possible_vat_location;
	uint32_t		 vat_entries;
	uint32_t		 vat_offset;		/* offset in table   */
	uint32_t		 vat_last_free_lb;	/* last free lb_num  */
	uint32_t		 vat_table_len;
	uint32_t		 vat_table_alloc_len;
	uint8_t			*vat_table;
	uint8_t			*vat_pages;		/* TODO */
	struct udf_node		*vat_node;		/* system node       */

	/* space bitmaps */
	struct space_bitmap_desc*part_unalloc_dscr[UDF_PARTITIONS];
	struct space_bitmap_desc*part_freed_dscr  [UDF_PARTITIONS];
	struct udf_bitmap	 part_unalloc_bits[UDF_PARTITIONS];
	struct udf_bitmap	 part_freed_bits  [UDF_PARTITIONS];

	/* sparable */
	uint32_t		 sparable_packet_size;
	uint32_t		 packet_size;
	struct udf_sparing_table*sparing_table;

	/* meta */
	struct udf_node 	*metadata_node;		/* system node       */
	struct udf_node 	*metadatamirror_node;	/* system node       */
	struct udf_node 	*metadatabitmap_node;	/* system node       */
	struct udf_bitmap	 metadata_bitmap;	/* TODO : readin */

	/* hash table to lookup icb -> udf_node and sorted list for sync */
	kmutex_t	ihash_lock;
	kmutex_t	get_node_lock;
	LIST_HEAD(, udf_node) udf_nodes[UDF_INODE_HASHSIZE];
	LIST_HEAD(, udf_node) sorted_udf_nodes;		/* sorted sync list  */

	/* syncing */
	int		syncing;			/* are we syncing?   */
	kcondvar_t 	dirtynodes_cv;			/* sleeping on sync  */

	/* late allocation */
	uint32_t		 uncomitted_lb;		/* for free space    */
	struct long_ad		*la_node_ad_cpy;		/* issue buf */
	uint64_t		*la_lmapping, *la_pmapping;	/* issue buf */

	/* lists */
	STAILQ_HEAD(udfmntpts, udf_mount) all_udf_mntpnts;

	/* device strategy */
	struct udf_strategy	*strategy;
	void			*strategy_private;
};


/* dirent's d_namlen is to avoid useless costly fid->dirent translations */
struct udf_dirhash_entry {
	uint32_t		 hashvalue;
	uint64_t		 offset;
	uint32_t		 d_namlen;
	uint32_t		 fid_size;
	LIST_ENTRY(udf_dirhash_entry) next;
};


struct udf_dirhash {
	LIST_HEAD(, udf_dirhash_entry) entries[UDF_DIRHASH_HASHSIZE];
	LIST_HEAD(, udf_dirhash_entry) free_entries;
};


/*
 * UDF node describing a file/directory.
 *
 * BUGALERT claim node_mutex before reading/writing to prevent inconsistencies !
 */
struct udf_node {
	struct genfs_node	i_gnode;		/* has to be first   */
	struct vnode		*vnode;			/* vnode associated  */
	struct udf_mount	*ump;

	kmutex_t		 node_mutex;
	kcondvar_t		 node_lock;		/* sleeping lock */
	char const		*lock_fname;
	int			 lock_lineno;

	/* one of `fe' or `efe' can be set, not both (UDF file entry dscr.)  */
	struct file_entry	*fe;
	struct extfile_entry	*efe;
	struct alloc_ext_entry	*ext[UDF_MAX_ALLOC_EXTENTS];
	int			 num_extensions;

	/* location found, recording location & hints */
	struct long_ad		 loc;			/* FID/hash loc.     */
	struct long_ad		 write_loc;		/* strat 4096 loc    */
	int			 needs_indirect;	/* has missing indr. */
	struct long_ad		 ext_loc[UDF_MAX_ALLOC_EXTENTS];

	struct udf_dirhash	*dir_hash;

	/* misc */
	uint32_t		 i_flags;		/* associated flags  */
	struct lockf		*lockf;			/* lock list         */
	uint32_t		 outstanding_bufs;	/* file data         */
	uint32_t		 outstanding_nodedscr;	/* node dscr         */

	/* references to associated nodes */
	struct udf_node		*extattr;
	struct udf_node		*streamdir;
	struct udf_node		*my_parent;		/* if extended attr. */

	LIST_ENTRY(udf_node)	 hashchain;		/* inside hash line  */
	LIST_ENTRY(udf_node)	 sortchain;		/* sorted udf nodes  */
};


/* misc. flags stored in i_flags (XXX needs cleaning up) */
#define	IN_ACCESS		0x0001	/* Inode access time update request  */
#define	IN_CHANGE		0x0002	/* Inode change time update request  */
#define	IN_UPDATE		0x0004	/* Inode was written to; update mtime*/
#define	IN_MODIFY		0x0008	/* Modification time update request  */
#define	IN_MODIFIED		0x0010	/* node has been modified */
#define	IN_ACCESSED		0x0020	/* node has been accessed */
#define	IN_RENAME		0x0040	/* node is being renamed. XXX ?? */
#define	IN_DELETED		0x0080	/* node is unlinked, no FID reference*/
#define	IN_LOCKED		0x0100	/* node is locked by condvar */
#define	IN_SYNCED		0x0200	/* node is being used by sync */
#define	IN_CALLBACK_ULK		0x0400	/* node will be unlocked by callback */
#define	IN_NODE_REBUILD		0x0800	/* node is rebuild */
#define	IN_DIRHASH_COMPLETE	0x1000	/* dirhash is complete */
#define	IN_DIRHASH_BROKEN	0x2000	/* dirhash is broken on readin */


#define IN_FLAGBITS \
	"\10\1IN_ACCESS\2IN_CHANGE\3IN_UPDATE\4IN_MODIFY\5IN_MODIFIED" \
	"\6IN_ACCESSED\7IN_RENAME\10IN_DELETED\11IN_LOCKED\12IN_SYNCED" \
	"\13IN_CALLBACK_ULK\14IN_NODE_REBUILD\15IN_DIRHASH_COMPLETE" \
	"\16IN_DIRHASH_BROKEN"

#endif /* !_FS_UDF_UDF_H_ */
