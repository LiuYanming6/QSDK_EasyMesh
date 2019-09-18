#ifndef	__OL_IF_DFS_H__
#define	__OL_IF_DFS_H__

#if ATH_SUPPORT_DFS
extern	void ol_if_dfs_setup(struct ieee80211com *ic);
extern	void ol_if_dfs_teardown(struct ieee80211com *ic);
extern	void ol_if_dfs_configure(struct ieee80211com *ic);
int ol_ath_wmi_dfs_status_check_event_handler(ol_scn_t sc,
                    u_int8_t *data, u_int16_t datalen);
void dfs_action_on_fw_spoof_test_status(struct ieee80211com *ic, int dfs_status_check);
#endif

#endif	/* __OL_IF_DFS_H__ */
