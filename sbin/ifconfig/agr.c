/*	$NetBSD: agr.c,v 1.3 2005/03/19 17:31:48 thorpej Exp $	*/

/*-
 * Copyright (c)2005 YAMAMOTO Takashi,
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#if !defined(lint)
__RCSID("$NetBSD: agr.c,v 1.3 2005/03/19 17:31:48 thorpej Exp $");
#endif /* !defined(lint) */

#include <sys/param.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <net/agr/if_agrioctl.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <util.h>

#include "extern.h"
#include "agr.h"

static int checkifname(const char *);
static void assertifname(const char *);

static int
checkifname(const char *ifname)
{

	return strncmp(ifname, "agr", 3) != 0 ||
	    !isdigit((unsigned char)ifname[3]);
}

static void
assertifname(const char *ifname)
{

	if (checkifname(ifname)) {
		errx(EXIT_FAILURE, "valid only with agr(4) interfaces");
	}
}

void
agraddport(const char *val, int d)
{
	struct agrreq ar;

	assertifname(ifr.ifr_name);

	memset(&ar, 0, sizeof(ar));
	ar.ar_version = AGRREQ_VERSION;
	ar.ar_cmd = AGRCMD_ADDPORT;
	ar.ar_buf = __UNCONST(val);
	ar.ar_buflen = strlen(val);
	ifr.ifr_data = (void *)&ar;

	if (ioctl(s, SIOCSETAGR, &ifr) == -1) {
		err(EXIT_FAILURE, "SIOCSETAGR");
	}
}

void
agrremport(const char *val, int d)
{
	struct agrreq ar;

	assertifname(ifr.ifr_name);

	memset(&ar, 0, sizeof(ar));
	ar.ar_version = AGRREQ_VERSION;
	ar.ar_cmd = AGRCMD_REMPORT;
	ar.ar_buf = __UNCONST(val);
	ar.ar_buflen = strlen(val);
	ifr.ifr_data = (void *)&ar;

	if (ioctl(s, SIOCSETAGR, &ifr) == -1) {
		err(EXIT_FAILURE, "SIOCSETAGR");
	}
}

void
agr_status()
{
	struct agrreq ar;
	void *buf = NULL;
	size_t buflen = 0;
	struct agrportlist *apl;
	struct agrportinfo *api;
	int i;

	if (checkifname(ifr.ifr_name)) {
		return;
	}

again:
	memset(&ar, 0, sizeof(ar));
	ar.ar_version = AGRREQ_VERSION;
	ar.ar_cmd = AGRCMD_PORTLIST;
	ar.ar_buf = buf;
	ar.ar_buflen = buflen;
	ifr.ifr_data = (void *)&ar;

	if (ioctl(s, SIOCGETAGR, &ifr) == -1) {
		if (errno != E2BIG) {
			warn("SIOCGETAGR");
			return;
		}

		free(buf);
		buf = NULL;
		buflen = 0;
		goto again;
	}

	if (buf == NULL) {
		buflen = ar.ar_buflen;
		buf = malloc(buflen);
		if (buf == NULL) {
			err(EXIT_FAILURE, "agr_status");
		}
		goto again;
	}

	apl = buf;
	api = (void *)(apl + 1);

	for (i = 0; i < apl->apl_nports; i++) {
		char tmp[256];

		snprintb(tmp, sizeof(tmp), AGRPORTINFO_BITS, api->api_flags);
		printf("\tagrport: %s, flags=%s\n", api->api_ifname, tmp);
		api++;
	}
}
