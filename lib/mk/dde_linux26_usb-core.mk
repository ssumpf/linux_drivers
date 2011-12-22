#
# DDE Linux 2.6 USB core library
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

SRC_C = buffer.c config.c devio.c driver.c endpoint.c file.c generic.c \
        hcd-pci.c hcd.c hub.c message.c notify.c sysfs.c urb.c usb.c

vpath % $(REP_DIR)/src/lib/dde_linux26/drivers/usb/core
vpath % $(REP_DIR)/src/linux26/drivers/usb/core
