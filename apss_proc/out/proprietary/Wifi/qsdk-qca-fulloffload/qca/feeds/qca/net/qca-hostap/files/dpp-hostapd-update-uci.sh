#!/bin/sh
#
# Copyright (c) 2018 Qualcomm Technologies, Inc.
#
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

IFNAME=$1
CMD=$2
CONFIG=$3

parent=$(cat /sys/class/net/${IFNAME}/parent)

case "$CMD" in
	DPP-CONF-RECEIVED)
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set wpa 2
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set wpa_key_mgmt DPP
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set ieee80211w 1
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set rsn_pairwise CCMP
		;;
	DPP-CONFOBJ-AKM)
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set wpa_key_mgmt $CONFIG
		;;
	DPP-CONFOBJ-SSID)
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set ssid $CONFIG
		;;
	DPP-CONNECTOR)
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set dpp_connector $CONFIG
		;;
	DPP-CONFOBJ-PASS)
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set wpa_key_mgmt WPA-PSK
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set wpa_passphrase $CONFIG
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent disable
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent enable
		;;
	DPP-CONFOBJ-PSK)
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set wpa_key_mgmt WPA-PSK
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set wpa_psk $CONFIG
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent disable
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent enable
		;;
	DPP-C-SIGN-KEY)
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set dpp_csign $CONFIG
		;;
	DPP-NET-ACCESS-KEY)
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent set dpp_netaccesskey $CONFIG
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent disable
		hostapd_cli -i$IFNAME -p/var/run/hostapd-$parent enable
		;;
esac
