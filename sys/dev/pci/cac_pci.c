/*	$NetBSD: cac_pci.c,v 1.28 2008/04/28 20:23:54 martin Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
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

/*
 * PCI front-end for cac(4) driver.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: cac_pci.c,v 1.28 2008/04/28 20:23:54 martin Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/queue.h>

#include <machine/endian.h>
#include <sys/bus.h>

#include <dev/pci/pcidevs.h>
#include <dev/pci/pcivar.h>

#include <dev/ic/cacreg.h>
#include <dev/ic/cacvar.h>

static struct	cac_ccb *cac_pci_l0_completed(struct cac_softc *);
static int	cac_pci_l0_fifo_full(struct cac_softc *);
static void	cac_pci_l0_intr_enable(struct cac_softc *, int);
static int	cac_pci_l0_intr_pending(struct cac_softc *);
static void	cac_pci_l0_submit(struct cac_softc *, struct cac_ccb *);

static const struct cac_linkage cac_pci_l0 = {
	cac_pci_l0_completed,
	cac_pci_l0_fifo_full,
	cac_pci_l0_intr_enable,
	cac_pci_l0_intr_pending,
	cac_pci_l0_submit
};

#define CT_STARTFW	0x01	/* Need to start controller firmware */

static struct cac_pci_type {
	int	ct_subsysid;
	int	ct_flags;
	const struct	cac_linkage *ct_linkage;
	const char	*ct_typestr;
} const cac_pci_type[] = {
	{ 0x40300e11,	0, 		&cac_l0,	"SMART-2/P" },
	{ 0x40310e11,	0, 		&cac_l0, 	"SMART-2SL" },
	{ 0x40320e11,	0, 		&cac_l0,	"Smart Array 3200" },
	{ 0x40330e11,	0, 		&cac_l0,	"Smart Array 3100ES" },
	{ 0x40340e11,	0, 		&cac_l0,	"Smart Array 221" },
	{ 0x40400e11,	CT_STARTFW, 	&cac_pci_l0,	"Integrated Array" },
	{ 0x40480e11,	CT_STARTFW,	&cac_pci_l0,	"RAID LC2" },
	{ 0x40500e11,	0,	 	&cac_pci_l0,	"Smart Array 4200" },
	{ 0x40510e11,	0, 		&cac_pci_l0,	"Smart Array 4200ES" },
	{ 0x40580e11,	0,		&cac_pci_l0,	"Smart Array 431" },
};

static struct cac_pci_product {
	u_short	cp_vendor;
	u_short	cp_product;
} const cac_pci_product[] = {
	{ PCI_VENDOR_COMPAQ,	PCI_PRODUCT_COMPAQ_SMART2P },
	{ PCI_VENDOR_DEC,	PCI_PRODUCT_DEC_21554 },
	{ PCI_VENDOR_SYMBIOS,	PCI_PRODUCT_SYMBIOS_1510 },
};

static const struct cac_pci_type *
cac_pci_findtype(struct pci_attach_args *pa)
{
	const struct cac_pci_type *ct;
	const struct cac_pci_product *cp;
	pcireg_t subsysid;
	int i;

	cp = cac_pci_product;
	i = 0;
	while (i < sizeof(cac_pci_product) / sizeof(cac_pci_product[0])) {
		if (PCI_VENDOR(pa->pa_id) == cp->cp_vendor &&
		    PCI_PRODUCT(pa->pa_id) == cp->cp_product)
		    	break;
		cp++;
		i++;
	}
	if (i == sizeof(cac_pci_product) / sizeof(cac_pci_product[0]))
		return (NULL);

	subsysid = pci_conf_read(pa->pa_pc, pa->pa_tag, PCI_SUBSYS_ID_REG);
	ct = cac_pci_type;
	i = 0;
	while (i < sizeof(cac_pci_type) / sizeof(cac_pci_type[0])) {
		if (subsysid == ct->ct_subsysid)
			break;
		ct++;
		i++;
	}
	if (i == sizeof(cac_pci_type) / sizeof(cac_pci_type[0]))
		return (NULL);

	return (ct);
}

static int
cac_pci_match(struct device *parent, struct cfdata *match,
    void *aux)
{

	return (cac_pci_findtype(aux) != NULL);
}

