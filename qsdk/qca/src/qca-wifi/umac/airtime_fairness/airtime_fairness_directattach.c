/*
 * Copyright (c) 2014, 2017 Qualcomm Innovation Center, Inc.
 *
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 *
 * 2014 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*
 Air Time Fairness module
*/
#if QCA_AIRTIME_FAIRNESS

#include <ieee80211_var.h>
#include <ieee80211_airtime_fairness.h>
#include <osif_private.h>

/* Definition */
#define IEEE80211_INVALID_MAC(addr) \
    ((!addr[0]) && (!addr[1]) && (!addr[2]) && \
     (!addr[3]) && (!addr[4]) && (!addr[5]))


/**
 * @brief Indicate if the node is ATF capable
 *
 * @param ic handle to the radio
 *
 * @return None
 */
int
node_atf_capable_indicate(struct ieee80211com *ic)
{
    struct     ieee80211_node *ni = NULL;
    int32_t i;
    u_int8_t node_atf_state_prev = 0, atfstate_change = 0;

    /* For each entry in atfcfg structure, find corresponding entry in the node table */
    if(ic->atfcfg_set.peer_num_cal != 0)
    {
        for (i = 0; i < ATF_ACTIVED_MAX_CLIENTS; i++)
        {
            if((ic->atfcfg_set.peer_id[i].index_vap != 0)&&(ic->atfcfg_set.peer_id[i].sta_assoc_status == 1))
            {
                ni = ieee80211_find_node(&ic->ic_sta, ic->atfcfg_set.peer_id[i].sta_mac);
                if(ni == NULL) {
                    continue;
                } else {
                    /* getting the previous node state */
                    node_atf_state_prev =  ni->ni_atfcapable;
                    atfstate_change = 0;

                    /* Mark atf capable clients - if there is a corresponding VAP entry
                       or STA based config */
                    if(ic->ic_atf_maxclient)
                    {
                        if( (ic->atfcfg_set.peer_id[i].index_vap !=0xFF) ||
                            (ic->atfcfg_set.peer_id[i].cfg_flag !=0) )
                        {
                            ni->ni_atfcapable = 1;
                        } else {
                            ni->ni_atfcapable = 0;
                        }
                    } else {
                            ni->ni_atfcapable = 1;
                    }

                    /* Node ATF state changed */
                    if(node_atf_state_prev != ni->ni_atfcapable)
                    {
                        atfstate_change = 1;
                    }
                    if(ic->ic_atf_capable_node) {
                        ic->ic_atf_capable_node(ic, ni, ni->ni_atfcapable, atfstate_change);
                    }
                    ieee80211_free_node(ni);
                }
            }
        }
    }
    return EOK;
}

/**
 * @brief Find matching ac rule
 *
 * @param [in] pointer to radio structure
 *        [in] pointer to group list
 *        [in] pointer to subgroup list
 * @return  peer index on SUCCESS
 *          -EINVAL on Error
 *
 */
int atf_configure_ac_subgroup(struct ieee80211com *ic, struct group_list *group, struct subgroup_list *subgroup)
{
    u_int32_t peerindex = 0, index =0;
    struct atf_ac_config *atfac = NULL;
    u_int8_t peermatch = 0, ac_id = 0, acbitmap = 0;

    for (peerindex = 0; peerindex < ATF_ACTIVED_MAX_CLIENTS; peerindex++) {
        if (!IEEE80211_ADDR_IS_VALID(ic->atfcfg_set.peer_id[peerindex].sta_mac)) {
            continue;
        }
        ac_id = peermatch = acbitmap = 0;
        atfac = NULL;

        /* The ssid/Group name that the client is now associated to should match
         * with the group name passed as func argument
         */
        if (ic->ic_atf_ssidgroup) {
            index = ic->atfcfg_set.peer_id[peerindex].index_group;
            /* check if Peer's group name matches with the group name
             */
            if ((ic->atfcfg_set.peer_id[peerindex].index_group != 0) &&
                (ic->atfcfg_set.peer_id[peerindex].sta_assoc_status == 1) &&
                (ic->atfcfg_set.peer_id[peerindex].sta_cfg_value[index] == 0)) {
                if (strncmp(ic->atfcfg_set.atfgroup[index - 1].grpname, group->group_name, IEEE80211_NWID_LEN) == 0) {
                    atfac = &ic->atfcfg_set.atfgroup[index - 1].atf_cfg_ac;
                    peermatch = 1;
                }
            }
        } else {
            index = ic->atfcfg_set.peer_id[peerindex].index_vap;
            if ((ic->atfcfg_set.peer_id[peerindex].index_vap != 0) &&
                (ic->atfcfg_set.peer_id[peerindex].sta_assoc_status == 1) &&
                (ic->atfcfg_set.peer_id[peerindex].sta_cfg_value[index] == 0)) {
                /* compare ssid with node essid */
                if (strncmp(ic->atfcfg_set.vap[index - 1].essid, group->group_name, IEEE80211_NWID_LEN) == 0) {
                    atfac = &ic->atfcfg_set.vap[index - 1].atf_cfg_ac;
                    peermatch = 1;
                }
            }
        }

        /* if the group name matched, check for ac based configuration
         * in the respective VAP or Group index of the peer */
        if (atfac && peermatch && atfac->ac_bitmap) {
            acbitmap = atfac->ac_bitmap;
            while (acbitmap) {
                if ((1 & acbitmap) &&
                    (ac_id == subgroup->ac_id)) {
                    subgroup->sg_atf_units = ic->atfcfg_set.peer_id[peerindex].sta_cal_ac_value[ac_id];
                }
                ac_id++;
                acbitmap = acbitmap >> 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Find matching peer in the ATF Peer table
 *
 * @param [in] mac address to match
 * @return  peer index on SUCCESS
 *          -EINVAL on Error
 *
 */
int atf_find_peer_config_match(struct ieee80211com *ic, struct group_list *group, u_int8_t *sta_mac)
{
    int index = 0, ret = -1;
    struct     ieee80211vap *vap = NULL;
    struct     ieee80211_node *ni = NULL;
    int i = 0;

    for (index = 0; index < ATF_ACTIVED_MAX_CLIENTS; index++) {
        /* compare if peer mac in the ATF peer table & the subgroup matches */
        if (IEEE80211_ADDR_EQ(ic->atfcfg_set.peer_id[index].sta_mac, sta_mac)) {
            ni = ieee80211_find_node(&ic->ic_sta, sta_mac);
            if (ni == NULL) {
                continue;
            }
            vap = ni->ni_vap;
            if (vap == NULL)
                continue;

            /* the group/ssid name is the name where the peer is associated to.
             * Match group/ssid name with the subgroup's parent group name
             */
            if (ic->ic_atf_ssidgroup) {
                i = ic->atfcfg_set.peer_id[index].index_group;
                if ((ic->atfcfg_set.peer_id[index].index_group != 0) &&
                    (ic->atfcfg_set.peer_id[index].sta_assoc_status == 1)) {
                    /* check if the SSID to which peer is connected to, is part of the group */
                    if (strncmp(ic->atfcfg_set.atfgroup[i - 1].grpname, group->group_name, IEEE80211_NWID_LEN) == 0) {
                        ret = index;
                        ieee80211_free_node(ni);
                        goto exit;
                    }
                }
            } else {
                i = ic->atfcfg_set.peer_id[index].index_vap;
                if ((ic->atfcfg_set.peer_id[index].index_vap != 0) &&
                    (ic->atfcfg_set.peer_id[index].sta_assoc_status == 1)) {
                    /* SSID of the VAP should match the group name */
                    if (strncmp(ic->atfcfg_set.vap[i - 1].essid, group->group_name, IEEE80211_NWID_LEN) == 0) {
                        ret = index;
                        ieee80211_free_node(ni);
                        goto exit;
                    }
                }
            }
            ieee80211_free_node(ni);
        }
    }

exit:
    return ret;
}

/**
 * @brief Assign airtime to the subgroups of PEER type
 *
 * @param [in] ic  the handle to the radio
 * @return  0 on SUCCESS
 *
 */
int
assign_airtime_sg_peer(struct ieee80211com *ic)
{
    int peer_index = 0;
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            if (subgroup->sg_type == IEEE80211_ATF_SUBGROUP_TYPE_PEER) {
                peer_index = atf_find_peer_config_match(ic, group, subgroup->sg_peermac);
                if (peer_index >= 0) {
                    subgroup->sg_atf_units = ic->atfcfg_set.peer_id[peer_index].sta_cal_value;
                }
            }
        }
    }

    return 0;
}

/**
 * @brief Assign airtime to the subgroups of AC type
 *
 * @param [in] ic  the handle to the radio
 * @param [in] subgroup List of subgroups
 * @return  airtime for peer on SUCCESS
 *          -EINVAL on Error
 *
 */
int
assign_airtime_sg_ac(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            if (subgroup->sg_type == IEEE80211_ATF_SUBGROUP_TYPE_AC) {
                atf_configure_ac_subgroup(ic, group, subgroup);
            }
        }
    }

    return 0;
}

/**
 * @brief Assign airtime to the subgroups of SSID type
 *
 * @param [in] ic  the handle to the radio
 * @return 0 on SUCCESS
 *
 */
int
assign_airtime_sg_ssid(struct ieee80211com *ic)
{
    int airtime = 0, peer_ac_airtime = 0;
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    TAILQ_FOREACH (group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH (subgroup, &group->atf_subgroups, sg_next) {
            if (subgroup->sg_type == IEEE80211_ATF_SUBGROUP_TYPE_SSID) {
                subgroup->sg_atf_units = 0;
                peer_ac_airtime = group->tot_airtime_peer + group->tot_airtime_ac;
                if (peer_ac_airtime > group->group_airtime) {
                    peer_ac_airtime = 0;
                }
                airtime = group->group_airtime - peer_ac_airtime;
                if (airtime >= 0) {
                    subgroup->sg_atf_units = airtime;
                } else {
                    subgroup->sg_atf_units = 0;
                }
            }
        }
    }

    return 0;
}

/**
 * @brief Calculate total airtime assigned to peer & ac subgroup within a group
 * @param [in] ic  the handle to the radio
 * @return  0 on SUCCESS
 *          -EINVAL on Error
 *
 */
void calculate_airtime_group_peer_ac(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;


    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        group->tot_airtime_peer = 0;
        group->tot_airtime_ac = 0;
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            if (subgroup->sg_type == IEEE80211_ATF_SUBGROUP_TYPE_PEER) {
                group->tot_airtime_peer += subgroup->sg_atf_units;
            } else if (subgroup->sg_type == IEEE80211_ATF_SUBGROUP_TYPE_AC) {
                group->tot_airtime_ac += subgroup->sg_atf_units;
            }
        }
    }
}

/**
 * @brief Reset airtime assigned to all subgroups
 * @param [in] ic  the handle to the radio
 *
 */
void ieee80211_atf_reset_airtime_sg_all(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
             subgroup->sg_atf_units = 0;

             /* reset contribute level when atf table is updated (node joins)
              */
             subgroup->sg_contribute_level = 0;
             subgroup->sg_contribute_id = 0;
        }
    }
}

