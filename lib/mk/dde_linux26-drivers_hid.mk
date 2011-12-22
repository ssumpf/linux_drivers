#
# DDE Linux 2.6 drivers/hid library
#

#
# Include common configuration
#
include $(REP_DIR)/lib/import/import-dde_linux26.mk

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

SRC_C = hid-core.c hid-input.c

vpath % $(REP_DIR)/src/linux26/drivers/hid
