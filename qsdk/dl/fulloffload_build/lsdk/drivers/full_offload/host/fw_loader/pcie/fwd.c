/*
 * Copyright (c) 2008-2010, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <adf_os_types.h>
#include <adf_os_mem.h>
#include <adf_os_util.h>
#include <adf_os_lock.h>
#include <adf_os_timer.h>
#include <adf_os_time.h>
#include <adf_os_module.h>
#include <adf_os_atomic.h>
#include <adf_net_types.h>
#include <adf_nbuf.h>
#include <adf_os_io.h>

#include "fwd.h"

/* to get more flexibility, we can have two different
   modes, either target can accept huge set of bytes, 
   that need to be copied from one address to other
   OR just simple memory writes, host can choose to
   do selective write 
*/

#ifdef ROM_MODE

//void fwd_parse_str(char str[], char addr[], char sep, int len);
//void fwd_parse_params(hif_gmac_params_t   *params);

a_uint16_t      dump_pkt = 0;
a_uint16_t      dump_pkt_lim = 20;

module_param(dump_pkt, ushort, 0600);
MODULE_PARM_DESC(dump_pkt,"Dump packet at magpie = 0(no) & 1(yes)");

module_param(dump_pkt_lim, ushort, 0600);
MODULE_PARM_DESC(dump_pkt_lim,"Dump packet limit amount in bytes");

#ifdef AH_OFFLOAD_CALDATA_FROM_HOST
#define WLAN_CALDATA_SIZE_PER_CARD          (16*1024)
#endif

#define TGT_STAGE1_FW_LOCATION	            (0xbd004000)
#define TGT_STAGE2_FW_LOCATION              (0x81400000)

//static a_uint32_t host_flash_mem_io_addr[2]= {0, 0}; /* save ioremap addresses for unmap when unloading */

extern const unsigned long waspRomInitFw[]; 
extern const unsigned long waspRomInitFwSize;

extern unsigned long vmlrd_lzma_entry;
extern const unsigned long vmlrd_lzma[];
extern const unsigned long vmlrd_lzmaSize;

/**
 * Prototypes
 */ 
a_status_t      fwd_send_next(fwd_softc_t *sc);
static void     fwd_start_upload(fwd_softc_t *sc);

#ifdef AH_OFFLOAD_CALDATA_FROM_HOST
/* calculate the image checksum and send it to the target for verification */
unsigned int calculate_image_cksum(unsigned int *image, unsigned int size)
{
        unsigned int i, checksum = 0;

        for (i = 0; i < size; i += 4, image++)
                checksum = checksum ^ *image;

	return checksum;
}

/* Read the caldata from the host perminent memory and validate the data 
 * this function assumes host perminent memory that contains caldata can be memory mapped.
 * validation of data depends on the validation method implemented. (could be checksum).
 */
static int read_and_verify_caldata(unsigned int address, unsigned int *buffer, unsigned int size)
{
   unsigned char *io_addr = NULL;

   io_addr = adf_os_ioremap(address, size);
   if(io_addr == NULL) {
       adf_os_print("can't map address 0x%x\n", (unsigned int)address);
       return -1;
   }

   /* read data from flash */
   adf_os_mem_copy(buffer, io_addr, size);

   return 0; 
}
#endif

/********************************************************************/
void 
fwd_timer_expire(void *arg)
{
  fwd_softc_t *sc = arg;
  hif_callback_t fwd_cb;

  sc->chunk_retries ++;
  adf_os_print("Retry ");

  if (sc->chunk_retries >= FWD_MAX_CHUNK_RETRIES) {
      adf_os_print("\nFWD:Failed ...uploaded %#x bytes\n", sc->offset);

        /* Free the mutex here also deregister fw here */
        fwd_cb.context   = NULL;
        fwd_cb.recv_buf  = NULL;
        fwd_cb.send_buf_done  = NULL;
        hif_register(sc->hif_handle, &fwd_cb);
        adf_os_mutex_release(&sc->mutex);
  }
  else
      fwd_send_next(sc);
}
  
