/* $NetBSD: siisata.c,v 1.2 2008/09/14 21:53:49 jakllsch Exp $ */

/* from ahcisata_core.c */

/*
 * Copyright (c) 2006 Manuel Bouyer.
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
 *	This product includes software developed by Manuel Bouyer.
 * 4. The name of the author may not be used to endorse or promote products
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
 *
 */

/* from atapi_wdc.c */

/*
 * Copyright (c) 1998, 2001 Manuel Bouyer.
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
 *	This product includes software developed by Manuel Bouyer.
 * 4. The name of the author may not be used to endorse or promote products
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
 * Copyright (c) 2007, 2008 Jonathan A. Kollasch.
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

#include <sys/types.h>
#include <sys/malloc.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/syslog.h>
#include <sys/disklabel.h>
#include <sys/buf.h>

#include <uvm/uvm_extern.h>

#include <dev/ic/wdcreg.h>
#include <dev/ata/atareg.h>
#include <dev/ata/satavar.h>
#include <dev/ata/satareg.h>
#include <dev/ic/siisatavar.h>

#include "atapibus.h"

#ifdef SIISATA_DEBUG
#if 0
int siisata_debug_mask = 0xffff;
#else
int siisata_debug_mask = 0;
#endif
#endif

#define ATA_DELAY 10000		/* 10s for a drive I/O */

static void siisata_attach_port(struct siisata_softc *, int);
static void siisata_intr_port(struct siisata_softc *,
    struct siisata_channel *);

void siisata_probe_drive(struct ata_channel *);
void siisata_setup_channel(struct ata_channel *);

int siisata_ata_bio(struct ata_drive_datas *, struct ata_bio *);
void siisata_reset_drive(struct ata_drive_datas *, int);
void siisata_reset_channel(struct ata_channel *, int);
int siisata_ata_addref(struct ata_drive_datas *);
void siisata_ata_delref(struct ata_drive_datas *);
void siisata_killpending(struct ata_drive_datas *);

void siisata_cmd_start(struct ata_channel *, struct ata_xfer *);
int siisata_cmd_complete(struct ata_channel *, struct ata_xfer *, int);
void siisata_cmd_done(struct ata_channel *, struct ata_xfer *, int);
void siisata_cmd_kill_xfer(struct ata_channel *, struct ata_xfer *, int);

void siisata_bio_start(struct ata_channel *, struct ata_xfer *);
int siisata_bio_complete(struct ata_channel *, struct ata_xfer *, int);
void siisata_bio_kill_xfer(struct ata_channel *, struct ata_xfer *, int);
int siisata_exec_command(struct ata_drive_datas *, struct ata_command *);

void siisata_timeout(void *);

static void siisata_reinit_port(struct ata_channel *);
static void siisata_device_reset(struct ata_channel *);
static void siisata_activate_prb(struct siisata_channel *, int);
static void siisata_deactivate_prb(struct siisata_channel *, int);
static int siisata_dma_setup(struct ata_channel *chp, int slot,
    void *data, size_t, int);

#if NATAPIBUS > 0
void siisata_atapibus_attach(struct atabus_softc *);
void siisata_atapi_probe_device(struct atapibus_softc *, int);
void siisata_atapi_minphys(struct buf *);
void siisata_atapi_start(struct ata_channel *,struct ata_xfer *);
int siisata_atapi_complete(struct ata_channel *, struct ata_xfer *, int);
void siisata_atapi_kill_xfer(struct ata_channel *, struct ata_xfer *, int);
void siisata_atapi_done(struct ata_channel *, struct ata_xfer *, int);
void siisata_atapi_reset(struct ata_channel *, struct ata_xfer *);
void siisata_atapi_scsipi_request(struct scsipi_channel *,
    scsipi_adapter_req_t, void *);
void siisata_atapi_kill_pending(struct scsipi_periph *);
#endif /* NATAPIBUS */

const struct ata_bustype siisata_ata_bustype = {
	SCSIPI_BUSTYPE_ATA,
	siisata_ata_bio,
	siisata_reset_drive,
	siisata_reset_channel,
	siisata_exec_command,
	ata_get_params,
	siisata_ata_addref,
	siisata_ata_delref,
	siisata_killpending
};

#if NATAPIBUS > 0
static const struct scsipi_bustype siisata_atapi_bustype = {
	SCSIPI_BUSTYPE_ATAPI,
	atapi_scsipi_cmd,
	atapi_interpret_sense,
	atapi_print_addr,
	siisata_atapi_kill_pending
};
#endif /* NATAPIBUS */


void
siisata_attach(struct siisata_softc *sc)
{
	int i;

	SIISATA_DEBUG_PRINT(("%s: %s: GR_GC: 0x%08x\n",
	    SIISATANAME(sc), __func__, GRREAD(sc, GR_GC)), DEBUG_FUNCS);

	sc->sc_atac.atac_cap = ATAC_CAP_DMA | ATAC_CAP_UDMA;
	sc->sc_atac.atac_pio_cap = 4;
	sc->sc_atac.atac_dma_cap = 2;
	sc->sc_atac.atac_udma_cap = 6;
	sc->sc_atac.atac_channels = sc->sc_chanarray;
	sc->sc_atac.atac_probe = siisata_probe_drive;
	sc->sc_atac.atac_bustype_ata = &siisata_ata_bustype;
	sc->sc_atac.atac_set_modes = siisata_setup_channel;
#if NATAPIBUS > 0
	sc->sc_atac.atac_atapibus_attach = siisata_atapibus_attach;
#endif	

	/* come out of reset state */
	GRWRITE(sc, GR_GC, 0);

	for (i = 0; i < sc->sc_atac.atac_nchannels; i++) {
		siisata_attach_port(sc, i);
	}

	SIISATA_DEBUG_PRINT(("%s: %s: GR_GC: 0x%08x\n",
	    SIISATANAME(sc), __func__, GRREAD(sc, GR_GC)),
	    DEBUG_FUNCS);
	return;
}

static void
siisata_init_port(struct siisata_softc *sc, int port)
{
	struct siisata_channel *schp;
	struct ata_channel *chp;

	schp = &sc->sc_channels[port];
	chp = (struct ata_channel *)schp;

	/* come out of reset, 64-bit activation */
	PRWRITE(sc, PRX(chp->ch_channel, PRO_PCC),
	    PR_PC_32BA | PR_PC_PORT_RESET);
	/* initialize port */
	siisata_reinit_port(chp);
	/* clear any interrupts */
	PRWRITE(sc, PRX(chp->ch_channel, PRO_PIS), 0xffffffff);
	/* enable CmdErrr+CmdCmpl interrupting */
	PRWRITE(sc, PRX(chp->ch_channel, PRO_PIES),
	    PR_PIS_CMDERRR | PR_PIS_CMDCMPL);
	/* enable port interrupt */
	GRWRITE(sc, GR_GC, GRREAD(sc, GR_GC) | GR_GC_PXIE(chp->ch_channel));
}