/**
 * @brief Assign airtime to the subgroups
 * @param [in] ic  the handle to the radio
 * @return  0 on SUCCESS
 *          -EINVAL on Error
 *
 */
int
assign_airtime_subgroup(struct ieee80211com *ic)
{
    ieee80211_atf_reset_airtime_sg_all(ic);
    assign_airtime_sg_peer(ic);
    assign_airtime_sg_ac(ic);
    calculate_airtime_group_peer_ac(ic);
    assign_airtime_sg_ssid(ic);

    return 0;
}

/**
 * @brief Derive txtokens based on the airtime assigned for the node.
 *
 * @param [in] node table, airtime, token distribution timer interval.
 *
 * @return None
 */
u_int32_t ieee80211_atf_compute_txtokens(struct ieee80211com *ic,
                             u_int32_t atf_units, u_int32_t token_interval_ms)
{
    u_int32_t tx_tokens;

    if (!atf_units) {
        return 0;
    }

    if (ic->ic_atf_sched & IEEE80211_ATF_SCHED_OBSS) {
        /* If OBSS scheduling is enabled, use the actual availabe tokens */
        token_interval_ms = ic->atf_avail_tokens;
    }

    /* if token interval is 1 sec & atf_units assigned is 100 %,
       tx_tokens = 1000000
     */
    tx_tokens = token_interval_ms * 1000; /* Convert total token time to uses. */
    /* Derive tx_tokens for this peer, w.r.t. ATF denomination and scheduler token_units */
    tx_tokens = (atf_units * tx_tokens) / WMI_ATF_DENOMINATION;
    return tx_tokens;
}


/**
 * @brief Check if the peer if valid
 *
 * @param [in] node table
 *
 * @return node table entry
 */
struct ieee80211_node *ieee80211_atf_valid_peer(struct ieee80211_node *ni)
{
    /* uninitialized peer */
    if( IEEE80211_INVALID_MAC(ni->ni_macaddr) ) {
        goto peer_invalid;
    }

    /* skip peers that aren't attached to a VDEV */
    if( ni->ni_vap ==NULL ) {
        goto peer_invalid;
    }

    /* skip non-AP vdevs */
    if( ni->ni_vap->iv_opmode != IEEE80211_M_HOSTAP ) {
        goto peer_invalid;
    }

    /* skip NAWDS-AP vdevs */

    /* skip AP BSS peer */
    if( ni == ni->ni_bss_node ) {
        goto peer_invalid;
    }

    return ni;

peer_invalid:
    return NULL;
}

u_int32_t ieee80211_atf_avail_tokens(struct ieee80211com *ic)
{
    u_int8_t ctlrxc, extrxc, rfcnt, tfcnt, obss;
    u_int32_t avail = ATF_TOKEN_INTVL_MS;
    
    /* get individual percentages */
    ctlrxc = ic->ic_atf_chbusy & 0xff;
    extrxc = (ic->ic_atf_chbusy & 0xff00) >> 8;
    rfcnt = (ic->ic_atf_chbusy & 0xff0000) >> 16;
    tfcnt = (ic->ic_atf_chbusy & 0xff000000) >> 24;
    
    if ((ctlrxc == 255) || (extrxc == 255) || (rfcnt == 255) || (tfcnt == 255))
        return ic->atf_avail_tokens;
    
    if (ic->ic_curchan->ic_flags & IEEE80211_CHAN_HT20)
        obss = ctlrxc - tfcnt;
    else
        obss = (ctlrxc + extrxc) - tfcnt;
    
    /* availabe % is 100 minus obss usage */
    avail = (100 - obss);
    
    /* Add a scaling factor and calculate the tokens*/
    if (ic->atf_obss_scale) {
        avail += avail * ic->atf_obss_scale / 100;
        avail = (avail * ATF_TOKEN_INTVL_MS / 100);
    }
    else {
        avail = (avail * ATF_TOKEN_INTVL_MS / 100) + 15;
    }    

    /* Keep a min of 30 tokens */
    if (avail < 30)
        avail = 30;
    
    return (avail < ATF_TOKEN_INTVL_MS) ? avail : ATF_TOKEN_INTVL_MS;
}

/* Parse the group list and remove any group marked for deletion */
int ieee80211_atf_del_group(struct ieee80211com *ic)
{
    struct group_list *group = NULL, *tmpgroup_next = NULL;

    /* Parse the group list and remove any group marked for deletion */
    TAILQ_FOREACH_SAFE(group, &ic->ic_atfgroups, group_next, tmpgroup_next) {
        if(group->group_del == 1)
        {
            TAILQ_REMOVE(&ic->ic_atfgroups, group, group_next);
            OS_FREE(group);
            group = NULL;
        }
    }

    return 0;
}

/**
 * @brief Delete subgroups marked for delete
 *
 * @param [in] ic  the handle to the radio
 *
 * @return None
 */
int ieee80211_atf_del_subgroup(struct ieee80211com *ic)
{
    struct subgroup_list *subgroup = NULL, *tmpsg_next = NULL;
    struct group_list *group = NULL;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH_SAFE(subgroup, &group->atf_subgroups, sg_next, tmpsg_next) {
            if(subgroup->sg_del == 1) {
                qdf_spinlock_destroy(&subgroup->atf_sg_lock);
                TAILQ_REMOVE(&group->atf_subgroups, subgroup, sg_next);
                OS_FREE(subgroup);
                subgroup = NULL;
            }
        }
    }

//    dump_atf_group_subgroup(ic);
    return 0;
}

/**
 * @brief Resets variables used for buffer account
 *
 * @param [in] subgroup pointer to the subgroup
 *
 * @return 0
 */
void ieee80211_atf_reset_buf_account(struct ieee80211com *ic, struct subgroup_list *subgroup)
{
    subgroup->sg_min_num_buf_held = subgroup->sg_num_buf_held;
    subgroup->sg_max_num_buf_held = subgroup->sg_num_buf_held;
    subgroup->sg_pkt_drop_nobuf = 0;
    subgroup->sg_allowed_buf_updated = 0;
    subgroup->sg_num_bufs_sent = 0;
    subgroup->sg_num_bytes_sent = 0;

    if (IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) {
        if (subgroup->sg_num_buf_held > ic->atf_txbuf_min) {
            subgroup->sg_allowed_bufs = subgroup->sg_num_buf_held;
        } else {
            subgroup->sg_allowed_bufs = ic->atf_txbuf_min;
        }
    } else {
        subgroup->sg_allowed_bufs = 0;
    }
}

/**
 * ieee802111_atf_compute_actual_tokens
 * Deduct any excess tokens consumed in previous cycle
 * @subgroup: Subgroup list
 *
 * Return: 0 on success
 *         -EIVAL on error
 */
int ieee802111_atf_compute_actual_tokens(struct ieee80211com *ic, struct subgroup_list *subgroup)
{
    if (!subgroup) {
        return -EINVAL;
    }

    ieee80211_atf_reset_buf_account(ic, subgroup);

    if (subgroup->tokens_excess_consumed) {
        /* deduct any excess tokens consumed in the previous cycle */
        if(subgroup->tx_tokens >= subgroup->tokens_excess_consumed)
            subgroup->tx_tokens -= subgroup->tokens_excess_consumed;
        else
            subgroup->tx_tokens = 0;

        subgroup->tokens_excess_consumed = 0;
    }

    if (subgroup->tokens_less_consumed) {
        /* estimation < actual. Add the difference */
        subgroup->tx_tokens += subgroup->tokens_less_consumed;
        subgroup->tokens_less_consumed = 0;
    }
    subgroup->sg_act_tx_tokens = subgroup->tx_tokens;

    return 0;
}

/**
 * @ ATF strict queue scheduling algorithm
 *
 * @param [in] ic  the handle to the radio
 *
 * @return true
 */
int ieee80211_atf_strictq_algo(struct ieee80211com *ic)
{
    struct subgroup_list *subgroup = NULL;
    struct group_list *group = NULL;

    ic->ic_alloted_tx_tokens = 0;
    ieee80211_atf_manage_unassigned_tokens(ic);
    if (ic->ic_atf_maxclient)
        ic->ic_atf_tokens_unassigned(ic, ic->atf_tokens_unassigned);

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            ieee80211_atf_update_stats(ic, group, subgroup, subgroup->tx_tokens);
            subgroup->tx_tokens = ieee80211_atf_compute_txtokens(ic, subgroup->sg_atf_units, ATF_TOKEN_INTVL_MS);
            subgroup->sg_raw_tx_tokens = subgroup->sg_shadow_tx_tokens = subgroup->tx_tokens;
            ic->ic_alloted_tx_tokens += subgroup->tx_tokens;

            ieee802111_atf_compute_actual_tokens(ic, subgroup);
        }
    }
    if (ic->ic_atf_maxclient) {
        ic->ic_alloted_tx_tokens += ic->atf_tokens_unassigned;
        ic->ic_txtokens_common = ic->atf_tokens_unassigned;
     } else
        ic->ic_txtokens_common = 0;
    ic->ic_shadow_alloted_tx_tokens = ic->ic_alloted_tx_tokens;

    /* Iterate Node table & unblock all nodes at every timer tick */
    ieee80211_iterate_node(ic, ieee80211_node_iter_unblock_nodes, ic);

    /* Updated TIDstate stat */
    ieee80211_atf_update_stats_tidstate(ic);

    /* Update stats pointer */
    ieee80211_atf_stats_history(ic);

    return 0;
}

/**
 * @brief Unpause nodes every on every timer tick
 * Unpause the ATF paused TIDs on every atf scheduler timer tick
 *
 * @param [in] ic  the handle to the radio
 *        [ni] node pointer
 *
 * @return None
 */
void ieee80211_atf_node_unblock(struct ieee80211com *ic, struct ieee80211_node *ni)
{
    ic->ic_atf_node_unblock(ic, ni, &ni->ni_atf_stats);
}

/**
 * @brief If the peer is valid, update txtokens to the lmac layer
 * Txtokens will be used for Tx scheduling
 *
 * @param [in] ic  the handle to the radio
 *
 * @return true if handle is valid; otherwise false
 */
void ieee80211_node_iter_dist_txtokens_strictq(void *arg, struct ieee80211_node *ni)
{
    struct ieee80211com *ic = (struct ieee80211com *)arg;

    ieee80211_atf_node_unblock(ic, ni);
}

/**
 * @brief Reset ic & group list variables each timer cycle
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return None
 */
void ieee80211_atf_init_ic_group_var(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    ic->atf_groups_borrow = 0;
    ic->atf_total_num_clients_borrow = 0;
    ic->atf_total_contributable_tokens = 0;
    ic->ic_atf_num_active_sg = 0;
    ic->ic_atf_num_active_sg_strict = 0;
    ic->ic_atf_num_active_sg_fair_ub = 0;
    ic->atf_num_sg_borrow_strict = 0;
    ic->atf_num_sg_borrow_fair_ub = 0;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        group->shadow_atf_contributabletokens = group->atf_contributabletokens;
        group->atf_num_sg_borrow = 0;
        group->atf_contributabletokens = 0;
        group->atf_num_active_sg = 0;
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            IEEE80211_ATF_RESET_SG_ACTIVE(subgroup);
        }
    }
}