#define fwd_is_first(_sc)   ((_sc)->offset == 0)
#define fwd_is_last(_sc)    (((_sc)->size - (_sc)->offset) <= FWD_MAX_CHUNK)

static a_uint32_t
fwd_chunk_len(fwd_softc_t *sc)
{
    a_uint32_t left, max_chunk = FWD_MAX_CHUNK;
    
    left     =   sc->size - sc->offset;
    return(adf_os_min(left, max_chunk));
}

a_status_t
fwd_send_next(fwd_softc_t *sc) 
{
    a_uint32_t len, alloclen; 
    adf_nbuf_t nbuf;
    fwd_cmd_t  *h;
    a_uint8_t  *pld;
    a_uint32_t  target_jmp_loc;

    len      =   fwd_chunk_len(sc);
    alloclen =   sizeof(fwd_cmd_t) + len;

    /* accomdate start address */
    if (fwd_is_first(sc)) {
        alloclen += 4;
    }

    /* accomdate execution address */
    if (fwd_is_last(sc)){
        alloclen += 4;
    }

    nbuf = adf_nbuf_alloc(NULL, alloclen + 20, 20, 0);
    if (!nbuf) {
        adf_os_print("FWD: packet allocation failed. \n");
        return A_STATUS_ENOMEM;
    }

    h            =  (fwd_cmd_t *)adf_nbuf_put_tail(nbuf, alloclen);        

    h->more_data =  adf_os_htons(!fwd_is_last(sc));
    h->len       =  adf_os_htons(len);
    h->offset    =  adf_os_htonl(sc->offset);

    pld          =  (a_uint8_t *)(h + 1);

    if (fwd_is_first(sc)) {
        *(a_uint32_t *)pld  =   adf_os_htonl(sc->target_upload_addr);
                       pld +=   4;
    }

    adf_os_mem_copy(pld, &sc->image[sc->offset], len);

    if(h->more_data == 0) {
        target_jmp_loc = adf_os_htonl(sc->target_image_exec_addr);
        adf_os_mem_copy(pld+len, (a_uint8_t *)&target_jmp_loc, 4);
    }

    hif_send(sc->hif_handle, sc->tx_pipe, nbuf);
    adf_os_timer_start(&sc->tmr, FWD_TIMEOUT_MSECS);

    return A_STATUS_OK;
}

hif_status_t
fwd_txdone(void *context, adf_nbuf_t nbuf)
{
  adf_nbuf_free(nbuf);  
  return HIF_STATUS_OK;
}

hif_status_t
fwd_recv(void *context, adf_nbuf_t nbuf, a_uint8_t epid)
{
    fwd_softc_t *sc = (fwd_softc_t *)context;
    hif_callback_t fwd_cb;
    a_uint8_t download_done = 0;
    a_uint8_t *pld;
    a_uint32_t plen, rsp, offset;
    fwd_rsp_t *h; 

    adf_nbuf_peek_header(nbuf, &pld, &plen);

    h       = (fwd_rsp_t *)pld;
    rsp     = adf_os_ntohl(h->rsp);
    offset  = adf_os_ntohl(h->offset);
    
    adf_os_timer_cancel(&sc->tmr);

    switch(rsp) {

    case FWD_RSP_ACK:
        if (offset == sc->offset) {
            // adf_os_printk("ACK for %#x\n", offset);
            adf_os_print(".");
                sc->offset += fwd_chunk_len(sc);
                fwd_send_next(sc);
        }
        else
        {
            download_done = 1;
            adf_os_print("\n ***** FWD: image load Error: ACK Received with Invalid offset, aborting FW Download ***** \n");
        }

        break;
            
    case FWD_RSP_SUCCESS:
        download_done = 1;
        adf_os_print("done!\n");
        break;

    case FWD_RSP_FAILED:
        if (sc->ntries < FWD_MAX_IMAGE_RETRIES) {
            fwd_start_upload(sc);
        }
        else {
            download_done = 1;
            adf_os_print("FWD: image load Error: Max retries exceeded\n");
        }
        break;
        
    default:
            adf_os_assert(0);
    }

    if(download_done) {
        /* Free the mutex here also deregister fw here */
        fwd_cb.context   = NULL;
        fwd_cb.recv_buf  = NULL;
        fwd_cb.send_buf_done  = NULL;
        hif_register(sc->hif_handle, &fwd_cb);
        adf_os_mutex_release(&sc->mutex);
    }

    adf_nbuf_free(nbuf);

    return A_STATUS_OK;
}


