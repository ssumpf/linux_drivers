/*
 * \brief  Interfaces internal to the Linux emulation environment
 * \author Norman Feske
 * \date   2010-08-03
 */

#ifndef _LX_EMUL_PRIV_H_
#define _LX_EMUL_PRIV_H_

/* Linux emulation environment includes */
#include <lx_emul.h>

/* Genode includes */
#include <pci_device/capability.h>

struct Lx_pci_dev : public pci_dev
{
	Pci::Device_capability cap;
};

/*
 * Symbols provided by driver.cc, specific for the i915 driver
 */
extern struct Lx_pci_dev bridge_pdev;
extern struct Lx_pci_dev gpu_pdev;

#endif /*_LX_EMUL_PRIV_H_ */
