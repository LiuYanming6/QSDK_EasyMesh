#
# Copyright 2018-2019, twinA Corporation
# All Rights Reserved.
# 
# $Id$
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cgibox
PKG_RELEASE:=1.0


include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=twb
  CATEGORY:=TWB base
  TITLE:=CGIBOX - Apps use CGI
  DEPENDS:= +libjson-c +libuci +libopenssl +fcgi +twbox
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(CP) ./src/cgibox.h $(STAGING_DIR)/usr/include
endef


define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		TARGET_CFLAGS="$(TARGET_CFLAGS)" \
		STAGING_DIR="$(STAGING_DIR)"
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/enap.fcgi
endef

$(eval $(call BuildPackage,cgibox,+libjson-c, +libuci, +openssl, +fcgi, +twbox))
