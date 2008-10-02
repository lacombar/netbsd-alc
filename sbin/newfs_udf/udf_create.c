/* $NetBSD: udf_create.c,v 1.12 2008/07/26 20:20:56 reinoud Exp $ */

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

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: udf_create.c,v 1.12 2008/07/26 20:20:56 reinoud Exp $");
#endif /* not lint */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <err.h>
#include <sys/types.h>
#include <sys/param.h>
#include "udf_create.h"


#if 0
# ifndef DEBUG
#   define DEBUG
#  endif
#endif


void
udf_init_create_context(void)
{
	/* clear */
	memset(&context, 0, sizeof(struct udf_create_context));

	/* fill with defaults currently known */
	context.dscrver = 3;
	context.min_udf = 0x0102;
	context.max_udf = 0x0260;
	context.serialnum = 1;		/* default */

	context.gmtoff  = 0;
	context.sector_size = 512;	/* minimum for UDF */

	context.logvol_name  = NULL;
	context.primary_name = NULL;
	context.volset_name  = NULL;
	context.fileset_name = NULL;

	context.app_name	  = "*NetBSD newfs";
	context.app_version_main =  __NetBSD_Version__ / 100000000;
	context.app_version_sub  = (__NetBSD_Version__ / 1000000) % 100;
	context.impl_name        = "*NetBSD kernel UDF";

	context.vds_seq = 0;		/* first one starts with zero */

	/* Minimum value of 16 : UDF 3.2.1.1, 3.3.3.4. */
	context.unique_id       = 0x10;

	context.num_files       = 0;
	context.num_directories = 0;
}


static uint32_t
udf_space_bitmap_len(uint32_t part_size)
{
	return  sizeof(struct space_bitmap_desc)-1 +
		part_size/8;
}


static uint32_t
udf_bytes_to_sectors(uint64_t bytes)
{
	uint32_t sector_size = layout.sector_size;
	return (bytes + sector_size -1) / sector_size;
}


int
udf_calculate_disc_layout(int format_flags, int min_udf,
	uint32_t wrtrack_skew,
	uint32_t first_lba, uint32_t last_lba,
	uint32_t sector_size, uint32_t blockingnr,
	uint32_t sparable_blocks, float meta_fract)
{
	uint64_t kbsize, bytes;
	uint32_t sparable_blockingnr;
	uint32_t align_blockingnr;
	uint32_t pos, mpos;

	/* clear */
	bzero(&layout, sizeof(struct udf_disclayout));

	/* fill with parameters */
	layout.wrtrack_skew    = wrtrack_skew;
	layout.first_lba       = first_lba;
	layout.last_lba        = last_lba;
	layout.sector_size     = sector_size;
	layout.blockingnr      = blockingnr;
	layout.sparable_blocks = sparable_blocks;

	/* start disc layouting */

	/*
	 * location of iso9660 vrs is defined as first sector AFTER 32kb,
	 * minimum `sector size' 2048
	 */
	layout.iso9660_vrs = ((32*1024 + sector_size - 1) / sector_size)
		+ first_lba;

	/* anchor starts at specified offset in sectors */
	layout.anchors[0] = first_lba + 256;
	if (format_flags & FORMAT_TRACK512)
		layout.anchors[0] = first_lba + 512;
	layout.anchors[1] = last_lba - 256;
	layout.anchors[2] = last_lba;

	/* update workable space */
	first_lba = layout.anchors[0] + blockingnr;
	last_lba  = layout.anchors[1] - 1;

	/* XXX rest of anchor packet can be added to unallocated space descr */

	/* reserve space for VRS and VRS copy and associated tables */
	layout.vds_size = MAX(16, blockingnr);     /* UDF 2.2.3.1+2 */
	layout.vds1 = first_lba;
	first_lba += layout.vds_size;              /* next packet */

	if (format_flags & FORMAT_SEQUENTIAL) {
		/* for sequential, append them ASAP */
		layout.vds2 = first_lba;
		first_lba += layout.vds_size;
	} else {
		layout.vds2 = layout.anchors[1] - layout.vds_size;
		last_lba = layout.vds2 - 1;	/* XXX -1 ?? */
	}

	/* reserve space for logvol integrity sequence */
	layout.lvis_size = MAX(8192/sector_size, 2 * blockingnr);
	if (format_flags & FORMAT_VAT)
		layout.lvis_size = 2;
	if (format_flags & FORMAT_WORM)
		layout.lvis_size = 64 * blockingnr;

	/* TODO skip bad blocks in LVID sequence; for now use f.e. */
//first_lba+=128;
	layout.lvis = first_lba;
	first_lba += layout.lvis_size;

	/* initial guess of UDF partition size */
	layout.part_start_lba = first_lba;
	layout.part_size_lba = last_lba - layout.part_start_lba;

	/* all non sequential media needs an unallocated space bitmap */
	layout.alloc_bitmap_dscr_size = 0;
	if ((format_flags & FORMAT_SEQUENTIAL) == 0) {
		bytes = udf_space_bitmap_len(layout.part_size_lba);
		layout.alloc_bitmap_dscr_size = udf_bytes_to_sectors(bytes);

		/* XXX freed space map when applicable */
	}

	/*
	 * Note that for (bug) compatibility with version UDF 2.00 (fixed in
	 * 2.01 and higher) the blocking size needs to be 32 sectors otherwise
	 * the drive's blockingnr.
	 */

	sparable_blockingnr = blockingnr;
	if (min_udf <= 0x200)
		sparable_blockingnr = 32;

	align_blockingnr = blockingnr;
	if (format_flags & (FORMAT_SPARABLE | FORMAT_META))
		align_blockingnr = sparable_blockingnr;

	layout.align_blockingnr    = align_blockingnr;
	layout.sparable_blockingnr = sparable_blockingnr;

	/*
	 * Align partition LBA space to blocking granularity. Not strickly
	 * nessisary for non sparables but safer for the VRS data since it is
	 * not updated sporadically
	 */

	if ((format_flags & FORMAT_SEQUENTIAL) == 0) {
#ifdef DEBUG
		printf("Lost %d slack sectors at start\n", UDF_ROUNDUP(
			first_lba - wrtrack_skew, align_blockingnr) -
				(first_lba - wrtrack_skew));
		printf("Lost %d slack sectors at end\n",
			(first_lba - wrtrack_skew) - UDF_ROUNDDOWN(
				first_lba - wrtrack_skew, align_blockingnr));
#endif

		first_lba = UDF_ROUNDUP( first_lba - wrtrack_skew, align_blockingnr);
		last_lba  = UDF_ROUNDDOWN(last_lba - wrtrack_skew, align_blockingnr);
	}

	if ((format_flags & FORMAT_SPARABLE) == 0)
		layout.sparable_blocks = 0;

	if (format_flags & FORMAT_SPARABLE) {
		layout.sparable_area_size =
			layout.sparable_blocks * sparable_blockingnr;

		/* a sparing table descriptor is a whole blockingnr sectors */
		layout.sparing_table_dscr_lbas = sparable_blockingnr;

		/* place the descriptors at the start and end of the area */
		layout.spt_1 = first_lba;
		first_lba += layout.sparing_table_dscr_lbas;

		layout.spt_2 = last_lba - layout.sparing_table_dscr_lbas;
		last_lba -= layout.sparing_table_dscr_lbas;

		/* allocate sparable section */
		layout.sparable_area = first_lba;
		first_lba += layout.sparable_area_size;
	}

	/* update guess of UDF partition size */
	layout.part_start_lba = first_lba;
	layout.part_size_lba = last_lba - layout.part_start_lba;

	/* determine partition selection for data and metadata */
	context.data_part     = 0;
	context.metadata_part = context.data_part;
	if ((format_flags & FORMAT_VAT) || (format_flags & FORMAT_META))
		context.metadata_part = context.data_part + 1;

	/*
	 * Pick fixed logical space sector numbers for main FSD, rootdir and
	 * unallocated space. The reason for this pre-allocation is that they
	 * are referenced in the volume descriptor sequence and hence can't be
	 * allocated later.
	 */
	pos = 0;
	layout.unalloc_space = pos;
	pos += layout.alloc_bitmap_dscr_size;

	/* claim metadata descriptors and partition space [UDF 2.2.10] */
	if (format_flags & FORMAT_META) {
		/* note: all in backing partition space */
		layout.meta_file   = pos++;
		layout.meta_bitmap = pos++;;
		layout.meta_mirror = layout.part_size_lba-1;
		layout.meta_alignment  = MAX(blockingnr, sparable_blockingnr);
		layout.meta_blockingnr = MAX(layout.meta_alignment, 32);

		/* calculate our partition length and store in sectors */
		layout.meta_part_size_lba = layout.part_size_lba * meta_fract;
		layout.meta_part_size_lba = MAX(layout.meta_part_size_lba, 32);
		layout.meta_part_size_lba =
			UDF_ROUNDDOWN(layout.meta_part_size_lba, layout.meta_blockingnr);

		/* calculate positions */
		bytes = udf_space_bitmap_len(layout.meta_part_size_lba);
		layout.meta_bitmap_dscr_size = udf_bytes_to_sectors(bytes);

		layout.meta_bitmap_space = pos;
		pos += layout.meta_bitmap_dscr_size;

		layout.meta_part_start_lba  = UDF_ROUNDUP(pos, layout.meta_alignment);
	}

	mpos = (context.metadata_part == context.data_part) ? pos : 0;
	layout.fsd           = mpos;	mpos += 1;
	layout.rootdir       = mpos;	mpos += 1;
	layout.vat           = mpos;	mpos += 1;	/* if present */

#if 0
	printf("Summary so far\n");
	printf("\tiso9660_vrs\t\t%d\n", layout.iso9660_vrs);
	printf("\tanchor0\t\t\t%d\n", layout.anchors[0]);
	printf("\tanchor1\t\t\t%d\n", layout.anchors[1]);
	printf("\tanchor2\t\t\t%d\n", layout.anchors[2]);
	printf("\tvds_size\t\t%d\n", layout.vds_size);
	printf("\tvds1\t\t\t%d\n", layout.vds1);
	printf("\tvds2\t\t\t%d\n", layout.vds2);
	printf("\tlvis_size\t\t%d\n", layout.lvis_size);
	printf("\tlvis\t\t\t%d\n", layout.lvis);
	if (format_flags & FORMAT_SPARABLE) {
		printf("\tsparable size\t\t%d\n", layout.sparable_area_size);
		printf("\tsparable\t\t%d\n", layout.sparable_area);
	}
	printf("\tpartion start lba\t%d\n", layout.part_start_lba);
	printf("\tpartition size\t\t%d Kb, %d Mb\n",
		(layout.part_size_lba * sector_size) / 1024,
		(layout.part_size_lba * sector_size) / (1024*1024));
	if ((format_flags & FORMAT_SEQUENTIAL) == 0) {
		printf("\tpart bitmap start\t%d\n",   layout.unalloc_space);
		printf("\t\tfor %d lba\n", layout.alloc_bitmap_dscr_size);
	}
	if (format_flags & FORMAT_META) {
		printf("\tmeta blockingnr\t\t%d\n", layout.meta_blockingnr);
		printf("\tmeta alignment\t\t%d\n",  layout.meta_alignment);
		printf("\tmeta size\t\t%d Kb, %d Mb\n",
			(layout.meta_part_size_lba * sector_size) / 1024,
			(layout.meta_part_size_lba * sector_size) / (1024*1024));
		printf("\tmeta file\t\t%d\n", layout.meta_file);
		printf("\tmeta mirror\t\t%d\n", layout.meta_mirror);
		printf("\tmeta bitmap\t\t%d\n", layout.meta_bitmap);
		printf("\tmeta bitmap start\t%d\n", layout.meta_bitmap_space);
		printf("\t\tfor %d lba\n", layout.meta_bitmap_dscr_size);
		printf("\tmeta space start\t%d\n",  layout.meta_part_start_lba);
		printf("\t\tfor %d lba\n", layout.meta_part_size_lba);
	}
	printf("\n");
#endif

	kbsize = (uint64_t) last_lba * sector_size;
	printf("Total space on this medium aprox. %"PRIu64" Kb, %"PRIu64" Mb\n",
			kbsize/1024, kbsize/(1024*1024));
	kbsize = (uint64_t) (layout.part_size_lba - layout.alloc_bitmap_dscr_size
		- layout.meta_bitmap_dscr_size) * sector_size;
	printf("Free space on this volume aprox.  %"PRIu64" Kb, %"PRIu64" Mb\n\n",
			kbsize/1024, kbsize/(1024*1024));

	return 0;
}


