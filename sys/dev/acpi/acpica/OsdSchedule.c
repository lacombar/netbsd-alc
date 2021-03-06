/*	$NetBSD: OsdSchedule.c,v 1.8 2008/05/31 21:38:02 jmcneill Exp $	*/

/*
 * Copyright 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
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
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * OS Services Layer
 *
 * 6.3: Scheduling services
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: OsdSchedule.c,v 1.8 2008/05/31 21:38:02 jmcneill Exp $");

#include <sys/param.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/condvar.h>
#include <sys/mutex.h>

#include <dev/acpi/acpica.h>

#include <dev/acpi/acpi_osd.h>

#include <dev/sysmon/sysmon_taskq.h>

#define	_COMPONENT	ACPI_OS_SERVICES
ACPI_MODULE_NAME("SCHEDULE")

static kcondvar_t	acpi_osd_sleep_cv;
static kmutex_t		acpi_osd_sleep_mtx;

/*
 * acpi_osd_sched_init:
 *
 *	Initialize the APCICA Osd scheduler.  Called from AcpiOsInitialize().
 */
void
acpi_osd_sched_init(void)
{

	ACPI_FUNCTION_TRACE(__func__);

	sysmon_task_queue_init();
	mutex_init(&acpi_osd_sleep_mtx, MUTEX_DEFAULT, IPL_NONE);
	cv_init(&acpi_osd_sleep_cv, "acpislp");

	return_VOID;
}

/*
 * acpi_osd_sched_fini:
 *
 *	Clean up the ACPICA Osd scheduler.  Called from AcpiOsdTerminate().
 */
void
acpi_osd_sched_fini(void)
{

	ACPI_FUNCTION_TRACE(__func__);

	sysmon_task_queue_fini();

	return_VOID;
}

/*
 * AcpiOsGetThreadId:
 *
 *	Obtain the ID of the currently executing thread.
 */
ACPI_THREAD_ID
AcpiOsGetThreadId(void)
{
	return (ACPI_THREAD_ID)curlwp;
}

/*
 * AcpiOsQueueForExecution:
 *
 *	Schedule a procedure for deferred execution.
 */
ACPI_STATUS
AcpiOsExecute(ACPI_EXECUTE_TYPE Type, ACPI_OSD_EXEC_CALLBACK Function,
    void *Context)
{
	int pri;

	ACPI_FUNCTION_TRACE(__func__);

	switch (Type) {
	case OSL_GPE_HANDLER:
		pri = 10;
		break;
	case OSL_GLOBAL_LOCK_HANDLER:
	case OSL_EC_POLL_HANDLER:
	case OSL_EC_BURST_HANDLER:
		pri = 5;
		break;
	case OSL_NOTIFY_HANDLER:
		pri = 3;
		break;
	case OSL_DEBUGGER_THREAD:
		pri = 0;
		break;
	default:
		return_ACPI_STATUS(AE_BAD_PARAMETER);
	}

	switch (sysmon_task_queue_sched(pri, Function, Context)) {
	case 0:
		return_ACPI_STATUS(AE_OK);

	case ENOMEM:
		return_ACPI_STATUS(AE_NO_MEMORY);

	default:
		return_ACPI_STATUS(AE_BAD_PARAMETER);
	}
}

/*
 * AcpiOsSleep:
 *
 *	Suspend the running task (coarse granularity).
 */
void
AcpiOsSleep(ACPI_INTEGER Milliseconds)
{
	ACPI_FUNCTION_TRACE(__func__);

	if (cold)
		DELAY(Milliseconds * 1000);
	else {
		mutex_enter(&acpi_osd_sleep_mtx);
		cv_timedwait_sig(&acpi_osd_sleep_cv, &acpi_osd_sleep_mtx,
		    MAX(mstohz(Milliseconds), 1));
		mutex_exit(&acpi_osd_sleep_mtx);
	}
}

/*
 * AcpiOsStall:
 *
 *	Suspend the running task (fine granularity).
 */
void
AcpiOsStall(UINT32 Microseconds)
{

	ACPI_FUNCTION_TRACE(__func__);

	/*
	 * sleep(9) isn't safe because AcpiOsStall may be called
	 * with interrupt-disabled. (eg. by AcpiEnterSleepState)
	 * we should watch out for long stall requests.
	 */
#ifdef ACPI_DEBUG
	if (Microseconds > 1000)
		ACPI_DEBUG_PRINT((ACPI_DB_INFO, "long stall: %uus\n",
		    Microseconds));
#endif

	delay(Microseconds);

	return_VOID;

}

/*
 * AcpiOsGetTimer:
 *
 *	Get the current system time in 100 nanosecond units
 */
UINT64
AcpiOsGetTimer(void)
{
	struct timeval tv;
	UINT64 t;

	/* XXX During early boot there is no (decent) timer available yet. */
	if (cold)
		panic("acpi: timer op not yet supported during boot");

	microtime(&tv);
	t = (UINT64)10 * tv.tv_usec;
	t += (UINT64)10000000 * tv.tv_sec;

	return (t);
}