/**
 * @brief Get unused txtokens of each subgroup per timer tick
 *
 * @param [in] subgroup pointer to subgroup list
 *
 * @return unused tokens
 *         -EINVAL on error
 */
uint32_t ieee80211_atf_get_unused_txtokens(struct subgroup_list *subgroup)
{
    u_int32_t unusedtokens = 0;

    if (subgroup == NULL)
        return -EINVAL;

    qdf_spin_lock_bh(&subgroup->atf_sg_lock);
    unusedtokens = subgroup->tx_tokens;
    qdf_spin_unlock_bh(&subgroup->atf_sg_lock);

    return unusedtokens;
}

void ieee80211_atf_update_stats_tidstate(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    /* These stats is applicable only if there is one Node associated to a subgroup.
     * If there are multiple nodes linked to the subgroup, this won't reflect the
     * actual state
     */
    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            subgroup->sg_atf_stats.atftidstate = subgroup->sg_atftidstate;
        }
    }
}

/**
 * @brief Updates atf statistics
 *
 * @param [in] ic  pointer to ic radio structure
 *        [in] subgroup pointer to subgroup list
 *
 * @return None
 */
void ieee80211_atf_update_stats(struct ieee80211com *ic, struct group_list *group, struct subgroup_list *subgroup, u_int32_t sg_unusedtokens)
{
    if (!subgroup) {
        return;
    }

    /* Clear the stats structure before updating */
    qdf_mem_set((uint8_t *)&subgroup->sg_atf_stats, sizeof(struct atf_stats), 0);

    /* Stats related to token */
    if (!group->group_sched)
        subgroup->sg_atf_stats.tot_contribution = group->shadow_atf_contributabletokens;
    else
        subgroup->sg_atf_stats.tot_contribution = 0;

    subgroup->sg_atf_stats.contribution = subgroup->sg_contributedtokens;
    subgroup->sg_atf_stats.contr_level = subgroup->sg_contribute_level;
    subgroup->sg_atf_stats.borrow = subgroup->sg_borrowedtokens;
    subgroup->sg_atf_stats.unused = sg_unusedtokens;
    subgroup->sg_atf_stats.tokens = subgroup->sg_shadow_tx_tokens;
    subgroup->sg_atf_stats.raw_tx_tokens = subgroup->sg_raw_tx_tokens;
    subgroup->sg_atf_stats.total = ic->ic_shadow_alloted_tx_tokens;
    subgroup->sg_atf_stats.act_tokens = subgroup->sg_act_tx_tokens;
    subgroup->sg_atf_stats.timestamp = OS_GET_TIMESTAMP();

    /*buffer accouting stats */
    subgroup->sg_atf_stats.min_num_buf_held = subgroup->sg_min_num_buf_held;
    subgroup->sg_atf_stats.max_num_buf_held = subgroup->sg_max_num_buf_held;
    subgroup->sg_atf_stats.pkt_drop_nobuf = subgroup->sg_pkt_drop_nobuf;
    subgroup->sg_atf_stats.allowed_bufs = subgroup->sg_allowed_bufs;
    subgroup->sg_atf_stats.num_tx_bufs = subgroup->sg_num_bufs_sent;
    subgroup->sg_atf_stats.num_tx_bytes = subgroup->sg_num_bytes_sent;

    if ( (subgroup->sg_atf_stats.act_tokens > sg_unusedtokens) && (subgroup->sg_atf_stats.total > 0)) {
        // Note the math: 200k tokens every 200 ms => 1000k tokens / second => 1 token = 1 us.
        subgroup->sg_atf_stats.total_used_tokens += (subgroup->sg_atf_stats.act_tokens - sg_unusedtokens);
        if (ic->ic_atf_logging) {
            QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "subgroup %s is currently using %d usecs which is %d%% of available airtime\n",
                    subgroup->sg_name,
                    (subgroup->sg_atf_stats.act_tokens - sg_unusedtokens),
                    (((subgroup->sg_atf_stats.act_tokens - sg_unusedtokens)*100) /subgroup->sg_atf_stats.total) );
        }
    }
    subgroup->sg_atf_stats.tokens_common = ic->ic_txtokens_common;
    subgroup->sg_atf_stats.tokens_unassigned = ic->atf_tokens_unassigned;
    ic->atf_tokens_unassigned = 0;

    //subgroup->sg_atf_stats.throughput = ni->ni_throughput;
}

void ieee80211_atf_stats_history(struct ieee80211com *ic)
{
    struct subgroup_list *subgroup = NULL;
    struct group_list *group = NULL;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            qdf_spin_lock_bh(&subgroup->atf_sg_lock);
            if (subgroup->sg_atf_debug) {
                /* Make sure that the history buffer didn't get freed while taking the lock */
                if (subgroup->sg_atf_debug) {
                    subgroup->sg_atf_debug[subgroup->sg_atf_debug_id++] = subgroup->sg_atf_stats;
                    subgroup->sg_atf_debug_id &= subgroup->sg_atf_debug_mask;
                }
            }
            qdf_spin_unlock_bh(&subgroup->atf_sg_lock);
        }
    }
}

/*
 * @brief Log atf data (initial few values, till ATF_DATA_LOG_SIZE);
 *
 * @param [in] ic  pointer to ic radio structure
 *        [in] subgroup pointer to subgroup list
 *
 * @return  0 on success
 *          -EINVAL on error
 */
int ieee80211_atf_logdata_init(struct ieee80211com *ic,
                               struct subgroup_list *subgroup, u_int32_t sg_unusedtokens)
{
    if (subgroup == NULL) {
        qdf_print("%s subgroup is NULL \n",__func__);
        return -EINVAL;
    }

    if (subgroup->sg_atfindex) {
        if (!IEEE80211_ATF_IS_SG_ACTIVE(subgroup) ||
            !subgroup->sg_shadow_tx_tokens) {
            subgroup->sg_unusedtokenpercent[subgroup->sg_atfindex - 1] = 100;
        } else if (sg_unusedtokens <= subgroup->sg_shadow_tx_tokens) {
            subgroup->sg_unusedtokenpercent[subgroup->sg_atfindex - 1] =
                ((sg_unusedtokens * 100)/subgroup->sg_shadow_tx_tokens);
        } else {
            subgroup->sg_unusedtokenpercent[subgroup->sg_atfindex - 1] = 0;
        }
        IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                "(sg %s) atfdata_logged : %d sg_atfindex : %d unused token per : %d\n",
                subgroup->sg_name, subgroup->sg_atfdata_logged, subgroup->sg_atfindex - 1,
                subgroup->sg_unusedtokenpercent[subgroup->sg_atfindex - 1]);
    }
    subgroup->sg_atfdata_logged++;
    subgroup->sg_atfindex++;

     if (subgroup->sg_atfindex >= ATF_DATA_LOG_SIZE) {
         IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                  "%s sg_atfindex > %d . reset to 0 \n\r", subgroup->sg_name, subgroup->sg_atfindex);
         subgroup->sg_atfindex = 0;
     }
     subgroup->sg_atf_stats.weighted_unusedtokens_percent = 0;

     return 0;
}

/**
 * @brief log unused tokens for each subgroup
 *
 * @param [in]  subgroup pointer to subgroup lis
 *              sg_unusedtokens unused tokens from previous cycle
 *
 * @return  subgroup index on success
 *          -EINVAL on error
 */
u_int32_t ieee80211_atf_subgroup_compute_usage(struct ieee80211com *ic,
                        struct subgroup_list *subgroup, u_int32_t sg_unusedtokens)
{
    int32_t sg_index = 0;

    if (subgroup == NULL) {
        qdf_print("%s subgroup is NULL \n",__func__);
        return -EINVAL;
    }

    switch(subgroup->sg_atfindex) {
        case 0:
            sg_index = (ATF_DATA_LOG_SIZE - 1);
            break;
        case ATF_DATA_LOG_SIZE:
            sg_index = 0;
            break;
        default:
            sg_index = (subgroup->sg_atfindex - 1);
     }

    /* If subgroup had borrowed tokens in the previous iteration,
     * Do not account for it while calculating unusedtoken percent
     */
    if (!IEEE80211_ATF_IS_SG_ACTIVE(subgroup) ||
       (!subgroup->sg_shadow_tx_tokens)) {
        subgroup->sg_unusedtokenpercent[sg_index] = 100;
    } else if (subgroup->sg_atfborrow) {
        if (sg_unusedtokens >= subgroup->sg_borrowedtokens) {
            sg_unusedtokens = sg_unusedtokens - subgroup->sg_borrowedtokens;
        } else {
            sg_unusedtokens = 0;
        }
        if (subgroup->sg_shadow_tx_tokens >= subgroup->sg_borrowedtokens) {
            subgroup->sg_unusedtokenpercent[sg_index] =
                ((sg_unusedtokens * 100)/(subgroup->sg_shadow_tx_tokens - subgroup->sg_borrowedtokens));
        } else {
            subgroup->sg_unusedtokenpercent[sg_index] = 0;
        }
    } else if (sg_unusedtokens <= subgroup->sg_shadow_tx_tokens) {
        subgroup->sg_unusedtokenpercent[sg_index] =
            ((sg_unusedtokens * 100)/subgroup->sg_shadow_tx_tokens);
    } else {
        subgroup->sg_unusedtokenpercent[sg_index] = 100;
    }
    /* update unused token percent stat for the previous cycle */
    subgroup->sg_atf_stats.unusedtokens_percent = subgroup->sg_unusedtokenpercent[sg_index];
    IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
            "%s unused token percent [%d]: %d \n", subgroup->sg_name, sg_index, subgroup->sg_unusedtokenpercent[sg_index]);

    return sg_index;
}

/**
 * @brief log usage
 *
 * @param [in]  ic pointer to radio structure
 *              subgroup pointer to subgroup list
 *
 * @return 0 on success
 *         -EINVAL on error
 */
int ieee80211_atf_log_usage(struct ieee80211com *ic, struct subgroup_list *subgroup, int32_t sg_unusedtokens)
{
    if (subgroup == NULL)
        return -EINVAL;

    if (subgroup->sg_atfdata_logged < ATF_DATA_LOG_SIZE) {
        /* log the initial values */
        ieee80211_atf_logdata_init(ic, subgroup, sg_unusedtokens);
        return -EINVAL;
    }

    /* compute unused tokens of subgroup and log*/
    return ieee80211_atf_subgroup_compute_usage(ic, subgroup, sg_unusedtokens);
}

/**
 * @brief compute weighted unused percentage
 * Compute weighted unused percentage. The recent entries
 * are given more weight than older entries
 *
 * @param [in]  ic pointer to radio structure
 *              subgroup pointer to subgroup list
 *              index to the Log
 *
 * @return 0 on success
 *         -EINVAL on error
 */
int ieee80211_atf_compute_weighted_tokens(struct ieee80211com *ic,
                            struct subgroup_list *subgroup, int32_t sg_index)
{
    int32_t j = 0, i = 0;
    u_int32_t weighted_unusedtokens_percent = 0;
    int32_t unusedairtime_weights[ATF_DATA_LOG_SIZE] = {60, 30, 10};

