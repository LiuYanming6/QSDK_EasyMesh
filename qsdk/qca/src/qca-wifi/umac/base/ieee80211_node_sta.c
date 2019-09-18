/*
* Copyright (c) 2011, 2018 Qualcomm Innovation Center, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Innovation Center, Inc.
*
*/

/*
 * Copyright (c) 2010, Atheros Communications Inc. 
 * All Rights Reserved.
 * 
 * 2011 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 * 
 */

#include "ieee80211_node_priv.h"

#if UMAC_SUPPORT_STA

/* should this be configurable ?*/
#define IEE80211_STA_MAX_NODE_SAVEQ_LEN 300
/*
 * Join an infrastructure network
 */
int
ieee80211_sta_join(struct ieee80211vap *vap, ieee80211_scan_entry_t scan_entry)
{
    struct ieee80211com *ic = vap->iv_ic;
    struct ieee80211_node_table *nt = &ic->ic_sta;
    struct ieee80211_node *ni = NULL;
    struct ieee80211vap *tempvap = NULL;
    const u_int8_t *macaddr = ieee80211_scan_entry_macaddr(scan_entry);
    int error = 0;
    
    ASSERT(vap->iv_opmode == IEEE80211_M_STA);
    
    /*If AP mac to which our sta vap is trying to connect has
    same mac as one of our ap vaps ,dont set that as sta bssid */
    TAILQ_FOREACH(tempvap, &ic->ic_vaps, iv_next) {
        if (tempvap->iv_opmode == IEEE80211_M_HOSTAP && IEEE80211_ADDR_EQ(tempvap->iv_myaddr,macaddr)) {
            QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "[%s] Mac collision for [%s]\n",__func__,ether_sprintf(tempvap->iv_myaddr));
             return -EINVAL;
        }
    }

    ni = ieee80211_vap_find_node(vap, macaddr);
    if (ni) {
        /* 
         * reusing old node has a potential for several bugs . The old node may have some state info from previous association.
         * get rid of the old bss node and create a new bss node.
         */
        ieee80211_sta_leave(ni); 
        ieee80211_free_node(ni); 
    }
    /*
     * Create a BSS node.
     */
    ni = ieee80211_alloc_node(nt, vap, macaddr);
    if (ni == NULL)
        return -ENOMEM;
    /* set the maximum number frmaes to be queued when the vap is in fake sleep */        
    ieee80211_node_saveq_set_param(ni,IEEE80211_NODE_SAVEQ_DATA_Q_LEN,IEE80211_STA_MAX_NODE_SAVEQ_LEN);
    /* To become a bss node, a node need an extra reference count, which alloc node already gives */

    /* setup the bss node for association */
    error = ieee80211_setup_node(ni, scan_entry);
    if (error != 0) {
        ieee80211_free_node(ni);
        return error;
    }

    /* copy the beacon timestamp */
    OS_MEMCPY(ni->ni_tstamp.data,
              ieee80211_scan_entry_tsf(scan_entry),
              sizeof(ni->ni_tstamp));

    /*
     * Join the BSS represented by this new node.
     * This function will free up the old BSS node
     * and use this one as the new BSS node.
     */
    ieee80211_sta_join_bss(ni);

    IEEE80211_ADD_NODE_TARGET(ni, ni->ni_vap, 0);

    /* Save our home channel */
    vap->iv_bsschan = ni->ni_chan;
    vap->iv_cur_mode = ieee80211_chan2mode(ni->ni_chan);

    /* Update the DotH falg */
    ieee80211_update_spectrumrequirement(vap);

    /*
     *  The OS will control our security keys.  
     *  If clear, keys will be cleared.
     *  If static WEP, keys will be plumbed before JoinInfra.
     *  If WPA/WPA2, ciphers will be setup, but no keys will be plumbed until 
     *    after they are negotiated.
     *  XXX We should ASSERT that all of the foregoing is true.
     */
    return 0;
}

#endif
