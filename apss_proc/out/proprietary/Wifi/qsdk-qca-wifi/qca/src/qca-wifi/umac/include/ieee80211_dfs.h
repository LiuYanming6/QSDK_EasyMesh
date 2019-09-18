/*-
 * Copyright (c) 2007-2008 Sam Leffler, Errno Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/net80211/ieee80211_dfs.h,v 1.2 2008/12/15 01:26:33 sam Exp $
 */
#ifndef _NET80211_IEEE80211_DFS_H_
#define _NET80211_IEEE80211_DFS_H_

/*
 * 802.11h/DFS definitions.
 */

#if ATH_SUPPORT_ZERO_CAC_DFS
struct ieee80211_precac_entry {
    TAILQ_ENTRY(ieee80211_precac_entry)   pe_list;
    u_int8_t                              vht80_freq;
#if ATH_SUPPORT_ETSI_CAC_DFS
    u_int16_t                             freq; /* primary freq */
    unsigned long                         etsi_caclist_start_ticks; /* start ticks, OS specific */
#endif
    os_timer_t                            precac_nol_timer;   /* per element precac NOL timer */
    struct ieee80211com                   *ic;
};

typedef enum preCAC_chan_state {
    PRECAC_ERR      = -1,
    PRECAC_REQUIRED,
    PRECAC_NOW,
    PRECAC_DONE,
    PRECAC_NOL,
} PCAC_CHAN_STATE;
#endif

struct ieee80211_dfs_state {
	int		nol_event[IEEE80211_CHAN_MAX];
	os_timer_t	nol_timer;		/* NOL list processing */
	os_timer_t	cac_timer;		/* CAC timer */
	os_timer_t      cac_valid_timer;         /* Ignore CAC when this timer is running*/
	//struct timeval 	lastevent;		/* time of last radar event */
	int		cureps;			/* current events/second */
	const struct ieee80211_ath_channel *lastchan;/* chan w/ last radar event */
	struct ieee80211_ath_channel *newchan;	/* chan selected next */
	int		cac_timeout_override;	/* overridden cac timeout */
        int8_t     enable:1,
                   cac_timer_running:1,
                   ignore_dfs:1, 
                   ignore_cac:1,
                   dfs_disable:1,
		   cac_valid:1;
	uint32_t cac_valid_time;		/* time for which CAC will be valid and will not be re-done */ 
	os_timer_t ath_dfs_fw_status_wait_timer;   /* Host dfs status wait timer */
#if ATH_SUPPORT_ZERO_CAC_DFS
	os_timer_t	precac_timer;   /* PRECAC timer */
	int		precac_timeout_override;	/* overridden precac timeout */
	uint8_t  *precac_list;          /* List of precac VHT80 frequencies */
	uint8_t  num_precac_freqs;      /* Number of PreCAC VHT80 frequencies */

    /********** IF WE NOT NEED TO DELETE ARBITRARY ELEMENT WE CA USE STAILQ */
	TAILQ_HEAD(,ieee80211_precac_entry)    precac_required_list;
	TAILQ_HEAD(,ieee80211_precac_entry)    precac_done_list;
	TAILQ_HEAD(,ieee80211_precac_entry)    precac_nol_list;
	TAILQ_HEAD(,ieee80211_precac_entry)    etsi_precac_done_list;
	TAILQ_HEAD(,ieee80211_precac_entry)    etsi_precac_required_list;
#endif
	struct ieee80211_ath_channel *curchan;	/* curren chan */
};

void	ieee80211_dfs_attach(struct ieee80211com *);
void	ieee80211_dfs_detach(struct ieee80211com *);

int	 ieee80211_dfs_override_cac_timeout(struct ieee80211com *ic,
	    int cac_timeout);
int	ieee80211_dfs_get_override_cac_timeout(struct ieee80211com *ic,
	    int *cac_timeout);

void	ieee80211_dfs_reset(struct ieee80211com *);
void 	ieee80211_dfs_cac_valid_reset(struct ieee80211com *ic);

int	ieee80211_dfs_cac_start(struct ieee80211com *);
void	ieee80211_dfs_cac_stop(struct ieee80211vap *, int force);
void ieee80211_update_dfs_next_channel(struct ieee80211com *ic);
#if ATH_SUPPORT_DFS && ATH_SUPPORT_STA_DFS
void	ieee80211_dfs_stacac_stop(struct ieee80211vap *);
#endif

void	ieee80211_dfs_cac_clear(struct ieee80211com *,
		const struct ieee80211_ath_channel *);
int	ieee80211_dfs_cac_cancel(struct ieee80211com *);

#if ATH_SUPPORT_DFS && ATH_SUPPORT_STA_DFS
int	ieee80211_dfs_stacac_cancel(struct ieee80211com *);
int	ieee80211_get_cac_timeout(struct ieee80211com *ic,
        struct ieee80211_ath_channel *chan);
#endif
int	ieee80211_dfs_is_ap_cac_timer_running(struct ieee80211com * ic);
void	ieee80211_dfs_notify_radar(struct ieee80211com *,
		struct ieee80211_ath_channel *);
void	ieee80211_unmark_radar(struct ieee80211com *,
		struct ieee80211_ath_channel *);
void ieee80211_bringup_ap_vaps(struct ieee80211com *);
struct ieee80211_ath_channel *ieee80211_dfs_pickchannel(struct ieee80211com *);

#if ATH_SUPPORT_ZERO_CAC_DFS
void ieee80211_dfs_init_precac_list(struct ieee80211com *ic);
void ieee80211_dfs_deinit_precac_list(struct ieee80211com *ic);
uint8_t ieee80211_dfs_get_freq_from_precac_required_list(struct ieee80211com *ic,uint8_t cur_ieee_chan);
void ieee80211_dfs_start_precac_timer(struct ieee80211com *ic,uint8_t precac_chan);
void ieee80211_dfs_cancel_precac_timer(struct ieee80211com *ic);
void ieee80211_dfs_mark_precac_done(struct ieee80211com *ic, int freq);
int	 ieee80211_dfs_override_precac_timeout(struct ieee80211com *ic,
	    int precac_timeout);
int	ieee80211_dfs_get_override_precac_timeout(struct ieee80211com *ic,
	    int *precac_timeout);
void ieee80211_print_precaclists(struct ieee80211com *ic);
void ieee80211_reset_precaclists(struct ieee80211com *ic);
int	 ieee80211_dfs_get_override_host_wait_timeout(struct ieee80211com *ic,
	    int *host_wait_timeout);
int ieee80211_set_precac_prefered_channel(struct ieee80211vap *iv, uint8_t chan);
bool ieee80211_dfs_is_precac_done(struct ieee80211com *ic, struct ieee80211_ath_channel *chan);
#endif
int	 ieee80211_dfs_override_host_wait_timeout(struct ieee80211com *ic,
	    int host_wait_timeout);
#if ATH_SUPPORT_DFS && QCA_DFS_NOL_VAP_RESTART
void ieee80211_bringup_ap_vaps_after_nol(struct ieee80211com *ic);
#endif
void ieee80211_reset_radar_event_log_counter(struct ieee80211com *ic);
#endif /* _NET80211_IEEE80211_DFS_H_ */