    if (subgroup == NULL) {
        qdf_print("%s subgroup is NULL\n",__func__);
        return -EINVAL;
    }

    for (j = sg_index, i =0 ; i < ATF_DATA_LOG_SIZE; i++) {
        weighted_unusedtokens_percent += ((subgroup->sg_unusedtokenpercent[j] * unusedairtime_weights[i]) / 100);
        IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                              "i: %d index : %d weight : %d , unusedtokenpercent : %d weighted_cal : %d \n\r",
                              i, j, unusedairtime_weights[i], subgroup->sg_unusedtokenpercent[j],
                              weighted_unusedtokens_percent);
        j++;
        if (j == ATF_DATA_LOG_SIZE)
        {
            j = 0;
        }
    }

    /* Weighted unused token percent stat for the previous cycle */
    subgroup->sg_atf_stats.weighted_unusedtokens_percent = weighted_unusedtokens_percent;

    return weighted_unusedtokens_percent;
}

int ieee80211_atf_compute_tokens_contribute(struct ieee80211com *ic,
                    struct group_list *group, struct subgroup_list *subgroup, u_int32_t wt_unusedtokens_per)
{
    u_int32_t compensation = 0;
    int32_t reserve_percent[ATF_MAX_CONTRIBUTE_LEVEL + 1] = {75, 50, 20, 5, 1};
    u_int32_t contribute_level = 0, contribute_percent = 0;

    if (subgroup == NULL) {
        qdf_print("%s subgroup is NULL \n",__func__);
        return -EINVAL;
    }

    if (group == NULL) {
        qdf_print("%s group is NULL \n",__func__);
        return -EINVAL;
    }

    if (subgroup->sg_atfdata_logged < ATF_DATA_LOG_SIZE) {
        return 0;
    }

    if (!subgroup->sg_shadow_tx_tokens) {
        return 0;
    }

    /* subgroups with unusedtokens greater than a threshold can contribute */
    if (wt_unusedtokens_per > ATF_UNUSEDTOKENS_CONTRIBUTE_THRESHOLD) {
        /* Compute the node tokens that can be contributed and deduct it from node tokens */
        subgroup->sg_atfborrow = subgroup->sg_borrowedtokens = 0;
        /* tx_tokens will be zero until atfcfg_timer updates atf_units */
        if (subgroup->sg_shadow_tx_tokens) {
            contribute_level = subgroup->sg_contribute_level;
            contribute_percent = wt_unusedtokens_per - ((reserve_percent[contribute_level] * wt_unusedtokens_per) / 100);
            subgroup->sg_contributedtokens = (contribute_percent * subgroup->sg_shadow_tx_tokens) / 100;
            if (subgroup->sg_contributedtokens <= subgroup->sg_shadow_tx_tokens) {
                subgroup->sg_shadow_tx_tokens -= subgroup->sg_contributedtokens;
            } else {
                printk("%s CHECK!!! sg_contributed tokens %d > shadow_tx_tokens : %d \n",
                        __func__, subgroup->sg_contributedtokens, subgroup->sg_shadow_tx_tokens);
                subgroup->sg_contributedtokens = 0;
            }

            /* set a lower threshold for ni->tx_tokens */
            if (subgroup->sg_shadow_tx_tokens < (ATF_RESERVERD_TOKEN_PERCENT * ATF_TOKEN_INTVL_MS * 10) &&
                subgroup->sg_num_buf_held && IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) { /* 1% of airtime */
                compensation = (ATF_RESERVERD_TOKEN_PERCENT * ATF_TOKEN_INTVL_MS * 10) - subgroup->sg_shadow_tx_tokens;
                /* can compensate back upto a max of what the node was contributing */
                if (compensation > subgroup->sg_contributedtokens) {
                    compensation = subgroup->sg_contributedtokens;
                }
                subgroup->sg_shadow_tx_tokens += compensation;
                subgroup->sg_contributedtokens -= compensation;
            }

            /* Set the level of contribution
             * Increment contribution level every ic_atf_contrlevel_incr iteration count */
            subgroup->sg_contribute_id++;
            subgroup->sg_contribute_id &= (ic->ic_atf_contrlevel_incr - 1);
            if (!IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) {
                subgroup->sg_contribute_level = ATF_MAX_CONTRIBUTE_LEVEL;
            } else if (!subgroup->sg_contribute_id &&
                       subgroup->sg_contribute_level < ATF_MAX_CONTRIBUTE_LEVEL) {
                subgroup->sg_contribute_level++;
            }
        } else {
            subgroup->sg_contributedtokens = subgroup->sg_shadow_tx_tokens = 0;
            subgroup->sg_contribute_id = 0;
            subgroup->sg_contribute_level = 0;
        }
        group->atf_contributabletokens += subgroup->sg_contributedtokens;
        IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                            "%s, Tokens to contribute : %d total_contributable tokens : %d tx_tokens : %d\n\r",
                            subgroup->sg_name, subgroup->sg_contributedtokens,
                            group->atf_contributabletokens, subgroup->sg_shadow_tx_tokens);
    } else {
        /* If unused tokens percentage is less than a min threshold
         * set borrow flag */
        subgroup->sg_atfborrow = 1;
        subgroup->sg_contributedtokens = 0;
        group->atf_num_sg_borrow++;
        IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                             "subgroup: %s, borrow enabled! atf_num_sg_borrow : %d tx_tokens : %d \n\r",
                             subgroup->sg_name, group->atf_num_sg_borrow, subgroup->sg_shadow_tx_tokens);
        /* reset the contribution level */
        subgroup->sg_contribute_id = 0;
        subgroup->sg_contribute_level = 0;
    }

    return 0;
}

/**
 * @brief Compute unassigned tokens
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return 0
 */
uint32_t ieee80211_atf_get_unassigned_tokens(struct ieee80211com *ic)
{
    u_int32_t txtokens_unassigned = 0, airtime_unassigned = 0;

    if (!ic->ic_atf_tput_based) {
        airtime_unassigned = ieee80211_atf_airtime_unassigned(ic);
        txtokens_unassigned = ieee80211_atf_compute_txtokens(ic, airtime_unassigned, ATF_TOKEN_INTVL_MS);
    }
    IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                         "%s, airtime_unassigned : %d tokens : %d \n",
                         __func__, airtime_unassigned, txtokens_unassigned);

    return txtokens_unassigned;
}

/**
 * @brief Assign unassigned tokens to ic
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return 0
 */
int ieee80211_atf_manage_unassigned_tokens(struct ieee80211com *ic)
{
    u_int32_t txtokens_unassigned = 0;

    /* If max client support is enabled & if the total number of clients
     * exceeds the number supported in ATF, do not contribute unalloted tokens.
     * Unalloted tokens will be used by non-atf capable clients
     */
    txtokens_unassigned = ieee80211_atf_get_unassigned_tokens(ic);
    if (ic->ic_atf_maxclient) {
        ic->ic_atf_tokens_unassigned(ic, txtokens_unassigned);
        /* With Maxclient feature enabled, unassigned tokens are used by non-atf clients
           Hence, do not add unassigned tokens to node tokens */
        ic->atf_tokens_unassigned = 0;
    } else {
        /* Add unassigned tokens to the contributable token pool*/
        if (txtokens_unassigned) {
            txtokens_unassigned -= ((ATF_RESERVED_UNALLOTED_TOKEN_PERCENT * txtokens_unassigned) / 100);
        }
        /* Unassigned tokens will be added to node tokens */
        ic->atf_tokens_unassigned = txtokens_unassigned;
    }

    return 0;
}

/**
 * @brief Iterates through group & subgroup list
 *        Computes the contributable tokens in the common pool (across groups)
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return 0
 */
int ieee80211_atf_common_contribute_pool(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    u_int32_t group_noclients_txtokens = 0;

    ic->atf_total_num_clients_borrow = 0;
    ic->atf_total_contributable_tokens = 0;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        if (group->group_sched != IEEE80211_ATF_GROUP_SCHED_STRICT &&
            ic->atfcfg_set.grp_num_cfg) {
            /* If there aren't any clients in the group, add group's airtime
             * to the common contributable pool
             */
            if (!group->atf_num_active_sg) {
                group_noclients_txtokens = ieee80211_atf_compute_txtokens(ic, group->group_airtime, ATF_TOKEN_INTVL_MS);
                ic->atf_total_contributable_tokens += (group_noclients_txtokens -
                                    ((ATF_RESERVERD_TOKEN_PERCENT * group_noclients_txtokens) / 100));
                group->atf_contributabletokens = 0;
            }

            /* Total number of 'subgroups looking to borrow' per radio */
            ic->atf_total_num_clients_borrow += group->atf_num_sg_borrow;

            /* If group sched is set to 'Fair with Upper Bound' & if there are
             * subgroups looking to borrow, do not add contributable tokens in
             * the common contributable pool
             */
            if (!(group->group_sched == IEEE80211_ATF_GROUP_SCHED_FAIR_UB &&
                group->atf_num_sg_borrow)) {
                ic->atf_total_contributable_tokens += group->atf_contributabletokens;
            }

            /* Update number of subgroups looking to borrow in STRICT & FAIR_UB groups */
            if (group->group_sched == IEEE80211_ATF_GROUP_SCHED_STRICT) {
                ic->atf_num_sg_borrow_strict += group->atf_num_sg_borrow;
            } else if (group->group_sched == IEEE80211_ATF_GROUP_SCHED_FAIR_UB) {
                ic->atf_num_sg_borrow_fair_ub += group->atf_num_sg_borrow;
            }
        }

        /* Total number of groups that have subgroups looking to borrow */
        if (group->atf_num_sg_borrow) {
            ic->atf_groups_borrow++;
        }
    }

    return 0;
}

/**
 * @brief Iterates through group & subgroup list
 *        Identifies subgroups looking to borrow & contribute tokens
 *        Computes total tokens available for contribution
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return 0
 */
int ieee80211_atf_fairq_compute_tokens(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;
    u_int32_t sg_unused_tokens = 0;
    u_int32_t wt_unusedtokens_per = 0;
    int32_t sg_index = 0;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            sg_index = 0;
            /*unused tokens of the subgroup in the previous cycle*/
            sg_unused_tokens = ieee80211_atf_get_unused_txtokens(subgroup);

            /* Update the statistics */
            ieee80211_atf_update_stats(ic, group, subgroup, sg_unused_tokens);

            /* log subgroup usage */
            sg_index = ieee80211_atf_log_usage(ic, subgroup, sg_unused_tokens);
            if (sg_index >= 0) {
                wt_unusedtokens_per = ieee80211_atf_compute_weighted_tokens(ic, subgroup, sg_index);
            }

            /* Compute tokens based on user allocation */
            subgroup->sg_shadow_tx_tokens = ieee80211_atf_compute_txtokens(ic, subgroup->sg_atf_units, ATF_TOKEN_INTVL_MS);
            IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                    "%s() - subgroup: %s, atf_units: %d shadow_tx_tokens: %d unused tokens : %d INTVL : %d \n",
                    __func__, subgroup->sg_name, subgroup->sg_atf_units, subgroup->sg_shadow_tx_tokens,
                    sg_unused_tokens, ATF_TOKEN_INTVL_MS);

            subgroup->sg_contributedtokens = 0;
            subgroup->sg_raw_tx_tokens = subgroup->sg_shadow_tx_tokens;
            if (sg_index >= 0) {
                IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                    "%s() - group: %s, Fair-queue enabled \n", __func__, group->group_name);
                ieee80211_atf_compute_tokens_contribute(ic, group, subgroup, wt_unusedtokens_per);
            } else {
                /* Strict policy configured for SSID */
                IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                    "%s() - group: %s, Databased not built \n", __func__, group->group_name);
                subgroup->sg_borrowedtokens = 0;
                subgroup->sg_atfborrow = 0;
            }

            /* Increment subgroup index */
            subgroup->sg_atfindex++;
            if(subgroup->sg_atfindex >= ATF_DATA_LOG_SIZE)
                subgroup->sg_atfindex = 0;
        }
    }

    return 0;
}

