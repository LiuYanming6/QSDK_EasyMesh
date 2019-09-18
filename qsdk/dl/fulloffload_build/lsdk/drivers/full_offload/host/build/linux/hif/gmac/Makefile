OFFLOAD_GMAC	    = ../../../../hif/gmac

OFFLOAD_ADF_INC     = ../../../../../../adf
OFFLOAD_STACK_INC   = ../../../../include
OFFLOAD_GMAC_INC    = ../../hif/gmac

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/nbuf
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/net
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/os
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/linux/os
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/linux/net
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/linux/nbuf
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_GMAC_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_GMAC_INC)/linux
EXTRA_CFLAGS    += -Werror

ifeq ($(strip $(GMAC_FRAGMENT_SUPPORT)),1)
EXTRA_CFLAGS    += -DGMAC_FRAGMENT_SUPPORT
endif

ifeq ($(BOOT),rom)
EXTRA_CFLAGS    += -DROM_MODE
ifeq  ($(strip $(OFFLOAD_TGT_RECOVERY)),1)
EXTRA_CFLAGS    += -DOFFLOAD_TGT_RECOVERY
endif
endif

obj-m           += hif_gmac.o
hif_gmac-objs   := $(OFFLOAD_GMAC)/hif_gmac.o

