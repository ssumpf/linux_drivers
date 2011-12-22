TARGET  = test-dde_linux26_net
SRC_CC  = main.cc network.cc
LIBS    = cxx env dde_linux26_net

SRC_C   = uip.c uip_arp.c uiplib.c psock.c timer.c uip-neighbor.c \
          httpd.c http-strings.c httpd-fs.c httpd-cgi.c \
          dhcpc.c

#
# Use the default (relaxed) warning level to dim the warning noise when
# compiling the contributed sources.
#
CC_C_OPT += -fno-builtin
CC_WARN   =

vpath %.c $(PRG_DIR)/contrib/uip
vpath %.c $(PRG_DIR)/contrib/apps/webserver
vpath %.c $(PRG_DIR)/contrib/apps/dhcpc

INC_DIR += $(REP_DIR)/src/test/dde_linux26_net
INC_DIR += $(REP_DIR)/src/test/dde_linux26_net/contrib
INC_DIR += $(REP_DIR)/src/test/dde_linux26_net/contrib/uip
INC_DIR += $(REP_DIR)/src/test/dde_linux26_net/include
