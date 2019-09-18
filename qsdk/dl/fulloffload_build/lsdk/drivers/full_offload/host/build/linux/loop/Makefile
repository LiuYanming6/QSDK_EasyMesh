OFFLOAD_LOOP	    = ../../../hif/loop/linux

OFFLOAD_ADF_INC     = ../../adf
OFFLOAD_STACK_INC   = ../../../../include
OFFLOAD_HIF_INC     = ../../hif/$(HIF)

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/nbuf
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/net
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/os
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/linux/os
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/linux/net
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/linux/nbuf
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_HIF_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_HIF_INC)/linux
EXTRA_CFLAGS    += -Werror

obj-m			+= hif_loop.o
hif_loop-objs	:= $(OFFLOAD_LOOP)/hif_loop.o

