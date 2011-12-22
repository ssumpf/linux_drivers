#
# DDE Linux 2.6 USB host controller driver library
#

#
# Include common configuration
#
include $(REP_DIR)/lib/import/import-dde_linux26.mk

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

INC_DIR += $(REP_DIR)/src/linux26/drivers/usb/core

SRC_C = ehci-hcd.c ohci-hcd.c pci-quirks.c uhci-hcd.c

vpath % $(REP_DIR)/src/linux26/drivers/usb/host
