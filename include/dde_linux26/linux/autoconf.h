/* Include Linux' contributed autoconf file */
#include_next <linux/autoconf.h>

#undef CONFIG_MODULES
#undef CONFIG_MODULE_UNLOAD

/* Because we don't need INET support */
#undef CONFIG_INET
#undef CONFIG_XFRM
#undef CONFIG_IP_NF_IPTABLES
#undef CONFIG_IP_FIB_HASH
#undef CONFIG_NETFILTER_XT_MATCH_STATE
#undef CONFIG_INET_XFRM_MODE_TRANSPORT
#undef CONFIG_INET_XFRM_MODE_TUNNEL
#undef CONFIG_IP_NF_CONNTRACK
#undef CONFIG_TCP_CONG_BIC
#undef CONFIG_IP_NF_FILTER
#undef CONFIG_IP_NF_FTP
#undef CONFIG_IP_NF_TARGET_LOG

/* No highmem for our drivers */
#undef CONFIG_HIGHMEM
#undef CONFIG_HIGHMEM4G
#define CONFIG_NOHIGHMEM 1

/* No PROC fs for us */
#undef CONFIG_PROC_FS

/* Also, no sysFS */
#undef CONFIG_SYSFS

/* No NFS support */
#undef CONFIG_ROOT_NFS

/* We don't support hotplug */
#undef CONFIG_HOTPLUG

/* No Sysctl */
#undef CONFIG_SYSCTL

/* No power management */
#undef CONFIG_PM

/* irqs assigned statically */
#undef CONFIG_GENERIC_IRQ_PROBE

/* No message-signalled interrupts for PCI. */
#undef CONFIG_PCI_MSI

/* We don't need event counters, because we don't have /proc/vmstat */
#undef CONFIG_VM_EVENT_COUNTERS

#define WANT_PAGE_VIRTUAL

/* USB configuration */
#undef CONFIG_USB_EHCI_SPLIT_ISO
#define CONFIG_USB_EHCI_SPLIT_ISO 1
#undef CONFIG_USB_DEVICEFS
#undef CONFIG_USB_SUSPEND

/* ALSA configuration */
#define CONFIG_SND 1
#define CONFIG_SND_AC97_CODEC 1
#define CONFIG_SND_ENS1370 1
#define CONFIG_SND_ENS1371 1
#define CONFIG_SND_HDA_CODEC_ANALOG 1
#define CONFIG_SND_HDA_GENERIC 1
#define CONFIG_SND_HDA_INTEL 1
#define CONFIG_SND_INTEL8X0 1
#define CONFIG_SND_PCM 1
#define CONFIG_SND_RAWMIDI 1
#define CONFIG_SND_TIMER 1
