OFFLOAD_HIF_LOOP = ../../../hif/loop

OFFLOAD_TGT_INC     = ../../../
OFFLOAD_DRV_INC     = $(OFFLOAD_TGT_INC)/../
OFFLOAD_STACK_INC   = $(OFFLOAD_DRV_INC)../
OFFLOAD_HIF_INC     = ../../hif

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)/include
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_DRV_INC)/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_TGT_INC)/linux/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_HIF_INC)/$(BUS)

EXTRA_CFLAGS    += -DBUILD_PARTNER_PLATFORM

obj-m           += hif_loop.o
hif_loop-objs   := $(OFFLOAD_HIF_LOOP)/hif_loop.o