static void
cac_pci_attach(struct device *parent, struct device *self, void *aux)
{
	struct pci_attach_args *pa;
	const struct cac_pci_type *ct;
	struct cac_softc *sc;
	pci_chipset_tag_t pc;
	pci_intr_handle_t ih;
	const char *intrstr;
	pcireg_t reg;
	int memr, ior, i;

	aprint_naive(": RAID controller\n");

	sc = (struct cac_softc *)self;
	pa = (struct pci_attach_args *)aux;
	pc = pa->pa_pc;
	ct = cac_pci_findtype(pa);

	/*
	 * Map the PCI register window.
	 */
	memr = -1;
	ior = -1;

	for (i = 0x10; i <= 0x14; i += 4) {
		reg = pci_conf_read(pa->pa_pc, pa->pa_tag, i);

		if (PCI_MAPREG_TYPE(reg) == PCI_MAPREG_TYPE_IO) {
			if (ior == -1 && PCI_MAPREG_IO_SIZE(reg) != 0)
				ior = i;
		} else {
			if (memr == -1 && PCI_MAPREG_MEM_SIZE(reg) != 0)
				memr = i;
		}
	}

	if (memr != -1) {
		if (pci_mapreg_map(pa, memr, PCI_MAPREG_TYPE_MEM, 0,
		    &sc->sc_iot, &sc->sc_ioh, NULL, NULL))
			memr = -1;
		else
			ior = -1;
	}
	if (ior != -1)
		if (pci_mapreg_map(pa, ior, PCI_MAPREG_TYPE_IO, 0,
		    &sc->sc_iot, &sc->sc_ioh, NULL, NULL))
		    	ior = -1;
	if (memr == -1 && ior == -1) {
		aprint_error_dev(self, "can't map i/o or memory space\n");
		return;
	}

	sc->sc_dmat = pa->pa_dmat;

	/* Enable the device. */
	reg = pci_conf_read(pa->pa_pc, pa->pa_tag, PCI_COMMAND_STATUS_REG);
	pci_conf_write(pa->pa_pc, pa->pa_tag, PCI_COMMAND_STATUS_REG,
		       reg | PCI_COMMAND_MASTER_ENABLE);

	/* Map and establish the interrupt. */
	if (pci_intr_map(pa, &ih)) {
		aprint_error("can't map interrupt\n");
		return;
	}
	intrstr = pci_intr_string(pc, ih);
	sc->sc_ih = pci_intr_establish(pc, ih, IPL_BIO, cac_intr, sc);
	if (sc->sc_ih == NULL) {
		aprint_error("can't establish interrupt");
		if (intrstr != NULL)
			aprint_normal(" at %s", intrstr);
		aprint_normal("\n");
		return;
	}

	aprint_normal(": Compaq %s\n", ct->ct_typestr);

	/* Now attach to the bus-independent code. */
	memcpy(&sc->sc_cl, ct->ct_linkage, sizeof(sc->sc_cl));
	cac_init(sc, intrstr, (ct->ct_flags & CT_STARTFW) != 0);
}

CFATTACH_DECL(cac_pci, sizeof(struct cac_softc),
    cac_pci_match, cac_pci_attach, NULL, NULL);

static void
cac_pci_l0_submit(struct cac_softc *sc, struct cac_ccb *ccb)
{

	bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap,
	    (char *)ccb - (char *)sc->sc_ccbs,
	    sizeof(struct cac_ccb), BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);
	cac_outl(sc, CAC_42REG_CMD_FIFO, ccb->ccb_paddr);
}

static struct cac_ccb *
cac_pci_l0_completed(struct cac_softc *sc)
{
	struct cac_ccb *ccb;
	u_int32_t off;

	if ((off = cac_inl(sc, CAC_42REG_DONE_FIFO)) == 0xffffffffU)
		return (NULL);

	cac_outl(sc, CAC_42REG_DONE_FIFO, 0);

	if ((off & 3) != 0)
		printf("%s: failed command list returned: %lx\n",
		    device_xname(&sc->sc_dv), (long)off);

	off = (off & ~3) - sc->sc_ccbs_paddr;
	ccb = (struct cac_ccb *)((char *)sc->sc_ccbs + off);

	bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap, off, sizeof(struct cac_ccb),
	    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);

	if ((off & 3) != 0 && ccb->ccb_req.error == 0)
		ccb->ccb_req.error = CAC_RET_CMD_REJECTED;

	return (ccb);
}

static int
cac_pci_l0_intr_pending(struct cac_softc *sc)
{

	return ((cac_inl(sc, CAC_42REG_STATUS) & CAC_42_EXTINT) != 0);
}

static void
cac_pci_l0_intr_enable(struct cac_softc *sc, int state)
{

	cac_outl(sc, CAC_42REG_INTR_MASK, (state ? 0 : 8));	/* XXX */
}

static int
cac_pci_l0_fifo_full(struct cac_softc *sc)
{

	return (cac_inl(sc, CAC_42REG_CMD_FIFO) != 0);
}
