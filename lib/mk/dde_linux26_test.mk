#
# DDE Linux 2.6 test library
#
# This library is used for the DDE Linux 2.6 test program in
# linux_drivers/src/test/dde_linux26.
#

LIBS = dde_linux26

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

SRC_C = test.c

vpath % $(REP_DIR)/src/test/dde_linux26
