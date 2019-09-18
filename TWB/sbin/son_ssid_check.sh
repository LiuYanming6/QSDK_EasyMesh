#!/bin/sh
# TwinA 
# Wi-Fi SON Auto Extender Credential Monitor
# This program monitors SSID of REs for a period of time
# after CAP's credentials changed by external applications


echo "Wi-Fi SON Auto Extender Credential Monitor" > /dev/console
# wait for 3 mintues to get stable topology
sleep 180
 
count=0
while true; do
 
    # Get current fronthaul SSID of CAP
    ssid=
    ssid=`iwconfig ath0 | awk 'BEGIN {FS="\""} /ESSID/ {print $2}'`
    echo "CAP SSID: $ssid" > /dev/console

  if [ $(ps | grep hyd | grep -v "grep" | wc -l) -ge 1 ]; then
    # hyd exists	    
    # Dump topology to file
    rm /tmp/son_topology
    (echo td s2; sleep 1) | hyt > /tmp/son_topology

    # Get the number of RE
    numofRE=
    numofRE=`awk '/-- DB/ {print substr($3,2,1)}' /tmp/son_topology `
    echo "Number of RE: $numofRE" > /dev/console
    if [ "$numofRE" -eq 0 ]; then
        echo "No RE has found... waiting" > /dev/console
        sleep 60
        continue
    fi

    # The number of SSID need to be matched 
    match=
    match=$(($numofRE * 2))
 
    # parser
    eval $( awk -v ssid=$ssid -v num=$numofRE '
    BEGIN {total=0}
    / SSID/ {
    ressid=$2
    if (ssid == ressid) total++ }
    END {print "total="total } ' /tmp/son_topology )

    # All RE's fronthaul SSID are consistent 
   if [ "$total" == "$match" ]; then
       echo "TwinA: All REs' fronthaul SSID consistent" > /dev/console
       break
   elif [ "$total" != "$match" ] && [ "$count" -ge 10 ]; then
       echo "TwinA: some REs' fronthaul SSID inconsistent " > /dev/console
       /etc/init.d/wsplcd restart_after_config_change
       break
   fi

  else
  # no hyd
    echo "No HYD... waiting" > /dev/console
    sleep 60
    continue
  fi

    count=$(( $count + 1 ))
    sleep 30
done