int
udf_validate_tag_sum(union dscrptr *dscr)
{
	struct desc_tag *tag = &dscr->tag;
	uint8_t *pos, sum, cnt;

	/* calculate TAG header checksum */
	pos = (uint8_t *) tag;
	sum = 0;

	for(cnt = 0; cnt < 16; cnt++) {
		if (cnt != 4) sum += *pos;
		pos++;
	};
	tag->cksum = sum;	/* 8 bit */

	return 0;
}


/* assumes sector number of descriptor to be allready present */
int
udf_validate_tag_and_crc_sums(union dscrptr *dscr)
{
	struct desc_tag *tag = &dscr->tag;
	uint16_t crc;

	/* check payload CRC if applicable */
	if (udf_rw16(tag->desc_crc_len) > 0) {
		crc = udf_cksum(((uint8_t *) tag) + UDF_DESC_TAG_LENGTH,
			udf_rw16(tag->desc_crc_len));
		tag->desc_crc = udf_rw16(crc);
	};

	/* calculate TAG header checksum */
	return udf_validate_tag_sum(dscr);
}


void
udf_inittag(struct desc_tag *tag, int tagid, uint32_t loc)
{
	tag->id 		= udf_rw16(tagid);
	tag->descriptor_ver	= udf_rw16(context.dscrver);
	tag->cksum		= 0;
	tag->reserved		= 0;
	tag->serial_num		= udf_rw16(context.serialnum);
	tag->tag_loc            = udf_rw32(loc);
}


int
udf_create_anchor(int num)
{
	struct anchor_vdp *avdp;
	uint32_t vds_extent_len = layout.vds_size * context.sector_size;
	
	if ((avdp = calloc(1, context.sector_size)) == NULL)
		return ENOMEM;

	udf_inittag(&avdp->tag, TAGID_ANCHOR, layout.anchors[num]);

	avdp->main_vds_ex.loc = udf_rw32(layout.vds1);
	avdp->main_vds_ex.len = udf_rw32(vds_extent_len);

	avdp->reserve_vds_ex.loc = udf_rw32(layout.vds2);
	avdp->reserve_vds_ex.len = udf_rw32(vds_extent_len);

	/* CRC length for an anchor is 512 - tag length; defined in Ecma 167 */
	avdp->tag.desc_crc_len = udf_rw16(512-UDF_DESC_TAG_LENGTH);

	context.anchors[num] = avdp;
	return 0;
}


void
udf_create_terminator(union dscrptr *dscr, uint32_t loc)
{
	bzero(dscr, context.sector_size);
	udf_inittag(&dscr->tag, TAGID_TERM, loc);

	/* CRC length for an anchor is 512 - tag length; defined in Ecma 167 */
	dscr->tag.desc_crc_len = udf_rw16(512-UDF_DESC_TAG_LENGTH);
}


void
udf_osta_charset(struct charspec *charspec)
{
	bzero(charspec, sizeof(struct charspec));
	charspec->type = 0;
	strcpy((char *) charspec->inf, "OSTA Compressed Unicode");
}


void
udf_encode_osta_id(char *osta_id, uint16_t len, char *text)
{
	uint16_t  u16_name[1024];
	uint8_t  *pos;
	uint16_t *pos16;

	bzero(osta_id, len);
	if (!text || (strlen(text) == 0)) return;

	bzero(u16_name, sizeof(uint16_t) * 1023);

	/* convert ascii to 16 bits unicode */
	pos   = (uint8_t *) text;
	pos16 = u16_name;
	while (*pos) {
		*pos16 = *pos;
		pos++; pos16++;
	};
	*pos16 = 0;

	udf_CompressUnicode(len, 8, (unicode_t *) u16_name, (byte *) osta_id);

	/* Ecma 167/7.2.13 states that length is recorded in the last byte */
	osta_id[len-1] = strlen(text)+1;
}


/* first call udf_set_regid and then the suffix */
void
udf_set_regid(struct regid *regid, char const *name)
{
	bzero(regid, sizeof(struct regid));
	regid->flags    = 0;		/* not dirty and not protected */
	strcpy((char *) regid->id, name);
}


void
udf_add_domain_regid(struct regid *regid)
{
	uint16_t *ver;

	ver  = (uint16_t *) regid->id_suffix;
	*ver = udf_rw16(context.min_udf);
}


