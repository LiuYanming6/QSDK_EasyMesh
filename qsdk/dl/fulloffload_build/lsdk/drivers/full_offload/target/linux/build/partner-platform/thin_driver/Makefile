
OFFLOAD_TGT_INC     = ../../../
OFFLOAD_DRV_INC     = $(OFFLOAD_TGT_INC)/../
OFFLOAD_STACK_INC   = $(OFFLOAD_DRV_INC)../
ACFG_INC            = $(OFFLOAD_DRV_INC)/../../

OFFLOAD_THIN_INC    = ../../thin_driver
OFFLOAD_HIF_INC     = ../../hif/

OFFLOAD_THIN_DRV = ../../../thin_driver

#EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)/include 
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_DRV_INC)/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_TGT_INC)/linux/include/
EXTRA_CFLAGS    += -I$(PWD)/$(ACFG_INC)/include/

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_THIN_INC)/wmi/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_THIN_INC)/htc/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_THIN_INC)/atd/

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_HIF_INC)/$(BUS)
EXTRA_CFLAGS    += -DATD_DEBUG_ENABLE

EXTRA_CFLAGS    += -DWMI_RETRY=1

EXTRA_CFLAGS    += -DBUILD_PARTNER_PLATFORM

ifeq ($(BUS), usb)
EXTRA_CFLAGS    += -DNETDEV_ALLMULTI
endif

#
# WLAN Driver includes
#
ifeq (${DRIVERS_WLAN_INC},)
DRIVERS_WLAN_INC    = ../../../../../../drivers/wlan_modules
else
EXTRA_CFLAGS 		+= -I$(DRIVERS_WLAN_INC)
endif
EXTRA_CFLAGS    	+= $(COPTS)
EXTRA_CFLAGS 		+= -I$(PWD)/$(DRIVERS_WLAN_INC) 
EXTRA_CFLAGS        += -I$(PWD)/$(DRIVERS_WLAN_INC)/include 
EXTRA_CFLAGS        += -I$(PWD)/$(DRIVERS_WLAN_INC)/include/sys 
EXTRA_CFLAGS        += -I$(PWD)/$(DRIVERS_WLAN_INC)/os/linux/include 
EXTRA_CFLAGS        += -I$(PWD)/$(DRIVERS_WLAN_INC)/lmac/ath_dev
EXTRA_CFLAGS        += -I$(PWD)/$(DRIVERS_WLAN_INC)/adf/net/linux
EXTRA_CFLAGS    	+= -D_BIG_ENDIAN -D_BYTE_ORDER=_BIG_ENDIAN

ifneq ($(strip ${ATH_SUPPORT_SPECTRAL}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_SPECTRAL=0
endif
ifneq ($(strip ${ATH_WOW}),1)
EXTRA_CFLAGS    	+= -DATH_WOW=0
endif
ifneq ($(strip ${ATH_SUPPORT_CFEND}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_CFEND=0
endif
ifneq ($(strip ${ATH_SUPPORT_IBSS_HT}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_IBSS_HT=0
endif
ifneq ($(strip ${ATH_SUPPORT_IBSS_DFS}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_IBSS_DFS=0
endif
ifneq ($(strip ${ATH_SUPPORT_WAPI}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_WAPI=0
endif
ifneq ($(strip ${ATH_SUPPORT_AOW}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_AOW=0
endif
ifneq ($(strip ${APPLE}),1)
EXTRA_CFLAGS    	+= -DAPPLE=0
endif
ifneq ($(strip ${ATH_SUPPORT_IQUE}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_IQUE=0
endif
ifneq ($(strip ${ATH_SUPPORT_VOWEXT}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_VOWEXT=0
endif
ifneq ($(strip ${ATH_VOW_EXT_STATS}),1)
EXTRA_CFLAGS    	+= -DATH_VOW_EXT_STATS=0
endif
ifeq ($(strip ${TARGET_STANDALONE}),1)
EXTRA_CFLAGS    	+= -DTARGET_STANDALONE=1
else
EXTRA_CFLAGS    	+= -DTARGET_STANDALONE=0
endif
ifneq ($(strip ${UMAC_SUPPORT_VI_DBG}),1)
EXTRA_CFLAGS    	+= -DUMAC_SUPPORT_VI_DBG=0
endif
ifneq ($(strip ${ATH_C3_WAR}),1)
EXTRA_CFLAGS    	+= -DATH_C3_WAR=0
endif
ifneq ($(strip ${ATH_SUPPORT_IBSS_NOTIFICATION}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_IBSS_NETLINK_NOTIFICATION=0
endif
ifneq ($(strip ${ATH_ANI_NOISE_SPUR_OPT}),1)
EXTRA_CFLAGS    	+= -DATH_ANI_NOISE_SPUR_OPT=0
endif
ifneq ($(strip ${ATH_SUPPORT_5G_EACS}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_5G_EACS=0
endif
ifneq ($(strip ${UMAC_SUPPORT_RRM_MISC}),1)
EXTRA_CFLAGS    	+= -DUMAC_SUPPORT_RRM_MISC=0
endif
ifneq ($(strip ${ATH_SUPPORT_LINKDIAG_EXT}),1)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_LINKDIAG_EXT=0
endif
ifneq ($(strip ${ATH_SUPPORT_UAPSD}),0)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_UAPSD=1
endif
ifneq ($(strip ${ATH_SUPPORT_TxBF}),0)
EXTRA_CFLAGS    	+= -DATH_SUPPORT_TxBF=1
endif
ifneq ($(strip ${BIG_ENDIAN_HOST}),0)
EXTRA_CFLAGS    	+= -DBIG_ENDIAN_HOST=1
endif

obj-m       += atd.o
atd-objs    := $(OFFLOAD_THIN_DRV)/htc/htc.o
atd-objs    += $(OFFLOAD_THIN_DRV)/atd/atd_cfg.o
#atd-objs    += $(OFFLOAD_THIN_DRV)/atd/atd_profile.o
atd-objs    += $(OFFLOAD_THIN_DRV)/atd/atd_dev.o
atd-objs    += $(OFFLOAD_THIN_DRV)/atd/atd_wsupp.o
atd-objs    += $(OFFLOAD_THIN_DRV)/wmi/wmi.o
atd-objs    += $(OFFLOAD_THIN_DRV)/atd/atd_netlink.o

EXTRA_CFLAGS    += -DATH_SUPPORT_FLOWMAC_MODULE=0

ifeq ($(strip ${OFFLOAD_LAN_BYPASS}),1)
EXTRA_CFLAGS    += -DOFFLOAD_LAN_BYPASS=1
atd-objs        += $(OFFLOAD_THIN_DRV)/atd/atd_byp.o
else
EXTRA_CFLAGS    += -DOFFLOAD_LAN_BYPASS=0
endif

ifeq ($(strip ${OFFLOAD_APPBR}),1)
EXTRA_CFLAGS    += -DOFFLOAD_APPBR=1
atd-objs        += $(OFFLOAD_THIN_DRV)/atd/atd_appbr.o
endif

ifeq ($(strip $(OFFLOAD_GMAC_8021Q)),1)
EXTRA_CFLAGS    += -DGMAC_8021Q_SUPPORT
endif
