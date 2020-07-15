# Makefile for OpenWrt
#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

TOPDIR:=${CURDIR}
LC_ALL:=C
LANG:=C
export TOPDIR LC_ALL LANG

empty:=
space:= $(empty) $(empty)
$(if $(findstring $(space),$(TOPDIR)),$(error ERROR: The path to the OpenWrt directory must not include any spaces))

DATE:= $(shell git log -1 --pretty=format:%ai | cut -c 1-10 | tr -d '-')
GIT_VERSION:=$(shell git log -1 --pretty=format:%H | cut -c 1-8)
VERSION_NUMBER = $(shell printf "$(CONFIG_VERSION_NUMBER)" | tr -d '"' | tr -d " ")

firmware:

include $(TOPDIR)/include/host.mk

ifneq ($(OPENWRT_BUILD),1)
  _SINGLE=export MAKEFLAGS=$(space);

  override OPENWRT_BUILD=1
  export OPENWRT_BUILD
  GREP_OPTIONS=
  export GREP_OPTIONS
  include $(TOPDIR)/include/debug.mk
  include $(TOPDIR)/include/depends.mk
  include $(TOPDIR)/include/toplevel.mk
else
  include rules.mk
  include $(INCLUDE_DIR)/depends.mk
  include $(INCLUDE_DIR)/subdir.mk
  include target/Makefile
  include package/Makefile
  include tools/Makefile
  include toolchain/Makefile

$(toolchain/stamp-install): $(tools/stamp-install)
$(target/stamp-compile): $(toolchain/stamp-install) $(tools/stamp-install) $(BUILD_DIR)/.prepared
$(package/stamp-compile): $(target/stamp-compile) $(package/stamp-cleanup)
$(package/stamp-install): $(package/stamp-compile)
$(target/stamp-install): $(package/stamp-compile) $(package/stamp-install)

printdb:
	@true

prepare: $(target/stamp-compile)

clean_kernel:
ifdef CONFIG_EXTERNAL_KERNEL_TREE
	make target/linux/clean
else
	@true
endif

clean: FORCE clean_kernel
	rm -rf $(BUILD_DIR) $(STAGING_DIR) $(BIN_DIR) $(BUILD_LOG_DIR)
# Remove firmware
	rm -rf $(BIN_DIR)/20*.bin

dirclean: clean
	rm -rf $(STAGING_DIR_HOST) $(TOOLCHAIN_DIR) $(BUILD_DIR_HOST) $(BUILD_DIR_TOOLCHAIN)
	rm -rf $(TMP_DIR)

ifndef DUMP_TARGET_DB
$(BUILD_DIR)/.prepared: Makefile
	@mkdir -p $$(dirname $@)
	@touch $@

tmp/.prereq_packages: .config
	unset ERROR; \
	for package in $(sort $(prereq-y) $(prereq-m)); do \
		$(_SINGLE)$(NO_TRACE_MAKE) -s -r -C package/$$package prereq || ERROR=1; \
	done; \
	if [ -n "$$ERROR" ]; then \
		echo "Package prerequisite check failed."; \
		false; \
	fi
	touch $@
endif

# check prerequisites before starting to build
prereq: $(target/stamp-prereq) tmp/.prereq_packages
	@if [ ! -f "$(INCLUDE_DIR)/site/$(ARCH)" ]; then \
		echo 'ERROR: Missing site config for architecture "$(ARCH)" !'; \
		echo '       The missing file will cause configure scripts to fail during compilation.'; \
		echo '       Please provide a "$(INCLUDE_DIR)/site/$(ARCH)" file and restart the build.'; \
		exit 1; \
	fi

prepare: .config $(tools/stamp-install) $(toolchain/stamp-install)
world: prepare $(target/stamp-compile) $(package/stamp-compile) $(package/stamp-install) $(target/stamp-install) FORCE
	$(_SINGLE)$(SUBMAKE) -r package/index

firmware: world
firmware:
		@$(eval MD5NUMBER = $(shell md5sum $(BIN_DIR)/openwrt-ar71xx-generic-ap151-16M-squashfs-sysupgrade.bin | cut -c 1-8 ))
		@cp $(BIN_DIR)/openwrt-ar71xx-generic-ap151-16M-squashfs-sysupgrade.bin $(BIN_DIR)/$(DATE)_$(strip $(VERSION_NUMBER))_QSDK5.3_$(MD5NUMBER).bin
		@cp $(BIN_DIR)/openwrt-ar71xx-generic-ap151-16M-squashfs-sysupgrade.bin $(BIN_DIR)/$(strip $(VERSION_NUMBER)).bin
		@echo "====================================================================";
		@echo "Build Date: $(DATE)";
		@echo "Git Commit: $(GIT_VERSION)";
		@echo "MD5sum: $(MD5NUMBER)";
		@echo "file path: $(BIN_DIR)";
		@echo "file Name: $(DATE)_$(strip $(VERSION_NUMBER))_QSDK5.3_$(MD5NUMBER).bin";
		@echo "file Name: $(strip $(VERSION_NUMBER)).bin";
		@echo "====================================================================";


.PHONY: clean dirclean prereq prepare world firmware package/symlinks package/symlinks-install package/symlinks-clean

endif
