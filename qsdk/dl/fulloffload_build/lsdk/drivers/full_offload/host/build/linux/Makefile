#
# Copyright (c) 2012 Qualcomm Atheros, Inc..
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
#

ASF_DIR  = ../../../../asf
HOST_PATH = ../../
ADF_PATH = ../../../../adf/

HIF ?= none

# By Default Target Firmware is Statically linked, define this to '0' if not required
AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK ?=0

ifeq ($(strip $(OFFLOAD_CFG)),)
OFFLOAD_CFG=default
endif

include $(PWD)/Kbuild.$(OFFLOAD_CFG)


define cleanup_files
	@find $(1) -type f -name "*.ko" -exec rm -vf {} ';';
	@find $(1) -type f -name "*.o" -exec rm -vf {} ';'; 
	@find $(1) -type f -name ".*.cmd" -exec rm -vf {} ';';
	@find $(1) -type f -name "modules.order" -exec rm -vf {} ';';
endef

	
EXTRA_CFLAGS	+= -I$(PWD)/$(ASF_DIR)/include/

ifeq (${DRIVERS_WLAN_INC},)
DRIVERS_WLAN_INC    = ../../../../../../drivers/wlan_modules
else
EXTRA_CFLAGS 		+= -I$(DRIVERS_WLAN_INC)
endif

ifeq ($(KERNELRELEASE),)
KERNELVER   ?= $(shell uname -r)
ifeq ($(BUILD_X86),y)
KERNELPATH  = /lib/modules/$(KERNELVER)/build/
else
KERNELPATH  ?= /lib/modules/$(KERNELVER)/build/
endif
PWD	     	?= $(shell pwd)
OFFLOAD_MOD ?= $(DESTDIR)/lib/modules/$(KERNELVER)/offload/

CC			= $(TOOLPREFIX)gcc 

all:
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(PWD) BOOT=rom HIF=$(BUS) AH_OFFLOAD_CALDATA_FROM_HOST=$(AH_OFFLOAD_CALDATA_FROM_HOST) AH_OFFLOAD_RADIOS_AHB=$(AH_OFFLOAD_RADIOS_AHB) AH_OFFLOAD_RADIOS_PCI=$(AH_OFFLOAD_RADIOS_PCI) AH_OFFLOAD_RADIOS_AHB_CALDATA_ADDR=$(AH_OFFLOAD_RADIOS_AHB_CALDATA_ADDR) AH_OFFLOAD_RADIOS_PCI_CALDATA_ADDR=$(AH_OFFLOAD_RADIOS_PCI_CALDATA_ADDR) AH_OFFLOAD_CALDATA_LOCATION_ON_HOST=$(AH_OFFLOAD_CALDATA_LOCATION_ON_HOST) AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK=$(AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK) modules 

clean:
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(PWD) BOOT=rom HIF=$(BUS) clean
	$(call cleanup_files,$(ADF_PATH))
	$(call cleanup_files,$(HOST_PATH))
flash:
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(PWD) BOOT=flash HIF=$(BUS) modules

flash_clean:
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(PWD) BOOT=flash HIF=$(BUS) clean 
	$(call cleanup_files,$(ADF_PATH))
	$(call cleanup_files,$(HOST_PATH))

# Indents the kernel source the way linux/Documentation/CodingStyle.txt
# wants it to be.
indent:
	indent -kr -i8 $($(MODULE_NAME)-objs:.o=.c)

install:
	@echo "dest is ${DESTDIR}"
	mkdir -p $(OFFLOAD_MOD);
	echo $(BUS)>$(OFFLOAD_MOD)/hif_type;
	echo $(SUFFIX)>$(OFFLOAD_MOD)/mod_suffix;
	cp -vf `cat modules.order|sed -e 's/kernel\///g'` $(OFFLOAD_MOD)
ifeq ($(AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK),0)
	@echo "copying tgt *.fw files to /lib/firmware" ';';
	@mkdir -p $(DESTDIR)/lib/firmware
	@find $(HOST_PATH) -type f -name "*.fw" -exec cp -vf {} $(DESTDIR)/lib/firmware/ ';';
endif

endif    
