/*
 * \brief   DDE Linux 2.6 monolithic USB service
 * \author  Christian Helmuth
 * \author  Christian Prochaska
 * \date    2011-07-15
 *
 * The following services are provided:
 *
 * - Input from HID mouse, keyboard, etc.
 * - Block from storage
 */

#include <base/printf.h>
#include <base/rpc_server.h>
#include <base/sleep.h>

#include <cap_session/connection.h>

#include <os/config.h>
#include <util/xml_node.h>

extern "C" {
#include <dde_linux26/general.h>
}


using namespace Genode;

extern void start_input_service(Rpc_entrypoint *ep, Xml_node hid_subnode);
extern void start_storage_service(Rpc_entrypoint *ep, Xml_node storage_subnode);


/**************************
 ** Initialization calls **
 **************************/

extern int (*dde_kit_initcall_1_dde_linux26_page_cache_init)(void);
extern int (*dde_kit_initcall_1_helper_init)(void);
extern int (*dde_kit_initcall_2_pci_driver_init)(void);
extern int (*dde_kit_initcall_2_pcibus_class_init)(void);
extern int (*dde_kit_initcall_4__call_init_workqueues)(void);
extern int (*dde_kit_initcall_4_dde_linux26_init_pci)(void);
extern int (*dde_kit_initcall_4_input_init)(void); /* input-specific */
extern int (*dde_kit_initcall_4_usb_init)(void);
extern int (*dde_kit_initcall_6_ehci_hcd_init)(void);
extern int (*dde_kit_initcall_6_hid_init)(void); /* input-specific */
extern int (*dde_kit_initcall_6_ohci_hcd_pci_init)(void);
extern int (*dde_kit_initcall_6_pci_init)(void);
extern int (*dde_kit_initcall_6_uhci_hcd_init)(void);
extern int (*dde_kit_initcall_6_usb_stor_init)(void); /* storage-specific */

static void do_initcalls(void)
{
	dde_kit_initcall_1_dde_linux26_page_cache_init();
	dde_kit_initcall_1_helper_init();
	dde_kit_initcall_2_pci_driver_init();
	dde_kit_initcall_2_pcibus_class_init();
	dde_kit_initcall_4__call_init_workqueues();
	dde_kit_initcall_4_dde_linux26_init_pci();
	dde_kit_initcall_4_input_init();
	dde_kit_initcall_4_usb_init();
	dde_kit_initcall_6_ehci_hcd_init();
	dde_kit_initcall_6_hid_init();
	dde_kit_initcall_6_ohci_hcd_pci_init();
	dde_kit_initcall_6_pci_init();
	dde_kit_initcall_6_uhci_hcd_init();
	dde_kit_initcall_6_usb_stor_init();
}


/******************
 ** Main program **
 ******************/

int main(int argc, char **argv)
{
	/*
	 * Initialize server entry point
	 */
	enum { STACK_SIZE = 4096 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "usb_ep");

	dde_linux26_init();
	PDBG("--- initcalls");
	do_initcalls();

	try {
		Xml_node hid_subnode = config()->xml_node().sub_node("hid");
		start_input_service(&ep, hid_subnode);
	} catch (Config::Invalid) {
		PDBG("No <config> node found - not starting any USB services");
		return 0;
	} catch (Xml_node::Nonexistent_sub_node) {
		PDBG("No <hid> config node found - not starting the USB HID (Input) service");
	}

	try {
		Xml_node storage_subnode = config()->xml_node().sub_node("storage");
		start_storage_service(&ep, storage_subnode);
	} catch (Xml_node::Nonexistent_sub_node) {
		PDBG("No <storage> config node found - not starting the USB Storage (Block) service");
	}

	sleep_forever();
	return 0;
}
