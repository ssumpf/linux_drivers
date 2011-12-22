/*
 * \brief  DDE Linux 2.6 NET API
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 */

#ifndef _DDE_LINUX26__NET_H_
#define _DDE_LINUX26__NET_H_

/**
 * Packet reception callback
 *
 * \param   if_index    index of the receiving network interface
 * \param   packet      buffer containing the packet
 * \param   packet_len  packet length
 */
typedef void (*dde_linux26_net_rx_cb)(unsigned if_index, const unsigned char *packet, unsigned packet_len);

/**
 * Register packet reception callback
 *
 * \param   cb   new callback function
 *
 * \return  old callback function pointer
 *
 * This registers a function pointer as rx callback. Whenever an ethernet
 * packet arrives and is processed by a driver or a softirq, it will end up in
 * either netif_rx() or netif_receive_skb(). Both will finally hand over the
 * packet to the previously registered callback.
 */
extern dde_linux26_net_rx_cb dde_linux26_net_register_rx_callback(dde_linux26_net_rx_cb cb);

/**
 * Send packet
 *
 * \param   if_index    index of the network interface to be used for sending
 * \param   packet      buffer containing the packet
 * \param   packet_len  packet length
 *
 * \return  0 on success, -1 otherwise
 */
extern int dde_linux26_net_tx(unsigned if_index, const unsigned char *packet, unsigned packet_len);

/**
 * Get MAC address of device
 *
 * \param   if_index      index of the network interface
 * \param   out_mac_addr  buffer for MAC address (buffer size must be 6 byte)
 *
 * \return  0 on success, -1 otherwise
 */
extern int dde_linux26_net_get_mac_addr(unsigned if_index, unsigned char *out_mac_addr);

/**
 * Initialize network sub-system
 *
 * \return  number of network devices
 */
extern int dde_linux26_net_init(void);

#endif /* _DDE_LINUX26__NET_H_ */