void
udf_add_udf_regid(struct regid *regid)
{
	uint16_t *ver;

	ver  = (uint16_t *) regid->id_suffix;
	*ver = udf_rw16(context.min_udf);

	regid->id_suffix[2] = 4;	/* unix */
	regid->id_suffix[3] = 8;	/* NetBSD */
}


void
udf_add_impl_regid(struct regid *regid)
{
	regid->id_suffix[0] = 4;	/* unix */
	regid->id_suffix[1] = 8;	/* NetBSD */
}


void
udf_add_app_regid(struct regid *regid)
{
	regid->id_suffix[0] = context.app_version_main;
	regid->id_suffix[1] = context.app_version_sub;
}


/*
 * Fill in timestamp structure based on clock_gettime(). Time is reported back as a time_t
 * accompanied with a nano second field.
 *
 * The husec, usec and csec could be relaxed in type.
 */
static void
udf_timespec_to_timestamp(struct timespec *timespec, struct timestamp *timestamp)
{
	struct tm tm;
	uint64_t husec, usec, csec;

	bzero(timestamp, sizeof(struct timestamp));
	gmtime_r(&timespec->tv_sec, &tm);

	/*
	 * Time type and time zone : see ECMA 1/7.3, UDF 2., 2.1.4.1, 3.1.1.
	 *
	 * Lower 12 bits are two complement signed timezone offset if bit 12
	 * (method 1) is clear. Otherwise if bit 12 is set, specify timezone
	 * offset to -2047 i.e. unsigned `zero'
	 */

	/* set method 1 for CUT/GMT */
	timestamp->type_tz	= udf_rw16((1<<12) + 0);
	timestamp->year		= udf_rw16(tm.tm_year + 1900);
	timestamp->month	= tm.tm_mon + 1;	/* `tm' uses 0..11 for months */
	timestamp->day		= tm.tm_mday;
	timestamp->hour		= tm.tm_hour;
	timestamp->minute	= tm.tm_min;
	timestamp->second	= tm.tm_sec;

	usec   = (timespec->tv_nsec + 500) / 1000;	/* round */
	husec  =   usec / 100;
	usec  -=  husec * 100;				/* only 0-99 in usec  */
	csec   =  husec / 100;				/* only 0-99 in csec  */
	husec -=   csec * 100;				/* only 0-99 in husec */

	timestamp->centisec	= csec;
	timestamp->hund_usec	= husec;
	timestamp->usec		= usec;
}



void
udf_set_timestamp_now(struct timestamp *timestamp)
{
	struct timespec now;

	clock_gettime(CLOCK_REALTIME, &now);
	udf_timespec_to_timestamp(&now, timestamp);
}


int
udf_create_primaryd(void)
{
	struct pri_vol_desc *pri;
	int crclen;

	pri = calloc(1, context.sector_size);
	if (pri == NULL)
		return ENOMEM;

	bzero(pri, context.sector_size);
	udf_inittag(&pri->tag, TAGID_PRI_VOL, /* loc */ 0);
	pri->seq_num = udf_rw32(context.vds_seq); context.vds_seq++;

	pri->pvd_num = udf_rw32(0);		/* default serial */
	udf_encode_osta_id(pri->vol_id, 32, context.primary_name);

	/* set defaults for single disc volumes as UDF prescribes */
	pri->vds_num      = udf_rw16(1);
	pri->max_vol_seq  = udf_rw16(1);
	pri->ichg_lvl     = udf_rw16(2);
	pri->max_ichg_lvl = udf_rw16(3);
	pri->flags        = udf_rw16(0);

	pri->charset_list     = udf_rw32(1);	/* only CS0 */
	pri->max_charset_list = udf_rw32(1);	/* only CS0 */

	udf_encode_osta_id(pri->volset_id, 128, context.volset_name);
	udf_osta_charset(&pri->desc_charset);
	udf_osta_charset(&pri->explanatory_charset);

	udf_set_regid(&pri->app_id, context.app_name);
	udf_add_app_regid(&pri->app_id);

	udf_set_regid(&pri->imp_id, context.impl_name);
	udf_add_impl_regid(&pri->imp_id);

	udf_set_timestamp_now(&pri->time);

	crclen = sizeof(struct pri_vol_desc) - UDF_DESC_TAG_LENGTH;
	pri->tag.desc_crc_len = udf_rw16(crclen);

	context.primary_vol = pri;

	return 0;
}


/* XXX no support for unallocated or freed space tables yet (!) */
int
udf_create_partitiond(int part_num, int part_accesstype)
{
	struct part_desc     *pd;
	struct part_hdr_desc *phd;
	uint32_t sector_size, bitmap_bytes;
	int crclen;

	sector_size = context.sector_size;
	bitmap_bytes = layout.alloc_bitmap_dscr_size * sector_size;

	if (context.partitions[part_num]) {
		printf("Internal error: partition %d allready defined\n",
			part_num);
		return EINVAL;
	}

	pd = calloc(1, context.sector_size);
	if (pd == NULL)
		return ENOMEM;
	phd = &pd->_impl_use.part_hdr;

	udf_inittag(&pd->tag, TAGID_PARTITION, /* loc */ 0);
	pd->seq_num  = udf_rw32(context.vds_seq); context.vds_seq++;

	pd->flags    = udf_rw16(1);		/* allocated */
	pd->part_num = udf_rw16(part_num);	/* only one physical partition */

	if (context.dscrver == 2) {
		udf_set_regid(&pd->contents, "+NSR02");
	} else {
		udf_set_regid(&pd->contents, "+NSR03");
	}
	udf_add_app_regid(&pd->contents);

	phd->unalloc_space_bitmap.len    = udf_rw32(bitmap_bytes);
	phd->unalloc_space_bitmap.lb_num = udf_rw32(layout.unalloc_space);

	if (layout.freed_space) {
		phd->freed_space_bitmap.len    = udf_rw32(bitmap_bytes);
		phd->freed_space_bitmap.lb_num = udf_rw32(layout.freed_space);
	}

	pd->access_type = udf_rw32(part_accesstype);
	pd->start_loc   = udf_rw32(layout.part_start_lba);
	pd->part_len    = udf_rw32(layout.part_size_lba);

	udf_set_regid(&pd->imp_id, context.impl_name);
	udf_add_impl_regid(&pd->imp_id);

	crclen = sizeof(struct part_desc) - UDF_DESC_TAG_LENGTH;
	pd->tag.desc_crc_len = udf_rw16(crclen);

	context.partitions[part_num] = pd;

	return 0;
}


int
udf_create_unalloc_spaced(void)
{
	struct unalloc_sp_desc *usd;
	int crclen;

	usd = calloc(1, context.sector_size);
	if (usd == NULL)
		return ENOMEM;

	udf_inittag(&usd->tag, TAGID_UNALLOC_SPACE, /* loc */ 0);
	usd->seq_num  = udf_rw32(context.vds_seq); context.vds_seq++;

	/* no default entries */
	usd->alloc_desc_num = udf_rw32(0);		/* no entries */

	crclen  = sizeof(struct unalloc_sp_desc) - sizeof(struct extent_ad);
	crclen -= UDF_DESC_TAG_LENGTH;
	usd->tag.desc_crc_len = udf_rw16(crclen);

	context.unallocated = usd;

	return 0;
}


static int
udf_create_base_logical_dscr(void)
{
	struct logvol_desc *lvd;
	uint32_t sector_size;
	int crclen;

	sector_size = context.sector_size;

	lvd = calloc(1, sector_size);
	if (lvd == NULL)
		return ENOMEM;

	udf_inittag(&lvd->tag, TAGID_LOGVOL, /* loc */ 0);
	lvd->seq_num  = udf_rw32(context.vds_seq); context.vds_seq++;

	udf_osta_charset(&lvd->desc_charset);
	udf_encode_osta_id(lvd->logvol_id, 128, context.logvol_name);
	lvd->lb_size = udf_rw32(context.sector_size);

	udf_set_regid(&lvd->domain_id, "*OSTA UDF Compliant");
	udf_add_domain_regid(&lvd->domain_id);

	/* no partition mappings/entries yet */
	lvd->mt_l = udf_rw32(0);
	lvd->n_pm = udf_rw32(0);

	udf_set_regid(&lvd->imp_id, context.impl_name);
	udf_add_impl_regid(&lvd->imp_id);

	lvd->integrity_seq_loc.loc = udf_rw32(layout.lvis);
	lvd->integrity_seq_loc.len = udf_rw32(layout.lvis_size * sector_size);

	/* just one fsd for now */
	lvd->lv_fsd_loc.len = udf_rw32(sector_size);
	lvd->lv_fsd_loc.loc.part_num = udf_rw32(context.metadata_part);
	lvd->lv_fsd_loc.loc.lb_num   = udf_rw32(layout.fsd);

	crclen  = sizeof(struct logvol_desc) - 1 - UDF_DESC_TAG_LENGTH;
	lvd->tag.desc_crc_len = udf_rw16(crclen);

	context.logical_vol = lvd;
	context.vtop_tp[UDF_VTOP_RAWPART]      = UDF_VTOP_TYPE_RAW;
	context.vtop_offset[UDF_VTOP_RAWPART] = 0;

	return 0;
}


