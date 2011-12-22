/*
 * \brief  Linux DDE kit wifi functions.
 * \author Stefan Kalkowski
 * \date   2010-07-28
 */

#include <linux/netdevice.h>
#include <linux/wireless.h>
#include <net/iw_handler.h>

#include <dde_kit/printf.h>
#include <dde_linux26/wifi.h>


int dde_linux26_wifi_atheros_idx()
{
	struct net_device *dev = dev_get_by_name("ath0");
	return dev->ifindex;
}


void dde_linux26_wifi_set_essid(int idx, const char *essid)
{
	struct net_device *dev  = dev_get_by_index(idx);
	struct iwreq request;
	request.u.essid.flags   = 1;
	request.u.essid.pointer = (void*) essid;
	request.u.essid.length  = strlen(essid);
	strncpy(request.ifr_ifrn.ifrn_name, dev->name, IFNAMSIZ);

	if (wireless_process_ioctl((struct ifreq *)&request, SIOCSIWESSID))
		dde_kit_printf("Failed to set ESSID %s\n", essid);
}
