OFFLOAD_HIF_GMAC = ../../../hif/gmac/

OFFLOAD_TGT_INC     = ../../../
OFFLOAD_DRV_INC     = $(OFFLOAD_TGT_INC)/../
OFFLOAD_STACK_INC   = $(OFFLOAD_DRV_INC)../
OFFLOAD_HIF_INC     = ../../hif

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)/include
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_DRV_INC)/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_TGT_INC)/linux/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_HIF_INC)/gmac


ifeq ($(strip $(GMAC_FRAGMENT_SUPPORT)),1)
EXTRA_CFLAGS    += -DGMAC_FRAGMENT_SUPPORT
endif

ifeq ($(strip $(OFFLOAD_GMAC_8021Q)),1)
EXTRA_CFLAGS    += -DGMAC_8021Q_SUPPORT
endif

obj-m          += hif_gmac.o
hif_gmac-objs    := $(OFFLOAD_HIF_GMAC)/hif_gmac.o



