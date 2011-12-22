#
# DDE Linux 2.6 kernel library
#

#
# Include common configuration
#
include $(REP_DIR)/lib/import/import-dde_linux26.mk

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

SRC_C = capability.c exit.c kthread.c mutex.c pid.c rcupdate.c resource.c \
        rwsem.c sched.c sys.c time.c wait.c workqueue.c latency.c

vpath % $(REP_DIR)/src/lib/dde_linux26/kernel
vpath % $(REP_DIR)/src/linux26/kernel
