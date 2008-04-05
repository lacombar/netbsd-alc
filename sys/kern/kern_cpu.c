/*	$NetBSD: kern_cpu.c,v 1.22 2008/03/22 18:04:42 ad Exp $	*/

/*-
 * Copyright (c) 2007, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Andrew Doran.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
 * Copyright (c)2007 YAMAMOTO Takashi,
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

__KERNEL_RCSID(0, "$NetBSD: kern_cpu.c,v 1.22 2008/03/22 18:04:42 ad Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/idle.h>
#include <sys/sched.h>
#include <sys/intr.h>
#include <sys/conf.h>
#include <sys/cpu.h>
#include <sys/cpuio.h>
#include <sys/proc.h>
#include <sys/percpu.h>
#include <sys/kernel.h>
#include <sys/kauth.h>
#include <sys/xcall.h>
#include <sys/pool.h>
#include <sys/kmem.h>
#include <sys/select.h>

#include <uvm/uvm_extern.h>

void	cpuctlattach(int);

static void	cpu_xc_online(struct cpu_info *);
static void	cpu_xc_offline(struct cpu_info *);

dev_type_ioctl(cpuctl_ioctl);

const struct cdevsw cpuctl_cdevsw = {
	nullopen, nullclose, nullread, nullwrite, cpuctl_ioctl,
	nullstop, notty, nopoll, nommap, nokqfilter,
	D_OTHER | D_MPSAFE
};

kmutex_t cpu_lock;
int	ncpu;
int	ncpuonline;
bool	mp_online;

static struct cpu_info *cpu_infos[MAXCPUS];

int
mi_cpu_attach(struct cpu_info *ci)
{
	struct schedstate_percpu *spc = &ci->ci_schedstate;
	int error;

	ci->ci_index = ncpu;

	KASSERT(sizeof(kmutex_t) <= CACHE_LINE_SIZE);
	spc->spc_lwplock = kmem_alloc(CACHE_LINE_SIZE, KM_SLEEP);
	mutex_init(spc->spc_lwplock, MUTEX_DEFAULT, IPL_SCHED);
	sched_cpuattach(ci);
	uvm_cpu_attach(ci);

	error = create_idle_lwp(ci);
	if (error != 0) {
		/* XXX revert sched_cpuattach */
		return error;
	}

	if (ci == curcpu())
		ci->ci_data.cpu_onproc = curlwp;
	else
		ci->ci_data.cpu_onproc = ci->ci_data.cpu_idlelwp;

	percpu_init_cpu(ci);
	softint_init(ci);
	xc_init_cpu(ci);
	pool_cache_cpu_init(ci);
	selsysinit(ci);
	TAILQ_INIT(&ci->ci_data.cpu_biodone);
	ncpu++;
	ncpuonline++;
	cpu_infos[cpu_index(ci)] = ci;

	return 0;
}

void
cpuctlattach(int dummy)
{

}

int
cpuctl_ioctl(dev_t dev, u_long cmd, void *data, int flag, lwp_t *l)
{
	CPU_INFO_ITERATOR cii;
	cpustate_t *cs;
	struct cpu_info *ci;
	int error, i;
	u_int id;

	error = 0;

	mutex_enter(&cpu_lock);
	switch (cmd) {
	case IOC_CPU_SETSTATE:
		cs = data;
		error = kauth_authorize_system(l->l_cred,
		    KAUTH_SYSTEM_CPU, KAUTH_REQ_SYSTEM_CPU_SETSTATE, cs, NULL,
		    NULL);
		if (error != 0)
			break;
		if ((ci = cpu_lookup(cs->cs_id)) == NULL) {
			error = ESRCH;
			break;
		}
		if (!cs->cs_intr) {
			error = EOPNOTSUPP;
			break;
		}
		error = cpu_setonline(ci, cs->cs_online);
		break;

	case IOC_CPU_GETSTATE:
		cs = data;
		id = cs->cs_id;
		memset(cs, 0, sizeof(*cs));
		cs->cs_id = id;
		if ((ci = cpu_lookup(id)) == NULL) {
			error = ESRCH;
			break;
		}
		if ((ci->ci_schedstate.spc_flags & SPCF_OFFLINE) != 0)
			cs->cs_online = false;
		else
			cs->cs_online = true;
		cs->cs_intr = true;
		cs->cs_lastmod = ci->ci_schedstate.spc_lastmod;
		break;

	case IOC_CPU_MAPID:
		i = 0;
		for (CPU_INFO_FOREACH(cii, ci)) {
			if (i++ == *(int *)data)
				break;
		}
		if (ci == NULL)
			error = ESRCH;
		else
			*(int *)data = ci->ci_cpuid;
		break;

	case IOC_CPU_GETCOUNT:
		*(int *)data = ncpu;
		break;

	default:
		error = ENOTTY;
		break;
	}
	mutex_exit(&cpu_lock);

	return error;
}