static void 
udf_add_logvol_part_physical(uint16_t phys_part)
{
	struct logvol_desc *logvol = context.logical_vol;
	union  udf_pmap *pmap;
	uint8_t         *pmap_pos;
	int crclen, pmap1_size;
	int log_part;

	log_part = udf_rw32(logvol->n_pm);
	pmap_pos = logvol->maps + udf_rw32(logvol->mt_l);
	pmap1_size = sizeof(struct part_map_1);

	pmap = (union udf_pmap *) pmap_pos;
	pmap->pm1.type        = 1;
	pmap->pm1.len         = sizeof(struct part_map_1);
	pmap->pm1.vol_seq_num = udf_rw16(1);		/* no multi-volume */
	pmap->pm1.part_num    = udf_rw16(phys_part);

	context.vtop       [log_part] = phys_part;
	context.vtop_tp    [log_part] = UDF_VTOP_TYPE_PHYS;
	context.vtop_offset[log_part] = layout.part_start_lba;
	context.part_size[log_part] = layout.part_size_lba;
	context.part_free[log_part] = layout.part_size_lba;

	/* increment number of partions and length */
	logvol->n_pm = udf_rw32(log_part + 1);
	logvol->mt_l = udf_rw32(udf_rw32(logvol->mt_l) + pmap1_size);

	crclen = udf_rw16(logvol->tag.desc_crc_len) + pmap1_size;
	logvol->tag.desc_crc_len = udf_rw16(crclen);
}


static void
udf_add_logvol_part_virtual(uint16_t phys_part)
{
	union  udf_pmap *pmap;
	struct logvol_desc *logvol = context.logical_vol;
	uint8_t *pmap_pos;
	int crclen, pmapv_size;
	int log_part;

	log_part = udf_rw32(logvol->n_pm);
	pmap_pos = logvol->maps + udf_rw32(logvol->mt_l);
	pmapv_size = sizeof(struct part_map_2);

	pmap = (union udf_pmap *) pmap_pos;
	pmap->pmv.type        = 2;
	pmap->pmv.len         = pmapv_size;

	udf_set_regid(&pmap->pmv.id, "*UDF Virtual Partition");
	udf_add_udf_regid(&pmap->pmv.id);

	pmap->pmv.vol_seq_num = udf_rw16(1);		/* no multi-volume */
	pmap->pmv.part_num    = udf_rw16(phys_part);

	context.vtop       [log_part] = phys_part;
	context.vtop_tp    [log_part] = UDF_VTOP_TYPE_VIRT;
	context.vtop_offset[log_part] = context.vtop_offset[phys_part];
	context.part_size[log_part] = 0xffffffff;
	context.part_free[log_part] = 0xffffffff;

	/* increment number of partions and length */
	logvol->n_pm = udf_rw32(log_part + 1);
	logvol->mt_l = udf_rw32(udf_rw32(logvol->mt_l) + pmapv_size);

	crclen = udf_rw16(logvol->tag.desc_crc_len) + pmapv_size;
	logvol->tag.desc_crc_len = udf_rw16(crclen);
}


/* sparing table size is in bytes */
static void
udf_add_logvol_part_sparable(uint16_t phys_part)
{
	union  udf_pmap *pmap;
	struct logvol_desc *logvol = context.logical_vol;
	uint32_t *st_pos, sparable_bytes;;
	uint8_t  *pmap_pos;
	int num, crclen, pmaps_size;
	int log_part;

	log_part = udf_rw32(logvol->n_pm);
	pmap_pos = logvol->maps + udf_rw32(logvol->mt_l);
	pmaps_size = sizeof(struct part_map_2);
	sparable_bytes = layout.sparable_area_size * context.sector_size;

	pmap = (union udf_pmap *) pmap_pos;
	pmap->pms.type        = 2;
	pmap->pms.len         = pmaps_size;

	udf_set_regid(&pmap->pmv.id, "*UDF Sparable Partition");
	udf_add_udf_regid(&pmap->pmv.id);

	pmap->pms.vol_seq_num = udf_rw16(1);		/* no multi-volume */
	pmap->pms.part_num    = udf_rw16(phys_part);

	pmap->pms.packet_len  = udf_rw16(layout.sparable_blockingnr);
	pmap->pms.st_size     = udf_rw32(sparable_bytes);

	/* enter spare tables  */
	st_pos = &pmap->pms.st_loc[0];
	*st_pos++ = udf_rw32(layout.spt_1);
	*st_pos++ = udf_rw32(layout.spt_2);

	num = 2;
	if (layout.spt_2 == 0) num--;
	if (layout.spt_1 == 0) num--;
	pmap->pms.n_st = num;		/* 8 bit */

	/* the vtop_offset needs to explicitly set since there is no phys. */
	context.vtop       [log_part] = phys_part;
	context.vtop_tp    [log_part] = UDF_VTOP_TYPE_SPARABLE;
	context.vtop_offset[log_part] = layout.part_start_lba;
	context.part_size[log_part] = layout.part_size_lba;
	context.part_free[log_part] = layout.part_size_lba;

	/* increment number of partions and length */
	logvol->n_pm = udf_rw32(log_part + 1);
	logvol->mt_l = udf_rw32(udf_rw32(logvol->mt_l) + pmaps_size);

	crclen = udf_rw16(logvol->tag.desc_crc_len) + pmaps_size;
	logvol->tag.desc_crc_len = udf_rw16(crclen);
}


int
udf_create_sparing_tabled(void)
{
	struct udf_sparing_table *spt;
	struct spare_map_entry   *sme;
	uint32_t loc;
	int crclen, cnt;

	spt = calloc(context.sector_size, layout.sparing_table_dscr_lbas);
	if (spt == NULL)
		return ENOMEM;

	/* a sparing table descriptor is a whole sparable_blockingnr sectors */
	udf_inittag(&spt->tag, TAGID_SPARING_TABLE, /* loc */ 0);

	udf_set_regid(&spt->id, "*UDF Sparing Table");
	udf_add_udf_regid(&spt->id);

	spt->rt_l    = udf_rw16(layout.sparable_blocks);
	spt->seq_num = udf_rw32(0);			/* first generation */

	for (cnt = 0; cnt < layout.sparable_blocks; cnt++) {
		sme = &spt->entries[cnt];
		loc = layout.sparable_area + cnt * layout.sparable_blockingnr;
		sme->org = udf_rw32(0xffffffff);	/* open for reloc */
		sme->map = udf_rw32(loc);
	}

	/* calculate crc len for actual size */
	crclen  = sizeof(struct udf_sparing_table) - UDF_DESC_TAG_LENGTH;
	crclen += (layout.sparable_blocks-1) * sizeof(struct spare_map_entry);
	spt->tag.desc_crc_len = udf_rw16(crclen);

	context.sparing_table = spt;

	return 0;
}


