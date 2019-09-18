/*
 * Copyright (c) 2011, 2017 Qualcomm Innovation Center, Inc.
 *
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 *
 * Copyright (c) 2008-2010, Atheros Communications Inc.
 * All Rights Reserved.
 *
 * 2011 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include "opt_ah.h"

#ifdef AH_SUPPORT_AR9300

#include "ah.h"
#include "ah_internal.h"
#include "ah_devid.h"
#ifdef AH_DEBUG
#include "ah_desc.h"                    /* NB: for HAL_PHYERR* */
#endif
#include "ar9300/ar9300.h"
#include "ar9300/ar9300eep.h"
#include "ar9300/ar9300template_generic.h"
#include "ar9300/ar9300template_xb112.h"
#include "ar9300/ar9300template_hb116.h"
#include "ar9300/ar9300template_xb113.h"
#include "ar9300/ar9300template_hb112.h"
#include "ar9300/ar9300template_ap121.h"
#include "ar9300/ar9300template_osprey_k31.h"
#include "ar9300/ar9300template_wasp_2.h"
#include "ar9300/ar9300template_wasp_k31.h"
#include "ar9300/ar9300template_aphrodite.h"
#include "ar9300/ar9300reg.h"
#include "ar9300/ar9300phy.h"

#if ATH_DRIVER_SIM
#include "ah_sim.h"
#endif

#if defined(AR5500_EMULATION) && !defined(ART_BUILD)
#include <linux/jiffies.h>
#endif

#ifdef MDK_AP
#include <stdio.h>
#include "instance.h"
#endif
#if AH_BYTE_ORDER == AH_BIG_ENDIAN
#ifndef ART_BUILD
void ar9300_eeprom_template_swap(struct ath_hal *ah);
void jet_swap_eeprom(ar9300_eeprom_t *eep);
void osprey_swap_eeprom(osprey_eeprom_t *eep);
#else
void ar9300_eeprom_template_swap(void);
void ar9300_swap_eeprom(ar9300_eeprom_t *eep);
void ar5500_swap_eeprom(ar5500_eeprom_t *eep);
#endif
#endif
#ifdef AH_CAL_IN_FILE_HOST
int __ahdecl qdf_fs_read(char *filename,
		loff_t offset,
		unsigned int size,
		uint8_t *buffer);
#endif
bool ar9300_otp_read_byte(struct ath_hal *ah, u_int off, u_int8_t *data, bool is_wifi);
static u_int16_t ar9300_eeprom_get_spur_chan(struct ath_hal *ah,
    u_int16_t spur_chan, bool is_2ghz);
#ifdef UNUSED
static inline bool ar9300_fill_eeprom(struct ath_hal *ah);
static inline HAL_STATUS ar9300_check_eeprom(struct ath_hal *ah);
#endif
static u_int8_t current_preferred_therm_chain = 0x1;
bool ar9300_set_temp_deg_c_chain(struct ath_hal *ah, u_int8_t preferred_therm_chain);
bool ar9300_get_temp_deg_c(struct ath_hal *ah, int16_t *temp);

#ifndef ART_BUILD
static ar9300_eeprom_t *jet_default_template = &ar9300_template_jet;
static osprey_eeprom_t *osprey_eeprom_9300[] =
{
    &ar9300_template_generic,
    &ar9300_template_xb112,
    &ar9300_template_hb116,
    &ar9300_template_hb112,
    &ar9300_template_xb113,
    &ar9300_template_ap121,
    &ar9300_template_wasp_2,
    &ar9300_template_wasp_k31,
    &ar9300_template_osprey_k31,
    &ar9300_template_aphrodite,
};
#else
static ar5500_eeprom_t *jet_default_template = &ar5500_template;
static ar9300_eeprom_t *default9300[] =
{
    &ar9300_template_generic,
    &ar9300_template_xb112,
    &ar9300_template_hb116,
    &ar9300_template_hb112,
    &ar9300_template_xb113,
    &ar9300_template_ap121,
    &ar9300_template_wasp_2,
    &ar9300_template_wasp_k31,
    &ar9300_template_osprey_k31,
    &ar9300_template_aphrodite,
};
#endif
/*
 * Different types of memory where the calibration data might be stored.
 * All types are searched in ar9300_eeprom_restore()
 * in the order flash, eeprom, otp.
 * To disable searching a type, set its parameter to 0.
 */

/*
 * This is where we look for the calibration data.
 * must be set before ath_attach() is called
 */
static int calibration_data_try = calibration_data_none;
static int calibration_data_try_address = 0;

/*
 * Timing Parameter
 */
/* 66MHz for Ruby */
#define EFUSE_TRS_CSRST_REG_V 66
#define EFUSE_TCPS_REG_V 198 /* For write only */
#define EFUSE_TCPH_REG_V 132 /* For write only */
#define EFUSE_TPW_REG_V 264 /* For write only */
#define EFUSE_TPGSV_REG_V 7 /* For write only */
#define EFUSE_TPVSR_REG_V 60 /* For write only */
#define EFUSE_TPVSA_REG_V 7 /* For write only */
#define EFUSE_TPEH_REG_V 198 /* For write only */
#define EFUSE_TPES_REG_V 66 /* For write only */
#define EFUSE_TACC_REG_V 7
#define EFUSE_TRW_REG_V 2
#define EFUSE_TRWH_REG_V 7
#define EFUSE_TPVHR_REG_V 7 /* For write only */
#define EFUSE_TPRYCEB_REG_V 7

void efuse_enable_write(struct ath_hal *ah)
{
    int32_t otp_status;

    OS_REG_WRITE(ah, AR_JET_EFUSE( TRS_CSRST_REG), EFUSE_TRS_CSRST_REG_V); //1000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TCPS_REG), EFUSE_TCPS_REG_V); //3000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TCPH_REG), EFUSE_TCPH_REG_V); //2000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPW_REG), EFUSE_TPW_REG_V); //3500-4500ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPGSV_REG), EFUSE_TPGSV_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPVSR_REG), EFUSE_TPVSR_REG_V); //900ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPVSA_REG), EFUSE_TPVSA_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPEH_REG), EFUSE_TPEH_REG_V); //3000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPES_REG), EFUSE_TPES_REG_V); //1000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TACC_REG), EFUSE_TACC_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TRW_REG), EFUSE_TRW_REG_V); //20ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TRWH_REG), EFUSE_TRWH_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPVHR_REG), EFUSE_TPVHR_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPRYCEB_REG), EFUSE_TPRYCEB_REG_V); //100ns

    OS_REG_RMW_FIELD_2(ah, EFUSE_WR_ENABLE_REG, EFUSE_LDO25_EN_REG_V, 1);
    otp_status = OS_REG_READ_FIELD_2(ah, EFUSE_WR_ENABLE_REG, EFUSE_BUSY_REG_V);
    printk("%s: otp_status=%x\n", __func__, otp_status);

    OS_REG_RMW_FIELD_2(ah, EFUSE_WR_ENABLE_REG, EFUSE_WR_ENABLE_REG_V, 1);
    OS_REG_RMW_FIELD_2(ah, EFUSE_INT_ENABLE_REG, EFUSE_INT_ENABLE_REG_V, 1);
    OS_REG_WRITE(ah, AR_JET_EFUSE(BITMASK_WR_REG), 0x00); //3000ns
}

void efuse_enable_read(struct ath_hal *ah)
{

    int32_t otp_status;
    OS_REG_WRITE(ah, AR_JET_EFUSE( TRS_CSRST_REG), EFUSE_TRS_CSRST_REG_V); //1000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TCPS_REG), EFUSE_TCPS_REG_V); //3000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TCPH_REG), EFUSE_TCPH_REG_V); //2000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPW_REG), EFUSE_TPW_REG_V); //3500-4500ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPGSV_REG), EFUSE_TPGSV_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPVSR_REG), EFUSE_TPVSR_REG_V); //900ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPVSA_REG), EFUSE_TPVSA_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPEH_REG), EFUSE_TPEH_REG_V); //3000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPES_REG), EFUSE_TPES_REG_V); //1000ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TACC_REG), EFUSE_TACC_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TRW_REG), EFUSE_TRW_REG_V); //20ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TRWH_REG), EFUSE_TRWH_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPVHR_REG), EFUSE_TPVHR_REG_V); //100ns
    OS_REG_WRITE(ah, AR_JET_EFUSE( TPRYCEB_REG), EFUSE_TPRYCEB_REG_V); //100ns
    OS_REG_RMW_FIELD_2(ah, EFUSE_WR_ENABLE_REG, EFUSE_LDO25_EN_REG_V, 1);

    do
    {
        otp_status = OS_REG_READ_FIELD_2(ah, EFUSE_WR_ENABLE_REG, EFUSE_LDO25_STATUS_REG_V);
    }
    while (otp_status == 0);
    OS_REG_RMW_FIELD_2(ah, EFUSE_INT_ENABLE_REG, EFUSE_INT_ENABLE_REG_V, 1);
}

void efuse_disable(struct ath_hal *ah)
{
    int32_t otp_status;
    OS_REG_RMW_FIELD_2(ah, EFUSE_WR_ENABLE_REG, EFUSE_WR_ENABLE_REG_V, 0);
    OS_REG_RMW_FIELD_2(ah, EFUSE_WR_ENABLE_REG, EFUSE_LDO25_EN_REG_V, 0);
    do
    {
        otp_status = OS_REG_READ_FIELD_2(ah, EFUSE_WR_ENABLE_REG, EFUSE_BUSY_REG_V);
    }
    while (otp_status != 0);
}

/*
 * Set the type of memory used to store calibration data.
 * Used by nart to force reading/writing of a specific type.
 * The driver can normally allow autodetection
 * by setting source to calibration_data_none=0.
 */
void ar9300_calibration_data_set(struct ath_hal *ah, int32_t source)
{
    if (ah != 0) {
        AH9300(ah)->calibration_data_source = source;
    } else {
        calibration_data_try = source;
    }
}

int32_t ar9300_calibration_data_get(struct ath_hal *ah)
{
    if (ah != 0) {
        return AH9300(ah)->calibration_data_source;
    } else {
        return calibration_data_try;
    }
}

/*
 * Set the address of first byte used to store calibration data.
 * Used by nart to force reading/writing at a specific address.
 * The driver can normally allow autodetection by setting size=0.
 */
void ar9300_calibration_data_address_set(struct ath_hal *ah, int32_t size)
{
    if (ah != 0) {
        AH9300(ah)->calibration_data_source_address = size;
    } else {
        calibration_data_try_address = size;
    }
}

int32_t ar9300_calibration_data_address_get(struct ath_hal *ah)
{
    if (ah != 0) {
        return AH9300(ah)->calibration_data_source_address;
    } else {
        return calibration_data_try_address;
    }
}

/*
 * This is the template that is loaded if ar9300_eeprom_restore()
 * can't find valid data in the memory.
 */
#ifndef ART_BUILD
static int Osprey_eeprom_template_preference = ar9300_eeprom_template_generic;
#else
static int Osprey_eeprom_template_preference = ar9300_eeprom_template_aphrodite;
#endif

void osprey_eeprom_template_preference(int32_t value)
{
    Osprey_eeprom_template_preference = value;
}

/*
 * Install the specified default template.
 * Overwrites any existing calibration and configuration information in memory.
 */
int32_t ar9300_eeprom_template_install(struct ath_hal *ah, int32_t value)
{
    struct ath_hal_9300 *ahp = AH9300(ah);
    ar9300_eeprom_t *mptr;
    int mdata_size;
#ifndef ART_BUILD
    osprey_eeprom_t *osprey_eeprom, *osprey_dptr;
#else
    ar9300_eeprom_t *dptr;
    ar5500_eeprom_t *mptr_jet, *dptr_jet;
#endif


#ifndef ART_BUILD
    mptr = &ahp->ah_eeprom;
    mdata_size = ar9300_eeprom_struct_size();

    if(!AR_SREV_JET(ah)) {
	osprey_eeprom = ath_hal_malloc(ah, sizeof(osprey_eeprom_t));
	if(osprey_eeprom == AH_NULL) {
	    HDPRINTF(ah, HAL_DBG_MALLOC, "%s: cannot allocate osprey_eeprom\n", __func__);
	    return AH_FALSE;
	}
	mdata_size = sizeof(osprey_eeprom_t);

#if 0
	    calibration_data_source = calibration_data_none;
	    calibration_data_source_address = 0;
#endif
	    osprey_dptr = osprey_eeprom_struct_default_find_by_id(value);
	    if (osprey_dptr != 0) {
		OS_MEMCPY(osprey_eeprom, osprey_dptr, mdata_size);
		HDPRINTF(ah, HAL_DBG_EEPROM, "%s: template id : %d, osprey template loaded \n", __func__, value);
		copy_osprey_to_jet(mptr, osprey_eeprom);
		ath_hal_free(ah, osprey_eeprom);
	    }
    } else {
	if (jet_default_template != 0) {
	    OS_MEMCPY(mptr, jet_default_template, mdata_size);
	    HDPRINTF(ah, HAL_DBG_EEPROM, "%s: Jet default template loaded \n", __func__);
	}
    }
    return -1;
#else
    if (!AR_SREV_JET(ah)) {
        mptr = &ahp->ah_eeprom;
        mdata_size = ar9300_eeprom_struct_size(ah);
        dptr = ar9300_eeprom_struct_default_find_by_id(value);
        if (dptr != 0) {
            OS_MEMCPY(mptr, dptr, mdata_size);
            return 0;
        }
    }
    else
    {
        mptr_jet = &ahp->ah_eeprom_jet;
        mdata_size = ar9300_eeprom_struct_size(ah);
        dptr_jet = ar5500_eeprom_struct_default();
        if (dptr_jet != 0) {
            OS_MEMCPY(mptr_jet, dptr_jet, mdata_size);
            return 0;
        }
    }
    return -1;
#endif
}

static int
ar9300_eeprom_restore_something(struct ath_hal *ah, void *mptr,
	int mdata_size)
{
    int it;
    int nptr;
#ifndef ART_BUILD
    osprey_eeprom_t *osprey_eeprom, *osprey_dptr;
#else
    ar9300_eeprom_t *dptr;
#endif

    nptr = -1;

    /*
     * if we didn't find any blocks in the memory,
     * put the prefered template in place
     */
    if(AR_SREV_JET(ah)) {
	    if (nptr < 0) {
	        AH9300(ah)->calibration_data_source = calibration_data_none;
	        AH9300(ah)->calibration_data_source_address = 0;
	        if (jet_default_template != 0) {
		        OS_MEMCPY(mptr, jet_default_template, mdata_size);
		        HDPRINTF(ah, HAL_DBG_EEPROM, "%s: Jet default template loaded \n", __func__);
                nptr = 0;
	        }
	    }
    }

#ifndef ART_BUILD
    if(!AR_SREV_JET(ah)) {
	    osprey_eeprom = ath_hal_malloc(ah, sizeof(osprey_eeprom_t));
	    if(osprey_eeprom == AH_NULL) {
	        HDPRINTF(ah, HAL_DBG_MALLOC, "%s: cannot allocate osprey_eeprom\n", __func__);
	        return AH_FALSE;
	    }
	    mdata_size = sizeof(osprey_eeprom_t);

	    /*
	    * if we didn't find any blocks in the memory,
	    * put the prefered template in place
	    */
	    if (nptr < 0) {
	        AH9300(ah)->calibration_data_source = calibration_data_none;
	        AH9300(ah)->calibration_data_source_address = 0;
	        osprey_dptr = osprey_eeprom_struct_default_find_by_id(Osprey_eeprom_template_preference);
	        if (osprey_dptr != 0) {
		        HDPRINTF(ah, HAL_DBG_EEPROM, "%s(%d) Osprey_eeprom_template_preference=%d\n", __func__, __LINE__,
			    Osprey_eeprom_template_preference);
		        OS_MEMCPY(osprey_eeprom, osprey_dptr, mdata_size);
		        nptr = 0;
	        }
	    }

	    /*
	     * if we can't find the best template, put any old template in place
	     * presume that newer ones are better, so search backwards
	     */
	    if (nptr < 0) {
	        AH9300(ah)->calibration_data_source = calibration_data_none;
	        AH9300(ah)->calibration_data_source_address = 0;
	        for (it = osprey_eeprom_struct_default_many() - 1; it >= 0; it--) {
		        osprey_dptr = osprey_eeprom_struct_default(it);
		        if (osprey_dptr != 0) {
		            OS_MEMCPY(osprey_eeprom, osprey_dptr, mdata_size);
	                nptr = 0;
		            HDPRINTF(ah, HAL_DBG_EEPROM, "%s(%d) template id: %d, osprey template loaded \n", __func__, __LINE__, it);
		            break;
		        }
	        }
	    }
	    copy_osprey_to_jet((ar9300_eeprom_t *)mptr, osprey_eeprom);
	    HDPRINTF(ah, HAL_DBG_EEPROM, "%s(%d):copy_osprey_to_jet \n", __func__, __LINE__);
	    ath_hal_free(ah, osprey_eeprom);
    }
#else
    if(!AR_SREV_JET(ah)) {
        if (nptr < 0) {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
            dptr = ar9300_eeprom_struct_default_find_by_id(
                Osprey_eeprom_template_preference);
            if (dptr != 0) {
                OS_MEMCPY(mptr, dptr, mdata_size);
                nptr = 0;
            }
        }
        /*
         * if we didn't find the prefered one,
         * put the normal default template in place
         */
        if (nptr < 0) {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
            dptr = ar9300_eeprom_struct_default_find_by_id(
                ar9300_eeprom_template_default);
            if (dptr != 0) {
                OS_MEMCPY(mptr, dptr, mdata_size);
                nptr = 0;
            }
        }
        /*
         * if we can't find the best template, put any old template in place
         * presume that newer ones are better, so search backwards
         */
        if (nptr < 0) {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
            for (it = ar9300_eeprom_struct_default_many() - 1; it >= 0; it--) {
                dptr = ar9300_eeprom_struct_default(it);
                if (dptr != 0) {
                    OS_MEMCPY(mptr, dptr, mdata_size);
                    nptr = 0;
                    break;
                }
            }
        }
    }
#endif
    return nptr;
}

/*
 * Read 16 bits of data from offset into *data
 */
bool
ar9300_eeprom_read_word(struct ath_hal *ah, u_int off, u_int16_t *data)
{
    if (AR_SREV_OSPREY(ah) || AR_SREV_POSEIDON(ah))
    {
        (void) OS_REG_READ(ah, AR9300_EEPROM_OFFSET + (off << AR9300_EEPROM_S));
        if (!ath_hal_wait(ah,
			  AR_HOSTIF_REG(ah, AR_EEPROM_STATUS_DATA),
			  AR_EEPROM_STATUS_DATA_BUSY | AR_EEPROM_STATUS_DATA_PROT_ACCESS,
			  0, AH_WAIT_TIMEOUT))
	{
            return false;
	}
        *data = MS(OS_REG_READ(ah,
			       AR_HOSTIF_REG(ah, AR_EEPROM_STATUS_DATA)), AR_EEPROM_STATUS_DATA_VAL);
       return true;
    }
    else
    {
        *data = 0;
        return false;
    }
}

#ifdef AH_SUPPORT_WRITE_EEPROM

/*
 * Write 16 bits of data from data to the specified EEPROM offset.
 */
bool
ar9300_eeprom_write(struct ath_hal *ah, u_int off, u_int16_t data)
{
    u_int32_t status;
    u_int32_t write_to = 50000;      /* write timeout */
    u_int32_t eeprom_value;
    struct ath_hal_9300 *ahp = AH9300(ah);
    u_int32_t eeprom_write_gpio;
    u_int32_t output_mux_register = AR_HOSTIF_REG(ah, AR_GPIO_OUTPUT_MUX1);
    u_int32_t mux_bit_position = 15;  /* old default of GPIO-3 */
    u_int32_t config_register = AR_HOSTIF_REG(ah, AR_GPIO_OE_OUT);
    u_int32_t config_bit_position;

#ifndef ART_BUILD
    eeprom_write_gpio = ahp->ah_eeprom.base_eep_header.eeprom_write_enable_gpio;
    config_bit_position = 2 * eeprom_write_gpio;
#else
    if(!AR_SREV_JET(ah)){
        eeprom_write_gpio = ahp->ah_eeprom.base_eep_header.eeprom_write_enable_gpio;
        config_bit_position = 2 * eeprom_write_gpio;
    }else{
        eeprom_write_gpio = ahp->ah_eeprom_jet.base_eep_header.eeprom_write_enable_gpio;
        config_bit_position = 2 * eeprom_write_gpio;
    }

#endif
    eeprom_value = (u_int32_t)data & 0xffff;
    /* get the correct registers and bit position for GPIO configuration */
    if (eeprom_write_gpio <= 5) {
        output_mux_register = AR_HOSTIF_REG(ah, AR_GPIO_OUTPUT_MUX1);
        mux_bit_position = eeprom_write_gpio * 5;
    } else if ((eeprom_write_gpio >= 6) && (eeprom_write_gpio <= 11)) {
        output_mux_register = AR_HOSTIF_REG(ah, AR_GPIO_OUTPUT_MUX2);
        mux_bit_position = (eeprom_write_gpio-6) * 5;
    } else if ((eeprom_write_gpio >= 12) && (eeprom_write_gpio <= 16)) {
        output_mux_register = AR_HOSTIF_REG(ah, AR_GPIO_OUTPUT_MUX3);
        mux_bit_position = (eeprom_write_gpio-12) * 5;
    }

    if (eeprom_write_gpio == 16) {
        config_register = AR_HOSTIF_REG(ah, AR_GPIO_OE1_OUT);
        config_bit_position = 0;
    }

    /* Setup EEPROM device to write */
    /* Mux GPIO-3 as GPIO */
    OS_REG_RMW(ah, output_mux_register, 0, 0x1f << mux_bit_position);
    OS_DELAY(1);
    /* Configure GPIO-3 as output */
    OS_REG_RMW(ah, config_register,
        0x3 << config_bit_position, 0x3 << config_bit_position);
    OS_DELAY(1);
#ifdef AR5500_EMULATION
    HDPRINTF(AH_NULL, HAL_DBG_UNMASKABLE, "ar9300_eeprom_write: FIXME\n");
#else
    /* drive GPIO-3 low */
    OS_REG_RMW(ah, AR_GPIO_IN_OUT, 0, 1 << eeprom_write_gpio);
#endif
    OS_DELAY(1);

    /* Send write data, as 32 bit data */
    OS_REG_WRITE(ah,
        AR9300_EEPROM_OFFSET + (off << AR9300_EEPROM_S), eeprom_value);

    /* check busy bit to see if eeprom write succeeded */
    while (write_to > 0) {
        status = OS_REG_READ(ah, AR_HOSTIF_REG(ah, AR_EEPROM_STATUS_DATA)) &
            (AR_EEPROM_STATUS_DATA_BUSY |
             AR_EEPROM_STATUS_DATA_BUSY_ACCESS |
             AR_EEPROM_STATUS_DATA_PROT_ACCESS |
             AR_EEPROM_STATUS_DATA_ABSENT_ACCESS);
        if (status == 0) {
#ifndef AR5500_EMULATION
            /* drive GPIO-3 hi */
            OS_REG_RMW(ah,
                AR_GPIO_IN_OUT, 1 << eeprom_write_gpio, 1 << eeprom_write_gpio);
#endif
            return true;
        }
        OS_DELAY(1);
        write_to--;
    }

#ifndef AR5500_EMULATION
    /* drive GPIO-3 hi */
    OS_REG_RMW(ah,
        AR_GPIO_IN_OUT, 1 << eeprom_write_gpio, 1 << eeprom_write_gpio);
#endif
    return false;
}
#endif
bool
ar9300_set_temp_deg_c_chain(struct ath_hal *ah, u_int8_t preferred_therm_chain)
{
  if (preferred_therm_chain==0x01 || preferred_therm_chain==0x02 || preferred_therm_chain==0x04 || preferred_therm_chain==0x08)
  {
    current_preferred_therm_chain = preferred_therm_chain;
    OS_REG_RMW_FIELD(ah,AR_PHY_BB_THERM_ADC_4,AR_PHY_BB_THERM_ADC_4_FORCE_THERM_CHAIN,0x1);
    OS_REG_RMW_FIELD(ah,AR_PHY_BB_THERM_ADC_4,AR_PHY_BB_THERM_ADC_4_PREFERRED_THERM_CHAIN,preferred_therm_chain);
    return true;
  }
  else
  {
    current_preferred_therm_chain = 0x01;
    OS_REG_RMW_FIELD(ah,AR_PHY_BB_THERM_ADC_4,AR_PHY_BB_THERM_ADC_4_FORCE_THERM_CHAIN,0x1);
    OS_REG_RMW_FIELD(ah,AR_PHY_BB_THERM_ADC_4,AR_PHY_BB_THERM_ADC_4_PREFERRED_THERM_CHAIN,0x01);
    return false;
  }
}
/* This temperature reading API only for Jet */
bool
ar9300_get_temp_deg_c(struct ath_hal *ah, int16_t *temp)
{
  // OTP address
  #define ate_temp_addr                 19
  #define ate_temp_code_chain0_7_0_addr 20
  #define ate_temp_code_chain1_7_0_addr 22
  #define ate_temp_code_chain2_7_0_addr 24
  #define ate_temp_code_chain3_7_0_addr 26

  u_int8_t val_low_byte;
  int32_t Ko_a=0, Ko=0, Kg=0;
  int32_t ate_temp, ate_temp_code[4], slope_temp[4], latest_therm_value[4];
  u_int8_t preferred_therm_chain;
  preferred_therm_chain = current_preferred_therm_chain;
#ifndef ART_BUILD
  ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#else
  ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
  if (!AR_SREV_JET(ah))
    return false; /* only support Jet */
#ifndef ART_BUILD
  if (eep->modal_header_2g.temp_slope_ext[0] == 0)
      slope_temp[0]=slope_temp[1]=slope_temp[2]=slope_temp[3]=0x61;
  else {
      slope_temp[0] = eep->modal_header_2g.temp_slope_ext[0];
      slope_temp[1] = eep->modal_header_2g.temp_slope_ext[1];
      slope_temp[2] = eep->modal_header_2g.temp_slope_ext[2];
      slope_temp[3] = eep->modal_header_2g.temp_slope_ext[3];
  }
  Kg=eep->chipCalData.thermAdcScaledGain;
  Ko=eep->chipCalData.thermAdcOffset;
#else

  if( eep_jet->modal_header_2g.temp_slope_ext[0] == 0) {
      slope_temp[0]=slope_temp[1]=slope_temp[2]=slope_temp[3]=0x61;
  }
  else
  {
      slope_temp[0] = eep_jet->modal_header_2g.temp_slope_ext[0];
      slope_temp[1] = eep_jet->modal_header_2g.temp_slope_ext[1];
      slope_temp[2] = eep_jet->modal_header_2g.temp_slope_ext[2];
      slope_temp[3] = eep_jet->modal_header_2g.temp_slope_ext[3];
  }

  Kg=eep_jet->chipCalData.thermAdcScaledGain;
  Ko=eep_jet->chipCalData.thermAdcOffset;
#endif
  OS_REG_RMW_FIELD(ah,AR_PHY_BB_THERM_ADC_3,AR_PHY_BB_THERM_ADC_3_THERM_ADC_OFFSET,Ko);
  OS_REG_RMW_FIELD(ah,AR_PHY_BB_THERM_ADC_3,AR_PHY_BB_THERM_ADC_3_THERM_ADC_SCALE_GAIN,Kg);

  if(Ko<128) Ko_a=Ko;
  else       Ko_a=Ko-256;

  // OTP value
  ar9300_otp_read_byte(ah,ate_temp_addr,&val_low_byte,true);
  if(val_low_byte==0)
  {
    ate_temp=65;
    ate_temp_code[0]=178;
    ate_temp_code[1]=186;
    ate_temp_code[2]=182;
    ate_temp_code[3]=177;
  }
  else
  {
    ar9300_otp_read_byte(ah,ate_temp_addr,&val_low_byte,true);
    ate_temp=(val_low_byte&0xff);
    ar9300_otp_read_byte(ah,ate_temp_code_chain0_7_0_addr,&val_low_byte,true);
    ate_temp_code[0]=((val_low_byte&0xff)+Ko_a) *Kg/256;
    ar9300_otp_read_byte(ah,ate_temp_code_chain1_7_0_addr,&val_low_byte,true);
    ate_temp_code[1]=((val_low_byte&0xff)+Ko_a) *Kg/256;
    ar9300_otp_read_byte(ah,ate_temp_code_chain2_7_0_addr,&val_low_byte,true);
    ate_temp_code[2]=((val_low_byte&0xff)+Ko_a) *Kg/256;
    ar9300_otp_read_byte(ah,ate_temp_code_chain3_7_0_addr,&val_low_byte,true);
    ate_temp_code[3]=((val_low_byte&0xff)+Ko_a) *Kg/256;
  }

  if (preferred_therm_chain==0x01)
  {
    latest_therm_value[0]=OS_REG_READ_FIELD(ah,AR_PHY_BB_THERM_ADC_4,AR_PHY_BB_THERM_ADC_4_LATEST_THERM);
    *temp=(int)(-slope_temp[0]*(latest_therm_value[0]-ate_temp_code[0])/128+ate_temp-40);
  }
  else if (preferred_therm_chain==0x02)
  {
    latest_therm_value[1]=OS_REG_READ_FIELD(ah,AR_PHY_BB_THERM_ADC_4,AR_PHY_BB_THERM_ADC_4_LATEST_THERM);
    *temp=(int)(-slope_temp[1]*(latest_therm_value[1]-ate_temp_code[1])/128+ate_temp-40);
  }
  else if (preferred_therm_chain==0x04)
  {
    latest_therm_value[2]=OS_REG_READ_FIELD(ah,AR_PHY_BB_THERM_ADC_4,AR_PHY_BB_THERM_ADC_4_LATEST_THERM);
    *temp=(int)(-slope_temp[2]*(latest_therm_value[2]-ate_temp_code[2])/128+ate_temp-40);
  }
  else if (preferred_therm_chain==0x08)
  {
    latest_therm_value[3]=OS_REG_READ_FIELD(ah,AR_PHY_BB_THERM_ADC_4,AR_PHY_BB_THERM_ADC_4_LATEST_THERM);
    *temp=(int)(-slope_temp[3]*(latest_therm_value[3]-ate_temp_code[3])/128+ate_temp-40);
  }
  else
  {
    *temp=0;
    return false;
  }
  return true;
}

/*
 * off: offset, must be 4-byte aligmnt, and always return 4-byte data.
 * *data: where we read and store 4-byte data, begin at byte offset, 'off'
 * is_wifi: dummy.
 */

bool
qcn5500_otp_read(struct ath_hal *ah, u_int off, u_int32_t *data, bool is_wifi)
{
    int i;
    int offset;
    u_int32_t otp_byte;
    efuse_enable_read(ah);
    offset = off*4; // begin offset to read.
    *data = 0; // clean storage.
    for(i=0; i<4; i++, offset++)
    {
        otp_byte = OS_REG_READ(ah, AR_JET_EFUSE(EFUSE_INTF_0) + (offset << 2)) & 0xFF;
        (*data) = (*data) | (otp_byte << (8*i));
    }
    efuse_disable(ah);
    return true;
}
/*
 * off: offset, byte address, and always return 1-byte data.
 * *data: where we read and store 1-byte data
 * is_wifi: dummy.
 */

bool
ar9300_otp_read_byte(struct ath_hal *ah, u_int off, u_int8_t *data, bool is_wifi)
{
    efuse_enable_read(ah);
    //offset = off*4; // begin offset to read.
    *data = 0; // clean storage.
    *data = OS_REG_READ(ah, AR_JET_EFUSE(EFUSE_INTF_0) + (off<<2)) & 0xFF;
    efuse_disable(ah);
    return true;
}

/*
 * off: offset, 4-byte aligned
 * data: consecutive 4-byte data to be written.
 * is_wifi: dummy
 */

bool
qcn5500_otp_write(struct ath_hal *ah, u_int off, u_int32_t data, bool is_wifi)
{
    int i;
    u_int32_t offset;

    efuse_enable_write(ah);
    /* otp write begin */
    offset = off*4;
    for (i=0; i<4; i++, offset++)
    {
        OS_REG_WRITE(ah, AR_JET_EFUSE(EFUSE_INTF_0) + (offset << 2), (data >> (8*i)));
        //OS_REG_READ(ah, AR_JET_EFUSE(EFUSE_INTF_0) + (offset << 2));
    }
    efuse_disable(ah);
    return true;
}

bool
ar9300_otp_read(struct ath_hal *ah, u_int off, u_int32_t *data, bool is_wifi)
{
    int time_out = 1000;
    int status = 0;
    u_int32_t addr;

    if (AR_SREV_JET(ah)) {
        qcn5500_otp_read(ah, off, data, is_wifi);
        return true;
    } else if (AR_SREV_DRAGONFLY(ah) || AR_SREV_HONEYBEE(ah)){ /* no OTP for Dragonfly and honeybee */
        return false;
    }

    addr = (AR_SREV_WASP(ah) || AR_SREV_SCORPION(ah))?
        OTP_MEM_START_ADDRESS_WASP : OTP_MEM_START_ADDRESS;
	if (!is_wifi) {
        addr = BTOTP_MEM_START_ADDRESS;
    }
    addr += off * 4; /* OTP is 32 bit addressable */
    (void) OS_REG_READ(ah, addr);

    addr = (AR_SREV_WASP(ah) || AR_SREV_SCORPION(ah)) ?
        OTP_STATUS0_OTP_SM_BUSY_WASP : OTP_STATUS0_OTP_SM_BUSY;
	if (!is_wifi) {
        addr = BTOTP_STATUS0_OTP_SM_BUSY;
    }
    while ((time_out > 0) && (!status)) { /* wait for access complete */
        /* Read data valid, access not busy, sm not busy */
        status = ((OS_REG_READ(ah, addr) & 0x7) == 0x4) ? 1 : 0;
        time_out--;
    }
    if (time_out == 0) {
        HDPRINTF(ah, HAL_DBG_EEPROM,
            "%s: Timed out during OTP Status0 validation\n", __func__);
        return false;
    }

    addr = (AR_SREV_WASP(ah) || AR_SREV_SCORPION(ah)) ?
        OTP_STATUS1_EFUSE_READ_DATA_WASP : OTP_STATUS1_EFUSE_READ_DATA;
	if (!is_wifi) {
        addr = BTOTP_STATUS1_EFUSE_READ_DATA;
    }
    *data = OS_REG_READ(ah, addr);
    return true;
}

#ifdef AH_SUPPORT_WRITE_EEPROM

bool
ar9300_otp_write(struct ath_hal *ah, u_int off, u_int32_t data, bool is_wifi)
{
    u_int32_t addr_wr, addr_rd;
    u_int32_t wr_val;

#define AR9300_OTP_WRITE_DELAY 1000 /* was 10000 */
#define AR9300_BTOTP_WRITE_DELAY 100000

    if (AR_SREV_JET(ah)) {
        qcn5500_otp_write(ah, off, data, is_wifi);
        return true;
    }
    /* Power on LDO */
    addr_wr = (AR_SREV_WASP(ah) || AR_SREV_SCORPION(ah)) ?
        OTP_LDO_CONTROL_ENABLE_WASP : OTP_LDO_CONTROL_ENABLE;
	if (AR_SREV_JUPITER(ah) || AR_SREV_APHRODITE(ah)) {
        addr_wr = GLB_OTP_LDO_CONTROL_ENABLE;
    }
    OS_REG_WRITE(ah, addr_wr, 0x1);

    OS_DELAY(AR9300_OTP_WRITE_DELAY);

    if (AR_SREV_WASP(ah) || AR_SREV_SCORPION(ah)) {
        addr_rd = OTP_LDO_STATUS_POWER_ON_WASP;
        addr_wr = OTP_INTF0_EFUSE_WR_ENABLE_REG_V_WASP;
        wr_val = 0x10AD079; /* set 0x1 also works */
    } else {
		if (AR_SREV_JUPITER(ah) || AR_SREV_APHRODITE(ah)) {
            addr_rd = GLB_OTP_LDO_STATUS_POWER_ON;
		} else {
            addr_rd = OTP_LDO_STATUS_POWER_ON;
        }
		if (!is_wifi) {
            addr_wr = BTOTP_INTF0_EFUSE_WR_ENABLE_REG_V;
        } else {
            addr_wr = OTP_INTF0_EFUSE_WR_ENABLE_REG_V;
        }
        wr_val = 0x10AD079;
    }

    if (!(OS_REG_READ(ah, addr_rd) & 0x1)) {
        return false;
    }
    OS_REG_WRITE(ah, addr_wr, wr_val);

	if (!is_wifi) {
        OS_DELAY(AR9300_BTOTP_WRITE_DELAY);
	} else {
        OS_DELAY(AR9300_OTP_WRITE_DELAY);
    }

    if (AR_SREV_WASP(ah) || AR_SREV_SCORPION(ah)) {
        addr_wr = OTP_MEM_START_ADDRESS_WASP;
        addr_rd = OTP_STATUS0_OTP_SM_BUSY_WASP;
    } else {
        if (is_wifi) {
            addr_wr = OTP_MEM_START_ADDRESS;
            addr_rd = OTP_STATUS0_OTP_SM_BUSY;
		} else {
            addr_wr = BTOTP_MEM_START_ADDRESS;
            addr_rd = BTOTP_STATUS0_OTP_SM_BUSY;
		}
    }
    /* OTP is 32 bit addressable */
    addr_wr += off * 4;

    OS_REG_WRITE(ah, addr_wr, data);
	// test
	if (!is_wifi) {
        OS_DELAY(AR9300_BTOTP_WRITE_DELAY);
	} else {
        OS_DELAY(AR9300_OTP_WRITE_DELAY);
    }

	// test
    while (((OS_REG_READ(ah, addr_rd)) & 0x3) != 0) {
        OS_DELAY(AR9300_OTP_WRITE_DELAY);
    }

    /* Power Off LDO */
    if (AR_SREV_WASP(ah) || AR_SREV_SCORPION(ah)) {
        addr_wr = OTP_LDO_CONTROL_ENABLE_WASP;
        addr_rd = OTP_LDO_STATUS_POWER_ON_WASP;
	} else if (AR_SREV_JUPITER(ah) || AR_SREV_APHRODITE(ah)) {
        addr_wr = GLB_OTP_LDO_CONTROL_ENABLE;
        addr_rd = GLB_OTP_LDO_STATUS_POWER_ON;
    } else {
        addr_wr = OTP_LDO_CONTROL_ENABLE;
        addr_rd = OTP_LDO_STATUS_POWER_ON;
    }
    OS_REG_WRITE(ah, addr_wr, 0x0);
    OS_DELAY(AR9300_OTP_WRITE_DELAY);
    if ((OS_REG_READ(ah, addr_rd) & 0x1)) {
        return false;
    }
    OS_DELAY(AR9300_OTP_WRITE_DELAY);
    return true;
}

#endif /* AH_SUPPORT_WRITE_EEPROM */

HAL_STATUS
ar9300_get_rate_power_limit_from_eeprom(struct ath_hal *ah, u_int16_t freq,
                                        int8_t *max_rate_power, int8_t *min_rate_power)
{
    u_int8_t target_rate_power_limit_val_t2[ar9300_rate_size];
    int i;

    *max_rate_power = 0;
    *min_rate_power = AR9300_MAX_RATE_POWER;

    ar9300_set_target_power_from_eeprom(ah, freq, target_rate_power_limit_val_t2);

    for (i=0; i<ar9300_rate_size; i++) {
        if (target_rate_power_limit_val_t2[i] > *max_rate_power)
            *max_rate_power = target_rate_power_limit_val_t2[i];
        if (target_rate_power_limit_val_t2[i] < *min_rate_power)
            *min_rate_power = target_rate_power_limit_val_t2[i];
    }

    return HAL_OK;
}

#ifndef WIN32

static HAL_STATUS
ar9300_flash_map(struct ath_hal *ah)
{
#ifndef ART_BUILD
    struct ath_hal_9300 *ahp = AH9300(ah);
    if (ar9300_eep_data_in_flash(ah)) {
#if defined(AR9100) || defined(__NetBSD__)
    	ahp->ah_cal_mem = OS_REMAP(ah, AR9300_EEPROM_START_ADDR, AR9300_EEPROM_MAX);
#else
    	ahp->ah_cal_mem = OS_REMAP((uintptr_t)(AH_PRIVATE(ah)->ah_st),
        	(AR9300_EEPROM_MAX + AR9300_FLASH_CAL_START_OFFSET));
#endif
    	if (!ahp->ah_cal_mem) {
        	HDPRINTF(ah, HAL_DBG_EEPROM,
            	"%s: cannot remap eeprom region \n", __func__);
        	return HAL_EIO;
    	}
    }
#endif
     return HAL_OK;
}

bool
ar9300_flash_read(struct ath_hal *ah, u_int off, u_int16_t *data)
{
    struct ath_hal_9300 *ahp = AH9300(ah);

    *data = ((u_int16_t *)ahp->ah_cal_mem)[off];
    return true;
}

bool
ar9300_flash_write(struct ath_hal *ah, u_int off, u_int16_t data)
{
    struct ath_hal_9300 *ahp = AH9300(ah);

    ((u_int16_t *)ahp->ah_cal_mem)[off] = data;
    return true;
}
#endif /* WIN32 */

#ifdef UNUSED
#ifdef ART_BUILD
u_int16_t ar9300_caldata_type_detect(struct ath_hal *ah)
{
    struct ath_hal_9300 *ahp = AH9300(ah);
    u_int8_t value[2];
    u_int16_t *svalue;

    /*
     * Determine storage type.
     * If storage type is automatic, search for magic number??
     * If magic number is found record associated storage type.
     * If un-initialized number (0xffff or 0x0000) record
     * associated storage type.
     */
    /* NEED TO DO MORE WORK ON THIS */
    if (cal_data == CALDATA_AUTO) {
        svalue = (u_int16_t *) value;
        if (!ahp->ah_priv.priv.ah_eeprom_read(ah, 0, svalue)) {
            HDPRINTF(ah, HAL_DBG_EEPROM,
                "%s: Unable to read eeprom region \n", __func__);
            return false;
        }
        /*printf("Auto detect eeprom read  %x\n", *svalue);*/
        if ((*svalue == 0xa55a) || (*svalue == 0xffff)) {
            cal_data = CALDATA_EEPROM;
            /*printf("Calibration Data is in eeprom\n");*/
            goto Done_auto_storage_detect;
        }

        /*
         * If eeprom is NOT present on the board then read from eeprom
         * will return zero.
         * We can say a non-zero data of eeprom location 0 means eeprom
         * is present (may be corrupt).
         */
        if (*svalue != 0x0000) {
            cal_data = CALDATA_EEPROM;
            /*printf("Calibration Data is in eeprom\n");*/
            goto Done_auto_storage_detect;
        }
#ifdef MDK_AP
        {
            int fd;
            unsigned short data;
            if (!((fd = open("/dev/caldata", O_RDWR)) < 0)) {
                read(fd, value, 2);
                data = (value[0] & 0xff) | ((value[1] << 8) & 0xff00);
                /*printf("Auto detect flash read = 0x%x\n", data);*/
                if ((data == 0xa55a) || (data == 0xffff)) {
                    cal_data = CALDATA_FLASH;
                    /*printf("Calibration Data is in flash\n");*/
                    goto Done_auto_storage_detect;
                }
            }
        }
#endif
        /* Check for OTP is not implemented yet. */
        /*printf("TEST OTP\n");*/
    }

Done_auto_storage_detect:
    /*printf("cal_data type = %d\n", cal_data);*/

    return cal_data;
}

#endif /* end of ART_BUILD */
#endif

// myAtoi() function
u_int32_t myAtoi(u_int8_t *str)
{
    u_int32_t res = 0; // Initialize result
    u_int32_t i;

    // Iterate through all characters of input string and update result
    for (i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';

    // return result.
    return res;
}

#ifndef systime_t
typedef unsigned long systime_t;
#endif
HAL_STATUS
ar9300_eeprom_attach(struct ath_hal *ah)
{
    struct ath_hal_9300 *ahp = AH9300(ah);
    ar9300_eeprom_t *eep;
    u_int32_t res;
    u_int8_t num[5];
#ifdef AR5500_EMULATION
    ar9300_eeprom_t *mptr;
    int mdata_size;
#ifdef ART_BUILD
    ar5500_eeprom_t *mptr_jet;
#endif
#ifndef ART_BUILD
    systime_t current_system_time = jiffies;
#else
#if defined(AR9485_EMULATION) || defined(AR956X_EMULATION)
    systime_t current_system_time = OS_GET_TIMESTAMP();
#endif
#endif
#else
#if ATH_DRIVER_SIM
    ar9300_eeprom_t *mptr;
    int mdata_size;
#endif
#endif

    ahp->try_dram = 1;
    ahp->try_eeprom = 1;
    ahp->try_otp = 1;
#ifdef ATH_CAL_NAND_FLASH
    ahp->try_nand = 1;
#else
    ahp->try_flash = 1;
#endif
#ifdef AH_CAL_IN_FILE_HOST
    ahp->try_file = 1;
#endif
    ahp->calibration_data_source = calibration_data_none;
    ahp->calibration_data_source_address = 0;
    ahp->calibration_data_try = calibration_data_try;
    ahp->calibration_data_try_address = 0;
#ifdef __NetBSD__
    osprey_eeprom_template_preference(ar9300_eeprom_template_osprey_k31);
#endif

#ifndef WIN32
    /*
     * In case flash will be used for EEPROM. Otherwise ahp->ah_cal_mem
     * must be set to NULL or the real EEPROM address.
     */
    ar9300_flash_map(ah);
#endif
    /*
     * ###### This function always return NO SPUR.
     * This is not true for many board designs.
     * Does anyone use this?
     */
    AH_PRIVATE(ah)->ah_eeprom_get_spur_chan = ar9300_eeprom_get_spur_chan;

    AH_PRIVATE(ah)->ah_get_rate_power_limit_from_eeprom = ar9300_get_rate_power_limit_from_eeprom;
    ahp->ah_emu_eeprom = 1;
#ifdef AR5500_EMULATION
#ifndef ART_BUILD
    mptr = &ahp->ah_eeprom;
    mdata_size = ar9300_eeprom_struct_size();
    ar9300_eeprom_restore_something(ah, mptr, mdata_size);
    ahp->ah_eeprom.mac_addr[0] = 0x00;
    ahp->ah_eeprom.mac_addr[1] = 0x03;
    ahp->ah_eeprom.mac_addr[2] = 0x7F;
    ahp->ah_eeprom.mac_addr[3] = 0xBA;
#if !defined(ART_BUILD) || defined(AR9485_EMULATION) || defined(AR956X_EMULATION)
    /* Get random MAC to avoid Station address confilct */
    ahp->ah_eeprom.mac_addr[4] = (u_int8_t)((current_system_time >> 8) & 0xff);
    ahp->ah_eeprom.mac_addr[5] = (u_int8_t)(current_system_time & 0xff);
#else
    ahp->ah_eeprom.mac_addr[4] = 0xD0;
    ahp->ah_eeprom.mac_addr[5] = 0x00;
#endif
#else
    if (!AR_SREV_JET(ah)){
        mptr = &ahp->ah_eeprom;
        mdata_size = ar9300_eeprom_struct_size(ah);
        ar9300_eeprom_restore_something(ah, mptr, mdata_size);
    ahp->ah_eeprom.mac_addr[0] = 0x00;
    ahp->ah_eeprom.mac_addr[1] = 0x03;
    ahp->ah_eeprom.mac_addr[2] = 0x7F;
    ahp->ah_eeprom.mac_addr[3] = 0xBA;
#if !defined(ART_BUILD) || defined(AR9485_EMULATION) || defined(AR956X_EMULATION)
    /* Get random MAC to avoid Station address confilct */
    ahp->ah_eeprom.mac_addr[4] = (u_int8_t)((current_system_time >> 8) & 0xff);
    ahp->ah_eeprom.mac_addr[5] = (u_int8_t)(current_system_time & 0xff);
#else
    ahp->ah_eeprom.mac_addr[4] = 0xD0;
    ahp->ah_eeprom.mac_addr[5] = 0x00;
#endif
    }else{
        mptr_jet = &ahp->ah_eeprom_jet;
        mdata_size = ar9300_eeprom_struct_size(ah);
        ar9300_eeprom_restore_something(ah, mptr_jet, mdata_size);
    ahp->ah_eeprom_jet.mac_addr[0] = 0x00;
    ahp->ah_eeprom_jet.mac_addr[1] = 0x03;
    ahp->ah_eeprom_jet.mac_addr[2] = 0x7F;
    ahp->ah_eeprom_jet.mac_addr[3] = 0xBA;
#if !defined(ART_BUILD) || defined(AR9485_EMULATION) || defined(AR956X_EMULATION)
    /* Get random MAC to avoid Station address confilct */
    ahp->ah_eeprom_jet.mac_addr[4] = (u_int8_t)((current_system_time >> 8) & 0xff);
    ahp->ah_eeprom_jet.mac_addr[5] = (u_int8_t)(current_system_time & 0xff);
#else
    ahp->ah_eeprom_jet.mac_addr[4] = 0xD0;
    ahp->ah_eeprom_jet.mac_addr[5] = 0x00;
#endif
    }
#endif
#else
#if ATH_DRIVER_SIM
    if (1) { /* always true but avoids compile warning re unreachable code */
        ar9300_eeprom_t *mptr = &ahp->ah_eeprom;
        int mdata_size;
#ifndef ART_BUILD
        mdata_size = ar9300_eeprom_struct_size();
#else
        mdata_size = ar9300_eeprom_struct_size(ah);
#endif
        ar9300_eeprom_restore_something(ah, mptr, mdata_size);
        ahp->ah_eeprom.mac_addr[0] = 0x00;
        ahp->ah_eeprom.mac_addr[1] = 0x03;
        ahp->ah_eeprom.mac_addr[2] = 0x7F;
        ahp->ah_eeprom.mac_addr[3] = AHSIM_sim_info.sim_mach_id;
        ahp->ah_eeprom.mac_addr[4] = AHSIM_sim_info.sim_eng_id;
        ahp->ah_eeprom.mac_addr[5] = AH_PRIVATE(ah)->ah_hal_sim.sim_index;
        return HAL_OK;
    }
#endif
#endif

#if 0
/*#ifdef MDK_AP*/ /* MDK_AP is defined only in NART AP build */
    u_int8_t buffer[10];
    int caldata_check = 0;

    ar9300_calibration_data_read_flash(
        ah, FLASH_BASE_CALDATA_OFFSET, buffer, 4);
    printf("flash caldata:: %x\n", buffer[0]);
    if (buffer[0] != 0xff) {
        caldata_check = 1;
    }
    if (!caldata_check) {
        ar9300_eeprom_t *mptr;
        int mdata_size;
        if (AR_SREV_HORNET(ah)) {
            /* XXX: For initial testing */
            mptr = &ahp->ah_eeprom;
            mdata_size = ar9300_eeprom_struct_size();
            ahp->ah_eeprom = ar9300_template_ap121;
            ahp->ah_emu_eeprom = 1;
            /* need it to let art save in to flash ????? */
            calibration_data_source = calibration_data_flash;
        } else if (AR_SREV_WASP(ah)) {
            /* XXX: For initial testing */
            ath_hal_printf(ah, " wasp eep attach\n");
            mptr = &ahp->ah_eeprom;
            mdata_size = ar9300_eeprom_struct_size();
            ahp->ah_eeprom = ar9300_template_generic;
            ahp->ah_eeprom.mac_addr[0] = 0x00;
            ahp->ah_eeprom.mac_addr[1] = 0x03;
            ahp->ah_eeprom.mac_addr[2] = 0x7F;
            ahp->ah_eeprom.mac_addr[3] = 0xBA;
            ahp->ah_eeprom.mac_addr[4] = 0xD0;
            ahp->ah_eeprom.mac_addr[5] = 0x00;
            ahp->ah_emu_eeprom = 1;
            ahp->ah_eeprom.base_eep_header.txrx_mask = 0x33;
            ahp->ah_eeprom.base_eep_header.txrxgain = 0x10;
            /* need it to let art save in to flash ????? */
            calibration_data_source = calibration_data_flash;
        }
        return HAL_OK;
    }
#endif
    if (AR_SREV_HORNET(ah) || AR_SREV_WASP(ah) || AR_SREV_SCORPION(ah)
        || AR_SREV_HONEYBEE(ah) || AR_SREV_DRAGONFLY(ah) || AR_SREV_JET(ah)) {
        ahp->try_eeprom = 0;
    }

    if (AR_SREV_HONEYBEE(ah) || AR_SREV_DRAGONFLY(ah) || AR_SREV_JET(ah)) {
        ahp->try_otp = 0;
    }

    if (!ar9300_eeprom_restore(ah)) {
        return HAL_EIO;
    }
#ifndef ART_BUILD
    eep = &AH9300(ah)->ah_eeprom;
#else
    if (!AR_SREV_JET(ah))
        eep = &AH9300(ah)->ah_eeprom;
    else{
        int i;
        ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
        eep = (ar9300_eeprom_t *)&AH9300(ah)->ah_eeprom_jet;
        /* always clear all sticky write settings in FTM */
        for (i=0; i<128; i++)
        {
            if (eep_jet->configAddr[i] != -1)
                eep_jet->configAddr[i] = -1;
        }
    }
#endif
    if ((eep->custData[0] >=0 || eep->custData[0] <=9) ||
       (eep->custData[0] >= 'A' && eep->custData[0] <= 'F' )) {
            printk("\nART Version : ");

        if ((eep->custData[0]) >= 'A' && (eep->custData[0]) <= 'F' )
            printk("%d", eep->custData[0]-55);
        else
            printk("%d", eep->custData[0]-'0');

        memcpy(num, (unsigned char *)((eep->custData) + 1), 2);
        num[2] = '\0';
        res = myAtoi(num);
        if (res >= 0 && res <=99)
            printk(".%d", res);

        memcpy(num, (unsigned char * )((eep->custData) + 3), 3);
        num[3] = '\0';
        res = myAtoi(num);
        if (res >= 0 && res <=999)
            printk(".%d", res);

        printk("\nSW Image Version : ");

        if ((eep->custData[6]) >= 'A' && (eep->custData[6]) <= 'F' )
            printk("%d.", (eep->custData[6])-55);
        else
            printk("%d.", (eep->custData[6])-'0');

        memcpy(num, (unsigned char * )((eep->custData) + 7), 2);
        num[2] = '\0';
        printk("%d.", myAtoi(num));

        memcpy(num, (unsigned char * )((eep->custData) + 9), 2);
        num[2] = '\0';
        printk("%d.", myAtoi(num));

        memcpy(num, (unsigned char * )((eep->custData) + 11), 3);
        num[3] = '\0';
        printk("%d.", myAtoi(num));

        memcpy(num, (unsigned char * )((eep->custData) + 14), 2);
        num[2] = '\0';
        printk("%d", myAtoi(num));

        printk("\nBoard Revision : ");

        memcpy(num, (unsigned char *) ((eep->custData) + 16), 4);
        num[4] = '\0';
        printk("%s \n", num);
    }
    return HAL_OK;
}

u_int32_t
ar9300_eeprom_get(struct ath_hal_9300 *ahp, EEPROM_PARAM param)
{
    ar9300_eeprom_t *eep = &ahp->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &ahp->ah_eeprom_jet;
    struct ath_hal *ah = (struct ath_hal *) ahp;
#endif

#ifndef ART_BUILD
    OSPREY_BASE_EEP_HEADER *p_base = &eep->base_eep_header;
#else
    OSPREY_BASE_EEP_HEADER *p_base;
    if (!AR_SREV_JET(ah))
        p_base = &eep->base_eep_header;
    else
    {
        p_base = &eep_jet->base_eep_header;
        eep = (ar9300_eeprom_t *)eep_jet; /* same structure before 2G Modal header */
    }
#endif

    switch (param) {
#if NOTYET
    case EEP_NFTHRESH_5:
        return p_modal[0].noise_floor_thresh_ch[0];
    case EEP_NFTHRESH_2:
        return p_modal[1].noise_floor_thresh_ch[0];
#endif
    case EEP_MAC_LSW:
        return eep->mac_addr[0] << 8 | eep->mac_addr[1];
    case EEP_MAC_MID:
        return eep->mac_addr[2] << 8 | eep->mac_addr[3];
    case EEP_MAC_MSW:
        return eep->mac_addr[4] << 8 | eep->mac_addr[5];
    case EEP_REG_0:
        return p_base->reg_dmn[0];
    case EEP_REG_1:
        return p_base->reg_dmn[1];
    case EEP_OP_CAP:
        return p_base->device_cap;
    case EEP_OP_MODE:
        return p_base->op_cap_flags.op_flags;
    case EEP_OP_MISC_FLAG:
        return p_base->op_cap_flags.eepMisc;
    case EEP_RF_SILENT:
        return p_base->rf_silent;
#if NOTYET
    case EEP_OB_5:
        return p_modal[0].ob;
    case EEP_DB_5:
        return p_modal[0].db;
    case EEP_OB_2:
        return p_modal[1].ob;
    case EEP_DB_2:
        return p_modal[1].db;
    case EEP_MINOR_REV:
        return p_base->eeprom_version & AR9300_EEP_VER_MINOR_MASK;
#endif
    case EEP_TX_MASK:
        return (p_base->txrx_mask >> 4) & 0xf;
    case EEP_RX_MASK:
        return p_base->txrx_mask & 0xf;
#if NOTYET
    case EEP_FSTCLK_5G:
        return p_base->fast_clk5g;
    case EEP_RXGAIN_TYPE:
        return p_base->rx_gain_type;
#endif
    case EEP_DRIVE_STRENGTH:
#define AR9300_EEP_BASE_DRIVE_STRENGTH    0x1
        return p_base->misc_configuration & AR9300_EEP_BASE_DRIVE_STRENGTH;
    case EEP_DOUBLING_ENABLED:
        return ((p_base->feature_enable & 0x8) >> 3);
    case EEP_INTERNAL_REGULATOR:
        /* Bit 4 is internal regulator flag */
        return ((p_base->feature_enable & 0x10) >> 4);
    case EEP_SWREG:
        return (p_base->swreg);
    case EEP_PAPRD_ENABLED:
        /* Bit 5 is paprd flag */
        return ((p_base->feature_enable & 0x20) >> 5);
    case EEP_ANTDIV_control:
#ifndef ART_BUILD
        return (u_int32_t)(eep->base_ext1.ant_div_control);
#else
    if (!AR_SREV_JET(ah))
        return (u_int32_t)(eep->base_ext1.ant_div_control);
    else
        return (u_int32_t)(eep_jet->base_ext1.ant_div_control);
#endif
    case EEP_CHAIN_MASK_REDUCE:
        return ((p_base->misc_configuration >> 3) & 0x1);
    case EEP_OL_PWRCTRL:
        return 0;
     case EEP_DEV_TYPE:
        return p_base->device_type;
    default:
        HALASSERT(0);
        return 0;
    }
}

#ifdef AH_SUPPORT_WRITE_EEPROM

/*
 * ar9300_eeprom_set_param
 */
bool
ar9300_eeprom_set_param(struct ath_hal *ah, EEPROM_PARAM param, u_int32_t value)
{
    bool result = true;
#if 0
    struct ath_hal_9300 *ahp = AH9300(ah);
    ar9300_eeprom_t *eep = &ahp->ah_eeprom;
    BASE_EEPDEF_HEADER  *p_base = &eep->base_eep_header;
    int offset_rd = 0;
    int offset_chksum = 0;
    u_int16_t checksum;

    offset_rd =
        AR9300_EEPDEF_START_LOC +
        (int) (offsetof(struct base_eep_def_header, reg_dmn[0]) >> 1);
    offset_chksum =
        AR9300_EEPDEF_START_LOC +
        (offsetof(struct base_eep_def_header, checksum) >> 1);

    switch (param) {
    case EEP_REG_0:
        p_base->reg_dmn[0] = (u_int16_t) value;

        result = false;
        if (ar9300_eeprom_write(ah, offset_rd, (u_int16_t) value)) {
            ar9300_eeprom_def_update_checksum(ahp);
            checksum = p_base->checksum;
#if AH_BYTE_ORDER == AH_BIG_ENDIAN
            checksum  = SWAP16(checksum );
#endif
            if (ar9300_eeprom_write(ah, offset_chksum, checksum )) {
                result = true;
            }
}
        break;
    default:
        HALASSERT(0);
        break;
    }
#endif
    return result;
}
#endif /* #ifdef AH_SUPPORT_WRITE_EEPROM */


/******************************************************************************/
/*!
**  \brief EEPROM fixup code for INI values
**
** This routine provides a place to insert "fixup" code for specific devices
** that need to modify INI values based on EEPROM values, BEFORE the INI values
** are written.
** Certain registers in the INI file can only be written once without
** undesired side effects, and this provides a place for EEPROM overrides
** in these cases.
**
** This is called at attach time once.  It should not affect run time
** performance at all
**
**  \param ah       Pointer to HAL object (this)
**  \param p_eep_data Pointer to (filled in) eeprom data structure
**  \param reg      register being inspected on this call
**  \param value    value in INI file
**
**  \return Updated value for INI file.
*/
u_int32_t
ar9300_ini_fixup(struct ath_hal *ah, ar9300_eeprom_t *p_eep_data,
    u_int32_t reg, u_int32_t value)
{
    HDPRINTF(AH_NULL, HAL_DBG_UNMASKABLE,
        "ar9300_eeprom_def_ini_fixup: FIXME\n");
#if 0
    BASE_EEPDEF_HEADER  *p_base  = &(p_eep_data->base_eep_header);

    switch (AH_PRIVATE(ah)->ah_devid)
    {
    case AR9300_DEVID_AR9300_PCI:
        /*
        ** Need to set the external/internal regulator bit to the proper value.
        ** Can only write this ONCE.
        */

        if ( reg == 0x7894 )
        {
            /*
            ** Check for an EEPROM data structure of "0x0b" or better
            */

            HDPRINTF(ah, HAL_DBG_EEPROM, "ini VAL: %x  EEPROM: %x\n",
                     value, (p_base->version & 0xff));

            if ( (p_base->version & 0xff) > 0x0a) {
                HDPRINTF(ah, HAL_DBG_EEPROM,
                    "PWDCLKIND: %d\n", p_base->pwdclkind);
                value &= ~AR_AN_TOP2_PWDCLKIND;
                value |=
                    AR_AN_TOP2_PWDCLKIND &
                    (p_base->pwdclkind <<  AR_AN_TOP2_PWDCLKIND_S);
            } else {
                HDPRINTF(ah, HAL_DBG_EEPROM, "PWDCLKIND Earlier Rev\n");
            }

            HDPRINTF(ah, HAL_DBG_EEPROM, "final ini VAL: %x\n", value);
        }
        break;

    }

    return (value);
#else
    return 0;
#endif
}

/*
 * Returns the interpolated y value corresponding to the specified x value
 * from the np ordered pairs of data (px,py).
 * The pairs do not have to be in any order.
 * If the specified x value is less than any of the px,
 * the returned y value is equal to the py for the lowest px.
 * If the specified x value is greater than any of the px,
 * the returned y value is equal to the py for the highest px.
 */
static int
interpolate(int32_t x, int32_t *px, int32_t *py, u_int16_t np)
{
    int ip = 0;
    int lx = 0, ly = 0, lhave = 0;
    int hx = 0, hy = 0, hhave = 0;
    int dx = 0;
    int y = 0;
    int bf, factor, plus;

    lhave = 0;
    hhave = 0;
    /*
     * identify best lower and higher x calibration measurement
     */
    for (ip = 0; ip < np; ip++) {
        dx = x - px[ip];
        /* this measurement is higher than our desired x */
        if (dx <= 0) {
            if (!hhave || dx > (x - hx)) {
                /* new best higher x measurement */
                hx = px[ip];
                hy = py[ip];
                hhave = 1;
            }
        }
        /* this measurement is lower than our desired x */
        if (dx >= 0) {
            if (!lhave || dx < (x - lx)) {
                /* new best lower x measurement */
                lx = px[ip];
                ly = py[ip];
                lhave = 1;
            }
        }
    }
    /* the low x is good */
    if (lhave) {
        /* so is the high x */
        if (hhave) {
            /* they're the same, so just pick one */
            if (hx == lx) {
                y = ly;
            } else {
                /* interpolate with round off */
                bf = (2 * (hy - ly) * (x - lx)) / (hx - lx);
                plus = (bf % 2);
                factor = bf / 2;
                y = ly + factor + plus;
            }
        } else {
            /* only low is good, use it */
            y = ly;
        }
    } else if (hhave) {
        /* only high is good, use it */
        y = hy;
    } else {
        /* nothing is good,this should never happen unless np=0, ????  */
        y = -(1 << 30);
    }

    return y;
}

u_int8_t
ar9300_eeprom_get_legacy_trgt_pwr(struct ath_hal *ah, u_int16_t rate_index,
    u_int16_t freq, bool is_2ghz)
{
    u_int16_t            num_piers, i;
    int32_t              target_power_array[OSPREY_NUM_5G_20_TARGET_POWERS];
    int32_t              freq_array[OSPREY_NUM_5G_20_TARGET_POWERS];
    u_int8_t             *p_freq_bin;
    ar9300_eeprom_t      *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t      *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
    CAL_TARGET_POWER_LEG *p_eeprom_target_pwr;

    if (is_2ghz) {
        num_piers = OSPREY_NUM_2G_20_TARGET_POWERS;
#ifndef ART_BUILD
        p_eeprom_target_pwr = eep->cal_target_power_2g;
        p_freq_bin = eep->cal_target_freqbin_2g;
#else
        if (!AR_SREV_JET(ah)){
            p_eeprom_target_pwr = eep->cal_target_power_2g;
            p_freq_bin = eep->cal_target_freqbin_2g;
        }else{
            p_eeprom_target_pwr = eep_jet->cal_target_power_2g;
            p_freq_bin = eep_jet->cal_target_freqbin_2g;
        }
#endif
    } else {
        num_piers = OSPREY_NUM_5G_20_TARGET_POWERS;
#ifndef ART_BUILD
        p_eeprom_target_pwr = eep->cal_target_power_5g;
        p_freq_bin = eep->cal_target_freqbin_5g;
#else
        if (!AR_SREV_JET(ah)){
            p_eeprom_target_pwr = eep->cal_target_power_5g;
            p_freq_bin = eep->cal_target_freqbin_5g;
        }else{
            p_eeprom_target_pwr = eep_jet->cal_target_power_5g;
            p_freq_bin = eep_jet->cal_target_freqbin_5g;
        }
#endif
   }

    /*
     * create array of channels and targetpower from
     * targetpower piers stored on eeprom
     */
    for (i = 0; i < num_piers; i++) {
        freq_array[i] = FBIN2FREQ(p_freq_bin[i], is_2ghz);
        target_power_array[i] = p_eeprom_target_pwr[i].t_pow2x[rate_index];
    }

    /* interpolate to get target power for given frequency */
    return
        ((u_int8_t)interpolate(
            (int32_t)freq, freq_array, target_power_array, num_piers));
}

u_int8_t
ar9300_eeprom_get_ht20_trgt_pwr(struct ath_hal *ah, u_int16_t rate_index,
    u_int16_t freq, bool is_2ghz)
{
    u_int16_t               num_piers, i;
    int32_t                 target_power_array[OSPREY_NUM_5G_20_TARGET_POWERS];
    int32_t                 freq_array[OSPREY_NUM_5G_20_TARGET_POWERS];
    u_int8_t                *p_freq_bin;
#ifndef ART_BUILD
    ar9300_eeprom_t         *eep = &AH9300(ah)->ah_eeprom;
    JET_CAL_TARGET_POWER_HT *p_eeprom_target_pwr;
#else
    ar9300_eeprom_t         *eep = &AH9300(ah)->ah_eeprom;
    ar5500_eeprom_t         *eep_jet = &AH9300(ah)->ah_eeprom_jet;
    OSP_CAL_TARGET_POWER_HT *p_eeprom_target_pwr;
    JET_CAL_TARGET_POWER_HT *p_eeprom_target_pwr_jet;
#endif
#ifndef ART_BUILD
    if (is_2ghz) {
        num_piers = OSPREY_NUM_2G_20_TARGET_POWERS;
        p_eeprom_target_pwr = eep->cal_target_power_2g_ht20;
        p_freq_bin = eep->cal_target_freqbin_2g_ht20;
    } else {
        num_piers = OSPREY_NUM_5G_20_TARGET_POWERS;
        p_eeprom_target_pwr = eep->cal_target_power_5g_ht20;
        p_freq_bin = eep->cal_target_freqbin_5g_ht20;
    }

    /*
     * create array of channels and targetpower from
     * targetpower piers stored on eeprom
     */
    for (i = 0; i < num_piers; i++) {
        freq_array[i] = FBIN2FREQ(p_freq_bin[i], is_2ghz);
        target_power_array[i] = p_eeprom_target_pwr[i].t_pow2x[rate_index];
    }
#else
    if (is_2ghz) {
        num_piers = OSPREY_NUM_2G_20_TARGET_POWERS;
        if (!AR_SREV_JET(ah)){
            p_eeprom_target_pwr = eep->cal_target_power_2g_ht20;
            p_freq_bin = eep->cal_target_freqbin_2g_ht20;
        }
        else{
            p_eeprom_target_pwr_jet = eep_jet->cal_target_power_2g_ht20;
            p_freq_bin = eep_jet->cal_target_freqbin_2g_ht20;
        }
    } else {
        num_piers = OSPREY_NUM_5G_20_TARGET_POWERS;
        if (!AR_SREV_JET(ah)){
            p_eeprom_target_pwr = eep->cal_target_power_5g_ht20;
            p_freq_bin = eep->cal_target_freqbin_5g_ht20;
        }
        else{
            p_eeprom_target_pwr_jet = eep_jet->cal_target_power_5g_ht20;
            p_freq_bin = eep_jet->cal_target_freqbin_5g_ht20;
        }
    }

    /*
     * create array of channels and targetpower from
     * targetpower piers stored on eeprom
     */
    for (i = 0; i < num_piers; i++) {
        freq_array[i] = FBIN2FREQ(p_freq_bin[i], is_2ghz);
        if (!AR_SREV_JET(ah)){
            if (rate_index < 14)
                target_power_array[i] = p_eeprom_target_pwr[i].t_pow2x[rate_index];
        }
        else {
            target_power_array[i] = p_eeprom_target_pwr_jet[i].t_pow2x[rate_index];
        }
    }

#endif
    /* interpolate to get target power for given frequency */
    return
        ((u_int8_t)interpolate(
            (int32_t)freq, freq_array, target_power_array, num_piers));
}

u_int8_t
ar9300_eeprom_get_ht40_trgt_pwr(struct ath_hal *ah, u_int16_t rate_index,
    u_int16_t freq, bool is_2ghz)
{
    u_int16_t               num_piers, i;
    int32_t                 target_power_array[OSPREY_NUM_5G_40_TARGET_POWERS];
    int32_t                 freq_array[OSPREY_NUM_5G_40_TARGET_POWERS];
    u_int8_t                *p_freq_bin;
#ifndef ART_BUILD
    ar9300_eeprom_t         *eep = &AH9300(ah)->ah_eeprom;
    JET_CAL_TARGET_POWER_HT *p_eeprom_target_pwr;
#else
    ar9300_eeprom_t         *eep = &AH9300(ah)->ah_eeprom;
    ar5500_eeprom_t         *eep_jet = &AH9300(ah)->ah_eeprom_jet;
    OSP_CAL_TARGET_POWER_HT *p_eeprom_target_pwr;
    JET_CAL_TARGET_POWER_HT *p_eeprom_target_pwr_jet;
#endif
#ifndef ART_BUILD
    if (is_2ghz) {
        num_piers = OSPREY_NUM_2G_40_TARGET_POWERS;
        p_eeprom_target_pwr = eep->cal_target_power_2g_ht40;
        p_freq_bin = eep->cal_target_freqbin_2g_ht40;
    } else {
        num_piers = OSPREY_NUM_5G_40_TARGET_POWERS;
        p_eeprom_target_pwr = eep->cal_target_power_5g_ht40;
        p_freq_bin = eep->cal_target_freqbin_5g_ht40;
    }

    /*
     * create array of channels and targetpower from
     * targetpower piers stored on eeprom
     */
    for (i = 0; i < num_piers; i++) {
        freq_array[i] = FBIN2FREQ(p_freq_bin[i], is_2ghz);
        target_power_array[i] = p_eeprom_target_pwr[i].t_pow2x[rate_index];
    }
#else
    if (is_2ghz) {
        num_piers = OSPREY_NUM_2G_40_TARGET_POWERS;
        if (!AR_SREV_JET(ah)){
            p_eeprom_target_pwr = eep->cal_target_power_2g_ht40;
            p_freq_bin = eep->cal_target_freqbin_2g_ht40;
        }
        else{
            p_eeprom_target_pwr_jet = eep_jet->cal_target_power_2g_ht40;
            p_freq_bin = eep_jet->cal_target_freqbin_2g_ht40;
        }
    } else {
        num_piers = OSPREY_NUM_5G_40_TARGET_POWERS;
        if (!AR_SREV_JET(ah)){
            p_eeprom_target_pwr = eep->cal_target_power_5g_ht40;
            p_freq_bin = eep->cal_target_freqbin_5g_ht40;
        }
        else{
            p_eeprom_target_pwr_jet = eep_jet->cal_target_power_5g_ht40;
            p_freq_bin = eep_jet->cal_target_freqbin_5g_ht40;
        }
    }

    /*
     * create array of channels and targetpower from
     * targetpower piers stored on eeprom
     */
    for (i = 0; i < num_piers; i++) {
        freq_array[i] = FBIN2FREQ(p_freq_bin[i], is_2ghz);
        if (!AR_SREV_JET(ah)){
            if (rate_index < 14)
                target_power_array[i] = p_eeprom_target_pwr[i].t_pow2x[rate_index];
        }
        else {
            target_power_array[i] = p_eeprom_target_pwr_jet[i].t_pow2x[rate_index];
        }
    }

#endif
    /* interpolate to get target power for given frequency */
    return
        ((u_int8_t)interpolate(
            (int32_t)freq, freq_array, target_power_array, num_piers));
}

u_int8_t
ar9300_eeprom_get_cck_trgt_pwr(struct ath_hal *ah, u_int16_t rate_index,
    u_int16_t freq)
{
    u_int16_t            num_piers = OSPREY_NUM_2G_CCK_TARGET_POWERS, i;
    int32_t              target_power_array[OSPREY_NUM_2G_CCK_TARGET_POWERS];
    int32_t              freq_array[OSPREY_NUM_2G_CCK_TARGET_POWERS];
#ifndef ART_BUILD
    ar9300_eeprom_t      *eep = &AH9300(ah)->ah_eeprom;
    u_int8_t             *p_freq_bin = eep->cal_target_freqbin_cck;
    CAL_TARGET_POWER_LEG *p_eeprom_target_pwr = eep->cal_target_power_cck;
#else
    ar9300_eeprom_t      *eep = &AH9300(ah)->ah_eeprom;
    ar5500_eeprom_t      *eep_jet = &AH9300(ah)->ah_eeprom_jet;
    u_int8_t             *p_freq_bin;
    CAL_TARGET_POWER_LEG *p_eeprom_target_pwr;
    if (!AR_SREV_JET(ah))
    {
        p_freq_bin = eep->cal_target_freqbin_cck;
        p_eeprom_target_pwr = eep->cal_target_power_cck;
    }
    else
    {
        p_freq_bin = eep_jet->cal_target_freqbin_cck;
        p_eeprom_target_pwr = eep_jet->cal_target_power_cck;
    }

#endif
    /*
     * create array of channels and targetpower from
     * targetpower piers stored on eeprom
     */
    for (i = 0; i < num_piers; i++) {
        freq_array[i] = FBIN2FREQ(p_freq_bin[i], 1);
        target_power_array[i] = p_eeprom_target_pwr[i].t_pow2x[rate_index];
    }

    /* interpolate to get target power for given frequency */
    return
        ((u_int8_t)interpolate(
            (int32_t)freq, freq_array, target_power_array, num_piers));
}

/*
 * Set tx power registers to array of values passed in
 */
int
ar9300_transmit_power_reg_write(struct ath_hal *ah, u_int8_t *p_pwr_array)
{
#define POW_SM(_r, _s)     (((_r) & 0x3f) << (_s))
    /* make sure forced gain is not set */
#if 0
    field_write("force_dac_gain", 0);
    OS_REG_WRITE(ah, 0xa3f8, 0);
    field_write("force_tx_gain", 0);
#endif

    OS_REG_WRITE(ah, AR_PHY_TX_FORCED_GAIN, 0);

    /* Write the OFDM power per rate set */
    /* 6 (LSB), 9, 12, 18 (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE1,
        POW_SM(p_pwr_array[ALL_TARGET_LEGACY_6_24], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_6_24], 16)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_6_24],  8)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_6_24],  0)
    );
    /* 24 (LSB), 36, 48, 54 (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE2,
        POW_SM(p_pwr_array[ALL_TARGET_LEGACY_54], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_48], 16)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_36],  8)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_6_24],  0)
    );

    /* Write the CCK power per rate set */
    /* 1L (LSB), reserved, 2L, 2S (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE3,
        POW_SM(p_pwr_array[ALL_TARGET_LEGACY_1L_5L], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_1L_5L],  16)
/*          | POW_SM(tx_power_times2,  8)*/ /* this is reserved for Osprey */
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_1L_5L],   0)
    );
    /* 5.5L (LSB), 5.5S, 11L, 11S (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE4,
        POW_SM(p_pwr_array[ALL_TARGET_LEGACY_11S], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_11L], 16)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_5S],  8)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_1L_5L],  0)
    );

	/* write the power for duplicated frames - HT40 */
	/* dup40_cck (LSB), dup40_ofdm, ext20_cck, ext20_ofdm  (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE9,
        POW_SM(p_pwr_array[ALL_TARGET_LEGACY_6_24], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_1L_5L], 16)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_6_24],  8)
          | POW_SM(p_pwr_array[ALL_TARGET_LEGACY_1L_5L],  0)
    );

    /* Write the HT20 power per rate set */
    /* 0/8/16/24 (LSB), 1-3/9-11/17-19/25-27, 4, 5 (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE5,
        POW_SM(p_pwr_array[ALL_TARGET_HT20_5], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_4],  16)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_1_3_9_11_17_19_25_27],  8)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_0_8_16_24],   0)
    );

    /* 6 (LSB), 7, 12, 13 (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE6,
        POW_SM(p_pwr_array[ALL_TARGET_HT20_13], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_12],  16)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_7],  8)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_6],   0)
    );

    /* 14 (LSB), 15, 20, 21 */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE10,
        POW_SM(p_pwr_array[ALL_TARGET_HT20_21], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_20],  16)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_15],  8)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_14],   0)
    );

    /* Mixed HT20 and HT40 rates */
    /* HT20 22 (LSB), HT20 23, HT40 22, HT40 23 (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE11,
        POW_SM(p_pwr_array[ALL_TARGET_HT40_23], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_22],  16)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_23],   8)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_22],   0)
    );

    /*JET: Check with Sujatha on this.In Hal phy diff, changes are not there*/
    /* 28 (LSB), 29, 30, 31 */
    if (AR_SREV_JET(ah))
    {
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE13,
        POW_SM(p_pwr_array[ALL_TARGET_HT20_31], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_30],  16)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_29],   8)
          | POW_SM(p_pwr_array[ALL_TARGET_HT20_28],   0)
    );
    }
    /* Write the HT40 power per rate set */
    /* correct PAR difference between HT40 and HT20/LEGACY */
    /* 0/8/16/24 (LSB), 1-3/9-11/17-19/25-27, 4, 5 (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE7,
        POW_SM(p_pwr_array[ALL_TARGET_HT40_5], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_4],  16)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_1_3_9_11_17_19_25_27],  8)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_0_8_16_24],   0)
    );

    /* 6 (LSB), 7, 12, 13 (MSB) */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE8,
        POW_SM(p_pwr_array[ALL_TARGET_HT40_13], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_12],  16)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_7], 8)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_6], 0)
    );

    /* 14 (LSB), 15, 20, 21 */
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE12,
        POW_SM(p_pwr_array[ALL_TARGET_HT40_21], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_20],  16)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_15],   8)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_14],   0)
    );

    /*JET: Check with Sujatha on this.In Hal phy diff, changes are not there*/
    /* 28 (LSB), 29, 30, 31 */
    if (AR_SREV_JET(ah))
    {
    OS_REG_WRITE(ah, AR_PHY_POWERTX_RATE14,
        POW_SM(p_pwr_array[ALL_TARGET_HT40_31], 24)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_30],  16)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_29],   8)
          | POW_SM(p_pwr_array[ALL_TARGET_HT40_28],   0)
    );
    }

    return 0;
#undef POW_SM
}

static void
ar9300_selfgen_tpc_reg_write(struct ath_hal *ah, HAL_CHANNEL_INTERNAL *chan,
                             u_int8_t *p_pwr_array)
{
    u_int32_t tpc_reg_val;

    /* Set the target power values for self generated frames (ACK,RTS/CTS) to
     * be within limits. This is just a safety measure.With per packet TPC mode
     * enabled the target power value used with self generated frames will be
     * MIN( TPC reg, BB_powertx_rate register)
     */

    if (IS_CHAN_2GHZ(chan)) {
        tpc_reg_val = (SM(p_pwr_array[ALL_TARGET_LEGACY_1L_5L], AR_TPC_ACK) |
                       SM(p_pwr_array[ALL_TARGET_LEGACY_1L_5L], AR_TPC_CTS) |
                       SM(0x3f, AR_TPC_CHIRP) |
                       SM(0x3f, AR_TPC_RPT));
    } else {
        tpc_reg_val = (SM(p_pwr_array[ALL_TARGET_LEGACY_6_24], AR_TPC_ACK) |
                       SM(p_pwr_array[ALL_TARGET_LEGACY_6_24], AR_TPC_CTS) |
                       SM(0x3f, AR_TPC_CHIRP) |
                       SM(0x3f, AR_TPC_RPT));
    }
    OS_REG_WRITE(ah, AR_TPC, tpc_reg_val);
}

void
ar9300_set_target_power_from_eeprom(struct ath_hal *ah, u_int16_t freq,
    u_int8_t *target_power_val_t2)
{
    /* hard code for now, need to get from eeprom struct */
    u_int8_t ht40_power_inc_for_pdadc = 0;
    bool  is_2ghz = 0;

    if (freq < 4000) {
        is_2ghz = 1;
    }

    target_power_val_t2[ALL_TARGET_LEGACY_6_24] =
        ar9300_eeprom_get_legacy_trgt_pwr(
            ah, LEGACY_TARGET_RATE_6_24, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_LEGACY_36] =
        ar9300_eeprom_get_legacy_trgt_pwr(
            ah, LEGACY_TARGET_RATE_36, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_LEGACY_48] =
        ar9300_eeprom_get_legacy_trgt_pwr(
            ah, LEGACY_TARGET_RATE_48, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_LEGACY_54] =
        ar9300_eeprom_get_legacy_trgt_pwr(
            ah, LEGACY_TARGET_RATE_54, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_LEGACY_1L_5L] =
        ar9300_eeprom_get_cck_trgt_pwr(
            ah, LEGACY_TARGET_RATE_1L_5L, freq);
    target_power_val_t2[ALL_TARGET_LEGACY_5S] =
        ar9300_eeprom_get_cck_trgt_pwr(
            ah, LEGACY_TARGET_RATE_5S, freq);
    target_power_val_t2[ALL_TARGET_LEGACY_11L] =
        ar9300_eeprom_get_cck_trgt_pwr(
            ah, LEGACY_TARGET_RATE_11L, freq);
    target_power_val_t2[ALL_TARGET_LEGACY_11S] =
        ar9300_eeprom_get_cck_trgt_pwr(
            ah, LEGACY_TARGET_RATE_11S, freq);
    target_power_val_t2[ALL_TARGET_HT20_0_8_16_24] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_0_8_16_24, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_1_3_9_11_17_19_25_27] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_1_3_9_11_17_19_25_27, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_4] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_4, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_5] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_5, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_6] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_6, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_7] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_7, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_12] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_12, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_13] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_13, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_14] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_14, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_15] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_15, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_20] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_20, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_21] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_21, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_22] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_22, freq, is_2ghz);
    target_power_val_t2[ALL_TARGET_HT20_23] =
        ar9300_eeprom_get_ht20_trgt_pwr(
            ah, HT_TARGET_RATE_23, freq, is_2ghz);

    if(AR_SREV_JET(ah)) {
	    target_power_val_t2[ALL_TARGET_HT20_28] =
	        ar9300_eeprom_get_ht20_trgt_pwr(
	            ah, HT_TARGET_RATE_28, freq, is_2ghz);
	    target_power_val_t2[ALL_TARGET_HT20_29] =
	        ar9300_eeprom_get_ht20_trgt_pwr(
	            ah, HT_TARGET_RATE_29, freq, is_2ghz);
	    target_power_val_t2[ALL_TARGET_HT20_30] =
	        ar9300_eeprom_get_ht20_trgt_pwr(
	            ah, HT_TARGET_RATE_30, freq, is_2ghz);
	    target_power_val_t2[ALL_TARGET_HT20_31] =
	        ar9300_eeprom_get_ht20_trgt_pwr(
	            ah, HT_TARGET_RATE_31, freq, is_2ghz);
    }

    target_power_val_t2[ALL_TARGET_HT40_0_8_16_24] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_0_8_16_24, freq, is_2ghz) +
        ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_1_3_9_11_17_19_25_27] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_1_3_9_11_17_19_25_27, freq, is_2ghz) +
        ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_4] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_4, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_5] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_5, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_6] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_6, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_7] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_7, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_12] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_12, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_13] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_13, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_14] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_14, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_15] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_15, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_20] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_20, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_21] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_21, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_22] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_22, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    target_power_val_t2[ALL_TARGET_HT40_23] =
        ar9300_eeprom_get_ht40_trgt_pwr(
            ah, HT_TARGET_RATE_23, freq, is_2ghz) + ht40_power_inc_for_pdadc;

    if(AR_SREV_JET(ah)) {
	    target_power_val_t2[ALL_TARGET_HT40_28] =
	        ar9300_eeprom_get_ht40_trgt_pwr(
	            ah, HT_TARGET_RATE_28, freq, is_2ghz) + ht40_power_inc_for_pdadc;
	    target_power_val_t2[ALL_TARGET_HT40_29] =
	        ar9300_eeprom_get_ht40_trgt_pwr(
	            ah, HT_TARGET_RATE_29, freq, is_2ghz) + ht40_power_inc_for_pdadc;
	    target_power_val_t2[ALL_TARGET_HT40_30] =
	        ar9300_eeprom_get_ht40_trgt_pwr(
	            ah, HT_TARGET_RATE_30, freq, is_2ghz) + ht40_power_inc_for_pdadc;
	    target_power_val_t2[ALL_TARGET_HT40_31] =
	        ar9300_eeprom_get_ht40_trgt_pwr(
	            ah, HT_TARGET_RATE_31, freq, is_2ghz) + ht40_power_inc_for_pdadc;
    }

#ifdef AH_DEBUG
    {
        int  i = 0;

        HDPRINTF(ah, HAL_DBG_EEPROM, "%s: APPLYING TARGET POWERS\n", __func__);
        while (i < ar9300_rate_size) {
            HDPRINTF(ah, HAL_DBG_EEPROM, "%s: TPC[%02d] 0x%08x ",
                     __func__, i, target_power_val_t2[i]);
            i++;
			if (i == ar9300_rate_size) {
                break;
			}
            HDPRINTF(ah, HAL_DBG_EEPROM, "%s: TPC[%02d] 0x%08x ",
                     __func__, i, target_power_val_t2[i]);
            i++;
			if (i == ar9300_rate_size) {
                break;
			}
            HDPRINTF(ah, HAL_DBG_EEPROM, "%s: TPC[%02d] 0x%08x ",
                     __func__, i, target_power_val_t2[i]);
            i++;
			if (i == ar9300_rate_size) {
                break;
			}
            HDPRINTF(ah, HAL_DBG_EEPROM, "%s: TPC[%02d] 0x%08x \n",
                     __func__, i, target_power_val_t2[i]);
            i++;
        }
    }
#endif
}

u_int16_t *ar9300_regulatory_domain_get(struct ath_hal *ah)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    return eep->base_eep_header.reg_dmn;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
    return eep->base_eep_header.reg_dmn;
#else
    if (!AR_SREV_JET(ah))
        return eep->base_eep_header.reg_dmn;
    else
        return eep_jet->base_eep_header.reg_dmn;
#endif
}


int32_t
ar9300_eeprom_write_enable_gpio_get(struct ath_hal *ah)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
    return eep->base_eep_header.eeprom_write_enable_gpio;
#else
    if (!AR_SREV_JET(ah))
        return eep->base_eep_header.eeprom_write_enable_gpio;
    else
        return eep_jet->base_eep_header.eeprom_write_enable_gpio;
#endif
}

int32_t
ar9300_wlan_disable_gpio_get(struct ath_hal *ah)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
    return eep->base_eep_header.wlan_disable_gpio;
#else
    if (!AR_SREV_JET(ah))
        return eep->base_eep_header.wlan_disable_gpio;
    else
        return eep_jet->base_eep_header.wlan_disable_gpio;
#endif
}

int32_t
ar9300_wlan_led_gpio_get(struct ath_hal *ah)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
    return eep->base_eep_header.wlan_led_gpio;
#else
    if (!AR_SREV_JET(ah))
        return eep->base_eep_header.wlan_led_gpio;
    else
        return eep_jet->base_eep_header.wlan_led_gpio;
#endif
}

int32_t
ar9300_rx_band_select_gpio_get(struct ath_hal *ah)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
    return eep->base_eep_header.rx_band_select_gpio;
#else
    if (!AR_SREV_JET(ah))
        return eep->base_eep_header.rx_band_select_gpio;
    else
        return eep_jet->base_eep_header.rx_band_select_gpio;
#endif
}

/*
 * since valid noise floor values are negative, returns 1 on error
 */
int32_t
ar9300_noise_floor_cal_or_power_get(struct ath_hal *ah, int32_t frequency,
    int32_t ichain, bool use_cal)
{
    int     nf_use = 1; /* start with an error return value */
    int32_t fx[OSPREY_NUM_5G_CAL_PIERS + OSPREY_NUM_2G_CAL_PIERS];
    int32_t nf[OSPREY_NUM_5G_CAL_PIERS + OSPREY_NUM_2G_CAL_PIERS];
    int     nnf;
    int     is_2ghz;
    int     ipier, npier;
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
    u_int8_t        *p_cal_pier;
    OSP_CAL_DATA_PER_FREQ_OP_LOOP *p_cal_pier_struct;

    /*
     * check chain value
     */
    if (ichain < 0 || ichain >= JET_MAX_CHAINS) {
        return 1;
    }

    /* figure out which band we're using */
    is_2ghz = (frequency < 4000);
    if (is_2ghz) {
        npier = OSPREY_NUM_2G_CAL_PIERS;
#ifndef ART_BUILD
        p_cal_pier = eep->cal_freq_pier_2g;
        p_cal_pier_struct = eep->cal_pier_data_2g[ichain];
#else
        if(!AR_SREV_JET(ah)){
            p_cal_pier = eep->cal_freq_pier_2g;
            p_cal_pier_struct = eep->cal_pier_data_2g[ichain];
        }else{
            p_cal_pier = eep_jet->cal_freq_pier_2g;
            p_cal_pier_struct = eep_jet->cal_pier_data_2g[ichain];
        }
#endif
    } else {
        npier = OSPREY_NUM_5G_CAL_PIERS;
#ifndef ART_BUILD
        p_cal_pier = eep->cal_freq_pier_5g;
        p_cal_pier_struct = eep->cal_pier_data_5g[ichain];
#else
        if(!AR_SREV_JET(ah)){
            p_cal_pier = eep->cal_freq_pier_5g;
            p_cal_pier_struct = eep->cal_pier_data_5g[ichain];
        }else{
            p_cal_pier = eep_jet->cal_freq_pier_5g;
            p_cal_pier_struct = eep_jet->cal_pier_data_5g[ichain];
        }
#endif
    }
    /* look for valid noise floor values */
    nnf = 0;
    for (ipier = 0; ipier < npier; ipier++) {
        fx[nnf] = FBIN2FREQ(p_cal_pier[ipier], is_2ghz);
        nf[nnf] = use_cal ?
            p_cal_pier_struct[ipier].rx_noisefloor_cal :
            p_cal_pier_struct[ipier].rx_noisefloor_power;
        if (nf[nnf] < 0) {
            nnf++;
        }
    }
    /*
     * If we have some valid values, interpolate to find the value
     * at the desired frequency.
     */
    if (nnf > 0) {
        nf_use = interpolate(frequency, fx, nf, nnf);
    }

    return nf_use;
}

/*
 * Return the Rx NF offset for specific channel.
 * The values saved in EEPROM/OTP/Flash is converted through the following way:
 *     ((_p) - NOISE_PWR_DATA_OFFSET) << 2
 * So we need to convert back to the original values.
 */
int ar9300_get_rx_nf_offset(struct ath_hal *ah, HAL_CHANNEL *chan, int8_t *nf_pwr, int8_t *nf_cal) {
    HAL_CHANNEL_INTERNAL *ichan = ath_hal_checkchannel(ah, chan);
    int8_t rx_nf_pwr, rx_nf_cal;
    int i;
    //HALASSERT(ichan);

    /* Fill 0 if valid internal channel is not found */
    if (ichan == AH_NULL) {
	if(AR_SREV_JET(ah)) {
	        OS_MEMZERO(nf_pwr, sizeof(nf_pwr[0]) * JET_MAX_CHAINS);
	        OS_MEMZERO(nf_cal, sizeof(nf_cal[0]) * JET_MAX_CHAINS);
	} else {
		OS_MEMZERO(nf_pwr, sizeof(nf_pwr[0]) * OSPREY_MAX_CHAINS);
	        OS_MEMZERO(nf_cal, sizeof(nf_cal[0]) * OSPREY_MAX_CHAINS);
	}
        return -1;
    }

    for (i = 0; i < JET_MAX_CHAINS; i++) {

	    if( i == 3 && !AR_SREV_JET(ah))
		break;

	    if ((rx_nf_pwr = ar9300_noise_floor_cal_or_power_get(ah, ichan->channel, i, 0)) == 1) {
	        nf_pwr[i] = 0;
	    } else {
	        //printk("%s: raw nf_pwr[%d] = %d\n", __func__, i, rx_nf_pwr);
            nf_pwr[i] = NOISE_PWR_DBM_2_INT(rx_nf_pwr);
	    }

	    if ((rx_nf_cal = ar9300_noise_floor_cal_or_power_get(ah, ichan->channel, i, 1)) == 1) {
	        nf_cal[i] = 0;
	    } else {
	        //printk("%s: raw nf_cal[%d] = %d\n", __func__, i, rx_nf_cal);
            nf_cal[i] = NOISE_PWR_DBM_2_INT(rx_nf_cal);
	    }
    }

    return 0;
}

int32_t ar9300_rx_gain_index_get(struct ath_hal *ah)
{
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#else
    u_int8_t txrxgain_eep;
    ar9300_eeprom_t *eep =     &AH9300(ah)->ah_eeprom;
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif

#ifndef ART_BUILD
    return (eep->base_eep_header.txrxgain) & 0xf;        /* bits 3:0 */
#else

    if (!AR_SREV_JET(ah))
        txrxgain_eep = eep->base_eep_header.txrxgain;
    else
        txrxgain_eep = eep_jet->base_eep_header.txrxgain;

    return txrxgain_eep & 0xf;        /* bits 3:0 */
#endif
}


int32_t ar9300_tx_gain_index_get(struct ath_hal *ah)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif

#ifndef ART_BUILD
    return (eep->base_eep_header.txrxgain >> 4) & 0xf;    /* bits 7:4 */
#else
    if (!AR_SREV_JET(ah))
        return (eep->base_eep_header.txrxgain >> 4) & 0xf;    /* bits 7:4 */
    else
        return (eep_jet->base_eep_header.txrxgain >> 4) & 0xf;    /* bits 7:4 */

#endif
}

bool ar9300_internal_regulator_apply(struct ath_hal *ah)
{
    struct ath_hal_9300 *ahp = AH9300(ah);
    int internal_regulator = ar9300_eeprom_get(ahp, EEP_INTERNAL_REGULATOR);
    int reg_pmu1, reg_pmu2, reg_pmu1_set, reg_pmu2_set;
    u_int32_t reg_PMU1, reg_PMU2;
    unsigned long eep_addr;
    u_int32_t reg_val, reg_usb = 0, reg_pmu = 0;
    int usb_valid = 0, pmu_valid = 0;
    unsigned char pmu_refv;

    if (AR_SREV_JUPITER(ah) || AR_SREV_APHRODITE(ah)) {
        reg_PMU1 = AR_PHY_PMU1_JUPITER;
        reg_PMU2 = AR_PHY_PMU2_JUPITER;
    } else if (AR_SREV_DRAGONFLY(ah)) {
        reg_PMU1 = AR_PHY_PMU1_DRAGONFLY;
        reg_PMU2 = AR_PHY_PMU2_DRAGONFLY;
    } else if (AR_SREV_JET(ah)) {
        reg_PMU1 = QCN5500_PHY_PMU1;
        reg_PMU2 = QCN5500_PHY_PMU2;
    } else {
        reg_PMU1 = AR_PHY_PMU1;
        reg_PMU2 = AR_PHY_PMU2;
    }
    if (internal_regulator) {
        if (AR_SREV_HORNET(ah) || AR_SREV_POSEIDON(ah)) {
            if (AR_SREV_HORNET(ah)) {
                /* Read OTP first */
                for (eep_addr = 0x14; ; eep_addr -= 0x10) {

                    ar9300_otp_read(ah, eep_addr / 4, &reg_val, 1);

                    if ((reg_val & 0x80) == 0x80){
                        usb_valid = 1;
                        reg_usb = reg_val & 0x000000ff;
                    }

                    if ((reg_val & 0x80000000) == 0x80000000){
                        pmu_valid = 1;
                        reg_pmu = (reg_val & 0xff000000) >> 24;
                    }

                    if (eep_addr == 0x4) {
                        break;
                    }
                }

                if (pmu_valid) {
                    pmu_refv = reg_pmu & 0xf;
                } else {
                    pmu_refv = 0x8;
                }

                /*
                 * If (valid) {
                 *   Usb_phy_ctrl2_tx_cal_en -> 0
                 *   Usb_phy_ctrl2_tx_cal_sel -> 0
                 *   Usb_phy_ctrl2_tx_man_cal -> 0, 1, 3, 7 or 15 from OTP
                 * }
                 */
                if (usb_valid) {
                    OS_REG_RMW_FIELD(ah, 0x16c88, AR_PHY_CTRL2_TX_CAL_EN, 0x0);
                    OS_REG_RMW_FIELD(ah, 0x16c88, AR_PHY_CTRL2_TX_CAL_SEL, 0x0);
                    OS_REG_RMW_FIELD(ah, 0x16c88,
                        AR_PHY_CTRL2_TX_MAN_CAL, (reg_usb & 0xf));
                }

            } else {
                pmu_refv = 0x8;
            }
            /*#ifndef USE_HIF*/
            /* Follow the MDK settings for Hornet PMU.
             * my $pwd               = 0x0;
             * my $Nfdiv             = 0x3;  # xtal_freq = 25MHz
             * my $Nfdiv             = 0x4;  # xtal_freq = 40MHz
             * my $Refv              = 0x7;  # 0x5:1.22V; 0x8:1.29V
             * my $Gm1               = 0x3;  #Poseidon $Gm1=1
             * my $classb            = 0x0;
             * my $Cc                = 0x1;  #Poseidon $Cc=7
             * my $Rc                = 0x6;
             * my $ramp_slope        = 0x1;
             * my $Segm              = 0x3;
             * my $use_local_osc     = 0x0;
             * my $force_xosc_stable = 0x0;
             * my $Selfb             = 0x0;  #Poseidon $Selfb=1
             * my $Filterfb          = 0x3;  #Poseidon $Filterfb=0
             * my $Filtervc          = 0x0;
             * my $disc              = 0x0;
             * my $discdel           = 0x4;
             * my $spare             = 0x0;
             * $reg_PMU1 =
             *     $pwd | ($Nfdiv<<1) | ($Refv<<4) | ($Gm1<<8) |
             *     ($classb<<11) | ($Cc<<14) | ($Rc<<17) | ($ramp_slope<<20) |
             *     ($Segm<<24) | ($use_local_osc<<26) |
             *     ($force_xosc_stable<<27) | ($Selfb<<28) | ($Filterfb<<29);
             * $reg_PMU2 = $handle->reg_rd("ch0_PMU2");
             * $reg_PMU2 = ($reg_PMU2 & 0xfe3fffff) | ($Filtervc<<22);
             * $reg_PMU2 = ($reg_PMU2 & 0xe3ffffff) | ($discdel<<26);
             * $reg_PMU2 = ($reg_PMU2 & 0x1fffffff) | ($spare<<29);
             */
            if (ahp->clk_25mhz) {
                reg_pmu1_set = 0 |
                    (3 <<  1) | (pmu_refv << 4) | (3 <<  8) | (0 << 11) |
                    (1 << 14) | (6 << 17) | (1 << 20) | (3 << 24) |
                    (0 << 26) | (0 << 27) | (0 << 28) | (0 << 29);
            } else {
                if (AR_SREV_POSEIDON(ah)) {
                    reg_pmu1_set = 0 |
                        (5 <<  1) | (7 <<  4) | (2 <<  8) | (0 << 11) |
                        (2 << 14) | (6 << 17) | (1 << 20) | (3 << 24) |
                        (0 << 26) | (0 << 27) | (1 << 28) | (0 << 29) ;
                } else {
                    reg_pmu1_set = 0 |
                        (4 <<  1) | (7 <<  4) | (3 <<  8) | (0 << 11) |
                        (1 << 14) | (6 << 17) | (1 << 20) | (3 << 24) |
                        (0 << 26) | (0 << 27) | (0 << 28) | (0 << 29) ;
                }
            }
            OS_REG_RMW_FIELD(ah, reg_PMU2, AR_PHY_PMU2_PGM, 0x0);

            OS_REG_WRITE(ah, reg_PMU1, reg_pmu1_set);   /* 0x638c8376 */
            reg_pmu1 = OS_REG_READ(ah, reg_PMU1);
            while (reg_pmu1 != reg_pmu1_set) {
                OS_REG_WRITE(ah, reg_PMU1, reg_pmu1_set);  /* 0x638c8376 */
                OS_DELAY(10);
                reg_pmu1 = OS_REG_READ(ah, reg_PMU1);
            }

            reg_pmu2_set =
                 (OS_REG_READ(ah, reg_PMU2) & (~0xFFC00000)) | (4 << 26);
            OS_REG_WRITE(ah, reg_PMU2, reg_pmu2_set);
            reg_pmu2 = OS_REG_READ(ah, reg_PMU2);
            while (reg_pmu2 != reg_pmu2_set) {
                OS_REG_WRITE(ah, reg_PMU2, reg_pmu2_set);
                OS_DELAY(10);
                reg_pmu2 = OS_REG_READ(ah, reg_PMU2);
            }
            reg_pmu2_set =
                 (OS_REG_READ(ah, reg_PMU2) & (~0x00200000)) | (1 << 21);
            OS_REG_WRITE(ah, reg_PMU2, reg_pmu2_set);
            reg_pmu2 = OS_REG_READ(ah, reg_PMU2);
            while (reg_pmu2 != reg_pmu2_set) {
                OS_REG_WRITE(ah, reg_PMU2, reg_pmu2_set);
                OS_DELAY(10);
                reg_pmu2 = OS_REG_READ(ah, reg_PMU2);
            }
            /*#endif*/
        } else if (AR_SREV_JUPITER(ah) || AR_SREV_APHRODITE(ah)) {
            /* Internal regulator is ON. Write swreg register. */
            int swreg = ar9300_eeprom_get(ahp, EEP_SWREG);
            OS_REG_WRITE(ah, reg_PMU1, swreg);
        } else if (AR_SREV_DRAGONFLY(ah) || AR_SREV_JET(ah)) {
            /* Internal regulator is ON. Write swreg register. */
            /*
             * AP151: 0x633ec176,JET MDK: 0x633c8176,
             * PMU1 0x633c8176 -->1.231v,
             * 0x633c8186 -->1.260 1 step increase about 25mv
             */
            int swreg = ar9300_eeprom_get(ahp, EEP_SWREG);
            OS_REG_WRITE(ah, reg_PMU1, swreg);
            OS_REG_WRITE(ah, reg_PMU2, 0x10200000);
	} else if (AR_SREV_HONEYBEE_20(ah)) {
            /* Internal regulator is ON. Write swreg register. */
            int swreg = ar9300_eeprom_get(ahp, EEP_SWREG);
            OS_REG_WRITE(ah, reg_PMU1, swreg);
        } else {
            /* Internal regulator is ON. Write swreg register. */
            int swreg = ar9300_eeprom_get(ahp, EEP_SWREG);
            OS_REG_WRITE(ah, AR_RTC_REG_CONTROL1,
                         OS_REG_READ(ah, AR_RTC_REG_CONTROL1) &
                         (~AR_RTC_REG_CONTROL1_SWREG_PROGRAM));
            OS_REG_WRITE(ah, AR_RTC_REG_CONTROL0, swreg);
            /* Set REG_CONTROL1.SWREG_PROGRAM */
            OS_REG_WRITE(ah, AR_RTC_REG_CONTROL1,
                OS_REG_READ(ah, AR_RTC_REG_CONTROL1) |
                AR_RTC_REG_CONTROL1_SWREG_PROGRAM);
        }
    } else {
        if (AR_SREV_HORNET(ah) || AR_SREV_POSEIDON(ah)) {
            OS_REG_RMW_FIELD(ah, reg_PMU2, AR_PHY_PMU2_PGM, 0x0);
            reg_pmu2 = OS_REG_READ_FIELD(ah, reg_PMU2, AR_PHY_PMU2_PGM);
            while (reg_pmu2) {
                OS_DELAY(10);
                reg_pmu2 = OS_REG_READ_FIELD(ah, reg_PMU2, AR_PHY_PMU2_PGM);
            }
            OS_REG_RMW_FIELD(ah, reg_PMU1, AR_PHY_PMU1_PWD, 0x1);
            reg_pmu1 = OS_REG_READ_FIELD(ah, reg_PMU1, AR_PHY_PMU1_PWD);
            while (!reg_pmu1) {
                OS_DELAY(10);
                reg_pmu1 = OS_REG_READ_FIELD(ah, reg_PMU1, AR_PHY_PMU1_PWD);
            }
            OS_REG_RMW_FIELD(ah, reg_PMU2, AR_PHY_PMU2_PGM, 0x1);
            reg_pmu2 = OS_REG_READ_FIELD(ah, reg_PMU2, AR_PHY_PMU2_PGM);
            while (!reg_pmu2) {
                OS_DELAY(10);
                reg_pmu2 = OS_REG_READ_FIELD(ah, reg_PMU2, AR_PHY_PMU2_PGM);
            }
        } else if (AR_SREV_JUPITER(ah) || AR_SREV_APHRODITE(ah)) {
            OS_REG_RMW_FIELD(ah, reg_PMU1, AR_PHY_PMU1_PWD, 0x1);
        } else {
            OS_REG_WRITE(ah, AR_RTC_SLEEP_CLK,
                (OS_REG_READ(ah, AR_RTC_SLEEP_CLK) |
                AR_RTC_FORCE_SWREG_PRD | AR_RTC_PCIE_RST_PWDN_EN));
        }
    }

    return 0;
}

bool ar9300_drive_strength_apply(struct ath_hal *ah)
{
    struct ath_hal_9300 *ahp = AH9300(ah);
    int drive_strength;
    unsigned long reg;

    drive_strength = ar9300_eeprom_get(ahp, EEP_DRIVE_STRENGTH);
    if (drive_strength) {
        reg = OS_REG_READ(ah, AR_PHY_65NM_CH0_BIAS1);
        reg &= ~0x00ffffc0;
        reg |= 0x5 << 21;
        reg |= 0x5 << 18;
        reg |= 0x5 << 15;
        reg |= 0x5 << 12;
        reg |= 0x5 << 9;
        reg |= 0x5 << 6;
        OS_REG_WRITE(ah, AR_PHY_65NM_CH0_BIAS1, reg);

        reg = OS_REG_READ(ah, AR_PHY_65NM_CH0_BIAS2);
        reg &= ~0xffffffe0;
        reg |= 0x5 << 29;
        reg |= 0x5 << 26;
        reg |= 0x5 << 23;
        reg |= 0x5 << 20;
        reg |= 0x5 << 17;
        reg |= 0x5 << 14;
        reg |= 0x5 << 11;
        reg |= 0x5 << 8;
        reg |= 0x5 << 5;
        OS_REG_WRITE(ah, AR_PHY_65NM_CH0_BIAS2, reg);

        reg = OS_REG_READ(ah, AR_PHY_65NM_CH0_BIAS4);
        reg &= ~0xff800000;
        reg |= 0x5 << 29;
        reg |= 0x5 << 26;
        reg |= 0x5 << 23;
        OS_REG_WRITE(ah, AR_PHY_65NM_CH0_BIAS4, reg);
    }
    return 0;
}

int32_t ar9300_xpa_bias_level_get(struct ath_hal *ah, bool is_2ghz)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
    if (is_2ghz) {
        return eep->modal_header_2g.xpa_bias_lvl;
    } else {
        return eep->modal_header_5g.xpa_bias_lvl;
    }
#else
    if (is_2ghz) {
        if (!AR_SREV_JET(ah))
            return eep->modal_header_2g.xpa_bias_lvl;
        else
            return eep_jet->modal_header_2g.xpa_bias_lvl;

    } else {
        if (!AR_SREV_JET(ah))
            return eep->modal_header_5g.xpa_bias_lvl;
        else
            return eep_jet->modal_header_5g.xpa_bias_lvl;
    }

#endif

}

bool ar9300_xpa_bias_level_apply(struct ath_hal *ah, bool is_2ghz)
{
    /*
     * In ar9330 emu, we can't access radio registers,
     * merlin is used for radio part.
     */
#if !defined(AR9330_EMULATION) && !defined(AR9300_EMULATION) && !defined(AR9485_EMULATION) && !defined(AR956X_EMULATION)
    int bias;
    bias = ar9300_xpa_bias_level_get(ah, is_2ghz);

    if (AR_SREV_HORNET(ah) || AR_SREV_POSEIDON(ah) || AR_SREV_WASP(ah)) {
        OS_REG_RMW_FIELD(ah,
            AR_HORNET_CH0_TOP2, AR_HORNET_CH0_TOP2_XPABIASLVL, bias);
    } else if (AR_SREV_SCORPION(ah)) {
        OS_REG_RMW_FIELD(ah,
            AR_SCORPION_CH0_TOP, AR_SCORPION_CH0_TOP_XPABIASLVL, bias);
    } else if (AR_SREV_HONEYBEE(ah)) {
        OS_REG_RMW_FIELD(ah,
            AR_HONEYBEE_CH0_TOP2, AR_HONEYBEE_CH0_TOP2_XPABIASLVL, bias);
    } else if (AR_SREV_DRAGONFLY(ah)) {
        OS_REG_RMW_FIELD(ah,
            AR_DRAGONFLY_CH0_TOP2, AR_DRAGONFLY_CH0_TOP2_XPABIASLVL, bias);
    } else if (AR_SREV_JET(ah)) {
        OS_REG_RMW_FIELD(ah,
            QCN5500_CH0_TOP3, QCN5500_CH0_TOP3_XPABIASLVL, bias);
    } else if (AR_SREV_JUPITER(ah) || AR_SREV_APHRODITE(ah)) {
        OS_REG_RMW_FIELD(ah,
            AR_PHY_65NM_CH0_TOP_JUPITER, AR_PHY_65NM_CH0_TOP_XPABIASLVL, bias);
    } else {
        OS_REG_RMW_FIELD(ah,
            AR_PHY_65NM_CH0_TOP, AR_PHY_65NM_CH0_TOP_XPABIASLVL, bias);
        OS_REG_RMW_FIELD(ah,
            AR_PHY_65NM_CH0_THERM, AR_PHY_65NM_CH0_THERM_XPABIASLVL_MSB,
            bias >> 2);
        OS_REG_RMW_FIELD(ah,
            AR_PHY_65NM_CH0_THERM, AR_PHY_65NM_CH0_THERM_XPASHORT2GND, 1);
    }
#endif
    return 0;
}

#ifndef ART_BUILD
u_int32_t ar9300_ant_ctrl_common_get(struct ath_hal *ah, bool is_2ghz)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    if (is_2ghz) {
        return eep->modal_header_2g.ant_ctrl_common;
    } else {
        return eep->modal_header_5g.ant_ctrl_common;
    }
}
static u_int16_t
ar9300_switch_com_spdt_get(struct ath_hal *ah, bool is_2ghz)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    if (is_2ghz) {
        return eep->modal_header_2g.switchcomspdt;
    } else {
        return eep->modal_header_5g.switchcomspdt;
    }
}
u_int32_t ar9300_ant_ctrl_common2_get(struct ath_hal *ah, bool is_2ghz)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    if (is_2ghz) {
        return eep->modal_header_2g.ant_ctrl_common2;
    } else {
        return eep->modal_header_5g.ant_ctrl_common2;
    }
}

u_int16_t ar9300_ant_ctrl_chain_get(struct ath_hal *ah, int chain,
    bool is_2ghz)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    if (chain >= 0 && chain < JET_MAX_CHAINS) {
        if (is_2ghz) {
            return eep->modal_header_2g.ant_ctrl_chain[chain];
        } else {
            return eep->modal_header_5g.ant_ctrl_chain[chain];
        }
    }
    return 0;
}

/*
 * Select the usage of antenna via the RF switch.
 * Default values are loaded from eeprom.
 */
bool ar9300_ant_swcom_sel(struct ath_hal *ah, u_int8_t ops,
                        u_int32_t *common_tbl1, u_int32_t *common_tbl2)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    struct ath_hal_private  *ap  = AH_PRIVATE(ah);
    HAL_CHANNEL_INTERNAL    *curchan = ap->ah_curchan;
    enum {
        ANT_SELECT_OPS_GET,
        ANT_SELECT_OPS_SET,
    };

    if (AR_SREV_JUPITER(ah) || AR_SREV_SCORPION(ah))
        return false;

    if (!curchan)
        return false;

#define AR_SWITCH_TABLE_COM_ALL (0xffff)
#define AR_SWITCH_TABLE_COM_ALL_S (0)
#define AR_SWITCH_TABLE_COM2_ALL (0xffffff)
#define AR_SWITCH_TABLE_COM2_ALL_S (0)
    switch (ops) {
    case ANT_SELECT_OPS_GET:
        *common_tbl1 = OS_REG_READ_FIELD(ah, AR_PHY_SWITCH_COM,
                            AR_SWITCH_TABLE_COM_ALL);
        *common_tbl2 = OS_REG_READ_FIELD(ah, AR_PHY_SWITCH_COM_2,
                            AR_SWITCH_TABLE_COM2_ALL);
        break;
    case ANT_SELECT_OPS_SET:
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM,
            AR_SWITCH_TABLE_COM_ALL, *common_tbl1);
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM_2,
            AR_SWITCH_TABLE_COM2_ALL, *common_tbl2);

        /* write back to eeprom */
        if (IS_CHAN_2GHZ(curchan)) {
            eep->modal_header_2g.ant_ctrl_common = *common_tbl1;
            eep->modal_header_2g.ant_ctrl_common2 = *common_tbl2;
        } else {
            eep->modal_header_5g.ant_ctrl_common = *common_tbl1;
            eep->modal_header_5g.ant_ctrl_common2 = *common_tbl2;
        }

        break;
    default:
        break;
    }

    return true;
}
#else
/* ART_BUILD */
u_int32_t ar9300_ant_ctrl_common_get(struct ath_hal *ah, bool is_2ghz)
{
    if (!AR_SREV_JET(ah)){
        ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
        if (is_2ghz) {
            return eep->modal_header_2g.ant_ctrl_common;
        } else {
            return eep->modal_header_5g.ant_ctrl_common;
        }
    }else{
        ar5500_eeprom_t *eep = &AH9300(ah)->ah_eeprom_jet;
        if (is_2ghz) {
            return eep->modal_header_2g.ant_ctrl_common;
        } else {
            return eep->modal_header_5g.ant_ctrl_common;
        }
    }
}
static u_int16_t
ar9300_switch_com_spdt_get(struct ath_hal *ah, bool is_2ghz)
{
    if (!AR_SREV_JET(ah)){
        ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
        if (is_2ghz) {
            return eep->modal_header_2g.switchcomspdt;
        } else {
            return eep->modal_header_5g.switchcomspdt;
        }
    }else{
        ar5500_eeprom_t *eep = &AH9300(ah)->ah_eeprom_jet;
        if (is_2ghz) {
            return eep->modal_header_2g.switchcomspdt;
        } else {
            return eep->modal_header_5g.switchcomspdt;
        }
    }
}
u_int32_t ar9300_ant_ctrl_common2_get(struct ath_hal *ah, bool is_2ghz)
{
    if (!AR_SREV_JET(ah)){
        ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
        if (is_2ghz) {
            return eep->modal_header_2g.ant_ctrl_common2;
        } else {
            return eep->modal_header_5g.ant_ctrl_common2;
        }
    }else{
        ar5500_eeprom_t *eep = &AH9300(ah)->ah_eeprom_jet;
        if (is_2ghz) {
            return eep->modal_header_2g.ant_ctrl_common2;
        } else {
            return eep->modal_header_5g.ant_ctrl_common2;
        }
    }
}

u_int16_t ar9300_ant_ctrl_chain_get(struct ath_hal *ah, int chain,
    bool is_2ghz)
{
    if (!AR_SREV_JET(ah)){
        ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
        if (chain >= 0 && chain < OSPREY_MAX_CHAINS) {
            if (is_2ghz) {
                return eep->modal_header_2g.ant_ctrl_chain[chain];
            } else {
                return eep->modal_header_5g.ant_ctrl_chain[chain];
            }
        }
    }else{
        ar5500_eeprom_t *eep = &AH9300(ah)->ah_eeprom_jet;
        if (chain >= 0 && chain < JET_MAX_CHAINS) {
            if (is_2ghz) {
                return eep->modal_header_2g.ant_ctrl_chain[chain];
            } else {
                return eep->modal_header_5g.ant_ctrl_chain[chain];
            }
        }
    }
    return 0;
}

/*
 * Select the usage of antenna via the RF switch.
 * Default values are loaded from eeprom.
 */
bool ar9300_ant_swcom_sel(struct ath_hal *ah, u_int8_t ops,
                        u_int32_t *common_tbl1, u_int32_t *common_tbl2)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
    struct ath_hal_private  *ap  = AH_PRIVATE(ah);
    HAL_CHANNEL_INTERNAL    *curchan = ap->ah_curchan;
    enum {
        ANT_SELECT_OPS_GET,
        ANT_SELECT_OPS_SET,
    };

    if (AR_SREV_JUPITER(ah) || AR_SREV_SCORPION(ah))
        return false;

    if (!curchan)
        return false;

#define AR_SWITCH_TABLE_COM_ALL (0xffff)
#define AR_SWITCH_TABLE_COM_ALL_S (0)
#define AR_SWITCH_TABLE_COM2_ALL (0xffffff)
#define AR_SWITCH_TABLE_COM2_ALL_S (0)
    switch (ops) {
    case ANT_SELECT_OPS_GET:
        *common_tbl1 = OS_REG_READ_FIELD(ah, AR_PHY_SWITCH_COM,
                            AR_SWITCH_TABLE_COM_ALL);
        *common_tbl2 = OS_REG_READ_FIELD(ah, AR_PHY_SWITCH_COM_2,
                            AR_SWITCH_TABLE_COM2_ALL);
        break;
    case ANT_SELECT_OPS_SET:
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM,
            AR_SWITCH_TABLE_COM_ALL, *common_tbl1);
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM_2,
            AR_SWITCH_TABLE_COM2_ALL, *common_tbl2);

        /* write back to eeprom */
        if (IS_CHAN_2GHZ(curchan)) {
            if (!AR_SREV_JET(ah)){
                eep->modal_header_2g.ant_ctrl_common = *common_tbl1;
                eep->modal_header_2g.ant_ctrl_common2 = *common_tbl2;
            }else{
                eep->modal_header_2g.ant_ctrl_common = *common_tbl1;
                eep->modal_header_2g.ant_ctrl_common2 = *common_tbl2;
            }
        } else {
            if (!AR_SREV_JET(ah)){
                eep_jet->modal_header_5g.ant_ctrl_common = *common_tbl1;
                eep_jet->modal_header_5g.ant_ctrl_common2 = *common_tbl2;
            }else{
                eep_jet->modal_header_5g.ant_ctrl_common = *common_tbl1;
                eep_jet->modal_header_5g.ant_ctrl_common2 = *common_tbl2;
            }
        }

        break;
    default:
        break;
    }

    return true;
}
#endif // ART_BUILD end.
bool ar9300_ant_ctrl_apply(struct ath_hal *ah, bool is_2ghz)
{
    u_int32_t value;
    struct ath_hal_9300 *ahp = AH9300(ah);
    u_int32_t regval;
    struct ath_hal_private *ahpriv = AH_PRIVATE(ah);
#if ATH_ANT_DIV_COMB
    HAL_CAPABILITIES *pcap = &ahpriv->ah_caps;
#endif  /* ATH_ANT_DIV_COMB */
    u_int32_t xlan_gpio_cfg;
    u_int8_t  i;

    if (AR_SREV_POSEIDON(ah)) {
        xlan_gpio_cfg = ahpriv->ah_config.ath_hal_ext_lna_ctl_gpio;
        if (xlan_gpio_cfg) {
            for (i = 0; i < 32; i++) {
                if (xlan_gpio_cfg & (1 << i)) {
                    ath_hal_gpio_cfg_output(ah, i,
                            HAL_GPIO_OUTPUT_MUX_AS_PCIE_ATTENTION_LED);
                }
            }
        }
    }
#define AR_SWITCH_TABLE_COM_ALL (0xffff)
#define AR_SWITCH_TABLE_COM_ALL_S (0)
#define AR_SWITCH_TABLE_COM_JUPITER_ALL (0xffffff)
#define AR_SWITCH_TABLE_COM_JUPITER_ALL_S (0)
#define AR_SWITCH_TABLE_COM_SCORPION_ALL (0xffffff)
#define AR_SWITCH_TABLE_COM_SCORPION_ALL_S (0)
#define AR_SWITCH_TABLE_COM_HONEYBEE_ALL (0xffffff)
#define AR_SWITCH_TABLE_COM_HONEYBEE_ALL_S (0)
#define AR_SWITCH_TABLE_COM_SPDT (0x00f00000)
#define AR_SWITCH_TABLE_COM_DRAGONFLY_ALL (0xffffff)
#define AR_SWITCH_TABLE_COM_DRAGONFLY_ALL_S (0)
#define QCN5500_SWITCH_TABLE_COM_ALL (0xffffff)
#define QCN5500_SWITCH_TABLE_COM_ALL_S (0)
    value = ar9300_ant_ctrl_common_get(ah, is_2ghz);
    if (AR_SREV_JUPITER(ah) || AR_SREV_APHRODITE(ah)) {
        if (AR_SREV_JUPITER_10(ah)) {
            /* Force SPDT setting for Jupiter 1.0 chips. */
            value &= ~AR_SWITCH_TABLE_COM_SPDT;
            value |= 0x00100000;
        }
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM,
                AR_SWITCH_TABLE_COM_JUPITER_ALL, value);
    }
    else if (AR_SREV_SCORPION(ah)) {
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM,
                AR_SWITCH_TABLE_COM_SCORPION_ALL, value);
    }
    else if (AR_SREV_HONEYBEE(ah)) {
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM,
                AR_SWITCH_TABLE_COM_HONEYBEE_ALL, value);
    }
    else if (AR_SREV_DRAGONFLY(ah)) {
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM,
                AR_SWITCH_TABLE_COM_DRAGONFLY_ALL, value);
    }
    else if (AR_SREV_JET(ah)) {
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM,
                QCN5500_SWITCH_TABLE_COM_ALL, value);
    }
    else {
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM,
                AR_SWITCH_TABLE_COM_ALL, value);
    }
    /*
     *   Jupiter2.0 defines new switch table for BT/WLAN,
     *	here's new field name in WB222.ref for both 2G and 5G.
     *   Register: [GLB_CONTROL] GLB_CONTROL (@0x20044)
     *   15:12	R/W	SWITCH_TABLE_COM_SPDT_WLAN_RX	SWITCH_TABLE_COM_SPDT_WLAN_RX
     *   11:8	R/W	SWITCH_TABLE_COM_SPDT_WLAN_TX	SWITCH_TABLE_COM_SPDT_WLAN_TX
     *   7:4	R/W	SWITCH_TABLE_COM_SPDT_WLAN_IDLE	SWITCH_TABLE_COM_SPDT_WLAN_IDLE
     */
#define AR_SWITCH_TABLE_COM_SPDT_ALL (0x0000fff0)
#define AR_SWITCH_TABLE_COM_SPDT_ALL_S (4)
    if (AR_SREV_JUPITER_20_OR_LATER(ah) || AR_SREV_APHRODITE(ah)) {
        value = ar9300_switch_com_spdt_get(ah, is_2ghz);
        OS_REG_RMW_FIELD(ah, AR_GLB_CONTROL,
                AR_SWITCH_TABLE_COM_SPDT_ALL, value);

        OS_REG_SET_BIT(ah, AR_GLB_CONTROL,
                AR_BTCOEX_CTRL_SPDT_ENABLE);
        //OS_REG_SET_BIT(ah, AR_GLB_CONTROL,
        //    AR_BTCOEX_CTRL_BT_OWN_SPDT_CTRL);
    }

#define AR_SWITCH_TABLE_COM2_ALL (0xffffff)
#define AR_SWITCH_TABLE_COM2_ALL_S (0)
    value = ar9300_ant_ctrl_common2_get(ah, is_2ghz);
#if ATH_ANT_DIV_COMB
    if ( AR_SREV_POSEIDON(ah) && (ahp->ah_lna_div_use_bt_ant_enable == TRUE) ) {
        value &= ~AR_SWITCH_TABLE_COM2_ALL;
        value |= ahpriv->ah_config.ath_hal_ant_ctrl_comm2g_switch_enable;
    }
#endif  /* ATH_ANT_DIV_COMB */
    OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM_2, AR_SWITCH_TABLE_COM2_ALL, value);

#define AR_SWITCH_TABLE_ALL (0xfff)
#define AR_SWITCH_TABLE_ALL_S (0)
    value = ar9300_ant_ctrl_chain_get(ah, 0, is_2ghz);
    OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_CHAIN_0, AR_SWITCH_TABLE_ALL, value);

    if (!AR_SREV_HORNET(ah) && !AR_SREV_POSEIDON(ah) && !AR_SREV_APHRODITE(ah)) {
        value = ar9300_ant_ctrl_chain_get(ah, 1, is_2ghz);
        OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_CHAIN_1, AR_SWITCH_TABLE_ALL, value);

        if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah)) {
            value = ar9300_ant_ctrl_chain_get(ah, 2, is_2ghz);
            OS_REG_RMW_FIELD(ah,
                    AR_PHY_SWITCH_CHAIN_2, AR_SWITCH_TABLE_ALL, value);
            if (AR_SREV_JET(ah)){
                value = ar9300_ant_ctrl_chain_get(ah, 3, is_2ghz);
                OS_REG_RMW_FIELD(ah,
                        QCN5500_PHY_SWITCH_CHAIN_3, AR_SWITCH_TABLE_ALL, value);
            }
        }
    }

    if (AR_SREV_HORNET(ah) || AR_SREV_POSEIDON(ah)) {
        value = ar9300_eeprom_get(ahp, EEP_ANTDIV_control);
        /* main_lnaconf, alt_lnaconf, main_tb, alt_tb */
        regval = OS_REG_READ(ah, AR_PHY_MC_GAIN_CTRL);
        regval &= (~ANT_DIV_CONTROL_ALL); /* clear bit 25~30 */
        regval |= (value & 0x3f) << ANT_DIV_CONTROL_ALL_S;
        /* enable_lnadiv */
        regval &= (~MULTICHAIN_GAIN_CTRL__ENABLE_ANT_DIV_LNADIV__MASK);
        regval |= ((value >> 6) & 0x1) <<
            MULTICHAIN_GAIN_CTRL__ENABLE_ANT_DIV_LNADIV__SHIFT;
#if ATH_ANT_DIV_COMB
        if ( AR_SREV_POSEIDON(ah) && (ahp->ah_lna_div_use_bt_ant_enable == TRUE) ) {
            regval |= ANT_DIV_ENABLE;
        }
#endif  /* ATH_ANT_DIV_COMB */
        OS_REG_WRITE(ah, AR_PHY_MC_GAIN_CTRL, regval);

        /* enable fast_div */
        regval = OS_REG_READ(ah, AR_PHY_CCK_DETECT);
        regval &= (~BBB_SIG_DETECT__ENABLE_ANT_FAST_DIV__MASK);
        regval |= ((value >> 7) & 0x1) <<
            BBB_SIG_DETECT__ENABLE_ANT_FAST_DIV__SHIFT;
#if ATH_ANT_DIV_COMB
        if ( AR_SREV_POSEIDON(ah) && (ahp->ah_lna_div_use_bt_ant_enable == TRUE) ) {
            regval |= FAST_DIV_ENABLE;
        }
#endif  /* ATH_ANT_DIV_COMB */
        OS_REG_WRITE(ah, AR_PHY_CCK_DETECT, regval);
    }

#if ATH_ANT_DIV_COMB
    if (AR_SREV_HORNET(ah) || AR_SREV_POSEIDON_11_OR_LATER(ah)) {
        if (pcap->hal_ant_div_comb_support) {
            /* If support DivComb, set MAIN to LNA1, ALT to LNA2 at beginning */
            regval = OS_REG_READ(ah, AR_PHY_MC_GAIN_CTRL);
            /* clear bit 25~30 main_lnaconf, alt_lnaconf, main_tb, alt_tb */
            regval &= (~(MULTICHAIN_GAIN_CTRL__ANT_DIV_MAIN_LNACONF__MASK |
                        MULTICHAIN_GAIN_CTRL__ANT_DIV_ALT_LNACONF__MASK |
                        MULTICHAIN_GAIN_CTRL__ANT_DIV_ALT_GAINTB__MASK |
                        MULTICHAIN_GAIN_CTRL__ANT_DIV_MAIN_GAINTB__MASK));
            regval |= (HAL_ANT_DIV_COMB_LNA1 <<
                    MULTICHAIN_GAIN_CTRL__ANT_DIV_MAIN_LNACONF__SHIFT);
            regval |= (HAL_ANT_DIV_COMB_LNA2 <<
                    MULTICHAIN_GAIN_CTRL__ANT_DIV_ALT_LNACONF__SHIFT);
            OS_REG_WRITE(ah, AR_PHY_MC_GAIN_CTRL, regval);
        }

    }
#endif /* ATH_ANT_DIV_COMB */
    if (AR_SREV_POSEIDON(ah) && ( ahp->ah_diversity_control == HAL_ANT_FIXED_A
                || ahp->ah_diversity_control == HAL_ANT_FIXED_B))
    {
        u_int32_t reg_val = OS_REG_READ(ah, AR_PHY_MC_GAIN_CTRL);
        reg_val &=  ~(MULTICHAIN_GAIN_CTRL__ANT_DIV_MAIN_LNACONF__MASK |
                MULTICHAIN_GAIN_CTRL__ANT_DIV_ALT_LNACONF__MASK |
                MULTICHAIN_GAIN_CTRL__ANT_FAST_DIV_BIAS__MASK |
                MULTICHAIN_GAIN_CTRL__ANT_DIV_MAIN_GAINTB__MASK |
                MULTICHAIN_GAIN_CTRL__ANT_DIV_ALT_GAINTB__MASK );

        switch (ahp->ah_diversity_control) {
            case HAL_ANT_FIXED_A:
                /* Enable first antenna only */
                reg_val |= (HAL_ANT_DIV_COMB_LNA1 <<
                        MULTICHAIN_GAIN_CTRL__ANT_DIV_MAIN_LNACONF__SHIFT);
                reg_val |= (HAL_ANT_DIV_COMB_LNA2 <<
                        MULTICHAIN_GAIN_CTRL__ANT_DIV_ALT_LNACONF__SHIFT);
                /* main/alt gain table and Fast Div Bias all set to 0 */
                OS_REG_WRITE(ah, AR_PHY_MC_GAIN_CTRL, reg_val);
                regval = OS_REG_READ(ah, AR_PHY_CCK_DETECT);
                regval &= (~BBB_SIG_DETECT__ENABLE_ANT_FAST_DIV__MASK);
                OS_REG_WRITE(ah, AR_PHY_CCK_DETECT, regval);
                break;
            case HAL_ANT_FIXED_B:
                /* Enable second antenna only, after checking capability */
                reg_val |= (HAL_ANT_DIV_COMB_LNA2 <<
                        MULTICHAIN_GAIN_CTRL__ANT_DIV_MAIN_LNACONF__SHIFT);
                reg_val |= (HAL_ANT_DIV_COMB_LNA1 <<
                        MULTICHAIN_GAIN_CTRL__ANT_DIV_ALT_LNACONF__SHIFT);
                /* main/alt gain table and Fast Div all set to 0 */
                OS_REG_WRITE(ah, AR_PHY_MC_GAIN_CTRL, reg_val);
                regval = OS_REG_READ(ah, AR_PHY_CCK_DETECT);
                regval &= (~BBB_SIG_DETECT__ENABLE_ANT_FAST_DIV__MASK);
                OS_REG_WRITE(ah, AR_PHY_CCK_DETECT, regval);
                /* For WB225, need to swith ANT2 from BT to Wifi
                 * This will not affect HB125 LNA diversity feature.
                 */
                OS_REG_RMW_FIELD(ah, AR_PHY_SWITCH_COM_2, AR_SWITCH_TABLE_COM2_ALL,
                        ahpriv->ah_config.ath_hal_ant_ctrl_comm2g_switch_enable);
                break;
            default:
                break;
        }
    }
    return 0;
}

static u_int16_t
ar9300_attenuation_chain_get(struct ath_hal *ah, int chain, u_int16_t channel)
{
    int32_t f[3], t[3];
    u_int16_t value;
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#else
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
    if (chain >= 0 && chain < JET_MAX_CHAINS) {
        if (channel < 4000) {
#ifndef ART_BUILD
            return eep->modal_header_2g.xatten1_db[chain];
#else
            if(!AR_SREV_JET(ah))
                return eep->modal_header_2g.xatten1_db[chain];
            else
                return eep_jet->modal_header_2g.xatten1_db[chain];
#endif
        } else {
#ifndef ART_BUILD
            if (eep->base_ext2.xatten1_db_low[chain] != 0) {
                t[0] = eep->base_ext2.xatten1_db_low[chain];
                f[0] = 5180;
                t[1] = eep->modal_header_5g.xatten1_db[chain];
                f[1] = 5500;
                t[2] = eep->base_ext2.xatten1_db_high[chain];
                f[2] = 5785;
                value = interpolate(channel, f, t, 3);
                return value;
            } else {
                return eep->modal_header_5g.xatten1_db[chain];
            }
#else
            if(!AR_SREV_JET(ah)){
                if (eep->base_ext2.xatten1_db_low[chain] != 0) {
                    t[0] = eep->base_ext2.xatten1_db_low[chain];
                    f[0] = 5180;
                    t[1] = eep->modal_header_5g.xatten1_db[chain];
                    f[1] = 5500;
                    t[2] = eep->base_ext2.xatten1_db_high[chain];
                    f[2] = 5785;
                    value = interpolate(channel, f, t, 3);
                    return value;
                } else {
                    return eep->modal_header_5g.xatten1_db[chain];
                }
            }else{
                if (eep_jet->base_ext2.xatten1_db_low[chain] != 0) {
                    t[0] = eep_jet->base_ext2.xatten1_db_low[chain];
                    f[0] = 5180;
                    t[1] = eep_jet->modal_header_5g.xatten1_db[chain];
                    f[1] = 5500;
                    t[2] = eep_jet->base_ext2.xatten1_db_high[chain];
                    f[2] = 5785;
                    value = interpolate(channel, f, t, 3);
                    return value;
                } else {
                    return eep_jet->modal_header_5g.xatten1_db[chain];
                }
            }
#endif
        }
    }
    return 0;
}

static u_int16_t
ar9300_attenuation_margin_chain_get(struct ath_hal *ah, int chain,
    u_int16_t channel)
{
    int32_t f[3], t[3];
    u_int16_t value;
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#else
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
    if (chain >= 0 && chain < JET_MAX_CHAINS) {
        if (channel < 4000) {
#ifndef ART_BUILD
            return eep->modal_header_2g.xatten1_margin[chain];
#else
            if(!AR_SREV_JET(ah))
                return eep->modal_header_2g.xatten1_margin[chain];
            else
                return eep_jet->modal_header_2g.xatten1_margin[chain];
#endif
        } else {
#ifndef ART_BUILD
            if (eep->base_ext2.xatten1_margin_low[chain] != 0) {
                t[0] = eep->base_ext2.xatten1_margin_low[chain];
                f[0] = 5180;
                t[1] = eep->modal_header_5g.xatten1_margin[chain];
                f[1] = 5500;
                t[2] = eep->base_ext2.xatten1_margin_high[chain];
                f[2] = 5785;
                value = interpolate(channel, f, t, 3);
                return value;
            } else {
                return eep->modal_header_5g.xatten1_margin[chain];
            }
#else
            if(!AR_SREV_JET(ah)){
                if (eep->base_ext2.xatten1_margin_low[chain] != 0) {
                    t[0] = eep->base_ext2.xatten1_margin_low[chain];
                    f[0] = 5180;
                    t[1] = eep->modal_header_5g.xatten1_margin[chain];
                    f[1] = 5500;
                    t[2] = eep->base_ext2.xatten1_margin_high[chain];
                    f[2] = 5785;
                    value = interpolate(channel, f, t, 3);
                    return value;
                } else {
                    return eep->modal_header_5g.xatten1_margin[chain];
                }
            }else{
                if (eep_jet->base_ext2.xatten1_margin_low[chain] != 0) {
                    t[0] = eep_jet->base_ext2.xatten1_margin_low[chain];
                    f[0] = 5180;
                    t[1] = eep_jet->modal_header_5g.xatten1_margin[chain];
                    f[1] = 5500;
                    t[2] = eep_jet->base_ext2.xatten1_margin_high[chain];
                    f[2] = 5785;
                    value = interpolate(channel, f, t, 3);
                    return value;
                } else {
                    return eep_jet->modal_header_5g.xatten1_margin[chain];
                }
            }
#endif
        }
    }
    return 0;
}

bool ar9300_attenuation_apply(struct ath_hal *ah, u_int16_t channel)
{
    u_int32_t value;
    struct ath_hal_private *ahpriv = AH_PRIVATE(ah);

    /* Test value. if 0 then attenuation is unused. Don't load anything. */
    value = ar9300_attenuation_chain_get(ah, 0, channel);
    OS_REG_RMW_FIELD(ah,
            AR_PHY_EXT_ATTEN_CTL_0, AR_PHY_EXT_ATTEN_CTL_XATTEN1_DB, value);
    value = ar9300_attenuation_margin_chain_get(ah, 0, channel);
    if (ar9300_rx_gain_index_get(ah) == 0
            && ahpriv->ah_config.ath_hal_ext_atten_margin_cfg)
    {
        value = 5;
    }
    OS_REG_RMW_FIELD(ah,
            AR_PHY_EXT_ATTEN_CTL_0, AR_PHY_EXT_ATTEN_CTL_XATTEN1_MARGIN, value);

    if (!AR_SREV_HORNET(ah) && !AR_SREV_POSEIDON(ah)) {
        value = ar9300_attenuation_chain_get(ah, 1, channel);
        OS_REG_RMW_FIELD(ah,
                AR_PHY_EXT_ATTEN_CTL_1, AR_PHY_EXT_ATTEN_CTL_XATTEN1_DB, value);
        value = ar9300_attenuation_margin_chain_get(ah, 1, channel);
        OS_REG_RMW_FIELD(ah,
                AR_PHY_EXT_ATTEN_CTL_1, AR_PHY_EXT_ATTEN_CTL_XATTEN1_MARGIN,
                value);
        if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah)&& !AR_SREV_HONEYBEE(ah) ) {
            value = ar9300_attenuation_chain_get(ah, 2, channel);
            OS_REG_RMW_FIELD(ah,
                    AR_PHY_EXT_ATTEN_CTL_2, AR_PHY_EXT_ATTEN_CTL_XATTEN1_DB, value);
            value = ar9300_attenuation_margin_chain_get(ah, 2, channel);
            OS_REG_RMW_FIELD(ah,
                    AR_PHY_EXT_ATTEN_CTL_2, AR_PHY_EXT_ATTEN_CTL_XATTEN1_MARGIN,
                    value);
            if (AR_SREV_JET(ah)){
                value = ar9300_attenuation_chain_get(ah, 3, channel);
                OS_REG_RMW_FIELD(ah,
                        QCN5500_PHY_EXT_ATTEN_CTL_3, AR_PHY_EXT_ATTEN_CTL_XATTEN1_DB, value);
                value = ar9300_attenuation_margin_chain_get(ah, 3, channel);
                OS_REG_RMW_FIELD(ah,
                        QCN5500_PHY_EXT_ATTEN_CTL_3, AR_PHY_EXT_ATTEN_CTL_XATTEN1_MARGIN, value);
            }
        }
    }
    return 0;
}

static u_int16_t ar9300_quick_drop_get(struct ath_hal *ah,
								int chain, u_int16_t channel)
{
    int32_t f[3], t[3];
    u_int16_t value;
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#else
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
    if (channel < 4000) {
#ifndef ART_BUILD
        return eep->modal_header_2g.quick_drop;
#else
        if(!AR_SREV_JET(ah))
            return eep->modal_header_2g.quick_drop;
        else
            return eep_jet->modal_header_2g.quick_drop;
#endif
    } else {
#ifndef ART_BUILD
        t[0] = eep->base_ext1.quick_drop_low;
        f[0] = 5180;
        t[1] = eep->modal_header_5g.quick_drop;
        f[1] = 5500;
        t[2] = eep->base_ext1.quick_drop_high;
        f[2] = 5785;
        value = interpolate(channel, f, t, 3);
        return value;
#else
        if(!AR_SREV_JET(ah)){
            t[0] = eep->base_ext1.quick_drop_low;
            f[0] = 5180;
            t[1] = eep->modal_header_5g.quick_drop;
            f[1] = 5500;
            t[2] = eep->base_ext1.quick_drop_high;
            f[2] = 5785;
            value = interpolate(channel, f, t, 3);
            return value;
        }else{
            t[0] = eep_jet->base_ext1.quick_drop_low;
            f[0] = 5180;
            t[1] = eep_jet->modal_header_5g.quick_drop;
            f[1] = 5500;
            t[2] = eep_jet->base_ext1.quick_drop_high;
            f[2] = 5785;
            value = interpolate(channel, f, t, 3);
            return value;
        }
#endif
    }
}


static HAL_BOOL ar9300_quick_drop_apply(struct ath_hal *ah, u_int16_t channel)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
    u_int32_t value;
    //
    // Test value. if 0 then quickDrop is unused. Don't load anything.
    //
#ifndef ART_BUILD
    if (eep->base_eep_header.misc_configuration & 0x10)
#else
    if ((!AR_SREV_JET(ah) && (eep->base_eep_header.misc_configuration & 0x10)) ||
        (AR_SREV_JET(ah) && (eep_jet->base_eep_header.misc_configuration & 0x10)))
#endif
	{
        if (AR_SREV_OSPREY(ah) || AR_SREV_AR9580(ah) || AR_SREV_WASP(ah))
        {
            value = ar9300_quick_drop_get(ah, 0, channel);
            OS_REG_RMW_FIELD(ah, AR_PHY_AGC, AR_PHY_AGC_QUICK_DROP, value);
        }
    }
    return 0;
}

static u_int16_t ar9300_tx_end_to_xpa_off_get(struct ath_hal *ah, u_int16_t channel)
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
    if (channel < 4000) {
        return eep->modal_header_2g.tx_end_to_xpa_off;
    } else {
        return eep->modal_header_5g.tx_end_to_xpa_off;
    }
#else
    if (channel < 4000) {
        if (!AR_SREV_JET(ah))
            return eep->modal_header_2g.tx_end_to_xpa_off;
        else
            return eep_jet->modal_header_2g.tx_end_to_xpa_off;
    } else {
        if (!AR_SREV_JET(ah))
            return eep->modal_header_5g.tx_end_to_xpa_off;
        else
            return eep_jet->modal_header_5g.tx_end_to_xpa_off;
    }
#endif
}

static HAL_BOOL ar9300_tx_end_to_xpab_off_apply(struct ath_hal *ah, u_int16_t channel)
{
    u_int32_t value;

    value = ar9300_tx_end_to_xpa_off_get(ah, channel);
    /* Apply to both xpaa and xpab */
    if (AR_SREV_OSPREY(ah) || AR_SREV_AR9580(ah) || AR_SREV_WASP(ah))
    {
        OS_REG_RMW_FIELD(ah, AR_PHY_XPA_TIMING_CTL,
            AR_PHY_XPA_TIMING_CTL_TX_END_XPAB_OFF, value);
        OS_REG_RMW_FIELD(ah, AR_PHY_XPA_TIMING_CTL,
            AR_PHY_XPA_TIMING_CTL_TX_END_XPAA_OFF, value);
    }
    return 0;
}

int32_t ar9300_minCCApwr_thres_get(struct ath_hal *ah, int chain, u_int16_t channel)
{
        ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
        ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
        if (channel < 4000) {
                return eep->modal_header_2g.noise_floor_thresh_ch[chain];
        } else {
                return eep->modal_header_5g.noise_floor_thresh_ch[chain];
        }
#else
        if (channel < 4000) {
            if (!AR_SREV_JET(ah))
                return eep->modal_header_2g.noise_floor_thresh_ch[chain];
            else
                return eep_jet->modal_header_2g.noise_floor_thresh_ch[chain];
        } else {
            if (!AR_SREV_JET(ah))
                return eep->modal_header_5g.noise_floor_thresh_ch[chain];
            else
                return eep_jet->modal_header_5g.noise_floor_thresh_ch[chain];
        }
#endif
}

HAL_BOOL ar9300_minCCApwr_thres_apply(struct ath_hal *ah, u_int16_t channel)
{
        int32_t value;
        ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
        ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
		/* Applying individual chain values */
        if ((((eep->base_ext1.misc_enable & 0x4) >> 2 ) & 0x1) == 0 && channel < 4000) {
                return 0;
        }
         if ((((eep->base_ext1.misc_enable & 0x8) >> 2 ) & 0x2) == 0 && channel > 4000) {
                return 0;
        }
        if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x1) {
                value = ar9300_minCCApwr_thres_get(ah, 0, channel);
                OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_0, AR_PHY_EXT_CCA0_THRESH62_1, value);
        }
         if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x2) {
                value = ar9300_minCCApwr_thres_get(ah, 1, channel);
                OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_1, AR_PHY_EXT_CCA0_THRESH62_1, value);
        }
        if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x4) {
                value = ar9300_minCCApwr_thres_get(ah, 2, channel);
                OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_2, AR_PHY_EXT_CCA0_THRESH62_1, value);
        }
        /* JET: TODO */
        if (AR_SREV_JET(ah)){
            if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x8) {
                value = ar9300_minCCApwr_thres_get(ah, 3, channel);
                OS_REG_RMW_FIELD(ah, QCN5500_PHY_CCA_CTRL_3, AR_PHY_EXT_CCA0_THRESH62_1, value);
            }
        }
#else
        if (!AR_SREV_JET(ah)){
		    /* Applying individual chain values */
            if ((((eep->base_ext1.misc_enable & 0x4) >> 2 ) & 0x1) == 0 && channel < 4000) {
                return 0;
            }
            if ((((eep->base_ext1.misc_enable & 0x8) >> 2 ) & 0x2) == 0 && channel > 4000) {
                return 0;
            }
            if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x1) {
                value = ar9300_minCCApwr_thres_get(ah, 0, channel);
                OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_0, AR_PHY_EXT_CCA0_THRESH62_1, value);
            }
            if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x2) {
                value = ar9300_minCCApwr_thres_get(ah, 1, channel);
                OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_1, AR_PHY_EXT_CCA0_THRESH62_1, value);
            }
            if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x4) {
                value = ar9300_minCCApwr_thres_get(ah, 2, channel);
                OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_2, AR_PHY_EXT_CCA0_THRESH62_1, value);
            }
        }else{
		    /* Applying individual chain values */
            if ((((eep_jet->base_ext1.misc_enable & 0x4) >> 2 ) & 0x1) == 0 && channel < 4000) {
                return 0;
            }
            if ((((eep_jet->base_ext1.misc_enable & 0x8) >> 2 ) & 0x2) == 0 && channel > 4000) {
                return 0;
            }
            if (((eep_jet->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x1) {
                value = ar9300_minCCApwr_thres_get(ah, 0, channel);
                OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_0, AR_PHY_EXT_CCA0_THRESH62_1, value);
            }
            if (((eep_jet->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x2) {
                value = ar9300_minCCApwr_thres_get(ah, 1, channel);
                OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_1, AR_PHY_EXT_CCA0_THRESH62_1, value);
            }
            if (((eep_jet->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x4) {
                value = ar9300_minCCApwr_thres_get(ah, 2, channel);
                OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_2, AR_PHY_EXT_CCA0_THRESH62_1, value);
            }
            if (((eep_jet->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x8) {
                value = ar9300_minCCApwr_thres_get(ah, 3, channel);
                OS_REG_RMW_FIELD(ah, QCN5500_PHY_CCA_CTRL_3, AR_PHY_EXT_CCA0_THRESH62_1, value);
            }
        }
#endif
        OS_REG_RMW_FIELD(ah, AR_PHY_CCA_CTRL_0, AR_PHY_EXT_CCA0_THRESH62_ENABLE, 1);

        return 0;
}

static int
ar9300_eeprom_cal_pier_get(struct ath_hal *ah, int mode, int ipier, int ichain,
    int *pfrequency, int *pcorrection, int *ptemperature, int *pvoltage)
{
    u_int8_t *p_cal_pier;
    OSP_CAL_DATA_PER_FREQ_OP_LOOP *p_cal_pier_struct;
    int is_2ghz;
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif

    if (ichain >= JET_MAX_CHAINS) {
        HDPRINTF(ah, HAL_DBG_EEPROM,
            "%s: Invalid chain index, must be less than %d\n",
            __func__, JET_MAX_CHAINS);
        return -1;
    }

    if (mode) {/* 5GHz */
        if (ipier >= OSPREY_NUM_5G_CAL_PIERS){
            HDPRINTF(ah, HAL_DBG_EEPROM,
                "%s: Invalid 5GHz cal pier index, must be less than %d\n",
                __func__, OSPREY_NUM_5G_CAL_PIERS);
            return -1;
        }
#ifndef ART_BUILD
        p_cal_pier = &(eep->cal_freq_pier_5g[ipier]);
        p_cal_pier_struct = &(eep->cal_pier_data_5g[ichain][ipier]);
#else
        if(!AR_SREV_JET(ah)){
            p_cal_pier = &(eep->cal_freq_pier_5g[ipier]);
            p_cal_pier_struct = &(eep->cal_pier_data_5g[ichain][ipier]);
        }else{
            p_cal_pier = &(eep_jet->cal_freq_pier_5g[ipier]);
            p_cal_pier_struct = &(eep_jet->cal_pier_data_5g[ichain][ipier]);
        }
#endif
        is_2ghz = 0;
    } else {
        if (ipier >= OSPREY_NUM_2G_CAL_PIERS){
            HDPRINTF(ah, HAL_DBG_EEPROM,
                "%s: Invalid 2GHz cal pier index, must be less than %d\n",
                __func__, OSPREY_NUM_2G_CAL_PIERS);
            return -1;
        }

#ifndef ART_BUILD
        p_cal_pier = &(eep->cal_freq_pier_2g[ipier]);
        p_cal_pier_struct = &(eep->cal_pier_data_2g[ichain][ipier]);
#else
        if(!AR_SREV_JET(ah)){
            p_cal_pier = &(eep->cal_freq_pier_2g[ipier]);
            p_cal_pier_struct = &(eep->cal_pier_data_2g[ichain][ipier]);
        }else{
            p_cal_pier = &(eep_jet->cal_freq_pier_2g[ipier]);
            p_cal_pier_struct = &(eep_jet->cal_pier_data_2g[ichain][ipier]);
        }
#endif
        is_2ghz = 1;
    }
    *pfrequency = FBIN2FREQ(*p_cal_pier, is_2ghz);
    *pcorrection = p_cal_pier_struct->ref_power;
    *ptemperature = p_cal_pier_struct->temp_meas;
    *pvoltage = p_cal_pier_struct->volt_meas;
    return 0;
}

/*
 * Apply the recorded correction values.
 */
static int
ar9300_calibration_apply(struct ath_hal *ah, int frequency)
{
    int ichain, ipier, npier;
    int mode;
    int fdiff;
    int pfrequency, pcorrection, ptemperature, pvoltage;
    int bf, factor, plus;

    int lfrequency[AR9300_MAX_CHAINS] = {0};
    int hfrequency[AR9300_MAX_CHAINS] = {0};

    int lcorrection[AR9300_MAX_CHAINS] = {0};
    int hcorrection[AR9300_MAX_CHAINS] = {0};
    int correction[AR9300_MAX_CHAINS] = {0};

    int ltemperature[AR9300_MAX_CHAINS] = {0};
    int htemperature[AR9300_MAX_CHAINS] = {0};
    int temperature[AR9300_MAX_CHAINS] = {0};

    int lvoltage[AR9300_MAX_CHAINS] = {0};
    int hvoltage[AR9300_MAX_CHAINS] = {0};
    int voltage[AR9300_MAX_CHAINS] = {0};

    mode = (frequency >= 4000);
    npier = (mode) ?  OSPREY_NUM_5G_CAL_PIERS : OSPREY_NUM_2G_CAL_PIERS;

    for (ichain = 0; ichain < AR9300_MAX_CHAINS; ichain++) {
        lfrequency[ichain] = 0;
        hfrequency[ichain] = 100000;
    }
    /*
     * identify best lower and higher frequency calibration measurement
     */
    for (ichain = 0; ichain < AR9300_MAX_CHAINS; ichain++) {
        if ((ichain == 3) && !AR_SREV_JET(ah)) /* only allow chain0, chain1, chain2 for non-Jet */
            break;
        for (ipier = 0; ipier < npier; ipier++) {
            if (ar9300_eeprom_cal_pier_get(
                    ah, mode, ipier, ichain,
                    &pfrequency, &pcorrection, &ptemperature, &pvoltage) == 0)
            {
                fdiff = frequency - pfrequency;
                /*
                 * this measurement is higher than our desired frequency
                 */
                if (fdiff <= 0) {
                    if (hfrequency[ichain] <= 0 ||
                        hfrequency[ichain] >= 100000 ||
                        fdiff > (frequency - hfrequency[ichain]))
                    {
                        /*
                         * new best higher frequency measurement
                         */
                        hfrequency[ichain] = pfrequency;
                        hcorrection[ichain] = pcorrection;
                        htemperature[ichain] = ptemperature;
                        hvoltage[ichain] = pvoltage;
                    }
                }
                if (fdiff >= 0) {
                    if (lfrequency[ichain] <= 0 ||
                        fdiff < (frequency - lfrequency[ichain]))
                    {
                        /*
                         * new best lower frequency measurement
                         */
                        lfrequency[ichain] = pfrequency;
                        lcorrection[ichain] = pcorrection;
                        ltemperature[ichain] = ptemperature;
                        lvoltage[ichain] = pvoltage;
                    }
                }
            }
        }
    }
    /* interpolate */
    for (ichain = 0; ichain < AR9300_MAX_CHAINS; ichain++) {
        HDPRINTF(ah, HAL_DBG_EEPROM,
            "%s: ch=%d f=%d low=%d %d h=%d %d\n",
            __func__, ichain, frequency,
            lfrequency[ichain], lcorrection[ichain],
            hfrequency[ichain], hcorrection[ichain]);
        /*
         * they're the same, so just pick one
         */
        if (hfrequency[ichain] == lfrequency[ichain]) {
            correction[ichain] = lcorrection[ichain];
            voltage[ichain] = lvoltage[ichain];
            temperature[ichain] = ltemperature[ichain];
        } else if (frequency - lfrequency[ichain] < 1000) {
            /* the low frequency is good */
            if (hfrequency[ichain] - frequency < 1000) {
                /*
                 * The high frequency is good too -
                 * interpolate with round off.
                 */
                int mult, div, diff;
                mult = frequency - lfrequency[ichain];
                div = hfrequency[ichain] - lfrequency[ichain];

                diff = hcorrection[ichain] - lcorrection[ichain];
                bf = 2 * diff * mult / div;
                plus = (bf % 2);
                factor = bf / 2;
                correction[ichain] = lcorrection[ichain] + factor + plus;

                diff = htemperature[ichain] - ltemperature[ichain];
                bf = 2 * diff * mult / div;
                plus = (bf % 2);
                factor = bf / 2;
                temperature[ichain] = ltemperature[ichain] + factor + plus;

                diff = hvoltage[ichain] - lvoltage[ichain];
                bf = 2 * diff * mult / div;
                plus = (bf % 2);
                factor = bf / 2;
                voltage[ichain] = lvoltage[ichain] + factor + plus;
            } else {
                /* only low is good, use it */
                correction[ichain] = lcorrection[ichain];
                temperature[ichain] = ltemperature[ichain];
                voltage[ichain] = lvoltage[ichain];
            }
        } else if (hfrequency[ichain] - frequency < 1000) {
            /* only high is good, use it */
            correction[ichain] = hcorrection[ichain];
            temperature[ichain] = htemperature[ichain];
            voltage[ichain] = hvoltage[ichain];
        } else {
            /* nothing is good, presume 0???? */
            correction[ichain] = 0;
            temperature[ichain] = 0;
            voltage[ichain] = 0;
        }
    }

    /* GreenTx */
    if (AH_PRIVATE(ah)->ah_config.ath_hal_sta_update_tx_pwr_enable) {
        if (AR_SREV_POSEIDON(ah)) {
            /* Get calibrated OLPC gain delta value for GreenTx */
            AH_PRIVATE(ah)->ah_db2[POSEIDON_STORED_REG_G2_OLPC_OFFSET] =
                (u_int32_t) correction[0];
        }
    }

    ar9300_power_control_override(
        ah, frequency, correction, voltage, temperature);
    HDPRINTF(ah, HAL_DBG_EEPROM,
        "%s: for frequency=%d, calibration correction = %d %d %d\n",
         __func__, frequency, correction[0], correction[1], correction[2]);

    return 0;
}

int
ar9300_power_control_override(struct ath_hal *ah, int frequency,
    int *correction, int *voltage, int *temperature)
{
    int temp_slope = 0;
    int temp_slope_1 = 0;
    int temp_slope_2 = 0;
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#else
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
    int32_t f[8], t[8], t1[3], t2[3];
	int i;

    OS_REG_RMW(ah, AR_PHY_TPC_11_B0,
            (correction[0] << AR_PHY_TPC_OLPC_GAIN_DELTA_S),
            AR_PHY_TPC_OLPC_GAIN_DELTA);
    if (!AR_SREV_POSEIDON(ah)) {
        OS_REG_RMW(ah, AR_PHY_TPC_11_B1,
                (correction[1] << AR_PHY_TPC_OLPC_GAIN_DELTA_S),
                AR_PHY_TPC_OLPC_GAIN_DELTA);
        if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah) ) {
            OS_REG_RMW(ah, AR_PHY_TPC_11_B2,
                    (correction[2] << AR_PHY_TPC_OLPC_GAIN_DELTA_S),
                    AR_PHY_TPC_OLPC_GAIN_DELTA);
        }
        if (AR_SREV_JET(ah)){
            OS_REG_RMW(ah, QCN5500_PHY_TPC_11_B3,
                    (correction[3] << AR_PHY_TPC_OLPC_GAIN_DELTA_S),
                    AR_PHY_TPC_OLPC_GAIN_DELTA);
        }
    }
    /*
     * enable open loop power control on chip
     */
    OS_REG_RMW(ah, AR_PHY_TPC_6_B0,
            (3 << AR_PHY_TPC_6_ERROR_EST_MODE_S), AR_PHY_TPC_6_ERROR_EST_MODE);
    if (!AR_SREV_POSEIDON(ah)) {
        OS_REG_RMW(ah, AR_PHY_TPC_6_B1,
                (3 << AR_PHY_TPC_6_ERROR_EST_MODE_S), AR_PHY_TPC_6_ERROR_EST_MODE);
        if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah)  ) {
            OS_REG_RMW(ah, AR_PHY_TPC_6_B2,
                    (3 << AR_PHY_TPC_6_ERROR_EST_MODE_S),
                    AR_PHY_TPC_6_ERROR_EST_MODE);
        }
        if (AR_SREV_JET(ah)){
            OS_REG_RMW(ah, QCN5500_PHY_TPC_6_B3,
                    (3 << AR_PHY_TPC_6_ERROR_EST_MODE_S), AR_PHY_TPC_6_ERROR_EST_MODE);
        }
    }

    /*
     * Enable temperature compensation
     * Need to use register names
     */
#ifndef ART_BUILD

    if (frequency < 4000) {
        temp_slope = eep->modal_header_2g.temp_slope;
    } else {
		if ((eep->base_eep_header.misc_configuration & 0x20) != 0)
		{
				for(i=0;i<8;i++)
				{
					t[i]=eep->base_ext1.tempslopextension[i];
					f[i]=FBIN2FREQ(eep->cal_freq_pier_5g[i], 0);
				}
				temp_slope=interpolate(frequency,f,t,8);
		}
		else
		{
	        if(!AR_SREV_SCORPION(ah)) {
					if (eep->base_ext2.temp_slope_low != 0) {
							t[0] = eep->base_ext2.temp_slope_low;
							f[0] = 5180;
							t[1] = eep->modal_header_5g.temp_slope;
							f[1] = 5500;
							t[2] = eep->base_ext2.temp_slope_high;
							f[2] = 5785;
							temp_slope = interpolate(frequency, f, t, 3);
					} else {
							temp_slope = eep->modal_header_5g.temp_slope;
					}
			} else {
					/*
					 * Scorpion has individual chain tempslope values
					*/
					t[0] = eep->base_ext1.tempslopextension[2];
					t1[0]= eep->base_ext1.tempslopextension[3];
					t2[0]= eep->base_ext1.tempslopextension[4];
					f[0] = 5180;
					t[1] = eep->modal_header_5g.temp_slope;
					t1[1]= eep->base_ext1.tempslopextension[0];
					t2[1]= eep->base_ext1.tempslopextension[1];
					f[1] = 5500;
					t[2] = eep->base_ext1.tempslopextension[5];
					t1[2]= eep->base_ext1.tempslopextension[6];
					t2[2]= eep->base_ext1.tempslopextension[7];
					f[2] = 5785;
					temp_slope = interpolate(frequency, f, t, 3);
					temp_slope_1=interpolate(frequency, f, t1,3);
					temp_slope_2=interpolate(frequency, f, t2,3);
			}
		}
    }

    if ((!AR_SREV_SCORPION(ah)) && (!AR_SREV_HONEYBEE(ah)) && (!AR_SREV_DRAGONFLY(ah)) && (!AR_SREV_JET(ah))) {
        OS_REG_RMW_FIELD(ah,
            AR_PHY_TPC_19, AR_PHY_TPC_19_ALPHA_THERM, temp_slope);
    } else {
        /*Scorpion, Honeybee and Dragonfly has tempSlope register for each chain*/
        /*Check whether temp_compensation feature is enabled or not*/
        if (eep->base_eep_header.feature_enable & 0x1){
            if(frequency < 4000) {
                if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x1) {
                    OS_REG_RMW_FIELD(ah,
                            AR_PHY_TPC_19, AR_PHY_TPC_19_ALPHA_THERM,
                            eep->base_ext2.temp_slope_low);
                }
                if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x2) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B1, AR_PHY_TPC_19_ALPHA_THERM,
                            temp_slope);
                }
                if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x4) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B2, AR_PHY_TPC_19_ALPHA_THERM,
                            eep->base_ext2.temp_slope_high);
                }
                if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x8) {
                    OS_REG_RMW_FIELD(ah,
                            QCN5500_PHY_TPC_19_B3, AR_PHY_TPC_19_ALPHA_THERM,
                            eep->modal_header_2g.temp_slope_high_3); // jet alpha_thermal chain 3
                }
            } else {
                if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x1) {
                    OS_REG_RMW_FIELD(ah,
                            AR_PHY_TPC_19, AR_PHY_TPC_19_ALPHA_THERM,
                            temp_slope);
                }
                if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x2) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B1, AR_PHY_TPC_19_ALPHA_THERM,
                            temp_slope_1);
                }
                if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x4) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B2, AR_PHY_TPC_19_ALPHA_THERM,
                            temp_slope_2);
                }
            }
        }else {
            /* If temp compensation is not enabled, set all registers to 0*/
            if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x1) {
                OS_REG_RMW_FIELD(ah,
                        AR_PHY_TPC_19, AR_PHY_TPC_19_ALPHA_THERM, 0);
            }
            if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x2) {
                OS_REG_RMW_FIELD(ah,
                        AR_SCORPION_PHY_TPC_19_B1, AR_PHY_TPC_19_ALPHA_THERM, 0);
            }
            if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x4) {
                OS_REG_RMW_FIELD(ah,
                        AR_SCORPION_PHY_TPC_19_B2, AR_PHY_TPC_19_ALPHA_THERM, 0);
            }
            if (((eep->base_eep_header.txrx_mask & 0xf0) >> 4) & 0x8) {
                OS_REG_RMW_FIELD(ah,
                        QCN5500_PHY_TPC_19_B3, AR_PHY_TPC_19_ALPHA_THERM, 0);
            }
        }
    }
#else // ART_BUILD
    if (frequency < 4000) {
        if (!AR_SREV_JET(ah))
            temp_slope = eep->modal_header_2g.temp_slope;
        else
            temp_slope = eep_jet->modal_header_2g.temp_slope;
    } else {
        if (!AR_SREV_JET(ah)) {
			if ((eep->base_eep_header.misc_configuration & 0x20) != 0)
			{
					for(i=0;i<8;i++)
					{
						t[i]=eep->base_ext1.tempslopextension[i];
						f[i]=FBIN2FREQ(eep->cal_freq_pier_5g[i], 0);
					}
					temp_slope=interpolate(frequency,f,t,8);
            } else {
                if(!AR_SREV_SCORPION(ah)) {
                    if (eep->base_ext2.temp_slope_low != 0) {
                        t[0] = eep->base_ext2.temp_slope_low;
                        f[0] = 5180;
                        t[1] = eep->modal_header_5g.temp_slope;
                        f[1] = 5500;
                        t[2] = eep->base_ext2.temp_slope_high;
                        f[2] = 5785;
                        temp_slope = interpolate(frequency, f, t, 3);
                    } else {
                        temp_slope = eep->modal_header_5g.temp_slope;
                    }
                } else {
                    /*
                     * Scorpion has individual chain tempslope values
                     */
                    t[0] = eep->base_ext1.tempslopextension[2];
                    t1[0]= eep->base_ext1.tempslopextension[3];
                    t2[0]= eep->base_ext1.tempslopextension[4];
                    f[0] = 5180;
                    t[1] = eep->modal_header_5g.temp_slope;
                    t1[1]= eep->base_ext1.tempslopextension[0];
                    t2[1]= eep->base_ext1.tempslopextension[1];
                    f[1] = 5500;
                    t[2] = eep->base_ext1.tempslopextension[5];
                    t1[2]= eep->base_ext1.tempslopextension[6];
                    t2[2]= eep->base_ext1.tempslopextension[7];
                    f[2] = 5785;
                    temp_slope = interpolate(frequency, f, t, 3);
                    temp_slope_1=interpolate(frequency, f, t1,3);
                    temp_slope_2=interpolate(frequency, f, t2,3);
                }
            }
        }else{
            if ((eep_jet->base_eep_header.misc_configuration & 0x20) != 0)
            {
                for(i=0;i<8;i++)
                {
                    t[i]=eep_jet->base_ext1.tempslopextension[i];
                    f[i]=FBIN2FREQ(eep_jet->cal_freq_pier_5g[i], 0);
                }
                temp_slope=interpolate(frequency,f,t,8);
            } else {
                if(!AR_SREV_SCORPION(ah)) {
                    if (eep_jet->base_ext2.temp_slope_low != 0) {
                        t[0] = eep_jet->base_ext2.temp_slope_low;
                        f[0] = 5180;
                        t[1] = eep_jet->modal_header_5g.temp_slope;
                        f[1] = 5500;
                        t[2] = eep_jet->base_ext2.temp_slope_high;
                        f[2] = 5785;
                        temp_slope = interpolate(frequency, f, t, 3);
                    } else {
                        temp_slope = eep_jet->modal_header_5g.temp_slope;
                    }
                } else {
                    /*
                     * Scorpion has individual chain tempslope values
                     */
                    t[0] = eep_jet->base_ext1.tempslopextension[2];
                    t1[0]= eep_jet->base_ext1.tempslopextension[3];
                    t2[0]= eep_jet->base_ext1.tempslopextension[4];
                    f[0] = 5180;
                    t[1] = eep_jet->modal_header_5g.temp_slope;
                    t1[1]= eep_jet->base_ext1.tempslopextension[0];
                    t2[1]= eep_jet->base_ext1.tempslopextension[1];
                    f[1] = 5500;
                    t[2] = eep_jet->base_ext1.tempslopextension[5];
                    t1[2]= eep_jet->base_ext1.tempslopextension[6];
                    t2[2]= eep_jet->base_ext1.tempslopextension[7];
                    f[2] = 5785;
                    temp_slope = interpolate(frequency, f, t, 3);
                    temp_slope_1=interpolate(frequency, f, t1,3);
                    temp_slope_2=interpolate(frequency, f, t2,3);
                }
            }
        }
    }

    if ((!AR_SREV_SCORPION(ah)) && (!AR_SREV_HONEYBEE(ah)) && (!AR_SREV_DRAGONFLY(ah)) && (!AR_SREV_JET(ah))) {
        OS_REG_RMW_FIELD(ah,
            AR_PHY_TPC_19, AR_PHY_TPC_19_ALPHA_THERM, temp_slope);
    } else {
        u_int8_t tx_mask;
        if (!AR_SREV_JET(ah))
            tx_mask = (eep->base_eep_header.txrx_mask & 0xf0) >> 4;
        else
            tx_mask = (eep_jet->base_eep_header.txrx_mask & 0xf0) >> 4;

        /*Scorpion, Honeybee and Dragonfly has tempSlope register for each chain*/
        /*Check whether temp_compensation feature is enabled or not*/
        if (( !AR_SREV_JET(ah) && (eep->base_eep_header.feature_enable & 0x1)) ||
            (  AR_SREV_JET(ah) && (eep_jet->base_eep_header.feature_enable & 0x1)))
        {
            if(frequency < 4000) {
              if(!AR_SREV_JET(ah))
              {
                if (tx_mask & 0x1) {
                    OS_REG_RMW_FIELD(ah,
                            AR_PHY_TPC_19, AR_PHY_TPC_19_ALPHA_THERM,
                            eep->base_ext2.temp_slope_low);
                }
                if (tx_mask & 0x2) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B1, AR_PHY_TPC_19_ALPHA_THERM,
                            temp_slope);
                }
                if (tx_mask & 0x4) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B2, AR_PHY_TPC_19_ALPHA_THERM,
                            eep->base_ext2.temp_slope_high);
                }
              }
              else
              {
                if (tx_mask & 0x1) {
                    OS_REG_RMW_FIELD(ah,
                            AR_PHY_TPC_19, AR_PHY_TPC_19_ALPHA_THERM,
                            eep_jet->base_ext2.temp_slope_low);
                }
                if (tx_mask & 0x2) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B1, AR_PHY_TPC_19_ALPHA_THERM,
                            temp_slope);
                }
                if (tx_mask & 0x4) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B2, AR_PHY_TPC_19_ALPHA_THERM,
                            eep_jet->base_ext2.temp_slope_high);
                }
                if (tx_mask & 0x8) {
                    OS_REG_RMW_FIELD(ah,
                            QCN5500_PHY_TPC_19_B3, AR_PHY_TPC_19_ALPHA_THERM,
                            eep_jet->modal_header_2g.temp_slope_high_3); // jet alpha_thermal chain 3
                }

              }

            } else {
                if (tx_mask & 0x1) {
                    OS_REG_RMW_FIELD(ah,
                            AR_PHY_TPC_19, AR_PHY_TPC_19_ALPHA_THERM,
                            temp_slope);
                }
                if (tx_mask & 0x2) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B1, AR_PHY_TPC_19_ALPHA_THERM,
                            temp_slope_1);
                }
                if (tx_mask & 0x4) {
                    OS_REG_RMW_FIELD(ah,
                            AR_SCORPION_PHY_TPC_19_B2, AR_PHY_TPC_19_ALPHA_THERM,
                            temp_slope_2);
                }
            }
        }else {
            /* If temp compensation is not enabled, set all registers to 0*/
            if (tx_mask & 0x1) {
                OS_REG_RMW_FIELD(ah,
                        AR_PHY_TPC_19, AR_PHY_TPC_19_ALPHA_THERM, 0);
            }
            if (tx_mask & 0x2) {
                OS_REG_RMW_FIELD(ah,
                        AR_SCORPION_PHY_TPC_19_B1, AR_PHY_TPC_19_ALPHA_THERM, 0);
            }
            if (tx_mask & 0x4) {
                OS_REG_RMW_FIELD(ah,
                        AR_SCORPION_PHY_TPC_19_B2, AR_PHY_TPC_19_ALPHA_THERM, 0);
            }
            if (tx_mask & 0x8) {
                OS_REG_RMW_FIELD(ah,
                        QCN5500_PHY_TPC_19_B3, AR_PHY_TPC_19_ALPHA_THERM, 0);
            }
        }
    }

#endif
    if (AR_SREV_JET(ah)) {
        OS_REG_RMW_FIELD(ah,
                AR_PHY_TPC_18, AR_PHY_TPC_18_THERM_CAL_VALUE, temperature[0]);
        OS_REG_RMW_FIELD(ah,
                AR_PHY_TPC_18_B1, AR_PHY_TPC_18_THERM_CAL_VALUE, temperature[1]);
        OS_REG_RMW_FIELD(ah,
                AR_PHY_TPC_18_B2, AR_PHY_TPC_18_THERM_CAL_VALUE, temperature[2]);
        OS_REG_RMW_FIELD(ah,
                QCN5500_PHY_TPC_18_B3, AR_PHY_TPC_18_THERM_CAL_VALUE, temperature[3]);
    } else {
        OS_REG_RMW_FIELD(ah,
                AR_PHY_TPC_18, AR_PHY_TPC_18_THERM_CAL_VALUE, temperature[0]);
    }

    return 0;
}

#ifndef _UNINET_REGDMN_ /* uninet regdomain programs it's own CTLs */
/**************************************************************
 * ar9300_eep_def_get_max_edge_power
 *
 * Find the maximum conformance test limit for the given channel and CTL info
 */
static inline u_int16_t
ar9300_eep_def_get_max_edge_power(struct ath_hal *ah, ar9300_eeprom_t *p_eep_data, u_int16_t freq,
    int idx, bool is_2ghz)
{
    u_int16_t twice_max_edge_power = AR9300_MAX_RATE_POWER;
    u_int8_t *ctl_freqbin = is_2ghz ?
        &p_eep_data->ctl_freqbin_2G[idx][0] :
        &p_eep_data->ctl_freqbin_5G[idx][0];
    u_int16_t num_edges = is_2ghz ?
        OSPREY_NUM_BAND_EDGES_2G : OSPREY_NUM_BAND_EDGES_5G;
    int i;
#ifdef ART_BUILD
    ar5500_eeprom_t *p_eep_data_jet = (ar5500_eeprom_t *) p_eep_data;
    if (!AR_SREV_JET(ah)){
        if (is_2ghz)
            ctl_freqbin = &p_eep_data->ctl_freqbin_2G[idx][0];
        else
            ctl_freqbin = &p_eep_data->ctl_freqbin_5G[idx][0];
    }else{
        if (is_2ghz)
            ctl_freqbin = &p_eep_data_jet->ctl_freqbin_2G[idx][0];
        else
            ctl_freqbin = &p_eep_data_jet->ctl_freqbin_5G[idx][0];
    }

#endif

    /* Get the edge power */
    for (i = 0; (i < num_edges) && (ctl_freqbin[i] != AR9300_BCHAN_UNUSED); i++)
    {
#ifndef ART_BUILD
        /*
         * If there's an exact channel match or an inband flag set
         * on the lower channel use the given rd_edge_power
         */
        if (freq == fbin2freq(ctl_freqbin[i], is_2ghz)) {
            if (is_2ghz) {
                twice_max_edge_power =
                    p_eep_data->ctl_power_data_2g[idx].ctl_edges[i].t_power;
            } else {
                twice_max_edge_power =
                    p_eep_data->ctl_power_data_5g[idx].ctl_edges[i].t_power;
            }
            break;
        } else if ((i > 0) && (freq < fbin2freq(ctl_freqbin[i], is_2ghz))) {
            if (is_2ghz) {
                if (fbin2freq(ctl_freqbin[i - 1], 1) < freq &&
                        p_eep_data->ctl_power_data_2g[idx].ctl_edges[i - 1].flag)
                {
                    twice_max_edge_power =
                        p_eep_data->ctl_power_data_2g[idx].
                        ctl_edges[i - 1].t_power;
                }
            } else {
                if (fbin2freq(ctl_freqbin[i - 1], 0) < freq &&
                        p_eep_data->ctl_power_data_5g[idx].ctl_edges[i - 1].flag)
                {
                    twice_max_edge_power =
                        p_eep_data->ctl_power_data_5g[idx].
                        ctl_edges[i - 1].t_power;
                }
            }
            /*
             * Leave loop - no more affecting edges possible
             * in this monotonic increasing list
             */
            break;
        }
#else
        if (!AR_SREV_JET(ah)) {
            /*
             * If there's an exact channel match or an inband flag set
             * on the lower channel use the given rd_edge_power
             */
            if (freq == fbin2freq(ctl_freqbin[i], is_2ghz)) {
                if (is_2ghz) {
                    twice_max_edge_power =
                        p_eep_data->ctl_power_data_2g[idx].ctl_edges[i].t_power;
                } else {
                    twice_max_edge_power =
                        p_eep_data->ctl_power_data_5g[idx].ctl_edges[i].t_power;
                }
                break;
            } else if ((i > 0) && (freq < fbin2freq(ctl_freqbin[i], is_2ghz))) {
                if (is_2ghz) {
                    if (fbin2freq(ctl_freqbin[i - 1], 1) < freq &&
                            p_eep_data->ctl_power_data_2g[idx].ctl_edges[i - 1].flag)
                    {
                        twice_max_edge_power =
                            p_eep_data->ctl_power_data_2g[idx].
                            ctl_edges[i - 1].t_power;
                    }
                } else {
                    if (fbin2freq(ctl_freqbin[i - 1], 0) < freq &&
                            p_eep_data->ctl_power_data_5g[idx].ctl_edges[i - 1].flag)
                    {
                        twice_max_edge_power =
                            p_eep_data->ctl_power_data_5g[idx].
                            ctl_edges[i - 1].t_power;
                    }
                }
                /*
                 * Leave loop - no more affecting edges possible
                 * in this monotonic increasing list
                 */
                break;
            }

        }else{
            /*
             * If there's an exact channel match or an inband flag set
             * on the lower channel use the given rd_edge_power
             */
            if (freq == fbin2freq(ctl_freqbin[i], is_2ghz)) {
                if (is_2ghz) {
                    twice_max_edge_power =
                        p_eep_data_jet->ctl_power_data_2g[idx].ctl_edges[i].t_power;
                } else {
                    twice_max_edge_power =
                        p_eep_data_jet->ctl_power_data_5g[idx].ctl_edges[i].t_power;
                }
                break;
            } else if ((i > 0) && (freq < fbin2freq(ctl_freqbin[i], is_2ghz))) {
                if (is_2ghz) {
                    if (fbin2freq(ctl_freqbin[i - 1], 1) < freq &&
                            p_eep_data_jet->ctl_power_data_2g[idx].ctl_edges[i - 1].flag)
                    {
                        twice_max_edge_power =
                            p_eep_data_jet->ctl_power_data_2g[idx].
                            ctl_edges[i - 1].t_power;
                    }
                } else {
                    if (fbin2freq(ctl_freqbin[i - 1], 0) < freq &&
                            p_eep_data_jet->ctl_power_data_5g[idx].ctl_edges[i - 1].flag)
                    {
                        twice_max_edge_power =
                            p_eep_data_jet->ctl_power_data_5g[idx].
                            ctl_edges[i - 1].t_power;
                    }
                }
                /*
                 * Leave loop - no more affecting edges possible
                 * in this monotonic increasing list
                 */
                break;
            }
        }
#endif
    }


    /*
     * EV89475: EEPROM might contain 0 txpower in CTL table for certain
     * 2.4GHz channels. We workaround it by overwriting 60 (30 dBm) here.
     */
    if (is_2ghz && (twice_max_edge_power == 0)) {
        twice_max_edge_power = 60;
    }

    HALASSERT(twice_max_edge_power > 0);
    return twice_max_edge_power;
}
#endif /* #ifndef _UNINET_REGDMN_ */

static inline void
ctlpwscale_reduction(u_int8_t *p_pwr_array, u_int16_t reduction,
        u_int16_t rate_index)
{
    if(( reduction * 2) > p_pwr_array[rate_index]) {
        p_pwr_array[rate_index] =  0;
    } else {
        p_pwr_array[rate_index] -= (reduction * 2);
    }
}

bool
ar9300_eeprom_set_power_per_rate_table(
    struct ath_hal *ah,
    ar9300_eeprom_t *p_eep_data,
    HAL_CHANNEL_INTERNAL *chan,
    u_int8_t *p_pwr_array,
    u_int16_t cfg_ctl,
    u_int16_t antenna_reduction,
    u_int16_t twice_max_regulatory_power,
    u_int16_t power_limit,
    u_int8_t chainmask)
{
    /* Local defines to distinguish between extension and control CTL's */
#define EXT_ADDITIVE (0x8000)
#define CTL_11A_EXT (CTL_11A | EXT_ADDITIVE)
#define CTL_11G_EXT (CTL_11G | EXT_ADDITIVE)
#define CTL_11B_EXT (CTL_11B | EXT_ADDITIVE)
#define REDUCE_SCALED_POWER_BY_TWO_CHAIN     6  /* 10*log10(2)*2 */
#define REDUCE_SCALED_POWER_BY_THREE_CHAIN  10  /* 10*log10(3)*2 */
#define REDUCE_SCALED_POWER_BY_FOUR_CHAINS  12  /* 10*log10(4)*2 */
#define PWRINCR_4_TO_1_CHAIN     12             /* 10*log(4)*2 */
#define PWRINCR_4_TO_2_CHAIN      6             /* 10*log(2)*2 */
#define PWRINCR_4_TO_3_CHAIN      3             /* floor(10*log(4/3)*2) */
#define PWRINCR_3_TO_1_CHAIN      9             /* 10*log(3)*2 */
#define PWRINCR_3_TO_2_CHAIN      3             /* floor(10*log(3/2)*2) */
#define PWRINCR_2_TO_1_CHAIN      6             /* 10*log(2)*2 */

    static const u_int16_t tp_scale_reduction_table[5] =
        { 0, 3, 6, 9, AR9300_MAX_RATE_POWER };
    int i;
    int16_t twice_largest_antenna;
    u_int16_t twice_antenna_reduction = 2*antenna_reduction ;
    int16_t scaled_power = 0, min_ctl_power, max_reg_allowed_power;
#define SUB_NUM_CTL_MODES_AT_5G_40 2    /* excluding HT40, EXT-OFDM */
#define SUB_NUM_CTL_MODES_AT_2G_40 3    /* excluding HT40, EXT-OFDM, EXT-CCK */
    u_int16_t ctl_modes_for11a[] =
        {CTL_11A, CTL_5GHT20, CTL_11A_EXT, CTL_5GHT40};
    u_int16_t ctl_modes_for11g[] =
        {CTL_11B, CTL_11G, CTL_2GHT20, CTL_11B_EXT, CTL_11G_EXT, CTL_2GHT40};
    u_int16_t num_ctl_modes, *p_ctl_mode, ctl_mode, freq;
    CHAN_CENTERS centers;
    int tx_chainmask;
    struct ath_hal_9300 *ahp = AH9300(ah);
#ifndef _UNINET_REGDMN_
    u_int8_t *ctl_index;
    u_int8_t ctl_num;
    u_int16_t twice_min_edge_power;
    u_int16_t twice_max_edge_power = AR9300_MAX_RATE_POWER;
#endif
    u_int16_t reduction;
#ifdef ART_BUILD
    ar5500_eeprom_t *p_eep_data_jet = (ar5500_eeprom_t *) p_eep_data;
#endif
    if (chainmask)
        tx_chainmask = chainmask;
    else
        tx_chainmask = ahp->ah_tx_chainmaskopt ?
                            ahp->ah_tx_chainmaskopt :ahp->ah_tx_chainmask;

    ar9300_get_channel_centers(ah, chan, &centers);

#ifdef _UNINET_REGDMN_
    /* Use the CTL data stored in max_reg_tx_power instead of EEPROM */
    u_int16_t ctl_value[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    u_int16_t chan_mode;

    twice_max_regulatory_power = AR9300_MAX_RATE_POWER;

    for (i = 0; i < AH_PRIVATE(ah)->ah_nchan; i++) {
        if (AH_TABLES(ah)->ah_channels[i].channel == chan->channel) {
            /* Determine channel mode */
            if (IS_CHAN_5GHZ(&AH_TABLES(ah)->ah_channels[i])) {
                if ((AH_TABLES(ah)->ah_channels[i].channel_flags &
                     CHANNEL_HT40PLUS) ||
                    (AH_TABLES(ah)->ah_channels[i].channel_flags &
                     CHANNEL_HT40MINUS))
                {
                    chan_mode = CTL_5GHT40;
                } else if (AH_TABLES(ah)->ah_channels[i].channel_flags &
                           CHANNEL_HT20)
                {
                    chan_mode = CTL_5GHT20;
                } else {
                    chan_mode = CTL_11A;
                }
            } else {
                if ((AH_TABLES(ah)->ah_channels[i].channel_flags &
                     CHANNEL_HT40PLUS) ||
                    (AH_TABLES(ah)->ah_channels[i].channel_flags &
                     CHANNEL_HT40MINUS))
                {
                    chan_mode = CTL_2GHT40;
                } else if (AH_TABLES(ah)->ah_channels[i].channel_flags &
                    CHANNEL_HT20)
                {
                    chan_mode = CTL_2GHT20;
                } else if (AH_TABLES(ah)->ah_channels[i].channel_flags &
                           CHANNEL_CCK)
                {
                    chan_mode = CTL_11B;
                } else {
                    chan_mode = CTL_11G;
				}
            }
            /* Save CTL limit for this mode */
            ctl_value[chan_mode] =
                AH_TABLES(ah)->ah_channels[i].max_reg_tx_power;
        }
    }
#endif /* ifdef _UNINET_REGDMN_ */
#ifndef ART_BUILD
    if (IS_CHAN_2GHZ(chan)) {
        ahp->twice_antenna_gain = AH_MAX(p_eep_data->modal_header_2g.antenna_gain,
                                         AH_PRIVATE(ah)->ah_antenna_gain_2g);
    } else {
        ahp->twice_antenna_gain = AH_MAX(p_eep_data->modal_header_5g.antenna_gain,
                                         AH_PRIVATE(ah)->ah_antenna_gain_5g);
    }
#else
    if (!AR_SREV_JET(ah)){
        if (IS_CHAN_2GHZ(chan)) {
            ahp->twice_antenna_gain = AH_MAX(p_eep_data->modal_header_2g.antenna_gain,
                                             AH_PRIVATE(ah)->ah_antenna_gain_2g);
        } else {
            ahp->twice_antenna_gain = AH_MAX(p_eep_data->modal_header_5g.antenna_gain,
                                         AH_PRIVATE(ah)->ah_antenna_gain_5g);
        }
    }else{
        if (IS_CHAN_2GHZ(chan)) {
            ahp->twice_antenna_gain = AH_MAX(p_eep_data_jet->modal_header_2g.antenna_gain,
                                             AH_PRIVATE(ah)->ah_antenna_gain_2g);
        } else {
            ahp->twice_antenna_gain = AH_MAX(p_eep_data_jet->modal_header_5g.antenna_gain,
                                         AH_PRIVATE(ah)->ah_antenna_gain_5g);
        }
    }

#endif

    /* Save max allowed antenna gain to ease future lookups */
    ahp->twice_antenna_reduction = twice_antenna_reduction;

    /*  Deduct antenna gain from  EIRP to get the upper limit */
    twice_largest_antenna = (int16_t)AH_MIN((twice_antenna_reduction -
                                       ahp->twice_antenna_gain), 0);
    max_reg_allowed_power = twice_max_regulatory_power + twice_largest_antenna;

    /* Use ah_tp_scale - see bug 30070. */
    if (AH_PRIVATE(ah)->ah_tp_scale != HAL_TP_SCALE_MAX) {
        max_reg_allowed_power -=
            (tp_scale_reduction_table[(AH_PRIVATE(ah)->ah_tp_scale)] * 2);
    }

    scaled_power = AH_MIN(power_limit, max_reg_allowed_power);

    /*
     * Reduce scaled Power by number of chains active to get to
     * per chain tx power level
     */
    /* TODO: better value than these? */
    switch (ar9300_get_ntxchains(tx_chainmask)) {
    case 1:
        ahp->upper_limit[0] = AH_MAX(0, scaled_power);
        ahp->num_txchain_comp[0] = 0;
        break;
    case 2:
        ahp->num_txchain_comp[1] = AH_MAX(0, scaled_power);
        scaled_power -= REDUCE_SCALED_POWER_BY_TWO_CHAIN;
        ahp->upper_limit[1] = AH_MAX(0, scaled_power);
        ahp->num_txchain_comp[1] -= ahp->upper_limit[1];
        break;
    case 3:
        ahp->num_txchain_comp[2] = AH_MAX(0, scaled_power);
        scaled_power -= REDUCE_SCALED_POWER_BY_THREE_CHAIN;
        ahp->upper_limit[2] = AH_MAX(0, scaled_power);
        ahp->num_txchain_comp[2] -= ahp->upper_limit[2];
        break;
    case 4:
        ahp->num_txchain_comp[3] = AH_MAX(0, scaled_power);
        scaled_power -= REDUCE_SCALED_POWER_BY_FOUR_CHAINS;
        ahp->upper_limit[3] = AH_MAX(0, scaled_power);
        ahp->num_txchain_comp[3] -= ahp->upper_limit[3];
        break;
    default:
        HALASSERT(0); /* Unsupported number of chains */
    }

    scaled_power = AH_MAX(0, scaled_power);

    /* Get target powers from EEPROM - our baseline for TX Power */
    if (IS_CHAN_2GHZ(chan)) {
        /* Setup for CTL modes */
        /* CTL_11B, CTL_11G, CTL_2GHT20 */
        num_ctl_modes =
            ARRAY_LENGTH(ctl_modes_for11g) - SUB_NUM_CTL_MODES_AT_2G_40;
        p_ctl_mode = ctl_modes_for11g;

        if (IS_CHAN_HT40(chan)) {
            num_ctl_modes = ARRAY_LENGTH(ctl_modes_for11g); /* All 2G CTL's */
        }
    } else {
        /* Setup for CTL modes */
        /* CTL_11A, CTL_5GHT20 */
        num_ctl_modes =
            ARRAY_LENGTH(ctl_modes_for11a) - SUB_NUM_CTL_MODES_AT_5G_40;
        p_ctl_mode = ctl_modes_for11a;

        if (IS_CHAN_HT40(chan)) {
            num_ctl_modes = ARRAY_LENGTH(ctl_modes_for11a); /* All 5G CTL's */
        }
    }
    reduction = tp_scale_reduction_table[(AH_PRIVATE(ah)->ah_power_scale)];

    /*
     * For MIMO, need to apply regulatory caps individually across dynamically
     * running modes: CCK, OFDM, HT20, HT40
     *
     * The outer loop walks through each possible applicable runtime mode.
     * The inner loop walks through each ctl_index entry in EEPROM.
     * The ctl value is encoded as [7:4] == test group, [3:0] == test mode.
     *
     */
    for (ctl_mode = 0; ctl_mode < num_ctl_modes; ctl_mode++) {
        bool is_ht40_ctl_mode =
            (p_ctl_mode[ctl_mode] == CTL_5GHT40) ||
            (p_ctl_mode[ctl_mode] == CTL_2GHT40);
        if (is_ht40_ctl_mode) {
            freq = centers.synth_center;
        } else if (p_ctl_mode[ctl_mode] & EXT_ADDITIVE) {
            freq = centers.ext_center;
        } else {
            freq = centers.ctl_center;
        }

#ifdef _UNINET_REGDMN_
        /* uninet regdomain programs it's own CTLs */
        chan_mode = p_ctl_mode[ctl_mode] & CTL_MODE_M;
        min_ctl_power = (u_int8_t)AH_MIN(ctl_value[chan_mode], scaled_power);
#else
        HDPRINTF(ah, HAL_DBG_POWER_MGMT,
            "LOOP-Mode ctl_mode %d < %d, "
            "is_ht40_ctl_mode %d, EXT_ADDITIVE %d\n",
            ctl_mode, num_ctl_modes, is_ht40_ctl_mode,
            (p_ctl_mode[ctl_mode] & EXT_ADDITIVE));
        /* walk through each CTL index stored in EEPROM */
#ifndef ART_BUILD
        if (IS_CHAN_2GHZ(chan)) {
            ctl_index = p_eep_data->ctl_index_2g;
            ctl_num = OSPREY_NUM_CTLS_2G;
        } else {
            ctl_index = p_eep_data->ctl_index_5g;
            ctl_num = OSPREY_NUM_CTLS_5G;
        }
#else
        if (!AR_SREV_JET(ah)) {
            if (IS_CHAN_2GHZ(chan)) {
                ctl_index = p_eep_data->ctl_index_2g;
                ctl_num = OSPREY_NUM_CTLS_2G;
            } else {
                ctl_index = p_eep_data->ctl_index_5g;
                ctl_num = OSPREY_NUM_CTLS_5G;
            }
        }else{
            if (IS_CHAN_2GHZ(chan)) {
                ctl_index = p_eep_data_jet->ctl_index_2g;
                ctl_num = OSPREY_NUM_CTLS_2G;
            } else {
                ctl_index = p_eep_data_jet->ctl_index_5g;
                ctl_num = OSPREY_NUM_CTLS_5G;
            }
        }

#endif
        for (i = 0; (i < ctl_num) && ctl_index[i]; i++) {
            HDPRINTF(ah, HAL_DBG_POWER_MGMT,
                "  LOOP-Ctlidx %d: cfg_ctl 0x%2.2x p_ctl_mode 0x%2.2x "
                "ctl_index 0x%2.2x chan %d chanctl 0x%x\n",
                i, cfg_ctl, p_ctl_mode[ctl_mode], ctl_index[i],
                chan->channel, chan->conformance_test_limit);

            /*
             * compare test group from regulatory channel list
             * with test mode from p_ctl_mode list
             */
            if ((((cfg_ctl & ~CTL_MODE_M) |
                  (p_ctl_mode[ctl_mode] & CTL_MODE_M)) == ctl_index[i]) ||
                (((cfg_ctl & ~CTL_MODE_M) |
                  (p_ctl_mode[ctl_mode] & CTL_MODE_M)) ==
                 ((ctl_index[i] & CTL_MODE_M) | SD_NO_CTL)))
            {
                twice_min_edge_power =
                    ar9300_eep_def_get_max_edge_power(ah,
                        p_eep_data, freq, i, IS_CHAN_2GHZ(chan));

                HDPRINTF(ah, HAL_DBG_POWER_MGMT,
                    "    MATCH-EE_IDX %d: ch %d is2 %d "
                    "2xMinEdge %d chainmask %d chains %d\n",
                    i, freq, IS_CHAN_2GHZ(chan),
                    twice_min_edge_power, tx_chainmask,
                    ar9300_get_ntxchains(tx_chainmask));

                if ((cfg_ctl & ~CTL_MODE_M) == SD_NO_CTL) {
                    /*
                     * Find the minimum of all CTL edge powers
                     * that apply to this channel
                     */
                    twice_max_edge_power =
                        AH_MIN(twice_max_edge_power, twice_min_edge_power);
                } else {
                    /* specific */
                    twice_max_edge_power = twice_min_edge_power;
                    break;
                }
            }
        }

        min_ctl_power = (u_int8_t)AH_MIN(twice_max_edge_power, scaled_power);

        HDPRINTF(ah, HAL_DBG_POWER_MGMT,
            "    SEL-Min ctl_mode %d p_ctl_mode %d "
            "2xMaxEdge %d sP %d min_ctl_pwr %d\n",
            ctl_mode, p_ctl_mode[ctl_mode],
            twice_max_edge_power, scaled_power, min_ctl_power);
#endif  /* _UNINET_REGDMN_ */

        /* Apply ctl mode to correct target power set */
        switch (p_ctl_mode[ctl_mode]) {
        case CTL_11B:
            for (i = ALL_TARGET_LEGACY_1L_5L; i <= ALL_TARGET_LEGACY_11S; i++) {
                p_pwr_array[i] =
                    (u_int8_t)AH_MIN(p_pwr_array[i], min_ctl_power);
                ctlpwscale_reduction(p_pwr_array, reduction, i);
            }
            break;
        case CTL_11A:
        case CTL_11G:
            for (i = ALL_TARGET_LEGACY_6_24; i <= ALL_TARGET_LEGACY_54; i++) {
                p_pwr_array[i] =
                    (u_int8_t)AH_MIN(p_pwr_array[i], min_ctl_power);
#ifdef ATH_BT_COEX
                if ((ahp->ah_bt_coex_config_type == HAL_BT_COEX_CFG_3WIRE) ||
                    (ahp->ah_bt_coex_config_type == HAL_BT_COEX_CFG_MCI))
                {
                    if ((ahp->ah_bt_coex_flag & HAL_BT_COEX_FLAG_LOWER_TX_PWR)
                        && (ahp->ah_bt_wlan_isolation
                         < HAL_BT_COEX_ISOLATION_FOR_NO_COEX))
                    {

                        u_int8_t reduce_pow;

                        reduce_pow = (HAL_BT_COEX_ISOLATION_FOR_NO_COEX
                                     - ahp->ah_bt_wlan_isolation) << 1;

                        if (reduce_pow <= p_pwr_array[i]) {
                            p_pwr_array[i] -= reduce_pow;
                        }
                    }
                    if ((ahp->ah_bt_coex_flag &
                          HAL_BT_COEX_FLAG_LOW_ACK_PWR) &&
                          (i != ALL_TARGET_LEGACY_36) &&
                          (i != ALL_TARGET_LEGACY_48) &&
                          (i != ALL_TARGET_LEGACY_54) &&
                          (p_ctl_mode[ctl_mode] == CTL_11G))
                    {
                        p_pwr_array[i] = 0;
                    }
                }
#endif
                ctlpwscale_reduction(p_pwr_array, reduction, i);
            }
            break;
        case CTL_5GHT20:
        case CTL_2GHT20:
            for (i = ALL_TARGET_HT20_0_8_16_24; i <= ALL_TARGET_HT20_31; i++) {
                p_pwr_array[i] =
                    (u_int8_t)AH_MIN(p_pwr_array[i], min_ctl_power);
#ifdef ATH_BT_COEX
                if (((ahp->ah_bt_coex_config_type == HAL_BT_COEX_CFG_3WIRE) ||
                     (ahp->ah_bt_coex_config_type == HAL_BT_COEX_CFG_MCI)) &&
                    (ahp->ah_bt_coex_flag & HAL_BT_COEX_FLAG_LOWER_TX_PWR) &&
                    (ahp->ah_bt_wlan_isolation
                        < HAL_BT_COEX_ISOLATION_FOR_NO_COEX)) {

                    u_int8_t reduce_pow = (HAL_BT_COEX_ISOLATION_FOR_NO_COEX
                                           - ahp->ah_bt_wlan_isolation) << 1;

                    if (reduce_pow <= p_pwr_array[i]) {
                        p_pwr_array[i] -= reduce_pow;
                    }
                }
#if ATH_SUPPORT_MCI
                else if ((ahp->ah_bt_coex_flag &
                          HAL_BT_COEX_FLAG_MCI_MAX_TX_PWR) &&
                         (p_ctl_mode[ctl_mode] == CTL_2GHT20) &&
                         (ahp->ah_bt_coex_config_type == HAL_BT_COEX_CFG_MCI))
                {
                    u_int8_t max_pwr;

                    max_pwr = MS(mci_concur_tx_max_pwr[2][1],
                                 ATH_MCI_CONCUR_TX_LOWEST_PWR_MASK);
                    if (p_pwr_array[i] > max_pwr) {
                        p_pwr_array[i] = max_pwr;
                    }
                }
#endif
#endif
                ctlpwscale_reduction(p_pwr_array, reduction, i);
            }
            break;
        case CTL_11B_EXT:
            break;
        case CTL_11A_EXT:
        case CTL_11G_EXT:
            break;
        case CTL_5GHT40:
        case CTL_2GHT40:
            for (i = ALL_TARGET_HT40_0_8_16_24; i <= ALL_TARGET_HT40_31; i++) {
                p_pwr_array[i] = (u_int8_t)
                    AH_MIN(p_pwr_array[i], min_ctl_power);
#ifdef ATH_BT_COEX
                if (((ahp->ah_bt_coex_config_type == HAL_BT_COEX_CFG_3WIRE) ||
                     (ahp->ah_bt_coex_config_type == HAL_BT_COEX_CFG_MCI)) &&
                    (ahp->ah_bt_coex_flag & HAL_BT_COEX_FLAG_LOWER_TX_PWR) &&
                    (ahp->ah_bt_wlan_isolation
                        < HAL_BT_COEX_ISOLATION_FOR_NO_COEX)) {

                    u_int8_t reduce_pow = (HAL_BT_COEX_ISOLATION_FOR_NO_COEX
                                              - ahp->ah_bt_wlan_isolation) << 1;

                    if (reduce_pow <= p_pwr_array[i]) {
                        p_pwr_array[i] -= reduce_pow;
                    }
                }
#if ATH_SUPPORT_MCI
                else if ((ahp->ah_bt_coex_flag &
                          HAL_BT_COEX_FLAG_MCI_MAX_TX_PWR) &&
                         (p_ctl_mode[ctl_mode] == CTL_2GHT40) &&
                         (ahp->ah_bt_coex_config_type == HAL_BT_COEX_CFG_MCI))
                {
                    u_int8_t max_pwr;

                    max_pwr = MS(mci_concur_tx_max_pwr[3][1],
                                 ATH_MCI_CONCUR_TX_LOWEST_PWR_MASK);
                    if (p_pwr_array[i] > max_pwr) {
                        p_pwr_array[i] = max_pwr;
                    }
                }
#endif
#endif
                ctlpwscale_reduction(p_pwr_array, reduction, i);
            }
            break;
        default:
            HALASSERT(0);
            break;
        }
    } /* end ctl mode checking */

    return true;
#undef EXT_ADDITIVE
#undef CTL_11A_EXT
#undef CTL_11G_EXT
#undef CTL_11B_EXT
#undef REDUCE_SCALED_POWER_BY_TWO_CHAIN
#undef REDUCE_SCALED_POWER_BY_THREE_CHAIN
#undef REDUCE_SCALED_POWER_BY_FOUR_CHAINS
}

/**************************************************************
 * ar9300_eeprom_set_transmit_power
 *
 * Set the transmit power in the baseband for the given
 * operating channel and mode.
 */
HAL_STATUS
ar9300_eeprom_set_transmit_power(struct ath_hal *ah,
    ar9300_eeprom_t *p_eep_data, HAL_CHANNEL_INTERNAL *chan, u_int16_t cfg_ctl,
    u_int16_t antenna_reduction, u_int16_t twice_max_regulatory_power,
    u_int16_t power_limit)
{
#define ABS(_x, _y) ((int)_x > (int)_y ? (int)_x - (int)_y : (int)_y - (int)_x)
    u_int8_t target_power_val_t2[ar9300_rate_size];
    u_int8_t target_power_val_t2_eep[ar9300_rate_size]= {0};
    struct ath_hal_9300 *ahp = AH9300(ah);
    int  i = 0;
    u_int32_t tmp_paprd_rate_mask = 0, *tmp_ptr = NULL;
    int      paprd_scale_factor = 5;
    ar9300_eeprom_t *eep2 = NULL;
#ifdef ART_BUILD
    ar5500_eeprom_t *p_eep_data_jet = (ar5500_eeprom_t *) p_eep_data;
#endif

    u_int8_t *ptr_mcs_rate2power_table_index;
    u_int8_t mcs_rate2power_table_index_ht20[32] =
    {
        ALL_TARGET_HT20_0_8_16_24,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_4,
        ALL_TARGET_HT20_5,
        ALL_TARGET_HT20_6,
        ALL_TARGET_HT20_7,
        ALL_TARGET_HT20_0_8_16_24,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_12,
        ALL_TARGET_HT20_13,
        ALL_TARGET_HT20_14,
        ALL_TARGET_HT20_15,
        ALL_TARGET_HT20_0_8_16_24,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_20,
        ALL_TARGET_HT20_21,
        ALL_TARGET_HT20_22,
        ALL_TARGET_HT20_23,
        ALL_TARGET_HT20_0_8_16_24,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT20_28,
        ALL_TARGET_HT20_29,
        ALL_TARGET_HT20_30,
        ALL_TARGET_HT20_31
    };

    u_int8_t mcs_rate2power_table_index_ht40[32] =
    {
        ALL_TARGET_HT40_0_8_16_24,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_4,
        ALL_TARGET_HT40_5,
        ALL_TARGET_HT40_6,
        ALL_TARGET_HT40_7,
        ALL_TARGET_HT40_0_8_16_24,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_12,
        ALL_TARGET_HT40_13,
        ALL_TARGET_HT40_14,
        ALL_TARGET_HT40_15,
        ALL_TARGET_HT40_0_8_16_24,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_20,
        ALL_TARGET_HT40_21,
        ALL_TARGET_HT40_22,
        ALL_TARGET_HT40_23,
        ALL_TARGET_HT40_0_8_16_24,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_1_3_9_11_17_19_25_27,
        ALL_TARGET_HT40_28,
        ALL_TARGET_HT40_29,
        ALL_TARGET_HT40_30,
        ALL_TARGET_HT40_31
    };

    HDPRINTF(ah, HAL_DBG_CALIBRATE,
        "%s[%d] +++chan %d,cfgctl 0x%04x  "
        "antenna_reduction 0x%04x, twice_max_regulatory_power 0x%04x "
        "power_limit 0x%04x\n",
        __func__, __LINE__, chan->channel, cfg_ctl,
        antenna_reduction, twice_max_regulatory_power, power_limit);
    ar9300_set_target_power_from_eeprom(ah, chan->channel, target_power_val_t2);

    if (ar9300_eeprom_get(ahp, EEP_PAPRD_ENABLED)) {
#ifndef ART_BUILD
        if (IS_CHAN_2GHZ(chan)) {
            if (IS_CHAN_HT40(chan)) {
                tmp_paprd_rate_mask =
                    p_eep_data->modal_header_2g.paprd_rate_mask_ht40;
                tmp_ptr = &AH9300(ah)->ah_2g_paprd_rate_mask_ht40;
            } else {
                tmp_paprd_rate_mask =
                    p_eep_data->modal_header_2g.paprd_rate_mask_ht20;
                tmp_ptr = &AH9300(ah)->ah_2g_paprd_rate_mask_ht20;
            }
        } else {
            if (IS_CHAN_HT40(chan)) {
                tmp_paprd_rate_mask =
                    p_eep_data->modal_header_5g.paprd_rate_mask_ht40;
                tmp_ptr = &AH9300(ah)->ah_5g_paprd_rate_mask_ht40;
            } else {
                tmp_paprd_rate_mask =
                    p_eep_data->modal_header_5g.paprd_rate_mask_ht20;
                tmp_ptr = &AH9300(ah)->ah_5g_paprd_rate_mask_ht20;
            }
        }
        AH_PAPRD_GET_SCALE_FACTOR(
            paprd_scale_factor, p_eep_data, IS_CHAN_2GHZ(chan), chan->channel);
#else

        if (IS_CHAN_2GHZ(chan)) {
            if (IS_CHAN_HT40(chan)) {
                if (!AR_SREV_JET(ah)){
                    tmp_paprd_rate_mask =
                        p_eep_data->modal_header_2g.paprd_rate_mask_ht40;
                }else{
                    tmp_paprd_rate_mask =
                        p_eep_data_jet->modal_header_2g.paprd_rate_mask_ht40;
                }
                tmp_ptr = &AH9300(ah)->ah_2g_paprd_rate_mask_ht40;
            } else {
                if (!AR_SREV_JET(ah)){
                    tmp_paprd_rate_mask =
                        p_eep_data->modal_header_2g.paprd_rate_mask_ht20;
                }else{
                    tmp_paprd_rate_mask =
                        p_eep_data_jet->modal_header_2g.paprd_rate_mask_ht20;
                }
                tmp_ptr = &AH9300(ah)->ah_2g_paprd_rate_mask_ht20;
            }
        } else {
            if (IS_CHAN_HT40(chan)) {
                if (!AR_SREV_JET(ah)){
                    tmp_paprd_rate_mask =
                        p_eep_data->modal_header_5g.paprd_rate_mask_ht40;
                }else{
                    tmp_paprd_rate_mask =
                        p_eep_data_jet->modal_header_5g.paprd_rate_mask_ht40;
                }
                tmp_ptr = &AH9300(ah)->ah_5g_paprd_rate_mask_ht40;
            } else {
                if (!AR_SREV_JET(ah)){
                    tmp_paprd_rate_mask =
                        p_eep_data->modal_header_5g.paprd_rate_mask_ht20;
                }else{
                    tmp_paprd_rate_mask =
                        p_eep_data_jet->modal_header_5g.paprd_rate_mask_ht20;
                }
                tmp_ptr = &AH9300(ah)->ah_5g_paprd_rate_mask_ht20;
            }
        }
        AH_PAPRD_GET_SCALE_FACTOR(
            paprd_scale_factor, p_eep_data, IS_CHAN_2GHZ(chan), chan->channel);
#endif
        HDPRINTF(ah, HAL_DBG_CALIBRATE, "%s[%d] paprd_scale_factor %d\n",
            __func__, __LINE__, paprd_scale_factor);
        /* PAPRD is not done yet, Scale down the EEP power */
        if (IS_CHAN_HT40(chan)) {
            ptr_mcs_rate2power_table_index =
                &mcs_rate2power_table_index_ht40[0];
        } else {
            ptr_mcs_rate2power_table_index =
                &mcs_rate2power_table_index_ht20[0];
        }
        if (!chan->paprd_table_write_done) {
            //JET TODO: In Hal phy patch, they have not changed it to 32 from 24
            for (i = 0;  i < 32; i++) {
                /* PAPRD is done yet, so Scale down Power for PAPRD Rates*/
                if (tmp_paprd_rate_mask & (1 << i)) {
                    target_power_val_t2[ptr_mcs_rate2power_table_index[i]] -=
                        paprd_scale_factor;
                    HDPRINTF(ah, HAL_DBG_CALIBRATE,
                        "%s[%d]: Chan %d "
                        "Scale down target_power_val_t2[%d] = 0x%04x\n",
                        __func__, __LINE__,
                        chan->channel, i, target_power_val_t2[i]);
                }
            }
        } else {
            HDPRINTF(ah, HAL_DBG_CALIBRATE,
                "%s[%d]: PAPRD Done No TGT PWR Scaling\n", __func__, __LINE__);
        }
    }

    /* Save the Target power for future use */
    OS_MEMCPY(target_power_val_t2_eep, target_power_val_t2,
                                   sizeof(target_power_val_t2));
#ifndef ART_BUILD
    eep2 = p_eep_data;
#else
    if (!AR_SREV_JET(ah))
        eep2 = p_eep_data;
    else
        eep2 = (ar9300_eeprom_t *) p_eep_data_jet;
#endif
    ar9300_eeprom_set_power_per_rate_table(ah, eep2, chan,
                                     target_power_val_t2, cfg_ctl,
                                     antenna_reduction,
                                     twice_max_regulatory_power,
                                     power_limit, 0);

    /* Save this for quick lookup */
    ahp->reg_dmn = chan->conformance_test_limit;

    /*
     * Always use CDD/direct per rate power table for register based approach.
     * For FCC, CDD calculations should factor in the array gain, hence
     * this adjust call. ETSI and MKK does not have this requirement.
     */
    if (is_reg_dmn_fcc(ahp->reg_dmn)) {
        ar9300_adjust_reg_txpower_cdd(ah, target_power_val_t2);
    }

#ifndef ART_BUILD
    if (ar9300_eeprom_get(ahp, EEP_PAPRD_ENABLED)) {
        for (i = 0;  i < ar9300_rate_size; i++) {
            /*
             * EEPROM TGT PWR is not same as current TGT PWR,
             * so Disable PAPRD for this rate.
             * Some of APs might ask to reduce Target Power,
             * if target power drops significantly,
             * disable PAPRD for that rate.
             */
            if (tmp_paprd_rate_mask & (1 << i)) {
                if (ABS(target_power_val_t2_eep[i], target_power_val_t2[i]) >
                    paprd_scale_factor)
                {
                    tmp_paprd_rate_mask &= ~(1 << i);
                    HDPRINTF(ah, HAL_DBG_CALIBRATE,
                        "%s: EEP TPC[%02d] 0x%08x "
                        "Curr TPC[%02d] 0x%08x mask = 0x%08x\n",
                        __func__, i, target_power_val_t2_eep[i], i,
                        target_power_val_t2[i], tmp_paprd_rate_mask);
                }
            }

        }
        HDPRINTF(ah, HAL_DBG_CALIBRATE,
            "%s: Chan %d After tmp_paprd_rate_mask = 0x%08x\n",
            __func__, chan->channel, tmp_paprd_rate_mask);
        if (tmp_ptr) {
            *tmp_ptr = tmp_paprd_rate_mask;
        }
    }
#endif

    /* Write target power array to registers */
    ar9300_transmit_power_reg_write(ah, target_power_val_t2);

    /* Write target power for self generated frames to the TPC register */
    ar9300_selfgen_tpc_reg_write(ah, chan, target_power_val_t2);

    /* GreenTx or Paprd */
    if (AH_PRIVATE(ah)->ah_config.ath_hal_sta_update_tx_pwr_enable ||
        AH_PRIVATE(ah)->ah_caps.hal_paprd_enabled)
    {
        if (AR_SREV_POSEIDON(ah)) {
            /*For HAL_RSSI_TX_POWER_NONE array*/
            OS_MEMCPY(ahp->ah_default_tx_power,
                target_power_val_t2,
                sizeof(target_power_val_t2));
            /* Get defautl tx related register setting for GreenTx */
            /* Record OB/DB */
            AH_PRIVATE(ah)->ah_ob_db1[POSEIDON_STORED_REG_OBDB] =
                OS_REG_READ(ah, AR_PHY_65NM_CH0_TXRF2);
            /* Record TPC settting */
            AH_PRIVATE(ah)->ah_ob_db1[POSEIDON_STORED_REG_TPC] =
                OS_REG_READ(ah, AR_TPC);
            /* Record BB_powertx_rate9 setting */
            AH_PRIVATE(ah)->ah_ob_db1[POSEIDON_STORED_REG_BB_PWRTX_RATE9] =
                OS_REG_READ(ah, AR_PHY_BB_POWERTX_RATE9);
        }
    }

    /*
     * Return tx power used to iwconfig.
     * Since power is rate dependent, use one of the indices from the
     * AR9300_Rates enum to select an entry from target_power_val_t2[]
     * to report.
     * Currently returns the power for HT40 MCS 0, HT20 MCS 0, or OFDM 6 Mbps
     * as CCK power is less interesting (?).
     */
    i = ALL_TARGET_LEGACY_6_24;         /* legacy */
    if (IS_CHAN_HT40(chan)) {
        i = ALL_TARGET_HT40_0_8_16_24;     /* ht40 */
    } else if (IS_CHAN_HT20(chan)) {
        i = ALL_TARGET_HT20_0_8_16_24;     /* ht20 */
    }
    AH_PRIVATE(ah)->ah_max_power_level = target_power_val_t2[i];
    /* Adjusting the ah_max_power_level based on chains and antennaGain*/
    switch (ar9300_get_ntxchains(((ahp->ah_tx_chainmaskopt > 0) ?
                                    ahp->ah_tx_chainmaskopt : ahp->ah_tx_chainmask)))
    {
        case 1:
            break;
        case 2:
            if (is_reg_dmn_fcc(ahp->reg_dmn)) {
                AH_PRIVATE(ah)->ah_max_power_level += ahp->cdd_gain_comp[1];
            }
            AH_PRIVATE(ah)->ah_max_power_level += ahp->num_txchain_comp[1];
            break;
        case 3:
            if (is_reg_dmn_fcc(ahp->reg_dmn)) {
                AH_PRIVATE(ah)->ah_max_power_level += ahp->cdd_gain_comp[2];
            }
            AH_PRIVATE(ah)->ah_max_power_level += ahp->num_txchain_comp[2];
            break;
        case 4:
            if (is_reg_dmn_fcc(ahp->reg_dmn)) {
                AH_PRIVATE(ah)->ah_max_power_level += ahp->cdd_gain_comp[3];
            }
            AH_PRIVATE(ah)->ah_max_power_level += ahp->num_txchain_comp[3];
            break;
        default:
            HALASSERT(0); /* Unsupported number of chains */
    }

    ar9300_calibration_apply(ah, chan->channel);
#undef ABS

    /* Handle per packet TPC initializations */
    if (AH_PRIVATE(ah)->ah_config.ath_hal_desc_tpc) {
        /* Transmit Power per-rate per-chain  are  computed here. A separate
         * power table is maintained for different MIMO modes (i.e. TXBF ON,
         * STBC) to enable easy lookup during packet transmit.
         * The reason for maintaing each of these tables per chain is that
         * the transmit power used for different number of chains is different
         * depending on whether the power has been limited by the target power,
         * the regulatory domain  or the CTL limits.
         */
        u_int mode = ath_hal_get_curmode(ah, chan);
        u_int32_t val = 0;
        u_int8_t chainmasks[AR9300_MAX_CHAINS] =
            {OSPREY_1_CHAINMASK, OSPREY_2LOHI_CHAINMASK, OSPREY_3_CHAINMASK,JET_4_CHAINMASK};
        for (i = 0; i < AR9300_MAX_CHAINS; i++) {
            OS_MEMCPY(target_power_val_t2, target_power_val_t2_eep,
                                   sizeof(target_power_val_t2_eep));
#ifndef ART_BUILD
            eep2 = p_eep_data;
#else
            if (!AR_SREV_JET(ah))
                eep2 = p_eep_data;
            else
                eep2 = (ar9300_eeprom_t *) p_eep_data_jet;
#endif
            ar9300_eeprom_set_power_per_rate_table(ah, eep2, chan,
                                     target_power_val_t2, cfg_ctl,
                                     antenna_reduction,
                                     twice_max_regulatory_power,
                                     power_limit, chainmasks[i]);
            HDPRINTF(ah, HAL_DBG_POWER_MGMT,
                 " Channel = %d Chainmask = %d, Upper Limit = [%2d.%1d dBm]\n",
                                       chan->channel, i, ahp->upper_limit[i]/2,
                                       ahp->upper_limit[i]%2 * 5);
            ar9300_init_rate_txpower(ah, mode, chan, target_power_val_t2,
                                                           chainmasks[i]);

        }

        /* Enable TPC */
        OS_REG_WRITE(ah, AR_PHY_PWRTX_MAX, AR_PHY_PWRTX_MAX_TPC_ENABLE);
        /*
         * Disable per chain power reduction since we are already
         * accounting for this in our calculations
         */
        val = OS_REG_READ(ah, AR_PHY_POWER_TX_SUB);
        if (AR_SREV_WASP(ah)) {
            OS_REG_WRITE(ah, AR_PHY_POWER_TX_SUB,
                       val & AR_PHY_POWER_TX_SUB_2_DISABLE);
        } else if (AR_SREV_JET(ah)) {
            OS_REG_WRITE(ah, AR_PHY_POWER_TX_SUB,
                       val & QCN5500_PHY_POWER_TX_SUB_4_DISABLE);
        } else {
            OS_REG_WRITE(ah, AR_PHY_POWER_TX_SUB,
                       val & AR_PHY_POWER_TX_SUB_3_DISABLE);
        }
    }

    return HAL_OK;
}

/**************************************************************
 * ar9300_eeprom_set_addac
 *
 * Set the ADDAC from eeprom.
 */
void
ar9300_eeprom_set_addac(struct ath_hal *ah, HAL_CHANNEL_INTERNAL *chan)
{

    HDPRINTF(AH_NULL, HAL_DBG_UNMASKABLE,
                 "FIXME: ar9300_eeprom_def_set_addac called\n");
#if 0
    MODAL_EEPDEF_HEADER *p_modal;
    struct ath_hal_9300 *ahp = AH9300(ah);
    ar9300_eeprom_t *eep = &ahp->ah_eeprom.def;
    u_int8_t biaslevel;

    if (AH_PRIVATE(ah)->ah_mac_version != AR_SREV_VERSION_SOWL) {
        return;
    }

    HALASSERT(owl_get_eepdef_ver(ahp) == AR9300_EEP_VER);

    /* Xpa bias levels in eeprom are valid from rev 14.7 */
    if (owl_get_eepdef_rev(ahp) < AR9300_EEP_MINOR_VER_7) {
        return;
    }

    if (ahp->ah_emu_eeprom) {
        return;
    }

    p_modal = &(eep->modal_header[IS_CHAN_2GHZ(chan)]);

    if (p_modal->xpa_bias_lvl != 0xff) {
        biaslevel = p_modal->xpa_bias_lvl;
    } else {
        /* Use freqeuncy specific xpa bias level */
        u_int16_t reset_freq_bin, freq_bin, freq_count = 0;
        CHAN_CENTERS centers;

        ar9300_get_channel_centers(ah, chan, &centers);

        reset_freq_bin = FREQ2FBIN(centers.synth_center, IS_CHAN_2GHZ(chan));
        freq_bin = p_modal->xpa_bias_lvl_freq[0] & 0xff;
        biaslevel = (u_int8_t)(p_modal->xpa_bias_lvl_freq[0] >> 14);

        freq_count++;

        while (freq_count < 3) {
            if (p_modal->xpa_bias_lvl_freq[freq_count] == 0x0) {
                break;
            }

            freq_bin = p_modal->xpa_bias_lvl_freq[freq_count] & 0xff;
            if (reset_freq_bin >= freq_bin) {
                biaslevel =
                    (u_int8_t)(p_modal->xpa_bias_lvl_freq[freq_count] >> 14);
            } else {
                break;
            }
            freq_count++;
        }
    }

    /* Apply bias level to the ADDAC values in the INI array */
    if (IS_CHAN_2GHZ(chan)) {
        INI_RA(&ahp->ah_ini_addac, 7, 1) =
            (INI_RA(&ahp->ah_ini_addac, 7, 1) & (~0x18)) | biaslevel << 3;
    } else {
        INI_RA(&ahp->ah_ini_addac, 6, 1) =
            (INI_RA(&ahp->ah_ini_addac, 6, 1) & (~0xc0)) | biaslevel << 6;
    }
#endif
}

u_int
ar9300_eeprom_dump_support(struct ath_hal *ah, void **pp_e)
{
#ifndef ART_BUILD
    *pp_e = &(AH9300(ah)->ah_eeprom);
    return sizeof(ar9300_eeprom_t);
#else
    if(!AR_SREV_JET(ah)){
        *pp_e = &(AH9300(ah)->ah_eeprom);
        return sizeof(ar9300_eeprom_t);
    }else{
        *pp_e = &(AH9300(ah)->ah_eeprom_jet);
        return sizeof(ar5500_eeprom_t);
    }

#endif
}

u_int8_t
ar9300_eeprom_get_num_ant_config(struct ath_hal_9300 *ahp,
    HAL_FREQ_BAND freq_band)
{
#if 0
    ar9300_eeprom_t  *eep = &ahp->ah_eeprom.def;
    MODAL_EEPDEF_HEADER *p_modal =
        &(eep->modal_header[HAL_FREQ_BAND_2GHZ == freq_band]);
    BASE_EEPDEF_HEADER  *p_base  = &eep->base_eep_header;
    u_int8_t         num_ant_config;

    num_ant_config = 1; /* default antenna configuration */

    if (p_base->version >= 0x0E0D) {
        if (p_modal->use_ant1) {
            num_ant_config += 1;
        }
    }

    return num_ant_config;
#else
    return 1;
#endif
}

HAL_STATUS
ar9300_eeprom_get_ant_cfg(struct ath_hal_9300 *ahp, HAL_CHANNEL_INTERNAL *chan,
                   u_int8_t index, u_int16_t *config)
{
#if 0
    ar9300_eeprom_t  *eep = &ahp->ah_eeprom.def;
    MODAL_EEPDEF_HEADER *p_modal = &(eep->modal_header[IS_CHAN_2GHZ(chan)]);
    BASE_EEPDEF_HEADER  *p_base  = &eep->base_eep_header;

    switch (index) {
    case 0:
        *config = p_modal->ant_ctrl_common & 0xFFFF;
        return HAL_OK;
    case 1:
        if (p_base->version >= 0x0E0D) {
            if (p_modal->use_ant1) {
                *config = ((p_modal->ant_ctrl_common & 0xFFFF0000) >> 16);
                return HAL_OK;
            }
        }
        break;
    default:
        break;
    }
#endif
    return HAL_EINVAL;
}

u_int8_t*
ar9300_eeprom_get_cust_data(struct ath_hal_9300 *ahp)
{
    return (u_int8_t *)ahp;
}


static u_int16_t
ar9300_eeprom_get_spur_chan(struct ath_hal *ah, u_int16_t i, bool is_2ghz)
{
    u_int16_t   spur_val = AR_NO_SPUR;
#if 0
    struct ath_hal_9300 *ahp = AH9300(ah);
    ar9300_eeprom_t *eep = (ar9300_eeprom_t *)&ahp->ah_eeprom;

    HALASSERT(i <  AR_EEPROM_MODAL_SPURS );

    HDPRINTF(ah, HAL_DBG_ANI,
             "Getting spur idx %d is2Ghz. %d val %x\n",
             i, is_2ghz,
             AH_PRIVATE(ah)->ah_config.ath_hal_spur_chans[i][is_2ghz]);

    switch (AH_PRIVATE(ah)->ah_config.ath_hal_spur_mode) {
    case SPUR_DISABLE:
        /* returns AR_NO_SPUR */
        break;
    case SPUR_ENABLE_IOCTL:
        spur_val = AH_PRIVATE(ah)->ah_config.ath_hal_spur_chans[i][is_2ghz];
        HDPRINTF(ah, HAL_DBG_ANI,
            "Getting spur val from new loc. %d\n", spur_val);
        break;
    case SPUR_ENABLE_EEPROM:
        spur_val = eep->modal_header[is_2ghz].spur_chans[i].spur_chan;
        break;

    }
#endif
    return spur_val;
}

#ifndef ART_BUILD
u_int16_t
ar9300_eeprom_struct_size(void)
{
    return sizeof(ar9300_eeprom_t);
}

int osprey_eeprom_struct_default_many(void)
{
    return ARRAY_LENGTH(osprey_eeprom_9300);
}

ar9300_eeprom_t *ar9300_eeprom_struct_default(void)
{
    return jet_default_template;
}

osprey_eeprom_t *osprey_eeprom_struct_default(int default_index)
{
    if (default_index >= 0 &&
        default_index < ARRAY_LENGTH(osprey_eeprom_9300))
    {
        return osprey_eeprom_9300[default_index];
    } else {
        return 0;
    }
}

osprey_eeprom_t *osprey_eeprom_struct_default_find_by_id(int id)
{
    int it;

    for (it = 0; it < ARRAY_LENGTH(osprey_eeprom_9300); it++) {
        if (osprey_eeprom_9300[it] != 0 && osprey_eeprom_9300[it]->template_version == id) {
            return osprey_eeprom_9300[it];
        }
    }
    return 0;
}
#else
u_int16_t
ar9300_eeprom_struct_size(struct ath_hal *ah)
{
        if (!AR_SREV_JET(ah))
                return sizeof(ar9300_eeprom_t);
        else
                return sizeof(ar5500_eeprom_t);
}

int ar9300_eeprom_struct_default_many(void)
{
    return ARRAY_LENGTH(default9300);
}

ar5500_eeprom_t *ar5500_eeprom_struct_default(void)
{
    return jet_default_template;
}

ar9300_eeprom_t *ar9300_eeprom_struct_default(int default_index)
{
    if (default_index >= 0 &&
        default_index < ARRAY_LENGTH(default9300))
    {
        return default9300[default_index];
    } else {
        return 0;
    }
}

ar9300_eeprom_t *ar9300_eeprom_struct_default_find_by_id(int id)
{
    int it;

    for (it = 0; it < ARRAY_LENGTH(default9300); it++) {
        if (default9300[it] != 0 && default9300[it]->template_version == id) {
            return default9300[it];
        }
    }
    return 0;
}
#endif

bool
ar9300_calibration_data_read_flash(struct ath_hal *ah, long address,
    u_int8_t *buffer, int many)
{

    if (((address) < 0) || ((address + many) > AR9300_EEPROM_SIZE - 1)) {
        return false;
    }
#ifdef ART_BUILD
#ifdef MDK_AP          /* MDK_AP is defined only in NART AP build */

    int fd;
#ifdef K31_CALDATA
    extern int ath_k31_mode;
#endif

#ifdef K31_CALDATA
    extern ath_k31_mode;
    if (ath_k31_mode)
    {
        FILE *fp;
        if (instance == 0) {
            fp = fopen("/tmp/caldata2g", "rb");
        } else {
            fp = fopen("/tmp/caldata5g", "rb");
        }
        if (fp == NULL) {
            return false;
        }
        fseek(fp, address, SEEK_SET);
        fread(buffer, many, 1, fp);
        fclose(fp);
        return true;
    }
#endif

    if ((fd = open("/dev/caldata", O_RDWR)) < 0) {
            perror("Flash caldata sector is not present: Writing caldata in file system /tmp/ \n");
            int i;
            for(i=0;i<many;i++) *buffer++=0;
    }else {
            lseek(fd, address, SEEK_SET);
            if (read(fd, buffer, many) != many) {
                    perror("\n_read\n");
                    close(fd);
                    return false;
            }
            close(fd);
    }
    return true;
#endif   /* MDK_AP */
#endif
    return false;
}
#ifndef ART_BUILD
void copy_osprey_modal_eep_header_to_jet(JET_MODAL_EEP_HEADER *jet_modal_header, OSPREY_MODAL_EEP_HEADER *osprey_modal_header)
{
	jet_modal_header->ant_ctrl_common	 = osprey_modal_header->ant_ctrl_common;
	jet_modal_header->ant_ctrl_common2 	 = osprey_modal_header->ant_ctrl_common2;

	OS_MEMCPY(jet_modal_header->ant_ctrl_chain,osprey_modal_header->ant_ctrl_chain, 	sizeof(u_int16_t) * OSPREY_MAX_CHAINS);
	OS_MEMCPY(jet_modal_header->xatten1_db,    osprey_modal_header->xatten1_db, 		sizeof(u_int8_t) * OSPREY_MAX_CHAINS);
	OS_MEMCPY(jet_modal_header->xatten1_margin,osprey_modal_header->xatten1_margin, 	sizeof(u_int8_t) * OSPREY_MAX_CHAINS);
	jet_modal_header->temp_slope 		 = osprey_modal_header->temp_slope;
	jet_modal_header->voltSlope 		 = osprey_modal_header->voltSlope;
	OS_MEMCPY(jet_modal_header->spur_chans,    osprey_modal_header->spur_chans, 		sizeof(osprey_modal_header->spur_chans));
	OS_MEMCPY(jet_modal_header->noise_floor_thresh_ch, osprey_modal_header->noise_floor_thresh_ch, sizeof(int8_t) * OSPREY_MAX_CHAINS);
	OS_MEMCPY(jet_modal_header->obdboffst_aux1,osprey_modal_header->obdboffst_aux1, 	sizeof(u_int8_t) * OSPREY_MAX_CHAINS);
	OS_MEMCPY(jet_modal_header->obdboffst_aux2,osprey_modal_header->obdboffst_aux2, 	sizeof(u_int8_t) * OSPREY_MAX_CHAINS);
	OS_MEMCPY(jet_modal_header->reserved, 	   osprey_modal_header->reserved, 		sizeof(jet_modal_header->reserved));

	jet_modal_header->quick_drop		 = osprey_modal_header->quick_drop;
	jet_modal_header->xpa_bias_lvl 		 = osprey_modal_header->xpa_bias_lvl;
	jet_modal_header->tx_frame_to_data_start = osprey_modal_header->tx_frame_to_data_start;
	jet_modal_header->tx_frame_to_pa_on 	 = osprey_modal_header->tx_frame_to_pa_on;
	jet_modal_header->txClip 		 = osprey_modal_header->txClip;
	jet_modal_header->antenna_gain 		 = osprey_modal_header->antenna_gain;
	jet_modal_header->switchSettling 	 = osprey_modal_header->switchSettling;
	jet_modal_header->adcDesiredSize 	 = osprey_modal_header->adcDesiredSize;
	jet_modal_header->tx_end_to_xpa_off 	 = osprey_modal_header->tx_end_to_xpa_off;
	jet_modal_header->txEndToRxOn 		 = osprey_modal_header->txEndToRxOn;
	jet_modal_header->tx_frame_to_xpa_on 	 = osprey_modal_header->tx_frame_to_xpa_on;
	jet_modal_header->thresh62 		 = osprey_modal_header->thresh62;
	jet_modal_header->paprd_rate_mask_ht20   = osprey_modal_header->paprd_rate_mask_ht20;
	jet_modal_header->paprd_rate_mask_ht40 	 = osprey_modal_header->paprd_rate_mask_ht40;
	jet_modal_header->switchcomspdt 	 = osprey_modal_header->switchcomspdt;
	jet_modal_header->xLNA_bias_strength 	 = osprey_modal_header->xLNA_bias_strength;
	jet_modal_header->rf_gain_cap 		 = osprey_modal_header->rf_gain_cap;
	jet_modal_header->tx_gain_cap 		 = osprey_modal_header->tx_gain_cap;
	OS_MEMCPY(jet_modal_header->futureModal, osprey_modal_header->futureModal, 	sizeof(osprey_modal_header->futureModal));
}

void copy_osprey_base_extension_1_to_jet(JET_BASE_EXTENSION_1 *jet_base_ext1, OSPREY_BASE_EXTENSION_1 *osprey_base_ext1)
{
    jet_base_ext1->ant_div_control = osprey_base_ext1->ant_div_control;
    OS_MEMCPY(jet_base_ext1->future, osprey_base_ext1->future, sizeof(osprey_base_ext1->future));
    jet_base_ext1->misc_enable = osprey_base_ext1->misc_enable;
    OS_MEMCPY(jet_base_ext1->tempslopextension,osprey_base_ext1->tempslopextension,sizeof(u_int8_t) * MAX_TEMP_SLOPE);
    jet_base_ext1->quick_drop_low = osprey_base_ext1->quick_drop_low;
    jet_base_ext1->quick_drop_high = osprey_base_ext1->quick_drop_high;
}

void copy_osprey_base_extension_2_to_jet(JET_BASE_EXTENSION_2 *jet_base_ext2, OSPREY_BASE_EXTENSION_2 *osprey_base_ext2)
{
    jet_base_ext2->temp_slope_low 		= osprey_base_ext2->temp_slope_low;
    jet_base_ext2->temp_slope_high 		= osprey_base_ext2->temp_slope_high;
    OS_MEMCPY(jet_base_ext2->xatten1_db_low, 	osprey_base_ext2->xatten1_db_low, 	sizeof(u_int8_t) * OSPREY_MAX_CHAINS);
    OS_MEMCPY(jet_base_ext2->xatten1_margin_low,osprey_base_ext2->xatten1_margin_low, 	sizeof(u_int8_t) * OSPREY_MAX_CHAINS);
    OS_MEMCPY(jet_base_ext2->xatten1_db_high, 	osprey_base_ext2->xatten1_db_high,  	sizeof(u_int8_t) * OSPREY_MAX_CHAINS);
    OS_MEMCPY(jet_base_ext2->xatten1_margin_high,osprey_base_ext2->xatten1_margin_high,	sizeof(u_int8_t) * OSPREY_MAX_CHAINS);
}

void copy_osp_cal_target_power_ht_to_jet(JET_CAL_TARGET_POWER_HT *jet_cal_target_power,
		OSP_CAL_TARGET_POWER_HT *osprey_cal_target_power, u_int32_t size)
{
	u_int32_t i;
	for(i = 0; i < size; i++) {
		OS_MEMCPY(jet_cal_target_power->t_pow2x, osprey_cal_target_power->t_pow2x, sizeof(osprey_cal_target_power->t_pow2x));
		jet_cal_target_power++;
		osprey_cal_target_power++;
	}
}

void copy_osprey_to_jet(struct jetEeprom *jet_eeprom, struct ospreyEeprom *osprey_eeprom)
{


	copy_osprey_modal_eep_header_to_jet(&jet_eeprom->modal_header_2g, &osprey_eeprom->modal_header_2g);


	copy_osprey_modal_eep_header_to_jet(&jet_eeprom->modal_header_5g, &osprey_eeprom->modal_header_5g);
	copy_osprey_base_extension_1_to_jet(&jet_eeprom->base_ext1, &osprey_eeprom->base_ext1);
	copy_osprey_base_extension_2_to_jet(&jet_eeprom->base_ext2, &osprey_eeprom->base_ext2);
	copy_osp_cal_target_power_ht_to_jet(jet_eeprom->cal_target_power_2g_ht20, osprey_eeprom->cal_target_power_2g_ht20,
		OSPREY_NUM_2G_20_TARGET_POWERS);
	copy_osp_cal_target_power_ht_to_jet(jet_eeprom->cal_target_power_2g_ht40, osprey_eeprom->cal_target_power_2g_ht40,
		OSPREY_NUM_2G_40_TARGET_POWERS);
	copy_osp_cal_target_power_ht_to_jet(jet_eeprom->cal_target_power_5g_ht20, osprey_eeprom->cal_target_power_5g_ht20,
		OSPREY_NUM_5G_20_TARGET_POWERS);
	copy_osp_cal_target_power_ht_to_jet(jet_eeprom->cal_target_power_5g_ht40, osprey_eeprom->cal_target_power_5g_ht40,
		OSPREY_NUM_5G_40_TARGET_POWERS);

	jet_eeprom->eeprom_version 		= osprey_eeprom->eeprom_version;
	jet_eeprom->template_version 		= osprey_eeprom->template_version;

	OS_MEMCPY(jet_eeprom->mac_addr, osprey_eeprom->mac_addr, sizeof(osprey_eeprom->mac_addr));
	OS_MEMCPY(jet_eeprom->custData, osprey_eeprom->custData, sizeof(osprey_eeprom->custData));

	jet_eeprom->base_eep_header 		= osprey_eeprom->base_eep_header;

	OS_MEMCPY(jet_eeprom->cal_freq_pier_2g, 	osprey_eeprom->cal_freq_pier_2g, 	sizeof(osprey_eeprom->cal_freq_pier_2g));
	OS_MEMCPY(jet_eeprom->cal_pier_data_2g,		osprey_eeprom->cal_pier_data_2g,	sizeof(OSP_CAL_DATA_PER_FREQ_OP_LOOP) *
											OSPREY_MAX_CHAINS * OSPREY_NUM_2G_CAL_PIERS);

	OS_MEMCPY(jet_eeprom->cal_target_freqbin_cck, 	osprey_eeprom->cal_target_freqbin_cck, 	sizeof(osprey_eeprom->cal_target_freqbin_cck));
	OS_MEMCPY(jet_eeprom->cal_target_freqbin_2g, 	osprey_eeprom->cal_target_freqbin_2g, 	sizeof(osprey_eeprom->cal_target_freqbin_2g));
	OS_MEMCPY(jet_eeprom->cal_target_freqbin_2g_ht20,osprey_eeprom->cal_target_freqbin_2g_ht20, sizeof(osprey_eeprom->cal_target_freqbin_2g_ht20));
	OS_MEMCPY(jet_eeprom->cal_target_freqbin_2g_ht40,osprey_eeprom->cal_target_freqbin_2g_ht40, sizeof(osprey_eeprom->cal_target_freqbin_2g_ht40));
	OS_MEMCPY(jet_eeprom->cal_target_power_cck, 	osprey_eeprom->cal_target_power_cck, 	sizeof(osprey_eeprom->cal_target_power_cck));
	OS_MEMCPY(jet_eeprom->cal_target_power_2g, 	osprey_eeprom->cal_target_power_2g, 	sizeof(osprey_eeprom->cal_target_power_2g));
	OS_MEMCPY(jet_eeprom->ctl_index_2g, 		osprey_eeprom->ctl_index_2g, 		sizeof(osprey_eeprom->ctl_index_2g));
	OS_MEMCPY(jet_eeprom->ctl_freqbin_2G,		osprey_eeprom->ctl_freqbin_2G,		sizeof(osprey_eeprom->ctl_freqbin_2G));
	OS_MEMCPY(jet_eeprom->ctl_power_data_2g,	osprey_eeprom->ctl_power_data_2g, 	sizeof(osprey_eeprom->ctl_power_data_2g));
	OS_MEMCPY(jet_eeprom->cal_freq_pier_5g, 	osprey_eeprom->cal_freq_pier_5g, 	sizeof(osprey_eeprom->cal_freq_pier_5g));
	OS_MEMCPY(jet_eeprom->cal_pier_data_5g,		osprey_eeprom->cal_pier_data_5g,	sizeof(OSP_CAL_DATA_PER_FREQ_OP_LOOP) *
											OSPREY_MAX_CHAINS * OSPREY_NUM_5G_CAL_PIERS);
	OS_MEMCPY(jet_eeprom->cal_target_freqbin_5g, 	osprey_eeprom->cal_target_freqbin_5g,	sizeof(osprey_eeprom->cal_target_freqbin_5g));
	OS_MEMCPY(jet_eeprom->cal_target_freqbin_5g_ht20,osprey_eeprom->cal_target_freqbin_5g_ht20, sizeof(osprey_eeprom->cal_target_freqbin_5g_ht20));
	OS_MEMCPY(jet_eeprom->cal_target_freqbin_5g_ht40,osprey_eeprom->cal_target_freqbin_5g_ht40, sizeof(osprey_eeprom->cal_target_freqbin_5g_ht40));
	OS_MEMCPY(jet_eeprom->cal_target_power_5g, 	osprey_eeprom->cal_target_power_5g, 	sizeof(osprey_eeprom->cal_target_power_5g));
	OS_MEMCPY(jet_eeprom->ctl_index_5g, 		osprey_eeprom->ctl_index_5g, 		sizeof(osprey_eeprom->ctl_index_5g));
	OS_MEMCPY(jet_eeprom->ctl_freqbin_5G,		osprey_eeprom->ctl_freqbin_5G,		sizeof(osprey_eeprom->ctl_freqbin_5G));
	OS_MEMCPY(jet_eeprom->ctl_power_data_5g, 	osprey_eeprom->ctl_power_data_5g, 	sizeof(osprey_eeprom->ctl_power_data_5g));
}

void copy_jet_modal_eep_header_to_osprey(JET_MODAL_EEP_HEADER *jet_modal_header, OSPREY_MODAL_EEP_HEADER *osprey_modal_header)
{
    osprey_modal_header->ant_ctrl_common = jet_modal_header->ant_ctrl_common;
    osprey_modal_header->ant_ctrl_common2 = jet_modal_header->ant_ctrl_common2;
    OS_MEMCPY(osprey_modal_header->ant_ctrl_chain, jet_modal_header->ant_ctrl_chain, sizeof(u_int16_t) *OSPREY_MAX_CHAINS);
    OS_MEMCPY(osprey_modal_header->xatten1_db, jet_modal_header->xatten1_db, sizeof(u_int8_t) *OSPREY_MAX_CHAINS);
    OS_MEMCPY(osprey_modal_header->xatten1_margin, jet_modal_header->xatten1_margin, sizeof(u_int8_t) *OSPREY_MAX_CHAINS);
    osprey_modal_header->temp_slope = jet_modal_header->temp_slope;
    osprey_modal_header->voltSlope = jet_modal_header->voltSlope;
    OS_MEMCPY(osprey_modal_header->spur_chans, jet_modal_header->spur_chans, sizeof(osprey_modal_header->spur_chans));
    OS_MEMCPY(osprey_modal_header->noise_floor_thresh_ch, jet_modal_header->noise_floor_thresh_ch, sizeof(int8_t) *OSPREY_MAX_CHAINS);
    OS_MEMCPY(osprey_modal_header->obdboffst_aux1, jet_modal_header->obdboffst_aux1, sizeof(u_int8_t) *OSPREY_MAX_CHAINS);
    OS_MEMCPY(osprey_modal_header->obdboffst_aux2, jet_modal_header->obdboffst_aux2, sizeof(u_int8_t) *OSPREY_MAX_CHAINS);
    OS_MEMCPY(osprey_modal_header->reserved, jet_modal_header->reserved, sizeof(jet_modal_header->reserved));
    osprey_modal_header->quick_drop = jet_modal_header->quick_drop;
    osprey_modal_header->xpa_bias_lvl = jet_modal_header->xpa_bias_lvl;
    osprey_modal_header->tx_frame_to_data_start = jet_modal_header->tx_frame_to_data_start;
    osprey_modal_header->tx_frame_to_pa_on = jet_modal_header->tx_frame_to_pa_on;
    osprey_modal_header->txClip = jet_modal_header->txClip;
    osprey_modal_header->antenna_gain = jet_modal_header->antenna_gain;
    osprey_modal_header->switchSettling = jet_modal_header->switchSettling;
    osprey_modal_header->adcDesiredSize = jet_modal_header->adcDesiredSize;
    osprey_modal_header->tx_end_to_xpa_off = jet_modal_header->tx_end_to_xpa_off;
    osprey_modal_header->txEndToRxOn = jet_modal_header->txEndToRxOn;
    osprey_modal_header->tx_frame_to_xpa_on = jet_modal_header->tx_frame_to_xpa_on;
    osprey_modal_header->thresh62 = jet_modal_header->thresh62;
    osprey_modal_header->paprd_rate_mask_ht20 = jet_modal_header->paprd_rate_mask_ht20;
    osprey_modal_header->paprd_rate_mask_ht40 = jet_modal_header->paprd_rate_mask_ht40;
    osprey_modal_header->switchcomspdt = jet_modal_header->switchcomspdt;
    osprey_modal_header->xLNA_bias_strength = jet_modal_header->xLNA_bias_strength;
    osprey_modal_header->rf_gain_cap = jet_modal_header->rf_gain_cap;
    osprey_modal_header->tx_gain_cap = jet_modal_header->tx_gain_cap;
    OS_MEMCPY(osprey_modal_header->futureModal, jet_modal_header->futureModal, sizeof(osprey_modal_header->futureModal));
}

void copy_jet_base_extension_1_to_osprey(JET_BASE_EXTENSION_1 *jet_base_ext1, OSPREY_BASE_EXTENSION_1 *osprey_base_ext1)
{
    osprey_base_ext1->ant_div_control = jet_base_ext1->ant_div_control;
    OS_MEMCPY(osprey_base_ext1->future, jet_base_ext1->future, sizeof(u_int8_t));
    osprey_base_ext1->misc_enable = jet_base_ext1->misc_enable;
    OS_MEMCPY(osprey_base_ext1->tempslopextension, jet_base_ext1->tempslopextension, sizeof(u_int8_t) *MAX_TEMP_SLOPE);
    osprey_base_ext1->quick_drop_low = jet_base_ext1->quick_drop_low;
    osprey_base_ext1->quick_drop_high = jet_base_ext1->quick_drop_high;
}

void copy_jet_base_extension_2_to_osprey(JET_BASE_EXTENSION_2 *jet_base_ext2, OSPREY_BASE_EXTENSION_2 *osprey_base_ext2)
{
    osprey_base_ext2->temp_slope_low = jet_base_ext2->temp_slope_low;
    osprey_base_ext2->temp_slope_high = jet_base_ext2->temp_slope_high;
    OS_MEMCPY(osprey_base_ext2->xatten1_db_low, jet_base_ext2->xatten1_db_low, sizeof(u_int8_t) *OSPREY_MAX_CHAINS);
    OS_MEMCPY(osprey_base_ext2->xatten1_margin_low, jet_base_ext2->xatten1_margin_low, sizeof(u_int8_t) *OSPREY_MAX_CHAINS);
    OS_MEMCPY(osprey_base_ext2->xatten1_db_high, jet_base_ext2->xatten1_db_high, sizeof(u_int8_t) *OSPREY_MAX_CHAINS);
    OS_MEMCPY(osprey_base_ext2->xatten1_margin_high, jet_base_ext2->xatten1_margin_high, sizeof(u_int8_t) *OSPREY_MAX_CHAINS);
}

void copy_jet_cal_target_power_ht_to_osprey(JET_CAL_TARGET_POWER_HT *jet_cal_target_power, OSP_CAL_TARGET_POWER_HT *osprey_cal_target_power, u_int32_t size)
{
    u_int32_t i;
    for (i = 0;i<size;i++)
    {
        OS_MEMCPY(osprey_cal_target_power->t_pow2x, jet_cal_target_power->t_pow2x, sizeof(osprey_cal_target_power->t_pow2x));
        jet_cal_target_power++;
        osprey_cal_target_power++;
    }
}

void copy_jet_to_osprey(struct jetEeprom *jet_eeprom, struct ospreyEeprom *osprey_eeprom)
{
    copy_jet_modal_eep_header_to_osprey(&jet_eeprom->modal_header_2g, &osprey_eeprom->modal_header_2g);
    copy_jet_modal_eep_header_to_osprey(&jet_eeprom->modal_header_5g, &osprey_eeprom->modal_header_5g);
    copy_jet_base_extension_1_to_osprey(&jet_eeprom->base_ext1, &osprey_eeprom->base_ext1);
    copy_jet_base_extension_2_to_osprey(&jet_eeprom->base_ext2, &osprey_eeprom->base_ext2);
    copy_jet_cal_target_power_ht_to_osprey(jet_eeprom->cal_target_power_2g_ht20, osprey_eeprom->cal_target_power_2g_ht20, OSPREY_NUM_2G_20_TARGET_POWERS);
    copy_jet_cal_target_power_ht_to_osprey(jet_eeprom->cal_target_power_2g_ht40, osprey_eeprom->cal_target_power_2g_ht40, OSPREY_NUM_2G_40_TARGET_POWERS);
    copy_jet_cal_target_power_ht_to_osprey(jet_eeprom->cal_target_power_5g_ht20, osprey_eeprom->cal_target_power_5g_ht20, OSPREY_NUM_5G_20_TARGET_POWERS);
    copy_jet_cal_target_power_ht_to_osprey(jet_eeprom->cal_target_power_5g_ht40, osprey_eeprom->cal_target_power_5g_ht40, OSPREY_NUM_5G_40_TARGET_POWERS);
    osprey_eeprom->eeprom_version = jet_eeprom->eeprom_version;
    osprey_eeprom->template_version = jet_eeprom->template_version;
    OS_MEMCPY(osprey_eeprom->mac_addr, jet_eeprom->mac_addr, sizeof(osprey_eeprom->mac_addr));
    OS_MEMCPY(osprey_eeprom->custData, jet_eeprom->custData, sizeof(osprey_eeprom->custData));
    osprey_eeprom->base_eep_header = jet_eeprom->base_eep_header;
    OS_MEMCPY(osprey_eeprom->cal_freq_pier_2g, jet_eeprom->cal_freq_pier_2g, sizeof(osprey_eeprom->cal_freq_pier_2g));
    OS_MEMCPY(osprey_eeprom->cal_pier_data_2g, jet_eeprom->cal_pier_data_2g, sizeof(OSP_CAL_DATA_PER_FREQ_OP_LOOP) *OSPREY_MAX_CHAINS * OSPREY_NUM_2G_CAL_PIERS);
    OS_MEMCPY(osprey_eeprom->cal_target_freqbin_cck, jet_eeprom->cal_target_freqbin_cck, sizeof(osprey_eeprom->cal_target_freqbin_cck));
    OS_MEMCPY(osprey_eeprom->cal_target_freqbin_2g, jet_eeprom->cal_target_freqbin_2g, sizeof(osprey_eeprom->cal_target_freqbin_2g));
    OS_MEMCPY(osprey_eeprom->cal_target_freqbin_2g_ht20, jet_eeprom->cal_target_freqbin_2g_ht20, sizeof(osprey_eeprom->cal_target_freqbin_2g_ht20));
    OS_MEMCPY(osprey_eeprom->cal_target_freqbin_2g_ht40, jet_eeprom->cal_target_freqbin_2g_ht40, sizeof(osprey_eeprom->cal_target_freqbin_2g_ht40));
    OS_MEMCPY(osprey_eeprom->cal_target_power_cck, jet_eeprom->cal_target_power_cck, sizeof(osprey_eeprom->cal_target_power_cck));
    OS_MEMCPY(osprey_eeprom->cal_target_power_2g, jet_eeprom->cal_target_power_2g, sizeof(osprey_eeprom->cal_target_power_2g));
    OS_MEMCPY(osprey_eeprom->ctl_index_2g, jet_eeprom->ctl_index_2g, sizeof(osprey_eeprom->ctl_index_2g));
    OS_MEMCPY(osprey_eeprom->ctl_freqbin_2G, jet_eeprom->ctl_freqbin_2G, sizeof(osprey_eeprom->ctl_freqbin_2G));
    OS_MEMCPY(osprey_eeprom->ctl_power_data_2g, jet_eeprom->ctl_power_data_2g, sizeof(osprey_eeprom->ctl_power_data_2g));
    OS_MEMCPY(osprey_eeprom->cal_freq_pier_5g, jet_eeprom->cal_freq_pier_5g, sizeof(osprey_eeprom->cal_freq_pier_5g));
    OS_MEMCPY(osprey_eeprom->cal_pier_data_5g, jet_eeprom->cal_pier_data_5g, sizeof(OSP_CAL_DATA_PER_FREQ_OP_LOOP) *OSPREY_MAX_CHAINS * OSPREY_NUM_5G_CAL_PIERS);
    OS_MEMCPY(osprey_eeprom->cal_target_freqbin_5g, jet_eeprom->cal_target_freqbin_5g, sizeof(osprey_eeprom->cal_target_freqbin_5g));
    OS_MEMCPY(osprey_eeprom->cal_target_freqbin_5g_ht20, jet_eeprom->cal_target_freqbin_5g_ht20, sizeof(osprey_eeprom->cal_target_freqbin_5g_ht20));
    OS_MEMCPY(osprey_eeprom->cal_target_freqbin_5g_ht40, jet_eeprom->cal_target_freqbin_5g_ht40, sizeof(osprey_eeprom->cal_target_freqbin_5g_ht40));
    OS_MEMCPY(osprey_eeprom->cal_target_power_5g, jet_eeprom->cal_target_power_5g, sizeof(osprey_eeprom->cal_target_power_5g));
    OS_MEMCPY(osprey_eeprom->ctl_index_5g, jet_eeprom->ctl_index_5g, sizeof(osprey_eeprom->ctl_index_5g));
    OS_MEMCPY(osprey_eeprom->ctl_freqbin_5G, jet_eeprom->ctl_freqbin_5G, sizeof(osprey_eeprom->ctl_freqbin_5G));
    OS_MEMCPY(osprey_eeprom->ctl_power_data_5g, jet_eeprom->ctl_power_data_5g, sizeof(osprey_eeprom->ctl_power_data_5g));
}
#endif
bool
ar9300_calibration_data_read_eeprom(struct ath_hal *ah, long address,
    u_int8_t *buffer, int many)
{
    int i;
    u_int8_t value[2];
    unsigned long eep_addr;
    unsigned long byte_addr;
    u_int16_t *svalue;
    struct ath_hal_9300 *ahp = AH9300(ah);

    if (((address) < 0) || ((address + many) > AR9300_EEPROM_SIZE)) {
        return false;
    }

    for (i = 0; i < many; i++) {
        eep_addr = (u_int16_t) (address + i) / 2;
        byte_addr = (u_int16_t) (address + i) % 2;
        svalue = (u_int16_t *) value;
        if (!ahp->ah_priv.priv.ah_eeprom_read(ah, eep_addr, svalue)) {
            HDPRINTF(ah, HAL_DBG_EEPROM,
                "%s: Unable to read eeprom region \n", __func__);
            return false;
        }
        buffer[i] = (*svalue >> (8 * byte_addr)) & 0xff;
    }
    return true;
}

bool
ar9300_calibration_data_read_otp(struct ath_hal *ah, long address,
    u_int8_t *buffer, int many, bool is_wifi)
{
    int i;
    unsigned long eep_addr;
    unsigned long byte_addr;
    u_int32_t svalue;

    if (((address) < 0) || ((address + many) > 0x400)) {
        return false;
    }

    for (i = 0; i < many; i++) {
        eep_addr = (u_int16_t) (address + i) / 4; /* otp is 4 bytes long???? */
        byte_addr = (u_int16_t) (address + i) % 4;
        if (!ar9300_otp_read(ah, eep_addr, &svalue, is_wifi)) {
            HDPRINTF(ah, HAL_DBG_EEPROM,
                "%s: Unable to read otp region \n", __func__);
            return false;
        }
        buffer[i] = (svalue >> (8 * byte_addr)) & 0xff;
    }
    return true;
}

#ifdef ATH_CAL_NAND_FLASH
bool
ar9300_calibration_data_read_nand(struct ath_hal *ah, long address,
    u_int8_t *buffer, int many)
{
    int ret_val = 1;

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,4,103)
    int ret_len;
      /* Calling OS based API to read NAND */
    ret_val = OS_NAND_FLASH_READ(ATH_CAL_NAND_PARTITION, address, many, &ret_len, buffer);
#else
	printk("ERROR: OS_NAND_FLASH_READ is not supported in new kernel. Please handle\n");
	ret_val =  -EINVAL;
#endif

    return (ret_val ? AH_FALSE: AH_TRUE);
}
#endif

#ifdef QCA_PARTNER_PLATFORM
#define qdf_str_lcopy strncpy
#endif
#ifndef ART_BUILD
/*
 * Function to dump OSPREY_BASE_EEP_HEADER
 */
void dump_base_eep_header(OSPREY_BASE_EEP_HEADER *base_eep_header)
{
    printk(" reg_dmn 	    : [%d],[%x] [%d] \n", base_eep_header->reg_dmn[0],  base_eep_header->reg_dmn[1],
	    base_eep_header->reg_dmn[1]);
    printk(" txrx_mask 	    : [%d] \n", base_eep_header->txrx_mask);

    printk("op_cap_flags \n");
    printk("op_flags 	    : [%d] \n", base_eep_header->op_cap_flags.op_flags);
    printk("eepMisc  	    : [%d] \n", base_eep_header->op_cap_flags.eepMisc);


    printk("rf_silent 	    : [%d] \n", base_eep_header->rf_silent);
    printk("blue_tooth_options  : [%d] \n", base_eep_header->blue_tooth_options);
    printk("device_cap 	    : [%d] \n", base_eep_header->device_cap);
    printk("device_type  	    : [%d] \n", base_eep_header->device_type);
    printk("pwrTableOffset	    : [%d] \n", base_eep_header->pwrTableOffset);
    printk("params_for_tuning_caps[0],[1] : [%d] [%d] \n", base_eep_header->params_for_tuning_caps[0],
	    base_eep_header->params_for_tuning_caps[1]);
    printk("feature_enable      : [%d] \n", base_eep_header->feature_enable);
    printk("misc_configuration  : [%d] \n", base_eep_header->misc_configuration);
    printk("eeprom_write_enable_gpio  : [%d] \n", base_eep_header->eeprom_write_enable_gpio);
    printk("wlan_disable_gpio   : [%d] \n", base_eep_header->wlan_disable_gpio);
    printk("wlan_led_gpio	    : [%d] \n", base_eep_header->wlan_led_gpio);
    printk("rx_band_select_gpio : [%d] \n", base_eep_header->rx_band_select_gpio);
    printk("txrxgain	    : [%d] \n", base_eep_header->txrxgain);
    printk("swreg		    : [%d] \n", base_eep_header->swreg);
}

/*
 * Function to dump JET_MODAL_EEP_HEADER
 */
void dump_jet_modal_eep_header(JET_MODAL_EEP_HEADER *modal_eep_header)
{
    int i = 0;

    printk(" ant_ctrl_common  			: %x \n", modal_eep_header->ant_ctrl_common);
    printk(" ant_ctrl_common2 			: %x \n", modal_eep_header->ant_ctrl_common2);

    printk(" \n ant_ctrl_chain \n");
    for(i=0; i < JET_MAX_CHAINS; i++)
    {
	printk("[%x] ", modal_eep_header->ant_ctrl_chain[i]);
    }
    printk("\n");
    printk(" xatten1_db \n");
    for(i=0; i < JET_MAX_CHAINS; i++)
    {
	printk("[%d] ", modal_eep_header->xatten1_db[i]);
    }
    printk("\n xatten1_margin \n");
    for(i=0; i < JET_MAX_CHAINS; i++)
    {
	printk("[%d] ", modal_eep_header->xatten1_margin[i]);
    }

    printk(" temp_slope 				: %d \n", modal_eep_header->temp_slope);
    printk(" voltSlope 				: %d \n", modal_eep_header->voltSlope);

    printk("\n spur_chans \n");
    for(i=0; i < OSPREY_EEPROM_MODAL_SPURS; i++)
    {
	printk(" [%d] ", modal_eep_header->spur_chans[i]);
    }

    printk("\n noise_floor_thresh_ch \n");
    for(i=0; i < JET_MAX_CHAINS; i++)
    {
	printk(" [%d] ", modal_eep_header->noise_floor_thresh_ch[i]);
    }

    printk("\n obdboffst_aux1 \n");
    for(i=0; i < JET_MAX_CHAINS; i++)
    {
	printk(" [%d] ", modal_eep_header->obdboffst_aux1[i]);
    }

    printk("\n obdboffst_aux2 \n");
    for(i=0; i < JET_MAX_CHAINS; i++)
    {
	printk(" [%d] ", modal_eep_header->obdboffst_aux2[i]);
    }

    printk("\n reserved \n");
    for(i=0; i < JET_MAX_MODAL_RESERVED; i++)
    {
	printk(" [%d] ", modal_eep_header->reserved[i]);
    }

    printk("\n quick_drop 				: %d \n", modal_eep_header->quick_drop);
    printk(" xpa_bias_lvl 				: %d \n", modal_eep_header->xpa_bias_lvl);
    printk(" tx_frame_to_data_start 		: %d \n", modal_eep_header->tx_frame_to_data_start);
    printk(" tx_frame_to_pa_on 			: %d \n", modal_eep_header->tx_frame_to_pa_on);
    printk(" txClip 				: %d \n", modal_eep_header->txClip);
    printk(" antenna_gain	 			: %d \n", modal_eep_header->antenna_gain);
    printk(" switchSettling 			: %d \n", modal_eep_header->switchSettling);
    printk(" adcDesiredSize 			: %d \n", modal_eep_header->adcDesiredSize);
    printk(" tx_end_to_xpa_off 			: %d \n", modal_eep_header->tx_end_to_xpa_off);
    printk(" txEndToRxOn 				: %d \n", modal_eep_header->txEndToRxOn);
    printk(" tx_frame_to_xpa_on 			: %d \n", modal_eep_header->tx_frame_to_xpa_on);
    printk(" thresh62 				: %d \n", modal_eep_header->thresh62);
    printk(" paprd_rate_mask_ht20 			: %x \n", modal_eep_header->paprd_rate_mask_ht20);
    printk(" paprd_rate_mask_4ss_ht20 		: %x \n", modal_eep_header->paprd_rate_mask_4ss_ht20);
    printk(" paprd_rate_mask_ht40			: %x \n", modal_eep_header->paprd_rate_mask_ht40);
    printk(" paprd_rate_mask_4ss_ht40		: %x \n", modal_eep_header->paprd_rate_mask_4ss_ht40);
    printk(" switchcomspdt 				: %d \n", modal_eep_header->switchcomspdt);
    printk(" xLNA_bias_strength  			: %d \n", modal_eep_header->xLNA_bias_strength);
    printk(" rf_gain_cap 				: %d \n", modal_eep_header->rf_gain_cap);
    printk(" tx_gain_cap 				: %d \n", modal_eep_header->tx_gain_cap);

    printk("\n  futureModal \n");
    for(i=0; i < JET_MAX_MODAL_FUTURE; i++) {
	    printk(" [%d] ", modal_eep_header->futureModal[i]);
    }
    printk("\n xtal_ppm_target : %d \n", modal_eep_header->xtal_ppm_target);
    printk(" xtal_ppm_tolerance : %d \n", modal_eep_header->xtal_ppm_tolerance);
    printk(" temp slope for chain3: %d \n", modal_eep_header->temp_slope_high_3);
    printk(" iq mask : %d \n", modal_eep_header->eep_iqmask);
}

/*
 * Function to dump JET_BASE_EXTENSION_1
 */
void dump_jet_base_extension_1(JET_BASE_EXTENSION_1 *base_ext1)
{
    int i = 0;
    printk(" ant_div_control 		: %d \n", base_ext1->ant_div_control);

    printk(" future \n");
    for(i = 0; i < JET_MAX_BASE_EXTENSION_FUTURE; i++)
    {
	printk(" [%d] ", base_ext1->future[i]);
    }
    printk(" eep_paprd_trainning_power \n");
    printk(" eep_paprd_trainning_power: %d \n", base_ext1->eep_paprd_trainning_power);

    printk(" misc_enable 		: %d \n", base_ext1->misc_enable);

    printk(" tempslopextension \n");
    for(i = 0; i < MAX_TEMP_SLOPE; i++)
    {
	printk(" [%d] ", base_ext1->tempslopextension[i]);
    }
    printk(" quick_drop_low 		 : %d \n", base_ext1->quick_drop_low);
    printk(" quick_drop_high 		 : %d \n", base_ext1->quick_drop_high);
}

/*
 * Function to dump OSP_CAL_DATA_PER_FREQ_OP_LOOP
 */
void dump_jet_cal_data_per_freq_op_loop(u_int32_t row, u_int32_t column, OSP_CAL_DATA_PER_FREQ_OP_LOOP cal_data[row][column])
{
    int i, j;

    printk(" dump_OSP_CAL_DATA_PER_FREQ_OP_LOOP \n");
    for( i = 0; i < row; i++) {
	printk(" i : %d \n" , i);
	for( j = 0; j < column; j++) {
	    printk( "ref_power 		: %d \n", cal_data[i][j].ref_power);
	    printk( "volt_meas 		: %d \n", cal_data[i][j].volt_meas);
	    printk( "temp_meas 		: %d \n", cal_data[i][j].temp_meas);
	    printk( "rx_noisefloor_cal 	: %d \n", cal_data[i][j].rx_noisefloor_cal);
	    printk( "rx_noisefloor_power: %d \n", cal_data[i][j].rx_noisefloor_power);
	    printk( "rxTempMeas 	: %d \n", cal_data[i][j].rxTempMeas);
	}
    }
}

/*
 * Function to dump CAL_TARGET_POWER_LEG
 */
void dump_cal_target_power_leg(CAL_TARGET_POWER_LEG *power_leg, int len)
{
    int i = 0;
    for( i= 0; i < len; i++)
    {
	printk(" tPow2x   : [%d] [%d] [%d] [%d]\n", power_leg->t_pow2x[0],
		power_leg->t_pow2x[1],
		power_leg->t_pow2x[2],
		power_leg->t_pow2x[3]);
	power_leg++;
    }
}

/*
 * Function to dump JET_CAL_TARGET_POWER_HT
 */
void dump_jet_cal_target_power_ht(JET_CAL_TARGET_POWER_HT *power_ht, int len)
{
    int i;
    printk("\n OSP_CAL_TARGET_POWER_HT \n");
    for( i= 0; i < len; i++)
    {
	printk("[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d]:[%d] \n",
		power_ht->t_pow2x[0],
		power_ht->t_pow2x[1],
		power_ht->t_pow2x[2],
		power_ht->t_pow2x[3],
		power_ht->t_pow2x[4],
		power_ht->t_pow2x[5],
		power_ht->t_pow2x[6],
		power_ht->t_pow2x[7],
		power_ht->t_pow2x[8],
		power_ht->t_pow2x[9],
		power_ht->t_pow2x[10],
		power_ht->t_pow2x[11],
		power_ht->t_pow2x[12],
		power_ht->t_pow2x[13],
		power_ht->t_pow2x[14],
		power_ht->t_pow2x[15],
		power_ht->t_pow2x[16],
		power_ht->t_pow2x[17]);
	power_ht++;
    }
}

/*
 * Function to dump OSP_CAL_CTL_DATA_2G
 */
void dump_osp_cal_ctl_data_2g(OSP_CAL_CTL_DATA_2G *power_data, int len)
{
    int i, j;
    for(i=0; i< len; i++)
    {
	for(j=0;j<OSPREY_NUM_BAND_EDGES_2G;j++)
	{
	    printk("flag 	: %d \n", power_data->ctl_edges[j].flag);
	    printk("t_power : %d \n", power_data->ctl_edges[j].t_power);
	}
	power_data++;
    }
}

/*
 * Function to dump OSP_CAL_CTL_DATA_5G
 */
void dump_osp_cal_ctl_data_5g(OSP_CAL_CTL_DATA_5G *power_data, int len)
{
    int i, j;
    for(i=0; i< len; i++)
    {
	for(j=0;j<OSPREY_NUM_BAND_EDGES_5G;j++)
	{
	    printk("flag 	: %d \n", power_data->ctl_edges[j].flag);
	    printk("t_power : %d \n", power_data->ctl_edges[j].t_power);
	}
	power_data++;
    }
}


/*
 * Function to dump JET_BASE_EXTENSION_2
 */
void dump_jet_base_extension_2(JET_BASE_EXTENSION_2 *base_ext2)
{
    int i = 0;

    printk(" temp_slope_low 		 : %d \n", base_ext2->temp_slope_low);
    printk(" temp_slope_high 		 : %d \n", base_ext2->temp_slope_high);

    printk(" xatten1_db_low \n");
    for(i=0;i<JET_MAX_CHAINS; i++)
    {
	printk("[%d] ", base_ext2->xatten1_db_low[i]);
    }

    printk("\n xatten1_margin_low \n");
    for(i=0;i<JET_MAX_CHAINS; i++)
    {
	printk("[%d] ", base_ext2->xatten1_margin_low[i]);
    }

    printk("\n xatten1_db_high \n");
    for(i=0;i<JET_MAX_CHAINS; i++)
    {
	printk("[%d] ", base_ext2->xatten1_db_high[i]);
    }

    printk(" xatten1_margin_high \n");
    for(i=0;i<JET_MAX_CHAINS; i++)
    {
	printk("[%d] ", base_ext2->xatten1_margin_high[i]);
    }
    printk("\n");
}

/*
 * Function to dump struct jetEeprom
 */
void dump_eeprom_contents(ar9300_eeprom_t *jet_eeprom)
{
    int i, j;
    printk(" eeprom_version : %d \n", jet_eeprom->eeprom_version);
    printk(" template_version : %d \n", jet_eeprom->template_version);

    printk(" mac_addr \n");
    for( i= 0; i<6; i++) {
	printk("[%x] ", jet_eeprom->mac_addr[i]);
    }

    printk("\n");
    printk(" custdata \n");
    for( i= 0; i<OSPREY_CUSTOMER_DATA_SIZE; i++) {
	printk("[%d] ", jet_eeprom->custData[i]);
    }

    printk("\n base_eep_header \n");
    dump_base_eep_header(&jet_eeprom->base_eep_header);
    printk("\n modal_header_2g \n");
    dump_jet_modal_eep_header(&jet_eeprom->modal_header_2g);
    printk("\n base_ext1\n");
    dump_jet_base_extension_1(&jet_eeprom->base_ext1);

    printk("\n");
    printk(" cal_freq_pier_2g \n");
    for( i= 0; i<OSPREY_NUM_2G_CAL_PIERS; i++) {
	printk("[%d] ", jet_eeprom->cal_freq_pier_2g[i]);
    }

    printk("\n cal_pier_data_2g \n");
    dump_jet_cal_data_per_freq_op_loop(JET_MAX_CHAINS, OSPREY_NUM_2G_CAL_PIERS,
	    jet_eeprom->cal_pier_data_2g);

    printk("\n");
    printk(" cal_target_freqbin_cck \n");
    for( i= 0; i < OSPREY_NUM_2G_CCK_TARGET_POWERS; i++) {
	printk("[%d] ", jet_eeprom->cal_target_freqbin_cck[i]);
    }

    printk("\n");
    printk(" cal_target_freqbin_2g \n");
    for( i= 0; i < OSPREY_NUM_2G_20_TARGET_POWERS; i++) {
	printk("[%d] ", jet_eeprom->cal_target_freqbin_2g[i]);
    }

    printk("\n");
    printk(" cal_target_freqbin_2g_ht20 \n");
    for( i= 0; i < OSPREY_NUM_2G_20_TARGET_POWERS; i++) {
	printk("[%d] ", jet_eeprom->cal_target_freqbin_2g_ht20[i]);
    }

    printk("\n");
    printk(" cal_target_freqbin_2g_ht40 \n");
    for( i= 0; i < OSPREY_NUM_2G_40_TARGET_POWERS; i++) {
	printk("[%d] ", jet_eeprom->cal_target_freqbin_2g_ht40[i]);
    }

    printk("\n cal_target_power_cck \n");
    dump_cal_target_power_leg(jet_eeprom->cal_target_power_cck, OSPREY_NUM_2G_CCK_TARGET_POWERS);

    printk("\n cal_target_power_2g \n");
    dump_cal_target_power_leg(jet_eeprom->cal_target_power_2g, OSPREY_NUM_2G_20_TARGET_POWERS);

    printk("\n cal_target_power_2g_ht20 \n");
    dump_jet_cal_target_power_ht(jet_eeprom->cal_target_power_2g_ht20, OSPREY_NUM_2G_20_TARGET_POWERS);

    printk("\n cal_target_power_2g_ht40 \n");
    dump_jet_cal_target_power_ht(jet_eeprom->cal_target_power_2g_ht40, OSPREY_NUM_2G_40_TARGET_POWERS);

    printk("\n");
    printk(" ctl_index_2g \n");
    for( i= 0; i < OSPREY_NUM_CTLS_2G; i++) {
	printk("[%d] ", jet_eeprom->ctl_index_2g[i]);
    }

    printk("\n");
    printk(" ctl_freqbin_2G \n");
    for(i =0; i < OSPREY_NUM_CTLS_2G; i++)
    {
	for(j=0; j < OSPREY_NUM_BAND_EDGES_2G; j++)
	{
	    printk("[%d] ", jet_eeprom->ctl_freqbin_2G[i][j]);
	}
	printk("\n");
    }

    printk(" \n ctl_power_data_2g \n");
    dump_osp_cal_ctl_data_2g(jet_eeprom->ctl_power_data_2g, OSPREY_NUM_CTLS_2G);

    printk("\n modal_header_5g \n");
    dump_jet_modal_eep_header(&jet_eeprom->modal_header_5g);

    printk(" \n base_ext2 \n");
    dump_jet_base_extension_2(&jet_eeprom->base_ext2);

    printk("\n");
    printk("\n cal_freq_pier_5g \n");
    for( i= 0; i < OSPREY_NUM_5G_CAL_PIERS; i++) {
	printk("[%d] ", jet_eeprom->cal_freq_pier_5g[i]);
    }

    printk("\n cal_pier_data_5g \n");
    dump_jet_cal_data_per_freq_op_loop(JET_MAX_CHAINS, OSPREY_NUM_5G_CAL_PIERS,
	    jet_eeprom->cal_pier_data_5g);

    printk("\n");
    printk("\n cal_target_freqbin_5g \n");
    for( i= 0; i < OSPREY_NUM_5G_20_TARGET_POWERS; i++) {
	printk("[%d] ", jet_eeprom->cal_target_freqbin_5g[i]);
    }

    printk("\n");
    printk("\n cal_target_freqbin_5g_ht20 \n");
    for( i= 0; i < OSPREY_NUM_5G_20_TARGET_POWERS; i++) {
	printk("[%d] ", jet_eeprom->cal_target_freqbin_5g_ht20[i]);
    }

    printk("\n");
    printk("\n cal_target_freqbin_5g_ht40 \n");
    for( i= 0; i < OSPREY_NUM_5G_40_TARGET_POWERS; i++) {
	printk("[%d] ", jet_eeprom->cal_target_freqbin_5g_ht40[i]);
    }

    printk("\n cal_target_power_5g \n");
    dump_cal_target_power_leg(jet_eeprom->cal_target_power_5g, OSPREY_NUM_5G_20_TARGET_POWERS);

    printk("\n cal_target_power_5g_ht20 \n");
    dump_jet_cal_target_power_ht(jet_eeprom->cal_target_power_5g_ht20, OSPREY_NUM_5G_20_TARGET_POWERS);

    printk("\n cal_target_power_5g_ht40 \n");
    dump_jet_cal_target_power_ht(jet_eeprom->cal_target_power_5g_ht40, OSPREY_NUM_5G_40_TARGET_POWERS);

    printk("\n");
    printk("ctl_index_5g \n");
    for( i= 0; i < OSPREY_NUM_CTLS_5G; i++) {
	printk("[%d] ", jet_eeprom->ctl_index_5g[i]);
    }

    printk("\n");
    printk("\n ctl_freqbin_5G \n");
    for(i=0; i < OSPREY_NUM_CTLS_5G; i++)
    {
	for( j= 0; j < OSPREY_NUM_BAND_EDGES_5G; j++)
	{
	    printk("[%d] ", jet_eeprom->ctl_freqbin_5G[i][j]);
	}
	printk("\n");
    }

    printk("\n ctl_power_data_5g \n");
    dump_osp_cal_ctl_data_5g(jet_eeprom->ctl_power_data_5g, OSPREY_NUM_CTLS_5G);
}
#endif
#ifdef AH_CAL_IN_FILE_HOST
bool
ar9300_calibration_data_read_file(struct ath_hal *ah, u_int8_t *buffer, int many)
{
    int ret_val = -1;
    char * devname = NULL;
    char filename[64] = { 0 };

    ath_hal_printf(ah, "Restoring Cal data from FS\n");

    devname = (char *)(AH_PRIVATE(ah)->ah_st);

    if(0 == strncmp("wifi0", devname, 5))
    {
	qdf_str_lcopy(filename, CALDATA0_FILE_PATH, strlen(CALDATA0_FILE_PATH) + 1);
    }
    else if(0 == strncmp("wifi1", devname, 5))
    {
	qdf_str_lcopy(filename, CALDATA1_FILE_PATH, strlen(CALDATA1_FILE_PATH) + 1);
    }
    else if(0 == strncmp("wifi2", devname, 5))
    {
	qdf_str_lcopy(filename, CALDATA2_FILE_PATH, strlen(CALDATA2_FILE_PATH) + 1);
    }
    else
    {
        HDPRINTF(ah, HAL_DBG_UNMASKABLE, "%s[%d], Error: Please check why none of wifi0, wifi1 or wifi2 is your device name (%s).\n", __func__, __LINE__, devname);
        return -1;
    }

#if AH_DEBUG
    HDPRINTF(ah, HAL_DBG_EEPROM, "%s[%d] Get Caldata for %s.\n", __func__, __LINE__, devname);
#endif

    if(NULL == filename)
    {
        HDPRINTF(ah, HAL_DBG_UNMASKABLE, "%s[%d], Error: File name is null, please assign right caldata file name.\n", __func__, __LINE__);
        return -1;
    }

    ret_val = qdf_fs_read(filename, 0, (unsigned int)many, buffer);
    return ( (-1 == ret_val) ? AH_FALSE : AH_TRUE);
}
#endif

#ifdef QCA_PARTNER_PLATFORM
#undef qdf_str_lcopy
#endif
#ifndef ART_BUILD
bool
ar9300_calibration_data_read(struct ath_hal *ah, long address,
	void *buffer, int many)
{
    osprey_eeprom_t *osprey_eeprom = NULL;
    ar9300_eeprom_t *ar9300_jet_eeprom = NULL;
    bool retval = false;

    switch (AH9300(ah)->calibration_data_source) {
	case calibration_data_flash:
    {
            if(!AR_SREV_JET(ah)) {
	    ar9300_jet_eeprom = buffer;
	    osprey_eeprom = ath_hal_malloc(ah, sizeof(osprey_eeprom_t));
	    if(osprey_eeprom == AH_NULL) {
	        HDPRINTF(ah, HAL_DBG_MALLOC, "%s: cannot allocate osprey_eeprom\n", __func__);
	        return AH_FALSE;
	    }
	    buffer = osprey_eeprom;
	    many = sizeof(osprey_eeprom_t);
    }

	    retval = ar9300_calibration_data_read_flash(ah, address, buffer, many);
            if(!AR_SREV_JET(ah)) {
                copy_osprey_to_jet(ar9300_jet_eeprom, buffer);
                HDPRINTF(ah, HAL_DBG_EEPROM, "%s(%d): copy_osprey_to_jet \n", __func__, __LINE__);
                ath_hal_free(ah, osprey_eeprom);
            }
        }
	    break;
	case calibration_data_eeprom:
	    retval = ar9300_calibration_data_read_eeprom(ah, address, buffer, many);
	    break;
	case calibration_data_otp:
	    retval = ar9300_calibration_data_read_otp(ah, address, buffer, many, 1);
	    break;
#ifdef ATH_CAL_NAND_FLASH
	case calibration_data_nand:
        {
            if(!AR_SREV_JET(ah)) {
                ar9300_jet_eeprom = buffer;
                osprey_eeprom = ath_hal_malloc(ah, sizeof(osprey_eeprom_t));
                if(osprey_eeprom == AH_NULL) {
                    HDPRINTF(ah, HAL_DBG_MALLOC, "%s: cannot allocate osprey_eeprom\n", __func__);
                    return AH_FALSE;
                }
                buffer = osprey_eeprom;
                many = sizeof(osprey_eeprom_t);
            }
	    retval = ar9300_calibration_data_read_nand(ah,address,buffer,many);
            if(!AR_SREV_JET(ah)) {
                copy_osprey_to_jet(ar9300_jet_eeprom, buffer);
                HDPRINTF(ah, HAL_DBG_EEPROM, "%s(%d): copy_osprey_to_jet \n", __func__, __LINE__);
                ath_hal_free(ah, osprey_eeprom);
            }
        }
	    break;
#endif
#ifdef AH_CAL_IN_FILE_HOST
	case calibration_data_file:
        {
            if(!AR_SREV_JET(ah)) {
                ar9300_jet_eeprom = buffer;
                osprey_eeprom = ath_hal_malloc(ah, sizeof(osprey_eeprom_t));
                if(osprey_eeprom == AH_NULL) {
                    HDPRINTF(ah, HAL_DBG_MALLOC, "%s: cannot allocate osprey_eeprom\n", __func__);
                    return AH_FALSE;
    }
                buffer = osprey_eeprom;
                many = sizeof(osprey_eeprom_t);
            }
            retval = ar9300_calibration_data_read_file(ah, buffer, many);
            if(!AR_SREV_JET(ah)) {
	    copy_osprey_to_jet(ar9300_jet_eeprom, buffer);
	    HDPRINTF(ah, HAL_DBG_EEPROM, "%s(%d): copy_osprey_to_jet \n", __func__, __LINE__);
	    ath_hal_free(ah, osprey_eeprom);
            }
        }
        break;
#endif
    }
    return retval;
}
#else
bool
ar9300_calibration_data_read(struct ath_hal *ah, long address,
    void *buffer, int many)
{
    switch (AH9300(ah)->calibration_data_source) {
    case calibration_data_flash:
        return ar9300_calibration_data_read_flash(ah, address, buffer, many);
    case calibration_data_eeprom:
        return ar9300_calibration_data_read_eeprom(ah, address, buffer, many);
    case calibration_data_otp:
        return ar9300_calibration_data_read_otp(ah, address, buffer, many, 1);
#ifdef ATH_CAL_NAND_FLASH
    case calibration_data_nand:
        return ar9300_calibration_data_read_nand(ah,address,buffer,many);
#endif

    }
    return false;
}
#endif

bool
ar9300_calibration_data_read_array(struct ath_hal *ah, int address,
    u_int8_t *buffer, int many)
{
    int it;

    for (it = 0; it < many; it++) {
        (void)ar9300_calibration_data_read(ah, address - it, buffer + it, 1);
    }
    return true;
}


/*
 * the address where the first configuration block is written
 */
static const int base_address = 0x3ff;                /* 1KB */
static const int base_address_512 = 0x1ff;            /* 512Bytes */

/*
 * the address where the NAND first configuration block is written
 */
#ifdef ATH_CAL_NAND_FLASH
static const int base_address_nand = AR9300_FLASH_CAL_START_OFFSET;
#endif


/*
 * the lower limit on configuration data
 */
static const int low_limit = 0x040;

/*
 * returns size of the physical eeprom in bytes.
 * 1024 and 2048 are normal sizes.
 * 0 means there is no eeprom.
 */
int32_t
ar9300_eeprom_size(struct ath_hal *ah)
{
    u_int16_t data;
    /*
     * first we'll try for 4096 bytes eeprom
     */
    if (ar9300_eeprom_read_word(ah, 2047, &data)) {
        if (data != 0) {
            return 4096;
        }
    }
    /*
     * then we'll try for 2048 bytes eeprom
     */
    if (ar9300_eeprom_read_word(ah, 1023, &data)) {
        if (data != 0) {
            return 2048;
        }
    }
    /*
     * then we'll try for 1024 bytes eeprom
     */
    if (ar9300_eeprom_read_word(ah, 511, &data)) {
        if (data != 0) {
            return 1024;
        }
    }
    return 0;
}

/*
 * returns size of the physical otp in bytes.
 * 1024 and 2048 are normal sizes.
 * 0 means there is no eeprom.
 */
int32_t ar9300_otp_size(struct ath_hal *ah);
int32_t
ar9300_otp_size(struct ath_hal *ah)
{
    if (AR_SREV_POSEIDON(ah) || AR_SREV_HORNET(ah)) {
        return base_address_512+1;
    } else {
        return base_address+1;
    }
}


/*
 * find top of memory
 */
int
ar9300_eeprom_base_address(struct ath_hal *ah)
{
    int size;

    if (AH9300(ah)->calibration_data_source == calibration_data_otp) {
		return ar9300_otp_size(ah)-1;
	}
	else
	{
		size = ar9300_eeprom_size(ah);
		if (size > 0) {
			return size - 1;
		} else {
			return ar9300_otp_size(ah)-1;
		}
	}
}

int
ar9300_eeprom_volatile(struct ath_hal *ah)
{
    if (AH9300(ah)->calibration_data_source == calibration_data_otp) {
        return 0;        /* no eeprom, use otp */
    } else {
        return 1;        /* board has eeprom or flash */
    }
}

/*
 * need to change this to look for the pcie data in the low parts of memory
 * cal data needs to stop a few locations above
 */
int
ar9300_eeprom_low_limit(struct ath_hal *ah)
{
    return low_limit;
}

u_int16_t
ar9300_compression_checksum(u_int8_t *data, int dsize)
{
    int it;
    int checksum = 0;

    for (it = 0; it < dsize; it++) {
        checksum += data[it];
        checksum &= 0xffff;
    }

    return checksum;
}

int
ar9300_compression_header_unpack(u_int8_t *best, int *code, int *reference,
    int *length, int *major, int *minor)
{
    unsigned long value[4];

    value[0] = best[0];
    value[1] = best[1];
    value[2] = best[2];
    value[3] = best[3];
    *code = ((value[0] >> 5) & 0x0007);
    *reference = (value[0] & 0x001f) | ((value[1] >> 2) & 0x0020);
    *length = ((value[1] << 4) & 0x07f0) | ((value[2] >> 4) & 0x000f);
    *major = (value[2] & 0x000f);
    *minor = (value[3] & 0x00ff);

    return 4;
}


static bool
ar9300_uncompress_block(struct ath_hal *ah, u_int8_t *mptr, int mdata_size,
    u_int8_t *block, int size)
{
    int it;
    int spot;
    int offset;
    int length;

    spot = 0;
    for (it = 0; it < size; it += (length + 2)) {
        offset = block[it];
        offset &= 0xff;
        spot += offset;
        length = block[it + 1];
        length &= 0xff;
        if (length > 0 && spot >= 0 && spot + length <= mdata_size) {
            HDPRINTF(ah, HAL_DBG_EEPROM,
                "%s: Restore at %d: spot=%d offset=%d length=%d\n",
                __func__, it, spot, offset, length);
            OS_MEMCPY(&mptr[spot], &block[it + 2], length);
            spot += length;
        } else if (length > 0) {
            HDPRINTF(ah, HAL_DBG_EEPROM,
                "%s: Bad restore at %d: spot=%d offset=%d length=%d\n",
                __func__, it, spot, offset, length);
            return false;
        }
    }
    return true;
}
#ifndef ART_BUILD

static int
ar9300_eeprom_restore_internal_address(struct ath_hal *ah,
    ar9300_eeprom_t *mptr, int mdata_size, int cptr, u_int8_t blank)
{
    u_int8_t word[MOUTPUT];
    int code;
    int reference, length, major, minor;
    int osize;
    int it;
    int restored;
    u_int16_t checksum, mchecksum;

    osprey_eeprom_t *osprey_eeprom = NULL, *osprey_dptr = NULL;

    restored = 0;
    for (it = 0; it < MSTATE; it++) {
        (void) ar9300_calibration_data_read_array(
            ah, cptr, word, compression_header_length);
        if (word[0] == blank && word[1] == blank && word[2] == blank && word[3] == blank)
        {
            break;
        }
        ar9300_compression_header_unpack(
            word, &code, &reference, &length, &major, &minor);
        HDPRINTF(ah, HAL_DBG_EEPROM,
            "%s: Found block at %x: "
            "code=%d ref=%d length=%d major=%d minor=%d\n",
            __func__, cptr, code, reference, length, major, minor);
        osize = length;
        (void) ar9300_calibration_data_read_array(
            ah, cptr, word,
            compression_header_length + osize + compression_checksum_length);
        checksum = ar9300_compression_checksum(
            &word[compression_header_length], length);
        mchecksum =
            word[compression_header_length + osize] |
            (word[compression_header_length + osize + 1] << 8);
        HDPRINTF(ah, HAL_DBG_EEPROM,
            "%s: checksum %x %x\n", __func__, checksum, mchecksum);
        if (checksum == mchecksum) {
            switch (code) {
                case _compress_none:
                    if (length != mdata_size) {
                        HDPRINTF(ah, HAL_DBG_EEPROM,
                                "%s: EEPROM structure size mismatch "
                                "memory=%d eeprom=%d\n", __func__, mdata_size, length);
                        return -1;
                    }
                    OS_MEMCPY((u_int8_t *)mptr,
                            (u_int8_t *)(word + compression_header_length), length);
                    HDPRINTF(ah, HAL_DBG_EEPROM,
                            "%s: restored eeprom %d: uncompressed, length %d\n",
                            __func__, it, length);
                    restored = 1;
                    break;
                case _compress_block:

                    if (reference == reference_current) {
                        //dptr = mptr;
                    } else {
                        if(!AR_SREV_JET(ah)) {
                            osprey_eeprom = ath_hal_malloc(ah, sizeof(osprey_eeprom_t));
                            if(osprey_eeprom == AH_NULL) {
                                HDPRINTF(ah, HAL_DBG_MALLOC, "%s: cannot allocate osprey_eeprom\n", __func__);
                                return -1;
                            }
                            mdata_size = sizeof(osprey_eeprom_t);
                            osprey_dptr = osprey_eeprom_struct_default_find_by_id(reference);
                            if (osprey_dptr != 0) {
                                OS_MEMCPY(osprey_eeprom, osprey_dptr, mdata_size);
                                HDPRINTF(ah, HAL_DBG_EEPROM, "%s: template id : %d, osprey template loaded \n", __func__, reference);
                            } else {
                                HDPRINTF(ah, HAL_DBG_EEPROM,
                                        "%s: cant find reference eeprom struct %d\n",
                                        __func__, reference);
                                break;
                            }
                        } else {
                            if (jet_default_template != 0) {
                                OS_MEMCPY(mptr, jet_default_template, mdata_size);
                                HDPRINTF(ah, HAL_DBG_EEPROM, "%s: Jet default template loaded \n", __func__);
                            } else {
                                HDPRINTF(ah, HAL_DBG_EEPROM,
                                        "%s: cant find reference eeprom struct %d\n",
                                        __func__, reference);
                                break;
                            }
                        }
                    }
                    HDPRINTF(ah, HAL_DBG_EEPROM,
                            "%s: restore eeprom %d: block, reference %d, length %d\n",
                            __func__, it, reference, length);
                    if ((!AR_SREV_JET(ah)) && (osprey_eeprom != AH_NULL)) {
                        (void) ar9300_uncompress_block(ah,
                                (u_int8_t *) osprey_eeprom, mdata_size,
                                (u_int8_t *) (word + compression_header_length), length);
                        copy_osprey_to_jet(mptr, osprey_eeprom);
                        ath_hal_free(ah, osprey_eeprom);
                    } else {
                        (void) ar9300_uncompress_block(ah,
                                (u_int8_t *) mptr, mdata_size,
                                (u_int8_t *) (word + compression_header_length), length);
                    }
                    restored = 1;
                    break;
                default:
                    HDPRINTF(ah, HAL_DBG_EEPROM,
                            "%s: unknown compression code %d\n", __func__, code);
                    break;
            }
        } else {
            HDPRINTF(ah, HAL_DBG_EEPROM,
                "%s: skipping block with bad checksum\n", __func__);
        }
        cptr -= compression_header_length + osize + compression_checksum_length;
    }

    if (!restored) {
        cptr = -1;
    }
    return cptr;
}

static int
ar9300_eeprom_restore_from_dram(struct ath_hal *ah, ar9300_eeprom_t *mptr,
    int mdata_size)
{
    struct ath_hal_9300 *ahp = AH9300(ah);
#if !defined(USE_PLATFORM_FRAMEWORK)
    char *cal_ptr;
#endif

#ifndef WIN32

    HALASSERT(mdata_size > 0);

    /* if cal_in_flash is true, the address sent by LMAC to HAL
       (i.e. ah->ah_st) is corresponding to Flash. so return from
       here if ar9300_eep_data_in_flash(ah) returns true */
    if(ar9300_eep_data_in_flash(ah))
        return -1;

    /* check if LMAC sent DRAM address is valid */
    if (!(uintptr_t)(AH_PRIVATE(ah)->ah_st)) {
        return -1;
    }

    /* When calibration data is from host, Host will copy the
       compressed data to the predefined DRAM location saved at ah->ah_st */
    ath_hal_printf(ah, "Restoring Cal data from DRAM\n");
#ifdef __NetBSD__
    ahp->ah_cal_mem = OS_REMAP(ah, (uintptr_t)(AH_PRIVATE(ah)->ah_st),
							HOST_CALDATA_SIZE);
#else
    ahp->ah_cal_mem = OS_REMAP((uintptr_t)(AH_PRIVATE(ah)->ah_st),
							HOST_CALDATA_SIZE);
#endif
    if (!ahp->ah_cal_mem)
    {
       HDPRINTF(ah, HAL_DBG_EEPROM,"%s: can't remap dram region\n", __func__);
       return -1;
    }
#if !defined(USE_PLATFORM_FRAMEWORK)
    cal_ptr = &((char *)(ahp->ah_cal_mem))[AR9300_FLASH_CAL_START_OFFSET];
    OS_MEMCPY(mptr, cal_ptr, mdata_size);
#else
    OS_MEMCPY(mptr, ahp->ah_cal_mem, mdata_size);
#endif

    if (mptr->eeprom_version   == 0xff ||
        mptr->template_version == 0xff ||
        mptr->eeprom_version   == 0    ||
        mptr->template_version == 0)
    {
        /* The board is uncalibrated */
        return -1;
    }
    if (mptr->eeprom_version != 0x2)
    {
        return -1;
    }

    return mdata_size;
#else
    return -1;
#endif

}

static int
ar9300_eeprom_restore_from_flash(struct ath_hal *ah, ar9300_eeprom_t *mptr,
    int mdata_size)
{

#ifdef K31_CALDATA
    extern int ath_k31_mode;
#endif
#ifndef MDK_AP /* MDK_AP is defined only in NART AP build */
    struct ath_hal_9300 *ahp = AH9300(ah);
    char *cal_ptr;
#endif

    HALASSERT(mdata_size > 0);

#ifndef MDK_AP /* MDK_AP is defined only in NART AP build */
    if (!ahp->ah_cal_mem) {
        return -1;
    }

    ath_hal_printf(ah, "Restoring Cal data from Flash\n");
    /*
     * When calibration data is saved in flash, read
     * uncompressed eeprom structure from flash and return
     */
    cal_ptr = &((char *)(ahp->ah_cal_mem))[AR9300_FLASH_CAL_START_OFFSET];
    OS_MEMCPY(mptr, cal_ptr, mdata_size);
#if 0
    jet_swap_eeprom((ar9300_eeprom_t *)mptr); DONE IN ar9300_restore()
#endif
#else

#ifdef K31_CALDATA
    if (ath_k31_mode)
    {
        FILE *fp;
        int offset;

        if (instance == 0) {
            fp = fopen("/tmp/caldata2g", "rb");
        } else {
            fp = fopen("/tmp/caldata5g", "rb");
        }
        if (fp == NULL) {
            return -1;

        }
        fread(mptr, mdata_size, 1, fp);
        fclose(fp);
    } else
#endif
    {
        /*
         * When calibration data is saved in flash, read
         * uncompressed eeprom structure from flash and return
         */
        int fd;
        int offset;
        u_int8_t word[MOUTPUT];

        osprey_eeprom_t *osprey_eeprom = NULL;
        ar9300_eeprom_t *ar9300_jet_eeprom = NULL;
        bool retval = false;

        if(!AR_SREV_JET(ah))
        {
	        ar9300_jet_eeprom = mptr; // original buffer to retrieve caldata.
	        osprey_eeprom = ath_hal_malloc(ah, sizeof(osprey_eeprom_t));
	        if(osprey_eeprom == AH_NULL) {
	            HDPRINTF(ah, HAL_DBG_MALLOC, "%s: cannot allocate osprey_eeprom\n", __func__);
	            return -1;
	        }
	        mptr = (ar9300_eeprom_t *) osprey_eeprom;
	        mdata_size = sizeof(osprey_eeprom_t);
        }

        if ((fd = open("/dev/caldata", O_RDWR)) < 0) {
            perror("Flash caldata sector is not present: Writing caldata in file system /tmp/ \n");
            if ((fd = open("/tmp/wifi1.caldata.bin", O_RDWR)) < 0) {
                perror("Could not open flash\n");
                if (osprey_eeprom)
                    ath_hal_free(ah, osprey_eeprom);
                return -1;
	    }
	    offset = 0;
        } else {
            // First 0x1000 are reserved for ethernet mac address
            // and other config data.
            offset = instance * AR9300_EEPROM_SIZE + FLASH_BASE_CALDATA_OFFSET;
        }
        lseek(fd, offset, SEEK_SET);
        if (read(fd, mptr, mdata_size) != mdata_size) {
            perror("\n_read\n");
            close(fd);
            if (osprey_eeprom)
                ath_hal_free(ah, osprey_eeprom);
            return -1;
        }
        close(fd);
        if(!AR_SREV_JET(ah))
        {   /* Transfer each subfields from legacy EEP structure(Osprey) to new EEP structure (Jet). */
            copy_osprey_to_jet(ar9300_jet_eeprom, (struct ospreyEeprom *)mptr);
	        HDPRINTF(ah, HAL_DBG_EEPROM, "%s(%d): copy_osprey_to_jet \n", __func__, __LINE__);
	        ath_hal_free(ah, osprey_eeprom);
        }

    }

#endif
    if (mptr->eeprom_version   == 0xff ||
        mptr->template_version == 0xff ||
        mptr->eeprom_version   == 0    ||
        mptr->template_version == 0)
    {
        /* The board is uncalibrated */
        return -1;
    }
    if (mptr->eeprom_version != 0x2)
    {
        return -1;
    }
    return mdata_size;
}

bool is_valid_caldata(ar9300_eeprom_t *mptr,  int nptr )
{
    u_int32_t sum = 0;

    if (nptr > 0) {
        if(mptr->eeprom_version == 0 || mptr->eeprom_version == 0xff || mptr->template_version  == 0 || mptr->template_version == 0xff)
        {
            return false;
        }
        else
        {
            sum = mptr->mac_addr[0] + mptr->mac_addr[1] + mptr->mac_addr[2] + mptr->mac_addr[3] + mptr->mac_addr[4] + mptr->mac_addr[5];
            if (sum == 0 || sum == 6*0xff)
                return false;
        }
    }
    return true;
}

/*
 * Read the configuration data from the storage. We try the order with:
 * EEPROM, Flash, OTP, File System. If all of above failed, use the default template.
 * The data can be put in any specified memory buffer.
 *
 * Returns -1 on error.
 * Returns address of next memory location on success.
 */
int
ar9300_eeprom_restore_internal(struct ath_hal *ah, ar9300_eeprom_t *mptr,
    int mdata_size)
{
    int nptr;

    nptr = -1;

#ifdef AH_CAL_IN_FILE_HOST
    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_file) &&
         AH9300(ah)->try_file && nptr < 0)
    {
        AH9300(ah)->calibration_data_source = calibration_data_file;
        AH9300(ah)->calibration_data_source_address = 0;

        if(AH_TRUE == ar9300_calibration_data_read(
            ah, AH9300(ah)->calibration_data_source_address,
            (u_int8_t *)mptr, mdata_size) )
        {
            nptr = mdata_size;
        }

        if(nptr < 0 || !is_valid_caldata( mptr, nptr ) )
        {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
            nptr = -1;
        }
    }
#endif
    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_dram) &&
         AH9300(ah)->try_dram && nptr < 0)
    {
        AH9300(ah)->calibration_data_source = calibration_data_dram;
        AH9300(ah)->calibration_data_source_address = 0;
        nptr = ar9300_eeprom_restore_from_dram(ah, mptr, mdata_size);
        if(nptr < 0 || !is_valid_caldata( mptr, nptr ) )
        {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
            nptr = -1;
        }
    }

    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_eeprom) &&
        AH9300(ah)->try_eeprom && nptr < 0)
    {
        /*
         * need to look at highest eeprom address as well as at
         * base_address=0x3ff where we used to write the data
         */
        AH9300(ah)->calibration_data_source = calibration_data_eeprom;
        if (AH9300(ah)->calibration_data_try_address != 0) {
            AH9300(ah)->calibration_data_source_address =
                AH9300(ah)->calibration_data_try_address;
            nptr = ar9300_eeprom_restore_internal_address(
                ah, mptr, mdata_size,
                AH9300(ah)->calibration_data_source_address, 0xff);
        } else {
            AH9300(ah)->calibration_data_source_address =
                ar9300_eeprom_base_address(ah);
            nptr = ar9300_eeprom_restore_internal_address(
                ah, mptr, mdata_size,
                AH9300(ah)->calibration_data_source_address, 0xff);
            if (nptr < 0 &&
                AH9300(ah)->calibration_data_source_address != base_address)
            {
                AH9300(ah)->calibration_data_source_address = base_address;
                nptr = ar9300_eeprom_restore_internal_address(
                    ah, mptr, mdata_size,
                    AH9300(ah)->calibration_data_source_address, 0xff);
            }
        }
        if(nptr < 0 || !is_valid_caldata( mptr, nptr ) )
        {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
            nptr = -1;
        }
    }

    /*
     * ##### should be an ifdef test for any AP usage,
     * either in driver or in nart
     */
    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_flash) &&
        AH9300(ah)->try_flash && nptr < 0)
    {
        AH9300(ah)->calibration_data_source = calibration_data_flash;
        /* how are we supposed to set this for flash? */
        AH9300(ah)->calibration_data_source_address = 0;
        nptr = ar9300_eeprom_restore_from_flash(ah, mptr, mdata_size);
        if(nptr < 0 || !is_valid_caldata( mptr, nptr ) ) {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
            nptr = -1;
        }
    }

    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_otp) &&
        AH9300(ah)->try_otp && nptr < 0)
    {
        AH9300(ah)->calibration_data_source = calibration_data_otp;
        if (AH9300(ah)->calibration_data_try_address != 0) {
            AH9300(ah)->calibration_data_source_address =
                AH9300(ah)->calibration_data_try_address;
		} else {
            AH9300(ah)->calibration_data_source_address =
                ar9300_eeprom_base_address(ah);
		}
        nptr = ar9300_eeprom_restore_internal_address(
                ah, mptr, mdata_size, AH9300(ah)->calibration_data_source_address, 0);
        if(nptr < 0 || !is_valid_caldata( mptr, nptr ) )
        {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
            nptr = -1;
        }
    }

#ifdef ATH_CAL_NAND_FLASH
    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_nand) &&
        AH9300(ah)->try_nand && nptr < 0)
    {
        AH9300(ah)->calibration_data_source = calibration_data_nand;
        AH9300(ah)->calibration_data_source_address = ((unsigned int)(AH_PRIVATE(ah)->ah_st)) + base_address_nand;
        if(ar9300_calibration_data_read(
            ah, AH9300(ah)->calibration_data_source_address,
            (u_int8_t *)mptr, mdata_size) == AH_TRUE)
        {
            nptr = mdata_size;
        }
        /*nptr=ar9300EepromRestoreInternalAddress(ah, mptr, mdataSize, CalibrationDataSourceAddress);*/
        if(nptr < 0 || !is_valid_caldata( mptr, nptr ) )
        {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
            nptr = -1;
        }
    }
#endif
    if(nptr < 0 || !is_valid_caldata( mptr, nptr ) ) {
        ath_hal_printf(ah, "%s[%d] No vaid CAL, calling default template\n",
                __func__, __LINE__);
        nptr = ar9300_eeprom_restore_something(ah, mptr, mdata_size);
    }

    return nptr;
}
#else // ART_BUILD, two EEP struct co-exist, use void type for mptr argument in below API, and cast it according to Jet or not.
static int
ar9300_eeprom_restore_internal_address(struct ath_hal *ah,
    ar9300_eeprom_t *mptr, int mdata_size, int cptr, u_int8_t blank)
{
    u_int8_t word[MOUTPUT];
    ar9300_eeprom_t *dptr; /* was uint8 */
    int code;
    int reference, length, major, minor;
    int osize;
    int it;
    int restored;
    u_int16_t checksum, mchecksum;

    restored = 0;
    for (it = 0; it < MSTATE; it++) {
        (void) ar9300_calibration_data_read_array(
            ah, cptr, word, compression_header_length);
        if (word[0] == blank && word[1] == blank && word[2] == blank && word[3] == blank)
        {
            break;
        }
        ar9300_compression_header_unpack(
            word, &code, &reference, &length, &major, &minor);
        HDPRINTF(ah, HAL_DBG_EEPROM,
            "%s: Found block at %x: "
            "code=%d ref=%d length=%d major=%d minor=%d\n",
            __func__, cptr, code, reference, length, major, minor);
#ifdef DONTUSE
        if (length >= 1024) {
            HDPRINTF(ah, HAL_DBG_EEPROM, "%s: Skipping bad header\n", __func__);
            cptr -= compression_header_length;
            continue;
        }
#endif
        osize = length;
        (void) ar9300_calibration_data_read_array(
            ah, cptr, word,
            compression_header_length + osize + compression_checksum_length);
        checksum = ar9300_compression_checksum(
            &word[compression_header_length], length);
        mchecksum =
            word[compression_header_length + osize] |
            (word[compression_header_length + osize + 1] << 8);
        HDPRINTF(ah, HAL_DBG_EEPROM,
            "%s: checksum %x %x\n", __func__, checksum, mchecksum);
        if (checksum == mchecksum) {
            switch (code) {
            case _compress_none:
                if (length != mdata_size) {
                    HDPRINTF(ah, HAL_DBG_EEPROM,
                        "%s: EEPROM structure size mismatch "
                        "memory=%d eeprom=%d\n", __func__, mdata_size, length);
                    return -1;
                }
                OS_MEMCPY((u_int8_t *)mptr,
                    (u_int8_t *)(word + compression_header_length), length);
                HDPRINTF(ah, HAL_DBG_EEPROM,
                    "%s: restored eeprom %d: uncompressed, length %d\n",
                    __func__, it, length);
                restored = 1;
                break;
#ifdef UNUSED
            case _compress_lzma:
                if (reference == reference_current) {
                    dptr = mptr;
                } else {
                    dptr = (u_int8_t *)ar9300_eeprom_struct_default_find_by_id(
                        reference);
                    if (dptr == 0) {
                        HDPRINTF(ah, HAL_DBG_EEPROM,
                            "%s: Can't find reference eeprom struct %d\n",
                            __func__, reference);
                        goto done;
                    }
                }
                usize = -1;
                if (usize != mdata_size) {
                    HDPRINTF(ah, HAL_DBG_EEPROM,
                        "%s: uncompressed data is wrong size %d %d\n",
                        __func__, usize, mdata_size);
                    goto done;
                }

                for (ib = 0; ib < mdata_size; ib++) {
                    mptr[ib] = dptr[ib] ^ word[ib + overhead];
                }
                HDPRINTF(ah, HAL_DBG_EEPROM,
                    "%s: restored eeprom %d: compressed, "
                    "reference %d, length %d\n",
                    __func__, it, reference, length);
                break;
            case _compress_pairs:
                if (reference == reference_current) {
                    dptr = mptr;
                } else {
                    dptr = (u_int8_t *)ar9300_eeprom_struct_default_find_by_id(
                        reference);
                    if (dptr == 0) {
                        HDPRINTF(ah, HAL_DBG_EEPROM,
                            "%s: Can't find the reference "
                            "eeprom structure %d\n",
                            __func__, reference);
                        goto done;
                    }
                }
                HDPRINTF(ah, HAL_DBG_EEPROM,
                    "%s: restored eeprom %d: "
                    "pairs, reference %d, length %d,\n",
                    __func__, it, reference, length);
                break;
#endif
            case _compress_block:
                if (reference == reference_current) {
                    dptr = mptr;
                } else {
                    dptr = ar9300_eeprom_struct_default_find_by_id(reference);
                    if (dptr == 0) {
                        HDPRINTF(ah, HAL_DBG_EEPROM,
                            "%s: cant find reference eeprom struct %d\n",
                            __func__, reference);
                        break;
                    }
                    OS_MEMCPY(mptr, dptr, mdata_size);
                }

                HDPRINTF(ah, HAL_DBG_EEPROM,
                    "%s: restore eeprom %d: block, reference %d, length %d\n",
                    __func__, it, reference, length);
                (void) ar9300_uncompress_block(ah,
                    (u_int8_t *) mptr, mdata_size,
                    (u_int8_t *) (word + compression_header_length), length);
                restored = 1;
                break;
            default:
                HDPRINTF(ah, HAL_DBG_EEPROM,
                    "%s: unknown compression code %d\n", __func__, code);
                break;
            }
        } else {
            HDPRINTF(ah, HAL_DBG_EEPROM,
                "%s: skipping block with bad checksum\n", __func__);
        }
        cptr -= compression_header_length + osize + compression_checksum_length;
    }

    if (!restored) {
        cptr = -1;
    }
    return cptr;
}

static int
ar9300_eeprom_restore_from_dram(struct ath_hal *ah, ar9300_eeprom_t *mptr,
    int mdata_size)
{
    struct ath_hal_9300 *ahp = AH9300(ah);

#if !defined(USE_PLATFORM_FRAMEWORK)
    char *cal_ptr;
#endif

#ifndef WIN32

    HALASSERT(mdata_size > 0);

    /* if cal_in_flash is true, the address sent by LMAC to HAL
       (i.e. ah->ah_st) is corresponding to Flash. so return from
       here if ar9300_eep_data_in_flash(ah) returns true */
    if(ar9300_eep_data_in_flash(ah))
        return -1;

    /* check if LMAC sent DRAM address is valid */
    if (!(uintptr_t)(AH_PRIVATE(ah)->ah_st)) {
        return -1;
    }

    /* When calibration data is from host, Host will copy the
       compressed data to the predefined DRAM location saved at ah->ah_st */
    ath_hal_printf(ah, "Restoring Cal data from DRAM\n");
#ifdef __NetBSD__
    ahp->ah_cal_mem = OS_REMAP(ah, (uintptr_t)(AH_PRIVATE(ah)->ah_st),
							HOST_CALDATA_SIZE);
#else
    ahp->ah_cal_mem = (void *)OS_REMAP((uintptr_t)(AH_PRIVATE(ah)->ah_st),
							HOST_CALDATA_SIZE);
#endif
    if (!ahp->ah_cal_mem)
    {
       HDPRINTF(ah, HAL_DBG_EEPROM,"%s: can't remap dram region\n", __func__);
       return -1;
    }
#if !defined(USE_PLATFORM_FRAMEWORK)
    cal_ptr = &((char *)(ahp->ah_cal_mem))[AR9300_FLASH_CAL_START_OFFSET];
    OS_MEMCPY(mptr, cal_ptr, mdata_size);
#else
    OS_MEMCPY(mptr, ahp->ah_cal_mem, mdata_size);
#endif

    if (mptr->eeprom_version   == 0xff ||
        mptr->template_version == 0xff ||
        mptr->eeprom_version   == 0    ||
        mptr->template_version == 0)
    {
        /* The board is uncalibrated */
        return -1;
    }
    if (mptr->eeprom_version != 0x2)
    {
        return -1;
    }

    return mdata_size;
#else
    return -1;
#endif

}

static int
ar9300_eeprom_restore_from_flash(struct ath_hal *ah, ar9300_eeprom_t *mptr,
    int mdata_size)
{
#ifdef K31_CALDATA
    extern int ath_k31_mode;
#endif
#ifndef MDK_AP /* MDK_AP is defined only in NART AP build */
    struct ath_hal_9300 *ahp = AH9300(ah);
    char *cal_ptr;
#endif

    HALASSERT(mdata_size > 0);

#ifndef MDK_AP /* MDK_AP is defined only in NART AP build */
    if (!ahp->ah_cal_mem) {
        return -1;
    }

    ath_hal_printf(ah, "Restoring Cal data from Flash\n");
    /*
     * When calibration data is saved in flash, read
     * uncompressed eeprom structure from flash and return
     */
    cal_ptr = &((char *)(ahp->ah_cal_mem))[AR9300_FLASH_CAL_START_OFFSET];
    OS_MEMCPY(mptr, cal_ptr, mdata_size);
#if 0
    ar9300_swap_eeprom((ar9300_eeprom_t *)mptr); DONE IN ar9300_restore()
#endif
#else

#ifdef K31_CALDATA
    if (ath_k31_mode)
    {
        FILE *fp;
        int offset;

        if (instance == 0) {
            fp = fopen("/tmp/caldata2g", "rb");
        } else {
            fp = fopen("/tmp/caldata5g", "rb");
        }
        if (fp == NULL) {
            return -1;

        }
        fread(mptr, mdata_size, 1, fp);
        #if 0
        printf(" ===%s[%d] 0x%08x 0x%08x\n",
            __func__, __LINE__,
            mptr->eeprom_version, mptr->template_version);
        #endif
        fclose(fp);
    } else
#endif
    {
        /*
         * When calibration data is saved in flash, read
         * uncompressed eeprom structure from flash and return
         */
        int fd;
        int offset;
        u_int8_t word[MOUTPUT];
        if ((fd = open("/dev/caldata", O_RDWR)) < 0) {
                perror("Flash caldata sector is not present: Writing caldata in file system /tmp/ \n");
                if ((fd = open("/tmp/wifi1.caldata.bin", O_RDWR)) < 0) {
                        perror("Could not open file in flash\n");
                        return -1;
                }
                offset = 0;
        } else {
                // First 0x1000 are reserved for ethernet mac address
                // and other config data.
                offset = instance * AR9300_EEPROM_SIZE + FLASH_BASE_CALDATA_OFFSET;
        }
        lseek(fd, offset, SEEK_SET);
        if (read(fd, mptr, mdata_size) != mdata_size) {
            perror("\n_read\n");
            close(fd);
            return -1;
        }
        close(fd);
    }

#endif

    if (mptr->eeprom_version   == 0xff ||
        mptr->template_version == 0xff ||
        mptr->eeprom_version   == 0    ||
        mptr->template_version == 0)
    {
        /* The board is uncalibrated */
        return -1;
    }
    if (mptr->eeprom_version != 0x2)
    {
        return -1;
    }
    return mdata_size;
}

/*
 * Read the configuration data from the storage. We try the order with:
 * EEPROM, Flash, OTP. If all of above failed, use the default template.
 * The data can be put in any specified memory buffer.
 *
 * Returns -1 on error.
 * Returns address of next memory location on success.
 */
int
ar9300_eeprom_restore_internal(struct ath_hal *ah, ar9300_eeprom_t *mptr,
    int mdata_size)
{
    int nptr;

    nptr = -1;

    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_dram) &&
         AH9300(ah)->try_dram && nptr < 0)
    {
        AH9300(ah)->calibration_data_source = calibration_data_dram;
        AH9300(ah)->calibration_data_source_address = 0;
        nptr = ar9300_eeprom_restore_from_dram(ah, mptr, mdata_size);
        if (nptr < 0) {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
        }
    }

    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_eeprom) &&
        AH9300(ah)->try_eeprom && nptr < 0)
    {
        /*
         * need to look at highest eeprom address as well as at
         * base_address=0x3ff where we used to write the data
         */
        AH9300(ah)->calibration_data_source = calibration_data_eeprom;
        if (AH9300(ah)->calibration_data_try_address != 0) {
            AH9300(ah)->calibration_data_source_address =
                AH9300(ah)->calibration_data_try_address;
            nptr = ar9300_eeprom_restore_internal_address(
                ah, mptr, mdata_size,
                AH9300(ah)->calibration_data_source_address, 0xff);
        } else {
            AH9300(ah)->calibration_data_source_address =
                ar9300_eeprom_base_address(ah);
            nptr = ar9300_eeprom_restore_internal_address(
                ah, mptr, mdata_size,
                AH9300(ah)->calibration_data_source_address, 0xff);
            if (nptr < 0 &&
                AH9300(ah)->calibration_data_source_address != base_address)
            {
                AH9300(ah)->calibration_data_source_address = base_address;
                nptr = ar9300_eeprom_restore_internal_address(
                    ah, mptr, mdata_size,
                    AH9300(ah)->calibration_data_source_address, 0xff);
            }
        }
        if (nptr < 0) {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
        }
    }

    /*
     * ##### should be an ifdef test for any AP usage,
     * either in driver or in nart
     */

    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_flash) &&
        AH9300(ah)->try_flash && nptr < 0)
    {
        AH9300(ah)->calibration_data_source = calibration_data_flash;
        /* how are we supposed to set this for flash? */
        AH9300(ah)->calibration_data_source_address = 0;
        nptr = ar9300_eeprom_restore_from_flash(ah, mptr, mdata_size);
        if (nptr < 0) {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
        }
    }

    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_otp) &&
        AH9300(ah)->try_otp && nptr < 0)
    {
        AH9300(ah)->calibration_data_source = calibration_data_otp;
        if (AH9300(ah)->calibration_data_try_address != 0) {
            AH9300(ah)->calibration_data_source_address =
                AH9300(ah)->calibration_data_try_address;
		} else {
            AH9300(ah)->calibration_data_source_address =
                ar9300_eeprom_base_address(ah);
		}
        nptr = ar9300_eeprom_restore_internal_address(
            ah, mptr, mdata_size, AH9300(ah)->calibration_data_source_address, 0);
        if (nptr < 0) {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
        }
    }

#ifdef ATH_CAL_NAND_FLASH
    if ((AH9300(ah)->calibration_data_try == calibration_data_none ||
         AH9300(ah)->calibration_data_try == calibration_data_nand) &&
        AH9300(ah)->try_nand && nptr < 0)
    {
        AH9300(ah)->calibration_data_source = calibration_data_nand;
        AH9300(ah)->calibration_data_source_address = ((unsigned int)(AH_PRIVATE(ah)->ah_st)) + base_address_nand;
        if(ar9300_calibration_data_read(
            ah, AH9300(ah)->calibration_data_source_address,
            (u_int8_t *)mptr, mdata_size) == AH_TRUE)
        {
            nptr = mdata_size;
        }
        /*nptr=ar9300EepromRestoreInternalAddress(ah, mptr, mdataSize, CalibrationDataSourceAddress);*/
        if(nptr < 0)
        {
            AH9300(ah)->calibration_data_source = calibration_data_none;
            AH9300(ah)->calibration_data_source_address = 0;
        }
    }
#endif
    if (nptr < 0) {
        ath_hal_printf(ah, "%s[%d] No vaid CAL, calling default template\n",
            __func__, __LINE__);
        nptr = ar9300_eeprom_restore_something(ah, mptr, mdata_size);
    }

    return nptr;
}

#endif /* End ART_BUILD */
/******************************************************************************/
/*!
**  \brief Eeprom Swapping Function
**
**  This function will swap the contents of the "longer" EEPROM data items
**  to ensure they are consistent with the endian requirements for the platform
**  they are being compiled for
**
**  \param eh    Pointer to the EEPROM data structure
**  \return N/A
*/
#if AH_BYTE_ORDER == AH_BIG_ENDIAN

/*
 * Function to swap struct jetEeprom members
 */
#ifndef ART_BUILD
void jet_swap_eeprom(ar9300_eeprom_t *eep)
#else
void ar5500_swap_eeprom(ar5500_eeprom_t *eep)
#endif
{
    u_int32_t dword;
    u_int16_t word;
    int i;

    word = __bswap16(eep->base_eep_header.reg_dmn[0]);
    eep->base_eep_header.reg_dmn[0] = word;

    word = __bswap16(eep->base_eep_header.reg_dmn[1]);
    eep->base_eep_header.reg_dmn[1] = word;

    dword = __bswap32(eep->base_eep_header.swreg);
    eep->base_eep_header.swreg = dword;

    dword = __bswap32(eep->modal_header_2g.ant_ctrl_common);
    eep->modal_header_2g.ant_ctrl_common = dword;

    dword = __bswap32(eep->modal_header_2g.ant_ctrl_common2);
    eep->modal_header_2g.ant_ctrl_common2 = dword;

    dword = __bswap32(eep->modal_header_2g.paprd_rate_mask_ht20);
    eep->modal_header_2g.paprd_rate_mask_ht20 = dword;

    dword = __bswap32(eep->modal_header_2g.paprd_rate_mask_ht40);
    eep->modal_header_2g.paprd_rate_mask_ht40 = dword;

    dword = __bswap32(eep->modal_header_2g.paprd_rate_mask_4ss_ht20);
    eep->modal_header_2g.paprd_rate_mask_4ss_ht20 = dword;

    dword = __bswap32(eep->modal_header_2g.paprd_rate_mask_4ss_ht40);
    eep->modal_header_2g.paprd_rate_mask_4ss_ht40 = dword;

    dword = __bswap32(eep->modal_header_5g.ant_ctrl_common);
    eep->modal_header_5g.ant_ctrl_common = dword;

    dword = __bswap32(eep->modal_header_5g.ant_ctrl_common2);
    eep->modal_header_5g.ant_ctrl_common2 = dword;

    dword = __bswap32(eep->modal_header_5g.paprd_rate_mask_ht20);
    eep->modal_header_5g.paprd_rate_mask_ht20 = dword;

    dword = __bswap32(eep->modal_header_5g.paprd_rate_mask_ht40);
    eep->modal_header_5g.paprd_rate_mask_ht40 = dword;

    dword = __bswap32(eep->modal_header_5g.paprd_rate_mask_4ss_ht20);
    eep->modal_header_5g.paprd_rate_mask_4ss_ht20 = dword;

    dword = __bswap32(eep->modal_header_5g.paprd_rate_mask_4ss_ht40);
    eep->modal_header_5g.paprd_rate_mask_4ss_ht40 = dword;

    for (i = 0; i < JET_MAX_CHAINS; i++) {
	    word = __bswap16(eep->modal_header_2g.ant_ctrl_chain[i]);
        eep->modal_header_2g.ant_ctrl_chain[i] = word;

        word = __bswap16(eep->modal_header_5g.ant_ctrl_chain[i]);
        eep->modal_header_5g.ant_ctrl_chain[i] = word;
    }
    word = __bswap16(eep->chipCalData.thermAdcScaledGain);
    eep->chipCalData.thermAdcScaledGain = word;
    word = __bswap16(eep->chipCalData.thermAdcScaledGain1);
    eep->chipCalData.thermAdcScaledGain1 = word;
    word = __bswap16(eep->chipCalData.thermAdcScaledGain2);
    eep->chipCalData.thermAdcScaledGain2 = word;
    word = __bswap16(eep->chipCalData.thermAdcScaledGain3);
    eep->chipCalData.thermAdcScaledGain3 = word;
    dword = __bswap32(eep->chipCalData.ateCALTemp);
    eep->chipCalData.ateCALTemp = dword;
    dword = __bswap32(eep->chipCalData.ateCALTemp1);
    eep->chipCalData.ateCALTemp1 = dword;
}

/*
 * Function to swap struct ospreyEeprom members
 */
#ifndef ART_BUILD
void osprey_swap_eeprom(osprey_eeprom_t *eep)
#else
void ar9300_swap_eeprom(ar9300_eeprom_t *eep)
#endif
{
    u_int32_t dword;
    u_int16_t word;
    int i;

    word = __bswap16(eep->base_eep_header.reg_dmn[0]);
    eep->base_eep_header.reg_dmn[0] = word;

    word = __bswap16(eep->base_eep_header.reg_dmn[1]);
    eep->base_eep_header.reg_dmn[1] = word;

    dword = __bswap32(eep->base_eep_header.swreg);
    eep->base_eep_header.swreg = dword;

    dword = __bswap32(eep->modal_header_2g.ant_ctrl_common);
    eep->modal_header_2g.ant_ctrl_common = dword;

    dword = __bswap32(eep->modal_header_2g.ant_ctrl_common2);
    eep->modal_header_2g.ant_ctrl_common2 = dword;

    dword = __bswap32(eep->modal_header_2g.paprd_rate_mask_ht20);
    eep->modal_header_2g.paprd_rate_mask_ht20 = dword;

    dword = __bswap32(eep->modal_header_2g.paprd_rate_mask_ht40);
    eep->modal_header_2g.paprd_rate_mask_ht40 = dword;

    dword = __bswap32(eep->modal_header_5g.ant_ctrl_common);
    eep->modal_header_5g.ant_ctrl_common = dword;

    dword = __bswap32(eep->modal_header_5g.ant_ctrl_common2);
    eep->modal_header_5g.ant_ctrl_common2 = dword;

    dword = __bswap32(eep->modal_header_5g.paprd_rate_mask_ht20);
    eep->modal_header_5g.paprd_rate_mask_ht20 = dword;

    dword = __bswap32(eep->modal_header_5g.paprd_rate_mask_ht40);
    eep->modal_header_5g.paprd_rate_mask_ht40 = dword;

    for (i = 0; i < OSPREY_MAX_CHAINS; i++) {
	word = __bswap16(eep->modal_header_2g.ant_ctrl_chain[i]);
        eep->modal_header_2g.ant_ctrl_chain[i] = word;

        word = __bswap16(eep->modal_header_5g.ant_ctrl_chain[i]);
        eep->modal_header_5g.ant_ctrl_chain[i] = word;
    }
}
#ifndef ART_BUILD
void ar9300_eeprom_template_swap(struct ath_hal *ah)
{
    int it;
    osprey_eeprom_t *dptr;

    if(AR_SREV_JET(ah)) {
	    if(jet_default_template != 0)
		 jet_swap_eeprom(jet_default_template);
    } else {
	    for (it = 0; it < osprey_eeprom_struct_default_many(); it++) {
	        dptr = osprey_eeprom_struct_default(it);
	        if (dptr != 0) {
	            osprey_swap_eeprom(dptr);
	        }
	    }
    }
}
#else
void ar9300_eeprom_template_swap(void)
{
    int it;
    ar9300_eeprom_t *dptr;

    for (it = 0; it < ARRAY_LENGTH(default9300); it++) {
        dptr = ar9300_eeprom_struct_default(it);
        if (dptr != 0) {
            ar9300_swap_eeprom(dptr);
        }
    }
}
#endif

#endif

#if AH_BYTE_ORDER == AH_LITTLE_ENDIAN
void check_and_swap_ctl_data( ar9300_eeprom_t *mptr )
{
	int i=0,j=0,need_ctl_swap=0;
	u_int8_t ctl_data;

	if( mptr->base_eep_header.op_cap_flags.op_flags & 0x2a ){
		for ( i=0; i<OSPREY_NUM_CTLS_2G; i++){
			for( j=0; j<OSPREY_NUM_BAND_EDGES_2G; j++){
				if( mptr->ctl_power_data_2g[i].ctl_edges[j].flag > 1 ||
					mptr->ctl_power_data_2g[i].ctl_edges[j].t_power < 0 ||
					mptr->ctl_power_data_2g[i].ctl_edges[j].t_power > 63 )
				{
					need_ctl_swap=1;
					break;
				}
			}
			if( need_ctl_swap )
				break;
		}
		if( need_ctl_swap ){
			for ( i=0; i<OSPREY_NUM_CTLS_2G; i++){
				for( j=0; j<OSPREY_NUM_BAND_EDGES_2G; j++){
					ctl_data=*((u_int8_t*)&mptr->ctl_power_data_2g[i].ctl_edges[j]);
					mptr->ctl_power_data_2g[i].ctl_edges[j].flag=ctl_data&0x3;
					mptr->ctl_power_data_2g[i].ctl_edges[j].t_power=(ctl_data&0xfc)>>2;
				}
			}
		}
	}
    need_ctl_swap=0;
	if( mptr->base_eep_header.op_cap_flags.op_flags & 0x15 ){
		for ( i=0; i<OSPREY_NUM_CTLS_5G; i++){
			for( j=0; j<OSPREY_NUM_BAND_EDGES_5G; j++)
				if( mptr->ctl_power_data_5g[i].ctl_edges[j].flag > 1 ||
					mptr->ctl_power_data_5g[i].ctl_edges[j].t_power < 0 ||
					mptr->ctl_power_data_5g[i].ctl_edges[j].t_power > 63 )
				{
					need_ctl_swap=1;
					break;
				}
			if( need_ctl_swap )
				break;
		}
		if( need_ctl_swap ){
			for ( i=0; i<OSPREY_NUM_CTLS_5G; i++){
				for( j=0; j<OSPREY_NUM_BAND_EDGES_5G; j++){
					ctl_data=*((u_int8_t*)&mptr->ctl_power_data_5g[i].ctl_edges[j]);
					mptr->ctl_power_data_5g[i].ctl_edges[j].flag=ctl_data&0x3;
					mptr->ctl_power_data_5g[i].ctl_edges[j].t_power=(ctl_data&0xfc)>>2;
				}
			}
		}
	}
}
#endif


/*
 * Restore the configuration structure by reading the eeprom.
 * This function destroys any existing in-memory structure content.
 */
bool
ar9300_eeprom_restore(struct ath_hal *ah)
{
    struct ath_hal_9300 *ahp = AH9300(ah);
#ifndef ART_BUILD
    ar9300_eeprom_t *mptr;
#else
    ar9300_eeprom_t *mptr;
    ar5500_eeprom_t *mptr_jet;
#endif
    int mdata_size;
    bool status = false;


#ifndef ART_BUILD
    mptr = &ahp->ah_eeprom;
    mdata_size = ar9300_eeprom_struct_size();

    if (mptr != 0 && mdata_size > 0) {
#if AH_BYTE_ORDER == AH_BIG_ENDIAN
	ar9300_eeprom_template_swap(ah);
	jet_swap_eeprom(mptr);
#endif
        /*
         * At this point, mptr points to the eeprom data structure
         * in it's "default" state.  If this is big endian, swap the
         * data structures back to "little endian" form.
         */
        if (ar9300_eeprom_restore_internal(ah, mptr, mdata_size) >= 0) {
            status = true;
        }

#if AH_BYTE_ORDER == AH_BIG_ENDIAN
        /* Second Swap, back to Big Endian */
        ar9300_eeprom_template_swap(ah);
	jet_swap_eeprom(mptr);
#endif

    }
    ahp->ah_2g_paprd_rate_mask_ht40 =
        mptr->modal_header_2g.paprd_rate_mask_ht40;
    ahp->ah_2g_paprd_rate_mask_ht20 =
        mptr->modal_header_2g.paprd_rate_mask_ht20;
    if(AR_SREV_JET(ah)) {
        ahp->ah_2g_paprd_rate_mask_4ss_ht40 =
            mptr->modal_header_2g.paprd_rate_mask_4ss_ht40;
        ahp->ah_2g_paprd_rate_mask_4ss_ht20 =
            mptr->modal_header_2g.paprd_rate_mask_4ss_ht20;
    }
    ahp->ah_5g_paprd_rate_mask_ht40 =
        mptr->modal_header_5g.paprd_rate_mask_ht40;
    ahp->ah_5g_paprd_rate_mask_ht20 =
        mptr->modal_header_5g.paprd_rate_mask_ht20;

    //dumping (mptr)struct jetEeprom contents
    //dump_eeprom_contents(mptr);
#else
    /* FTM use two EEP structures in run-time */
    if (!AR_SREV_JET(ah))
        mptr =     &ahp->ah_eeprom;
    else
        mptr_jet = &ahp->ah_eeprom_jet;

    mdata_size = ar9300_eeprom_struct_size(ah);

    if (((!AR_SREV_JET(ah) && (mptr != 0))||(AR_SREV_JET(ah) && (mptr_jet != 0))) && mdata_size > 0) {
#if AH_BYTE_ORDER == AH_BIG_ENDIAN
        if(!AR_SREV_JET(ah)) {
            ar9300_eeprom_template_swap();
            ar9300_swap_eeprom(mptr);
        }
        else{
            ar5500_swap_eeprom(ar5500_eeprom_struct_default());
            ar5500_swap_eeprom(mptr_jet);
        }
#endif
        /*
         * At this point, mptr points to the eeprom data structure
         * in it's "default" state.  If this is big endian, swap the
         * data structures back to "little endian" form.
         */
        if(!AR_SREV_JET(ah)) {
            if (ar9300_eeprom_restore_internal(ah, mptr, mdata_size) >= 0) {
                status = true;
            }
        }else{
            if (ar9300_eeprom_restore_internal(ah, (ar9300_eeprom_t *)mptr_jet, mdata_size) >= 0) {
                status = true;
            }
        }

#if AH_BYTE_ORDER == AH_BIG_ENDIAN
        /* Second Swap, back to Big Endian */
        if(!AR_SREV_JET(ah)) {
            ar9300_eeprom_template_swap();
            ar9300_swap_eeprom(mptr);
        }
        else{
            ar5500_swap_eeprom(ar5500_eeprom_struct_default());
            ar5500_swap_eeprom(mptr_jet);
        }
#endif

    }
    if(!AR_SREV_JET(ah))
    {
        ahp->ah_2g_paprd_rate_mask_ht40 =
            mptr->modal_header_2g.paprd_rate_mask_ht40;
        ahp->ah_2g_paprd_rate_mask_ht20 =
            mptr->modal_header_2g.paprd_rate_mask_ht20;
        ahp->ah_5g_paprd_rate_mask_ht40 =
            mptr->modal_header_5g.paprd_rate_mask_ht40;
        ahp->ah_5g_paprd_rate_mask_ht20 =
            mptr->modal_header_5g.paprd_rate_mask_ht20;
    }
    else
    {
        ahp->ah_2g_paprd_rate_mask_ht40 =
            mptr_jet->modal_header_2g.paprd_rate_mask_ht40;
        ahp->ah_2g_paprd_rate_mask_ht20 =
            mptr_jet->modal_header_2g.paprd_rate_mask_ht20;
        ahp->ah_2g_paprd_rate_mask_4ss_ht40 =
            mptr_jet->modal_header_2g.paprd_rate_mask_4ss_ht40;
        ahp->ah_2g_paprd_rate_mask_4ss_ht20 =
            mptr_jet->modal_header_2g.paprd_rate_mask_4ss_ht20;
        ahp->ah_5g_paprd_rate_mask_ht40 =
            mptr_jet->modal_header_5g.paprd_rate_mask_ht40;
        ahp->ah_5g_paprd_rate_mask_ht20 =
            mptr_jet->modal_header_5g.paprd_rate_mask_ht20;
    }
#endif

    return status;
}
int32_t ar9300_thermometer_get(struct ath_hal *ah)
{
    struct ath_hal_9300 *ahp = AH9300(ah);
    int thermometer;
#ifndef ART_BUILD
    thermometer =
        (ahp->ah_eeprom.base_eep_header.misc_configuration >> 1) & 0x3;
#else
    if (!AR_SREV_JET(ah))
        thermometer = (ahp->ah_eeprom.base_eep_header.misc_configuration >> 1) & 0x3;
    else
        thermometer = (ahp->ah_eeprom_jet.base_eep_header.misc_configuration >> 1) & 0x3;
#endif
    thermometer--;
    return thermometer;
}

bool ar9300_thermometer_apply(struct ath_hal *ah)
{
    int thermometer = ar9300_thermometer_get(ah);

/* ch0_RXTX4 */
/*#define AR_PHY_65NM_CH0_RXTX4       AR_PHY_65NM(ch0_RXTX4)*/
/*#define AR_PHY_65NM_CH1_RXTX4       AR_PHY_65NM(ch1_RXTX4)*/
/*#define AR_PHY_65NM_CH2_RXTX4       AR_PHY_65NM(ch2_RXTX4)*/
/*#define AR_PHY_65NM_CH0_RXTX4_THERM_ON          0x10000000*/
/*#define AR_PHY_65NM_CH0_RXTX4_THERM_ON_S        28*/
#ifndef AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR_S
#define AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR_S      29
#define AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR        \
    (0x1<<AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR_S)
#endif
    if (thermometer < 0) {
        OS_REG_RMW_FIELD(ah,
            AR_PHY_65NM_CH0_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR, 0);
        if (!AR_SREV_HORNET(ah) && !AR_SREV_POSEIDON(ah)) {
            OS_REG_RMW_FIELD(ah,
                    AR_PHY_65NM_CH1_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR, 0);
            if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah)  ) {
                OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH2_RXTX4,
                        AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR, 0);
                if (AR_SREV_JET(ah)) { /* JET */
                    OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH3_RXTX4,
                            AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR, 0);
                }
            }
        }
        OS_REG_RMW_FIELD(ah,
            AR_PHY_65NM_CH0_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
        if (!AR_SREV_HORNET(ah) && !AR_SREV_POSEIDON(ah)) {
            OS_REG_RMW_FIELD(ah,
                    AR_PHY_65NM_CH1_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
            if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah) ) {
                OS_REG_RMW_FIELD(ah,
                        AR_PHY_65NM_CH2_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                if (AR_SREV_JET(ah)) { /* JET */
                    OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH3_RXTX4,
                            AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                }
            }
        }
    } else {
        OS_REG_RMW_FIELD(ah,
            AR_PHY_65NM_CH0_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR, 1);
        if (!AR_SREV_HORNET(ah) && !AR_SREV_POSEIDON(ah)) {
            OS_REG_RMW_FIELD(ah,
                    AR_PHY_65NM_CH1_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR, 1);
            if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah)  ) {
                OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH2_RXTX4,
                        AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR, 1);
                if (AR_SREV_JET(ah)) { /* JET */
                    OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH3_RXTX4,
                            AR_PHY_65NM_CH0_RXTX4_THERM_ON_OVR, 1);
                }
            }
        }
        if (thermometer == 0) {
            OS_REG_RMW_FIELD(ah,
                AR_PHY_65NM_CH0_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 1);
            if (!AR_SREV_HORNET(ah) && !AR_SREV_POSEIDON(ah)) {
                OS_REG_RMW_FIELD(ah,
                        AR_PHY_65NM_CH1_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah) ) {
                    OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH2_RXTX4,
                            AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                    if (AR_SREV_JET(ah)) { /* JET */
                        OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH3_RXTX4,
                                AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                    }
                }
            }
        } else if (thermometer == 1) {
            OS_REG_RMW_FIELD(ah,
                AR_PHY_65NM_CH0_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
            if (!AR_SREV_HORNET(ah) && !AR_SREV_POSEIDON(ah)) {
                OS_REG_RMW_FIELD(ah,
                    AR_PHY_65NM_CH1_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 1);
                if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah) ) {
                    OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH2_RXTX4,
                            AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                    if (AR_SREV_JET(ah)) { /* JET */
                        OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH3_RXTX4,
                                AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                    }
                }
            }
        } else if (thermometer == 2) {
            OS_REG_RMW_FIELD(ah,
                AR_PHY_65NM_CH0_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
            if (!AR_SREV_HORNET(ah) && !AR_SREV_POSEIDON(ah)) {
                OS_REG_RMW_FIELD(ah,
                        AR_PHY_65NM_CH1_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah) ) {
                    OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH2_RXTX4,
                            AR_PHY_65NM_CH0_RXTX4_THERM_ON, 1);
                    if (AR_SREV_JET(ah)) { /* JET */
                        OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH3_RXTX4,
                                AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                    }
                }
            }
        } else if (thermometer == 3) {
            OS_REG_RMW_FIELD(ah,
                    AR_PHY_65NM_CH0_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
            if (!AR_SREV_HORNET(ah) && !AR_SREV_POSEIDON(ah)) {
                OS_REG_RMW_FIELD(ah,
                        AR_PHY_65NM_CH1_RXTX4, AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                if (!AR_SREV_WASP(ah) && !AR_SREV_JUPITER(ah) && !AR_SREV_HONEYBEE(ah) ) {
                    OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH2_RXTX4,
                            AR_PHY_65NM_CH0_RXTX4_THERM_ON, 0);
                    if (AR_SREV_JET(ah)) { /* JET */
                        OS_REG_RMW_FIELD(ah, AR_PHY_65NM_CH3_RXTX4,
                                AR_PHY_65NM_CH0_RXTX4_THERM_ON, 1);
                    }
                }
            }
        }
    }
    return true;
}

static int32_t ar9300_tuning_caps_params_get(struct ath_hal *ah)
{
    int tuning_caps_params;
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif

#ifndef ART_BUILD
    tuning_caps_params = eep->base_eep_header.params_for_tuning_caps[0];
#else
    if (!AR_SREV_JET(ah))
        tuning_caps_params = eep->base_eep_header.params_for_tuning_caps[0];
    else
        tuning_caps_params = eep_jet->base_eep_header.params_for_tuning_caps[0];

#endif
    return tuning_caps_params;
}

/*
 * Read the tuning caps params from eeprom and set to correct register.
 * To regulation the frequency accuracy.
 */
bool ar9300_tuning_caps_apply(struct ath_hal *ah)
{
    int tuning_caps_params;
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
    tuning_caps_params = ar9300_tuning_caps_params_get(ah);
#ifndef ART_BUILD
    if ((eep->base_eep_header.feature_enable & 0x40) >> 6) {
#else
    if ((!AR_SREV_JET(ah) && (eep->base_eep_header.feature_enable & 0x40)) ||
        (AR_SREV_JET(ah) && (eep_jet->base_eep_header.feature_enable & 0x40))){
#endif
        tuning_caps_params &= 0x7f;

        if (AR_SREV_HORNET(ah) || AR_SREV_POSEIDON(ah) || AR_SREV_WASP(ah) || AR_SREV_HONEYBEE(ah)) {
            return true;
        } else if (AR_SREV_SCORPION(ah)) {
            OS_REG_RMW_FIELD(ah,
                    AR_SCORPION_CH0_XTAL, AR_OSPREY_CHO_XTAL_CAPINDAC,
                    tuning_caps_params);
            OS_REG_RMW_FIELD(ah,
                    AR_SCORPION_CH0_XTAL, AR_OSPREY_CHO_XTAL_CAPOUTDAC,
                    tuning_caps_params);
        } else if (AR_SREV_DRAGONFLY(ah)) {
            OS_REG_RMW_FIELD(ah,
                    AR_DRAGONFLY_CH0_XTAL, AR_OSPREY_CHO_XTAL_CAPINDAC,
                    tuning_caps_params);
            OS_REG_RMW_FIELD(ah,
                    AR_DRAGONFLY_CH0_XTAL, AR_OSPREY_CHO_XTAL_CAPOUTDAC,
                    tuning_caps_params);
        } else if (AR_SREV_JET(ah)) {
#ifndef ART_BUILD
            OS_REG_RMW_FIELD_2(ah, XTAL, XTAL_CAPINDAC, eep->base_eep_header.params_for_tuning_caps[0]);
            OS_REG_RMW_FIELD_2(ah, XTAL, XTAL_CAPOUTDAC,eep->base_eep_header.params_for_tuning_caps[1]);
#else
            if(!AR_SREV_JET(ah)){
                OS_REG_RMW_FIELD_2(ah, XTAL, XTAL_CAPINDAC, eep->base_eep_header.params_for_tuning_caps[0]);
                OS_REG_RMW_FIELD_2(ah, XTAL, XTAL_CAPOUTDAC,eep->base_eep_header.params_for_tuning_caps[1]);
            }else{
                OS_REG_RMW_FIELD_2(ah, XTAL, XTAL_CAPINDAC, eep_jet->base_eep_header.params_for_tuning_caps[0]);
                OS_REG_RMW_FIELD_2(ah, XTAL, XTAL_CAPOUTDAC,eep_jet->base_eep_header.params_for_tuning_caps[1]);
            }
#endif
        } else {
            OS_REG_RMW_FIELD(ah,
                    AR_OSPREY_CH0_XTAL, AR_OSPREY_CHO_XTAL_CAPINDAC,
                    tuning_caps_params);
            OS_REG_RMW_FIELD(ah,
                    AR_OSPREY_CH0_XTAL, AR_OSPREY_CHO_XTAL_CAPOUTDAC,
                    tuning_caps_params);
        }

    }
    return true;
}

/*
 * Read the tx_frame_to_xpa_on param from eeprom and apply the value to
 * correct register.
 */
bool ar9300_xpa_timing_control_apply(struct ath_hal *ah, bool is_2ghz)
{
    u_int8_t xpa_timing_control;
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif
#ifndef ART_BUILD
    if ((eep->base_eep_header.feature_enable & 0x80) >> 7) {
#else
    if ((!AR_SREV_JET(ah) && (eep->base_eep_header.feature_enable & 0x80)) ||
        (AR_SREV_JET(ah) && (eep_jet->base_eep_header.feature_enable & 0x80))) {
#endif
		if (AR_SREV_OSPREY(ah) || AR_SREV_AR9580(ah) || AR_SREV_WASP(ah) || AR_SREV_HONEYBEE(ah) || AR_SREV_DRAGONFLY(ah) || AR_SREV_JET(ah)) {
			if (is_2ghz) {
#ifndef ART_BUILD
                xpa_timing_control = eep->modal_header_2g.tx_frame_to_xpa_on;
#else
                if (!AR_SREV_JET(ah))
                    xpa_timing_control = eep->modal_header_2g.tx_frame_to_xpa_on;
                else
                    xpa_timing_control = eep_jet->modal_header_2g.tx_frame_to_xpa_on;
#endif
                OS_REG_RMW_FIELD(ah,
						AR_PHY_XPA_TIMING_CTL, AR_PHY_XPA_TIMING_CTL_FRAME_XPAB_ON,
						xpa_timing_control);
			} else {
#ifndef ART_BUILD
                xpa_timing_control = eep->modal_header_5g.tx_frame_to_xpa_on;
#else
                if (!AR_SREV_JET(ah))
                    xpa_timing_control = eep->modal_header_5g.tx_frame_to_xpa_on;
                else
                    xpa_timing_control = eep_jet->modal_header_5g.tx_frame_to_xpa_on;
#endif
                OS_REG_RMW_FIELD(ah,
						AR_PHY_XPA_TIMING_CTL, AR_PHY_XPA_TIMING_CTL_FRAME_XPAA_ON,
						xpa_timing_control);
			}
		}
	}
    return true;
}


/*
 * Read the xLNA_bias_strength param from eeprom and apply the value to
 * correct register.
 */
bool ar9300_x_lNA_bias_strength_apply(struct ath_hal *ah, bool is_2ghz)
{
    u_int8_t x_lNABias;
    u_int32_t value = 0;
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *eep_jet = &AH9300(ah)->ah_eeprom_jet;
#endif

#ifndef ART_BUILD
    if ((eep->base_eep_header.misc_configuration & 0x40) >> 6) {
#else
    if ((!AR_SREV_JET(ah) && (eep->base_eep_header.misc_configuration & 0x40)) ||
        (AR_SREV_JET(ah) && (eep_jet->base_eep_header.misc_configuration & 0x40))) {
#endif
        if (AR_SREV_OSPREY(ah)) {
            if (is_2ghz) {
                x_lNABias = eep->modal_header_2g.xLNA_bias_strength;
            } else {
                x_lNABias = eep->modal_header_5g.xLNA_bias_strength;
            }
            value = x_lNABias & ( 0x03 );	// bit0,1 for chain0
            OS_REG_RMW_FIELD(ah,
					AR_PHY_65NM_CH0_RXTX4, AR_PHY_65NM_RXTX4_XLNA_BIAS, value);
            value = (x_lNABias >> 2) & ( 0x03 );	// bit2,3 for chain1
            OS_REG_RMW_FIELD(ah,
					AR_PHY_65NM_CH1_RXTX4, AR_PHY_65NM_RXTX4_XLNA_BIAS, value);
            value = (x_lNABias >> 4) & ( 0x03 );	// bit4,5 for chain2
            OS_REG_RMW_FIELD(ah,
					AR_PHY_65NM_CH2_RXTX4, AR_PHY_65NM_RXTX4_XLNA_BIAS, value);
        }
    }
    return true;
}


u_int8_t ar9300_obdboffst_aux1_get(struct ath_hal *ah, int chain, bool is_2ghz)
{
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    if (is_2ghz) {
        return eep->modal_header_2g.obdboffst_aux1[chain];
    } else {
        return eep->modal_header_5g.obdboffst_aux1[chain];
    }
#else
if (!AR_SREV_JET(ah))
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    if (is_2ghz) {
        return eep->modal_header_2g.obdboffst_aux1[chain];
    } else {
        return eep->modal_header_5g.obdboffst_aux1[chain];
    }
}
else
{
    ar5500_eeprom_t *eep = &AH9300(ah)->ah_eeprom_jet;
    if (is_2ghz) {
        return eep->modal_header_2g.obdboffst_aux1[chain];
    } else {
        return eep->modal_header_5g.obdboffst_aux1[chain];
    }
}
#endif
}

u_int8_t ar9300_obdboffst_aux2_get(struct ath_hal *ah, int chain, bool is_2ghz)
{
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    if (is_2ghz) {
        return eep->modal_header_2g.obdboffst_aux2[chain];
    } else {
        return eep->modal_header_5g.obdboffst_aux2[chain];
    }
#else
if (!AR_SREV_JET(ah))
{
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
    if (is_2ghz) {
        return eep->modal_header_2g.obdboffst_aux2[chain];
    } else {
        return eep->modal_header_5g.obdboffst_aux2[chain];
    }
}
else
{
    ar5500_eeprom_t *eep = &AH9300(ah)->ah_eeprom_jet;
    if (is_2ghz) {
        return eep->modal_header_2g.obdboffst_aux2[chain];
    } else {
        return eep->modal_header_5g.obdboffst_aux2[chain];
    }
}
#endif
}

/* Jet-specific API */
int8_t ar9300_aux12_lboffset_get(struct ath_hal *ah)
{
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#else
    ar5500_eeprom_t *eep = &AH9300(ah)->ah_eeprom_jet;
#endif
    int8_t  aux12_lboffset;
    if (AR_SREV_JET(ah))
    {
        if (eep->template_version == ar9300_eeprom_template_jet || (eep->ref_file_version != 0xFF && eep->ref_file_version >= 2))
            aux12_lboffset = eep->aux12_lboffset;
        else
            aux12_lboffset = eep->modal_header_2g.reserved[0];

        if (aux12_lboffset == 0 || aux12_lboffset == -1)
            aux12_lboffset = 10;
        return aux12_lboffset;
    }
    else
        return 0;
}

/* Jet-specific API */
int8_t ar9300_aux12_hboffset_get(struct ath_hal *ah)
{
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
#else
    ar5500_eeprom_t *eep = &AH9300(ah)->ah_eeprom_jet;
#endif
    int8_t  aux12_hboffset;
    if (AR_SREV_JET(ah))
    {
        if (eep->template_version == ar9300_eeprom_template_jet || (eep->ref_file_version != 0xFF && eep->ref_file_version >= 2))
            aux12_hboffset = eep->aux12_hboffset;
        else
            aux12_hboffset = eep->modal_header_5g.reserved[0];

        if (aux12_hboffset == 0 || aux12_hboffset == -1)
            aux12_hboffset = 100;
        return aux12_hboffset;
    }
    else
        return 0;
}

/* Jet-specific API */
void ar9300_obdboffst_aux1_aux2_apply_ext(struct ath_hal *ah, bool is_2ghz)
{
  u_int8_t aux1, aux2;
  if (AR_SREV_JET(ah))
  {
    aux1 = ar9300_obdboffst_aux1_get(ah, 0, is_2ghz);
    aux2 = ar9300_obdboffst_aux2_get(ah, 0, is_2ghz);
    aux1 += 2; aux2 += 2;
    OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH0_TXRF2,
      AR_PHY_65NM_CH0_TXRF2_OBDBOFFST2G_AUX1, aux1);
    OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH0_TXRF2,
      AR_PHY_65NM_CH0_TXRF2_OBDBOFFST2G_AUX2, aux2);
    aux1 = ar9300_obdboffst_aux1_get(ah, 1, is_2ghz);
    aux2 = ar9300_obdboffst_aux2_get(ah, 1, is_2ghz);
    aux1 += 2; aux2 += 2;
    OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH1_TXRF2,
      AR_PHY_65NM_CH1_TXRF2_OBDBOFFST2G_AUX1, aux1);
    OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH1_TXRF2,
      AR_PHY_65NM_CH1_TXRF2_OBDBOFFST2G_AUX2, aux2);
    aux1 = ar9300_obdboffst_aux1_get(ah, 2, is_2ghz);
    aux2 = ar9300_obdboffst_aux2_get(ah, 2, is_2ghz);
    aux1 += 2; aux2 += 2;
    OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH2_TXRF2,
      AR_PHY_65NM_CH2_TXRF2_OBDBOFFST2G_AUX1, aux1);
    OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH2_TXRF2,
      AR_PHY_65NM_CH2_TXRF2_OBDBOFFST2G_AUX2, aux2);
    aux1 = ar9300_obdboffst_aux1_get(ah, 3, is_2ghz);
    aux2 = ar9300_obdboffst_aux2_get(ah, 3, is_2ghz);
    aux1 += 2; aux2 += 2;
    OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH3_TXRF2,
      AR_PHY_65NM_CH3_TXRF2_OBDBOFFST2G_AUX1, aux1);
    OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH3_TXRF2,
      AR_PHY_65NM_CH3_TXRF2_OBDBOFFST2G_AUX2, aux2);
  }
}

void ar9300_obdboffst_aux1_aux2_apply(struct ath_hal *ah, bool is_2ghz)
{
    u_int8_t aux1, aux2;
    struct ath_hal_9300     *ahp = AH9300(ah);
    u_int8_t ate_temp = 0;
    int8_t aux12_lboffset;
    int8_t aux12_hboffset;

    aux12_lboffset =  ar9300_aux12_lboffset_get(ah);
    aux12_hboffset =  ar9300_aux12_hboffset_get(ah);
    if (AR_SREV_JET(ah)) {
        if (ar9300_rx_gain_index_get(ah) == 1) { // add offset for iPA board(Qcn5500,12x12,YB413)
            ar9300_otp_read_byte(ah, ate_temp_code_chain1_7_0_addr,&ate_temp,true);
            //UserPrint("ate_temp_code_chain1=%d\n", ate_temp);
            //UserPrint("latest_therm_val=%d\n", ahp->lastThermalValueReading);
            //UserPrint("aux12_LB=%d aux12_HB=%d\n", (ate_temp + aux12_lboffset),(ate_temp - aux12_hboffset));
            if ((ahp->lastThermalValueReading != 0) && (ahp->lastThermalValueReading > (ate_temp + aux12_lboffset)))
	    	{
                //UserPrint("Meet low temp case\n");
                /* aux1/aux2 += 2 at low temp condition */
                ar9300_obdboffst_aux1_aux2_apply_ext(ah, is_2ghz);
                return;
            }
        }

	    /* apply original setting for both room and high temperature case.*/
        aux1 = ar9300_obdboffst_aux1_get(ah, 0, is_2ghz);
        aux2 = ar9300_obdboffst_aux2_get(ah, 0, is_2ghz);
        OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH0_TXRF2,
                AR_PHY_65NM_CH0_TXRF2_OBDBOFFST2G_AUX1, aux1);
        OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH0_TXRF2,
                AR_PHY_65NM_CH0_TXRF2_OBDBOFFST2G_AUX2, aux2);

        aux1 = ar9300_obdboffst_aux1_get(ah, 1, is_2ghz);
        aux2 = ar9300_obdboffst_aux2_get(ah, 1, is_2ghz);
        OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH1_TXRF2,
                AR_PHY_65NM_CH1_TXRF2_OBDBOFFST2G_AUX1, aux1);
        OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH1_TXRF2,
                AR_PHY_65NM_CH1_TXRF2_OBDBOFFST2G_AUX2, aux2);

        aux1 = ar9300_obdboffst_aux1_get(ah, 2, is_2ghz);
        aux2 = ar9300_obdboffst_aux2_get(ah, 2, is_2ghz);
        OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH2_TXRF2,
                AR_PHY_65NM_CH2_TXRF2_OBDBOFFST2G_AUX1, aux1);
        OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH2_TXRF2,
                AR_PHY_65NM_CH2_TXRF2_OBDBOFFST2G_AUX2, aux2);

        aux1 = ar9300_obdboffst_aux1_get(ah, 3, is_2ghz);
        aux2 = ar9300_obdboffst_aux2_get(ah, 3, is_2ghz);
        OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH3_TXRF2,
                AR_PHY_65NM_CH3_TXRF2_OBDBOFFST2G_AUX1, aux1);
        OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH3_TXRF2,
                AR_PHY_65NM_CH3_TXRF2_OBDBOFFST2G_AUX2, aux2);

        if (ar9300_rx_gain_index_get(ah) == 1) { // add offset for iPA board(Qcn5500,12x12,YB413)
            if ((ahp->lastThermalValueReading != 0) && (ahp->lastThermalValueReading < (ate_temp - aux12_hboffset)))
            {
                //UserPrint("Meet high temp case\n");
                /* adjust TXRF2.ocas2g at high temp condition after applying original setting.*/
                OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH0_TXRF2,
                AR_PHY_65NM_CH0_TXRF2_OCAS2G, 0);

                OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH1_TXRF2,
                AR_PHY_65NM_CH0_TXRF2_OCAS2G, 0);

                OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH2_TXRF2,
                AR_PHY_65NM_CH0_TXRF2_OCAS2G, 0);

                OS_REG_RMW_FIELD_ALT(ah, AR_PHY_65NM_CH3_TXRF2,
                AR_PHY_65NM_CH0_TXRF2_OCAS2G, 0);
            }
        }
    }

}

/*
 * Read EEPROM header info and program the device for correct operation
 * given the channel value.
 */
bool
ar9300_eeprom_set_board_values(struct ath_hal *ah, HAL_CHANNEL_INTERNAL *chan)
{
    ar9300_xpa_bias_level_apply(ah, IS_CHAN_2GHZ(chan));

    ar9300_xpa_timing_control_apply(ah, IS_CHAN_2GHZ(chan));

    ar9300_ant_ctrl_apply(ah, IS_CHAN_2GHZ(chan));
#if !defined(AR9300_EMULATION)
    ar9300_drive_strength_apply(ah);
#endif

    ar9300_x_lNA_bias_strength_apply(ah, IS_CHAN_2GHZ(chan));

	/* wait for Poseidon internal regular turnning */
    /* for Hornet we move it before initPLL to avoid an access issue */
#if !defined(AR9300_EMULATION)
    /* Function not used when EMULATION. */
    if (!AR_SREV_HORNET(ah) && !AR_SREV_WASP(ah) && !AR_SREV_HONEYBEE(ah)) {
        ar9300_internal_regulator_apply(ah);
    }
    if (AR_SREV_HONEYBEE_20(ah)) {
        ar9300_internal_regulator_apply(ah);
    }
#endif

    ar9300_attenuation_apply(ah, chan->channel);
    ar9300_quick_drop_apply(ah, chan->channel);
#if !defined(AR9300_EMULATION)
    /* set some radio reg */
    ar9300_thermometer_apply(ah);
#endif
    if(!AR_SREV_WASP(ah))
    {
        ar9300_tuning_caps_apply(ah);
    }
	ar9300_minCCApwr_thres_apply(ah, chan->channel);
    ar9300_tx_end_to_xpab_off_apply(ah, chan->channel);
    ar9300_obdboffst_aux1_aux2_apply(ah, IS_CHAN_2GHZ(chan));

    return true;
}

u_int8_t *
ar9300_eeprom_get_spur_chans_ptr(struct ath_hal *ah, bool is_2ghz)
{
#ifndef ART_BUILD
    ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;

    if (is_2ghz) {
        return &(eep->modal_header_2g.spur_chans[0]);
    } else {
        return &(eep->modal_header_5g.spur_chans[0]);
    }
#else
    if (!AR_SREV_JET(ah))
    {
        ar9300_eeprom_t *eep = &AH9300(ah)->ah_eeprom;
        if (is_2ghz) {
            return &(eep->modal_header_2g.spur_chans[0]);
        } else {
            return &(eep->modal_header_5g.spur_chans[0]);
        }
    } else {
        ar5500_eeprom_t *eep = &AH9300(ah)->ah_eeprom_jet;
        if (is_2ghz) {
            return &(eep->modal_header_2g.spur_chans[0]);
        } else {
            return &(eep->modal_header_5g.spur_chans[0]);
        }
    }

#endif
}

static u_int8_t ar9300_eeprom_get_tx_gain_table_number_max(struct ath_hal *ah)
{
    unsigned long tx_gain_table_max;
    tx_gain_table_max = OS_REG_READ_FIELD(ah,
        AR_PHY_TPC_7, AR_PHY_TPC_7_TX_GAIN_TABLE_MAX);
    return tx_gain_table_max;
}

u_int8_t ar9300_eeprom_tx_gain_table_index_max_apply(struct ath_hal *ah, u_int16_t channel)
{
    unsigned int index;
    struct ath_hal_9300 *ahp = AH9300(ah);
#ifndef ART_BUILD
    ar9300_eeprom_t *ahp_Eeprom;
    ahp_Eeprom = &ahp->ah_eeprom;

    if (ahp_Eeprom->base_ext1.misc_enable == 0)
        return false;

    if (channel < 4000)
        index = ahp_Eeprom->modal_header_2g.tx_gain_cap;
    else
        index = ahp_Eeprom->modal_header_5g.tx_gain_cap;
#else
    if (!AR_SREV_JET(ah)){
        ar9300_eeprom_t *ahp_Eeprom;
        ahp_Eeprom = &ahp->ah_eeprom;

        if (ahp_Eeprom->base_ext1.misc_enable == 0)
            return false;

        if (channel < 4000)
            index = ahp_Eeprom->modal_header_2g.tx_gain_cap;
        else
            index = ahp_Eeprom->modal_header_5g.tx_gain_cap;
    }
    else
    {
        ar5500_eeprom_t *ahp_Eeprom;
        ahp_Eeprom = &ahp->ah_eeprom_jet;

        if (ahp_Eeprom->base_ext1.misc_enable == 0)
            return false;

        if (channel < 4000)
            index = ahp_Eeprom->modal_header_2g.tx_gain_cap;
        else
            index = ahp_Eeprom->modal_header_5g.tx_gain_cap;

    }

#endif
    OS_REG_RMW_FIELD(ah,
        AR_PHY_TPC_7, AR_PHY_TPC_7_TX_GAIN_TABLE_MAX, index);
    return true;
}

static u_int8_t ar9300_eeprom_get_pcdac_tx_gain_table_i(struct ath_hal *ah,
                                               int i, u_int8_t *pcdac)
{
    unsigned long tx_gain;
    u_int8_t tx_gain_table_max;
    tx_gain_table_max = ar9300_eeprom_get_tx_gain_table_number_max(ah);
    if (i <= 0 || i > tx_gain_table_max) {
        *pcdac = 0;
        return false;
    }

    tx_gain = OS_REG_READ(ah, AR_PHY_TXGAIN_TAB(1) + i * 4);
    *pcdac = ((tx_gain >> 24) & 0xff);
    return true;
}

u_int8_t ar9300_eeprom_set_tx_gain_cap(struct ath_hal *ah,
                                               int *tx_gain_max)
// pcdac read back from reg, read back value depends on reset 2GHz/5GHz ini
// tx_gain_table, this function will be called twice after each
// band's calibration.
// after 2GHz cal, tx_gain_max[0] has 2GHz, calibration max txgain,
// tx_gain_max[1]=-100
// after 5GHz cal, tx_gain_max[0],tx_gain_max[1] have calibration
// value for both band
// reset is on 5GHz, reg reading from tx_gain_table is for 5GHz,
// so program can't recalculate 2g.tx_gain_cap at this point.
{
    int i = 0, ig, im = 0;
    u_int8_t pcdac = 0;
    u_int8_t tx_gain_table_max;
    ar9300_eeprom_t *ahp_Eeprom;
#ifdef ART_BUILD
    ar5500_eeprom_t *ahp_Eeprom_jet;
#endif
    struct ath_hal_9300 *ahp = AH9300(ah);

    if(ah == NULL) {
        return false;
    }
#ifndef ART_BUILD
    ahp_Eeprom = &ahp->ah_eeprom;
    if (ahp_Eeprom->base_ext1.misc_enable == 0)
        return false;
#else
    if(!AR_SREV_JET(ah))
    {
        ahp_Eeprom = &ahp->ah_eeprom;
        if (ahp_Eeprom->base_ext1.misc_enable == 0)
            return false;
    }else{
        ahp_Eeprom_jet = &ahp->ah_eeprom_jet;
        if (ahp_Eeprom_jet->base_ext1.misc_enable == 0)
            return false;
    }
#endif
    tx_gain_table_max = ar9300_eeprom_get_tx_gain_table_number_max(ah);

    for (i = 0; i < 2; i++) {
        if (tx_gain_max[i]>-100) {	// -100 didn't cal that band.
            if ( i== 0) {
                if (tx_gain_max[1]>-100) {
                    continue;
                    // both band are calibrated, skip 2GHz 2g.tx_gain_cap reset
                }
            }
            for (ig = 1; ig <= tx_gain_table_max; ig++) {
                if (ah->ah_reset != 0)
                {
                    ar9300_eeprom_get_pcdac_tx_gain_table_i(ah, ig, &pcdac);
                    if (pcdac >= tx_gain_max[i])
                        break;
                }
            }
            if (ig+1 <= tx_gain_table_max) {
                if (pcdac == tx_gain_max[i])
                    im = ig;
                else
                    im = ig + 1;
                if (i == 0) {
#ifndef ART_BUILD
                    ahp_Eeprom->modal_header_2g.tx_gain_cap = im;
#else
                    if (!AR_SREV_JET(ah))
                        ahp_Eeprom->modal_header_2g.tx_gain_cap = im;
                    else
                        ahp_Eeprom_jet->modal_header_2g.tx_gain_cap = im;
#endif
                } else {
#ifndef ART_BUILD
                    ahp_Eeprom->modal_header_5g.tx_gain_cap = im;
#else
                    if (!AR_SREV_JET(ah))
                        ahp_Eeprom->modal_header_5g.tx_gain_cap = im;
                    else
                        ahp_Eeprom_jet->modal_header_5g.tx_gain_cap = im;
#endif
                }
            } else {
                if (i == 0) {
#ifndef ART_BUILD
                    ahp_Eeprom->modal_header_2g.tx_gain_cap = ig;
#else
                    if (!AR_SREV_JET(ah))
                        ahp_Eeprom->modal_header_2g.tx_gain_cap = ig;
                    else
                        ahp_Eeprom_jet->modal_header_2g.tx_gain_cap = ig;
#endif
                } else {
#ifndef ART_BUILD
                    ahp_Eeprom->modal_header_5g.tx_gain_cap = ig;
#else
                    if (!AR_SREV_JET(ah))
                        ahp_Eeprom->modal_header_5g.tx_gain_cap = ig;
                    else
                        ahp_Eeprom_jet->modal_header_5g.tx_gain_cap = ig;
#endif
                }
            }
        }
    }
    return true;
}
#ifndef ART_BUILD
bool Ar5500EepromConfigAddrGet(u_int32_t *EepAddr,u_int32_t *value, int *num,u_int32_t *(*NextAddr))
{
    u_int32_t tmpConfigAddr=0;
    if((*EepAddr!=0) && (*EepAddr!= CONFIG_ADDR_INIT))
    {
        int i=0;
        tmpConfigAddr = *EepAddr;
        if(((tmpConfigAddr&CONFIG_ADDR_MODE_MASK)>>20) ==1){for(i=0;i<2;i++){value[i]=EepAddr[i];} *num =2; *NextAddr = EepAddr+2;}//2
        else if(((tmpConfigAddr&CONFIG_ADDR_MODE_MASK)>>20) ==2 || ((tmpConfigAddr&CONFIG_ADDR_MODE_MASK)>>20) ==4){for(i=0;i<3;i++){value[i]=EepAddr[i];} *num =3; *NextAddr = EepAddr+3;}//3
        else if(((tmpConfigAddr&CONFIG_ADDR_MODE_MASK)>>20) ==3){for(i=0;i<5;i++){value[i]=EepAddr[i];} *num =5; *NextAddr = EepAddr+5;}//5
        else { *NextAddr = EepAddr; return false;}
        return true;
    }
    else{ *NextAddr = EepAddr;  return false;}
}
#endif
/* Jet specific: Sticky write setting - a calibration parameter stored in Flash Cal-Data sector(Eeprom structure) */
bool ar9300_sticky_write_configaddr(struct ath_hal *ah, HAL_CHANNEL *chan, int doStickyWriteTiming)
{
#ifndef ART_BUILD
    ar9300_eeprom_t       *eep = &AH9300(ah)->ah_eeprom;
#else
    ar5500_eeprom_t       *eep = &AH9300(ah)->ah_eeprom_jet;
#endif

    u_int32_t *pEepromAddr     = eep->configAddr;
    u_int32_t *pNextEepromAddr = eep->configAddr;

    u_int mode_index=0;
    switch (chan->channel_flags & CHANNEL_ALL) {
    case CHANNEL_A:
        mode_index=0;  break; //5G
    case CHANNEL_A_HT20:
        mode_index=1;  break; //5G HT20
    case CHANNEL_A_HT40PLUS:
        mode_index=2;  break; //5G HT40 PLUS
    case CHANNEL_A_HT40MINUS:
        mode_index=3;  break; //5G HT40 MINUS
    case CHANNEL_B:
        mode_index=4;  break; //2G CCK
    case CHANNEL_G:
        mode_index=5;  break; //2G OFDM
    case CHANNEL_G_HT20:
        mode_index=6;  break; //2G HT20
    case CHANNEL_G_HT40PLUS:
        mode_index=7;  break; //2G HT40 PLUS
    case CHANNEL_G_HT40MINUS:
        mode_index=8;  break; //2G HT40 MINUS
    case CHANNEL_108G:
        mode_index=9;  break; //(CHANNEL_2GHZ|CHANNEL_OFDM|CHANNEL_TURBO)
    case CHANNEL_ALL:
        mode_index=10;  break; //all mode
    default: return false;
    }
    if(doStickyWriteTiming == CONFIG_ADDR_BEFORE_INIT_CAL)
    {
        bool bRet = false;
        u_int32_t tmpValue[5]={0,0,0,0,0};
        int tmpNum=0;
        do {
            bRet = Ar5500EepromConfigAddrGet(pEepromAddr, tmpValue, &tmpNum, &pNextEepromAddr);
            if(bRet ==true) {
                pEepromAddr = pNextEepromAddr;
                u_int32_t regAddr = tmpValue[0]&CONFIG_ADDR_ADDRESS_MASK;
                if(((tmpValue[0]&CONFIG_ADDR_PREPOST_MASK)>>24) ==1) continue;
                else {
                    if(((tmpValue[0]&CONFIG_ADDR_MODE_MASK)>>20) ==1) {
                        if( (mode_index>=0) && (mode_index<=10)) {
                            OS_REG_WRITE(ah,regAddr,tmpValue[1]);
                            OS_DELAY(50);
                        }
                    } else if(((tmpValue[0]&CONFIG_ADDR_MODE_MASK)>>20) ==2) {
                        if((mode_index>=4) && (mode_index<=9)) {
                            OS_REG_WRITE(ah,regAddr,tmpValue[1]);
                            OS_DELAY(50);
                        } else if((mode_index>=0) && (mode_index<=3) ){
                            OS_REG_WRITE(ah,regAddr,tmpValue[2]);
                            OS_DELAY(50);
                        }
                    } else if(((tmpValue[0]&CONFIG_ADDR_MODE_MASK)>>20) ==3) {
                        if(mode_index ==6 ) {
                            OS_REG_WRITE(ah,regAddr,tmpValue[1]);
                            OS_DELAY(50);
                        }  //2G HT20
                        else if(mode_index==7 ||mode_index==8) {
                            OS_REG_WRITE(ah,regAddr,tmpValue[2]);
                            OS_DELAY(50);
                        }  //2G HT40
                        else if(mode_index==1) {
                            OS_REG_WRITE(ah,regAddr,tmpValue[3]);
                            OS_DELAY(50);
                        }  //5G HT20
                        else if(mode_index==2 ||mode_index==3) {
                            OS_REG_WRITE(ah,regAddr,tmpValue[4]);
                            OS_DELAY(50);
                        }  //5G HT40
                    } else if(((tmpValue[0]&CONFIG_ADDR_MODE_MASK)>>20) ==4) {
                        /* sticky field write, only support single value setting.
                         * format of sticky write mode 4 which support subfield override
                         * <addr_32b> <value_32b> <bit_info_32b>
                         *  <bit_info_32b>[7:0]: LSB of subfield
                         *  <bit_info_32b>[15:8]:MSB of subfield
                         *  eg: LSB=2, MSB=6, bit mask would be 0x7C
                         */
                        u_int32_t mask, low, high, ui32_temp;
                        low = (tmpValue[2] & 0xFF);
                        high = (tmpValue[2] & 0xFF00)>>8;
                        mask = (1<<(high-low+1))-1;
                        mask = mask << low;
                        ui32_temp = OS_REG_READ(ah,regAddr);
                        ui32_temp = ui32_temp & ~mask;
                        ui32_temp = ui32_temp | ((tmpValue[1] << low) & mask);
                        if( (mode_index>=0) && (mode_index<=10)) {
                            OS_REG_WRITE(ah,regAddr,ui32_temp);
                            OS_DELAY(50);
                        }
                    } else{}
                }
            }
        }while(bRet==true);
    } else if(doStickyWriteTiming == CONFIG_ADDR_AFTER_INIT_CAL) {
        bool bRet = false;
        u_int32_t tmpValue[5]={0,0,0,0,0};
        int tmpNum=0;
        do {
            bRet = Ar5500EepromConfigAddrGet(pEepromAddr, tmpValue, &tmpNum, &pNextEepromAddr);
            if(bRet ==true) {
                pEepromAddr = pNextEepromAddr;
                u_int32_t regAddr = tmpValue[0] & CONFIG_ADDR_ADDRESS_MASK;
                if(((tmpValue[0]&CONFIG_ADDR_PREPOST_MASK)>>24) ==0) continue;
                else {
                    if(((tmpValue[0]&CONFIG_ADDR_MODE_MASK)>>20) ==1) {
                        if( (mode_index>=0) && (mode_index<=10)){OS_REG_WRITE(ah,regAddr,tmpValue[1]);    OS_DELAY(50);}
                    }
                    else if(((tmpValue[0]&CONFIG_ADDR_MODE_MASK)>>20) ==2)
                    {
                        if((mode_index>=4) && (mode_index<=9) )     {OS_REG_WRITE(ah,regAddr,tmpValue[1]);    OS_DELAY(50);}
                        else if((mode_index>=0) && (mode_index<=3) ){OS_REG_WRITE(ah,regAddr,tmpValue[2]);    OS_DELAY(50);}
                    }
                    else if(((tmpValue[0]&CONFIG_ADDR_MODE_MASK)>>20) ==3)
                    {
                        if(mode_index ==6 )                   {OS_REG_WRITE(ah,regAddr,tmpValue[1]);    OS_DELAY(50);}
                        else if(mode_index==7 ||mode_index==8){OS_REG_WRITE(ah,regAddr,tmpValue[2]);    OS_DELAY(50);}
                        else if(mode_index==1)                {OS_REG_WRITE(ah,regAddr,tmpValue[3]);    OS_DELAY(50);}
                        else if(mode_index==2 ||mode_index==3){OS_REG_WRITE(ah,regAddr,tmpValue[4]);    OS_DELAY(50);}
                    }
                    else if(((tmpValue[0]&CONFIG_ADDR_MODE_MASK)>>20) ==4)
                    {   /* sticky field write, only support single value setting.
                         * format of sticky write mode 4 which support subfield override
                         * <addr_32b> <value_32b> <bit_info_32b>
                         *  <bit_info_32b>[7:0]: LSB of subfield
                         *  <bit_info_32b>[15:8]:MSB of subfield
                         *  eg: LSB=2, MSB=6, bit mask would be 0x7C
                         */
                        u_int32_t mask, low, high, ui32_temp;
                        low = (tmpValue[2] & 0xFF);
                        high = (tmpValue[2] & 0xFF00)>>8;
                        mask = (1<<(high-low+1))-1;
                        mask = mask << low;
                        ui32_temp = OS_REG_READ(ah,regAddr);
                        ui32_temp = ui32_temp & ~mask;
                        ui32_temp = ui32_temp | ((tmpValue[1] << low) & mask);
                        if( (mode_index>=0) && (mode_index<=10))    {OS_REG_WRITE(ah,regAddr,ui32_temp);    OS_DELAY(50);}
                    }
                    else{}
                }
            }
        }while(bRet==true);
    }
    return true;
}


/* Jet specific: per-rate power offset setting - a calibration parameter stored in calibration storage */
#define NUM_POWER_OFFSET_USE_DESIRED_SCALED 18 /* 4 Higher Rate of 4SS (4x4), MCS0_8_16_24, MCS1_3_9_11_17_19_25_27 */
bool ar9300_modify_desired_scale_with_power_offset(struct ath_hal *ah)
{
#ifndef ART_BUILD
    ar9300_eeprom_t       *eep = &AH9300(ah)->ah_eeprom;
#else
    ar5500_eeprom_t       *eep = &AH9300(ah)->ah_eeprom_jet;
#endif
    int i;

    u_int32_t tmp_Desired_Scale=0;
    u_int32_t reg_HT20[NUM_POWER_OFFSET_USE_DESIRED_SCALED] = {AR_PHY_TPC_10,AR_PHY_TPC_10,AR_PHY_TPC_10,AR_PHY_TPC_10,AR_PHY_TPC_10,AR_PHY_TPC_10,AR_PHY_TPC_11_B0,AR_PHY_TPC_11_B0,
        AR_PHY_TPC_14,AR_PHY_TPC_14,AR_PHY_TPC_14,AR_PHY_TPC_14,AR_PHY_TPC_14,AR_PHY_TPC_14,QCN5500_PHY_TPC_21,QCN5500_PHY_TPC_21,QCN5500_PHY_TPC_21,QCN5500_PHY_TPC_21};
    u_int32_t reg_HT40[NUM_POWER_OFFSET_USE_DESIRED_SCALED] = {AR_PHY_TPC_12,AR_PHY_TPC_12,AR_PHY_TPC_12,AR_PHY_TPC_12,AR_PHY_TPC_12,AR_PHY_TPC_12,AR_PHY_TPC_13,AR_PHY_TPC_13,
        AR_PHY_TPC_15,AR_PHY_TPC_15,AR_PHY_TPC_15,AR_PHY_TPC_15,AR_PHY_TPC_15,AR_PHY_TPC_15,QCN5500_PHY_TPC_22,QCN5500_PHY_TPC_22,QCN5500_PHY_TPC_22,QCN5500_PHY_TPC_22};
    u_int32_t shift[NUM_POWER_OFFSET_USE_DESIRED_SCALED] = {AR_PHY_TPC_DESIRED_SCALE_0_S,AR_PHY_TPC_DESIRED_SCALE_5_S,AR_PHY_TPC_DESIRED_SCALE_10_S,AR_PHY_TPC_DESIRED_SCALE_15_S,AR_PHY_TPC_DESIRED_SCALE_20_S,AR_PHY_TPC_DESIRED_SCALE_25_S,AR_PHY_TPC_DESIRED_SCALE_0_S,AR_PHY_TPC_DESIRED_SCALE_5_S,
        AR_PHY_TPC_DESIRED_SCALE_0_S,AR_PHY_TPC_DESIRED_SCALE_5_S,AR_PHY_TPC_DESIRED_SCALE_10_S,AR_PHY_TPC_DESIRED_SCALE_15_S,AR_PHY_TPC_DESIRED_SCALE_20_S,AR_PHY_TPC_DESIRED_SCALE_25_S,AR_PHY_TPC_DESIRED_SCALE_0_S,AR_PHY_TPC_DESIRED_SCALE_5_S,AR_PHY_TPC_DESIRED_SCALE_10_S,AR_PHY_TPC_DESIRED_SCALE_15_S};

    if (eep->ref_file_version == 0xff || eep->ref_file_version < 3) /* apply power offset only if ref version >= 3 */
    {
        return false;
    }
    /* Not need do override if all desired scale offset are zero */
    for (i=0; i<6; i++)
    {
        if ((eep->powerOffset2GHT20[i] != 0) || (eep->powerOffset2GHT40[i] != 0))
            break;
    }
    if (i == 6)
    {
        return false;
    }

    int8_t powerOffset_HT20[NUM_POWER_OFFSET_USE_DESIRED_SCALED] = {eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_0_8_16_24],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_1_2_9_10_17_18_25_26],
        eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_3_4_11_12_19_20_27_28],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_5_13_21_29],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_6_14_22_30],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_7_15_23_31],
        eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_3_4_11_12_19_20_27_28],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_5_13_21_29],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_6_14_22_30],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_7_15_23_31],
        eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_3_4_11_12_19_20_27_28],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_5_13_21_29],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_6_14_22_30],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_7_15_23_31],
        eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_3_4_11_12_19_20_27_28],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_5_13_21_29],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_6_14_22_30],eep->powerOffset2GHT20[DESIRED_SCALE_OFFSET_ALL_7_15_23_31],};
    int8_t powerOffset_HT40[NUM_POWER_OFFSET_USE_DESIRED_SCALED] = {eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_0_8_16_24],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_1_2_9_10_17_18_25_26],
        eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_3_4_11_12_19_20_27_28],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_5_13_21_29],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_6_14_22_30],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_7_15_23_31],
        eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_3_4_11_12_19_20_27_28],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_5_13_21_29],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_6_14_22_30],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_7_15_23_31],
        eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_3_4_11_12_19_20_27_28],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_5_13_21_29],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_6_14_22_30],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_7_15_23_31],
        eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_3_4_11_12_19_20_27_28],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_5_13_21_29],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_6_14_22_30],eep->powerOffset2GHT40[DESIRED_SCALE_OFFSET_ALL_7_15_23_31],};
    for(i=0;i<NUM_POWER_OFFSET_USE_DESIRED_SCALED;i++) //HT20
    {
        tmp_Desired_Scale  = OS_REG_READ_FIELD_ALT_2(ah, reg_HT20[i], AR_PHY_TPC_DESIRED_SCALE, shift[i]);   OS_DELAY(30);
        tmp_Desired_Scale = tmp_Desired_Scale + powerOffset_HT20[i];
        OS_REG_RMW_FIELD_ALT_2(ah, reg_HT20[i],AR_PHY_TPC_DESIRED_SCALE, shift[i], tmp_Desired_Scale); OS_DELAY(30);
    }

    for(i=0;i<NUM_POWER_OFFSET_USE_DESIRED_SCALED;i++) //HT40
    {
        tmp_Desired_Scale  = OS_REG_READ_FIELD_ALT_2(ah, reg_HT40[i], AR_PHY_TPC_DESIRED_SCALE, shift[i]);   OS_DELAY(30);
        tmp_Desired_Scale = tmp_Desired_Scale + powerOffset_HT40[i];
        OS_REG_RMW_FIELD_ALT_2(ah, reg_HT40[i],AR_PHY_TPC_DESIRED_SCALE, shift[i], tmp_Desired_Scale); OS_DELAY(30);
    }
    return true;
}
#endif /* AH_SUPPORT_AR9300 */
