/* $NetBSD: kern_tc.c,v 1.36 2008/05/11 14:42:18 ad Exp $ */

/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
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

/*-
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ---------------------------------------------------------------------------
 */

#include <sys/cdefs.h>
/* __FBSDID("$FreeBSD: src/sys/kern/kern_tc.c,v 1.166 2005/09/19 22:16:31 andre Exp $"); */
__KERNEL_RCSID(0, "$NetBSD: kern_tc.c,v 1.36 2008/05/11 14:42:18 ad Exp $");

#include "opt_ntp.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/reboot.h>	/* XXX just to get AB_VERBOSE */
#include <sys/sysctl.h>
#include <sys/syslog.h>
#include <sys/systm.h>
#include <sys/timepps.h>
#include <sys/timetc.h>
#include <sys/timex.h>
#include <sys/evcnt.h>
#include <sys/kauth.h>
#include <sys/mutex.h>
#include <sys/atomic.h>

/*
 * A large step happens on boot.  This constant detects such steps.
 * It is relatively small so that ntp_update_second gets called enough
 * in the typical 'missed a couple of seconds' case, but doesn't loop
 * forever when the time step is large.
 */
#define LARGE_STEP	200

/*
 * Implement a dummy timecounter which we can use until we get a real one
 * in the air.  This allows the console and other early stuff to use
 * time services.
 */

static u_int
dummy_get_timecount(struct timecounter *tc)
{
	static u_int now;

	return (++now);
}

static struct timecounter dummy_timecounter = {
	dummy_get_timecount, 0, ~0u, 1000000, "dummy", -1000000, NULL, NULL,
};

struct timehands {
	/* These fields must be initialized by the driver. */
	struct timecounter	*th_counter;
	int64_t			th_adjustment;
	u_int64_t		th_scale;
	u_int	 		th_offset_count;
	struct bintime		th_offset;
	struct timeval		th_microtime;
	struct timespec		th_nanotime;
	/* Fields not to be copied in tc_windup start with th_generation. */
	volatile u_int		th_generation;
	struct timehands	*th_next;
};

static struct timehands th0;
static struct timehands th9 = { .th_next = &th0, };
static struct timehands th8 = { .th_next = &th9, };
static struct timehands th7 = { .th_next = &th8, };
static struct timehands th6 = { .th_next = &th7, };
static struct timehands th5 = { .th_next = &th6, };
static struct timehands th4 = { .th_next = &th5, };
static struct timehands th3 = { .th_next = &th4, };
static struct timehands th2 = { .th_next = &th3, };
static struct timehands th1 = { .th_next = &th2, };
static struct timehands th0 = {
	.th_counter = &dummy_timecounter,
	.th_scale = (uint64_t)-1 / 1000000,
	.th_offset = { .sec = 1, .frac = 0 },
	.th_generation = 1,
	.th_next = &th1,
};

static struct timehands *volatile timehands = &th0;
struct timecounter *timecounter = &dummy_timecounter;
static struct timecounter *timecounters = &dummy_timecounter;

time_t time_second = 1;
time_t time_uptime = 1;

static struct bintime timebasebin;

static int timestepwarnings;

kmutex_t timecounter_lock;
static u_int timecounter_mods;
static u_int timecounter_bad;

#ifdef __FreeBSD__
SYSCTL_INT(_kern_timecounter, OID_AUTO, stepwarnings, CTLFLAG_RW,
    &timestepwarnings, 0, "");
#endif /* __FreeBSD__ */

/*
 * sysctl helper routine for kern.timercounter.hardware
 */
static int
sysctl_kern_timecounter_hardware(SYSCTLFN_ARGS)
{
	struct sysctlnode node;
	int error;
	char newname[MAX_TCNAMELEN];
	struct timecounter *newtc, *tc;

	tc = timecounter;

	strlcpy(newname, tc->tc_name, sizeof(newname));

	node = *rnode;
	node.sysctl_data = newname;
	node.sysctl_size = sizeof(newname);

	error = sysctl_lookup(SYSCTLFN_CALL(&node));

	if (error ||
	    newp == NULL ||
	    strncmp(newname, tc->tc_name, sizeof(newname)) == 0)
		return error;

	if (l != NULL && (error = kauth_authorize_system(l->l_cred, 
	    KAUTH_SYSTEM_TIME, KAUTH_REQ_SYSTEM_TIME_TIMECOUNTERS, newname,
	    NULL, NULL)) != 0)
		return (error);

	if (!cold)
		mutex_spin_enter(&timecounter_lock);
	error = EINVAL;
	for (newtc = timecounters; newtc != NULL; newtc = newtc->tc_next) {
		if (strcmp(newname, newtc->tc_name) != 0)
			continue;
		/* Warm up new timecounter. */
		(void)newtc->tc_get_timecount(newtc);
		(void)newtc->tc_get_timecount(newtc);
		timecounter = newtc;
		error = 0;
		break;
	}
	if (!cold)
		mutex_spin_exit(&timecounter_lock);
	return error;
}

