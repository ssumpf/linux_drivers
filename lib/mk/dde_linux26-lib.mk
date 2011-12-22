#
# DDE Linux 2.6 lib library
#

#
# Include common configuration
#
include $(REP_DIR)/lib/import/import-dde_linux26.mk

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

SRC_C = bitmap.c bitrev.c cpumask.c crc32.c ctype.c hweight.c idr.c iomap.c \
        kernel_lock.c klist.c kobject.c kref.c parser.c radix-tree.c rwsem.c \
        semaphore-sleepers.c sha1.c string.c vsprintf.c find_next_bit.c

vpath % $(REP_DIR)/src/lib/dde_linux26/lib
vpath % $(REP_DIR)/src/linux26/lib
