/*
 * \brief  DDE Linux 2.6 initialization
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 */

#include <dde_linux26/general.h>
#include <dde_kit/dde_kit.h>

#include "local.h"

/* Didn't know where to put this. */
unsigned long __per_cpu_offset[NR_CPUS];

#include <asm/pgtable.h>
unsigned long long __PAGE_KERNEL = _PAGE_KERNEL;
EXPORT_SYMBOL(__PAGE_KERNEL);

extern void driver_init(void);


void dde_linux26_init()
{
	/* initialize DDE kit */
	dde_kit_init();

	/* initialize DDE Linux 2.6 subsystems */
	dde_linux26_printk_init();
	dde_linux26_kmalloc_init();
	dde_linux26_process_init();
	dde_linux26_timer_init();
	dde_linux26_softirq_init();

	/* add main thread as potential worker */
	dde_linux26_process_add_worker("DDE main");

	/* init Linux driver framework before trying to add PCI devs to the bus */
	driver_init();
}