static void
siisata_attach_port(struct siisata_softc *sc, int port)
{
	int j;
	bus_dma_segment_t seg;
	int dmasize;
	int error;
	int rseg;
	void *prbp;
	struct siisata_channel *schp;
	struct ata_channel *chp;

	schp = &sc->sc_channels[port];
	chp = (struct ata_channel *)schp;
	sc->sc_chanarray[port] = chp;
	chp->ch_channel = port;
	chp->ch_atac = &sc->sc_atac;
	chp->ch_queue = malloc(sizeof(struct ata_queue),
			       M_DEVBUF, M_NOWAIT);
	if (chp->ch_queue == NULL) {
		aprint_error_dev(sc->sc_atac.atac_dev,
		    "port %d: can't allocate memory "
		    "for command queue", chp->ch_channel);
		return;
	}


	dmasize = SIISATA_CMD_SIZE * SIISATA_MAX_SLOTS;

	SIISATA_DEBUG_PRINT(("%s: %s: dmasize: %d\n", SIISATANAME(sc),
	    __func__, dmasize), DEBUG_FUNCS);

	error = bus_dmamem_alloc(sc->sc_dmat, dmasize, PAGE_SIZE, 0,
	    &seg, 1, &rseg, BUS_DMA_NOWAIT);
	if (error) {
		aprint_error_dev(sc->sc_atac.atac_dev,
		    "unable to allocate PRB table memory, "
		    "error=%d\n", error);
		return;
	}

	error = bus_dmamem_map(sc->sc_dmat, &seg, rseg, dmasize,
	    &prbp, BUS_DMA_NOWAIT | BUS_DMA_COHERENT);
	if (error) {
		aprint_error_dev(sc->sc_atac.atac_dev,
		    "unable to map PRB table memory, "
		    "error=%d\n", error);
		bus_dmamem_free(sc->sc_dmat, &seg, rseg);
		return;
	}

	error = bus_dmamap_create(sc->sc_dmat, dmasize, 1, dmasize, 0,
	    BUS_DMA_NOWAIT, &schp->sch_prbd);
	if (error) {
		aprint_error_dev(sc->sc_atac.atac_dev,
		    "unable to create PRB table map, "
		    "error=%d\n", error);
		bus_dmamem_unmap(sc->sc_dmat, prbp, dmasize);
		bus_dmamem_free(sc->sc_dmat, &seg, rseg);
		return;
	}

	error = bus_dmamap_load(sc->sc_dmat, schp->sch_prbd,
	    prbp, dmasize, NULL, BUS_DMA_NOWAIT);
	if (error) {
		aprint_error_dev(sc->sc_atac.atac_dev,
		    "unable to load PRB table map, "
		    "error=%d\n", error);
		bus_dmamap_destroy(sc->sc_dmat, schp->sch_prbd);
		bus_dmamem_unmap(sc->sc_dmat, prbp, dmasize);
		bus_dmamem_free(sc->sc_dmat, &seg, rseg);
		return;
	}

	for (j = 0; j < SIISATA_MAX_SLOTS; j++) {
		schp->sch_prb[j] = (struct siisata_prb *)
		    ((char *)prbp + SIISATA_CMD_SIZE * j);
		schp->sch_bus_prb[j] =
		    schp->sch_prbd->dm_segs[0].ds_addr +
		    SIISATA_CMD_SIZE * j;
		error = bus_dmamap_create(sc->sc_dmat, MAXPHYS,
		    SIISATA_NSGE, MAXPHYS, 0,
		    BUS_DMA_NOWAIT | BUS_DMA_ALLOCNOW,
		    &schp->sch_datad[j]);
		if (error) {
			aprint_error_dev(sc->sc_atac.atac_dev,
			    "couldn't create xfer DMA map, error=%d\n",
			    error);
			return;
		}
	}

	chp->ch_ndrive = 1;
	if (bus_space_subregion(sc->sc_prt, sc->sc_prh,
	    PRX(chp->ch_channel, PRO_SSTATUS), 4, &schp->sch_sstatus) != 0) {
		aprint_error_dev(sc->sc_atac.atac_dev,
		    "couldn't map port %d SStatus regs\n",
		    chp->ch_channel);
		return;
	}
	if (bus_space_subregion(sc->sc_prt, sc->sc_prh,
	    PRX(chp->ch_channel, PRO_SCONTROL), 4, &schp->sch_scontrol) != 0) {
		aprint_error_dev(sc->sc_atac.atac_dev,
		    "couldn't map port %d SControl regs\n",
		    chp->ch_channel);
		return;
	}
	if (bus_space_subregion(sc->sc_prt, sc->sc_prh,
	    PRX(chp->ch_channel, PRO_SERROR), 4, &schp->sch_serror) != 0) {
		aprint_error_dev(sc->sc_atac.atac_dev,
		    "couldn't map port %d SError regs\n",
		    chp->ch_channel);
		return;
	}

	siisata_init_port(sc, port);

	ata_channel_attach(chp);

	return;
}

void
siisata_resume(struct siisata_softc *sc)
{
	int i;

	/* come out of reset state */
	GRWRITE(sc, GR_GC, 0);

	for (i = 0; i < sc->sc_atac.atac_nchannels; i++) {
		siisata_init_port(sc, i);
	}
	
}

int
siisata_intr(void *v)
{
	struct siisata_softc *sc = v;
	uint32_t is;
	int i, r = 0;
	while ((is = GRREAD(sc, GR_GIS))) {
		SIISATA_DEBUG_PRINT(("%s: %s: GR_GIS: 0x%08x\n",
		    SIISATANAME(sc), __func__, is), DEBUG_INTR);
		r = 1;
		for (i = 0; i < sc->sc_atac.atac_nchannels; i++)
			if (is & GR_GIS_PXIS(i))
				siisata_intr_port(sc, &sc->sc_channels[i]);
	}
	return r;
}

static void
siisata_intr_port(struct siisata_softc *sc, struct siisata_channel *schp)
{
	struct ata_channel *chp = &schp->ata_channel;
	struct ata_xfer *xfer = chp->ch_queue->active_xfer;
	int slot = SIISATA_NON_NCQ_SLOT;

	SIISATA_DEBUG_PRINT(("%s: %s port %d\n",
	    SIISATANAME(sc), __func__, chp->ch_channel), DEBUG_INTR);

	if ((xfer != NULL) && (xfer->c_intr != NULL))
		xfer->c_intr(chp, xfer, slot);
#ifdef DIAGNOSTIC
	else
		log(LOG_WARNING, "%s: unable to handle interrupt\n", __func__);
#endif

	/* clear some (ok, all) ints */
	PRWRITE(sc, PRX(chp->ch_channel, PRO_PIS), 0xffffffff);

	return;
}

void
siisata_reset_drive(struct ata_drive_datas *drvp, int flags)
{
	struct ata_channel *chp = drvp->chnl_softc;
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct siisata_prb *prb;
	int slot = SIISATA_NON_NCQ_SLOT;

	/* wait for ready */
	while (!(PRREAD(sc, PRX(chp->ch_channel, PRO_PCS)) & PR_PS_PORT_READY))
		DELAY(10);

	prb = schp->sch_prb[slot];
	memset(prb, 0, sizeof(struct siisata_prb));
	prb->prb_control =
	    htole16(PRB_CF_SOFT_RESET | PRB_CF_INTERRUPT_MASK);

	siisata_activate_prb(schp, slot);

	/* wait for completion */
	while (PRREAD(sc, PRX(chp->ch_channel, PRO_PSS)) & PR_PXSS(slot))
		DELAY(10);

	siisata_deactivate_prb(schp, slot);

	log(LOG_DEBUG, "%s: ch_status %x ch_error %x\n",
	    __func__, chp->ch_status, chp->ch_error);

#if 1
	/* attempt to downgrade signaling in event of CRC error */
	/* XXX should be part of the MI (S)ATA subsystem */
	if (chp->ch_status == 0x51 && chp->ch_error == 0x84) {
		bus_space_write_4(sc->sc_prt, schp->sch_scontrol, 0,
		    SControl_IPM_NONE | SControl_SPD_G1 | SControl_DET_INIT);
		DELAY(10);
		bus_space_write_4(sc->sc_prt, schp->sch_scontrol, 0,
		    SControl_IPM_NONE | SControl_SPD_G1);
		DELAY(10);
		for (;;) {
			if ((bus_space_read_4(sc->sc_prt, schp->sch_sstatus, 0)
			    & SStatus_DET_mask) == SStatus_DET_DEV)
				break;
			DELAY(10);
		}
	}
#endif

#if 1
	chp->ch_status = 0;
	chp->ch_error = 0;
#endif
	return;
}

void
siisata_reset_channel(struct ata_channel *chp, int flags)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;

	SIISATA_DEBUG_PRINT(("%s: %s\n", SIISATANAME(sc), __func__),
	    DEBUG_FUNCS);

	if (sata_reset_interface(chp, sc->sc_prt, schp->sch_scontrol,
	    schp->sch_sstatus) != SStatus_DET_DEV) {
		log(LOG_CRIT, "%s port %d: reset failed\n",
		    SIISATANAME(sc), chp->ch_channel);
		/* XXX and then ? */
	}
	while (!(PRREAD(sc, PRX(chp->ch_channel, PRO_PCS)) & PR_PS_PORT_READY))
		DELAY(10);
	PRWRITE(sc, PRX(chp->ch_channel, PRO_SERROR),
	    PRREAD(sc, PRX(chp->ch_channel, PRO_SERROR)));
	if (chp->ch_queue->active_xfer) {
		chp->ch_queue->active_xfer->c_kill_xfer(chp,
		    chp->ch_queue->active_xfer, KILL_RESET);
	}

	return;
}

