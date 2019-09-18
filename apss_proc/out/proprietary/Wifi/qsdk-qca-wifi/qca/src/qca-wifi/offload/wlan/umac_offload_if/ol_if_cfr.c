/*
 * Copyright (c) 2016-2018 Qualcomm Innovation Center, Inc.
 *
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 *
 * 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include "ol_if_athvar.h"
#include "ol_txrx_types.h"
#include "linux/socket.h"
#include "linux/netlink.h"
#include <net/netlink.h>
#include "linux/rtnetlink.h"
#include <net/sock.h>

#if ATH_SUPPORT_CFR

#define     NETLINK_CFR_FAMILY                          19
#define     NETLINK_CFR_PID                             0
#define     CFR_CAPTURE_HOST_MEM_REQ_ID                 9
#define     CFR_CAPTURE_HOST_MEM_DEFAULT_READ_OFFSET    8


enum cfrmetaversion
{
    CFR_META_VERSION_NONE,
    CFR_META_VERSION_1,
    CFR_META_VERSION_MAX = 0xFF,

};

enum cfrdataversion
{
    CFR_DATA_VERSION_NONE,
    CFR_DATA_VERSION_1,
    CFR_DATA_VERSION_MAX = 0xFF,


};

enum cfrplatformtype
{
    CFR_PLATFORM_TYPE_NONE,
    CFR_PLATFORM_TYPE_MIPS,
    CFR_PLATFORM_TYPE_ARM,
    CFR_PLATFFORM_TYPE_MAX = 0xFF,
};

enum cfrradiotype
{
    CFR_CAPTURE_RADIO_NONE,
    CFR_CAPTURE_RADIO_OSPREY,
    CFR_CAPTURE_RADIO_PEAKCOCK,
    CFR_CAPTURE_RADIO_SCORPION,
    CFR_CAPTURE_RADIO_HONEYBEE,
    CFR_CAPTURE_RADIO_DRAGONFLY,
    CFR_CAPTURE_RADIO_JET,
    CFR_CAPTURE_RADIO_PEREGRINE = 17,
    CFR_CAPTURE_RADIO_SWIFT,
    CFR_CAPTURE_RADIO_BEELINER,
    CFR_CAPTURE_RADIO_CASCADE,
    CFR_CAPTURE_RADIO_DAKOTA,
    CFR_CAPTURE_RADIO_BESRA,
    CFR_CAPTURE_RADIO_MAX = 0xFF,


};

struct cfr_netlink {
    /* netdev needed?*/
    struct sock      *cfr_sock;
    u_int32_t        cfr_pid;
    atomic_t         cfr_refcnt;
};

struct cfr_netlink *cfr_nl = NULL;

PREPACK struct cfr_metadata_version_1
{
    u_int8_t    peer_addr[IEEE80211_ADDR_LEN];
    u_int8_t    status;
    u_int8_t    capture_bw;
    u_int8_t    channel_bw;
    u_int8_t    phy_mode;
    u_int16_t   prim20_chan;
    u_int16_t   center_freq1;
    u_int16_t   center_freq2;
    u_int8_t    capture_mode;
    u_int8_t    capture_type;
    u_int8_t    sts_count;
    u_int8_t    num_rx_chain;
    u_int32_t   timestamp;
    u_int32_t   length;
}POSTPACK;

PREPACK struct csi_cfr_header {

    u_int32_t   start_magic_num;
    u_int32_t   vendorid;
    u_int8_t    cfr_metadata_version;
    u_int8_t    cfr_data_version;
    u_int8_t    chip_type;
    u_int8_t    pltform_type;
    u_int32_t   Reserved;

    union   {
        struct cfr_metadata_version_1 meta_v1;
    }u;

}POSTPACK;

/* TODO: Have not use this structure. Have to use it?? */
PREPACK struct cfr_info {

    struct csi_cfr_header cfrhdr;
    /* 0 byte array- this will include magic end also and be the last field in the structure */
    u_int8_t    *cfr_data;

}POSTPACK;

PREPACK struct cfr_app_payload_header
{
    u_int8_t    phynetdevidx;
    u_int32_t   seqno;
    u_int16_t   fragno;
}POSTPACK;

static inline int ol_fetch_csi_metadata(ol_txrx_pdev_handle txrx_pdev,
                                            struct htt_cfr_info *phttcfr,
                                                struct csi_cfr_header *pcfr)
{
    struct ol_ath_softc_net80211 *scn = (struct ol_ath_softc_net80211 *)txrx_pdev->ctrl_pdev;
    struct ieee80211com *ic = &scn->sc_ic;

