/* $NetBSD: pciif.h,v 1.3 2007/10/17 19:58:31 garbled Exp $ */
/*
 * PCI Backend/Frontend Common Data Structures & Macros
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 *   Author: Ryan Wilson <hap9@epoch.ncsc.mil>
 */
#ifndef __XEN_PCI_COMMON_H__
#define __XEN_PCI_COMMON_H__

/* Be sure to bump this number if you change this file */
#define XEN_PCI_MAGIC "7"

/* xen_pci_sharedinfo flags */
#define _XEN_PCIF_active     (0)
#define XEN_PCIF_active      (1<<_XEN_PCI_active)

/* xen_pci_op commands */
#define XEN_PCI_OP_conf_read    (0)
#define XEN_PCI_OP_conf_write   (1)

/* xen_pci_op error numbers */
#define XEN_PCI_ERR_success          (0)
#define XEN_PCI_ERR_dev_not_found   (-1)
#define XEN_PCI_ERR_invalid_offset  (-2)
#define XEN_PCI_ERR_access_denied   (-3)
#define XEN_PCI_ERR_not_implemented (-4)
/* XEN_PCI_ERR_op_failed - backend failed to complete the operation */
#define XEN_PCI_ERR_op_failed       (-5)

struct xen_pci_op {
    /* IN: what action to perform: XEN_PCI_OP_* */
    uint32_t cmd;

    /* OUT: will contain an error number (if any) from errno.h */
    int32_t err;

    /* IN: which device to touch */
    uint32_t domain; /* PCI Domain/Segment */
    uint32_t bus;
    uint32_t devfn;

    /* IN: which configuration registers to touch */
    int32_t offset;
    int32_t size;

    /* IN/OUT: Contains the result after a READ or the value to WRITE */
    uint32_t value;
};

struct xen_pci_sharedinfo {
    /* flags - XEN_PCIF_* */
    uint32_t flags;
    struct xen_pci_op op;
};

#endif /* __XEN_PCI_COMMON_H__ */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