int
siisata_ata_addref(struct ata_drive_datas *drvp)
{
	return 0;
}

void
siisata_ata_delref(struct ata_drive_datas *drvp)
{
	return;
}

void
siisata_killpending(struct ata_drive_datas *drvp)
{
	return;
}

void
siisata_probe_drive(struct ata_channel *chp)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	int i;
	int s;
	uint32_t sig;
	int slot = SIISATA_NON_NCQ_SLOT;
	struct siisata_prb *prb;

	DELAY(chp->ch_channel * 2048 + 1023);	/* XXX */

	SIISATA_DEBUG_PRINT(("%s: %s: port %d start\n", SIISATANAME(sc),
	    __func__, chp->ch_channel), DEBUG_FUNCS);

	/* XXX This should be done by other code. */
	for (i = 0; i < chp->ch_ndrive; i++) {
		chp->ch_drive[i].chnl_softc = chp;
		chp->ch_drive[i].drive = i;
	}

	switch (sata_reset_interface(chp, sc->sc_prt, schp->sch_scontrol,
		schp->sch_sstatus)) {
	case SStatus_DET_DEV:
		/* wait for ready */
		while (!(PRREAD(sc, PRX(chp->ch_channel, PRO_PCS))
		    & PR_PS_PORT_READY))
			DELAY(10);

		prb = schp->sch_prb[slot];
		memset(prb, 0, sizeof(struct siisata_prb));
		prb->prb_control =
		    htole16(PRB_CF_SOFT_RESET | PRB_CF_INTERRUPT_MASK);

		siisata_activate_prb(schp, slot);

		/* wait for completion */
		while (PRREAD(sc, PRX(chp->ch_channel, PRO_PSS))
		    & PR_PXSS(slot))
			DELAY(10);

		siisata_deactivate_prb(schp, slot);

		/* read the signature out of the FIS */
		sig = 0;
		sig |= (PRREAD(sc, PRSX(chp->ch_channel, slot,
		    PRSO_FIS+0x4)) & 0x00ffffff) << 8;
		sig |= PRREAD(sc, PRSX(chp->ch_channel, slot,
		    PRSO_FIS+0xc)) & 0xff;

		SIISATA_DEBUG_PRINT(("%s: %s: sig=0x%08x\n", SIISATANAME(sc),
		    __func__, sig), DEBUG_PROBE);

		/* some ATAPI devices have bogus lower two bytes, sigh */
		if ((sig & 0xffff0000) == 0xeb140000) {
			sig &= 0xffff0000;
			sig |= 0x00000101;
		}

		s = splbio();
		switch (sig) {
		case 0xeb140101:
			chp->ch_drive[0].drive_flags |= DRIVE_ATAPI;
			break;
		case 0x00000101:
			chp->ch_drive[0].drive_flags |= DRIVE_ATA;
			break;
		default:
			aprint_error_dev(sc->sc_atac.atac_dev,
			    "%s: unknown device signature 0x%08x\n",
			    __func__, sig);
		}
		splx(s);
		break;
	default:
		break;
	}
	SIISATA_DEBUG_PRINT(("%s: %s: port %d done\n", SIISATANAME(sc),
	    __func__, chp->ch_channel), DEBUG_PROBE);
	return;
}

void
siisata_setup_channel(struct ata_channel *chp)
{
	return;
}

int
siisata_exec_command(struct ata_drive_datas *drvp, struct ata_command *ata_c)
{
	struct ata_channel *chp = drvp->chnl_softc;
	struct ata_xfer *xfer;
	int ret;
	int s;

	SIISATA_DEBUG_PRINT(("%s: %s begins\n",
	    SIISATANAME((struct siisata_softc *)chp->ch_atac), __func__),
	    DEBUG_FUNCS);

	xfer = ata_get_xfer(ata_c->flags & AT_WAIT ?
	    ATAXF_CANSLEEP : ATAXF_NOSLEEP);
	if (xfer == NULL)
		return ATACMD_TRY_AGAIN;
	if (ata_c->flags & AT_POLL)
		xfer->c_flags |= C_POLL;
	if (ata_c->flags & AT_WAIT)
		xfer->c_flags |= C_WAIT;
	xfer->c_drive = drvp->drive;
	xfer->c_databuf = ata_c->data;
	xfer->c_bcount = ata_c->bcount;
	xfer->c_cmd = ata_c;
	xfer->c_start = siisata_cmd_start;
	xfer->c_intr = siisata_cmd_complete;
	xfer->c_kill_xfer = siisata_cmd_kill_xfer;
	s = splbio();
	ata_exec_xfer(chp, xfer);
#ifdef DIAGNOSTIC
	if ((ata_c->flags & AT_POLL) != 0 &&
	    (ata_c->flags & AT_DONE) == 0)
		panic("%s: polled command not done", __func__);
#endif
	if (ata_c->flags & AT_DONE) {
		ret = ATACMD_COMPLETE;
	} else {
		if (ata_c->flags & AT_WAIT) {
			while ((ata_c->flags & AT_DONE) == 0) {
				SIISATA_DEBUG_PRINT(("%s: %s: sleeping\n",
				    SIISATANAME(
				    (struct siisata_softc *)chp->ch_atac),
				    __func__), DEBUG_FUNCS);
				tsleep(ata_c, PRIBIO, "siicmd", 0);
			}
			ret = ATACMD_COMPLETE;
		} else {
			ret = ATACMD_QUEUED;
		}
	}
	splx(s);
	SIISATA_DEBUG_PRINT( ("%s: %s ends\n",
	    SIISATANAME((struct siisata_softc *)chp->ch_atac), __func__),
	    DEBUG_FUNCS);
	return ret;
}

void
siisata_cmd_start(struct ata_channel *chp, struct ata_xfer *xfer)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct ata_command *ata_c = xfer->c_cmd;
	int slot = SIISATA_NON_NCQ_SLOT;
	struct siisata_prb *prb;
	uint8_t *fis;
	int i;

	SIISATA_DEBUG_PRINT(("%s: %s port %d, slot %d\n",
	    SIISATANAME(sc), __func__, chp->ch_channel, slot), DEBUG_FUNCS);

	prb = schp->sch_prb[slot];
	memset(prb, 0, sizeof(struct siisata_prb));
	fis = prb->prb_fis;

	/* XXX probably needs to be some common FIS-related code */
	fis[0] = 0x27;		/* host to device */
	fis[1] = 0x80;		/* command FIS (also, PMP) */
	fis[2] = ata_c->r_command;
	fis[3] = ata_c->r_features;
	fis[4] = ata_c->r_sector;
	fis[5] = ata_c->r_cyl & 0xff;
	fis[6] = (ata_c->r_cyl >> 8) & 0xff;
	fis[7] = ata_c->r_head & 0x0f;
	fis[12] = ata_c->r_count;
	fis[15] = WDCTL_4BIT;

	memset(prb->prb_atapi, 0, sizeof(prb->prb_atapi));

	if (siisata_dma_setup(chp, slot,
	    (ata_c->flags & (AT_READ | AT_WRITE)) ? ata_c->data : NULL,
	    ata_c->bcount,
	    (ata_c->flags & AT_READ) ? BUS_DMA_READ : BUS_DMA_WRITE)) { 
		ata_c->flags |= AT_DF;
		siisata_cmd_complete(chp, xfer, slot);
		return;
	}

	if (xfer->c_flags & C_POLL) {
		/* polled command, disable interrupts */
		GRWRITE(sc, GR_GC,
		    GRREAD(sc, GR_GC) & ~(GR_GC_PXIE(chp->ch_channel)));
	}

	/* go for it */
	siisata_activate_prb(schp, slot);

	if ((ata_c->flags & AT_POLL) == 0) {
		chp->ch_flags |= ATACH_IRQ_WAIT; /* wait for interrupt */
		callout_reset(&chp->ch_callout, mstohz(ata_c->timeout),
		    siisata_timeout, chp);
		goto out;
	}

	for (i = 0; i < ata_c->timeout / 10; i++) {
		if (ata_c->flags & AT_DONE)
			break;
		siisata_intr_port(sc, schp);
		if (ata_c->flags & AT_WAIT)
			tsleep(&xfer, PRIBIO, "siipl", mstohz(10));
		else
			DELAY(10000);
	}

	if ((ata_c->flags & AT_DONE) == 0) {
		ata_c->flags |= AT_TIMEOU;
		siisata_cmd_complete(chp, xfer, slot);
	}

	/* reenable interrupts */
	GRWRITE(sc, GR_GC, GRREAD(sc, GR_GC) | GR_GC_PXIE(chp->ch_channel));
