#
# DDE Linux 2.6 arch library
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

SRC_C = cli_sti.c fs.c hw-helpers.c init.c init_task.c irq.c kmalloc.c \
        kmem_cache.c page_alloc.c param.c pci.c power.c process.c res.c \
        sched.c signal.c smp.c softirq.c timer.c vmalloc.c vmstat.c \
        printk.c \
        dummies.c

SRC_S = semaphore.S checksum.S

vpath % $(REP_DIR)/src/lib/dde_linux26/arch/dde_kit
vpath % $(REP_DIR)/src/lib/dde_linux26/arch/i386/lib
vpath % $(REP_DIR)/src/linux26/arch/i386/lib