static int
sysctl_kern_timecounter_choice(SYSCTLFN_ARGS)
{
	char buf[MAX_TCNAMELEN+48];
	char *where;
	const char *spc;
	struct timecounter *tc;
	size_t needed, left, slen;
	int error, mods;

	if (newp != NULL)
		return (EPERM);
	if (namelen != 0)
		return (EINVAL);

	mutex_spin_enter(&timecounter_lock);
 retry:
	spc = "";
	error = 0;
	needed = 0;
	left = *oldlenp;
	where = oldp;
	for (tc = timecounters; error == 0 && tc != NULL; tc = tc->tc_next) {
		if (where == NULL) {
			needed += sizeof(buf);  /* be conservative */
		} else {
			slen = snprintf(buf, sizeof(buf), "%s%s(q=%d, f=%" PRId64
					" Hz)", spc, tc->tc_name, tc->tc_quality,
					tc->tc_frequency);
			if (left < slen + 1)
				break;
		 	mods = timecounter_mods;
			mutex_spin_exit(&timecounter_lock);
			error = copyout(buf, where, slen + 1);
			mutex_spin_enter(&timecounter_lock);
			if (mods != timecounter_mods) {
				goto retry;
			}
			spc = " ";
			where += slen;
			needed += slen;
			left -= slen;
		}
	}
	mutex_spin_exit(&timecounter_lock);

	*oldlenp = needed;
	return (error);
}

SYSCTL_SETUP(sysctl_timecounter_setup, "sysctl timecounter setup")
{
	const struct sysctlnode *node;

	sysctl_createv(clog, 0, NULL, &node,
		       CTLFLAG_PERMANENT,
		       CTLTYPE_NODE, "timecounter",
		       SYSCTL_DESCR("time counter information"),
		       NULL, 0, NULL, 0,
		       CTL_KERN, CTL_CREATE, CTL_EOL);

	if (node != NULL) {
		sysctl_createv(clog, 0, NULL, NULL,
			       CTLFLAG_PERMANENT,
			       CTLTYPE_STRING, "choice",
			       SYSCTL_DESCR("available counters"),
			       sysctl_kern_timecounter_choice, 0, NULL, 0,
			       CTL_KERN, node->sysctl_num, CTL_CREATE, CTL_EOL);

		sysctl_createv(clog, 0, NULL, NULL,
			       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
			       CTLTYPE_STRING, "hardware",
			       SYSCTL_DESCR("currently active time counter"),
			       sysctl_kern_timecounter_hardware, 0, NULL, MAX_TCNAMELEN,
			       CTL_KERN, node->sysctl_num, CTL_CREATE, CTL_EOL);

		sysctl_createv(clog, 0, NULL, NULL,
			       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
			       CTLTYPE_INT, "timestepwarnings",
			       SYSCTL_DESCR("log time steps"),
			       NULL, 0, &timestepwarnings, 0,
			       CTL_KERN, node->sysctl_num, CTL_CREATE, CTL_EOL);
	}
}