out:
	SIISATA_DEBUG_PRINT(
	    ("%s: %s: done\n", SIISATANAME(sc), __func__), DEBUG_FUNCS);
	return;
}

void
siisata_cmd_kill_xfer(struct ata_channel *chp, struct ata_xfer *xfer,
    int reason)
{
	int slot = SIISATA_NON_NCQ_SLOT;

	struct ata_command *ata_c = xfer->c_cmd;
	switch (reason) {
	case KILL_GONE:
		ata_c->flags |= AT_GONE;
		break;
	case KILL_RESET:
		ata_c->flags |= AT_RESET;
		break;
	default:
		panic("%s: port %d: unknown reason %d",
		   __func__, chp->ch_channel, reason);
	}
	siisata_cmd_done(chp, xfer, slot);
}

int
siisata_cmd_complete(struct ata_channel *chp, struct ata_xfer *xfer, int slot)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct ata_command *ata_c = xfer->c_cmd;
	uint32_t pss, pis;
	uint8_t fis[4];
	uint32_t *prbfis = (void *)fis;
	
	SIISATA_DEBUG_PRINT(
	    ("%s: %s\n", SIISATANAME(sc), __func__), DEBUG_FUNCS);

	pis = PRREAD(sc, PRX(chp->ch_channel, PRO_PIS));

	if ((xfer->c_flags & C_TIMEOU) != 0)
		goto command_done;

	if (pis & PR_PIS_CMDCMPL) {
		/* get slot status, clearing completion interrupt */
		pss = PRREAD(sc, PRX(chp->ch_channel, PRO_PSS));
		/* is this expected? */
		if ((schp->sch_active_slots & __BIT(slot)) == 0) {
			log(LOG_WARNING, "%s: unexpected command "
			    "completion on port %d slot %d\n",
			    SIISATANAME(sc), chp->ch_channel,  slot);
			return 0;
		} else
			goto command_done;
	}

	if (pis & PR_PIS_CMDERRR) {
		uint32_t ec;

		/* emulate a CRC error by default */
		chp->ch_status = WDCS_ERR;
		chp->ch_error = WDCE_CRC;

		ec = PRREAD(sc, PRX(chp->ch_channel, PRO_PCE));
		if (ec <= PR_PCE_DATAFISERROR) {
			if (ec != PR_PCE_DATAFISERROR) {
				/* read in specific information about error */
				*prbfis = bus_space_read_stream_4(
				    sc->sc_prt, sc->sc_prh,
		    		    PRSX(chp->ch_channel, slot, PRSO_FIS));
				chp->ch_status = fis[2];
				chp->ch_error = fis[3];
			}
			siisata_reinit_port(chp);
		} else {
			/* okay, we have a "Fatal Error" */
			siisata_device_reset(chp);
		}
		goto command_done;
	}
	return 0;

command_done:
	chp->ch_flags &= ~ATACH_IRQ_WAIT;
	if (xfer->c_flags & C_TIMEOU)
		ata_c->flags |= AT_TIMEOU;
	else
		callout_stop(&chp->ch_callout);


	if (chp->ch_drive[xfer->c_drive].drive_flags & DRIVE_WAITDRAIN) {
		siisata_cmd_kill_xfer(chp, xfer, KILL_GONE);
		chp->ch_drive[xfer->c_drive].drive_flags &= ~DRIVE_WAITDRAIN;
		wakeup(&chp->ch_queue->active_xfer);
		return 0;
	}

	chp->ch_queue->active_xfer = NULL;

	if (pis) {
		ata_c->r_head = 0;
		ata_c->r_count = 0;
		ata_c->r_sector = 0;
		ata_c->r_cyl = 0;
		if (chp->ch_status & WDCS_BSY) {
			ata_c->flags |= AT_TIMEOU;
		} else if (chp->ch_status & WDCS_ERR) {
			ata_c->r_error = chp->ch_error;
			ata_c->flags |= AT_ERROR;
		}
	}
	siisata_cmd_done(chp, xfer, slot);
	return 0;
}

void
siisata_cmd_done(struct ata_channel *chp, struct ata_xfer *xfer, int slot)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct ata_command *ata_c = xfer->c_cmd;
	int i;
	uint16_t *idwordbuf;

	SIISATA_DEBUG_PRINT(
	    ("%s: %s.\n", SIISATANAME(sc), __func__), DEBUG_FUNCS);

	siisata_deactivate_prb(schp, slot);

	if (ata_c->flags & (AT_READ | AT_WRITE)) {
		bus_dmamap_sync(sc->sc_dmat, schp->sch_datad[slot], 0,
		    schp->sch_datad[slot]->dm_mapsize,
		    (ata_c->flags & AT_READ) ? BUS_DMASYNC_POSTREAD :
		    BUS_DMASYNC_POSTWRITE);
		bus_dmamap_unload(sc->sc_dmat, schp->sch_datad[slot]);
	}

	idwordbuf = xfer->c_databuf;

	/* correct the endianess of IDENTIFY data */
	if (ata_c->r_command == WDCC_IDENTIFY ||
	    ata_c->r_command == ATAPI_IDENTIFY_DEVICE) {
		for (i = 0; i < (xfer->c_bcount / sizeof(*idwordbuf)); i++) {
			idwordbuf[i] = le16toh(idwordbuf[i]);
		}
	}

	ata_c->flags |= AT_DONE;
	if (PRREAD(sc, PRSX(chp->ch_channel, slot, PRSO_RTC)))
		ata_c->flags |= AT_XFDONE;

	ata_free_xfer(chp, xfer);
	if (ata_c->flags & AT_WAIT)
		wakeup(ata_c);
	else if (ata_c->callback)
		ata_c->callback(ata_c->callback_arg);
	atastart(chp);
	return;
}

int
siisata_ata_bio(struct ata_drive_datas *drvp, struct ata_bio *ata_bio)
{
	struct ata_channel *chp = drvp->chnl_softc;
	struct ata_xfer *xfer;

	SIISATA_DEBUG_PRINT( ("%s: %s.\n",
	    SIISATANAME((struct siisata_softc *)chp->ch_atac),
	    __func__), DEBUG_FUNCS);

	xfer = ata_get_xfer(ATAXF_NOSLEEP);
	if (xfer == NULL)
		return ATACMD_TRY_AGAIN;
	if (ata_bio->flags & ATA_POLL)
		xfer->c_flags |= C_POLL;
	xfer->c_drive = drvp->drive;
	xfer->c_cmd = ata_bio;
	xfer->c_databuf = ata_bio->databuf;
	xfer->c_bcount = ata_bio->bcount;
	xfer->c_start = siisata_bio_start;
	xfer->c_intr = siisata_bio_complete;
	xfer->c_kill_xfer = siisata_bio_kill_xfer;
	ata_exec_xfer(chp, xfer);
	return (ata_bio->flags & ATA_ITSDONE) ?
	    ATACMD_COMPLETE : ATACMD_QUEUED;
}

