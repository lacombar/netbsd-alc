/* $NetBSD: udf_bswap.h,v 1.5 2008/05/14 16:49:48 reinoud Exp $	*/

/*
 * Copyright (c) 1998 Manuel Bouyer.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
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
 * adapted for UDF by Reinoud Zandijk <reinoud@netbsd.org>
 *
 */

#ifndef _FS_UDF_UDF_BSWAP_H_
#define _FS_UDF_UDF_BSWAP_H_

#if HAVE_ENDIAN_H
#include <endian.h>
#else
#if HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#else
#if HAVE_MACHINE_ENDIAN_H
#include <machine/endian.h>
#endif
#endif
#endif

/* rest only relevant for big endian machines */
#if (BYTE_ORDER == BIG_ENDIAN)

#include <machine/bswap.h>
#include <sys/bswap.h>

/* inlines for access to swapped data */
static __inline u_int16_t udf_rw16(u_int16_t);
static __inline u_int32_t udf_rw32(u_int32_t);
static __inline u_int64_t udf_rw64(u_int64_t);


static __inline u_int16_t
udf_rw16(a)
	u_int16_t a;
{
	return bswap16(a);
}


static __inline u_int32_t
udf_rw32(a)
	u_int32_t a;
{
	return bswap32(a);
}


static __inline u_int64_t
udf_rw64(a)
	u_int64_t a;
{
	return bswap64(a);
}

#else

#define udf_rw16(a) ((uint16_t)(a))
#define udf_rw32(a) ((uint32_t)(a))
#define udf_rw64(a) ((uint64_t)(a))

#endif


#endif /* !_FS_UDF_UDF_BSWAP_H_ */
