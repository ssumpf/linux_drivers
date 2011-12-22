/*
 * \brief   DDE Linux 2.6 test program
 * \author  Christian Helmuth
 * \date    2009-11-12
 */

#include <base/printf.h>
#include <base/sleep.h>

extern "C" {
#include <dde_kit/lock.h>
#include <dde_linux26/general.h>
}


/* in dde_linux26_test lib (test.c) */
extern "C" void test_main(void);


/**************************
 ** Initialization calls **
 **************************/

extern int (*dde_kit_initcall_1_dde_linux26_page_cache_init)(void);
extern int (*dde_kit_initcall_1_helper_init)(void);
extern int (*dde_kit_initcall_2_pci_driver_init)(void);
extern int (*dde_kit_initcall_2_pcibus_class_init)(void);
extern int (*dde_kit_initcall_3_bla)(void);
extern int (*dde_kit_initcall_4__call_init_workqueues)(void);
extern int (*dde_kit_initcall_4_blub)(void);
extern int (*dde_kit_initcall_4_dde_linux26_init_pci)(void);
extern int (*dde_kit_initcall_6_bar)(void);
extern int (*dde_kit_initcall_6_foo)(void);
extern int (*dde_kit_initcall_6_latency_init)(void);
extern int (*dde_kit_initcall_6_pci_init)(void);

void do_initcalls(void)
{
	dde_kit_initcall_1_dde_linux26_page_cache_init();
	dde_kit_initcall_1_helper_init();
	dde_kit_initcall_2_pci_driver_init();
	dde_kit_initcall_2_pcibus_class_init();
	dde_kit_initcall_3_bla();
	dde_kit_initcall_4__call_init_workqueues();
	dde_kit_initcall_4_blub();
	dde_kit_initcall_4_dde_linux26_init_pci();
	dde_kit_initcall_6_bar();
	dde_kit_initcall_6_foo();
	dde_kit_initcall_6_latency_init();
	dde_kit_initcall_6_pci_init();
}


/******************
 ** Main program **
 ******************/

int main(int argc, char **argv)
{
	dde_linux26_init();
	do_initcalls();

	test_main();

	Genode::sleep_forever();
	return 0;
}