void
siisata_bio_start(struct ata_channel *chp, struct ata_xfer *xfer)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct siisata_prb *prb;
	struct ata_bio *ata_bio = xfer->c_cmd;
	int slot = SIISATA_NON_NCQ_SLOT;
	int nblks, i;
	uint8_t *fis;

	SIISATA_DEBUG_PRINT(
	    ("%s: %s port %d, slot %d\n",
	    SIISATANAME(sc), __func__, chp->ch_channel, slot),
	    DEBUG_FUNCS);

	prb = schp->sch_prb[slot];
	memset(prb, 0, sizeof(struct siisata_prb));
	fis = prb->prb_fis;

	nblks = xfer->c_bcount / ata_bio->lp->d_secsize;

	/* XXX probably needs to be some common FIS-related code */
	fis[0] = 0x27;		/* host to device */
	fis[1] = 0x80;		/* command FIS (also, PMP) */
	if (ata_bio->flags & ATA_LBA48) {
		fis[2] = (ata_bio->flags & ATA_READ) ?
		    WDCC_READDMA_EXT : WDCC_WRITEDMA_EXT;
	} else {
		fis[2] =
		    (ata_bio->flags & ATA_READ) ? WDCC_READDMA : WDCC_WRITEDMA;
	}
	fis[4] = ata_bio->blkno & 0xff;
	fis[5] = (ata_bio->blkno >> 8) & 0xff;
	fis[6] = (ata_bio->blkno >> 16) & 0xff;
	if (ata_bio->flags & ATA_LBA48) {
		fis[7] = WDSD_LBA;
		fis[8] = (ata_bio->blkno >> 24) & 0xff;
		fis[9] = (ata_bio->blkno >> 32) & 0xff;
		fis[10] = (ata_bio->blkno >> 40) & 0xff;
	} else {
		fis[7] = ((ata_bio->blkno >> 24) & 0x0f) | WDSD_LBA;
	}
	fis[12] = nblks & 0xff;
	fis[13] = (ata_bio->flags & ATA_LBA48) ?
	    ((nblks >> 8) & 0xff) : 0;
	fis[15] = WDCTL_4BIT;

	memset(prb->prb_atapi, 0, sizeof(prb->prb_atapi));
	if (siisata_dma_setup(chp, slot, ata_bio->databuf, ata_bio->bcount,
	    (ata_bio->flags & ATA_READ) ? BUS_DMA_READ : BUS_DMA_WRITE)) {
		ata_bio->error = ERR_DMA;
		ata_bio->r_error = 0;
		siisata_bio_complete(chp, xfer, slot);
		return;
	}

	if (xfer->c_flags & C_POLL) {
		/* polled command, disable interrupts */
		GRWRITE(sc, GR_GC,
		    GRREAD(sc, GR_GC) & ~(GR_GC_PXIE(chp->ch_channel)));
	}

	siisata_activate_prb(schp, slot);

	if ((xfer->c_flags & C_POLL) == 0) {
		chp->ch_flags |= ATACH_IRQ_WAIT; /* wait for interrupt */
		callout_reset(&chp->ch_callout, mstohz(ATA_DELAY),
		    siisata_timeout, chp);
		goto out;
	}

	for (i = 0; i < ATA_DELAY / 10; i++) {
		if (ata_bio->flags & ATA_ITSDONE)
			break;
		siisata_intr_port(sc, schp);
		if (ata_bio->flags & ATA_NOSLEEP)
			DELAY(10000);
		else
			tsleep(&xfer, PRIBIO, "siipl", mstohz(10));
	}

	GRWRITE(sc, GR_GC, GRREAD(sc, GR_GC) | GR_GC_PXIE(chp->ch_channel));
out:
	SIISATA_DEBUG_PRINT(
	    ("%s: %s: done\n", SIISATANAME(sc), __func__), DEBUG_FUNCS);
	return;
}

void
siisata_bio_kill_xfer(struct ata_channel *chp, struct ata_xfer *xfer,
    int reason)
{
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct ata_bio *ata_bio = xfer->c_cmd;
	int drive = xfer->c_drive;
	int slot = SIISATA_NON_NCQ_SLOT;

	SIISATA_DEBUG_PRINT(("%s: %s: port %d\n",
	    SIISATANAME((struct siisata_softc *)chp->ch_atac),
	    __func__, chp->ch_channel), DEBUG_FUNCS);

	siisata_deactivate_prb(schp, slot);

	ata_free_xfer(chp, xfer);
	ata_bio->flags |= ATA_ITSDONE;
	switch (reason) {
	case KILL_GONE:
		ata_bio->error = ERR_NODEV;
		break;
	case KILL_RESET:
		ata_bio->error = ERR_RESET;
		break;
	default:
		panic("%s: port %d: unknown reason %d",
		   __func__, chp->ch_channel, reason);
	}
	ata_bio->r_error = WDCE_ABRT;
	(*chp->ch_drive[drive].drv_done)(chp->ch_drive[drive].drv_softc);
}

int
siisata_bio_complete(struct ata_channel *chp, struct ata_xfer *xfer, int slot)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct ata_bio *ata_bio = xfer->c_cmd;
	int drive = xfer->c_drive;
	uint32_t pss, pis;
	uint8_t fis[4];
	uint32_t *prbfis = (void *)fis;

	pis = PRREAD(sc, PRX(chp->ch_channel, PRO_PIS));

	if (pis & PR_PIS_CMDCMPL) {
		/* get slot status, clearing completion interrupt */
		pss = PRREAD(sc, PRX(chp->ch_channel, PRO_PSS));
		/* is this expected? */
		if ((schp->sch_active_slots & __BIT(slot)) == 0) {
			log(LOG_WARNING, "%s: unexpected command "
			    "completion on port %d slot %d\n",
			    SIISATANAME(sc), chp->ch_channel,  slot);
			return 0;
		} else {
			if (ata_bio->flags & ATA_READ)
				ata_bio->bcount -= PRREAD(sc,
				    PRSX(chp->ch_channel, slot, PRSO_RTC));
			else
				ata_bio->bcount = 0;

			/* XXX is reseting these right? */
			chp->ch_status = 0;
			chp->ch_error = 0;
			ata_bio->error = 0;
			goto command_done;
		}
	}

	if (pis & PR_PIS_CMDERRR) {
		uint32_t ec;

		/* emulate a CRC error by default */
		chp->ch_status = WDCS_ERR;
		chp->ch_error = WDCE_CRC;

		ec = PRREAD(sc, PRX(chp->ch_channel, PRO_PCE));
		if (ec <= PR_PCE_DATAFISERROR) {
			if (ec != PR_PCE_DATAFISERROR) {
				/* read in specific information about error */
				*prbfis = bus_space_read_stream_4(
				    sc->sc_prt, sc->sc_prh,
		    		    PRSX(chp->ch_channel, slot, PRSO_FIS));
				chp->ch_status = fis[2];
				chp->ch_error = fis[3];
			}
			siisata_reinit_port(chp);
		} else {
			/* okay, we have a "Fatal Error" */
			siisata_device_reset(chp);
		}
		goto command_done;
	}
	return 0;

command_done:
	schp->sch_active_slots &= ~__BIT(slot);
	chp->ch_flags &= ~ATACH_IRQ_WAIT;
	callout_stop(&chp->ch_callout);

	chp->ch_queue->active_xfer = NULL;

	bus_dmamap_sync(sc->sc_dmat, schp->sch_datad[slot], 0,
	    schp->sch_datad[slot]->dm_mapsize,
	    (ata_bio->flags & ATA_READ) ? BUS_DMASYNC_POSTREAD :
	    BUS_DMASYNC_POSTWRITE);
	bus_dmamap_unload(sc->sc_dmat, schp->sch_datad[slot]);

	if (chp->ch_drive[xfer->c_drive].drive_flags & DRIVE_WAITDRAIN) {
		siisata_bio_kill_xfer(chp, xfer, KILL_GONE);
		chp->ch_drive[xfer->c_drive].drive_flags &= ~DRIVE_WAITDRAIN;
		wakeup(&chp->ch_queue->active_xfer);
		return 0;
	}
	ata_free_xfer(chp, xfer);
	ata_bio->flags |= ATA_ITSDONE;
	if (chp->ch_status & WDCS_DWF) {
		ata_bio->error = ERR_DF;
	} else if (chp->ch_status & WDCS_ERR) {
		ata_bio->error = ERROR;
		ata_bio->r_error = chp->ch_error;
	} else if (chp->ch_status & WDCS_CORR)
		ata_bio->flags |= ATA_CORR;

	SIISATA_DEBUG_PRINT(("%s: %s bcount: %ld\n", SIISATANAME(sc),
	    __func__, ata_bio->bcount), DEBUG_XFERS); 

	(*chp->ch_drive[drive].drv_done)(chp->ch_drive[drive].drv_softc);

	atastart(chp);
	return 0;
}

