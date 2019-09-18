OFFLOAD_THIN	    =../../../thin_driver
OFFLOAD_THIN_INC	=../../thin_driver
OFFLOAD_FW_PCIE      =../../../../fw_loader/pcie

OFFLOAD_ADF_INC     = ../../../../adf
OFFLOAD_ASF_INC     = ../../../../asf
OFFLOAD_STACK_INC   = ../../../../include
OFFLOAD_DRV_INC     = ../../../include
OFFLOAD_HST_INC     = ../../include
OFFLOAD_HIF_INC     = ../../hif/pcie

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

#EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_HIF_INC)/linux
EXTRA_CFLAGS        += -I$(PWD)/$(OFFLOAD_HIF_INC)

obj-m				+= fwd.o
fwd-objs	        := $(OFFLOAD_FW_PCIE)/fwd.o

#ifeq ($(BOOT),rom)
#EXTRA_CFLAGS        += -DROM_MODE    
#fwd-objs            += $(OFFLOAD_FW_GMAC)/vmlrd_lzma.o
#fwd-objs            += $(OFFLOAD_FW_GMAC)/fwu_rombootdrv.o
#endif
