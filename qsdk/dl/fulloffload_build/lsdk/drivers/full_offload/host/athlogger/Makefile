#
# Makefile for athlogger
#
ATHLOGGER_DIR = $(shell pwd)

# TODO: To achieve Common athlogger.ko for both Host and Target, 
# HIF Includes should be common. Need to eliminate target specific includes
OFFLOAD_INCS = $(ATHLOGGER_DIR)/../../../drivers/full_offload/target/linux/include
OFFLOAD_HIF_INCS = $(ATHLOGGER_DIR)/../../../drivers/full_offload/target/linux/hif/gmac

EXTRA_CFLAGS += -I. -I$(KERNELPATH)/arch/mips/include
EXTRA_CFLAGS += -I$(OFFLOAD_INCS) -I$(OFFLOAD_HIF_INCS)

obj-m  := athlogger.o

athlogger-objs := athlogger_main.o athlogger_share.o athlogger_iface.o

