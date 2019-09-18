#ifndef __SPECTRAL_CMD_H__
#define __SPECTRAL_CMD_H__

#define DFS_LAST_IOCTL          22

/*
 * ioctl defines
 **/
#define SPECTRAL_SET_CONFIG              (DFS_LAST_IOCTL + 1)
#define SPECTRAL_GET_CONFIG              (DFS_LAST_IOCTL + 2)
#define SPECTRAL_SHOW_INTERFERENCE       (DFS_LAST_IOCTL + 3)
#define SPECTRAL_ENABLE_SCAN             (DFS_LAST_IOCTL + 4)
#define SPECTRAL_DISABLE_SCAN            (DFS_LAST_IOCTL + 5)
#define SPECTRAL_ACTIVATE_SCAN           (DFS_LAST_IOCTL + 6)
#define SPECTRAL_STOP_SCAN               (DFS_LAST_IOCTL + 7)
#define SPECTRAL_SET_DEBUG_LEVEL         (DFS_LAST_IOCTL + 8)
#define SPECTRAL_IS_ACTIVE               (DFS_LAST_IOCTL + 9)
#define SPECTRAL_IS_ENABLED              (DFS_LAST_IOCTL + 10)
#define SPECTRAL_CLASSIFY_SCAN           (DFS_LAST_IOCTL + 11)
#define SPECTRAL_GET_CLASSIFIER_CONFIG   (DFS_LAST_IOCTL + 12)
#define SPECTRAL_EACS                    (DFS_LAST_IOCTL + 13)
#define SPECTRAL_ACTIVATE_FULL_SCAN      (DFS_LAST_IOCTL + 14)
#define SPECTRAL_STOP_FULL_SCAN          (DFS_LAST_IOCTL + 15)
#define SPECTRAL_GET_CAPABILITY_INFO     (DFS_LAST_IOCTL + 16)
#define SPECTRAL_GET_DIAG_STATS          (DFS_LAST_IOCTL + 17)
#define SPECTRAL_GET_CHAN_WIDTH          (DFS_LAST_IOCTL + 18)
#define SPECTRAL_GET_CHANINFO            (DFS_LAST_IOCTL + 19)
#define SPECTRAL_CLEAR_CHANINFO          (DFS_LAST_IOCTL + 20)
#define SPECTRAL_SET_ICM_ACTIVE          (DFS_LAST_IOCTL + 21)
#define SPECTRAL_GET_NOMINAL_NOISEFLOOR  (DFS_LAST_IOCTL + 22)


#define AH_MAX_CHAINS 3

/* SPECTRAL SCAN defines begin */
typedef struct {
    u_int16_t   ss_fft_period;               /* Skip interval for FFT reports */
    u_int16_t   ss_period;                   /* Spectral scan period */
    u_int16_t   ss_count;                    /* # of reports to return from
                                                ss_active */
    u_int16_t   ss_short_report;             /* Set to report ony 1 set of FFT
                                                results */
    u_int8_t    radar_bin_thresh_sel;
    u_int16_t   ss_spectral_pri;             /* Priority, and are we doing a
                                                noise power cal ? */
    u_int16_t   ss_fft_size;                 /* Defines the number of FFT data
                                                points to compute, defined
                                                as a log index:
                                                num_fft_pts = 2^ss_fft_size */
    u_int16_t   ss_gc_ena;                   /* Set, to enable targeted gain
                                                change before starting the
                                                spectral scan FFT */
    u_int16_t   ss_restart_ena;              /* Set, to enable abort of receive
                                                frames when in high priority
                                                and a spectral scan is queued */
    u_int16_t   ss_noise_floor_ref;          /* Noise floor reference number
                                                signed) for the calculation
                                                of bin power (dBm).
                                                Though stored as an unsigned
                                                value, this should be treated
                                                as a signed 8-bit int. */
    u_int16_t   ss_init_delay;               /* Disallow spectral scan triggers
                                                after tx/rx packets by setting
                                                this delay value to roughly
                                                SIFS time period or greater.
                                                Delay timer counts in units of
                                                0.25us */
    u_int16_t   ss_nb_tone_thr;              /* Number of strong bins
                                                (inclusive) per sub-channel,
                                                below which a signal is declared
                                                a narrowband tone */
    u_int16_t   ss_str_bin_thr;              /* Bin/max_bin ratio threshold over
                                                which a bin is declared strong
                                                (for spectral scan bandwidth
                                                analysis). */
    u_int16_t   ss_wb_rpt_mode;              /* Set this bit to report spectral
                                                scans as EXT_BLOCKER
                                                (phy_error=36), if none of the
                                                sub-channels are deemed
                                                narrowband. */
    u_int16_t   ss_rssi_rpt_mode;            /* Set this bit to report spectral
                                                scans as EXT_BLOCKER
                                                (phy_error=36), if the ADC RSSI
                                                is below the threshold
                                                ss_rssi_thr */
    u_int16_t   ss_rssi_thr;                 /* ADC RSSI must be greater than or
                                                equal to this threshold
                                                (signed Db) to ensure spectral
                                                scan reporting with normal phy
                                                error codes (please see
                                                ss_rssi_rpt_mode above).
                                                Though stored as an unsigned
                                                value, this should be treated
                                                as a signed 8-bit int. */
    u_int16_t   ss_pwr_format;               /* Format of frequency bin
                                                magnitude for spectral scan
                                                triggered FFTs:
                                                0: linear magnitude
                                                1: log magnitude
                                                   (20*log10(lin_mag),
                                                   1/2 dB step size) */
    u_int16_t   ss_rpt_mode;                 /* Format of per-FFT reports to
                                                software for spectral scan
                                                triggered FFTs.
                                                0: No FFT report
                                                (only pulse end summary)
                                                1: 2-dword summary of metrics
                                                for each completed FFT
                                                2: 2-dword summary +
                                                1x-oversampled bins(in-band)
                                                per FFT
                                                3: 2-dword summary +
                                                2x-oversampled bins (all)
                                                per FFT */
    u_int16_t   ss_bin_scale;                /* Number of LSBs to shift out to
                                                scale the FFT bins for spectral
                                                scan triggered FFTs. */
    u_int16_t   ss_dBm_adj;                  /* Set (with ss_pwr_format=1), to
                                                report bin magnitudes converted
                                                to dBm power using the
                                                noisefloor calibration
                                                results. */
    u_int16_t   ss_chn_mask;                 /* Per chain enable mask to select
                                                input ADC for search FFT. */
    int8_t      ss_nf_cal[AH_MAX_CHAINS*2];  /* nf calibrated values for
                                                ctl+ext */
    int8_t      ss_nf_pwr[AH_MAX_CHAINS*2];  /* nf pwr values for ctl+ext */
    int32_t     ss_nf_temp_data;             /* temperature data taken during
                                                nf scan */
} HAL_SPECTRAL_PARAM;

#define MAX_NUM_CHANNELS 255

typedef struct spectral_chan_stats {
    int cycle_count;
    int channel_load;
    int per;
    int noisefloor;
    u_int16_t comp_usablity;
    int8_t maxregpower;
} SPECTRAL_CHAN_STATS_T;

#endif //__SPECTRAL_CMD_H__
