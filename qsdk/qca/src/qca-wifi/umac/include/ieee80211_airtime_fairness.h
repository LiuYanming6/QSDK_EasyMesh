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
#if QCA_AIRTIME_FAIRNESS
#ifndef _UMAC_AIRTIME_FAIRNESS_PRIV__
#define _UMAC_AIRTIME_FAIRNESS_PRIV__

#define ATF_TOKEN_INTVL_MS      200 /* 200 msec */
#define WMI_ATF_DENOMINATION    1000
#define ATF_CFG_GLOBAL_INDEX    0

#define DEFAULT_GROUPNAME   "QC-DeFaUlT-AtFgRoUp\0"/* SSID group name created & maintained in the driver by default */
#define DEFAULT_SUBGROUPNAME   "QC-DeFaUlT-AtFsUbGrOuP\0"/* Sub-group name created & maintained in the driver by default */
#define ATF_DATA_LOG_SIZE                       3  /* Minimum Data log (history) required for ATF Fairqueuing algo */
#define ATF_UNUSEDTOKENS_CONTRIBUTE_THRESHOLD   45 /* Any node with ununsed token percent (avg) greater than this value can contribute */
#define ATF_RESERVERD_TOKEN_PERCENT             1  /* Minimum token (percent) reserved for a node even when it is idle */
#define ATF_RESERVED_UNALLOTED_TOKEN_PERCENT    5 /* Minimum unalloted token (percent) reserved */
#define ATF_MAX_CONTRIBUTE_LEVEL                4   /* Max Contribution Level */

struct ieee80211com;
struct ieee80211vap;

int32_t ieee80211_atf_set(struct ieee80211vap *vap, u_int8_t enable);
int32_t ieee80211_atf_clear(struct ieee80211vap *vap, u_int8_t enable);
int ieee80211_atf_used_all_tokens(struct ieee80211com *ic, struct ieee80211_node *ni);
int ieee80211_atf_get_debug_dump(struct ieee80211com *ic, struct subgroup_list *subgroup,
                                 void **buf, u_int32_t *buf_sz, u_int32_t *id);
int ieee80211_atf_set_debug_size(struct ieee80211com *ic, int size);
void ieee80211_node_iter_dist_txtokens_strictq(void *arg, struct ieee80211_node *ni);
u_int32_t ieee80211_atf_compute_txtokens(struct ieee80211com *ic, u_int32_t atf_units, u_int32_t token_interval_ms);
u_int32_t ieee80211_atf_airtime_unassigned(struct ieee80211com *ic);
void ieee80211_atf_distribute_airtime(struct ieee80211com *ic);
int ieee80211_atf_get_debug_nodestate(struct ieee80211com *ic, struct ieee80211_node *ni, struct atf_nodestate *nodestate);
int ieee80211_atf_strictq_algo(struct ieee80211com *ic);
int assign_airtime_subgroup(struct ieee80211com *ic);
int node_atf_capable_indicate(struct ieee80211com *ic);
int ieee80211_atf_subgroup_exist(struct ieee80211com *ic, struct group_list *group,
                                      void *param, struct subgroup_list **subgroup, int8_t cfg_type);
int ieee80211_atf_del_subgroup(struct ieee80211com *ic);
int ieee80211_atf_del_group(struct ieee80211com *ic);
void dump_atf_group_subgroup(struct ieee80211com *ic);
int ieee80211_atf_mark_del_all_sg(struct ieee80211com *ic, struct group_list *group);
int ieee80211_atf_fairq_algo(struct ieee80211com *ic);
void ieee80211_atf_group_vap_sched(struct ieee80211vap *vap, u_int32_t val);
int ieee80211_atf_group_exist(struct ieee80211com *ic, u_int8_t *essid, int *groupindex);
struct group_list *ieee80211_atf_create_default_group(struct ieee80211com *ic);
struct subgroup_list  *ieee80211_atf_create_default_subgroup(struct ieee80211com *ic, struct group_list *group);
void ieee80211_atf_reset_airtime_sg_all(struct ieee80211com *ic);
int8_t ieee80211_atf_is_default_group(u_int8_t *grpname);
int8_t ieee80211_atf_is_default_subgroup(u_int8_t *subgrpname);
void ieee80211_atf_update_stats(struct ieee80211com *ic, struct group_list *group,
                       struct subgroup_list *subgroup, u_int32_t sg_unusedtokens);
void ieee80211_atf_update_stats_tidstate(struct ieee80211com *ic);
void ieee80211_node_iter_unblock_nodes(void *arg, struct ieee80211_node *ni);
int ieee80211_atf_manage_unassigned_tokens(struct ieee80211com *ic);
void ieee80211_atf_stats_history(struct ieee80211com *ic);
int ieee80211_atf_add_ic_group(struct ieee80211com *ic, char *group_name);
#endif
#endif
