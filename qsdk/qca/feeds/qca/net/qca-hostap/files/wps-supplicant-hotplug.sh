#
# Copyright (c) 2017 Qualcomm Technologies, Inc.
#
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

#
# Copyright (c) 2014, The Linux Foundation. All rights reserved.
#

activate_pbc_for_sta_iface() {
	local config=$1
	local ifname
	local mode
	config_get ifname "$config" ifname
	config_get mode "$config" mode

	if [ $mode == "sta" ]; then
		local wps_pbc
		local wps_pbc_skip_connected_sta
		config_get wps_pbc "$config" wps_pbc 0
		config_get wps_pbc_skip_connected_sta "$config" wps_pbc_skip_connected_sta 0
		if [ $wps_pbc -gt 0 ]; then
			if [ $wps_pbc_skip_connected_sta -eq 0 ] ||
			    iwconfig $ifname | head -2 | tail -1 | grep -q "Not-Associated"; then
				wpa_cli -p "$dir" wps_pbc
			fi
		fi
	fi
}

pid=
if [ "$ACTION" = "pressed" -a "$BUTTON" = "wps" ]; then
    [ -r /var/run/son_active ] && exit 0
	if [ -r /var/run/wifi-wps-enhc-extn.conf ] &&
		[ ! -r /var/run/son.conf ]; then
		exit 0
	fi
	config_load wireless
	for dir in /var/run/wpa_supplicant-*; do
		[ -d "$dir" ] || continue
		pid=/var/run/wps-hotplug-${dir#"/var/run/wpa_supplicant-"}.pid
		config_foreach activate_pbc_for_sta_iface wifi-iface
		[ -f $pid ] || {
			wpa_cli -p"$dir" -a/lib/wifi/wps-supplicant-update-uci -P$pid -B
		}
	done
fi