    /* TODO: Need predefined values for Radio and platform,versions */
    pcfr->start_magic_num       = 0xDEADBEAF;
    pcfr->vendorid              = 0x8cfdf0;
    pcfr->cfr_metadata_version  = CFR_META_VERSION_1;
    pcfr->cfr_data_version      = CFR_DATA_VERSION_1;

    if (scn->target_type == TARGET_TYPE_IPQ4019)
        pcfr->chip_type         = CFR_CAPTURE_RADIO_DAKOTA;
    else if (scn->target_type == TARGET_TYPE_QCA9984)
        pcfr->chip_type         = CFR_CAPTURE_RADIO_CASCADE;
    else if (scn->target_type == TARGET_TYPE_QCA9888)
        pcfr->chip_type         = CFR_CAPTURE_RADIO_BESRA;
    else if (scn->target_type == TARGET_TYPE_AR900B)
        pcfr->chip_type         = CFR_CAPTURE_RADIO_BEELINER;
    else
        pcfr->chip_type         = CFR_CAPTURE_RADIO_NONE;

    pcfr->pltform_type          = CFR_PLATFORM_TYPE_ARM;
    pcfr->Reserved              = 0;

    if ( phttcfr->capture_type == HTT_PEER_CFR_CAPTURE_MSG_TYPE_1){

        memcpy(pcfr->u.meta_v1.peer_addr,phttcfr->u.cfr_type_1.peeraddr,IEEE80211_ADDR_LEN);
        pcfr->u.meta_v1.status          = phttcfr->u.cfr_type_1.cap_status;
        pcfr->u.meta_v1.capture_bw      = phttcfr->u.cfr_type_1.capture_bw;
        pcfr->u.meta_v1.channel_bw      = phttcfr->u.cfr_type_1.channel_bw;
        pcfr->u.meta_v1.phy_mode        = phttcfr->u.cfr_type_1.chan_mode;
        pcfr->u.meta_v1.prim20_chan     = phttcfr->u.cfr_type_1.chan_mhz;
        pcfr->u.meta_v1.center_freq1    = phttcfr->u.cfr_type_1.band_center_freq1;
        pcfr->u.meta_v1.center_freq2    = phttcfr->u.cfr_type_1.band_center_freq2;
        pcfr->u.meta_v1.capture_mode    = phttcfr->u.cfr_type_1.capture_mode;
        pcfr->u.meta_v1.capture_type    = phttcfr->u.cfr_type_1.cap_type;
        pcfr->u.meta_v1.sts_count       = phttcfr->u.cfr_type_1.sts;
        pcfr->u.meta_v1.num_rx_chain    = ieee80211com_get_rx_chainmask(ic);
        pcfr->u.meta_v1.timestamp       = phttcfr->u.cfr_type_1.timestamp;
        pcfr->u.meta_v1.length          = phttcfr->u.cfr_type_1.length;

        return sizeof(struct cfr_metadata_version_1) + 16; /* TODO: Use offsetoff instead 16 */
    }else {
        qdf_print("CFR %s,Unsupported capture type,%d\n",__func__,phttcfr->capture_type);
        return -1;
    }
}

static inline int ol_fetch_csi_cfrdata(ol_txrx_pdev_handle txrx_pdev,
                                            struct htt_cfr_info *phttcfr,
                                                u_int8_t* pdata,u_int32_t maxlen)
{
    u_int32_t   *prindex;
    struct ol_ath_softc_net80211 *scn = (struct ol_ath_softc_net80211 *)txrx_pdev->ctrl_pdev;
    u_int32_t *vaddr =  scn->cfr_mem_chunk.vaddr;

    prindex = (u_int32_t*)((u_int8_t*)vaddr + phttcfr->u.cfr_type_1.index);

    if ( phttcfr->u.cfr_type_1.length > maxlen){
        qdf_print("CFR %s,cfr data size error,len:%x,maxlen:%x\n",__func__,phttcfr->u.cfr_type_1.length,maxlen);
        return -1;
    }
    /* TODO: Wrap around conditions needs to handled properly.To discuss with
     * FW and ath10k */
    if ( phttcfr->u.cfr_type_1.cap_status == 1){ 
        memcpy(pdata,(u_int8_t*)prindex ,phttcfr->u.cfr_type_1.length);
        *vaddr =  phttcfr->u.cfr_type_1.index + phttcfr->u.cfr_type_1.length;
        return (phttcfr->u.cfr_type_1.length);
    }else
        return 0;
}

