/*	$NetBSD: powernow.h,v 1.9 2007/03/24 15:35:15 xtraeme Exp $	*/

/*-
 * Copyright (c) 2004 Martin V�giard.
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 */

/*-
 * Copyright (c) 2004-2005 Bruno Ducrot
 * Copyright (c) 2004 FUKUDA Nobuhiko <nfukuda@spa.is.uec.ac.jp>
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
 */

#ifndef X86_POWERNOW_H
#define X86_POWERNOW_H

#ifdef POWERNOW_DEBUG
#define DPRINTF(x)		do { printf x; } while (0)
#else
#define DPRINTF(x)
#endif

#define BIOS_START			0xe0000
#define BIOS_LEN			0x20000
#define BIOS_STEP			16

/*
 * MSRs and bits used by Powernow technology
 */
#define MSR_AMDK7_FIDVID_CTL		0xc0010041
#define MSR_AMDK7_FIDVID_STATUS		0xc0010042
#define AMD_PN_FID_VID			0x06
#define AMD_ERRATA_A0_CPUSIG		0x660

#define PN7_FLAG_ERRATA_A0		0x01
#define PN7_FLAG_DESKTOP_VRM		0x02

/* Bitfields used by K7 */
#define PN7_PSB_VERSION			0x12
#define PN7_CTR_FID(x)			((x) & 0x1f)
#define PN7_CTR_VID(x)			(((x) & 0x1f) << 8)
#define PN7_CTR_FIDC			0x00010000
#define PN7_CTR_VIDC			0x00020000
#define PN7_CTR_FIDCHRATIO		0x00100000
#define PN7_CTR_SGTC(x)			(((uint64_t)(x) & 0x000fffff) << 32)

#define PN7_STA_CFID(x)			((x) & 0x1f)
#define PN7_STA_SFID(x)			(((x) >> 8) & 0x1f)
#define PN7_STA_MFID(x)			(((x) >> 16) & 0x1f)
#define PN7_STA_CVID(x)			(((x) >> 32) & 0x1f)
#define PN7_STA_SVID(x)			(((x) >> 40) & 0x1f)
#define PN7_STA_MVID(x)			(((x) >> 48) & 0x1f)

/*
 * ACPI ctr_val status register to powernow k7 configuration
 */
#define PN7_ACPI_CTRL_TO_VID(x)		(((x) >> 5) & 0x1f)
#define PN7_ACPI_CTRL_TO_SGTC(x)	(((x) >> 10) & 0xffff)

/* Bitfields used by K8 */
#define PN8_CTR_FID(x)			((x) & 0x3f)
#define PN8_CTR_VID(x)			(((x) & 0x1f) << 8)
#define PN8_CTR_PENDING(x)		(((x) & 1) << 32)

#define PN8_STA_CFID(x)			((x) & 0x3f)
#define PN8_STA_SFID(x)			(((x) >> 8) & 0x3f)
#define PN8_STA_MFID(x)			(((x) >> 16) & 0x3f)
#define PN8_STA_PENDING(x)		(((x) >> 31) & 0x01)
#define PN8_STA_CVID(x)			(((x) >> 32) & 0x1f)
#define PN8_STA_SVID(x)			(((x) >> 40) & 0x1f)
#define PN8_STA_MVID(x)			(((x) >> 48) & 0x1f)

#define COUNT_OFF_IRT(irt)		DELAY(10 * (1 << (irt)))
#define COUNT_OFF_VST(vst)		DELAY(20 * (vst))

#define FID_TO_VCO_FID(fid)	\
	(((fid) < 8) ? (8 + ((fid) << 1)) : (fid))

/* Reserved1 to powernow k8 configuration */
#define PN8_PSB_TO_RVO(x)		((x) & 0x03)
#define PN8_PSB_TO_IRT(x)		(((x) >> 2) & 0x03)
#define PN8_PSB_TO_MVS(x)		(((x) >> 4) & 0x03)
#define PN8_PSB_TO_BATT(x)		(((x) >> 6) & 0x03)

/* ACPI ctr_val status register to powernow k8 configuration */
#define ACPI_PN8_CTRL_TO_FID(x)		((x) & 0x3f)
#define ACPI_PN8_CTRL_TO_VID(x)		(((x) >> 6) & 0x1f)
#define ACPI_PN8_CTRL_TO_VST(x)		(((x) >> 11) & 0x1f)
#define ACPI_PN8_CTRL_TO_MVS(x)		(((x) >> 18) & 0x03)
#define ACPI_PN8_CTRL_TO_PLL(x)		(((x) >> 20) & 0x7f)
#define ACPI_PN8_CTRL_TO_RVO(x)		(((x) >> 28) & 0x03)
#define ACPI_PN8_CTRL_TO_IRT(x)		(((x) >> 30) & 0x03)

#define POWERNOW_MAX_STATES		16

struct powernow_state {
	int freq;
	uint8_t fid;
	uint8_t vid;
};

struct powernow_cpu_state {
	struct powernow_state state_table[POWERNOW_MAX_STATES];
	unsigned int fsb;
	unsigned int n_states;
	unsigned int sgtc;
	unsigned int vst;
	unsigned int mvs;
	unsigned int pll;
	unsigned int rvo;
	unsigned int irt;
	int low;
};

struct powernow_psb_s {
	char signature[10];	/* AMDK7PNOW! */
	uint8_t version;
	uint8_t flags;
	uint16_t ttime;		/* Min settling time */
	uint8_t reserved;
	uint8_t n_pst;
};

struct powernow_pst_s {
	uint32_t signature;
	uint8_t pll;
	uint8_t fid;
	uint8_t vid;
	uint8_t n_states;
};

int powernow_probe(struct cpu_info *);

/* i386/i386/powernow_k7.c */
void k7_powernow_init(void);
void k7_powernow_destroy(void);

/* x86/x86/powernow_k8.c */
void k8_powernow_init(void);
void k8_powernow_destroy(void);

#endif
