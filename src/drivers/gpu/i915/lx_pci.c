/*
 * \brief  PCI code copied from Linux-2.6.34
 * \author Norman Feske
 * \date   2010-07-23
 */

/* Linux includes */
#include <linux/pci.h>

/* DDE-Kit includes */
#include <dde_kit/pci.h>


/**********************************
 ** Definitions from linux/pci.h **
 **********************************/

/*
 * We do not place these macros into the public header file because the DRM
 * code does not depend on them.
 */

#define PCI_DEVFN(slot, func)	((((slot) & 0x1f) << 3) | ((func) & 0x07))


/***************************************
 ** Definitions from linux/pci_regs.h **
 ***************************************/

#define PCI_STATUS              0x06
#define PCI_STATUS_CAP_LIST     0x10
#define PCI_HEADER_TYPE_NORMAL  0
#define PCI_HEADER_TYPE_BRIDGE  1
#define PCI_HEADER_TYPE_CARDBUS 2
#define PCI_CAPABILITY_LIST     0x34
#define PCI_CAP_LIST_ID         0
#define PCI_CAP_LIST_NEXT       1
#define PCI_CB_CAPABILITY_LIST	0x14


/********************************************
 ** PCI driver code from drivers/pci/pci.c **
 ********************************************/

#define PCI_FIND_CAP_TTL	48


static int __pci_bus_find_cap_start(struct pci_bus *bus,
				    unsigned int devfn, u8 hdr_type)
{
	u16 status;

	pci_bus_read_config_word(bus, devfn, PCI_STATUS, &status);
	if (!(status & PCI_STATUS_CAP_LIST))
		return 0;

	switch (hdr_type) {
	case PCI_HEADER_TYPE_NORMAL:
	case PCI_HEADER_TYPE_BRIDGE:
		return PCI_CAPABILITY_LIST;
	case PCI_HEADER_TYPE_CARDBUS:
		return PCI_CB_CAPABILITY_LIST;
	default:
		return 0;
	}

	return 0;
}


static int __pci_find_next_cap_ttl(struct pci_bus *bus, unsigned int devfn,
				   u8 pos, int cap, int *ttl)
{
	u8 id;

	while ((*ttl)--) {
		pci_bus_read_config_byte(bus, devfn, pos, &pos);
		if (pos < 0x40)
			break;
		pos &= ~3;
		pci_bus_read_config_byte(bus, devfn, pos + PCI_CAP_LIST_ID,
					 &id);
		if (id == 0xff)
			break;
		if (id == cap)
			return pos;
		pos += PCI_CAP_LIST_NEXT;
	}
	return 0;
}


static int __pci_find_next_cap(struct pci_bus *bus, unsigned int devfn,
			       u8 pos, int cap)
{
	int ttl = PCI_FIND_CAP_TTL;

	return __pci_find_next_cap_ttl(bus, devfn, pos, cap, &ttl);
}


/*
 * Called by 'agp_intel_probe' with 'PCI_CAP_ID_AGP' (2) as argument
 */

int pci_find_capability(struct pci_dev *dev, int cap)
{
	int pos;

	pos = __pci_bus_find_cap_start(dev->bus, dev->devfn, dev->hdr_type);
	if (pos)
		pos = __pci_find_next_cap(dev->bus, dev->devfn, pos, cap);

	return pos;
}


