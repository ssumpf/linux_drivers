#
# DDE Linux 2.6 monolithic USB HID library
#

LIBS = dde_linux26 dde_linux26_usb-core dde_linux26_usb-hcd \
       dde_linux26-drivers_hid

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

#INC_DIR += $(REP_DIR)/src/linux26/drivers/usb/input

SRC_C = hid-core.c input.c evdev.c

vpath % $(REP_DIR)/src/lib/dde_linux26/arch/dde_kit
vpath % $(REP_DIR)/src/linux26/drivers/usb/input
vpath % $(REP_DIR)/src/linux26/drivers/input
