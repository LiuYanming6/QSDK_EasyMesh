#!/bin/sh

DBG_LOG="/tmp/dbg.log"

rm $DBG_LOG

echo "|----------- mesh topology -----------|" > $DBG_LOG
(echo td s2; sleep 1) | hyt >> $DBG_LOG
echo "|----------- kernel -----------|" >> $DBG_LOG
dmesg >> $DBG_LOG
echo "|----------- run-time system  -----------|" >> $DBG_LOG
logread -F $DBG_LOG
echo "|----------- uci parameters  -----------|" >> $DBG_LOG
uci show >> $DBG_LOG
echo "|----------- wireless interfaces information  -----------|" >> $DBG_LOG
iwconfig >> $DBG_LOG
echo "|----------- interfaces information  -----------|" >> $DBG_LOG
ifconfig >> $DBG_LOG
echo "|----------- 2.4G Fronthaul AP client information  -----------|" >> $DBG_LOG
wlanconfig ath0 list >> $DBG_LOG
echo "|----------- 2.4G Backhaul AP client information  -----------|" >> $DBG_LOG
wlanconfig ath01 list >> $DBG_LOG
echo "|----------- 5G Backhaulhaul STA information  -----------|" >> $DBG_LOG
wlanconfig ath1 list >> $DBG_LOG
echo "|----------- 5G Fronthaul AP client information  -----------|" >> $DBG_LOG
wlanconfig ath11 list >> $DBG_LOG
echo "|----------- 5G Backhaul AP client information  -----------|" >> $DBG_LOG
wlanconfig ath12 list >> $DBG_LOG
echo "|----------- run-time process  -----------|" >> $DBG_LOG
ps -w >> $DBG_LOG
echo "|----------- Route information  -----------|" >> $DBG_LOG
route >> $DBG_LOG
