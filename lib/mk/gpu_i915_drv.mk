TARGET      := gpu_i915_drv
CONTRIB_DIR := $(REP_DIR)/src/drivers/gpu/contrib
SRC_C       += $(notdir $(wildcard $(CONTRIB_DIR)/agp/*.c))
SRC_C       += $(notdir $(wildcard $(CONTRIB_DIR)/drm/i915/*.c))
SRC_C       += $(notdir $(wildcard $(CONTRIB_DIR)/drm/*.c))
INC_DIR     += $(REP_DIR)/src/drivers/gpu/i915/ \
               $(REP_DIR)/src/drivers/gpu/i915/include \
               $(REP_DIR)/src/drivers/gpu/i915/include/drm \
               $(CONTRIB_DIR)/drm \
               $(CONTRIB_DIR)/agp \
               $(CONTRIB_DIR)/include/drm \
               $(CONTRIB_DIR)/include
CC_OPT      += -U__linux__ -D__KERNEL__ -D__OS_HAS_AGP
SRC_C       += dummies.c probe.c
SRC_CC      += driver.cc lx_emul.cc lx_pci.c
LIBS        += dde_kit

#
# The following file specific type definition is needed because agp/generic.c
# uses the type __u32. Unfortunately, we cannot define this type in our
# sys/types.h because drm.h defines this type as well.
#
CC_OPT_generic += -D__u32=u32

#
# Reduce build noise of compiling contrib code
#
CC_WARN = -Wall -Wno-uninitialized

vpath %.c        $(CONTRIB_DIR)/drm/i915
vpath %.c        $(CONTRIB_DIR)/drm
vpath %.c        $(CONTRIB_DIR)/agp
vpath dummies.c  $(REP_DIR)/src/drivers/gpu/i915
vpath driver.cc  $(REP_DIR)/src/drivers/gpu/i915
vpath probe.c    $(REP_DIR)/src/drivers/gpu/i915
vpath lx_emul.cc $(REP_DIR)/src/drivers/gpu/i915
vpath lx_pci.c   $(REP_DIR)/src/drivers/gpu/i915

#
# Determine the header files included by the contrib code. For each
# of these header files we create a symlink to 'lx_emul.h'.
#
GEN_INCLUDES := $(shell grep -rh "^\#include .*\/" $(CONTRIB_DIR) |\
                        sed "s/^\#include *[<\"]\(.*\)[>\"].*/\1/" | sort | uniq)

#
# Prevent the creation of symlinks for header files that we actually
# use.
#
NO_GEN_INCLUDES := linux/agp_backend.h linux/list.h linux/pci_ids.h drm/drmP.h
GEN_INCLUDES := $(filter-out $(NO_GEN_INCLUDES),$(GEN_INCLUDES))

#
# Convert header names to absolute path names. This is needed because this
# build description file is evaluated twice, for dependency generation and
# for building. In the first case, it is executed from the build directory.
# However, we want to generate the include symlinks to the build location of
# the actual library.
#
GEN_INCLUDES := $(addprefix $(LIB_CACHE_DIR)/$(TARGET)/,$(GEN_INCLUDES))

#
# Make sure to create the header symlinks prior building
#
all: $(GEN_INCLUDES)

$(GEN_INCLUDES):
	@mkdir -p $(dir $@)
	@ln -s $(REP_DIR)/src/drivers/gpu/i915/lx_emul.h $@

