OFFLOAD_THIN	    =../../../thin_driver
OFFLOAD_THIN_INC	=../../thin_driver

FULL_OFFLOAD        = ../../../../full_offload
OFFLOAD_ADF_INC     = ../../../../adf
OFFLOAD_ASF_INC     = ../../../../asf
OFFLOAD_STACK_INC   = ../../../../include
OFFLOAD_DRV_INC     = ../../../include
OFFLOAD_HST_INC     = ../../include
OFFLOAD_HIF_INC     = ../../hif/$(HIF)

EXTRA_CFLAGS	    += -I$(PWD)/$(OFFLOAD_STACK_INC)
EXTRA_CFLAGS	    += -I$(PWD)/$(OFFLOAD_DRV_INC)
EXTRA_CFLAGS	    += -I$(PWD)/$(OFFLOAD_HST_INC)

EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_ASF_INC)
EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_ADF_INC)

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/nbuf
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/net
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/os
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/nbuf/linux
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/net/linux
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/os/linux

EXTRA_CFLAGS    += -I$(PWD)/$(FULL_OFFLOAD)/../include
EXTRA_CFLAGS    += -I$(PWD)/$(FULL_OFFLOAD)/../../include



EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_THIN_INC)/wmi/
EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_THIN_INC)/htc/
EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_THIN_INC)/atd/
EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_THIN_INC)/

EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_HIF_INC)/linux
EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_HIF_INC)

EXTRA_CFLAGS        += -I$(PWD)/../../../../wlan_modules/include

ifneq ($(strip ${ATH_SUPPORT_TxBF}),0)
EXTRA_CFLAGS        += -DATH_SUPPORT_TxBF=1
endif
ifneq ($(strip ${ATH_WOW}),1)
EXTRA_CFLAGS        += -DATH_WOW=0
endif
ifneq ($(strip ${ATH_SUPPORT_UAPSD}),0)
EXTRA_CFLAGS        += -DATH_SUPPORT_UAPSD=1
endif
ifneq ($(strip ${ATH_SUPPORT_VOWEXT}),1)
EXTRA_CFLAGS        += -DATH_SUPPORT_VOWEXT=0
endif
ifneq ($(strip ${ATH_SUPPORT_CFEND}),1)
EXTRA_CFLAGS        += -DATH_SUPPORT_CFEND=0
endif
ifneq ($(strip ${UMAC_SUPPORT_VI_DBG}),1)
EXTRA_CFLAGS        += -DUMAC_SUPPORT_VI_DBG=0
endif
ifneq ($(strip ${BIG_ENDIAN_HOST}),0)
EXTRA_CFLAGS        += -DBIG_ENDIAN_HOST=1
endif
ifneq ($(strip ${ATH_RX_LOOPLIMIT_TIMER}),1)
EXTRA_CFLAGS        += -DATH_RX_LOOPLIMIT_TIMER=0
endif

EXTRA_CFLAGS        += -DATD_DEBUG_ENABLE=1
EXTRA_CFLAGS        += -Werror

EXTRA_CFLAGS        += -DWMI_RETRY=1

ifeq ($(MOD_SUFFIX),_offload)
EXTRA_CFLAGS += -D'MODULE_SYMBOL_PREFIX="offload_"' 
endif

obj-m				+= atd$(MOD_SUFFIX).o
atd$(MOD_SUFFIX)-objs			:= $(OFFLOAD_THIN)/wmi/wmi.o
atd$(MOD_SUFFIX)-objs			+= $(OFFLOAD_THIN)/htc/htc.o
atd$(MOD_SUFFIX)-objs			+= $(OFFLOAD_THIN)/atd/atd_cfg.o
atd$(MOD_SUFFIX)-objs			+= $(OFFLOAD_THIN)/atd/atd_profile.o
atd$(MOD_SUFFIX)-objs			+= $(OFFLOAD_THIN)/atd/atd_dev.o
atd$(MOD_SUFFIX)-objs			+= $(OFFLOAD_THIN)/atd/atd_wsupp.o

ifeq ($(strip $(OFFLOAD_LAN_BYPASS)),1)
EXTRA_CFLAGS    += -DOFFLOAD_LAN_BYPASS=1
atd$(MOD_SUFFIX)-objs        += $(OFFLOAD_THIN)/atd/atd_byp.o
else
EXTRA_CFLAGS    += -DOFFLOAD_LAN_BYPASS=0
endif

ifeq ($(strip $(OFFLOAD_APPBR)),1)
EXTRA_CFLAGS    += -DOFFLOAD_APPBR=1
atd$(MOD_SUFFIX)-objs        += $(OFFLOAD_THIN)/atd/atd_appbr.o
endif

