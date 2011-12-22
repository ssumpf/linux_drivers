#
# DDE Linux 2.6 drivers library
#

LIBS = dde_linux26-drivers_pci

#
# Include common configuration
#
include $(REP_DIR)/lib/import/import-dde_linux26.mk

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

INC_DIR += $(REP_DIR)/src/linux26/drivers/base

SRC_C = sys.c init.c attribute_container.c bus.c class.c core.c cpu.c dd.c \
        dmapool.c driver.c init.c map.c platform.c sys.c

vpath % $(REP_DIR)/src/lib/dde_linux26/drivers/base
vpath % $(REP_DIR)/src/lib/dde_linux26/drivers/char
vpath % $(REP_DIR)/src/lib/dde_linux26/drivers/firmware
vpath % $(REP_DIR)/src/linux26/drivers/base
vpath % $(REP_DIR)/src/linux26/drivers/char
vpath % $(REP_DIR)/src/linux26/drivers/firmware