/*
 * @brief Distribute tokens when intergroup policy is FAIR
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return 0
 */
int ieee80211_atf_intergroup_fair_distribute(struct ieee80211com *ic)
{
    u_int32_t contrtokens_persg = 0;
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;
    u_int32_t num_clients_fair_borrow = 0;

    printk("%s ic->atf_total_num_clients_borrow %d atf_num_sg_borrow_fair_ub : %d \n",
            __func__, ic->atf_total_num_clients_borrow, ic->atf_num_sg_borrow_fair_ub);

    if (ic->atf_total_num_clients_borrow >= ic->atf_num_sg_borrow_fair_ub) {
        num_clients_fair_borrow = ic->atf_total_num_clients_borrow -
                                ic->atf_num_sg_borrow_fair_ub;
    } else {
        printk("%s check!num_clients_fair_borrow %d < atf_total_num_clients_borrow %d \n",
                __func__, ic->atf_total_num_clients_borrow, ic->atf_num_sg_borrow_fair_ub);
    }

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            /* If there aren't any Clients looking to borrow, distribute unassigned tokens */
            if ((!num_clients_fair_borrow) &&
                IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) {
                if (!group->group_sched) {
                    if (ic->ic_atf_num_active_sg &&
                       !ieee80211_atf_is_default_subgroup(subgroup->sg_name)) {
                        subgroup->sg_borrowedtokens = ic->atf_tokens_unassigned / ic->ic_atf_num_active_sg;
                    } else {
                        subgroup->sg_borrowedtokens = 0;
                    }
                    subgroup->sg_shadow_tx_tokens += subgroup->sg_borrowedtokens;

                    /* No clients looking to borrow; Distribute contributable tokens to all active subgroups equally */
                    contrtokens_persg =  ic->atf_total_contributable_tokens / ic->ic_atf_num_active_sg;
                    subgroup->sg_shadow_tx_tokens += contrtokens_persg;
                    group->atf_contributabletokens = 0;
                    subgroup->sg_contributedtokens = 0;
                }
            } else if (subgroup->sg_atfborrow &&
                      IEEE80211_ATF_IS_SG_ACTIVE(subgroup) &&
                      num_clients_fair_borrow) {
                /* For subgroups looking to borrow:
                 * Distribute any unassigned tokens (if any) equally
                 * Distribute tokens from global contributable pool equally
                 */
                if (!ieee80211_atf_is_default_subgroup(subgroup->sg_name)) {
                    contrtokens_persg = (ic->atf_total_contributable_tokens + ic->atf_tokens_unassigned) /
                                          num_clients_fair_borrow;
                } else {
                    contrtokens_persg = ic->atf_total_contributable_tokens /
                                            num_clients_fair_borrow;
                }
                //Update borrowed tokens for this subgroup.
                subgroup->sg_borrowedtokens = contrtokens_persg;
                subgroup->sg_shadow_tx_tokens += contrtokens_persg;
            }
        }
    }

    return 0;
}

/**
 * @brief Distribute tokens when intergroup policy is FAIR
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return 0
 */
int ieee80211_atf_intergroup_strict_distribute(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;
    u_int32_t contributabletokens_per_group = 0, contributabletokens_persubgroup = 0;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            if (!group->atf_num_sg_borrow) {
                /* No groups looking to borrow, distribute unassigned tokens */
                if ((!group->group_sched) && IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) {
                    if (!ic->atf_groups_borrow) {
                        if (ic->ic_atf_num_active_sg &&
                            !ieee80211_atf_is_default_subgroup(subgroup->sg_name)) {
                            subgroup->sg_borrowedtokens = ic->atf_tokens_unassigned / ic->ic_atf_num_active_sg;
                        } else {
                           subgroup->sg_borrowedtokens = 0;
                        }
                        subgroup->sg_shadow_tx_tokens += subgroup->sg_borrowedtokens;
                    }

                    /* In the group, if there are'nt any clients looking to borrow,
                       distribute contributable tokens to all active subgroups */
                    if (group->atf_num_active_sg) {
                        contributabletokens_persubgroup =  group->atf_contributabletokens / group->atf_num_active_sg;
                    }
                    subgroup->sg_shadow_tx_tokens += contributabletokens_persubgroup;
                    group->atf_contributabletokens -= contributabletokens_persubgroup;
                    printk("%s %d group->atf_contributabletokens : %d \n",__func__, __LINE__, group->atf_contributabletokens);
                    subgroup->sg_contributedtokens = 0;
                }
            } else if (subgroup->sg_atfborrow &&
                        IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) {
                /* For nodes with 'borrow' enabled, allocate additional tokens from contributable token pool */

                /* Distribute any unassigned tokens (if any) equally to groups looking to borrow*/
                if (!ieee80211_atf_is_default_subgroup(subgroup->sg_name)) {
                    contributabletokens_per_group = ic->atf_tokens_unassigned / ic->atf_groups_borrow;
                }

                contributabletokens_persubgroup = (group->atf_contributabletokens + contributabletokens_per_group) /
                                                    group->atf_num_sg_borrow;

                //Update borrowed tokens for this subgroup.
                subgroup->sg_borrowedtokens = contributabletokens_persubgroup;
                IEEE80211_DPRINTF_IC(ic, IEEE80211_VERBOSE_NORMAL, IEEE80211_MSG_ATF,
                    "%s() subgroup : %s atf_num_sg_borrow : %d tot atf_contributabletokens %d \
                    cont per subgroup : %d  tokens : %d --> %d  \n\r", __func__, subgroup->sg_name,
                    group->atf_num_sg_borrow, group->atf_contributabletokens, contributabletokens_persubgroup,
                    subgroup->sg_shadow_tx_tokens, (subgroup->sg_shadow_tx_tokens + contributabletokens_persubgroup));

                subgroup->sg_shadow_tx_tokens += contributabletokens_persubgroup;
                printk("%s subgroup : %s borrowing %d tokens \n",
                        __func__, subgroup->sg_name, contributabletokens_persubgroup);
            }
        }
    }
    return 0;
}

int ieee80211_atf_intergroup_sched_fair_ub(struct ieee80211com *ic, struct group_list *group, struct subgroup_list *subgroup)
{
    u_int32_t contrtokens_persg = 0;

    /* Handle unassigned token distribution ???*/
    if (subgroup->sg_atfborrow &&
        IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) {
        contrtokens_persg = group->atf_contributabletokens/group->atf_num_sg_borrow;
        subgroup->sg_borrowedtokens = contrtokens_persg;
        subgroup->sg_shadow_tx_tokens += contrtokens_persg;
        if (TAILQ_NEXT(subgroup, sg_next) == NULL) {
            group->atf_contributabletokens = 0;
        }
    }

    return 0;
}

int ieee80211_atf_intergroup_sched_strict(struct ieee80211com *ic, struct group_list *group, struct subgroup_list *subgroup)
{
    u_int32_t contrtokens_persg = 0;

    /* Handle unassigned token distribution ???*/
    if (group->atf_contributabletokens) {
        contrtokens_persg = group->atf_contributabletokens/group->num_subgroup;
        subgroup->sg_shadow_tx_tokens += contrtokens_persg;
        subgroup->sg_borrowedtokens = 0;
        subgroup->sg_contributedtokens = 0;
        if (TAILQ_NEXT(subgroup, sg_next) == NULL) {
            group->atf_contributabletokens = 0;
        }
    }

    return 0;
}

int ieee80211_atf_intergroup_sched_fair(struct ieee80211com *ic, struct group_list *group, struct subgroup_list *subgroup)
{
    u_int32_t active_sgs = 0, clients_borrow = 0;
    u_int32_t tot_num_active_sg_strict = 0;
    u_int32_t tot_num_sg_borrow_strict = 0;
    u_int32_t contrtokens_persg = 0;


    /* Total number of subgroups assoicated with groups marked
     * Strict or FAIR_UB sched policy */
    tot_num_sg_borrow_strict = ic->atf_num_sg_borrow_strict +
                                    ic->atf_num_sg_borrow_fair_ub;
    if (ic->atf_total_num_clients_borrow >= tot_num_sg_borrow_strict) {
        clients_borrow = ic->atf_total_num_clients_borrow - tot_num_sg_borrow_strict;
    } else {
        printk(" %s Check!!! num_borrow %d < ic->atf_num_sg_borrow_strict %d +  ic->atf_num_sg_borrow_fair_ub %d \n",
            __func__, ic->atf_total_num_clients_borrow, ic->atf_num_sg_borrow_strict, ic->atf_num_sg_borrow_fair_ub);
    }

    /* if there aren't any clients looking to borrow,
     * Distribute unassigned tokens to all active subgroups equally */
    if (!clients_borrow &&
        IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) {
        tot_num_active_sg_strict = ic->ic_atf_num_active_sg_strict + ic->ic_atf_num_active_sg_fair_ub;
        if (ic->ic_atf_num_active_sg >= tot_num_active_sg_strict) {
            active_sgs = ic->ic_atf_num_active_sg - tot_num_active_sg_strict;
        } else {
            printk(" %s Check!!! ic->ic_atf_num_active_sg  %d < ic->ic_atf_num_active_sg_strict %d +  ic->ic_atf_num_active_sg_fair_ub %d \n",
                    __func__, ic->ic_atf_num_active_sg, ic->ic_atf_num_active_sg_strict, ic->ic_atf_num_active_sg_fair_ub);
        }
        if (!ieee80211_atf_is_default_subgroup(subgroup->sg_name)) {
            subgroup->sg_borrowedtokens = ic->atf_tokens_unassigned / active_sgs;
        } else {
            subgroup->sg_borrowedtokens = 0;
        }
        subgroup->sg_shadow_tx_tokens += subgroup->sg_borrowedtokens;
        contrtokens_persg = ic->atf_total_contributable_tokens / active_sgs;
        subgroup->sg_shadow_tx_tokens += contrtokens_persg;
        subgroup->sg_contributedtokens = 0;
        group->atf_contributabletokens = 0;
    } else if (subgroup->sg_atfborrow &&
            IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) {
        /* For subgroups looking to borrow:
         * Distribute any unassigned tokens (if any) equally
         * Distribute tokens from global contributable pool equally
         */
        if (!ieee80211_atf_is_default_subgroup(subgroup->sg_name)) {
            contrtokens_persg = (ic->atf_total_contributable_tokens + ic->atf_tokens_unassigned) / clients_borrow;
        } else {
            contrtokens_persg = ic->atf_total_contributable_tokens / clients_borrow;
        }
        subgroup->sg_borrowedtokens = contrtokens_persg;
        subgroup->sg_shadow_tx_tokens += contrtokens_persg;
    }

    return 0;
}

