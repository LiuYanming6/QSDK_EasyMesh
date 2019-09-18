#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cwmpc
#PKG_RELEASE:= $(shell pwgen 16 1 -c -s)
#PKG_VERSION:= $(shell pwgen 16 1 -n -s)
PKG_VERSION:= 1
#CWMPC_STORE_PREPASSWORD:= $(shell echo -n $(PKG_RELEASE) > prepassword )
#CWMPC_STORE_SECRET:= $(shell echo -n $(PKG_VERSION) > secret )

include $(INCLUDE_DIR)/version.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/cwmpc
  SECTION:=twb
  CATEGORY:=TWB base
  DEPENDS:=+libopenssl +libcrypto +libpthread
  TITLE:=TR069-client
endef

#define Package/cwmpc/config
#  source "$(SOURCE)/Config.in"
#endef

TARGET_CPPFLAGS := \
	-fstack-protector \
	-Werror \
	-Wno-strict-prototypes \
	-Wno-unused-function \
	-D_GNU_SOURCE \
	-DDNS_PATCH \
	-DUSE_CWMP_MAIN \
	-L$(STAGING_DIR)/usr/lib/ \
	-L$(STAGING_DIR_HOST)/lib \
	-ldl -lssl -lcrypto -pthread \
	$(TARGET_CPPFLAGS) \
	-DVERSION_NUM='$(VERSION_NUMBER)'

ifeq ($(CONFIG_CWMPC_AUTO_SECRET),y)
TARGET_CPPFLAGS += \
-DAUTO_PREPASSWORD=$(PKG_RELEASE) \
-DAUTO_SECRET=$(PKG_VERSION) \
-DCWMPC_STORE_PREPASSWORD \
-DCWMPC_STORE_SECRET
endif

TARGET_LDFLAGS := \
	-L$(STAGING_DIR)/usr/lib/ \
	-L$(STAGING_DIR_HOST)/lib \
	-ldl -lssl -lcrypto -pthread 

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	EXTRA_CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		EXTRA_LDFLAGS="$(TARGET_LDFLAGS)" \
		testbox
endef

define Package/cwmpc/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cwmpc $(1)/sbin
	$(INSTALL_DIR) $(1)/etc
	install -m0664 $(PKG_BUILD_DIR)/cpestate-default.xml  $(1)/etc
	install -m0664 $(PKG_BUILD_DIR)/acs_middle_certs_pub_pinning.pem $(1)/etc
endef


$(eval $(call BuildPackage,cwmpc,+libopenssl))