static void
udf_add_logvol_part_meta(uint16_t phys_part)
{
	union  udf_pmap *pmap;
	struct logvol_desc *logvol = context.logical_vol;
	uint8_t *pmap_pos;
	int crclen, pmapv_size;
	int log_part;

	log_part = udf_rw32(logvol->n_pm);
	pmap_pos = logvol->maps + udf_rw32(logvol->mt_l);
	pmapv_size = sizeof(struct part_map_2);

	pmap = (union udf_pmap *) pmap_pos;
	pmap->pmm.type        = 2;
	pmap->pmm.len         = pmapv_size;

	udf_set_regid(&pmap->pmm.id, "*UDF Metadata Partition");
	udf_add_udf_regid(&pmap->pmm.id);

	pmap->pmm.vol_seq_num = udf_rw16(1);		/* no multi-volume */
	pmap->pmm.part_num    = udf_rw16(phys_part);

	/* fill in meta data file(s) and alloc/alignment unit sizes */
	pmap->pmm.meta_file_lbn        = udf_rw32(layout.meta_file);
	pmap->pmm.meta_mirror_file_lbn = udf_rw32(layout.meta_mirror);
	pmap->pmm.meta_bitmap_file_lbn = udf_rw32(layout.meta_bitmap);
	pmap->pmm.alloc_unit_size      = udf_rw32(layout.meta_blockingnr);
	pmap->pmm.alignment_unit_size  = udf_rw16(layout.meta_alignment);
	pmap->pmm.flags                = 0; /* METADATA_DUPLICATED */

	context.vtop       [log_part] = phys_part;
	context.vtop_tp    [log_part] = UDF_VTOP_TYPE_META;
	context.vtop_offset[log_part] =
		context.vtop_offset[phys_part] + layout.meta_part_start_lba;
	context.part_size[log_part] = layout.meta_part_size_lba;
	context.part_free[log_part] = layout.meta_part_size_lba;

	/* increment number of partions and length */
	logvol->n_pm = udf_rw32(log_part + 1);
	logvol->mt_l = udf_rw32(udf_rw32(logvol->mt_l) + pmapv_size);

	crclen = udf_rw16(logvol->tag.desc_crc_len) + pmapv_size;
	logvol->tag.desc_crc_len = udf_rw16(crclen);
}


int
udf_create_logical_dscr(int format_flags)
{
	int error;

	if ((error = udf_create_base_logical_dscr()))
		return error;

	/* we pass data_part for there might be a read-only part one day */
	if (format_flags & FORMAT_SPARABLE) {
		/* sparable partition mapping has no physical mapping */
		udf_add_logvol_part_sparable(context.data_part);
	} else {
		udf_add_logvol_part_physical(context.data_part);
	}

	if (format_flags & FORMAT_VAT) {
		/* add VAT virtual mapping; reflects on datapart */
		udf_add_logvol_part_virtual(context.data_part);
	}
	if (format_flags & FORMAT_META) {
		/* add META data mapping; reflects on datapart */
		udf_add_logvol_part_meta(context.data_part);
	}

	return 0;
}


int
udf_create_impvold(char *field1, char *field2, char *field3)
{
	struct impvol_desc *ivd;
	struct udf_lv_info *lvi;
	int crclen;

	ivd = calloc(1, context.sector_size);
	if (ivd == NULL)
		return ENOMEM;
	lvi = &ivd->_impl_use.lv_info;

	udf_inittag(&ivd->tag, TAGID_IMP_VOL, /* loc */ 0);
	ivd->seq_num  = udf_rw32(context.vds_seq); context.vds_seq++;

	udf_set_regid(&ivd->impl_id, "*UDF LV Info");
	udf_add_udf_regid(&ivd->impl_id);

	/* fill in UDF specific part */
	udf_osta_charset(&lvi->lvi_charset);
	udf_encode_osta_id(lvi->logvol_id, 128, context.logvol_name);

	udf_encode_osta_id(lvi->lvinfo1, 36, field1);
	udf_encode_osta_id(lvi->lvinfo2, 36, field2);
	udf_encode_osta_id(lvi->lvinfo3, 36, field3);

	udf_set_regid(&lvi->impl_id, context.impl_name);
	udf_add_impl_regid(&lvi->impl_id);

	crclen  = sizeof(struct impvol_desc) - UDF_DESC_TAG_LENGTH;
	ivd->tag.desc_crc_len = udf_rw16(crclen);

	context.implementation = ivd;

	return 0;
}


/* XXX might need to be sanitised a bit later */
void
udf_update_lvintd(int type)
{
	struct logvol_int_desc *lvid;
	struct udf_logvol_info *lvinfo;
	struct logvol_desc     *logvol;
	uint32_t *pos;
	int crclen, cnt, l_iu;
	int num_partmappings;

	lvid   = context.logvol_integrity;
	logvol = context.logical_vol;

	assert(lvid);
	assert(logvol);

	lvid->integrity_type = udf_rw16(type);

	num_partmappings = udf_rw32(logvol->n_pm);

	udf_set_timestamp_now(&lvid->time);

	lvinfo = (struct udf_logvol_info *)
		(lvid->tables + num_partmappings * 2);
	udf_set_regid(&lvinfo->impl_id, context.impl_name);
	udf_add_impl_regid(&lvinfo->impl_id);

	lvinfo->num_files          = udf_rw32(context.num_files);
	lvinfo->num_directories    = udf_rw32(context.num_directories);

	lvid->lvint_next_unique_id = udf_rw64(context.unique_id);

	/* XXX sane enough ? */
	lvinfo->min_udf_readver  = udf_rw16(context.min_udf);
	lvinfo->min_udf_writever = udf_rw16(context.min_udf);
	lvinfo->max_udf_writever = udf_rw16(context.max_udf);

	lvid->num_part = udf_rw32(num_partmappings);

	/* no impl. use needed */
	l_iu = sizeof(struct udf_logvol_info);
	lvid->l_iu = udf_rw32(l_iu);

	pos = &lvid->tables[0];
	for (cnt = 0; cnt < num_partmappings; cnt++) {
		*pos++ = udf_rw32(context.part_free[cnt]);
	}
	for (cnt = 0; cnt < num_partmappings; cnt++) {
		*pos++ = udf_rw32(context.part_size[cnt]);
	}

	crclen  = sizeof(struct logvol_int_desc) -4 -UDF_DESC_TAG_LENGTH + l_iu;
	crclen += num_partmappings * 2 * 4;
	lvid->tag.desc_crc_len = udf_rw16(crclen);

	context.logvol_info = lvinfo;
}


int
udf_create_lvintd(int type)
{
	struct logvol_int_desc *lvid;

	lvid = calloc(1, context.sector_size);
	if (lvid == NULL)
		return ENOMEM;

	udf_inittag(&lvid->tag, TAGID_LOGVOL_INTEGRITY, /* loc */ 0);

	context.logvol_integrity = lvid;

	udf_update_lvintd(type);

	return 0;
}


int
udf_create_fsd(void)
{
	struct fileset_desc *fsd;
	int crclen;

	fsd = calloc(1, context.sector_size);
	if (fsd == NULL)
		return ENOMEM;

	udf_inittag(&fsd->tag, TAGID_FSD, /* loc */ 0);

	udf_set_timestamp_now(&fsd->time);
	fsd->ichg_lvl     = udf_rw16(3);		/* UDF 2.3.2.1 */
	fsd->max_ichg_lvl = udf_rw16(3);		/* UDF 2.3.2.2 */

	fsd->charset_list     = udf_rw32(1);		/* only CS0 */
	fsd->max_charset_list = udf_rw32(1);		/* only CS0 */

	fsd->fileset_num      = udf_rw32(0);		/* only one fsd */
	fsd->fileset_desc_num = udf_rw32(0);		/* origional    */

	udf_osta_charset(&fsd->logvol_id_charset);
	udf_encode_osta_id(fsd->logvol_id, 128, context.logvol_name);

	udf_osta_charset(&fsd->fileset_charset);
	udf_encode_osta_id(fsd->fileset_id, 32, context.fileset_name);

	/* copyright file and abstract file names obmitted */

	fsd->rootdir_icb.len	      = udf_rw32(context.sector_size);
	fsd->rootdir_icb.loc.lb_num   = udf_rw32(layout.rootdir);
	fsd->rootdir_icb.loc.part_num = udf_rw16(context.metadata_part);

	udf_set_regid(&fsd->domain_id, "*OSTA UDF Compliant");
	udf_add_domain_regid(&fsd->domain_id);

	/* next_ex stays zero */
	/* no system streamdirs yet */

	crclen = sizeof(struct fileset_desc) - UDF_DESC_TAG_LENGTH;
	fsd->tag.desc_crc_len = udf_rw16(crclen);

	context.fileset_desc = fsd;

	return 0;
}


int
udf_create_space_bitmap(uint32_t dscr_size, uint32_t part_size_lba,
	struct space_bitmap_desc **sbdp)
{
	struct space_bitmap_desc *sbd;
	int crclen, cnt;

	*sbdp = NULL;
	sbd = calloc(context.sector_size, dscr_size);
	if (sbd == NULL)
		return ENOMEM;

	udf_inittag(&sbd->tag, TAGID_SPACE_BITMAP, /* loc */ 0);

	sbd->num_bits  = udf_rw32(part_size_lba);
	sbd->num_bytes = udf_rw32((part_size_lba + 7)/8);

	/* fill space with 0xff to indicate free */
	for (cnt = 0; cnt < udf_rw32(sbd->num_bytes); cnt++)
		sbd->data[cnt] = 0xff;