static inline void ol_txrx_htt_cfr_rx_ind_cb(void *ctxt,
                                                struct htt_cfr_info *pcfr,
                                                    u_int32_t cfrdatalen)
{
    struct sk_buff *skb = NULL;
    struct nlmsghdr *nlh;
    u_int8_t *nldata = NULL;
    u_int32_t nlmsglen;
    int metalen = 0,length;
    u_int32_t end_magic = 0xBEAFDEAD;

    ol_txrx_pdev_handle txrx_pdev = (ol_txrx_pdev_handle)ctxt;    
    struct ol_ath_softc_net80211 *scn = (struct ol_ath_softc_net80211 *)txrx_pdev->ctrl_pdev;
    struct csi_cfr_header cfr_header = {0};
    struct cfr_app_payload_header app_hdr = {0};

    int app_hdr_len = sizeof(struct cfr_app_payload_header);

    if (NULL == cfr_nl){
        /* TODO: Add mask print */
        qdf_print("CFR Nl socket not reday\n");
        return;
    }
    metalen = ol_fetch_csi_metadata(txrx_pdev,pcfr,&cfr_header);

    if(metalen < 0){
        qdf_print("CFR metadata fetch error:%d\n",metalen);
        return;
    }

    if (qdf_mem_cmp(scn->sc_osdev->netdev->name, "wifi0", 5) == 0)
        app_hdr.phynetdevidx = 0;
    else if (qdf_mem_cmp(scn->sc_osdev->netdev->name, "wifi1", 5) == 0)
        app_hdr.phynetdevidx = 1;
    else if (qdf_mem_cmp(scn->sc_osdev->netdev->name, "wifi2", 5) == 0)
        app_hdr.phynetdevidx = 2;
    else
        app_hdr.phynetdevidx = 0XFF;

    nlmsglen = app_hdr_len + metalen + cfrdatalen + 4;

    /* TODO: More than 4K size handling,if nlmsg_new fails for more than 4K?*/
    skb = nlmsg_new(nlmsglen, GFP_ATOMIC);

    if ( NULL == skb){
        qdf_print(" CFR,NLMSG alloc failed for len:%x\n",nlmsglen);
        return;
    }

    nlh = nlmsg_put(skb, cfr_nl->cfr_pid, 0, 0, nlmsglen, 0);

    if ( NULL == nlh){
        qdf_print("CFR,NLMSGHL failed for len:%x\n",nlmsglen);
        goto err;
    }

    nldata = NLMSG_DATA(nlh);

    /* TODO:This is internal app header.Sequence and frag  number
     * is zero for non multi data */
    memcpy(nldata,(u_int8_t *)&app_hdr, app_hdr_len);

    memcpy(nldata + app_hdr_len ,(u_int8_t *)&cfr_header,metalen);

    /* TODO: If nlmsg_new fails for more than 4K,need to send it as multiples */
    length = ol_fetch_csi_cfrdata(txrx_pdev,pcfr,nldata + app_hdr_len + metalen ,nlmsglen - app_hdr_len - metalen);
    if (length < 0){
        qdf_print("CFRdata fetch failed for len:%x,%x\n",length,nlmsglen);
        goto err;
    }

    memcpy(nldata + app_hdr_len + metalen + length,(u_int8_t *)&end_magic,4);

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0)
    NETLINK_CB(skb).pid = 0;        /* from kernel */
#else
    NETLINK_CB(skb).portid = 0;     /* from kernel */
#endif
    NETLINK_CB(skb).dst_group = 0;  /* unicast */

#if LINUX_VERSION_CODE == KERNEL_VERSION(3,12,59) && defined (QCA_PARTNER_PLATFORM) && defined (USE_LITEPATH)
    netlink_unicast(cfr_nl->cfr_sock, skb, cfr_nl->cfr_pid, MSG_DONTWAIT,0);
#else
    /* TODO: unicast is more optimized than broadcast ?*/
    //netlink_unicast(cfr_nl->cfr_sock, skb, cfr_nl->cfr_pid, MSG_DONTWAIT);
    length = netlink_broadcast(cfr_nl->cfr_sock, skb, 0,1, GFP_ATOMIC);
#endif
    return;

err:
    kfree_skb(skb);
}

void 
ol_ath_cfr_htt_detach(struct ieee80211com *ic){
    /* TODO:No cleanup action need foe now*/
}

