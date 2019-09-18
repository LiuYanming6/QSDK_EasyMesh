OFFLOAD_PCIE	    = ../../../../hif/pcie
OFFLOAD_HST_TGT_COMMON_INC  = ../../../include


OFFLOAD_ADF_INC     = ../../../../adf
OFFLOAD_ASF_INC     = ../../../../asf



OFFLOAD_STACK_INC   = ../../../../include
OFFLOAD_PCIE_INC    = ../../hif/pcie

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ATD_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_HST_TGT_COMMON_INC)

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/nbuf
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/net
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/os
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/nbuf/linux
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/net/linux
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/os/linux


EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_PCIE_INC)
#EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_PCIE_INC)/linux
EXTRA_CFLAGS    += -Werror

obj-m			+= hif_pcie.o
#hif_pcie-objs	:= $(OFFLOAD_PCIE)/linux/hif_pcie.o
hif_pcie-objs	:= $(OFFLOAD_PCIE)/hif_pcie.o
#hif_pcie-objs	+= $(OFFLOAD_PCIE)/linux/hif_pcie_dma.o
hif_pcie-objs	+= $(OFFLOAD_PCIE)/hif_pcie_dma.o