	/* set crc to only cover the header (UDF 2.3.1.2, 2.3.8.1) */
	crclen = sizeof(struct space_bitmap_desc) -1 - UDF_DESC_TAG_LENGTH;
	sbd->tag.desc_crc_len = udf_rw16(crclen);

	*sbdp = sbd;
	return 0;
}


/* --------------------------------------------------------------------- */

int 
udf_register_bad_block(uint32_t location)
{
	struct udf_sparing_table *spt;
	struct spare_map_entry   *sme, *free_sme;
	int cnt;

	spt = context.sparing_table;
	if (spt == NULL) {
		printf("internal error: adding bad block to non sparable\n");
		return EINVAL;
	}

	/* find us a free spare map entry */
	free_sme = NULL;
	for (cnt = 0; cnt < layout.sparable_blocks; cnt++) {
		sme = &spt->entries[cnt];
		/* if we are allready in it, bail out */
		if (udf_rw32(sme->org) == location)
			return 0;
		if (udf_rw32(sme->org) == 0xffffffff) {
			free_sme = sme;
			break;
		}
	}
	if (free_sme == NULL) {
		printf("Disc relocation blocks full; disc too damanged\n");
		return EINVAL;
	}
	free_sme->org = udf_rw32(location);

	return 0;
}


void
udf_mark_allocated(uint32_t start_lb, int partnr, uint32_t blocks)
{
	union dscrptr *dscr;
	uint8_t *bpos;
	uint32_t cnt, bit;

	/* make not on space used on underlying partition */
	context.part_free[partnr] -= blocks;
#ifdef DEBUG
	printf("mark allocated : partnr %d, start_lb %d for %d blocks\n",
		partnr, start_lb, blocks);
#endif

	switch (context.vtop_tp[partnr]) {
	case UDF_VTOP_TYPE_VIRT:
		/* nothing */
		break;
	case UDF_VTOP_TYPE_PHYS:
	case UDF_VTOP_TYPE_SPARABLE:
	case UDF_VTOP_TYPE_META:
#ifdef DEBUG
		printf("Marking %d+%d as used\n", start_lb, blocks);
#endif
		dscr = (union dscrptr *) (context.part_unalloc_bits[partnr]);
		for (cnt = start_lb; cnt < start_lb + blocks; cnt++) {
			 bpos  = &dscr->sbd.data[cnt / 8];
			 bit   = cnt % 8;
			*bpos &= ~(1<< bit);
		}
		break;
	default:
		printf("internal error: reality check in mapping type %d\n",
			context.vtop_tp[partnr]);
		exit(EXIT_FAILURE);
	}
}


/* --------------------------------------------------------------------- */

static void
udf_advance_uniqueid(void)
{
	/* Minimum value of 16 : UDF 3.2.1.1, 3.3.3.4. */
	context.unique_id++;
	if (context.unique_id < 0x10)
		context.unique_id = 0x10;
}


static int
udf_create_parentfid(struct fileid_desc *fid, struct long_ad *parent,
	uint64_t unique_id)
{
	/* the size of an empty FID is 38 but needs to be a multiple of 4 */
	int fidsize = 40;

	udf_inittag(&fid->tag, TAGID_FID, udf_rw32(parent->loc.lb_num));
	fid->file_version_num = udf_rw16(1);	/* UDF 2.3.4.1 */
	fid->file_char = UDF_FILE_CHAR_DIR | UDF_FILE_CHAR_PAR;
	fid->icb = *parent;
	fid->icb.longad_uniqueid = udf_rw32((uint32_t) unique_id);
	fid->tag.desc_crc_len = fidsize - UDF_DESC_TAG_LENGTH;

	/* we have to do the fid here explicitly for simplicity */
	udf_validate_tag_and_crc_sums((union dscrptr *) fid);

	return fidsize;
}


/*
 * Order of extended attributes :
 *   ECMA 167 EAs
 *   Non block aligned Implementation Use EAs
 *   Block aligned Implementation Use EAs	(not in newfs_udf)
 *   Application Use EAs			(not in newfs_udf)
 *
 *   no checks for doubles, must be called in-order
 */
static void
udf_extattr_append_internal(union dscrptr *dscr, struct extattr_entry *extattr)
{
	struct file_entry      *fe;
	struct extfile_entry   *efe;
	struct extattrhdr_desc *extattrhdr;
	struct impl_extattr_entry *implext;
	uint32_t impl_attr_loc, appl_attr_loc, l_ea, a_l, exthdr_len;
	uint32_t *l_eap, l_ad;
	uint16_t *spos;
	uint8_t *bpos, *data;

	if (udf_rw16(dscr->tag.id) == TAGID_FENTRY) {
		fe    = &dscr->fe;
		data  = fe->data;
		l_eap = &fe->l_ea;
		l_ad  = udf_rw32(fe->l_ad);
	} else if (udf_rw16(dscr->tag.id) == TAGID_EXTFENTRY) {
		efe   = &dscr->efe;
		data  = efe->data;
		l_eap = &efe->l_ea;
		l_ad  = udf_rw32(efe->l_ad);
	} else {
		errx(1, "Bad tag passed to udf_extattr_append_internal");
	}

	/* should have a header! */
	extattrhdr = (struct extattrhdr_desc *) data;
	l_ea = udf_rw32(*l_eap);
	if (l_ea == 0) {
		assert(l_ad == 0);
		/* create empty extended attribute header */
		exthdr_len = sizeof(struct extattrhdr_desc);

		udf_inittag(&extattrhdr->tag, TAGID_EXTATTR_HDR, /* loc */ 0);
		extattrhdr->impl_attr_loc = udf_rw32(exthdr_len);
		extattrhdr->appl_attr_loc = udf_rw32(exthdr_len);
		extattrhdr->tag.desc_crc_len = udf_rw16(8);

		/* record extended attribute header length */
		l_ea = exthdr_len;
		*l_eap = udf_rw32(l_ea);
	}

	/* extract locations */
	impl_attr_loc = udf_rw32(extattrhdr->impl_attr_loc);
	appl_attr_loc = udf_rw32(extattrhdr->appl_attr_loc);
	if (impl_attr_loc == UDF_IMPL_ATTR_LOC_NOT_PRESENT)
		impl_attr_loc = l_ea;
	if (appl_attr_loc == UDF_IMPL_ATTR_LOC_NOT_PRESENT)
		appl_attr_loc = l_ea;

	/* Ecma 167 EAs */
	if (udf_rw32(extattr->type) < 2048) {
		assert(impl_attr_loc == l_ea);
		assert(appl_attr_loc == l_ea);
	}

	/* implementation use extended attributes */
	if (udf_rw32(extattr->type) == 2048) {
		assert(appl_attr_loc == l_ea);

		/* calculate and write extended attribute header checksum */
		implext = (struct impl_extattr_entry *) extattr;
		assert(udf_rw32(implext->iu_l) == 4);	/* [UDF 3.3.4.5] */
		spos = (uint16_t *) implext->data;
		*spos = udf_rw16(udf_ea_cksum((uint8_t *) implext));
	}

	/* application use extended attributes */
	assert(udf_rw32(extattr->type) != 65536);
	assert(appl_attr_loc == l_ea);

	/* append the attribute at the end of the current space */
	bpos = data + udf_rw32(*l_eap);
	a_l  = udf_rw32(extattr->a_l);

	/* update impl. attribute locations */
	if (udf_rw32(extattr->type) < 2048) {
		impl_attr_loc = l_ea + a_l;
		appl_attr_loc = l_ea + a_l;
	}
	if (udf_rw32(extattr->type) == 2048) {
		appl_attr_loc = l_ea + a_l;
	}

	/* copy and advance */
	memcpy(bpos, extattr, a_l);
	l_ea += a_l;
	*l_eap = udf_rw32(l_ea);

	/* do the `dance` again backwards */
	if (context.dscrver != 2) {
		if (impl_attr_loc == l_ea)
			impl_attr_loc = UDF_IMPL_ATTR_LOC_NOT_PRESENT;
		if (appl_attr_loc == l_ea)
			appl_attr_loc = UDF_APPL_ATTR_LOC_NOT_PRESENT;
	}

	/* store offsets */
	extattrhdr->impl_attr_loc = udf_rw32(impl_attr_loc);
	extattrhdr->appl_attr_loc = udf_rw32(appl_attr_loc);

	/* make sure the header sums stays correct */
	udf_validate_tag_and_crc_sums((union dscrptr *) extattrhdr);
}


