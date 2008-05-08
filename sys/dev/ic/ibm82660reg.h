/* $NetBSD: ibm82660reg.h,v 1.2 2008/04/28 20:23:50 martin Exp $ */

/*-
 * Copyright (c) 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Tim Rightnour
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

#ifndef _DEV_IC_IBM82660REG_H_
#define _DEV_IC_IBM82660REG_H_

/* Register definitions for the IBM 82660 PCI Bridge Controller.
 * Also known as a Lanai/Kauai.
 */

#define IBM_82660_CACHE_STATUS          0xB1
#define IBM_82660_CACHE_STATUS_L1_EN    0x01
#define IBM_82660_CACHE_STATUS_L2_EN    0x02

#define IBM_82660_OPTIONS_1             0xBA
#define IBM_82660_OPTIONS_1_MCP         0x01
#define IBM_82660_OPTIONS_1_TEA         0x02
#define IBM_82660_OPTIONS_1_ISA         0x04

#define IBM_82660_OPTIONS_3             0xD4
#define IBM_82660_OPTIONS_3_ECC         0x01
#define IBM_82660_OPTIONS_3_DRAM        0x04
#define IBM_82660_OPTIONS_3_SRAM        0x08
#define IBM_82660_OPTIONS_3_SNOOP       0x80

#define IBM_82660_SYSTEM_CTRL           0x81C
#define IBM_82660_SYSTEM_CTRL_L2_EN     0x40
#define IBM_82660_SYSTEM_CTRL_L2_MI     0x80

#endif /* _DEV_IC_IBM82660REG_H_ */
