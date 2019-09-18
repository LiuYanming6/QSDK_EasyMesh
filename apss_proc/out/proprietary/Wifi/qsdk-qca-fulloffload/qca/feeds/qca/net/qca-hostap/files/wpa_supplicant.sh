#
# Copyright (c) 2017 Qualcomm Technologies, Inc.
#
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

#
# Copyright (c) 2014, The Linux Foundation. All rights reserved.
#

wpa_supplicant_setup_vif() {
	local vif="$1"
	local driver="$2"
	local key="$key"
	local options="$3"
	local freq="" crypto=""
	local sae owe sae_groups owe_group sae_password
	local sae_str owe_str

	[ -n "$4" ] && freq="frequency=$4"
	config_get_bool sae "$vif" sae 0
	config_get_bool owe "$vif" owe 0
	config_get sae_password "$vif" sae_password
	config_get ieee80211w "$vif" ieee80211w

	# make sure we have the encryption type and the psk
	[ -n "$enc" ] || {
		config_get enc "$vif" encryption
	}

	enc_list=`echo "$enc" | sed "s/+/ /g"`

	for enc_var in $enc_list; do
		case "$enc_var" in
			*tkip)
				crypto="TKIP $crypto"
				;;
			*aes)
				crypto="CCMP $crypto"
				;;
			*ccmp)
				crypto="CCMP $crypto"
				;;
			*ccmp-256)
				crypto="CCMP-256 $crypto"
				;;
			*gcmp)
				crypto="GCMP $crypto"
				;;
			*gcmp-256)
				crypto="GCMP-256 $crypto"
		esac
	done

	[ -n "$key" ] || {
		config_get key "$vif" key
	}

	local net_cfg bridge
	config_get bridge "$vif" bridge
	[ -z "$bridge" ] && {
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || bridge="$(bridge_interface "$net_cfg")"
		config_set "$vif" bridge "$bridge"
	}

	local mode ifname wds modestr=""
	config_get mode "$vif" mode
	config_get ifname "$vif" ifname
	config_get_bool wds "$vif" wds 0
	config_get_bool extap "$vif" extap 0
	config_get map "$vif" map

	config_get device "$vif" device
	config_get_bool qwrap_enable "$device" qwrap_enable 0

	[ -z "$bridge" ] || [ "$mode" = ap ] || [ "$mode" = sta -a $wds -eq 1 ] || \
	[ "$mode" = sta -a $extap -eq 1 ] || [ $qwrap_enable -ne 0 ] || {
		echo "wpa_supplicant_setup_vif($ifname): Refusing to bridge $mode mode interface"
		return 1
	}
	[ "$mode" = "adhoc" ] && modestr="mode=1"

	key_mgmt='NONE'
	case "$enc" in
		*none*) ;;
		*wep*)
			config_get key "$vif" key
			key="${key:-1}"
			case "$key" in
				[1234])
					for idx in 1 2 3 4; do
						local zidx
						zidx=$(($idx - 1))
						config_get ckey "$vif" "key${idx}"
						[ -n "$ckey" ] && \
							append "wep_key${zidx}" "wep_key${zidx}=$(prepare_key_wep "$ckey")"
					done
					wep_tx_keyidx="wep_tx_keyidx=$((key - 1))"
				;;
				*)
					wep_key0="wep_key0=$(prepare_key_wep "$key")"
					wep_tx_keyidx="wep_tx_keyidx=0"
				;;
			esac
			case "$enc" in
				*mixed*)
					wep_auth_alg='auth_alg=OPEN SHARED'
				;;
				*shared*)
					wep_auth_alg='auth_alg=SHARED'
				;;
				*open*)
					wep_auth_alg='auth_alg=OPEN'
				;;
			esac
		;;
		*psk*)
			key_mgmt='WPA-PSK'
			# if you want to use PSK with a non-nl80211 driver you
			# have to use WPA-NONE and wext driver for wpa_s
			[ "$mode" = "adhoc" -a "$driver" != "nl80211" ] && {
				key_mgmt='WPA-NONE'
				driver='wext'
			}
			if [ ${sae} -eq 1 ] && [ ${sae_password} ]
			then
				if [ ${#sae_password} -eq 64 ]
				then
					passphrase="psk=${sae_password}"
				else
					passphrase="psk=\"${sae_password}\""
				fi
			elif [ ${#key} -eq 64 ]; then
				passphrase="psk=${key}"
			else
				passphrase="psk=\"${key}\""
			fi

			[ -n "$crypto" ] || crypto="CCMP"
			pairwise="pairwise=$crypto"

			case "$enc" in
				*mixed*)
					proto='proto=RSN WPA'
				;;
				*psk2*)
					proto='proto=RSN'
					config_get ieee80211w "$vif" ieee80211w 0
				;;
				*psk*)
					proto='proto=WPA'
				;;
			esac
		;;
		*wpa*|*8021x*)
			proto='proto=WPA2'
			key_mgmt='WPA-EAP'
			config_get ieee80211w "$vif" ieee80211w 0
			config_get ca_cert "$vif" ca_cert
			config_get eap_type "$vif" eap_type
			ca_cert=${ca_cert:+"ca_cert=\"$ca_cert\""}

			[ -n "$crypto" ] || crypto="CCMP"
			pairwise="pairwise=$crypto"

			case "$eap_type" in
				tls)
					config_get identity "$vif" identity
					config_get client_cert "$vif" client_cert
					config_get priv_key "$vif" priv_key
					config_get priv_key_pwd "$vif" priv_key_pwd
					identity="identity=\"$identity\""
					client_cert="client_cert=\"$client_cert\""
					priv_key="private_key=\"$priv_key\""
					priv_key_pwd="private_key_passwd=\"$priv_key_pwd\""
				;;
				peap|ttls)
					config_get auth "$vif" auth
					config_get identity "$vif" identity
					config_get password "$vif" password
					phase2="phase2=\"auth=${auth:-MSCHAPV2}\""
					identity="identity=\"$identity\""
					password="password=\"$password\""
				;;
			esac
			eap_type="eap=$(echo $eap_type | tr 'a-z' 'A-Z')"
		;;
		ccmp*|gcmp*)
			proto='proto=RSN'
			if [ ${sae} -eq 1 ] && [ ${sae_password} ]
			then
				if [ ${#sae_password} -eq 64 ]
				then
					passphrase="psk=${sae_password}"
				else
					passphrase="psk=\"${sae_password}\""
				fi
			elif [ ${#key} -eq 64 ]; then
				passphrase="psk=${key}"
			else
				passphrase="psk=\"${key}\""
			fi
			case $enc in
				ccmp-256)
					[ -n "$crypto" ] || crypto="CCMP-256"
					pairwise="pairwise=$crypto"
				;;
				gcmp-256)
					[ -n "$crypto" ] || crypto="GCMP-256"
					pairwise="pairwise=$crypto"
				;;
				ccmp*)
					[ -n "$crypto" ] || crypto="CCMP"
					pairwise="pairwise=$crypto"
				;;
				gcmp*)
					[ -n "$crypto" ] || crypto="GCMP"
					pairwise="pairwise=$crypto"
				;;
			esac
		;;
	esac

	keymgmt='NONE'

	# Allow SHA256
	case "$enc" in
		*wpa*|*8021x*) keymgmt=WPA-EAP;;
		*psk*) keymgmt=WPA-PSK;;
		ccmp*|gcmp*) keymgmt=;;
	esac

	if [ "${sae}" -eq 1 -a "${ieee80211r}" -gt 0 ]
	then
		ieee80211w=2
		sae_str=FT-SAE
	elif [ "${sae}" -eq 1 ]
	then
		ieee80211w=2
		sae_str=SAE
	fi

	if [ "${owe}" -eq 1 ]
	then
		proto='proto=RSN'
		ieee80211w=2
		owe_only='owe_only=1'
		owe_str=OWE
		passphrase=
	fi

	case "$ieee80211w" in
		0)
			key_mgmt="${keymgmt}"
		;;
		1)
			key_mgmt="${keymgmt} ${keymgmt}-SHA256"
		;;
		2)
			if [ "$owe" -eq 1 ] || [ "$sae" -eq 1 ]
			then
				key_mgmt="${keymgmt}"
			else
				key_mgmt="${keymgmt}-SHA256"
			fi
		;;
	esac

	if [ "${sae}" -eq 1 ]
	then
		add_sae_groups() {
			sae_groups=$(echo $1 | tr "," " ")
		}
		config_list_foreach  "$vif" sae_groups add_sae_groups
		if [ "$sae_groups" ];then
			sae_groups="sae_groups=$sae_groups"
		else
			sae_groups=
		fi
		key_mgmt="${sae_str}"
	fi
	if [ "${owe}" -eq 1 ]
	then
		config_get  owe_group "$vif" owe_group
		if [ "$owe_group" ];then
			owe_group="owe_group=$owe_group"
		else
			owe_group=
		fi
		key_mgmt="${key_mgmt} ${owe_str}"
	fi

	[ -n "$ieee80211w" ] && ieee80211w="ieee80211w=$ieee80211w"

	if [ $sae -ne 1 ] && [ $owe -ne 1 ]
	then
		case "$pairwise" in
			*CCMP-256*) group="group=CCMP-256 GCMP-256 GCMP CCMP TKIP";;
			*GCMP-256*) group="group=GCMP-256 GCMP CCMP TKIP";;
			*GCMP*) group="group=GCMP CCMP TKIP";;
			*CCMP*) group="group=CCMP TKIP";;
			*TKIP*) group="group=TKIP";;
		esac
	else
		case "$pairwise" in
			*CCMP-256*) group="group=CCMP-256 GCMP-256 GCMP CCMP";;
			*GCMP-256*) group="group=GCMP-256 GCMP CCMP";;
			*GCMP*) group="group=GCMP CCMP";;
			*CCMP*) group="group=CCMP";;
		esac
	fi

	config_get ifname "$vif" ifname
	config_get bridge "$vif" bridge
	config_get ssid "$vif" ssid
	config_get bssid "$vif" bssid
	bssid=${bssid:+"bssid=$bssid"}

	config_get_bool wps_pbc "$vif" wps_pbc 0

	config_get config_methods "$vif" wps_config
	[ "$wps_pbc" -gt 0 ] && append config_methods push_button

	[ -n "$config_methods" ] && {
		wps_cred="wps_cred_processing=2"
		wps_config_methods="config_methods=$config_methods"
		update_config="update_config=1"
		# fix the overlap session of WPS PBC for two STA vifs
		macaddr=$(cat /sys/class/net/${bridge}/address)
		uuid=$(echo "$macaddr" | sed 's/://g')
		[ -n "$uuid" ] && {
			uuid_config="uuid=87654321-9abc-def0-1234-$uuid"
		}
	}

	local ctrl_interface wait_for_wrap=""

	if [ $qwrap_enable -ne 0 ]; then
		ctrl_interface="/var/run/wpa_supplicant"
		if [ -f "/tmp/qwrap_conf_filename-$ifname.conf" ]; then
			rm -rf /tmp/qwrap_conf_filename-$ifname.conf
		fi
		echo -e "/var/run/wpa_supplicant-$ifname.conf \c\h" > /tmp/qwrap_conf_filename-$ifname.conf
		wait_for_wrap="-W"
	fi

	ctrl_interface="/var/run/wpa_supplicant-$ifname"

	rm -rf $ctrl_interface
	rm -f /var/run/wpa_supplicant-$ifname.conf
	cat > /var/run/wpa_supplicant-$ifname.conf <<EOF
