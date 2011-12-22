/*
 * \brief   uIP network stack for DDE Linux 2.6 NET test
 * \author  Christian Helmuth
 * \date    2009-02-11
 */

#include <base/printf.h>

extern "C" {
#include <dde_kit/timer.h>
#include <dde_kit/printf.h>

#include <stdio.h>

#include <uip/uip.h>
#include <uip/timer.h>
#include <uip/uip_arp.h>

#include <apps/dhcpc/dhcpc.h>

#include "clock-arch.h"
}

#include "local.h"


/************************
 ** ANSI C used by uIP **
 ************************/

extern "C" int printf(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	Genode::vprintf(fmt, va);
	va_end(va);

	return 0;
}


extern "C" int puts(const char *msg)
{
	Genode::printf(msg);

	return 0;
}


/**************************
 ** Platform API for uIP **
 **************************/

clock_time_t clock_time(void)
{
	return dde_kit_timer_ticks;
}


extern "C" void uip_log(char *msg)
{
	PDBG("%s\n", msg);
}


extern "C" void uip_appcall(void)
{
	PDBG("called");
}


/************************
 ** DHCP configuration **
 ************************/

void dhcpc_configured(const struct dhcpc_state *s)
{
	uip_sethostaddr(s->ipaddr);
	uip_setnetmask(s->netmask);
	uip_setdraddr(s->default_router);
}


/*******************
 ** uIP main loop **
 *******************/

/**
 * Packet buffer access helper
 */
static inline struct uip_eth_hdr *global_buffer()
{
	return reinterpret_cast<struct uip_eth_hdr *>(&uip_buf[0]);
}


void uip_loop(const unsigned char *mac_addr)
{
	int i;
	struct timer periodic_timer, arp_timer;
	uip_ipaddr_t ipaddr;

	uip_init();

	uip_setethaddr((*(struct uip_eth_addr *)mac_addr));

	dhcpc_init(mac_addr, 6);

	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	httpd_init();

	while (1) {
		uip_len = dev_recv((unsigned char *)uip_buf, UIP_BUFSIZE);
		if (uip_len > 0) {
			if (global_buffer()->type == htons(UIP_ETHTYPE_IP)) {
				uip_arp_ipin();
				uip_input();
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if(uip_len > 0) {
					uip_arp_out();
					dev_send((const unsigned char *)uip_buf, uip_len);
				}
			} else if (global_buffer()->type == htons(UIP_ETHTYPE_ARP)) {
				uip_arp_arpin();
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if (uip_len > 0) {
					dev_send((const unsigned char *)uip_buf, uip_len);
				}
			}

		} else if (timer_expired(&periodic_timer)) {
			timer_reset(&periodic_timer);
			for(i = 0; i < UIP_CONNS; i++) {
				uip_periodic(i);
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if(uip_len > 0) {
					uip_arp_out();
					dev_send((const unsigned char *)uip_buf, uip_len);
				}
			}

#if UIP_UDP
			for(i = 0; i < UIP_UDP_CONNS; i++) {
				uip_udp_periodic(i);
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if(uip_len > 0) {
					uip_arp_out();
					dev_send((const unsigned char *)uip_buf, uip_len);
				}
			}
#endif /* UIP_UDP */

			/* Call the ARP timer function every 10 seconds. */
			if (timer_expired(&arp_timer)) {
				timer_reset(&arp_timer);
				uip_arp_timer();
			}
		}
	}
}