struct cpu_info *
cpu_lookup(cpuid_t id)
{
	CPU_INFO_ITERATOR cii;
	struct cpu_info *ci;

	for (CPU_INFO_FOREACH(cii, ci)) {
		if (ci->ci_cpuid == id)
			return ci;
	}

	return NULL;
}

struct cpu_info *
cpu_lookup_byindex(u_int idx)
{
	struct cpu_info *ci = cpu_infos[idx];

	KASSERT(idx < MAXCPUS);
	KASSERT(ci == NULL || cpu_index(ci) == idx);

	return ci;
}

static void
cpu_xc_offline(struct cpu_info *ci)
{
	struct schedstate_percpu *spc, *mspc = NULL;
	struct cpu_info *mci;
	struct lwp *l;
	CPU_INFO_ITERATOR cii;
	int s;

	spc = &ci->ci_schedstate;
	s = splsched();
	spc->spc_flags |= SPCF_OFFLINE;
	splx(s);

	/* Take the first available CPU for the migration */
	for (CPU_INFO_FOREACH(cii, mci)) {
		mspc = &mci->ci_schedstate;
		if ((mspc->spc_flags & SPCF_OFFLINE) == 0)
			break;
	}
	KASSERT(mci != NULL);

	/*
	 * Migrate all non-bound threads to the other CPU.
	 * Please note, that this runs from the xcall thread, thus handling
	 * of LSONPROC is not needed.
	 */
	mutex_enter(&proclist_lock);

	/*
	 * Note that threads on the runqueue might sleep after this, but
	 * sched_takecpu() would migrate such threads to the appropriate CPU.
	 */
	LIST_FOREACH(l, &alllwp, l_list) {
		lwp_lock(l);
		if (l->l_cpu == ci && (l->l_stat == LSSLEEP ||
		    l->l_stat == LSSTOP || l->l_stat == LSSUSPENDED)) {
			KASSERT((l->l_flag & LW_RUNNING) == 0);
			l->l_cpu = mci;
		}
		lwp_unlock(l);
	}

	/* Double-lock the run-queues */
	spc_dlock(ci, mci);

	/* Handle LSRUN and LSIDL cases */
	LIST_FOREACH(l, &alllwp, l_list) {
		if (l->l_cpu != ci || (l->l_flag & LW_BOUND))
			continue;
		if (l->l_stat == LSRUN && (l->l_flag & LW_INMEM) != 0) {
			sched_dequeue(l);
			l->l_cpu = mci;
			lwp_setlock(l, mspc->spc_mutex);
			sched_enqueue(l, false);
		} else if (l->l_stat == LSRUN || l->l_stat == LSIDL) {
			l->l_cpu = mci;
			lwp_setlock(l, mspc->spc_mutex);
		}
	}
	spc_dunlock(ci, mci);
	mutex_exit(&proclist_lock);

#ifdef __HAVE_MD_CPU_OFFLINE
	cpu_offline_md();
#endif
}

static void
cpu_xc_online(struct cpu_info *ci)
{
	struct schedstate_percpu *spc;
	int s;

	spc = &ci->ci_schedstate;
	s = splsched();
	spc->spc_flags &= ~SPCF_OFFLINE;
	splx(s);
}

int
cpu_setonline(struct cpu_info *ci, bool online)
{
	struct schedstate_percpu *spc;
	CPU_INFO_ITERATOR cii;
	struct cpu_info *ci2;
	uint64_t where;
	xcfunc_t func;
	int nonline;

	spc = &ci->ci_schedstate;

	KASSERT(mutex_owned(&cpu_lock));

	if (online) {
		if ((spc->spc_flags & SPCF_OFFLINE) == 0)
			return 0;
		func = (xcfunc_t)cpu_xc_online;
		ncpuonline++;
	} else {
		if ((spc->spc_flags & SPCF_OFFLINE) != 0)
			return 0;
		nonline = 0;
		for (CPU_INFO_FOREACH(cii, ci2)) {
			nonline += ((ci2->ci_schedstate.spc_flags &
			    SPCF_OFFLINE) == 0);
		}
		if (nonline == 1)
			return EBUSY;
		func = (xcfunc_t)cpu_xc_offline;
		ncpuonline--;
	}

	where = xc_unicast(0, func, ci, NULL, ci);
	xc_wait(where);
	if (online) {
		KASSERT((spc->spc_flags & SPCF_OFFLINE) == 0);
	} else {
		KASSERT(spc->spc_flags & SPCF_OFFLINE);
	}
	spc->spc_lastmod = time_second;

	return 0;
}
