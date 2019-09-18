OFFLOAD_MISC = ../../../misc
OFFLOAD_TGT_INC     = ../../../
OFFLOAD_DRV_INC     = $(OFFLOAD_TGT_INC)/../
OFFLOAD_STACK_INC   = $(OFFLOAD_DRV_INC)../
OFFLOAD_HIF_INC     = ../../hif

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)/include
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_DRV_INC)/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_TGT_INC)/linux/include/



obj-m          += xmiipsave.o
xmiipsave-objs    := $(OFFLOAD_MISC)/xmiipsave.o