void
siisata_timeout(void *v)
{
	struct ata_channel *chp = (struct ata_channel *)v;
	struct ata_xfer *xfer = chp->ch_queue->active_xfer;
	int slot = SIISATA_NON_NCQ_SLOT;
	int s = splbio();
	SIISATA_DEBUG_PRINT(("%s: %p\n", __func__, xfer), DEBUG_INTR);
	if ((chp->ch_flags & ATACH_IRQ_WAIT) != 0) {
		xfer->c_flags |= C_TIMEOU;
		xfer->c_intr(chp, xfer, slot);
	}
	splx(s);
}

static int
siisata_dma_setup(struct ata_channel *chp, int slot, void *data,
    size_t count, int op)
{

	int error, seg;
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;

	struct siisata_prb *prbp;

	prbp = schp->sch_prb[slot];

	if (data == NULL) {
		goto end;
	}

	error = bus_dmamap_load(sc->sc_dmat, schp->sch_datad[slot],
	    data, count, NULL, BUS_DMA_NOWAIT | BUS_DMA_STREAMING | op);
	if (error) {
		log(LOG_ERR, "%s port %d: "
		    "failed to load xfer in slot %d: error %d\n",
		    SIISATANAME(sc), chp->ch_channel, slot, error);
		return error;
	}

	bus_dmamap_sync(sc->sc_dmat, schp->sch_datad[slot], 0,
	    schp->sch_datad[slot]->dm_mapsize,
	    (op == BUS_DMA_READ) ? BUS_DMASYNC_PREREAD : BUS_DMASYNC_PREWRITE);

	/* make sure it's clean */
	memset(prbp->prb_sge, 0, SIISATA_NSGE * sizeof(struct siisata_prb));

	SIISATA_DEBUG_PRINT(("%s: %d segs, %ld count\n", __func__,
	    schp->sch_datad[slot]->dm_nsegs, (long unsigned int) count),
	    DEBUG_FUNCS | DEBUG_DEBUG);

	for (seg = 0; seg < schp->sch_datad[slot]->dm_nsegs; seg++) {
		prbp->prb_sge[seg].sge_da =
		    htole64(schp->sch_datad[slot]->dm_segs[seg].ds_addr);
		prbp->prb_sge[seg].sge_dc =
		    htole32(schp->sch_datad[slot]->dm_segs[seg].ds_len);
		prbp->prb_sge[seg].sge_flags = htole32(0);
	}
	prbp->prb_sge[seg - 1].sge_flags |= htole32(SGE_FLAG_TRM);
end:
	return 0;
}

static void
siisata_activate_prb(struct siisata_channel *schp, int slot)
{
	struct siisata_softc *sc;
	bus_size_t offset;
	bus_addr_t pprb;
	int port;

	sc = (struct siisata_softc *)schp->ata_channel.ch_atac;

	KASSERTMSG(((schp->sch_active_slots & __BIT(slot)) == __BIT(slot)),
	    ("%s: trying to activate active slot %d", SIISATANAME(sc), slot));

	port = schp->ata_channel.ch_channel;

	offset = PRO_CARX(port, slot);

	pprb = schp->sch_bus_prb[slot];


	SIISATA_PRB_SYNC(sc, schp, slot, BUS_DMASYNC_PREWRITE);
	/* keep track of what's going on */
	schp->sch_active_slots |= __BIT(slot);


	PRWRITE(sc, offset, pprb);
	offset += 4;
#if 0
	if (sizeof(bus_addr_t) == 8)
		PRWRITE(sc, offset, (pprb >> 32));
	else
#endif
		PRWRITE(sc, offset, 0);
}

static void
siisata_deactivate_prb(struct siisata_channel *schp, int slot)
{
	struct siisata_softc *sc;
	
	sc = (struct siisata_softc *)schp->ata_channel.ch_atac;

	KASSERTMSG(((schp->sch_active_slots & __BIT(slot)) == 0),
	    ("%s: trying to deactivate inactive slot %d", SIISATANAME(sc),
	    slot));

	schp->sch_active_slots &= ~__BIT(slot); /* mark free */
	SIISATA_PRB_SYNC(sc, schp, slot, BUS_DMASYNC_POSTWRITE);
}

static void
siisata_reinit_port(struct ata_channel *chp)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;

	PRWRITE(sc, PRX(chp->ch_channel, PRO_PCS),
	    PRREAD(sc, PRX(chp->ch_channel, PRO_PCS)) | PR_PC_PORT_INITIALIZE);
	while (!(PRREAD(sc, PRX(chp->ch_channel, PRO_PCS)) & PR_PS_PORT_READY))
		DELAY(10);
}

static void
siisata_device_reset(struct ata_channel *chp)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;

	PRWRITE(sc, PRX(chp->ch_channel, PRO_PCS),
	    PRREAD(sc, PRX(chp->ch_channel, PRO_PCS)) | PR_PC_DEVICE_RESET);
	while (!(PRREAD(sc, PRX(chp->ch_channel, PRO_PCS)) & PR_PS_PORT_READY))
		DELAY(10);
}


#if NATAPIBUS > 0
void
siisata_atapibus_attach(struct atabus_softc *ata_sc)
{
	struct ata_channel *chp = ata_sc->sc_chan;
	struct atac_softc *atac = chp->ch_atac;
	struct scsipi_adapter *adapt = &atac->atac_atapi_adapter._generic;
	struct scsipi_channel *chan = &chp->ch_atapi_channel;

	/*
	 * Fill in the scsipi_adapter.
	 */
	adapt->adapt_dev = atac->atac_dev;
	adapt->adapt_nchannels = atac->atac_nchannels;
	adapt->adapt_request = siisata_atapi_scsipi_request;
	adapt->adapt_minphys = siisata_atapi_minphys;
	atac->atac_atapi_adapter.atapi_probe_device =
	    siisata_atapi_probe_device;

	/*
	 * Fill in the scsipi_channel.
	 */
	memset(chan, 0, sizeof(*chan));
	chan->chan_adapter = adapt;
	chan->chan_bustype = &siisata_atapi_bustype;
	chan->chan_channel = chp->ch_channel;
	chan->chan_flags = SCSIPI_CHAN_OPENINGS;
	chan->chan_openings = 1;
	chan->chan_max_periph = 1;
	chan->chan_ntargets = 1;
	chan->chan_nluns = 1;

	chp->atapibus = config_found_ia(ata_sc->sc_dev, "atapi", chan,
	    atapiprint);
}

void
siisata_atapi_minphys(struct buf *bp)
{
	if (bp->b_bcount > MAXPHYS)
		bp->b_bcount = MAXPHYS;
	minphys(bp);
}

/*
 * Kill off all pending xfers for a periph.
 *
 * Must be called at splbio().
 */
void
siisata_atapi_kill_pending(struct scsipi_periph *periph)
{
	struct atac_softc *atac =
	    device_private(periph->periph_channel->chan_adapter->adapt_dev);
	struct ata_channel *chp =
	    atac->atac_channels[periph->periph_channel->chan_channel];

	ata_kill_pending(&chp->ch_drive[periph->periph_target]);
}

void
siisata_atapi_kill_xfer(struct ata_channel *chp, struct ata_xfer *xfer,
    int reason)
{
	struct scsipi_xfer *sc_xfer = xfer->c_cmd;

	/* remove this command from xfer queue */
	switch (reason) {
	case KILL_GONE:
		sc_xfer->error = XS_DRIVER_STUFFUP;
		break;
	case KILL_RESET:
		sc_xfer->error = XS_RESET;
		break;
	default:
		panic("%s: port %d: unknown reason %d",
		   __func__, chp->ch_channel, reason);
	}
	ata_free_xfer(chp, xfer);
	scsipi_done(sc_xfer);
}

