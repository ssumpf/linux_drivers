#ifndef __LINUX26__SOUND__CORE_H__
#define __LINUX26__SOUND__CORE_H__

#define CONFIG_SND_DEBUG 1

#include <sound/driver.h>

#include_next <sound/core.h>

/*
 * Snippets from Linux 2.6.27
 */

/* PCI quirk list helper */
struct snd_pci_quirk {
	unsigned short subvendor;	/* PCI subvendor ID */
	unsigned short subdevice;	/* PCI subdevice ID */
	int value;			/* value */
#ifdef CONFIG_SND_DEBUG_VERBOSE
	const char *name;		/* name of the device (optional) */
#endif
};

#define _SND_PCI_QUIRK_ID(vend,dev) \
	.subvendor = (vend), .subdevice = (dev)
#define SND_PCI_QUIRK_ID(vend,dev) {_SND_PCI_QUIRK_ID(vend, dev)}
#ifdef CONFIG_SND_DEBUG_VERBOSE
#define SND_PCI_QUIRK(vend,dev,xname,val) \
	{_SND_PCI_QUIRK_ID(vend, dev), .value = (val), .name = (xname)}
#else
#define SND_PCI_QUIRK(vend,dev,xname,val) \
	{_SND_PCI_QUIRK_ID(vend, dev), .value = (val)}
#endif

const struct snd_pci_quirk *
snd_pci_quirk_lookup(struct pci_dev *pci, const struct snd_pci_quirk *list);

#endif /* __LINUX26__SOUND__CORE_H__ */