hif_status_t
fwd_device_removed(void *ctx)
{
  adf_os_mem_free(ctx);
  return HIF_STATUS_OK;
}

static void 
fwd_start_upload(fwd_softc_t *sc)
{
    sc->ntries ++;
    sc->offset  = 0;
    fwd_send_next(sc);
}

#if 0
void
fwd_parse_params(hif_gmac_params_t   *params)
{
    params->dump_pkt = dump_pkt ? adf_os_htons(1) : 0;
    
    if (dump_pkt)
        params->dump_pkt_lim = adf_os_htons(dump_pkt_lim);

    if (mac_addr)
        fwd_parse_str(mac_addr, params->mac_addr, ':', 6);
    else /* default */
        fwd_parse_str(default_mac_addr, params->mac_addr, ':', 6);

    /* Parse the chip_type  */
    if (chip_type == ACT_MAC)
        params->chip_type = MII0_CTRL_TYPE_MAC;
    else /* default */
        params->chip_type = MII0_CTRL_TYPE_PHY;
    
    /* Parse the link_speed */
    if (link_speed == LINK_SPEED_10)
        params->link_speed = MII0_CTRL_SPEED_10;
    else if (link_speed == LINK_SPEED_100)
        params->link_speed = MII0_CTRL_SPEED_100;
    else /* default */
        params->link_speed = MII0_CTRL_SPEED_1000;
    
    /* Parse the rgmii_delay */
    if (rgmii_delay == RGMII_DELAY_NO)
        params->rgmii_delay = adf_os_htons(MII0_CTRL_RGMII_DELAY_NO);
    else if (rgmii_delay == RGMII_DELAY_SMALL)
        params->rgmii_delay = adf_os_htons(MII0_CTRL_RGMII_DELAY_SMALL);
    else if (rgmii_delay == RGMII_DELAY_MEDIUM)
        params->rgmii_delay = adf_os_htons(MII0_CTRL_RGMII_DELAY_MED);
    else /* default */
        params->rgmii_delay = adf_os_htons(MII0_CTRL_RGMII_DELAY_HUGE);
}


int
fwd_atoi(char c)
{
    if ( c >= '0' && c <= '9' )
        return ( c - '0');
    else if ( c >= 'A' && c <= 'F' )
        return ( 10 + (c - 'A' ));
    else if ( c >= 'a' && c <= 'f' )
        return ( 10 + (c - 'a' ));
    else 
        return -1;
}
void
fwd_parse_str(char str[], char addr[], char sep, int len)
{
    int i, j;

    for (i = 0, j = 0; str[i] != '\0' && j < len ; i++) {

        if (str[i] == sep) {
            j++;
            continue;
        }
        if ( ((i - j) % 2) == 0 )
            addr[j] |= (fwd_atoi(str[i]) << 4);
        else
            addr[j] |= fwd_atoi(str[i]);
    }
    
}
#endif