void
siisata_atapi_probe_device(struct atapibus_softc *sc, int target)
{
	struct scsipi_channel *chan = sc->sc_channel;
	struct scsipi_periph *periph;
	struct ataparams ids;
	struct ataparams *id = &ids;
	struct siisata_softc *siic =
	    device_private(chan->chan_adapter->adapt_dev);
	struct atac_softc *atac = &siic->sc_atac;
	struct ata_channel *chp = atac->atac_channels[chan->chan_channel];
	struct ata_drive_datas *drvp = &chp->ch_drive[target];
	struct scsipibus_attach_args sa;
	char serial_number[21], model[41], firmware_revision[9];
	int s;

	/* skip if already attached */
	if (scsipi_lookup_periph(chan, target, 0) != NULL)
		return;

	/* if no ATAPI device detected at attach time, skip */
	if ((drvp->drive_flags & DRIVE_ATAPI) == 0) {
		SIISATA_DEBUG_PRINT(("%s: drive %d "
		    "not present\n", __func__, target), DEBUG_PROBE);
		return;
	}

	/* Some ATAPI devices need a bit more time after software reset. */
	delay(5000);
	if (ata_get_params(drvp, AT_WAIT, id) == 0) {
#ifdef ATAPI_DEBUG_PROBE
		log(LOG_DEBUG, "%s drive %d: cmdsz 0x%x drqtype 0x%x\n",
		    device_xname(sc->sc_dev), target,
		    id->atap_config & ATAPI_CFG_CMD_MASK,
		    id->atap_config & ATAPI_CFG_DRQ_MASK);
#endif
		periph = scsipi_alloc_periph(M_NOWAIT);
		if (periph == NULL) {
			aprint_error_dev(sc->sc_dev,
			    "%s: unable to allocate periph for "
			    "channel %d drive %d", __func__,
			    chp->ch_channel, target);
			return;
		}
		periph->periph_dev = NULL;
		periph->periph_channel = chan;
		periph->periph_switch = &atapi_probe_periphsw;
		periph->periph_target = target;
		periph->periph_lun = 0;
		periph->periph_quirks = PQUIRK_ONLYBIG;

#ifdef SCSIPI_DEBUG
		if (SCSIPI_DEBUG_TYPE == SCSIPI_BUSTYPE_ATAPI &&
		    SCSIPI_DEBUG_TARGET == target)
			periph->periph_dbflags |= SCSIPI_DEBUG_FLAGS;
#endif
		periph->periph_type = ATAPI_CFG_TYPE(id->atap_config);
		if (id->atap_config & ATAPI_CFG_REMOV)
			periph->periph_flags |= PERIPH_REMOVABLE;
		if (periph->periph_type == T_SEQUENTIAL) {
			s = splbio();
			drvp->drive_flags |= DRIVE_ATAPIST;
			splx(s);
		}

		sa.sa_periph = periph;
		sa.sa_inqbuf.type = ATAPI_CFG_TYPE(id->atap_config);
		sa.sa_inqbuf.removable = id->atap_config & ATAPI_CFG_REMOV ?
		    T_REMOV : T_FIXED;
		scsipi_strvis((u_char *)model, 40, id->atap_model, 40);
		scsipi_strvis((u_char *)serial_number, 20,
		    id->atap_serial, 20);
		scsipi_strvis((u_char *)firmware_revision, 8,
		    id->atap_revision, 8);
		sa.sa_inqbuf.vendor = model;
		sa.sa_inqbuf.product = serial_number;
		sa.sa_inqbuf.revision = firmware_revision;

		/*
		 * Determine the operating mode capabilities of the device.
		 */
		if ((id->atap_config & ATAPI_CFG_CMD_MASK)
		    == ATAPI_CFG_CMD_16) {
			periph->periph_cap |= PERIPH_CAP_CMD16;
		
			/* configure port for packet length */
			PRWRITE(siic, PRX(chp->ch_channel, PRO_PCS),
			    PRREAD(siic, PRX(chp->ch_channel, PRO_PCS)) |
			    PR_PC_PACKET_LENGTH);
		}
		/* XXX This is gross. */
		periph->periph_cap |= (id->atap_config & ATAPI_CFG_DRQ_MASK);

		drvp->drv_softc = atapi_probe_device(sc, target, periph, &sa);

		if (drvp->drv_softc)
			ata_probe_caps(drvp);
		else {
			s = splbio();
			drvp->drive_flags &= ~DRIVE_ATAPI;
			splx(s);
		}
	} else {
		SIISATA_DEBUG_PRINT(("%s: ATAPI_IDENTIFY_DEVICE "
		    "failed for drive %s:%d:%d: error 0x%x\n",
		    __func__, SIISATANAME(siic), chp->ch_channel, target,
		    chp->ch_error), DEBUG_PROBE);
		s = splbio();
		drvp->drive_flags &= ~DRIVE_ATAPI;
		splx(s);
	}
}

void
siisata_atapi_scsipi_request(struct scsipi_channel *chan,
    scsipi_adapter_req_t req, void *arg)
{
	struct scsipi_adapter *adapt = chan->chan_adapter;
	struct scsipi_periph *periph;
	struct scsipi_xfer *sc_xfer;
	struct siisata_softc *sc = device_private(adapt->adapt_dev);
	struct atac_softc *atac = &sc->sc_atac;
	struct ata_xfer *xfer;
	int channel = chan->chan_channel;
	int drive, s;

	switch (req) {
	case ADAPTER_REQ_RUN_XFER:
		sc_xfer = arg;
		periph = sc_xfer->xs_periph;
		drive = periph->periph_target;

		SIISATA_DEBUG_PRINT(("%s: %s:%d:%d\n", __func__,
		    device_xname(atac->atac_dev), channel, drive),
		    DEBUG_XFERS);

		if (!device_is_active(atac->atac_dev)) {
			sc_xfer->error = XS_DRIVER_STUFFUP;
			scsipi_done(sc_xfer);
			return;
		}
		xfer = ata_get_xfer(ATAXF_NOSLEEP);
		if (xfer == NULL) {
			sc_xfer->error = XS_RESOURCE_SHORTAGE;
			scsipi_done(sc_xfer);
			return;
		}

		if (sc_xfer->xs_control & XS_CTL_POLL)
			xfer->c_flags |= C_POLL;
		xfer->c_drive = drive;
		xfer->c_flags |= C_ATAPI;
		xfer->c_cmd = sc_xfer;
		xfer->c_databuf = sc_xfer->data;
		xfer->c_bcount = sc_xfer->datalen;
		xfer->c_start = siisata_atapi_start;
		xfer->c_intr = siisata_atapi_complete;
		xfer->c_kill_xfer = siisata_atapi_kill_xfer;
		xfer->c_dscpoll = 0;
		s = splbio();
		ata_exec_xfer(atac->atac_channels[channel], xfer);
#ifdef DIAGNOSTIC
		if ((sc_xfer->xs_control & XS_CTL_POLL) != 0 &&
		    (sc_xfer->xs_status & XS_STS_DONE) == 0)
			panic("%s: polled command not done", __func__);
#endif
		splx(s);
		return;

	default:
		/* Not supported, nothing to do. */
		;
	}
}

