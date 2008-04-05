/*	$NetBSD: af_inet.c,v 1.5 2006/11/13 05:13:39 dyoung Exp $	*/

/*
 * Copyright (c) 1983, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: af_inet.c,v 1.5 2006/11/13 05:13:39 dyoung Exp $");
#endif /* not lint */

#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h>

#include <net/if.h> 
#include <netinet/in.h>
#include <netinet/in_var.h>

#include <arpa/inet.h>

#include <err.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util.h>

#include "extern.h"
#include "af_inet.h"

static void in_preference(const char *, const struct sockaddr *);

struct in_aliasreq in_addreq;

int	setipdst;

void
setifipdst(const char *addr, int d)
{

	in_getaddr(addr, DSTADDR);
	setipdst++;
	clearaddr = 0;
	newaddr = 0;
}

void
in_alias(struct ifreq *creq)
{
	struct sockaddr_in *iasin;
	int alias;

	if (lflag)
		return;

	alias = 1;

	/* Get the non-alias address for this interface. */
	getsock(AF_INET);
	if (s < 0) {
		if (errno == EAFNOSUPPORT)
			return;
		err(EXIT_FAILURE, "socket");
	}
	(void) memset(&ifr, 0, sizeof(ifr));
	estrlcpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCGIFADDR, &ifr) == -1) {
		if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
			return;
		} else
			warn("SIOCGIFADDR");
	}
	/* If creq and ifr are the same address, this is not an alias. */
	if (memcmp(&ifr.ifr_addr, &creq->ifr_addr,
		   sizeof(creq->ifr_addr)) == 0)
		alias = 0;
	(void) memset(&in_addreq, 0, sizeof(in_addreq));
	estrlcpy(in_addreq.ifra_name, name, sizeof(in_addreq.ifra_name));
	memcpy(&in_addreq.ifra_addr, &creq->ifr_addr,
	    sizeof(in_addreq.ifra_addr));
	if (ioctl(s, SIOCGIFALIAS, &in_addreq) == -1) {
		if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
			return;
		} else
			warn("SIOCGIFALIAS");
	}

	iasin = &in_addreq.ifra_addr;
	printf("\tinet %s%s", alias ? "alias " : "", inet_ntoa(iasin->sin_addr));

	if (flags & IFF_POINTOPOINT) {
		iasin = &in_addreq.ifra_dstaddr;
		printf(" -> %s", inet_ntoa(iasin->sin_addr));
	}

	iasin = &in_addreq.ifra_mask;
	printf(" netmask 0x%x", ntohl(iasin->sin_addr.s_addr));

	if (flags & IFF_BROADCAST) {
		iasin = &in_addreq.ifra_broadaddr;
		printf(" broadcast %s", inet_ntoa(iasin->sin_addr));
	}
}

static uint16_t
in_get_preference(const char *ifname, const struct sockaddr *sa)
{
	struct if_addrprefreq ifap;

	getsock(AF_INET);
	if (s < 0) {
		if (errno == EPROTONOSUPPORT)
			return 0;
		err(EXIT_FAILURE, "socket");
	}
	(void)memset(&ifap, 0, sizeof(ifap));
	(void)strncpy(ifap.ifap_name, name, sizeof(ifap.ifap_name));
	(void)memcpy(&ifap.ifap_addr, sa,
	    MIN(sizeof(ifap.ifap_addr), sa->sa_len));
	if (ioctl(s, SIOCGIFADDRPREF, &ifap) == -1) {
		if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT)
			return 0;
		warn("SIOCGIFADDRPREF");
	}
	return ifap.ifap_preference;
}

static void
in_preference(const char *ifname, const struct sockaddr *sa)
{
	uint16_t preference;

	if (lflag)
		return;

	preference = in_get_preference(ifname, sa);
	printf(" preference %" PRIu16, preference);
}

void
in_status(int force)
{
	struct ifaddrs *ifap, *ifa;
	struct ifreq isifr;
	int printprefs = 0;

	if (getifaddrs(&ifap) != 0)
		err(EXIT_FAILURE, "getifaddrs");
	/* Print address preference numbers if any address has a non-zero
	 * preference assigned.
	 */
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (strcmp(name, ifa->ifa_name) != 0)
			continue;
		if (ifa->ifa_addr->sa_family != AF_INET)
			continue;
		if (in_get_preference(ifa->ifa_name, ifa->ifa_addr) != 0) {
			printprefs = 1;
			break;
		}
	}
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (strcmp(name, ifa->ifa_name) != 0)
			continue;
		if (ifa->ifa_addr->sa_family != AF_INET)
			continue;
		if (sizeof(isifr.ifr_addr) < ifa->ifa_addr->sa_len)
			continue;

		memset(&isifr, 0, sizeof(isifr));
		estrlcpy(isifr.ifr_name, ifa->ifa_name, sizeof(isifr.ifr_name));
		memcpy(&isifr.ifr_addr, ifa->ifa_addr, ifa->ifa_addr->sa_len);
		in_alias(&isifr);
		if (printprefs)
			in_preference(ifa->ifa_name, ifa->ifa_addr);
		printf("\n");
	}
	if (ifa != NULL) {
		for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
			if (strcmp(name, ifa->ifa_name) != 0)
				continue;
			if (ifa->ifa_addr->sa_family != AF_INET)
				continue;
		}
	}
	freeifaddrs(ifap);
}

#define SIN(x) ((struct sockaddr_in *) &(x))
struct sockaddr_in *sintab[] = {
    SIN(ridreq.ifr_addr), SIN(in_addreq.ifra_addr),
    SIN(in_addreq.ifra_mask), SIN(in_addreq.ifra_broadaddr)};

void
in_getaddr(const char *str, int which)
{
	struct sockaddr_in *gasin = sintab[which];
	struct hostent *hp;
	struct netent *np;

	gasin->sin_len = sizeof(*gasin);
	if (which != MASK)
		gasin->sin_family = AF_INET;

	if (which == ADDR) {
		char *p = NULL;
		if ((p = strrchr(str, '/')) != NULL) {
			*p = '\0';
			in_getprefix(p + 1, MASK);
		}
	}

	if (inet_aton(str, &gasin->sin_addr) == 0) {
		if ((hp = gethostbyname(str)) != NULL)
			(void) memcpy(&gasin->sin_addr, hp->h_addr, hp->h_length);
		else if ((np = getnetbyname(str)) != NULL)
			gasin->sin_addr = inet_makeaddr(np->n_net, INADDR_ANY);
		else
			errx(EXIT_FAILURE, "%s: bad value", str);
	}
}

void
in_getprefix(const char *plen, int which)
{
	struct sockaddr_in *igsin = sintab[which];
	u_char *cp;
	int len = strtol(plen, (char **)NULL, 10);

	if ((len < 0) || (len > 32))
		errx(EXIT_FAILURE, "%s: bad value", plen);
	igsin->sin_len = sizeof(*igsin);
	if (which != MASK)
		igsin->sin_family = AF_INET;
	if ((len == 0) || (len == 32)) {
		memset(&igsin->sin_addr, 0xff, sizeof(struct in_addr));
		return;
	}
	memset((void *)&igsin->sin_addr, 0x00, sizeof(igsin->sin_addr));
	for (cp = (u_char *)&igsin->sin_addr; len > 7; len -= 8)
		*cp++ = 0xff;
	if (len)
		*cp = 0xff << (8 - len);
}