#ifdef AH_OFFLOAD_CALDATA_FROM_HOST
a_status_t fwd_tgt_rf_calib_data_download(hif_handle_t hif, int card_num)
{
    fwd_softc_t    *sc;
    hif_callback_t fwd_cb;
    unsigned int *caldata_buf;
    unsigned int caldata_size;
    unsigned int caldata_address;

    if(card_num < 0 || card_num > 1 ) {
       adf_os_print("FWD: calib data for card_num %d is not supported\n", card_num);
       return-1;
    }

    sc = adf_os_mem_alloc(NULL, sizeof(fwd_softc_t));
    if (!sc) {
      adf_os_print("FWD: No memory for calib data fwd context\n");
      return -1;
    }
    adf_os_mem_set(sc, 0, sizeof(fwd_softc_t));

    caldata_size = WLAN_CALDATA_SIZE_PER_CARD;
    caldata_buf =  adf_os_mem_alloc(NULL, caldata_size); 
    if (!caldata_buf) {
       adf_os_mem_free(sc);
       adf_os_print(" Not able to allocate memory for caldata from flash\n");
       return -1;
    }
    adf_os_mem_set(caldata_buf, 0, caldata_size);

    caldata_address = AH_OFFLOAD_CALDATA_LOCATION_ON_HOST + (card_num * WLAN_CALDATA_SIZE_PER_CARD); 
    if(read_and_verify_caldata(caldata_address, caldata_buf, caldata_size) < 0) {
       adf_os_print(" Not able to read caldata from from flash\n");
       adf_os_mem_free(caldata_buf);
       adf_os_mem_free(sc);
       return -1;
    }

    /*last word is replaced with cksum. is checksum required? */
    caldata_buf[(caldata_size/sizeof(caldata_buf[0])) - 1] = 
	calculate_image_cksum(caldata_buf, caldata_size - sizeof(caldata_buf[0]));

    sc->hif_handle = hif;
    adf_os_init_mutex(&sc->mutex);
    adf_os_timer_init(NULL, &sc->tmr, fwd_timer_expire, sc);
    sc->rx_pipe = hif_get_ulpipe(HIF_CONTROL_PIPE);
    sc->tx_pipe = hif_get_dlpipe(HIF_CONTROL_PIPE);
    sc->image = (a_uint8_t*)caldata_buf;
    sc->size  = caldata_size;
    sc->target_image_exec_addr = 0;
    if(card_num == 0) {
       sc->target_upload_addr = AH_OFFLOAD_RADIOS_AHB_CALDATA_ADDR;
    }else if(card_num == 1) {
       sc->target_upload_addr = AH_OFFLOAD_RADIOS_PCI_CALDATA_ADDR;
    }

    fwd_cb.context  = sc;
    fwd_cb.recv_buf = fwd_recv;
    fwd_cb.send_buf_done  = fwd_txdone;

    sc->hif_handle = hif;
    hif_register(hif, &fwd_cb);

    adf_os_print("Downloading RF Calibration Data\n");

    adf_os_mutex_acquire(&sc->mutex);
    fwd_start_upload(sc);

    adf_os_print("Waiting for RF Calibration Data Download complete \n");
    adf_os_mutex_acquire(&sc->mutex);

    /* Download RF Calibration Data Completed. destroy the mutex after unlock*/
    adf_os_mutex_release(&sc->mutex);
    
    /* download completed. free memory*/
    adf_os_mem_free(caldata_buf);
    adf_os_mem_free(sc);

    adf_os_print("Downloaing RF Calibration Data Completed.\n");

    return HIF_STATUS_OK;
}
#endif

