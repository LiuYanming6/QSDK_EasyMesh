OFFLOAD_USB	    = ../../../../hif/usb
OFFLOAD_USB_INC	= ../../hif/usb/

OFFLOAD_ADF_INC     = ../../adf
OFFLOAD_STACK_INC   = ../../../../include

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)
EXTRA_CFLAGS	+= -I$(PWD)/$(OFFLOAD_HIF_USB_INC)
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_ADF_INC)/linux/os
EXTRA_CFLAGS    += -Werror

ifeq ($(strip ${ENABLE_USB_STREAM}), y)
EXTRA_CFLAGS    += -DHIF_STREAM_MODE=1
endif

ifeq ($(strip ${ATH_SUPPORT_AOW}), 1)
EXTRA_CFLAGS    += -DATH_SUPPORT_AOW=1
else
EXTRA_CFLAGS    += -DATH_SUPPORT_AOW=0
endif
ifeq ($(strip ${ATH_SUPPORT_AOW_VSOUND}), 1)
EXTRA_CFLAGS    += -DATH_SUPPORT_AOW_VSOUND=1
else
EXTRA_CFLAGS    += -DATH_SUPPORT_AOW_VSOUND=0
endif

ifeq ($(MOD_SUFFIX),_offload)
EXTRA_CFLAGS += -D'MODULE_SYMBOL_PREFIX="offload_"' 
endif

obj-m           += hif_usb$(MOD_SUFFIX).o
hif_usb$(MOD_SUFFIX)-objs	:= $(OFFLOAD_USB)/linux/hif_usb.o