int
udf_create_new_fe(struct file_entry **fep, int file_type,
	struct long_ad *parent_icb)
{
	struct file_entry      *fe;
	struct icb_tag         *icb;
	struct timestamp        birthtime;
	struct filetimes_extattr_entry *ft_extattr;
	uint32_t fidsize;
	uint8_t *bpos;
	int crclen;

	*fep = NULL;
	fe = calloc(1, context.sector_size);
	if (fe == NULL)
		return ENOMEM;

	udf_inittag(&fe->tag, TAGID_FENTRY, /* loc */ 0);
	icb = &fe->icbtag;

	/*
	 * Always use strategy type 4 unless on WORM wich we don't support
	 * (yet). Fill in defaults and set for internal allocation of data.
	 */
	icb->strat_type      = udf_rw16(4);
	icb->max_num_entries = udf_rw16(1);
	icb->file_type       = file_type;	/* 8 bit */
	icb->flags           = udf_rw16(UDF_ICB_INTERN_ALLOC);

	fe->perm     = udf_rw32(0x7fff);	/* all is allowed   */
	fe->link_cnt = udf_rw16(0);		/* explicit setting */

	fe->ckpoint  = udf_rw32(1);		/* user supplied file version */
	udf_set_timestamp_now(&birthtime);
	udf_set_timestamp_now(&fe->atime);
	udf_set_timestamp_now(&fe->attrtime);
	udf_set_timestamp_now(&fe->mtime);

	udf_set_regid(&fe->imp_id, context.impl_name);
	udf_add_impl_regid(&fe->imp_id);
	fe->unique_id = udf_rw64(context.unique_id);
	fe->l_ea = udf_rw32(0);

	/* create extended attribute to record our creation time */
	ft_extattr = calloc(1, UDF_FILETIMES_ATTR_SIZE(1));
	ft_extattr->hdr.type = udf_rw32(UDF_FILETIMES_ATTR_NO);
	ft_extattr->hdr.subtype = 1;	/* [4/48.10.5] */
	ft_extattr->hdr.a_l = udf_rw32(UDF_FILETIMES_ATTR_SIZE(1));
	ft_extattr->d_l     = udf_rw32(UDF_TIMESTAMP_SIZE); /* one item */
	ft_extattr->existence = UDF_FILETIMES_FILE_CREATION;
	ft_extattr->times[0]  = birthtime;

	udf_extattr_append_internal((union dscrptr *) fe,
		(struct extattr_entry *) ft_extattr);
	free(ft_extattr);

	/* if its a directory, create '..' */
	bpos = (uint8_t *) fe->data + udf_rw32(fe->l_ea);
	fidsize = 0;
	if (file_type == UDF_ICB_FILETYPE_DIRECTORY) {
		fidsize = udf_create_parentfid((struct fileid_desc *) bpos,
			parent_icb, context.unique_id);
	}
	udf_advance_uniqueid();

	/* record fidlength information */
	fe->inf_len = udf_rw64(fidsize);
	fe->l_ad    = udf_rw32(fidsize);
	fe->logblks_rec = udf_rw64(0);		/* intern */

	crclen  = sizeof(struct file_entry) - 1 - UDF_DESC_TAG_LENGTH;
	crclen += udf_rw32(fe->l_ea) + fidsize;
	fe->tag.desc_crc_len = udf_rw16(crclen);

	*fep = fe;
	return 0;
}


int
udf_create_new_efe(struct extfile_entry **efep, int file_type,
	struct long_ad *parent_icb)
{
	struct extfile_entry *efe;
	struct icb_tag       *icb;
	uint32_t fidsize;
	int crclen;

	*efep = NULL;
	efe = calloc(1, context.sector_size);
	if (efe == NULL)
		return ENOMEM;

	udf_inittag(&efe->tag, TAGID_EXTFENTRY, /* loc */ 0);
	icb = &efe->icbtag;

	/*
	 * Always use strategy type 4 unless on WORM wich we don't support
	 * (yet). Fill in defaults and set for internal allocation of data.
	 */
	icb->strat_type      = udf_rw16(4);
	icb->max_num_entries = udf_rw16(1);
	icb->file_type       = file_type;	/* 8 bit */
	icb->flags = udf_rw16(UDF_ICB_INTERN_ALLOC);

	efe->perm     = udf_rw32(0x7fff);	/* all is allowed   */
	efe->link_cnt = udf_rw16(0);		/* explicit setting */

	efe->ckpoint  = udf_rw32(1);		/* user supplied file version */
	udf_set_timestamp_now(&efe->ctime);
	udf_set_timestamp_now(&efe->atime);
	udf_set_timestamp_now(&efe->attrtime);
	udf_set_timestamp_now(&efe->mtime);

	udf_set_regid(&efe->imp_id, context.impl_name);
	udf_add_impl_regid(&efe->imp_id);

	fidsize = 0;
	efe->unique_id = udf_rw64(context.unique_id);
	if (file_type == UDF_ICB_FILETYPE_DIRECTORY) {
		fidsize = udf_create_parentfid((struct fileid_desc *) efe->data,
			parent_icb, context.unique_id);
	}
	udf_advance_uniqueid();

	/* record fidlength information */
	efe->inf_len  = udf_rw64(fidsize);
	efe->obj_size = udf_rw64(fidsize);
	efe->l_ad     = udf_rw32(fidsize);
	efe->logblks_rec = udf_rw64(0);

	crclen  = sizeof(struct extfile_entry) - 1 - UDF_DESC_TAG_LENGTH;
	crclen += fidsize;
	efe->tag.desc_crc_len = udf_rw16(crclen);

	*efep = efe;
	return 0;
}

/* --------------------------------------------------------------------- */

static void
udf_append_mapping_part_to_efe(struct extfile_entry *efe, struct short_ad *mapping)
{
	struct icb_tag *icb;
	uint64_t inf_len, obj_size, logblks_rec;
	uint32_t l_ad, l_ea;
	uint16_t crclen;
	uint8_t *bpos;

	inf_len     = udf_rw64(efe->inf_len);
	obj_size    = udf_rw64(efe->obj_size);
	logblks_rec = udf_rw64(efe->logblks_rec);
	l_ad   = udf_rw32(efe->l_ad);
	l_ea   = udf_rw32(efe->l_ea);
	crclen = udf_rw16(efe->tag.desc_crc_len);
	icb    = &efe->icbtag;

	/* set our allocation to shorts if not already done */
	icb->flags = udf_rw16(UDF_ICB_SHORT_ALLOC);

	/* append short_ad */
	bpos = (uint8_t *) efe->data + l_ea + l_ad;
	memcpy(bpos, mapping, sizeof(struct short_ad));

	l_ad   += sizeof(struct short_ad);
	crclen += sizeof(struct short_ad);
	inf_len  += UDF_EXT_LEN(udf_rw32(mapping->len));
	obj_size += UDF_EXT_LEN(udf_rw32(mapping->len));
	logblks_rec = UDF_ROUNDUP(inf_len, context.sector_size) /
				context.sector_size;

	efe->l_ad = udf_rw32(l_ad);
	efe->inf_len     = udf_rw64(inf_len);
	efe->obj_size    = udf_rw64(obj_size);
	efe->logblks_rec = udf_rw64(logblks_rec);
	efe->tag.desc_crc_len = udf_rw16(crclen);
}


static void
udf_append_meta_mapping_to_efe(struct extfile_entry *efe, uint16_t partnr, uint32_t lb_num,
	uint64_t len)
{
	struct short_ad mapping;
	uint64_t max_len, part_len;

	/* calculate max length meta allocation sizes */
	max_len = UDF_EXT_MAXLEN / context.sector_size; /* in sectors */
	max_len = (max_len / layout.meta_blockingnr) * layout.meta_blockingnr;
	max_len = max_len * context.sector_size;

	memset(&mapping, 0, sizeof(struct short_ad));
	while (len) {
		part_len = MIN(len, max_len);
		mapping.lb_num   = udf_rw32(lb_num);
		mapping.len      = udf_rw32(part_len);

		udf_append_mapping_part_to_efe(efe, &mapping);

		lb_num += part_len / context.sector_size;
		len    -= part_len;
	}
}


