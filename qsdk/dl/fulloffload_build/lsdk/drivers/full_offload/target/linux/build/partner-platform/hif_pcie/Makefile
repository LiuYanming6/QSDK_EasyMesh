OFFLOAD_HIF_PCIE = ../../../hif/pcie

OFFLOAD_TGT_INC     = ../../../
OFFLOAD_DRV_INC     = $(OFFLOAD_TGT_INC)/../
OFFLOAD_STACK_INC   = $(OFFLOAD_DRV_INC)../

OFFLOAD_HIF_INC     = ../../hif/

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)/include
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_DRV_INC)/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_TGT_INC)/linux/include/

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_HIF_INC)/pcie/
EXTRA_CFLAGS    += -Werror

EXTRA_CFLAGS    += -DBUILD_PARTNER_PLATFORM

obj-m   += hif_pcie.o
hif_pcie-objs    := $(OFFLOAD_HIF_PCIE)/hif_pcie.o
hif_pcie-objs    += $(OFFLOAD_HIF_PCIE)/ath_pci_tgt.o