void ieee80211_atf_compute_sg_tokens(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            if ((group->group_sched == IEEE80211_ATF_GROUP_SCHED_STRICT)) {
                ieee80211_atf_intergroup_sched_strict(ic, group, subgroup);
            }else if (group->group_sched == IEEE80211_ATF_GROUP_SCHED_FAIR_UB) {
                ieee80211_atf_intergroup_sched_fair_ub(ic, group, subgroup);
            } else if (group->group_sched == IEEE80211_ATF_GROUP_SCHED_FAIR) {
                /* can contribute & borrow */
                ieee80211_atf_intergroup_sched_fair(ic, group, subgroup);
            } else {
                qdf_print("%s unknown group sched policy \n",__func__);
            }
        }
    }
}


/**
 * @brief Distributes tokens to subgroup
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return 0
 */
int ieee80211_atf_distribute_tokens_subgroup(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;
    u_int32_t txtokens_unassigned = 0;

    ic->ic_alloted_tx_tokens = 0;
    ieee80211_atf_compute_sg_tokens(ic);

    /* Distribute tx_tokens for all subgroup */
    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            /* scheduler works on tx_tokens. Assign tx_tokens */
            qdf_spin_lock_bh(&subgroup->atf_sg_lock);
            subgroup->tx_tokens = subgroup->sg_shadow_tx_tokens;
            //ni->ni_atf_stats.tokens_common = ic->ic_txtokens_common;
            ic->ic_alloted_tx_tokens += subgroup->tx_tokens;
            ieee802111_atf_compute_actual_tokens(ic, subgroup);
            qdf_spin_unlock_bh(&subgroup->atf_sg_lock);
        }
    }

    if (ic->ic_atf_maxclient) {
        txtokens_unassigned = ieee80211_atf_get_unassigned_tokens(ic);
        ic->ic_alloted_tx_tokens += txtokens_unassigned;
        ic->ic_txtokens_common = txtokens_unassigned;
    } else {
        ic->ic_txtokens_common = 0;
    }

    ic->ic_shadow_alloted_tx_tokens = ic->ic_alloted_tx_tokens;

    return 0;
}

/**
 * @brief Iterates through the node table.
 *        Subgroups with atleast a node pointing to it
 *        will be marked active
 *
 * @param [in] arg  the handle to the radio
               ni   pointer to the node table
 *
 * @return none
 */
void ieee80211_node_iter_mark_active_sg(void *arg, struct ieee80211_node *ni)
{
    int8_t ac_id = 0;
    struct subgroup_list *subgroup = NULL;

    if (ni->ni_associd) {
        for (ac_id = 0; ac_id < WME_NUM_AC; ac_id++) {
            subgroup = ni->ni_ac_list_ptr[ac_id];
            if (subgroup) {
                IEEE80211_ATF_SET_SG_ACTIVE(subgroup);
            }
        }
    }
}

/**
 * @brief Find active subgroups
 *        subgroup with atleast one node pointing to it is marked active
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return 0
 */
void ieee80211_atf_find_active_subgroups(struct ieee80211com *ic)
{
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    /* iterate over node table and mark active subgroups */
    ieee80211_iterate_node(ic, ieee80211_node_iter_mark_active_sg, ic);

    /* Find the number of active subgroups within each group */
    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            if (IEEE80211_ATF_IS_SG_ACTIVE(subgroup)) {
                group->atf_num_active_sg++;
            }
        }
        ic->ic_atf_num_active_sg += group->atf_num_active_sg;

        /* Find number of subgroups in STRICT & FAIR_UB Active groups */
        if (group->group_sched == IEEE80211_ATF_GROUP_SCHED_STRICT) {
            ic->ic_atf_num_active_sg_strict += group->atf_num_active_sg;
        } else if (group->group_sched == IEEE80211_ATF_GROUP_SCHED_FAIR_UB) {
            ic->ic_atf_num_active_sg_fair_ub += group->atf_num_active_sg;
        }
    }
}

void ieee80211_node_iter_unblock_nodes(void *arg, struct ieee80211_node *ni)
{
    struct ieee80211com *ic = (struct ieee80211com *)arg;

    ieee80211_atf_node_unblock(ic, ni);
}

/**
 * @brief Implements ATF Fair-queue scheduling algorithm
 *
 * @param [in] ic  pointer to ic radio structure
 *
 * @return 0
 */
int ieee80211_atf_fairq_algo(struct ieee80211com *ic)
{
    /* initialize group & ic variables */
    ieee80211_atf_init_ic_group_var(ic);

    /* Find subgroups that are active */
    ieee80211_atf_find_active_subgroups(ic);

    /* Compute tokens for contribution */
    ieee80211_atf_fairq_compute_tokens(ic);

    /* Compute tokens for contribution across groups */
    ieee80211_atf_common_contribute_pool(ic);

    /* get unassigned tokens */
    ieee80211_atf_manage_unassigned_tokens(ic);

    /* Distribute tokens to subgroup */
    ieee80211_atf_distribute_tokens_subgroup(ic);

    /* Iterate Node table & unblock all nodes at every timer tick */
    ieee80211_iterate_node(ic, ieee80211_node_iter_unblock_nodes, ic);

    /* Updated TIDstate stat */
    ieee80211_atf_update_stats_tidstate(ic);

    /* Update stats pointer */
    ieee80211_atf_stats_history(ic);

    return 0;
}

/**
 * @brief Set ssid sched policy in the group
 *
 * @param [in] vap Pointer to VAP struct
 *        [in] sched_policy Sched policy to be set for the vap
 *
 * @return None
 */
void ieee80211_atf_group_vap_sched(struct ieee80211vap *vap, u_int32_t sched_policy)
{
    struct ieee80211com *ic = vap->iv_ic;
    u_int32_t groupindex = 0;
    struct group_list *group = NULL;

    /* Find corresponding group */
    if (ieee80211_atf_group_exist(ic, vap->iv_bss->ni_essid, &groupindex)) {
        group = ic->atfcfg_set.atfgroup[groupindex].grplist_entry;
        group->group_sched = sched_policy;
        ic->atfcfg_set.atfgroup[groupindex].grp_sched_policy = sched_policy;
        qdf_print("%s group : %s found. Index : %d sched policy set to %d\n", __func__, group->group_name, groupindex, group->group_sched);
    } else {
        qdf_print("%s group could not be found for vap :  %s \n", __func__, vap->iv_bss->ni_essid);
    }

}

/**
 * @brief Create Default subgroup & link to default group
 * @param [in] ic  the handle to the radio
 *
 * @return 0 on success
 *         -1 on error
 */
struct subgroup_list *ieee80211_atf_create_default_subgroup(struct ieee80211com *ic, struct group_list *group)
{
    struct subgroup_list *subgroup = NULL;

    if (!group) {
        qdf_print("%s group is NULL \n",__func__);
        return NULL;
    }

    /* Creating Default sub-group */
    TAILQ_INIT(&group->atf_subgroups);
    subgroup = (struct subgroup_list *)OS_MALLOC(ic->ic_osdev, sizeof(struct subgroup_list), GFP_KERNEL);
    if (strlcpy(subgroup->sg_name, DEFAULT_SUBGROUPNAME, sizeof(DEFAULT_SUBGROUPNAME) + 1) >= sizeof(DEFAULT_SUBGROUPNAME)) {
        qdf_print("source too long\n");
        return NULL;
    }
    TAILQ_INSERT_TAIL(&group->atf_subgroups, subgroup, sg_next);
    subgroup->sg_id  = group->subgroup_count;

    return subgroup;
}

/**
 * @brief Create Default group
 * @param [in] ic  the handle to the radio
 *
 * @return 0 on success
 *         -1 on error
 */
struct group_list *ieee80211_atf_create_default_group(struct ieee80211com *ic)
{
    struct group_list *group = NULL;

    TAILQ_INIT(&ic->ic_atfgroups);

    group = (struct group_list *)OS_MALLOC(ic->ic_osdev, sizeof(struct group_list), GFP_KERNEL);
    if (strlcpy(group->group_name, DEFAULT_GROUPNAME, sizeof(DEFAULT_GROUPNAME) + 1) >= sizeof(DEFAULT_GROUPNAME)) {
        qdf_print("source too long\n");
        return NULL;
    }

    group->atf_num_sg_borrow = 0;
    group->atf_num_active_sg = 0;
    group->atf_contributabletokens = 0;
    group->subgroup_count = 0;
    group->shadow_atf_contributabletokens = 0;
    TAILQ_INSERT_TAIL(&ic->ic_atfgroups, group, group_next);

    return group;
}

/**
 * @brief Return the unassigned airtime
 *        The airtime assigned to default subgroup is unassigned airtime
 *        convert unalloted atf_units to tokens and add to the
 *        contributable token pool
 * @param [in] ic  the handle to the radio
 *
 * @return unalloted tokens
 */
u_int32_t ieee80211_atf_airtime_unassigned(struct ieee80211com *ic)
{
    u_int32_t airtime_unassigned = 0;
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    group = TAILQ_FIRST(&ic->ic_atfgroups);
    if (group) {
        subgroup = TAILQ_FIRST(&group->atf_subgroups);
        if (subgroup) {
            airtime_unassigned = subgroup->sg_atf_units;
        }
    }
    return airtime_unassigned;
}