void 
ol_ath_cfr_htt_attach(struct ieee80211com *ic)
{
    int idx;
    u_int32_t  *read_offset;
    struct ol_ath_softc_net80211 *scn = OL_ATH_SOFTC_NET80211(ic);
    ol_txrx_pdev_handle txrx_pdev = scn->pdev_txrx_handle;

    txrx_pdev->cfr_callback.cfr_rx_htt_ind   = NULL;
    txrx_pdev->cfr_callback.ctxt             = NULL;

    if( scn->cfr_support){

        for ( idx = 0; idx < scn->num_mem_chunks; ++idx){

            if (scn->mem_chunks[idx].req_id == CFR_CAPTURE_HOST_MEM_REQ_ID){

                scn->cfr_mem_chunk.req_id   = scn->mem_chunks[idx].req_id;
                scn->cfr_mem_chunk.paddr    = scn->mem_chunks[idx].paddr;
                scn->cfr_mem_chunk.vaddr    = scn->mem_chunks[idx].vaddr;
                scn->cfr_mem_chunk.len      = scn->mem_chunks[idx].len;
                read_offset = scn->cfr_mem_chunk.vaddr;
                (*read_offset) = CFR_CAPTURE_HOST_MEM_DEFAULT_READ_OFFSET;
            }

            if( idx >= scn->num_mem_chunks){
                scn->cfr_support = 0;
                qdf_print("WLAN_CSI:Sharted memeory not allocated for CSI\n");
                return;
            }

        }

        ol_txrx_cfr_rx_cb_set(txrx_pdev,ol_txrx_htt_cfr_rx_ind_cb,txrx_pdev);
    }
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
void cfr_nl_data_ready(struct sock *sk, int len)
{
    return;
#else
void cfr_nl_data_ready(struct sk_buff *skb )
{
    return;
}
#endif

int cfr_netlink_init(void){

    extern struct net init_net;

#if LINUX_VERSION_CODE >= KERNEL_VERSION (3,10,0)

    struct netlink_kernel_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.groups = 1;
    cfg.input = &cfr_nl_data_ready;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)
    
    struct netlink_kernel_cfg cfg = {
        .groups = 1,
        .input  = &cfr_nl_data_ready,
    };

#endif

    if ( NULL == cfr_nl) {

            cfr_nl = (struct cfr_netlink *) kzalloc(sizeof(struct cfr_netlink), GFP_KERNEL);

            if (NULL == cfr_nl) {
                qdf_print("CFR memory alloc failed\n");
                return -ENODEV;
            }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)/* >= (3,10,0) */

            cfr_nl->cfr_sock = (struct sock *)netlink_kernel_create(
                &init_net,
                NETLINK_CFR_FAMILY,
                &cfg);

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)

            cfr_nl->cfr_sock = (struct sock *)netlink_kernel_create(
                &init_net,
                NETLINK_CFR_FAMILY,
                THIS_MODULE, &cfg);

#elif LINUX_VERSION_CODE >= KERNEL_VERSION (2,6,24)

            cfr_nl->cfr_sock = (struct sock *) netlink_kernel_create(
                &init_net,
                NETLINK_CFR_FAMILY, 1,
                &cfr_nl_data_ready,
                NULL, THIS_MODULE);

#elif LINUX_VERSION_CODE >= KERNEL_VERSION (2,6,22)

            cfr_nl->cfr_sock = (struct sock *) netlink_kernel_create(
                NETLINK_CFR_FAMILY, 1,
                &cfr_nl_data_ready,
                NULL, THIS_MODULE);

#else
            cfr_nl->cfr_sock = (struct sock *)netlink_kernel_create(
                NETLINK_CFR_FAMILY, 1,
                &cfr_nl_data_ready,
                THIS_MODULE);
#endif

            if ( NULL == cfr_nl->cfr_sock) {

                kfree(cfr_nl);
                cfr_nl = NULL;
                qdf_print("%s CFR NETLINK_KERNEL_CREATE FAILED\n", __func__);
                return -ENODEV;
            }

            atomic_set(&cfr_nl->cfr_refcnt, 1);
            cfr_nl->cfr_pid = NETLINK_CFR_PID;
            qdf_print("%s CFR Netlink successfully created\n", __func__);

    } else {
        atomic_inc(&cfr_nl->cfr_refcnt);
        qdf_print("%s Incremented CFR netlink ref count: %d\n", __func__,
                        atomic_read(&cfr_nl->cfr_refcnt));
    }
    return 0;

}

int cfr_netlink_delete(void)
{
    if ( NULL == cfr_nl) {
        qdf_print("%s cfr_nl is NULL\n", __func__);
        return -ENODEV;
    }
    
    if (!atomic_dec_and_test(&cfr_nl->cfr_refcnt)) {
        if (cfr_nl->cfr_sock) {
            netlink_kernel_release(cfr_nl->cfr_sock);
        }
        kfree(cfr_nl);
        cfr_nl = NULL;
    }
    return 0;
}

EXPORT_SYMBOL(cfr_netlink_init);
EXPORT_SYMBOL(cfr_netlink_delete);
#endif    /* ATH_SUPPORT_CFR */
