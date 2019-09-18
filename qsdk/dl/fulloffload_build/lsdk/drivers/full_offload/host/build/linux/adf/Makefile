ROOT_DIR        =  ../../../../../../
OFFLOAD_ADF_DIR = $(ROOT_DIR)/drivers/adf


OFFLOAD_ADF_INC     = ../../../../adf
ROOT_INC           = ../../../../../




EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/nbuf
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/net
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/os
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/nbuf/linux
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/net/linux
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/os/linux
EXTRA_CFLAGS	    += -I$(PWD)/$(ROOT_INC)/include
EXTRA_CFLAGS	    += -I$(PWD)/$(ROOT_INC)/drivers/wlan_modules/include

EXTRA_CFLAGS+=        -Wno-unused-function
#
# Standard includes
#
include $(PWD)/common.mk

#
# ADF specific includes
#
EXTRA_CFLAGS        += -Werror

ifeq ($(MOD_SUFFIX),_offload)
EXTRA_CFLAGS += -D'MODULE_SYMBOL_PREFIX="offload_"' 
endif

#
# Module name
#
obj-m		+=	adf$(MOD_SUFFIX).o

#
# ADF standard modules
#
adf$(MOD_SUFFIX)-objs	:=	$(OFFLOAD_ADF_DIR)/os/linux/adf_os_irq_pvt.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/os/linux/adf_os_defer_pvt.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/net/linux/adf_net_ioctl.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/net/linux/adf_net_wext.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/net/linux/adf_net_event.o 
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/os/linux/adf_os_netlink_pvt.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/os/linux/adf_os_mem_pvt.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/net/linux/adf_net.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/os/linux/adf_os_lock_pvt.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/net/linux/adf_net.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/os/adf_os_lock.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/os/adf_os_mem.o
adf$(MOD_SUFFIX)-objs	+=	$(OFFLOAD_ADF_DIR)/nbuf/linux/adf_nbuf.o



EXTRA_CFLAGS+= -DADF_NET_IOCTL_SUPPORT


###########################################
#### Add your conditional modules here ####
###########################################

ifneq ($(strip ${NO_SIMPLE_CONFIG}),1)
EXTRA_CFLAGS += -DNO_SIMPLE_CONFIG=0
else
EXTRA_CFLAGS += -DNO_SIMPLE_CONFIG=1
endif

#
# PCIE Bus specific modules
#
ifeq ($(strip $(HIF)),pcie) # {

EXTRA_CFLAGS	+= -DADF_DMA_SUPPORT
EXTRA_CFLAGS	+= -DHIF_PCIE

#adf$(MOD_SUFFIX)-objs		+=	$(OFFLOAD_ADF_DIR)/linux/os/adf_os_irq_pvt.o
#adf$(MOD_SUFFIX)-objs		+=	$(OFFLOAD_ADF_DIR)/linux/os/adf_os_pci_pvt.o

endif # } HIF_PCIE

#
#  TDLS feature support
#
ifneq ($(strip ${ATH_SUPPORT_TDLS}),1)
EXTRA_CFLAGS	+= -DWEXT_SUPPORT_TDLS=0
else
EXTRA_CFLAGS	+= -DWEXT_SUPPORT_TDLS=1

ifneq ($(strip ${ATH_TDLS_AUTO_CONNECT}),1)
EXTRA_CFLAGS	+= -DWEXT_TDLS_AUTO_CONNECT=0
else
EXTRA_CFLAGS	+= -DWEXT_TDLS_AUTO_CONNECT=1
endif

endif

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