ctrl_interface=$ctrl_interface
$wps_config_methods
$wps_cred
$update_config
$uuid_config
$sae_groups
network={
	$modestr
	scan_ssid=1
	ssid="$ssid"
	$bssid
	key_mgmt=$key_mgmt
	$proto
	$freq
	$ieee80211w
	$passphrase
	$pairwise
	$group
	$eap_type
	$ca_cert
	$client_cert
	$priv_key
	$priv_key_pwd
	$phase2
	$identity
	$password
	$wep_key0
	$wep_key1
	$wep_key2
	$wep_key3
	$wep_tx_keyidx
	$wep_auth_alg
	map=$map
	$owe_only
	$owe_group
}
EOF

if [ $sae -eq 1 ]
then
	config_get enc "$vif" encryption
	case "$enc" in
		*psk*)
			config_get ieee80211w "$vif" ieee80211w 0
			case "$ieee80211w" in
				0)
					key_mgmt=WPA-PSK
				;;
				1)
					key_mgmt="WPA-PSK WPA-PSK-SHA256"
				;;
				2)
					key_mgmt=WPA-PSK-SHA256
				;;
			esac
			ieee80211w="ieee80211w=$ieee80211w"
			group="$group TKIP"

			if [ ${#key} -eq 64 ]; then
				passphrase="psk=${key}"
			else
				passphrase="psk=\"${key}\""
			fi

			cat > /var/run/wpa_supplicant-psk-sae-$ifname.conf <<EOF
network={
	scan_ssid=1
	ssid="$ssid"
	key_mgmt=$key_mgmt
	$proto
	$freq
	$ieee80211w
	$passphrase
	$pairwise
	$group
}
EOF
		cat /var/run/wpa_supplicant-psk-sae-$ifname.conf >> /var/run/wpa_supplicant-$ifname.conf
		rm -rf /var/run/wpa_supplicant-psk-sae-$ifname.conf
	esac
fi

	[ -z "$proto" -a "$key_mgmt" != "NONE" ] || {\
                # If there is a change in path of wpa_supplicant-$ifname.lock file, please make the path
                # change also in wrapd_api.c file.
		[ -f "/var/run/wpa_supplicant-$ifname.lock" ] &&
			rm /var/run/wpa_supplicant-$ifname.lock
		wpa_cli -g /var/run/wpa_supplicantglobal interface_add  $ifname /var/run/wpa_supplicant-$ifname.conf athr /var/run/wpa_supplicant-$ifname "" $bridge
		touch /var/run/wpa_supplicant-$ifname.lock
    }
}
