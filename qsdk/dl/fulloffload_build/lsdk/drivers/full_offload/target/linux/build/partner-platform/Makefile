ifeq (${OFFLOAD_CFG},)
    OFFLOAD_CFG=default
endif

CC			= $(TOOLPREFIX)gcc

TGT_PATH= ../../../

include $(PWD)/Kbuild.$(OFFLOAD_CFG)

define cleanup_files
	@find $(1) -type f -name "*.o" -exec rm -vf {} ';'; 
	@find $(1) -type f -name ".*.cmd" -exec rm -vf {} ';';
	@find $(1) -type f -name "modules.order" -exec rm -vf {} ';';
endef

ifeq ($(KERNELRELEASE),)
KERNELVER   ?= $(shell uname -r)
KERNELPATH	?= /lib/modules/$(KERNELVER)/build/
PWD	     	?= $(shell pwd)
MODULEPATH  ?= $(DESTDIR)/lib/modules/$(KERNELVER)/offload/

all: 
	echo "Building full offload target" 
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(shell pwd) modules

clean:
	$(MAKE) -C $(KERNELPATH) SUBDIRS=$(shell pwd) clean
	$(call cleanup_files,$(TGT_PATH))
    
install:
	mkdir -p $(MODULEPATH);
	find . -type f -name "*.ko" -exec cp -v {} $(MODULEPATH) ';'

endif    