void
siisata_atapi_start(struct ata_channel *chp, struct ata_xfer *xfer)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct siisata_prb *prbp;

	struct scsipi_xfer *sc_xfer = xfer->c_cmd;

	int slot = SIISATA_NON_NCQ_SLOT;
	int i;
	uint8_t *fis;

	SIISATA_DEBUG_PRINT( ("%s: %s:%d:%d, scsi flags 0x%x\n", __func__,
	    SIISATANAME(sc), chp->ch_channel,
	    chp->ch_drive[xfer->c_drive].drive, sc_xfer->xs_control),
	    DEBUG_XFERS);

	prbp = schp->sch_prb[slot];
	memset(prbp, 0, sizeof(struct siisata_prb));
	fis = prbp->prb_fis;

	/* fill in direction for ATAPI command */
	if ((sc_xfer->xs_control & XS_CTL_DATA_IN))
		prbp->prb_control |= htole16(PRB_CF_PACKET_READ);
	if ((sc_xfer->xs_control & XS_CTL_DATA_OUT))
		prbp->prb_control |= htole16(PRB_CF_PACKET_WRITE);

	/* XXX probably needs to be some common FIS-related code */
	fis[0] = 0x27;  /* host to device */
	fis[1] = 0x80;  /* command FIS (and PMP) */
	fis[2] = ATAPI_PKT_CMD;
	fis[3] = (xfer->c_flags & C_DMA) ? ATAPI_PKT_CMD_FTRE_DMA : 0;
	fis[7] = WDSD_IBM;
	fis[15] = WDCTL_4BIT;

	/* copy over ATAPI command */
	memcpy(prbp->prb_atapi, sc_xfer->cmd, sc_xfer->cmdlen);

	if (siisata_dma_setup(chp, slot,
		(sc_xfer->xs_control & (XS_CTL_DATA_IN | XS_CTL_DATA_OUT)) ?
		xfer->c_databuf : NULL,
		xfer->c_bcount,
		(sc_xfer->xs_control & XS_CTL_DATA_IN) ?
		BUS_DMA_READ : BUS_DMA_WRITE)
	)
		panic("%s", __func__);

	if (xfer->c_flags & C_POLL) {
		/* polled command, disable interrupts */
		GRWRITE(sc, GR_GC,
		    GRREAD(sc, GR_GC) & ~(GR_GC_PXIE(chp->ch_channel)));
	}

	siisata_activate_prb(schp, slot);

	if ((xfer->c_flags & C_POLL) == 0) {
		chp->ch_flags |= ATACH_IRQ_WAIT; /* wait for interrupt */
		callout_reset(&chp->ch_callout, mstohz(sc_xfer->timeout),
		    siisata_timeout, chp);
		goto out;
	}
	/*
	 * polled command
	 */
	for (i = 0; i < ATA_DELAY / 10; i++) {
		if (sc_xfer->xs_status & XS_STS_DONE)
			break;
		siisata_intr_port(sc, schp);
		DELAY(10000);
	}
	if ((sc_xfer->xs_status & XS_STS_DONE) == 0) {
		sc_xfer->error = XS_TIMEOUT;
		siisata_atapi_complete(chp, xfer, slot);
	}
	/* reenable interrupts */
	GRWRITE(sc, GR_GC, GRREAD(sc, GR_GC) | GR_GC_PXIE(chp->ch_channel));
out:
	SIISATA_DEBUG_PRINT(
	    ("%s: %s: done\n", SIISATANAME(sc), __func__), DEBUG_FUNCS);
	return;
}

int
siisata_atapi_complete(struct ata_channel *chp, struct ata_xfer *xfer,
    int slot)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct scsipi_xfer *sc_xfer = xfer->c_cmd;
	uint8_t fis[4];
	uint32_t *prbfis = (void *)fis;
	uint32_t pss, pis;

	SIISATA_DEBUG_PRINT(
	    ("%s: %s()\n", SIISATANAME(sc), __func__), DEBUG_INTR);

	if ((xfer->c_flags & C_TIMEOU) != 0) {
		sc_xfer->error = XS_TIMEOUT;
		siisata_atapi_reset(chp, xfer);
		return 1;
	}

	pis = PRREAD(sc, PRX(chp->ch_channel, PRO_PIS));

	if (pis & PR_PIS_CMDCMPL) {
		/* get slot status, clearing completion interrupt */
		pss = PRREAD(sc, PRX(chp->ch_channel, PRO_PSS));
		/* is this expected? */
		if ((schp->sch_active_slots & __BIT(slot)) == 0) {
			log(LOG_WARNING, "%s: unexpected command "
			    "completion on port %d slot %d\n",
			    SIISATANAME(sc), chp->ch_channel,  slot);
			return 0;
		}
	}

	if (pis & PR_PIS_CMDERRR) {
		uint32_t ec;

		ec = PRREAD(sc, PRX(chp->ch_channel, PRO_PCE));
		if (ec <= PR_PCE_DATAFISERROR) {
			if (ec != PR_PCE_DATAFISERROR) {
				/* read in specific information about error */
				*prbfis = bus_space_read_stream_4(
				    sc->sc_prt, sc->sc_prh,
		    		    PRSX(chp->ch_channel, slot, PRSO_FIS));
				if (ec == PR_PCE_DEVICEERROR) {
					/* error code 1 implies *
					 * WDCS_ERR in fis[2]   */
					sc_xfer->error = XS_SHORTSENSE;
					sc_xfer->sense.atapi_sense = fis[3];
				}
			}
			siisata_reinit_port(chp);
		} else {
			/* okay, we have a "Fatal Error" */
			siisata_device_reset(chp);
		}
	}

	chp->ch_flags &= ~ATACH_IRQ_WAIT;
	siisata_atapi_done(chp, xfer, slot);
	return 1;
}

void
siisata_atapi_done(struct ata_channel *chp, struct ata_xfer *xfer, int slot)
{
	struct siisata_softc *sc = (struct siisata_softc *)chp->ch_atac;
	struct siisata_channel *schp = (struct siisata_channel *)chp;
	struct scsipi_xfer *sc_xfer = xfer->c_cmd;

	SIISATA_DEBUG_PRINT(("%s: %s:%d:%d: flags 0x%x\n", __func__,
	    device_xname(chp->ch_atac->atac_dev),
	    chp->ch_channel, xfer->c_drive,
	    (unsigned int)xfer->c_flags), DEBUG_XFERS);

	/* this comamnd is not active any more */
	schp->sch_active_slots &= ~__BIT(slot);

	if (sc_xfer->xs_control & (XS_CTL_DATA_IN | XS_CTL_DATA_OUT)) {
		bus_dmamap_sync(sc->sc_dmat, schp->sch_datad[slot], 0,
		    schp->sch_datad[slot]->dm_mapsize,
		    (sc_xfer->xs_control & XS_CTL_DATA_IN) ?
			 BUS_DMASYNC_POSTREAD : BUS_DMASYNC_POSTWRITE);
		bus_dmamap_unload(sc->sc_dmat, schp->sch_datad[slot]);
	}

	xfer->c_bcount -= sc_xfer->datalen;
	sc_xfer->resid = xfer->c_bcount;

	if (xfer->c_bcount != 0) {
		SIISATA_DEBUG_PRINT(("%s: bcount value is "
		    "%d after io\n", __func__, xfer->c_bcount), DEBUG_XFERS);
	}
#ifdef DIAGNOSTIC
	if (xfer->c_bcount < 0) {
		log(LOG_WARNING, "%s(): bcount value "
		    "is %d after io\n", __func__, xfer->c_bcount);
	}
#endif


	if (chp->ch_drive[xfer->c_drive].drive_flags & DRIVE_WAITDRAIN) {
		siisata_atapi_kill_xfer(chp, xfer, KILL_GONE);
		chp->ch_drive[xfer->c_drive].drive_flags &= ~DRIVE_WAITDRAIN;
		wakeup(&chp->ch_queue->active_xfer);
		return;
	}

	/* vvv  is this in the right order?  ^^^ */

	callout_stop(&chp->ch_callout);
	chp->ch_queue->active_xfer = NULL;
	ata_free_xfer(chp, xfer);

	SIISATA_DEBUG_PRINT(("%s: scsipi_done\n", __func__), DEBUG_XFERS);
	scsipi_done(sc_xfer);
	SIISATA_DEBUG_PRINT(("atastart from %s, flags 0x%x\n", __func__,
	    chp->ch_flags), DEBUG_XFERS);
	atastart(chp);
	return;
}

void
siisata_atapi_reset(struct ata_channel *chp, struct ata_xfer *xfer)
{
	struct ata_drive_datas *drvp = &chp->ch_drive[xfer->c_drive];
	drvp->state = 0;
	chp->ch_flags &= ~ATACH_IRQ_WAIT;
	siisata_atapi_done(chp, xfer, SIISATA_NON_NCQ_SLOT);
	return;
}
#endif /* NATAPIBUS */
