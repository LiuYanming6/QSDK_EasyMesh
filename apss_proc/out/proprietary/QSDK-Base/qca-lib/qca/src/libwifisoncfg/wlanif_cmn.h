/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 *
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

struct wlanif_config {
    void *ctx;
    uint32_t IsCfg80211;
    struct wlanif_config_ops *ops;
    int pvt_cmd_sock_id;
    int pvt_event_sock_id;
};

/* Netlink socket ports for different applications to bind to
 * These ports are reserved for these specific applications and
 * should take care not to reuse it.
 */
#define LBD_NL80211_CMD_SOCK      899
#define LBD_NL80211_EVENT_SOCK    900
#define WSPLCD_NL80211_CMD_SOCK   901
#define WSPLCD_NL80211_EVENT_SOCK 902
#define HYD_NL80211_CMD_SOCK      903
#define HYD_NL80211_EVENT_SOCK    904

struct wlanif_config_ops {
    int (* init) (struct wlanif_config *wext_conf);
    void (* deinit) (struct wlanif_config *wext_conf);
    int (* getName) (void *,const char *, char *);
    int (* isAP) (void *, const char *, uint32_t *);
    int (* getBSSID) (void *, const char *, struct ether_addr *BSSID );
    int (* getESSID) (void *, const char * , void *, uint32_t * );
    int (* getFreq) (void *, const char * , int32_t * freq);
    int (* getChannelWidth) (void *, const char *, int *);
    int (* getChannelExtOffset) (void *, const char *, int *);
    int (* getChannelBandwidth) (void *, const char *, int *);
    int (* getAcsState) (void *, const char *, int *);
    int (* getCacState) (void *, const char *, int *);
    int (* getParentIfindex) (void *, const char *, int *);
    int (* getSmartMonitor) (void *, const char *, int *);
    int (* getGenericInfoAtf) (void *, const char *, int, void *, int);
    int (* getGenericInfoAld) (void *, const char *, void *, int);
    int (* getGenericHmwds) (void *, const char *, void *, int);
    int (* getGenericNac) (void *, const char *, void *, int);
    int (* getCfreq2) (void *, const char * , int32_t *);
    int (* getChannelInfo) (void *, const char *, void *, int);
    int (* getChannelInfo160) (void *, const char *, void *, int);
    int (* getStationInfo) (void *, const char *, void *, int *);
    int (* getDbgreq) (void *, const char *, void *, uint32_t);
    int (* getExtended) (void *, const char *, void *, uint32_t);
    int (* addDelKickMAC) (void *, const char *, int , void *, uint32_t);
    int (* setFilter) (void *, const char *, void *, uint32_t);
    int (* getWirelessMode)(void *, const char *, void *, uint32_t);
    int (* sendMgmt) (void *, const char *, void *, uint32_t);
    int (* setParamMaccmd)(void *, const char *, void *, uint32_t);
    int (* setParam)(void *, const char *,int, void *, uint32_t);
    int (* getStaStats)(void *, const char *, void *, uint32_t);
    int (* getStaCount) (void *, const char *, int *);
    int (* setIntfMode) (void *, const char *, const char *, u_int8_t len);
};

/*enum to handle MAC operations*/
enum wlanif_ioops_t
{
    IO_OPERATION_ADDMAC=0,
    IO_OPERATION_DELMAC,
    IO_OPERATION_KICKMAC,
    IO_OPERATION_LOCAL_DISASSOC,
    IO_OPERATION_ADDMAC_VALIDITY_TIMER,
};

/* enum to handle wext/cfg80211 mode*/
enum wlanif_cfg_mode {
    WLANIF_CFG80211=0,
    WLANIF_WEXT
};

extern struct wlanif_config * wlanif_config_init(enum wlanif_cfg_mode mode,
                                                 int pvt_cmd_sock_id,
                                                 int pvt_event_sock_id);
extern void wlanif_config_deinit(struct wlanif_config *);
