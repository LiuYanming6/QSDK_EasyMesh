# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=fwhandle
PKG_VERSION:=1.0


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
#PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
#PKG_MD5SUM:=434e51a018b4c8ef377bf81520a53af0

TARGET_CFLAGS += $(FPIC) -DUSE_CRYPTO

define Package/fwhandle
  SECTION:=twb
  CATEGORY:=TWB base
  TITLE:=TWB fwhandle
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	EXTRA_CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		EXTRA_LDFLAGS="$(TARGET_LDFLAGS)" \
		build
endef

define Package/fwhandle/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fwhandle $(1)/sbin
endef



$(eval $(call BuildPackage,fwhandle))
