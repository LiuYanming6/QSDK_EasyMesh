#!/bin/sh

DBG_LOG="/tmp/dbg.log"

rm $DBG_LOG

echo "|----------- mesh topology -----------|" > $DBG_LOG
(echo td s2; sleep 1) | hyt >> $DBG_LOG
echo "|----------- kernel -----------|" >> $DBG_LOG
dmesg >> $DBG_LOG
echo "|----------- run-time system  -----------|" >> $DBG_LOG
logread -F $DBG_LOG
