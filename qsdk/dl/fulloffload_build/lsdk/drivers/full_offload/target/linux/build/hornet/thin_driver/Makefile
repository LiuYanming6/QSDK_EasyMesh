OFFLOAD_TGT_INC     = ../../../
OFFLOAD_DRV_INC     = $(OFFLOAD_TGT_INC)/../
OFFLOAD_STACK_INC   = $(OFFLOAD_DRV_INC)../

OFFLOAD_THIN_INC    = ../../thin_driver
OFFLOAD_HIF_INC     = ../../hif/

OFFLOAD_THIN_DRV = ../../../thin_driver

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_STACK_INC)/include 
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_DRV_INC)/include/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_TGT_INC)/linux/include/

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_THIN_INC)/wmi/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_THIN_INC)/htc/
EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_THIN_INC)/atd/

EXTRA_CFLAGS    += -I$(PWD)/$(OFFLOAD_HIF_INC)/$(BUS)
EXTRA_CFLAGS    += -Werror
EXTRA_CFLAGS    += -DATD_DEBUG_ENABLE

obj-m       += atd.o
atd-objs    := $(OFFLOAD_THIN_DRV)/htc/htc.o
atd-objs    += $(OFFLOAD_THIN_DRV)/atd/atd_cfg.o
atd-objs    += $(OFFLOAD_THIN_DRV)/atd/atd_dev.o
atd-objs    += $(OFFLOAD_THIN_DRV)/atd/atd_wsupp.o
atd-objs    += $(OFFLOAD_THIN_DRV)/wmi/wmi.o
    