#ifdef TC_COUNTERS
#define	TC_STATS(name)							\
static struct evcnt n##name =						\
    EVCNT_INITIALIZER(EVCNT_TYPE_MISC, NULL, "timecounter", #name);	\
EVCNT_ATTACH_STATIC(n##name)
TC_STATS(binuptime);    TC_STATS(nanouptime);    TC_STATS(microuptime);
TC_STATS(bintime);      TC_STATS(nanotime);      TC_STATS(microtime);
TC_STATS(getbinuptime); TC_STATS(getnanouptime); TC_STATS(getmicrouptime);
TC_STATS(getbintime);   TC_STATS(getnanotime);   TC_STATS(getmicrotime);
TC_STATS(setclock);
#define	TC_COUNT(var)	var.ev_count++
#undef TC_STATS
#else
#define	TC_COUNT(var)	/* nothing */
#endif	/* TC_COUNTERS */

static void tc_windup(void);

/*
 * Return the difference between the timehands' counter value now and what
 * was when we copied it to the timehands' offset_count.
 */
static __inline u_int
tc_delta(struct timehands *th)
{
	struct timecounter *tc;

	tc = th->th_counter;
	return ((tc->tc_get_timecount(tc) - 
		 th->th_offset_count) & tc->tc_counter_mask);
}

/*
 * Functions for reading the time.  We have to loop until we are sure that
 * the timehands that we operated on was not updated under our feet.  See
 * the comment in <sys/timevar.h> for a description of these 12 functions.
 */

void
binuptime(struct bintime *bt)
{
	struct timehands *th;
	u_int gen;

	TC_COUNT(nbinuptime);
	do {
		th = timehands;
		gen = th->th_generation;
		*bt = th->th_offset;
		bintime_addx(bt, th->th_scale * tc_delta(th));
	} while (gen == 0 || gen != th->th_generation);
}

void
nanouptime(struct timespec *tsp)
{
	struct bintime bt;

	TC_COUNT(nnanouptime);
	binuptime(&bt);
	bintime2timespec(&bt, tsp);
}

void
microuptime(struct timeval *tvp)
{
	struct bintime bt;

	TC_COUNT(nmicrouptime);
	binuptime(&bt);
	bintime2timeval(&bt, tvp);
}

void
bintime(struct bintime *bt)
{

	TC_COUNT(nbintime);
	binuptime(bt);
	bintime_add(bt, &timebasebin);
}

void
nanotime(struct timespec *tsp)
{
	struct bintime bt;

	TC_COUNT(nnanotime);
	bintime(&bt);
	bintime2timespec(&bt, tsp);
}

void
microtime(struct timeval *tvp)
{
	struct bintime bt;

	TC_COUNT(nmicrotime);
	bintime(&bt);
	bintime2timeval(&bt, tvp);
}

void
getbinuptime(struct bintime *bt)
{
	struct timehands *th;
	u_int gen;

	TC_COUNT(ngetbinuptime);
	do {
		th = timehands;
		gen = th->th_generation;
		*bt = th->th_offset;
	} while (gen == 0 || gen != th->th_generation);
}

void
getnanouptime(struct timespec *tsp)
{
	struct timehands *th;
	u_int gen;

	TC_COUNT(ngetnanouptime);
	do {
		th = timehands;
		gen = th->th_generation;
		bintime2timespec(&th->th_offset, tsp);
	} while (gen == 0 || gen != th->th_generation);
}

void
getmicrouptime(struct timeval *tvp)
{
	struct timehands *th;
	u_int gen;

	TC_COUNT(ngetmicrouptime);
	do {
		th = timehands;
		gen = th->th_generation;
		bintime2timeval(&th->th_offset, tvp);
	} while (gen == 0 || gen != th->th_generation);
}

void
getbintime(struct bintime *bt)
{
	struct timehands *th;
	u_int gen;

	TC_COUNT(ngetbintime);
	do {
		th = timehands;
		gen = th->th_generation;
		*bt = th->th_offset;
	} while (gen == 0 || gen != th->th_generation);
	bintime_add(bt, &timebasebin);
}

void
getnanotime(struct timespec *tsp)
{
	struct timehands *th;
	u_int gen;

	TC_COUNT(ngetnanotime);
	do {
		th = timehands;
		gen = th->th_generation;
		*tsp = th->th_nanotime;
	} while (gen == 0 || gen != th->th_generation);
}

void
getmicrotime(struct timeval *tvp)
{
	struct timehands *th;
	u_int gen;

	TC_COUNT(ngetmicrotime);
	do {
		th = timehands;
		gen = th->th_generation;
		*tvp = th->th_microtime;
	} while (gen == 0 || gen != th->th_generation);
}

/*
 * Initialize a new timecounter and possibly use it.
 */
void
tc_init(struct timecounter *tc)
{
	u_int u;

	u = tc->tc_frequency / tc->tc_counter_mask;
	/* XXX: We need some margin here, 10% is a guess */
	u *= 11;
	u /= 10;
	if (u > hz && tc->tc_quality >= 0) {
		tc->tc_quality = -2000;
		aprint_verbose(
		    "timecounter: Timecounter \"%s\" frequency %ju Hz",
			    tc->tc_name, (uintmax_t)tc->tc_frequency);
		aprint_verbose(" -- Insufficient hz, needs at least %u\n", u);
	} else if (tc->tc_quality >= 0 || bootverbose) {
		aprint_verbose(
		    "timecounter: Timecounter \"%s\" frequency %ju Hz "
		    "quality %d\n", tc->tc_name, (uintmax_t)tc->tc_frequency,
		    tc->tc_quality);
	}

	mutex_spin_enter(&timecounter_lock);
	tc->tc_next = timecounters;
	timecounters = tc;
	timecounter_mods++;
	/*
	 * Never automatically use a timecounter with negative quality.
	 * Even though we run on the dummy counter, switching here may be
	 * worse since this timecounter may not be monotonous.
	 */
	if (tc->tc_quality >= 0 && (tc->tc_quality > timecounter->tc_quality ||
	    (tc->tc_quality == timecounter->tc_quality &&
	    tc->tc_frequency > timecounter->tc_frequency))) {
		(void)tc->tc_get_timecount(tc);
		(void)tc->tc_get_timecount(tc);
		timecounter = tc;
		tc_windup();
	}
	mutex_spin_exit(&timecounter_lock);
}

/*
 * Pick a new timecounter due to the existing counter going bad.
 */
static void
tc_pick(void)
{
	struct timecounter *best, *tc;

	KASSERT(mutex_owned(&timecounter_lock));

	for (best = tc = timecounters; tc != NULL; tc = tc->tc_next) {
		if (tc->tc_quality > best->tc_quality)
			best = tc;
		else if (tc->tc_quality < best->tc_quality)
			continue;
		else if (tc->tc_frequency > best->tc_frequency)
			best = tc;
	}
	(void)best->tc_get_timecount(best);
	(void)best->tc_get_timecount(best);
	timecounter = best;
}

/*
 * A timecounter has gone bad, arrange to pick a new one at the next
 * clock tick.
 */
void
tc_gonebad(struct timecounter *tc)
{

	tc->tc_quality = -100;
	membar_producer();
	atomic_inc_uint(&timecounter_bad);
}

/*
 * Stop using a timecounter and remove it from the timecounters list.
 */
int
tc_detach(struct timecounter *target)
{
	struct timecounter *tc;
	struct timecounter **tcp = NULL;
	int rc = 0;

	mutex_spin_enter(&timecounter_lock);
	for (tcp = &timecounters, tc = timecounters;
	     tc != NULL;
	     tcp = &tc->tc_next, tc = tc->tc_next) {
		if (tc == target)
			break;
	}
	if (tc == NULL) {
		rc = ESRCH;
	} else {
		*tcp = tc->tc_next;
		if (timecounter == target) {
			tc_pick();
			tc_windup();
		}
		timecounter_mods++;
	}
	mutex_spin_exit(&timecounter_lock);
	return rc;
}

/* Report the frequency of the current timecounter. */
u_int64_t
tc_getfrequency(void)
{

	return (timehands->th_counter->tc_frequency);
}

/*
 * Step our concept of UTC.  This is done by modifying our estimate of
 * when we booted.
 */
void
tc_setclock(struct timespec *ts)
{
	struct timespec ts2;
	struct bintime bt, bt2;

	mutex_spin_enter(&timecounter_lock);
	TC_COUNT(nsetclock);
	binuptime(&bt2);
	timespec2bintime(ts, &bt);
	bintime_sub(&bt, &bt2);
	bintime_add(&bt2, &timebasebin);
	timebasebin = bt;
	tc_windup();
	mutex_spin_exit(&timecounter_lock);

	if (timestepwarnings) {
		bintime2timespec(&bt2, &ts2);
		log(LOG_INFO, "Time stepped from %jd.%09ld to %jd.%09ld\n",
		    (intmax_t)ts2.tv_sec, ts2.tv_nsec,
		    (intmax_t)ts->tv_sec, ts->tv_nsec);
	}
}

/*
 * Initialize the next struct timehands in the ring and make
 * it the active timehands.  Along the way we might switch to a different
 * timecounter and/or do seconds processing in NTP.  Slightly magic.
 */
static void
tc_windup(void)
{
	struct bintime bt;
	struct timehands *th, *tho;
	u_int64_t scale;
	u_int delta, ncount, ogen;
	int i, s_update;
	time_t t;

	KASSERT(mutex_owned(&timecounter_lock));

	s_update = 0;

	/*
	 * Make the next timehands a copy of the current one, but do not
	 * overwrite the generation or next pointer.  While we update
	 * the contents, the generation must be zero.  Ensure global
	 * visibility of the generation before proceeding.
	 */
	tho = timehands;
	th = tho->th_next;
	ogen = th->th_generation;
	th->th_generation = 0;
	membar_producer();
	bcopy(tho, th, offsetof(struct timehands, th_generation));

	/*
	 * Capture a timecounter delta on the current timecounter and if
	 * changing timecounters, a counter value from the new timecounter.
	 * Update the offset fields accordingly.
	 */
	delta = tc_delta(th);
	if (th->th_counter != timecounter)
		ncount = timecounter->tc_get_timecount(timecounter);
	else
		ncount = 0;
	th->th_offset_count += delta;
	th->th_offset_count &= th->th_counter->tc_counter_mask;
	bintime_addx(&th->th_offset, th->th_scale * delta);

	/*
	 * Hardware latching timecounters may not generate interrupts on
	 * PPS events, so instead we poll them.  There is a finite risk that
	 * the hardware might capture a count which is later than the one we
	 * got above, and therefore possibly in the next NTP second which might
	 * have a different rate than the current NTP second.  It doesn't
	 * matter in practice.
	 */
	if (tho->th_counter->tc_poll_pps)
		tho->th_counter->tc_poll_pps(tho->th_counter);

	/*
	 * Deal with NTP second processing.  The for loop normally
	 * iterates at most once, but in extreme situations it might
	 * keep NTP sane if timeouts are not run for several seconds.
	 * At boot, the time step can be large when the TOD hardware
	 * has been read, so on really large steps, we call
	 * ntp_update_second only twice.  We need to call it twice in
	 * case we missed a leap second.
	 * If NTP is not compiled in ntp_update_second still calculates
	 * the adjustment resulting from adjtime() calls.
	 */
	bt = th->th_offset;
	bintime_add(&bt, &timebasebin);
	i = bt.sec - tho->th_microtime.tv_sec;
	if (i > LARGE_STEP)
		i = 2;
	for (; i > 0; i--) {
		t = bt.sec;
		ntp_update_second(&th->th_adjustment, &bt.sec);
		s_update = 1;
		if (bt.sec != t)
			timebasebin.sec += bt.sec - t;
	}

	/* Update the UTC timestamps used by the get*() functions. */
	/* XXX shouldn't do this here.  Should force non-`get' versions. */
	bintime2timeval(&bt, &th->th_microtime);
	bintime2timespec(&bt, &th->th_nanotime);

	/* Now is a good time to change timecounters. */
	if (th->th_counter != timecounter) {
		th->th_counter = timecounter;
		th->th_offset_count = ncount;
		s_update = 1;
	}

	/*-
	 * Recalculate the scaling factor.  We want the number of 1/2^64
	 * fractions of a second per period of the hardware counter, taking
	 * into account the th_adjustment factor which the NTP PLL/adjtime(2)
	 * processing provides us with.
	 *
	 * The th_adjustment is nanoseconds per second with 32 bit binary
	 * fraction and we want 64 bit binary fraction of second:
	 *
	 *	 x = a * 2^32 / 10^9 = a * 4.294967296
	 *
	 * The range of th_adjustment is +/- 5000PPM so inside a 64bit int
	 * we can only multiply by about 850 without overflowing, but that
	 * leaves suitably precise fractions for multiply before divide.
	 *
	 * Divide before multiply with a fraction of 2199/512 results in a
	 * systematic undercompensation of 10PPM of th_adjustment.  On a
	 * 5000PPM adjustment this is a 0.05PPM error.  This is acceptable.
 	 *
	 * We happily sacrifice the lowest of the 64 bits of our result
	 * to the goddess of code clarity.
	 *
	 */
	if (s_update) {
		scale = (u_int64_t)1 << 63;
		scale += (th->th_adjustment / 1024) * 2199;
		scale /= th->th_counter->tc_frequency;
		th->th_scale = scale * 2;
	}
	/*
	 * Now that the struct timehands is again consistent, set the new
	 * generation number, making sure to not make it zero.  Ensure
	 * changes are globally visible before changing.
	 */
	if (++ogen == 0)
		ogen = 1;
	membar_producer();
	th->th_generation = ogen;

	/*
	 * Go live with the new struct timehands.  Ensure changes are
	 * globally visible before changing.
	 */
	time_second = th->th_microtime.tv_sec;
	time_uptime = th->th_offset.sec;
	membar_producer();
	timehands = th;

	/*
	 * Force users of the old timehand to move on.  This is
	 * necessary for MP systems; we need to ensure that the
	 * consumers will move away from the old timehand before
	 * we begin updating it again when we eventually wrap
	 * around.
	 */
	if (++tho->th_generation == 0)
		tho->th_generation = 1;
}

/*
 * RFC 2783 PPS-API implementation.
 */

int
pps_ioctl(u_long cmd, void *data, struct pps_state *pps)
{
	pps_params_t *app;
	pps_info_t *pipi;
#ifdef PPS_SYNC
	int *epi;
#endif

	KASSERT(mutex_owned(&timecounter_lock));

	KASSERT(pps != NULL); /* XXX ("NULL pps pointer in pps_ioctl") */
	switch (cmd) {
	case PPS_IOC_CREATE:
		return (0);
	case PPS_IOC_DESTROY:
		return (0);
	case PPS_IOC_SETPARAMS:
		app = (pps_params_t *)data;
		if (app->mode & ~pps->ppscap)
			return (EINVAL);
		pps->ppsparam = *app;
		return (0);
	case PPS_IOC_GETPARAMS:
		app = (pps_params_t *)data;
		*app = pps->ppsparam;
		app->api_version = PPS_API_VERS_1;
		return (0);
	case PPS_IOC_GETCAP:
		*(int*)data = pps->ppscap;
		return (0);
	case PPS_IOC_FETCH:
		pipi = (pps_info_t *)data;
		pps->ppsinfo.current_mode = pps->ppsparam.mode;
		*pipi = pps->ppsinfo;
		return (0);
	case PPS_IOC_KCBIND:
#ifdef PPS_SYNC
		epi = (int *)data;
		/* XXX Only root should be able to do this */
		if (*epi & ~pps->ppscap)
			return (EINVAL);
		pps->kcmode = *epi;
		return (0);
#else
		return (EOPNOTSUPP);
#endif
	default:
		return (EPASSTHROUGH);
	}
}

void
pps_init(struct pps_state *pps)
{

	KASSERT(mutex_owned(&timecounter_lock));

	pps->ppscap |= PPS_TSFMT_TSPEC;
	if (pps->ppscap & PPS_CAPTUREASSERT)
		pps->ppscap |= PPS_OFFSETASSERT;
	if (pps->ppscap & PPS_CAPTURECLEAR)
		pps->ppscap |= PPS_OFFSETCLEAR;
}

void
pps_capture(struct pps_state *pps)
{
	struct timehands *th;

	KASSERT(mutex_owned(&timecounter_lock));
	KASSERT(pps != NULL);

	th = timehands;
	pps->capgen = th->th_generation;
	pps->capth = th;
	pps->capcount = th->th_counter->tc_get_timecount(th->th_counter);
	if (pps->capgen != th->th_generation)
		pps->capgen = 0;
}

void
pps_event(struct pps_state *pps, int event)
{
	struct bintime bt;
	struct timespec ts, *tsp, *osp;
	u_int tcount, *pcount;
	int foff, fhard;
	pps_seq_t *pseq;

	KASSERT(mutex_owned(&timecounter_lock));

	KASSERT(pps != NULL); /* XXX ("NULL pps pointer in pps_event") */
	/* If the timecounter was wound up underneath us, bail out. */
	if (pps->capgen == 0 || pps->capgen != pps->capth->th_generation)
		return;

	/* Things would be easier with arrays. */
	if (event == PPS_CAPTUREASSERT) {
		tsp = &pps->ppsinfo.assert_timestamp;
		osp = &pps->ppsparam.assert_offset;
		foff = pps->ppsparam.mode & PPS_OFFSETASSERT;
		fhard = pps->kcmode & PPS_CAPTUREASSERT;
		pcount = &pps->ppscount[0];
		pseq = &pps->ppsinfo.assert_sequence;
	} else {
		tsp = &pps->ppsinfo.clear_timestamp;
		osp = &pps->ppsparam.clear_offset;
		foff = pps->ppsparam.mode & PPS_OFFSETCLEAR;
		fhard = pps->kcmode & PPS_CAPTURECLEAR;
		pcount = &pps->ppscount[1];
		pseq = &pps->ppsinfo.clear_sequence;
	}

	/*
	 * If the timecounter changed, we cannot compare the count values, so
	 * we have to drop the rest of the PPS-stuff until the next event.
	 */
	if (pps->ppstc != pps->capth->th_counter) {
		pps->ppstc = pps->capth->th_counter;
		*pcount = pps->capcount;
		pps->ppscount[2] = pps->capcount;
		return;
	}

	/* Convert the count to a timespec. */
	tcount = pps->capcount - pps->capth->th_offset_count;
	tcount &= pps->capth->th_counter->tc_counter_mask;
	bt = pps->capth->th_offset;
	bintime_addx(&bt, pps->capth->th_scale * tcount);
	bintime_add(&bt, &timebasebin);
	bintime2timespec(&bt, &ts);

	/* If the timecounter was wound up underneath us, bail out. */
	if (pps->capgen != pps->capth->th_generation)
		return;

	*pcount = pps->capcount;
	(*pseq)++;
	*tsp = ts;

	if (foff) {
		timespecadd(tsp, osp, tsp);
		if (tsp->tv_nsec < 0) {
			tsp->tv_nsec += 1000000000;
			tsp->tv_sec -= 1;
		}
	}
#ifdef PPS_SYNC
	if (fhard) {
		u_int64_t scale;

		/*
		 * Feed the NTP PLL/FLL.
		 * The FLL wants to know how many (hardware) nanoseconds
		 * elapsed since the previous event.
		 */
		tcount = pps->capcount - pps->ppscount[2];
		pps->ppscount[2] = pps->capcount;
		tcount &= pps->capth->th_counter->tc_counter_mask;
		scale = (u_int64_t)1 << 63;
		scale /= pps->capth->th_counter->tc_frequency;
		scale *= 2;
		bt.sec = 0;
		bt.frac = 0;
		bintime_addx(&bt, scale * tcount);
		bintime2timespec(&bt, &ts);
		hardpps(tsp, ts.tv_nsec + 1000000000 * ts.tv_sec);
	}
#endif
}

/*
 * Timecounters need to be updated every so often to prevent the hardware
 * counter from overflowing.  Updating also recalculates the cached values
 * used by the get*() family of functions, so their precision depends on
 * the update frequency.
 */

static int tc_tick;

void
tc_ticktock(void)
{
	static int count;

	if (++count < tc_tick)
		return;
	count = 0;
	mutex_spin_enter(&timecounter_lock);
	if (timecounter_bad != 0) {
		/* An existing timecounter has gone bad, pick a new one. */
		(void)atomic_swap_uint(&timecounter_bad, 0);
		if (timecounter->tc_quality < 0) {
			tc_pick();
		}
	}
	tc_windup();
	mutex_spin_exit(&timecounter_lock);
}

void
inittimecounter(void)
{
	u_int p;

	mutex_init(&timecounter_lock, MUTEX_DEFAULT, IPL_SCHED);

	/*
	 * Set the initial timeout to
	 * max(1, <approx. number of hardclock ticks in a millisecond>).
	 * People should probably not use the sysctl to set the timeout
	 * to smaller than its inital value, since that value is the
	 * smallest reasonable one.  If they want better timestamps they
	 * should use the non-"get"* functions.
	 */
	if (hz > 1000)
		tc_tick = (hz + 500) / 1000;
	else
		tc_tick = 1;
	p = (tc_tick * 1000000) / hz;
	aprint_verbose("timecounter: Timecounters tick every %d.%03u msec\n",
	    p / 1000, p % 1000);

	/* warm up new timecounter (again) and get rolling. */
	(void)timecounter->tc_get_timecount(timecounter);
	(void)timecounter->tc_get_timecount(timecounter);
}
