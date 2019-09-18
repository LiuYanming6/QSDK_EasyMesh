OFFLOAD_THIN	    =../../../thin_driver
OFFLOAD_THIN_INC	=../../thin_driver
OFFLOAD_FW_GMAC      =../../../../fw_loader/gmac

OFFLOAD_ADF_INC     = ../../../../adf
OFFLOAD_ASF_INC     = ../../../../../../asf
OFFLOAD_STACK_INC   = ../../../../include
OFFLOAD_DRV_INC     = ../../../include
OFFLOAD_HST_INC     = ../../include
OFFLOAD_HIF_INC     = ../../hif/gmac

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

obj-m				+= fwd.o
fwd-objs	        := $(OFFLOAD_FW_GMAC)/fwd.o

ifeq  ($(strip $(OFFLOAD_TGT_RECOVERY)),1)
EXTRA_CFLAGS        += -DOFFLOAD_TGT_RECOVERY    
endif

ifeq ($(BOOT),rom)
EXTRA_CFLAGS        += -DROM_MODE    

ifeq  ($(strip $(AH_OFFLOAD_CALDATA_FROM_HOST)),1)
EXTRA_CFLAGS += -DAH_OFFLOAD_CALDATA_LOCATION_ON_HOST=$(AH_OFFLOAD_CALDATA_LOCATION_ON_HOST)
EXTRA_CFLAGS += -DAH_OFFLOAD_CALDATA_FROM_HOST
ifeq ($(strip $(AH_OFFLOAD_RADIOS_AHB)),1)
EXTRA_CFLAGS += -DAH_OFFLOAD_RADIOS_AHB=$(AH_OFFLOAD_RADIOS_AHB) -DAH_OFFLOAD_RADIOS_AHB_CALDATA_ADDR=$(AH_OFFLOAD_RADIOS_AHB_CALDATA_ADDR)
endif
ifeq ($(strip $(AH_OFFLOAD_RADIOS_PCI)),1)
EXTRA_CFLAGS += -DAH_OFFLOAD_RADIOS_PCI=$(AH_OFFLOAD_RADIOS_PCI) -DAH_OFFLOAD_RADIOS_PCI_CALDATA_ADDR=$(AH_OFFLOAD_RADIOS_PCI_CALDATA_ADDR)
endif
endif

ifeq ($(strip $(AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK)),1)
EXTRA_CFLAGS        += -DAH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK
fwd-objs            += $(OFFLOAD_FW_GMAC)/vmlrd_lzma.o
fwd-objs            += $(OFFLOAD_FW_GMAC)/fwu_rombootdrv.o
endif

endif
