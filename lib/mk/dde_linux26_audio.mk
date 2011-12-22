#
# DDE Linux 2.6 audio liberary (providing SALSA front end)
#

LIBS = dde_linux26 dde_linux26_audio-salsa

INC_DIR += $(REP_DIR)/src/drivers/audio_out/posix \
           $(REP_DIR)/src/drivers/audio_out/salsa

#
# DDE audio library
#
SRC_C = audio.c dde.c linux_glue_dde.c vmalloc.c

#
# ALSA core
#
SRC_C += sound_core.c \
         init.c control.c device.c sound.c pcm.c pcm_native.c pcm_lib.c timer.c \
         pcm_timer.c pcm_misc.c pcm_memory.c sgbuf.c memalloc.c misc.c vmaster.c \
         last.c

#
# Drivers
#
# AC97 (support)
SRC_C += ac97_codec.c ac97_pcm.c ak4531_codec.c ac97_bus.c ac97_patch.c

# QEMU hardware
SRC_C += ens1370.c rawmidi.c intel8x0.c #ens1371.c

# HDA Intel
SRC_C += hda_codec.c hda_generic.c hda_intel.c patch_analog.c hda_generic.c

vpath % $(REP_DIR)/src/drivers/audio_out
vpath % $(REP_DIR)/src/drivers/audio_out/drivers
vpath % $(REP_DIR)/src/drivers/audio_out/drivers/core
vpath % $(REP_DIR)/src/drivers/audio_out/drivers/pci
vpath % $(REP_DIR)/src/drivers/audio_out/drivers/pci/ac97
vpath % $(REP_DIR)/src/drivers/audio_out/drivers/pci/hda
vpath % $(REP_DIR)/src/drivers/audio_out/glue
