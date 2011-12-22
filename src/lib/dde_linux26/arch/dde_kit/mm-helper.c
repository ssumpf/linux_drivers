/*
 * \brief  DDE Linux 2.6 memory helpers
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 */

/* Linux */
#include <linux/gfp.h>
#include <linux/string.h>
#include <asm/page.h>

/* DDE kit */
#include <dde_kit/memory.h>
#include <dde_kit/assert.h>
#include <dde_kit/panic.h>

#include "local.h"

int ioprio_best(unsigned short aprio, unsigned short bprio)
{
	WARN_UNIMPL;
	return 0;
}
