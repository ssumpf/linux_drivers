#
# DDE Linux 2.6 audio liberary SALSA front end
#

LIBS = dde_linux26

INC_DIR += $(REP_DIR)/src/drivers/audio_out/posix

SRC_C = pcm.c pcm_params.c cards.c control.c mixer.c hcontrol.c

vpath % $(REP_DIR)/src/drivers/audio_out/salsa