int
udf_create_meta_files(void)
{
	struct extfile_entry *efe;
	struct long_ad meta_icb;
	uint64_t bytes;
	uint32_t sector_size;
	int filetype, error;

	sector_size = context.sector_size;

	bzero(&meta_icb, sizeof(struct long_ad));
	meta_icb.len          = udf_rw32(sector_size);
	meta_icb.loc.part_num = udf_rw16(context.data_part);

	/* create metadata file */
	meta_icb.loc.lb_num   = udf_rw32(layout.meta_file);
	filetype = UDF_ICB_FILETYPE_META_MAIN;
	error = udf_create_new_efe(&efe, filetype, &meta_icb);
	if (error)
		return error;
	context.meta_file = efe;

	/* create metadata mirror file */
	meta_icb.loc.lb_num   = udf_rw32(layout.meta_mirror);
	filetype = UDF_ICB_FILETYPE_META_MIRROR;
	error = udf_create_new_efe(&efe, filetype, &meta_icb);
	if (error)
		return error;
	context.meta_mirror = efe;

	/* create metadata bitmap file */
	meta_icb.loc.lb_num   = udf_rw32(layout.meta_bitmap);
	filetype = UDF_ICB_FILETYPE_META_BITMAP;
	error = udf_create_new_efe(&efe, filetype, &meta_icb);
	if (error)
		return error;
	context.meta_bitmap = efe;

	/* patch up files */
	context.meta_file->unique_id   = udf_rw64(0);
	context.meta_mirror->unique_id = udf_rw64(0);
	context.meta_bitmap->unique_id = udf_rw64(0);

	/* restart unique id */
	context.unique_id = 0x10;

	/* XXX no support for metadata mirroring yet */
	/* insert extents */
	efe = context.meta_file;
	udf_append_meta_mapping_to_efe(efe, context.data_part,
		layout.meta_part_start_lba,
		(uint64_t) layout.meta_part_size_lba * sector_size);

	efe = context.meta_mirror;
	udf_append_meta_mapping_to_efe(efe, context.data_part,
		layout.meta_part_start_lba,
		(uint64_t) layout.meta_part_size_lba * sector_size);

	efe = context.meta_bitmap;
	bytes = udf_space_bitmap_len(layout.meta_part_size_lba);
	udf_append_meta_mapping_to_efe(efe, context.data_part,
		layout.meta_bitmap_space, bytes);

	return 0;
}


/* --------------------------------------------------------------------- */

int
udf_create_new_rootdir(union dscrptr **dscr)
{
	struct file_entry *fe;
	struct extfile_entry *efe;
	struct long_ad root_icb;
	int filetype, error;

	bzero(&root_icb, sizeof(struct long_ad));
	root_icb.len          = udf_rw32(context.sector_size);
	root_icb.loc.lb_num   = udf_rw32(layout.rootdir);
	root_icb.loc.part_num = udf_rw16(context.metadata_part);

	filetype = UDF_ICB_FILETYPE_DIRECTORY;
	if (context.dscrver == 2) {
		error = udf_create_new_fe(&fe, filetype, &root_icb);
		*dscr = (union dscrptr *) fe;
	} else {
		error = udf_create_new_efe(&efe, filetype, &root_icb);
		*dscr = (union dscrptr *) efe;
	}
	if (error)
		return error;

	/* Rootdir has explicit only one link on creation; '..' is no link */
	if (context.dscrver == 2) {
		fe->link_cnt  = udf_rw16(1);
	} else {
		efe->link_cnt = udf_rw16(1);
	}

	context.num_directories++;
	assert(context.num_directories == 1);

	return 0;
}


int
udf_create_new_VAT(union dscrptr **vat_dscr)
{
	struct file_entry *fe;
	struct extfile_entry *efe;
	struct impl_extattr_entry *implext;
	struct vatlvext_extattr_entry *vatlvext;
	struct udf_oldvat_tail *oldvat_tail;
	struct udf_vat *vathdr;
	uint32_t *vat_pos;
	uint8_t *bpos, *extattr;
	int inf_len, ea_len, vat_len, filetype;
	int error;

	assert((layout.rootdir < 2) && (layout.fsd < 2));
	if (context.dscrver == 2) {
		/* old style VAT */
		filetype = UDF_ICB_FILETYPE_UNKNOWN;
		error = udf_create_new_fe(&fe, filetype, NULL);
		if (error)
			return error;

		/* append VAT LVExtension attribute */
		ea_len = sizeof(struct impl_extattr_entry) - 1 +
			 sizeof(struct vatlvext_extattr_entry) + 4;

		extattr = calloc(1, ea_len);

		implext  = (struct impl_extattr_entry *) extattr;
		implext->hdr.type = udf_rw32(2048);	/* [4/48.10.8] */
		implext->hdr.subtype = 1;		/* [4/48.10.8.2] */
		implext->hdr.a_l = udf_rw32(ea_len);	/* VAT LVext EA size */
		/* use 4 bytes of imp use for UDF checksum [UDF 3.3.4.5] */
		implext->iu_l = udf_rw32(4);
		udf_set_regid(&implext->imp_id, "*UDF VAT LVExtension");
		udf_add_udf_regid(&implext->imp_id);

		/* VAT LVExtension data follows UDF IU space */
		bpos = ((uint8_t *) implext->data) + 4;
		vatlvext = (struct vatlvext_extattr_entry *) bpos;

		vatlvext->unique_id_chk = udf_rw64(fe->unique_id);
		vatlvext->num_files = udf_rw32(context.num_files);
		vatlvext->num_directories = udf_rw32(context.num_directories);
		memcpy(vatlvext->logvol_id, context.logical_vol->logvol_id, 128);

		udf_extattr_append_internal((union dscrptr *) fe,
			(struct extattr_entry *) extattr);

		free(extattr);

		/* writeout VAT locations (partion offsets) */
		vat_pos = (uint32_t *) (fe->data + udf_rw32(fe->l_ea));
		vat_pos[layout.rootdir] = udf_rw32(layout.rootdir); 
		vat_pos[layout.fsd    ] = udf_rw32(layout.fsd);

		/* Append "*UDF Virtual Alloc Tbl" id and prev. VAT location */
		oldvat_tail = (struct udf_oldvat_tail *) (vat_pos + 2);
		udf_set_regid(&oldvat_tail->id, "*UDF Virtual Alloc Tbl");
		udf_add_udf_regid(&oldvat_tail->id);
		oldvat_tail->prev_vat = udf_rw32(UDF_NO_PREV_VAT);

		/* set length */
		inf_len = 2 * 4 + sizeof(struct udf_oldvat_tail);
		fe->inf_len = udf_rw64(inf_len);
		fe->l_ad    = udf_rw32(inf_len);

		/* update vat descriptor's CRC length */
		vat_len = inf_len + udf_rw32(fe->l_ea) +
			sizeof(struct file_entry) - 1 - UDF_DESC_TAG_LENGTH;
		fe->tag.desc_crc_len = udf_rw16(vat_len);

		*vat_dscr = (union dscrptr *) fe;
	} else {
		/* new style VAT */
		filetype = UDF_ICB_FILETYPE_VAT;
		error = udf_create_new_efe(&efe, filetype, NULL);
		if (error)
			return error;

		/* set up VATv2 descriptor */
		vathdr = (struct udf_vat *) efe->data;
		vathdr->header_len      = udf_rw16(sizeof(struct udf_vat) - 1);
		vathdr->impl_use_len    = udf_rw16(0);
		memcpy(vathdr->logvol_id, context.logical_vol->logvol_id, 128);
		vathdr->prev_vat        = udf_rw32(UDF_NO_PREV_VAT);
		vathdr->num_files       = udf_rw32(context.num_files);
		vathdr->num_directories = udf_rw32(context.num_directories);

		vathdr->min_udf_readver  = udf_rw16(context.min_udf);
		vathdr->min_udf_writever = udf_rw16(context.min_udf);
		vathdr->max_udf_writever = udf_rw16(context.max_udf);

		/* writeout VAT locations */
		vat_pos = (uint32_t *) vathdr->data;
		vat_pos[layout.rootdir] = udf_rw32(layout.rootdir);
		vat_pos[layout.fsd    ] = udf_rw32(layout.fsd);

		/* set length */
		inf_len  = 2 * 4 + sizeof(struct udf_vat) - 1;
		efe->inf_len     = udf_rw64(inf_len);
		efe->obj_size    = udf_rw64(inf_len);
		efe->l_ad        = udf_rw32(inf_len);
		efe->logblks_rec = udf_rw32(0);

		vat_len  = sizeof(struct extfile_entry)-1 - UDF_DESC_TAG_LENGTH;
		vat_len += inf_len;
		efe->tag.desc_crc_len = udf_rw16(vat_len);

		*vat_dscr = (union dscrptr *) efe;
	}
	
	return 0;
}

