#
# DDE Linux 2.6 core library (depending on DDE kit)
#

LIBS = dde_kit dde_linux26-arch dde_linux26-drivers dde_linux26-kernel \
       dde_linux26-lib

#
# Include common configuration
#
include $(REP_DIR)/lib/import/import-dde_linux26.mk

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

SRC_C = mempool.c util.c

vpath % $(REP_DIR)/src/lib/dde_linux26/mm
vpath % $(REP_DIR)/src/lib/dde_linux26/security
vpath % $(REP_DIR)/src/linux26/mm
vpath % $(REP_DIR)/src/linux26/security
