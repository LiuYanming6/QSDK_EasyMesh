OFFLOAD_HIF_USB = ../../../hif/usb

OFFLOAD_TGT_INC     = ../../../
OFFLOAD_DRV_INC     = $(OFFLOAD_TGT_INC)/../
OFFLOAD_STACK_INC   = $(OFFLOAD_DRV_INC)../

OFFLOAD_HIF_INC     = ../../hif/

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)/include
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_DRV_INC)/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_TGT_INC)/linux/include/

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_HIF_INC)/usb/
EXTRA_CFLAGS    += -Werror

ifeq ($(strip ${ENABLE_USB_STREAM}), y)
EXTRA_CFLAGS    += -DHIF_STREAM_MODE=1
ifeq ($(strip ${HIF_STREAM_SCHEDULE_METHOD}), 1)
EXTRA_CFLAGS    += -DHIF_STREAM_SCHEDULE_METHOD=1
else
EXTRA_CFLAGS    += -DHIF_STREAM_SCHEDULE_METHOD=2
endif
endif

ifeq ($(strip ${BUILD_8M}), 1)
EXTRA_CFLAGS    += -DBUILD_8M=1
else
EXTRA_CFLAGS    += -DBUILD_8M=0
endif

ifeq ($(strip ${ATH_SUPPORT_AOW}), 1)
EXTRA_CFLAGS    += -DATH_SUPPORT_AOW=1
else
EXTRA_CFLAGS    += -DATH_SUPPORT_AOW=0
endif
ifeq ($(strip ${TARGET_STANDALONE}),1)
EXTRA_CFLAGS    	+= -DTARGET_STANDALONE=1
else
EXTRA_CFLAGS    	+= -DTARGET_STANDALONE=0
endif

obj-m          += hif_usb.o
hif_usb-objs    := $(OFFLOAD_HIF_USB)/hif_usb.o
hif_usb-objs    += $(OFFLOAD_HIF_USB)/usb_dev.o


