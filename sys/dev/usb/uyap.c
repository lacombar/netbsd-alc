/*	$NetBSD: uyap.c,v 1.14 2008/05/24 16:40:58 cube Exp $	*/

/*
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by  Lennart Augustsson <lennart@augustsson.net>.
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: uyap.c,v 1.14 2008/05/24 16:40:58 cube Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/conf.h>
#include <sys/tty.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/usb/usbdevs.h>

#include <dev/usb/ezload.h>

const struct ezdata uyap_firmware[] = {
#include "dev/usb/uyap_firmware.h"
};
const struct ezdata *uyap_firmwares[] = { uyap_firmware, NULL };

struct uyap_softc {
	USBBASEDEVICE		sc_dev;		/* base device */
};

USB_DECLARE_DRIVER(uyap);

USB_MATCH(uyap)
{
	USB_MATCH_START(uyap, uaa);

	/* Match the boot device. */
	if (uaa->vendor == USB_VENDOR_SILICONPORTALS &&
	    uaa->product == USB_PRODUCT_SILICONPORTALS_YAPPH_NF)
		return (UMATCH_VENDOR_PRODUCT);

	return (UMATCH_NONE);
}

USB_ATTACH(uyap)
{
	USB_ATTACH_START(uyap, sc, uaa);
	usbd_device_handle dev = uaa->device;
	usbd_status err;
	char *devinfop;

	sc->sc_dev = self;

	devinfop = usbd_devinfo_alloc(dev, 0);
	USB_ATTACH_SETUP;
	aprint_normal_dev(self, "%s\n", devinfop);
	usbd_devinfo_free(devinfop);

	aprint_verbose_dev(self, "downloading firmware\n");

	err = ezload_downloads_and_reset(dev, uyap_firmwares);
	if (err) {
		aprint_error_dev(self, "download ezdata error: %s\n",
		    usbd_errstr(err));
		USB_ATTACH_ERROR_RETURN;
	}

	aprint_verbose_dev(self,
	    "firmware download complete, disconnecting.\n");
	USB_ATTACH_SUCCESS_RETURN;
}

USB_DETACH(uyap)
{
	/*USB_DETACH_START(uyap, sc);*/

	return (0);
}

int
uyap_activate(device_ptr_t self, enum devact act)
{
	return 0;
}
