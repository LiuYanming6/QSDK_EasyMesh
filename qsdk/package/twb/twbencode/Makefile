#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=twbencode
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

TARGET_CFLAGS += $(FPIC) -DUSE_CRYPTO

define Package/twbencode
  SECTION:=twb
  CATEGORY:=TWB base
  DEPENDS:= +libopenssl
  TITLE:=TWB encode
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	EXTRA_CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -Werror" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		EXTRA_LDFLAGS="$(TARGET_LDFLAGS) -lcrypto -lssl \
		-L$(STAGING_DIR)/usr/lib/ \
		-L$(STAGING_DIR_HOST)/lib" \
		build
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/twbencode $(1)/sbin
endef



$(eval $(call BuildPackage,twbencode,+openssl))
