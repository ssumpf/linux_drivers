/*
 * \brief  Dummy functions for the atheros driver.
 * \author Stefan Kalkowski
 * \date   2010-07-28
 */

#include <linux/rtnetlink.h>
#include <linux/random.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <dde_linux26/general.h>

static struct sock *rtnl;

void __rta_fill(struct sk_buff *skb, int attrtype, int attrlen, const void *data)
{
	WARN_UNIMPL;
}


int rtnl_notify(struct sk_buff *skb, u32 pid, u32 group,
		struct nlmsghdr *nlh, gfp_t flags)
{
	WARN_UNIMPL;
	return 0;
}

void get_random_bytes(void *buf, int nbytes)
{
	WARN_UNIMPL;
}
