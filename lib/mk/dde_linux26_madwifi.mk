#
# DDE Linux 2.6 Wifi library
#
LIBS = dde_linux26 dde_linux26-sleep

#
# Include local configuration of library sources
#
include $(REP_DIR)/lib/mk/dde_linux26-common.inc

INC_DIR += $(REP_DIR)/src/madwifi/include \
           $(REP_DIR)/src/madwifi/include/net80211 \
           $(REP_DIR)/src/madwifi/contrib \
           $(REP_DIR)/src/madwifi/contrib/include \
           $(REP_DIR)/src/madwifi/contrib/ath \
           $(REP_DIR)/src/madwifi/contrib/ath_hal \
           $(REP_DIR)/src/madwifi/contrib/ath_rate/sample \
           $(REP_DIR)/src/madwifi/contrib/hal \
           $(REP_DIR)/src/madwifi/contrib/net80211

# DDEKit + DDELinux26
SRC_C  = dev.c dev_mcast.c dummies.c eth.c ethtool.c link_watch.c mii.c neighbour.c \
         net.c netevent.c rtnetlink.c sch_generic.c skbuff.c sysctl.c utils.c \
         wifi.c wireless.c

# Madwifi driver
SRC_C += ah_os.c \
         ieee80211_acl.c         ieee80211_input.c      ieee80211_scan_ap.c \
         ieee80211_beacon.c      ieee80211_linux.c      ieee80211_scan.c \
         ieee80211.c             ieee80211_monitor.c    ieee80211_scan_sta.c \
         ieee80211_crypto.c      ieee80211_node.c       ieee80211_wireless.c \
         ieee80211_crypto_ccmp.c ieee80211_output.c     ieee80211_xauth.c \
         ieee80211_crypto_none.c ieee80211_power.c      ieee80211_crypto_tkip.c \
         ieee80211_proto.c       ieee80211_crypto_wep.c ieee80211_rate.c \
         if_ath.c                if_ath_pci.c           if_media.c \
         sample.c

CC_OPT += -DATH_PCI -DOPT_AH_H="<opt_ah.h>"

vpath if_ath.c $(REP_DIR)/src/madwifi/ath
vpath %        $(REP_DIR)/src/madwifi/contrib/ath
vpath %        $(REP_DIR)/src/madwifi/contrib/ath_hal
vpath %        $(REP_DIR)/src/madwifi/contrib/ath_rate/sample
vpath %        $(REP_DIR)/src/madwifi/contrib/net80211
vpath %        $(REP_DIR)/src/madwifi/dde_linux26
vpath %        $(REP_DIR)/src/madwifi/dde_linux26/kernel
vpath %        $(REP_DIR)/src/lib/dde_linux26/arch/dde_kit
vpath %        $(REP_DIR)/src/lib/dde_linux26/net/core
vpath %        $(REP_DIR)/src/lib/dde_linux26/net/netlink
vpath %        $(REP_DIR)/src/lib/dde_linux26/net/sched
vpath %        $(REP_DIR)/src/linux26/drivers/net
vpath %        $(REP_DIR)/src/linux26/net/core
vpath %        $(REP_DIR)/src/linux26/net/ethernet
vpath %        $(REP_DIR)/src/linux26/net/netlink
