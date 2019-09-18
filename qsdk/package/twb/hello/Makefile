#
# Copyright 2018-2019, twinA Corporation
# All Rights Reserved.
# 
# $Id$
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hello
PKG_RELEASE:=1.0

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=twb
  CATEGORY:=TWB base
  TITLE:=TWB init
  DEPENDS:=
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		TARGET_CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hello $(1)/usr/sbin/hello
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/hello.init $(1)/etc/init.d/hello
endef

$(eval $(call BuildPackage,hello))
