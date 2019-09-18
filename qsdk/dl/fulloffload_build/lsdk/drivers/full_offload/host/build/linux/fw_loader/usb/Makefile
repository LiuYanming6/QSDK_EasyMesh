OFFLOAD_THIN	    =../../../thin_driver
OFFLOAD_THIN_INC	=../../thin_driver
OFFLOAD_FW_USB      =../../../../fw_loader/usb

OFFLOAD_ADF_INC     = ../../../../adf
OFFLOAD_ASF_INC     = ../../../../../../asf
OFFLOAD_STACK_INC   = ../../../../include
OFFLOAD_DRV_INC     = ../../../include
OFFLOAD_HST_INC     = ../../include
OFFLOAD_HIF_INC     = ../../hif/usb

EXTRA_CFLAGS	    += -I$(PWD)/$(OFFLOAD_STACK_INC)
EXTRA_CFLAGS	    += -I$(PWD)/$(OFFLOAD_DRV_INC)
EXTRA_CFLAGS	    += -I$(PWD)/$(OFFLOAD_HST_INC)
    

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/nbuf
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/net
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/include/os
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/nbuf/linux
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/net/linux
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/os/linux


EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_THIN_INC)/wmi/
EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_THIN_INC)/htc/
EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_THIN_INC)/atd/

EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_HIF_INC)/linux
EXTRA_CFLAGS        += -Werror

ifeq ($(MOD_SUFFIX),_offload)
EXTRA_CFLAGS += -D'MODULE_SYMBOL_PREFIX="offload_"' 
endif

obj-m	    	    += fwd$(MOD_SUFFIX).o
fwd$(MOD_SUFFIX)-objs            := $(OFFLOAD_FW_USB)/fwd.o
fwd$(MOD_SUFFIX)-objs            += $(OFFLOAD_FW_USB)/fwu_rombootdrv.o
fwd$(MOD_SUFFIX)-objs            += $(OFFLOAD_FW_USB)/vmlrd_lzma.o
