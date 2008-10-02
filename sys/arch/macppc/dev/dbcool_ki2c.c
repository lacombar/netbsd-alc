/*	$NetBSD: dbcool_ki2c.c,v 1.1 2008/10/02 00:55:43 pgoyette Exp $ */

/*-
 * Copyright (C) 2005 Michael Lorenz
 * Copyright (C) 2008 Paul Goyette
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
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* 
 * a driver for the dbCool family of environmental controllers found in the
 * iBook G4 and probably other Apple machines 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: dbcool_ki2c.c,v 1.1 2008/10/02 00:55:43 pgoyette Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/sysctl.h>

#include <uvm/uvm_extern.h>

#include <dev/ofw/openfirm.h>
#include <macppc/dev/ki2cvar.h>

#include <dev/i2c/dbcool_var.h>
#include <dev/i2c/dbcool_reg.h>

static void dbcool_ki2c_attach(device_t, device_t, void *);
static int dbcool_ki2c_match(device_t, cfdata_t, void *);

CFATTACH_DECL_NEW(dbcool_ki2c, sizeof(struct dbcool_softc),
    dbcool_ki2c_match, dbcool_ki2c_attach, NULL, NULL);

int
dbcool_ki2c_match(device_t parent, cfdata_t cf, void *aux)
{
	struct ki2c_confargs *ka = aux;
	char compat[32];
	
	if (strcmp(ka->ka_name, "fan") != 0)
		return 0;

	memset(compat, 0, sizeof(compat));
	OF_getprop(ka->ka_node, "compatible", compat, sizeof(compat));
	if (strcmp(compat, "adt7467") != 0)
		return 0;
	
	return 1;
}

void
dbcool_ki2c_attach(device_t parent, device_t self, void *aux)
{
	struct dbcool_softc *sc = device_private(self);
	struct ki2c_confargs *ka = aux;
	uint8_t ver;

	aprint_normal("\n");
	aprint_naive("\n");

	sc->parent = parent;
	sc->sc_tag = ka->ka_tag;
	sc->sc_addr = ka->ka_addr & 0xfe;
	if (dbcool_chip_ident(sc) < 0) {
		aprint_error_dev(self, "Unrecognized dbCool chip - "
					"set-up aborted\n");
		return;
	}

	ver = dbcool_readreg(sc, DBCOOL_REVISION_REG);

	if (sc->sc_chip->flags & DBCFLAG_4BIT_VER)
		aprint_normal_dev(self, "%s dBCool(tm) Controller "
			"(rev 0x%02x, stepping 0x%02x)\n", sc->sc_chip->name,
			ver >> 4, ver & 0x0f);
	else
		aprint_normal_dev(self, "%s dBCool(tm) Controller "
			"(rev 0x%04x)\n", sc->sc_chip->name, ver);

	dbcool_setup(self);

	if (!pmf_device_register(self, dbcool_pmf_suspend, dbcool_pmf_resume))
		aprint_error_dev(self, "couldn't establish power handler\n");
}
