/*	$NetBSD: sis_drv.c,v 1.5 2008/07/08 06:50:23 mrg Exp $	*/

/* sis.c -- sis driver -*- linux-c -*-
 */
/*-
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: sis_drv.c,v 1.5 2008/07/08 06:50:23 mrg Exp $");
/*
__FBSDID("$FreeBSD: src/sys/dev/drm/sis_drv.c,v 1.7 2005/12/20 22:44:36 jhb Exp $");
*/

#include "drmP.h"
#include "sis_drm.h"
#include "sis_drv.h"
#include "drm_pciids.h"

/* drv_PCI_IDs comes from drm_pciids.h, generated from drm_pciids.txt. */
static drm_pci_id_list_t sis_pciidlist[] = {
	sis_PCI_IDS
};

static void sis_configure(drm_device_t *dev)
{
	dev->driver.buf_priv_size	= 1; /* No dev_priv */
	dev->driver.context_ctor	= sis_init_context;
	dev->driver.context_dtor	= sis_final_context;

	dev->driver.ioctls		= sis_ioctls;
	dev->driver.max_ioctl		= sis_max_ioctl;

	dev->driver.name		= DRIVER_NAME;
	dev->driver.desc		= DRIVER_DESC;
	dev->driver.date		= DRIVER_DATE;
	dev->driver.major		= DRIVER_MAJOR;
	dev->driver.minor		= DRIVER_MINOR;
	dev->driver.patchlevel		= DRIVER_PATCHLEVEL;

	dev->driver.use_agp		= 1;
	dev->driver.use_mtrr		= 1;
}

#ifdef __FreeBSD__
static int
sis_probe(device_t dev)
{
	return drm_probe(dev, sis_pciidlist);
}

static int
sis_attach(device_t nbdev)
{
	drm_device_t *dev = device_get_softc(nbdev);

	bzero(dev, sizeof(drm_device_t));
	sis_configure(dev);
	return drm_attach(nbdev, sis_pciidlist);
}

static device_method_t sis_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		sis_probe),
	DEVMETHOD(device_attach,	sis_attach),
	DEVMETHOD(device_detach,	drm_detach),

	{ 0, 0 }
};

static driver_t sis_driver = {
	"drm",
	sis_methods,
	sizeof(drm_device_t)
};

extern devclass_t drm_devclass;
#if __FreeBSD_version >= 700010
DRIVER_MODULE(sisdrm, vgapci, sis_driver, drm_devclass, 0, 0);
#else
DRIVER_MODULE(sisdrm, pci, sis_driver, drm_devclass, 0, 0);
#endif
MODULE_DEPEND(sisdrm, drm, 1, 1, 1);

#elif defined(__NetBSD__) || defined(__OpenBSD__)

static int
sisdrm_probe(struct device *parent, struct cfdata *match, void *aux)
{
	struct pci_attach_args *pa = aux;
	return drm_probe(pa, sis_pciidlist);
}

static void
sisdrm_attach(struct device *parent, struct device *self, void *aux)
{
	struct pci_attach_args *pa = aux;
	drm_device_t *dev = device_private(self);

	sis_configure(dev);
	return drm_attach(self, pa, sis_pciidlist);
}

CFATTACH_DECL_NEW(sisdrm, sizeof(drm_device_t), sisdrm_probe, sisdrm_attach,
	drm_detach, drm_activate);

#endif