int32_t ieee80211_atf_set(struct ieee80211vap *vap, u_int8_t enable)
{

    struct ieee80211com *ic = vap->iv_ic;
    int32_t retv = EOK;

    if(!ic->ic_is_mode_offload(ic))
    {
        /* If atf_maxclient, set max client limit to IEEE80211_512_AID */
        if( (ic->ic_atf_maxclient) &&  (ic->ic_num_clients != IEEE80211_128_AID) )
        {
            retv = IEEE80211_128_AID;
        }

        /* if atf_maxclient is not enabled, set max client limit to IEEE80211_ATF_AID_DEF */
        if( (!ic->ic_atf_maxclient) &&  (ic->ic_num_clients != IEEE80211_ATF_AID_DEF) )
        {
            retv = IEEE80211_ATF_AID_DEF;
        }

        if (ic->ic_atf_tput_tbl_num && !ic->ic_atf_maxclient) {
            ic->ic_atf_set_enable_disable(ic, ATF_TPUT_BASED);
            if (!ic->ic_atf_tput_based)
                retv = IEEE80211_ATF_AID_DEF;
        } else {
            ic->ic_atf_set_enable_disable(ic, ATF_AIRTIME_BASED);
        }
    }

    ic->atf_commit = !!enable;
    if (ic->ic_atf_tput_tbl_num && !ic->ic_atf_maxclient) {
        ic->ic_atf_tput_based = 1;
    } else if (ic->ic_atf_tput_tbl_num) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "can't enable tput based atf as maxclients is enabled\n");
    }

    if((ic->atf_fmcap)&&(ic->atf_mode))
    {
        spin_lock(&ic->atf_lock);
        if (ic->atf_tasksched == 0)
        {
            ic->atf_tasksched = 1;
            ic->atf_vap_handler = vap;
            OS_SET_TIMER(&ic->atfcfg_timer, IEEE80211_ATF_WAIT*1000);
        }
        spin_unlock(&ic->atf_lock);

        if(!ic->ic_is_mode_offload(ic))
            OS_SET_TIMER(&ic->atf_tokenalloc_timer, ATF_TOKEN_INTVL_MS);

        /* send wmi command to target */
        if ( ic->ic_is_mode_offload(ic) && ic->atf_vap_handler ) {
            ic->ic_vap_set_param(ic->atf_vap_handler, IEEE80211_ATF_DYNAMIC_ENABLE, 1);
        }
    } else {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "Either firmware capability or host ATF configuration not support!!\n");
    }
    return retv;
}

static void ieee80211_node_atf_node_resume(void *arg, struct ieee80211_node *ni)
{
    struct ieee80211com *ic = (struct ieee80211com *) arg;
    if(ni)
    {
        ic->ic_atf_node_resume(ic, ni);
    }
}

int32_t ieee80211_atf_clear(struct ieee80211vap *vap, u_int8_t val)
{
    struct ieee80211com *ic = vap->iv_ic;
    int32_t retv = EOK;

    if(!ic->ic_is_mode_offload(ic))
    {
        /* When ATF is disabled, set ic_num_clients to default value */
        if ( ic->ic_num_clients != IEEE80211_128_AID) {
            retv = IEEE80211_128_AID;
        }

        /* Before disabling ATF, resume any paused nodes */
        ieee80211_iterate_node(ic,ieee80211_node_atf_node_resume,ic);
        ic->ic_atf_set_enable_disable(ic, ATF_DISABLED);
    } else {
        /* Send WMI command to target */
        if(ic->atf_vap_handler) {
            ic->ic_vap_set_param(ic->atf_vap_handler, IEEE80211_ATF_DYNAMIC_ENABLE, 0);
        }
    }

    ic->atf_commit = !!val;
    if (ic->ic_atf_tput_based && ic->ic_atf_maxclient) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "tput based atf as well as maxclients was enabled\n");
    }
    ic->ic_atf_tput_based = 0;

    spin_lock(&ic->atf_lock);
    if (ic->atf_tasksched == 1)
    {
        ic->atf_tasksched = 0;
        ic->atf_vap_handler = NULL;
    }
    spin_unlock(&ic->atf_lock);

    return retv;
}

int ieee80211_atf_used_all_tokens(struct ieee80211com *ic, struct ieee80211_node *ni)
{
    struct subgroup_list *subgroup = NULL;
    int32_t alltokens_used = 0;
    int8_t ac = 0;

    if (ic->atf_commit) {
        if (ic->ic_atf_sched & IEEE80211_ATF_SCHED_STRICT) {
            alltokens_used = ic->ic_atf_tokens_used(ic, ni);
        } else {
            /* Check if subgroups associated to any AC has borrowed tokens */
            for (ac = 0; ac < WME_NUM_AC; ac++) {
                subgroup = NULL;
                subgroup = ni->ni_ac_list_ptr[ac];
                if (subgroup && subgroup->sg_atfborrow) {
                    alltokens_used = 1;
                    break;
                }
            }
        }
    }

    return alltokens_used;
}

int ieee80211_atf_get_debug_nodestate(struct ieee80211com *ic, struct ieee80211_node *ni, struct atf_nodestate *atfnodestate)
{
    struct subgroup_list *subgroup = NULL;
    int i = 0, ret = 0;

    if(ni && ic->ic_atf_debug_nodestate)
    {
        ret = ic->ic_atf_debug_nodestate(ic, ni, atfnodestate);
        for (i = 0; i < WME_NUM_AC; i++) {
            subgroup = NULL;
            subgroup = ni->ni_ac_list_ptr[i];
            if (subgroup) {
                memcpy(&atfnodestate->subgroup_name[i][0], &subgroup->sg_name, IEEE80211_NWID_LEN);
            }
        }
    } else
        return -EINVAL;

    return 0;
}

int ieee80211_atf_get_debug_dump(struct ieee80211com *ic, struct subgroup_list *subgroup,
                                 void **buf, u_int32_t *buf_sz, u_int32_t *id)
{
    struct atf_stats *ptr = NULL;
    int size;

    if(!subgroup) {
        qdf_print("subgroup is NULL. return \n");
        return -1;
    }

    qdf_spin_lock_bh(&subgroup->atf_sg_lock);
    size = subgroup->sg_atf_debug_mask + 1;
    qdf_spin_unlock_bh(&subgroup->atf_sg_lock);

    if (size == 1)
        ptr = NULL;
    else
        ptr = OS_MALLOC(ic->ic_osdev, size * sizeof(struct atf_stats), GFP_KERNEL);

    if (ptr) {
        qdf_spin_lock_bh(&subgroup->atf_sg_lock);
        *buf = subgroup->sg_atf_debug;
        *buf_sz = size * sizeof(struct atf_stats);
        *id = subgroup->sg_atf_debug_id;

        subgroup->sg_atf_debug = ptr;
        subgroup->sg_atf_debug_id = 0;
        qdf_spin_unlock_bh(&subgroup->atf_sg_lock);
    }

    return 0;
}

int ieee80211_atf_set_debug_size(struct ieee80211com *ic, int size)
{
    struct atf_stats *ptr = NULL;
    struct group_list *group = NULL;
    struct subgroup_list *subgroup = NULL;

    if (size <= 0) {
        qdf_print("%s size cannot be %d \n",__func__, size);
        return -1;
    }

    if (size <= 16)
        size = 16;
    else if (size <= 32)
        size = 32;
    else if (size <= 64)
        size = 64;
    else if (size <= 128)
        size = 128;
    else if (size <= 256)
        size = 256;
    else if (size <= 512)
        size = 512;
    else
        size = 1024;

    TAILQ_FOREACH(group, &ic->ic_atfgroups, group_next) {
        TAILQ_FOREACH(subgroup, &group->atf_subgroups, sg_next) {
            ptr = NULL;
            qdf_spin_lock_bh(&subgroup->atf_sg_lock);
            if (subgroup->sg_atf_debug) {
                ptr = subgroup->sg_atf_debug;
                subgroup->sg_atf_debug = NULL;
            }
            subgroup->sg_atf_debug_mask = 0;
            subgroup->sg_atf_debug_id = 0;
            qdf_spin_unlock_bh(&subgroup->atf_sg_lock);

            if (ptr) {
                /* Free old history */
                OS_FREE(ptr);
                ptr = NULL;
            }
            /* Allocate new history */
            ptr = OS_MALLOC(ic->ic_osdev, size * sizeof(struct atf_stats), GFP_KERNEL);
            if (ptr) {
                qdf_spin_lock_bh(&subgroup->atf_sg_lock);
                subgroup->sg_atf_debug = ptr;
                subgroup->sg_atf_debug_mask = size - 1;
                subgroup->sg_atf_debug_id = 0;
                qdf_spin_unlock_bh(&subgroup->atf_sg_lock);
            }
        }
    }

    return 0;
}

static void
atf_notify_aitime(struct ieee80211vap *vap, u_int8_t macaddr[], int config)
{
    osif_dev *osifp = NULL;
    struct net_device *dev = NULL;
    union iwreq_data wreq;
    struct event_data_atf_config ev;

    if (vap) {
        osifp = (osif_dev *)vap->iv_ifp;
        if (osifp) {
            dev = osifp->netdev;
        }
    }

    if (dev) {
        ev.config = config;
        memcpy(ev.macaddr, macaddr, IEEE80211_ADDR_LEN);
        memset(&wreq, 0, sizeof(wreq));
        wreq.data.flags = IEEE80211_EV_ATF_CONFIG;
        wreq.data.length = sizeof(struct event_data_atf_config);
        WIRELESS_SEND_EVENT(dev, IWEVCUSTOM, &wreq, (void *)&ev);
    }
}

