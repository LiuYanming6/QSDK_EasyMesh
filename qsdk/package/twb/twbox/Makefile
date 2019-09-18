#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=twbox
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

TARGET_CFLAGS += $(FPIC) -DUSE_CRYPTO

define Package/twbox
  SECTION:=twb
  CATEGORY:=TWB base
  DEPENDS:= +libopenssl
  TITLE:=TWB ubox
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


define Build/InstallDev
	mkdir -p $(1)/usr/include/twbox
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/twbox/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libtwbox.so $(1)/usr/lib/
endef

define Package/twbox/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/*.so $(1)/usr/lib
endef



$(eval $(call BuildPackage,twbox,+openssl))
