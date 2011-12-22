#
# DDE Linux 2.6 USB virtual host controller library
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

SRC_C = usb_vhcd.c

vpath % $(REP_DIR)/src/lib/dde_linux26/arch/dde_kit
