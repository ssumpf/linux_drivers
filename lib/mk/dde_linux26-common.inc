# for local.h
INC_DIR += $(REP_DIR)/src/lib/dde_linux26/arch/dde_kit

#CC_OPT += -DDEBUG

#
# The content of 'ieee80211_ioctl.h' is tailored to either Linux or FreeBSD.
# Both platforms are distinguished via defines normally implicitly provided by
# the compiler. The Genode tool chain provides neither of both defines. Hence,
# we have to provide the desired definition manually. Similar problems exist
# for other Linux subsystems as well.
#
CC_OPT += -D__linux__