void ieee80211_atf_distribute_airtime(struct ieee80211com *ic)
{
    struct ieee80211_node_table *nt = &ic->ic_sta;
    struct ieee80211_node *ni, *next;
    struct ieee80211_node *rev_ni, *rev_next;
    rwlock_state_t lock_state;
    u_int32_t configured_airtime, airtime, associated_sta, airtime_limit, airtime_resv, configured_sta;
    struct subgroup_list *subgroup = NULL;
    struct group_list *group = NULL;

    OS_BEACON_DECLARE_AND_RESET_VAR(flags);
    OS_BEACON_READ_LOCK(&nt->nt_nodelock, &lock_state, flags);

    airtime_resv = ic->ic_atf_resv_airtime * (WMI_ATF_DENOMINATION / 100);

    /*
     * For each station having a throughput requirement, we find
     * out the airtime needed. If the new airtime needed is less than
     * or equal to what was previously needed, we configure the airtime
     * now itself. Else we record how much extra airtime is needed. Also,
     * we keep track of the total allotted airtime.
     */
    configured_airtime = 0;
    ni = next = NULL;
    TAILQ_FOREACH_SAFE(ni, &nt->nt_node, ni_list, next) {
        ieee80211_ref_node(ni);
        if (ni->ni_atf_tput) {
            ni->ni_atf_airtime_new = ic->ic_atf_airtime_estimate(ic,
                                                        ni, ni->ni_atf_tput, &ni->ni_throughput);
            ni->ni_atf_airtime_new *= WMI_ATF_DENOMINATION / 100;
            if (ic->ic_atf_airtime_override &&
                ic->ic_atf_airtime_override < (WMI_ATF_DENOMINATION - airtime_resv)) {
                ni->ni_atf_airtime_new = ic->ic_atf_airtime_override;
            }
            //printk("-- %u airtime required %u, max throughput %u --\n",
                     //ni->ni_associd, ni->ni_atf_airtime_new, ni->ni_throughput);
            airtime_limit = ni->ni_atf_airtime_cap * (WMI_ATF_DENOMINATION / 100);
            if (airtime_limit && (ni->ni_atf_airtime_new > airtime_limit)) {
                ni->ni_atf_airtime_new = airtime_limit;
                //printk("-- %u airtime required after capping %u --\n",
                         //ni->ni_associd, ni->ni_atf_airtime_new);
            }
            ni->ni_atf_airtime_subseq = 0;
            if (ni->ni_atf_airtime_new <= ni->ni_atf_airtime) {
                ni->ni_atf_airtime = ni->ni_atf_airtime_new;
                ni->ni_atf_airtime_more = 0;
            } else {
                ni->ni_atf_airtime_more = ni->ni_atf_airtime_new - ni->ni_atf_airtime;
            }
            configured_airtime += ni->ni_atf_airtime;
            //printk("-- %u airtime %u/%u/%u %u --\n",
                     //ni->ni_associd, ni->ni_atf_airtime, ni->ni_atf_airtime_new,
                     //ni->ni_atf_airtime_more, configured_airtime);
        } else {
            ni->ni_throughput = 0;
        }
        ieee80211_free_node(ni);
    }

    if (configured_airtime > (WMI_ATF_DENOMINATION - airtime_resv)) {
        /* Something wrong, as we only gave lesser airtimes compared to prev cycle */
        rev_ni = rev_next = NULL;
        TAILQ_FOREACH_REVERSE_SAFE(rev_ni, &nt->nt_node, node_list, ni_list, rev_next) {
            ieee80211_ref_node(rev_ni);
            if (configured_airtime <= (WMI_ATF_DENOMINATION - airtime_resv)) {
                ieee80211_free_node(rev_ni);
                break;
            }
            if (rev_ni->ni_atf_tput) {
                configured_airtime -= rev_ni->ni_atf_airtime;
                rev_ni->ni_atf_airtime = 0;
                rev_ni->ni_atf_airtime_more = rev_ni->ni_atf_airtime_new;
                //printk("-- %u deconfig as we gave more, airtime %u/%u/%u %u --\n",
                         //rev_ni->ni_associd, rev_ni->ni_atf_airtime, rev_ni->ni_atf_airtime_new,
                         // rev_ni->ni_atf_airtime_more, configured_airtime);
            }
            ieee80211_free_node(rev_ni);
        }
    }

    /*
     * For each station having a throughput requirement, we find out the
     * sum of airtimes allotted to stations newer than that station.
     */
    airtime = 0;
    rev_ni = rev_next = NULL;
    TAILQ_FOREACH_REVERSE_SAFE(rev_ni, &nt->nt_node, node_list, ni_list, rev_next) {
        ieee80211_ref_node(rev_ni);
        if (rev_ni->ni_atf_tput) {
            rev_ni->ni_atf_airtime_subseq = airtime;
            airtime += rev_ni->ni_atf_airtime;
            //printk("-- %u subsequent airtime %u --\n",
                     //rev_ni->ni_associd, rev_ni->ni_atf_airtime_subseq);
        }
        ieee80211_free_node(rev_ni);
    }

    /*
     * Starting with oldest station, go one by one and check which all have
     * extra airtime requirement. Either we give the extra airtime needed
     * or deconfigure the station.
     */
    airtime = 0;
    associated_sta = 0;
    configured_sta = 0;
    ni = next = NULL;
    TAILQ_FOREACH_SAFE(ni, &nt->nt_node, ni_list, next) {
        ieee80211_ref_node(ni);
        if (ni->ni_atf_airtime_more) {
            //printk("-- %u adjusted subsequent airtime %u --\n",
                     //ni->ni_associd, ni->ni_atf_airtime_subseq);
            if (ni->ni_atf_airtime_more <
                (WMI_ATF_DENOMINATION - configured_airtime - airtime_resv)) {
                /*
                 * There is enough unconfigured airtime to meet this extra requirement.
                 * So, we configure this station with it's new requirement.
                 */
                ni->ni_atf_airtime = ni->ni_atf_airtime_new;
                configured_airtime += ni->ni_atf_airtime_more;
                ni->ni_atf_airtime_more = 0;
                //printk("-- %u we had unconfigured airtime, airtime %u/%u/%u %u --\n",
                         //ni->ni_associd, ni->ni_atf_airtime, ni->ni_atf_airtime_new,
                         //ni->ni_atf_airtime_more, configured_airtime);
            } else {
                /* Adjust subsequent configured airtime for newer deconfigured stations */
                ni->ni_atf_airtime_subseq -=
                           airtime > ni->ni_atf_airtime_subseq ? ni->ni_atf_airtime_subseq : airtime;

                if (ni->ni_atf_airtime_subseq < ni->ni_atf_airtime_more) {
                    /*
                     * There isn't enough unconfigured airtime but at the same time
                     * deconfiguring newer stations won't help either. So, we just deconfigure
                     * the current station.
                     */
                    configured_airtime -= ni->ni_atf_airtime;
                    ni->ni_atf_airtime = 0;
                    ni->ni_atf_airtime_more = ni->ni_atf_airtime_new;
                    //printk("-- %u we don't have enough subsequent airtime, airtime %u/%u/%u %u --\n",
                             //ni->ni_associd, ni->ni_atf_airtime, ni->ni_atf_airtime_new,
                             //ni->ni_atf_airtime_more, configured_airtime);
                } else {
                    /*
                     * There isn't enough unconfigured airtime but we can deconfigure some
                     * newer stations to meet this station's extra requirement. So, we go from
                     * the newest station to the current station, deconfiguring newer stations
                     * one by one, till the current station's extra requirement can be met.
                     */

                    rev_ni = rev_next = NULL;
                    TAILQ_FOREACH_REVERSE_SAFE(rev_ni, &nt->nt_node, node_list, ni_list, rev_next) {
                        ieee80211_ref_node(rev_ni);
                        if (ni->ni_atf_airtime_more <
                            (WMI_ATF_DENOMINATION - configured_airtime - airtime_resv)) {
                            /* If we have enough airtime, configure the current station */
                            ni->ni_atf_airtime = ni->ni_atf_airtime_new;
                            configured_airtime += ni->ni_atf_airtime_more;
                            ni->ni_atf_airtime_more = 0;
                            //printk("-- %u we got enough from subsequent airtime(s), airtime %u/%u/%u %u --\n",
                                     //ni->ni_associd, ni->ni_atf_airtime, ni->ni_atf_airtime_new,
                                     //ni->ni_atf_airtime_more, configured_airtime);
                            ieee80211_free_node(rev_ni);
                            break;
                        }

                        if (rev_ni == ni) {
                            /* Something is wrong, we should have got enough airtime by now */
                            configured_airtime -= ni->ni_atf_airtime;
                            ni->ni_atf_airtime = 0;
                            ni->ni_atf_airtime_more = ni->ni_atf_airtime_new;
                            //printk("-- %u we didn't get enough subsequent airtime, airtime %u/%u/%u %u --\n",
                                     //ni->ni_associd, ni->ni_atf_airtime, ni->ni_atf_airtime_new,
                                     //ni->ni_atf_airtime_more, configured_airtime);
                            ieee80211_free_node(rev_ni);
                            break;
                        }

                        /* Try to deconfigure a newer station */
                        if (rev_ni->ni_atf_tput) {
                            airtime += rev_ni->ni_atf_airtime;
                            configured_airtime -= rev_ni->ni_atf_airtime;
                            rev_ni->ni_atf_airtime = 0;
                            rev_ni->ni_atf_airtime_more = rev_ni->ni_atf_airtime_new;
                            //printk("-- %u deconfig this, airtime %u/%u/%u %u --\n",
                                     //rev_ni->ni_associd, rev_ni->ni_atf_airtime, rev_ni->ni_atf_airtime_new,
                                     //rev_ni->ni_atf_airtime_more, configured_airtime);
                        }
                        ieee80211_free_node(rev_ni);
                    }
                }
            }
        }
        if (ni->ni_associd) {
            associated_sta++;
            if (ni->ni_atf_airtime)
                configured_sta++;
        }
        ieee80211_free_node(ni);
    }
    //printk("-- associated sta's %u and configured sta's %u --\n",
             //associated_sta, configured_sta);

    ni = next = NULL;
    TAILQ_FOREACH_SAFE(ni, &nt->nt_node, ni_list, next) {
        if (ni == ni->ni_bss_node) {
            continue;
        }
        ieee80211_ref_node(ni);

        if (ni->ni_atf_tput && ni->ni_atf_airtime) {
            group = ni->ni_atf_group;
            subgroup = ni->ni_ac_list_ptr[0];
            if (!group || !subgroup) {
                ieee80211_free_node(ni);
                break;
            }
            /* if configured client is not pointing to peer subgroup, modify */
            if (subgroup->sg_type != ATF_CFGTYPE_PEER) {
                group = TAILQ_FIRST(&ic->ic_atfgroups);
                atf_assign_subgroup(ni, group, 0, ATF_CFGTYPE_PEER);
                subgroup = ni->ni_ac_list_ptr[0];
            }
            if (subgroup) {
                subgroup->sg_atf_units =  ni->ni_atf_airtime;
            }
            if (!ni->ni_atf_airtime_configured) {
                atf_notify_aitime(ni->ni_vap, ni->ni_macaddr, 1);
            }
            ni->ni_atf_airtime_configured = 1;
        } else if (associated_sta > configured_sta) {
            group = ni->ni_atf_group;
            subgroup = ni->ni_ac_list_ptr[0];
            if (!group || !subgroup) {
                ieee80211_free_node(ni);
                break;
            }
            /* Point unconfigured clients to default subgroup */
            if (!ieee80211_atf_is_default_subgroup(subgroup->sg_name) ||
                !ieee80211_atf_is_default_group(group->group_name)) {
                group = TAILQ_FIRST(&ic->ic_atfgroups);
                ni->ni_atf_group = group;
                atf_assign_subgroup(ni, group, 0, ATF_CFGTYPE_SSID);
                subgroup = ni->ni_ac_list_ptr[0];
            }
            if (subgroup) {
                subgroup->sg_atf_units = (WMI_ATF_DENOMINATION - configured_airtime);
            }
            if (ni->ni_atf_airtime_configured) {
                atf_notify_aitime(ni->ni_vap, ni->ni_macaddr, 0);
            }
            ni->ni_atf_airtime_configured = 0;
        } else {
            group = ni->ni_atf_group;
            subgroup = ni->ni_ac_list_ptr[0];
            if (!group || !subgroup) {
                ieee80211_free_node(ni);
                break;
            }
            /* Point unconfigured clients to default subgroup */
            if (!ieee80211_atf_is_default_subgroup(subgroup->sg_name) ||
                !ieee80211_atf_is_default_group(group->group_name)) {
                group = TAILQ_FIRST(&ic->ic_atfgroups);
                ni->ni_atf_group = group;
                atf_assign_subgroup(ni, group, 0, ATF_CFGTYPE_SSID);
                subgroup = ni->ni_ac_list_ptr[0];
            }
            if (subgroup) {
                subgroup->sg_atf_units = 0;
            }
            if (ni->ni_atf_airtime_configured) {
                atf_notify_aitime(ni->ni_vap, ni->ni_macaddr, 0);
            }
            ni->ni_atf_airtime_configured = 0;
        }
        //printk("Result :: associd:%u max_throughput:%u throughput:%u max_airtime:%u "
                 //"airtime:%u atf_units:%u configured_airtime:%u reserved_airtime:%u\n",
                 //ni->ni_associd, ni->ni_throughput, ni->ni_atf_tput, ni->ni_atf_airtime_cap,
                 //ni->ni_atf_airtime, ni->atf_units, configured_airtime, airtime_resv);
        ieee80211_free_node(ni);
    }

    OS_BEACON_READ_UNLOCK(&nt->nt_nodelock, &lock_state, flags);
}

#endif