a_status_t fwd_tgt_firmware_download(hif_handle_t hif, a_uint32_t stage)
{
    fwd_softc_t    *sc;
    hif_callback_t fwd_cb;

    sc = adf_os_mem_alloc(NULL, sizeof(fwd_softc_t));
    if (!sc) {
      adf_os_print("FWD: No memory for tgt firmware fwd context\n");
      return -1;
    }
    adf_os_mem_set(sc, 0, sizeof(fwd_softc_t));
    sc->hif_handle = hif;
    adf_os_init_mutex(&sc->mutex);
    adf_os_timer_init(NULL, &sc->tmr, fwd_timer_expire, sc);
    sc->rx_pipe = hif_get_ulpipe(HIF_CONTROL_PIPE);
    sc->tx_pipe = hif_get_dlpipe(HIF_CONTROL_PIPE);

    switch(stage)
    {
        case 1:
            sc->image = (a_uint8_t*)waspRomInitFw;
             sc->size  =  waspRomInitFwSize;  
            sc->target_upload_addr = TGT_STAGE1_FW_LOCATION;
            sc->target_image_exec_addr = TGT_STAGE1_FW_LOCATION;
            break;

        case 2:
            sc->image =  (a_uint8_t*)vmlrd_lzma;
            sc->size  = vmlrd_lzmaSize;
            sc->target_upload_addr = TGT_STAGE2_FW_LOCATION;
            sc->target_image_exec_addr = TGT_STAGE2_FW_LOCATION;
            break ;

        default:
            adf_os_print("%s: Error: Unknown firmware stage...\n",__FUNCTION__);
            break;
    }
    adf_os_print("%s: received request to download satge %d firmware...\n",__FUNCTION__,stage);

    fwd_cb.context = sc;
    fwd_cb.recv_buf = fwd_recv;
    fwd_cb.send_buf_done = fwd_txdone;

    sc->hif_handle = hif;
    hif_register(hif, &fwd_cb);

    adf_os_print("Downloading target firmware image\t");

    adf_os_mutex_acquire(&sc->mutex);
    fwd_start_upload(sc);

    adf_os_print("Waiting for Download complete... \n");
    adf_os_mutex_acquire(&sc->mutex);

    /* Download Completed. destroy the mutex after unlock*/
    adf_os_mutex_release(&sc->mutex);
    
    /* download completed. free sc handle memory*/
    adf_os_mem_free(sc);

    adf_os_print("Downloading target firmware completed.\n");

    return HIF_STATUS_OK;
}
#endif

a_status_t fwd_firmware_download(hif_handle_t hif)
{
    a_status_t status = HIF_STATUS_OK;

#ifdef ROM_MODE
    /* Make Sure all images will be downloaded*/
    adf_os_print("\nStart Downloading all images\n");    

    /* Downlaod RF Calibration data images for all cards */
#ifdef AH_OFFLOAD_CALDATA_FROM_HOST
#ifdef AH_OFFLOAD_RADIOS_AHB
    status = fwd_tgt_rf_calib_data_download(hif, 0);
#endif
#ifdef AH_OFFLOAD_RADIOS_PCI
    status = fwd_tgt_rf_calib_data_download(hif, 1);
#endif
#endif

    /* 1st stage fw */
    status = fwd_tgt_firmware_download(hif,1);
    if(status == HIF_STATUS_OK) {    
        adf_os_mdelay(1000);
        /* 2nd stage fw */
        status = fwd_tgt_firmware_download(hif,2);
        if(status == HIF_STATUS_OK)
            goto fw_ok;
    }

    adf_os_print("\nFW download failed....\n");
    return status ;

fw_ok:
    adf_os_print("\nAll images completed.\n");    
#endif

    return status;    
}

#if 0
#ifdef MDIO_BOOT_LOAD 
unsigned int fw_compute_cksum(unsigned int *ptr, int len)
{
    unsigned int sum=0x0;
    int i=0;

    for (i=0; i<len; i++)
        sum += ~ptr[i];

    return sum;
}
#endif 
#endif

int
fwd_bootfirmware_download(void)
{
  adf_os_print("\n");
  
  return A_STATUS_OK;
}

int fwd_module_init(void)
{
    adf_os_print("FWD:loaded\n");
#ifdef ROM_MODE
    /* Parse load time parameters  */
    //fwd_parse_params(&fwd_params);
#endif
    adf_os_print("\n");

    return A_STATUS_OK;
}

void
fwd_module_exit(void) 
{
    adf_os_print("FWD:unloaded\n");
}

adf_os_export_symbol(fwd_firmware_download);
adf_os_export_symbol(fwd_bootfirmware_download);
adf_os_virt_module_name(fwd);
adf_os_virt_module_init(fwd_module_init);
adf_os_virt_module_exit(fwd_module_exit);




