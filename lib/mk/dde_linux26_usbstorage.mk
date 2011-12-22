#
# DDE Linux 2.6 monolithic USB storage library
#

#
# Set to 1 for debugging, 0 to disable debugging output
#
DEBUG = 0

LIBS = dde_linux26 dde_linux26_usb-core dde_linux26_usb-hcd

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

INC_DIR += $(REP_DIR)/src/linux26/drivers/usb/storage

SRC_C = scsiglue.c protocol.c transport.c usb.c initializers.c \
        scsi.c debug.c constants.c usb_storage.c

ifneq ($(DEBUG),0)
D_OPTS = -DCONFIG_USB_STORAGE_DEBUG
CC_DEF += $(D_OPTS)
endif

vpath % $(REP_DIR)/src/lib/dde_linux26/arch/dde_kit
vpath % $(REP_DIR)/src/linux26/drivers/usb/storage
