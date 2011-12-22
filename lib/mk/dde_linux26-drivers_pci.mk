#
# DDE Linux 2.6 drivers/pci library
#

#
# Include common configuration
#
include $(REP_DIR)/lib/import/import-dde_linux26.mk

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

INC_DIR += $(REP_DIR)/src/linux26/drivers/pci

SRC_C = access.c bus.c pci.c pci-driver.c probe.c search.c setup-bus.c \
        setup-res.c

vpath % $(REP_DIR)/src/lib/dde_linux26/drivers/pci
vpath % $(REP_DIR)/src/linux26/drivers/pci
