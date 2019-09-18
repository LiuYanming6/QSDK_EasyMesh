#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jiomqtt
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

#TARGET_CFLAGS += $(FPIC) -DUSE_CRYPTO

define Package/jiomqtt
  SECTION:=twb
  CATEGORY:=TWB base
  DEPENDS:= +libpthread +librt +libstdcpp +libopenssl +libjiot-paho-mqtt-ws +libjiot-client-mqtt
  TITLE:=Reliance MQTT
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(CP) ./src/libjiot-paho-mqtt-ws.s* $(STAGING_DIR)/usr/lib/
	$(CP) ./src/libjiot-client-mqtt.s* $(STAGING_DIR)/usr/lib/
endef

#TODO -L$(STAGING_DIR_HOST)/lib the necessity?
#TODO should we use -Werror?
define Build/Compile
	EXTRA_CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -pthread -std=c99 -DHAVE_CONFIG_H -DUSE_GLIB -D__packed__= -DFEATURE_DATA_LOG_STDERR" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		EXTRA_LDFLAGS="$(TARGET_LDFLAGS) -lpthread -lm -lrt -lcrypto -lssl -Llibs -ljiot-paho-mqtt-ws -ljiot-client-mqtt \
		-L$(STAGING_DIR)/usr/lib/ \
		-L$(STAGING_DIR_HOST)/lib" \
		build
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/jiomqtt $(1)/sbin
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(STAGING_DIR)/usr/lib/libjiot-client-mqtt.s* $(1)/usr/lib/
	$(CP) $(STAGING_DIR)/usr/lib/libjiot-paho-mqtt-ws.s* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,jiomqtt,+openssl))
