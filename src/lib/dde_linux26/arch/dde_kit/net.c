/*
 * \brief  DDE Linux 2.6 networking
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 *
 * Original header follows...
 */

/******************************************************************************
 * DDE Linux 2.6 networking utilities.                                        *
 *                                                                            *
 * Bjoern Doebel <doebel@tudos.org>                                           *
 *                                                                            *
 * (c) 2005 - 2007 Technische Universitaet Dresden                            *
 * This file is part of DROPS, which is distributed under the terms of the    *
 * GNU General Public License 2. Please see the COPYING file for details.     *
 ******************************************************************************/

#include <dde_linux26/net.h>

#include <asm/delay.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/if_ether.h>

#include "local.h"


dde_linux26_net_rx_cb current_rx_callback = NULL;


int dde_linux26_do_rx_callback(struct sk_buff *s)
{
	/*
	 * Linux drivers pull ethernet header out of data area. So, we have to push
	 * it back and hand out full packets to clients.
	 */
	skb_push(s, ETH_HLEN);

	if (current_rx_callback != NULL) {
		current_rx_callback(s->dev->ifindex, s->data, s->len);
	}

	kfree_skb(s);

	return NET_RX_SUCCESS;
}


/**************
 ** User API **
 **************/

int dde_linux26_net_tx(unsigned if_index, const unsigned char *packet, unsigned packet_len)
{
	/* find device */
	struct net_device *dev = dev_get_by_index(if_index);
	if (!dev) {
		printk("network device %d unknown\n", if_index);
		return -1;
	}

	/* prepare socket buffer */
	struct sk_buff *skb = alloc_skb(packet_len, GFP_KERNEL);
	if (!skb) {
		printk("out of memory for socket buffers\n");
		return -1;
	}
	skb_put(skb, packet_len);
	skb->dev = dev;
	memcpy(skb->data, packet, packet_len);

	/* deliver packet */
	int xmit;
	do {
		/*
		 * Wait some time (1ms) until the drivers calls netif_wake_
		 * queue(dev). We don't do l4_yield() here because that
		 * does not work if the priority of the IRQ threads(!) is
		 * less than the current priority.
		 */
		while (netif_queue_stopped(dev)) udelay(1000);

		/*
		 * Note: We could also use Linux' dev->queue_xmit() functions.
		 *       Thereby we can get rid of our hand-crafted xmit lock (it is
		 *       provided by the tx queues) and benefit from Linux' traffic
		 *       shaping algorithms. However we need to set up our own
		 *       queueing discipline beforehand, which is quite complex,
		 *       therefore we just use hard_start_xmit.
		 */
//		xmit_lock(dev->ifindex);
		xmit = dev->hard_start_xmit(skb, dev);
//		xmit_unlock(dev->ifindex);
		if (xmit) printk("Error sending packet: %d\n", xmit);
	} while (xmit != 0);

	dev_put(dev);
	return 0;
}


dde_linux26_net_rx_cb dde_linux26_net_register_rx_callback(dde_linux26_net_rx_cb cb)
{
	dde_linux26_net_rx_cb old = current_rx_callback;

	current_rx_callback = cb;

	return old;
}

int dde_linux26_net_get_mac_addr(unsigned if_index, unsigned char *out_mac_addr)
{
	/* find device */
	struct net_device *dev = dev_get_by_index(if_index);
	if (!dev) {
		printk("network device %d unknown\n", if_index);
		return -1;
	}

	memcpy(out_mac_addr, dev->dev_addr, ETH_ALEN);

	return 0;
}


int dde_linux26_net_init(void)
{
	skb_init();

	struct net_device *dev;
	int err, cnt = 0;

	for (dev = dev_base; dev; dev = dev->next) {
		err = dev_open(dev);
		if (err) {
			printk("    error opening %s : %d", dev->name, err);
			continue;
		}

		cnt++;
	}

	return cnt;
}
