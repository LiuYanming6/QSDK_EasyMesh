#!/bin/bash
#
#/*******************************************************************************
# *        Copyright (c) 2018 TWin Advanced Technology Corp., Ltd.
# *        All rights reserved.
# *
# *       THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
# *       PROPRIETARY AND TRADESECRET INFORMATION OF T&W INCORPORATED.
# *       ACCESS TO THIS WORK IS RESTRICTED TO (I) T&W EMPLOYEES WHO HAVE A
# *       NEED TO KNOW TO PERFORM TASKS WITHIN THE SCOPE OF THEIR ASSIGNMENTS
# *       AND (II) ENTITIES OTHER THAN T&W WHO HAVE ENTERED INTO APPROPRIATE
# *       LICENSE AGREEMENTS.  NO PART OF THIS WORK MAY BE USED, PRACTICED,
# *       PERFORMED, COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED,
# *       ABBRIDGED, CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
# *       TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF T&W.
# *       ANY USE OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD
# *       SUBJECT THE PERPERTRATOR TO CRIMINAL AND CIVIL LIABILITY.
# ******************************************************************************/

# /**
# * @file cgiboxtest.sh
# * @brief test cgibox function
# * @author TWin Advanced Technology Corp., Ltd.
# * @bug No known bugs
# *
# * usage:
# * ./cgitest.sh IPAddress
# * ./cgitest.sh IPAddress PASSWORD
# */
#/*-------------------------------------------------------------------------*/




uid=`curl -H "Content-Type:application/json" -X POST -d '{"Login": "admin,Jiocentrum,"}' http://$1:8080/SET_CGI | cut -c11-20`
#PASSWORD_SET=curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Admin_Name":"root","Admin_Password":"$2"}' http://$1:8080/SET_CGI 
#uid=`curl -H "Content-Type:application/json" -X POST -d '{"Login": "root,$2,"}' http://$1:8080/SET_CGI | cut -c11-20`
WIFISON=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFiSON":{}}' http://$1:8080/GET_CGI`
WIFISON_NUMBER=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFiSON_Number":""}' http://$1:8080/GET_CGI`
WIFISON_0_MAC=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFiSON_0_MAC":""}' http://$1:8080/GET_CGI`
WIFISON_1_MAC=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFiSON_1_MAC":""}' http://$1:8080/GET_CGI`
WIFISON_2_MAC=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFiSON_2_MAC":""}' http://$1:8080/GET_CGI`
WIFISON_0_ParentAPMAC=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFiSON_0_ParentAPMAC":""}' http://$1:8080/GET_CGI`
WIFISON_1_ParentAPMAC=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFiSON_1_ParentAPMAC":""}' http://$1:8080/GET_CGI`
WIFI_AP=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFi_AP":{}}' http://$1:8080/GET_CGI`
WIFI_AP_NUMBERS=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFi_AP_Numbers":""}' http://$1:8080/GET_CGI`
WIFI_AP_0_ENABLE=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFi_AP_0_Enable":""}' http://$1:8080/GET_CGI`
WIFI_AP_0_SSID=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFi_AP_0_SSID":""}' http://$1:8080/GET_CGI`
#WIFI_AP_0_SSID_SET=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFi_AP_0_SSID":"angustesttest"}' http://$1:8080/SET_CGI`
WIFI_AP_0_PASSWORD=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFi_AP_0_Password":""}' http://$1:8080/GET_CGI`
#WIFI_AP_0_PASSWORD_SET=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFi_AP_0_Password":"0123456789"}' http://$1:8080/SET_CGI`
WIFI_AP_0_SSID=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFi_AP_0_SSID":""}' http://$1:8080/GET_CGI`
WIFI_AP_0_PASSWORD=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"WiFi_AP_0_Password":""}' http://$1:8080/GET_CGI`
CLIENT=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client":{}}' http://$1:8080/GET_CGI`
CLIENT_NUMBER=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client_Numbers":""}' http://$1:8080/GET_CGI`
CLIENT_0_MAC=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client_0_MAC":""}' http://$1:8080/GET_CGI`
CLIENT_0_SIGNAL=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client_0_SignalStrength":""}' http://$1:8080/GET_CGI`
CLIENT_0_IPv4Address=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client_0_IPv4Address":""}' http://$1:8080/GET_CGI`
CLIENT_0_IPv6Address=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client_0_IPv6Address":""}' http://$1:8080/GET_CGI`
CLIENT_0_INTERFACE=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client_0_Interface":""}' http://$1:8080/GET_CGI`
CLIENT_0_CONNECTIONTYPE=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client_0_ConnectionType":""}' http://$1:8080/GET_CGI`
CLIENT_0_LINKRATETX=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client_0_LinkRateTx":""}' http://$1:8080/GET_CGI`
CLIENT_0_LINKRATERX=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Client_0_LinkRateRx":""}' http://$1:8080/GET_CGI`
DEVICE=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Device":{}}' http://$1:8080/GET_CGI`
DEVICE_MODELNUMBER=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Device_ModelNumber":""}' http://$1:8080/GET_CGI`
DEVICE_NICKNAME=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Device_NickName":""}' http://$1:8080/GET_CGI`
DEVICE_SN=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Device_SN":""}' http://$1:8080/GET_CGI`
DEVICE_HWVERSION=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Device_HWVersion":""}' http://$1:8080/GET_CGI`
DEVICE_FWVERSION=`curl -H "Content-Type:application/json" --cookie "uid=$uid" -X POST -d '{"Device_FWVersion":""}' http://$1:8080/GET_CGI`


echo $WIFISON
echo $WIFISON_NUMBER
echo $WIFISON_0_MAC
echo $WIFISON_1_MAC
echo $WIFISON_2_MAC
echo $WIFISON_0_ParentAPMAC
echo $WIFISON_1_ParentAPMAC
echo $WIFI_AP
echo $WIFI_AP_NUMBERS
echo $WIFI_AP_0_ENABLE
echo $WIFI_AP_0_SSID
echo $WIFI_AP_0_SSID_SET
echo $WIFI_AP_0_PASSWORD
echo $WIFI_AP_0_PASSWORD_SET
echo $WIFI_AP_0_SSID
echo $WIFI_AP_0_PASSWORD
echo $CLIENT
echo $CLIENT_NUMBER
echo $CLIENT_0_MAC
echo $CLIENT_0_SIGNAL
echo $CLIENT_0_IPv4Address
echo $CLIENT_0_IPv6Address
echo $CLIENT_0_INTERFACE
echo $CLIENT_0_CONNECTIONTYPE
echo $CLIENT_0_LINKRATETX
echo $CLIENT_0_LINKRATERX
echo $DEVICE
echo $DEVICE_MODELNUMBER
echo $DEVICE_NICKNAME
echo $DEVICE_SN
echo $DEVICE_HWVERSION
echo $DEVICE_FWVERSION








 
 
 
 


