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

#ifndef AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK
#include <linux/firmware.h>
#endif

#ifdef ROM_MODE
#define MDIO_BOOT_LOAD
#endif

/* to get more flexibility, we can have two different
   modes, either target can accept huge set of bytes, 
   that need to be copied from one address to other
   OR just simple memory writes, host can choose to
   do selective write 
*/
//#define DEBUG 
#ifdef ROM_MODE
#define MDIO_WRITE_MODE_BW_NOEXEC 0x1 /* { start_address, length, values } */
#define MDIO_WRITE_MODE_MW_NOEXEC 0x2 /* { start_address, value } */
#define MDIO_WRITE_MODE_BW_EXEC   0x4 /* { start_address, length, exec_address */
typedef struct mdio_bw_noexec {
    unsigned int start_address;
    unsigned int length;
    unsigned int exec_address;
    unsigned int checksum;
    int          fwd_state;
    unsigned int current_rd_ptr;
} mdio_bw_noexec_t;

#ifndef DEBUG
extern int mdio_boot_init(int unit);
extern int mdio_reg_read(int unit , int reg);
extern int mdio_reg_write(int unit , int reg, int val);
#else
int mdio_boot_init(int unit) { return 0; }
int mdio_reg_read (int unit, int reg) { return 0; }
int mdio_reg_write( int unit, int reg, int val) { return 0; }
#endif
unsigned char mdio_wait_for_lock(int unit);
extern void mdio_release_lock(int unit, uint16_t extra_flags);
extern int mdio_start_handshake(int unit );
int mdio_write_block(int unit, unsigned char *bytes, int len);

void fwd_parse_str(char str[], char addr[], char sep, int len);
void fwd_parse_params(hif_gmac_params_t   *params);

char default_mac_addr[] = "aa:aa:aa:aa:aa:aa"; 
/* char magpie_mac_addr[ETH_ALEN] = {0}; */

hif_gmac_params_t   fwd_params = {{0}}; 

#define LINK_SPEED_10           1
#define LINK_SPEED_100          2
#define LINK_SPEED_1000         3

#define RGMII_DELAY_NO          1
#define RGMII_DELAY_SMALL       2
#define RGMII_DELAY_MEDIUM      3
#define RGMII_DELAY_HUGE        4

#define ACT_MAC                 1
#define ACT_PHY                 2

char           *mac_addr = NULL;
char           *mii_type = "sgmii";
a_uint8_t       link_speed = 0;
a_uint8_t       chip_type  = 0;
a_uint16_t      rgmii_delay = 0;
a_uint16_t      dump_pkt = 0;
a_uint16_t      dump_pkt_lim = 20;


#define WASP_TARGET 1
#define HORNET_TARGET 2

a_uint32_t      mdio_boot_enable = 1;
a_uint32_t      target_type = WASP_TARGET;
a_uint8_t       mdio_mac = 0;
static char     *tgt_if = "eth0";

module_param(mdio_boot_enable, uint, 0);
MODULE_PARM_DESC(mdio_boot_enable, "If enabled, gmac driver will be downloaded over mdio first ");

module_param(mac_addr, charp, 0600);
MODULE_PARM_DESC(mac_addr,"magpie mac_addr = <11:22:33:44:55:66>");

module_param(mii_type, charp, 0600);
MODULE_PARM_DESC(mii_type, "Interface type used to connect target ");

module_param(chip_type, byte, 0600);
MODULE_PARM_DESC(chip_type,"magpie RGMII mode = 1 (act as MAC) or 2 (act as PHY)");

module_param(link_speed, byte, 0600);
MODULE_PARM_DESC(link_speed,"magpie RGMII speed = 1 (10 Mbps), 2 (100 Mbps) & 3 (1000 Mbps)");

module_param(rgmii_delay, ushort, 0600);
MODULE_PARM_DESC(rgmii_delay,"magpie RGMII delay = 1 (no), 2 (small), 3 (med) & 4 (huge)");

module_param(dump_pkt, ushort, 0600);
MODULE_PARM_DESC(dump_pkt,"Dump packet at magpie = 0(no) & 1(yes)");

module_param(dump_pkt_lim, ushort, 0600);
MODULE_PARM_DESC(dump_pkt_lim,"Dump packet limit amount in bytes");

module_param(mdio_mac, byte, 0600);
MODULE_PARM_DESC(mdio_mac, "MAC(mac0/mac1) Interface to which target's MDC/MDIO lines are connected");

module_param(tgt_if, charp, 0600);
MODULE_PARM_DESC(tgt_if, "Interface used to connect target ");

#ifdef AH_OFFLOAD_CALDATA_FROM_HOST
#define WLAN_CALDATA_SIZE_PER_CARD          (16*1024)
#endif

#define TGT_STAGE1_FW_LOCATION	            (0xbd004000)
#define TGT_RGMII_STAGE1_FW_LOCATION	    (0xbd003000)
#define TGT_STAGE2_FW_LOCATION              (0x81400000)

static a_uint32_t host_flash_mem_io_addr[2]= {0, 0}; /* save ioremap addresses for unmap when unloading */

#ifdef AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK
extern const unsigned long waspRomInitFw[]; 
extern const unsigned long waspRomInitFwSize;
extern unsigned long vmlrd_lzma_entry;
extern const unsigned long vmlrd_lzma[];
extern const unsigned long vmlrd_lzmaSize;
#else
unsigned long *firmware_data;
unsigned long firmware_Size;
const struct firmware *tgt_firmware;
#endif

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

/**
 * Prototypes
 */ 
a_status_t      fwd_send_next(fwd_softc_t *sc);
static void     fwd_start_upload(fwd_softc_t *sc);

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
        adf_os_mutex_release(NULL, &sc->mutex);
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

    nbuf = adf_nbuf_alloc(NULL, alloclen + 20, 20, 0, 0);
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
        adf_os_mutex_release(NULL, &sc->mutex);
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

    adf_os_mutex_acquire(NULL, &sc->mutex);
    fwd_start_upload(sc);

    adf_os_print("Waiting for RF Calibration Data Download complete \n");
    adf_os_mutex_acquire(NULL, &sc->mutex);

    /* Download RF Calibration Data Completed. destroy the mutex after unlock*/
    adf_os_mutex_release(NULL, &sc->mutex);
    
    /* download completed. free memory*/
    adf_os_mem_free(caldata_buf);
    adf_os_mem_free(sc);

    adf_os_print("Downloaing RF Calibration Data Completed.\n");

    return HIF_STATUS_OK;
}
#endif

#ifndef AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK
static a_status_t fwd_get_tgt_firmware(const char *fw_name)
{
    struct net_device *device = NULL;

    device = dev_get_by_name(&init_net, tgt_if);

    if (request_firmware(&tgt_firmware, fw_name, &device->dev)) {
         printk(KERN_ERR "\nQCA fwd: failed to load firmware file %s\n", fw_name);
         return A_STATUS_FAILED;
    }

    return A_STATUS_OK;
}

static a_status_t fwd_release_tgt_firmware(void)
{
    release_firmware(tgt_firmware);
    return A_STATUS_OK;
}
#endif

a_status_t fwd_tgt_firmware_download(hif_handle_t hif)
{
    fwd_softc_t    *sc;
    hif_callback_t fwd_cb;
    a_status_t status = A_STATUS_OK;

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

#ifdef AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK
    sc->image = (a_uint8_t*)vmlrd_lzma;
    sc->size  = vmlrd_lzmaSize;
#else
    status = fwd_get_tgt_firmware("ath_tgt_fw2.fw");
    if(status != A_STATUS_OK)
        goto error;
    sc->image = (a_uint8_t*)tgt_firmware->data;
    sc->size  = (unsigned long)tgt_firmware->size;
#endif

    sc->target_upload_addr = TGT_STAGE2_FW_LOCATION;
    sc->target_image_exec_addr = TGT_STAGE2_FW_LOCATION;

    fwd_cb.context = sc;
    fwd_cb.recv_buf = fwd_recv;
    fwd_cb.send_buf_done = fwd_txdone;

    sc->hif_handle = hif;
    hif_register(hif, &fwd_cb);

    adf_os_print("Downloading target firmware image\t");

    adf_os_mutex_acquire(NULL, &sc->mutex);
    fwd_start_upload(sc);

    adf_os_print("Waiting for Download complete... \n");
    adf_os_mutex_acquire(NULL, &sc->mutex);

#ifndef AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK
error:
    /* release target firmware memory */
    fwd_release_tgt_firmware();
#endif

    /* Download Completed. destroy the mutex after unlock*/
    adf_os_mutex_release(NULL, &sc->mutex);
    
    /* download completed. free sc handle memory*/
    adf_os_mem_free(sc);

    if(status == A_STATUS_OK)
       adf_os_print("Downloading target firmware completed.\n");

    return status;
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

    /* Finally the target firmware image*/
    status = fwd_tgt_firmware_download(hif);

    if(status == HIF_STATUS_OK)
        adf_os_print("\nAll images completed.\n");
#endif

    return status;    
}


#ifdef MDIO_BOOT_LOAD 
/* We have 8 mdio registers, 
    first register is used as locking register
    Remaining all will be used as I/O registers
    I/O registers offses ( 7 of them ): 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x11
*/
#define MDIO_OWN_TGT                0x01
#define MDIO_OWN_HST                0x02
#define MDIO_HOST_UP                0x4855 /* HU */
#define MDIO_HOST_DOWN              0x4844 /* HD */
#define MDIO_TARGET_UP              0x5455 /* TU */
#define MDIO_TARGET_DOWN            0x5444 /* TD */
#define MDIO_BLOCK_TRANSFER         0x4254 /* BT */
#define MDIO_SINGLEBYTE_TRANSFER    0x5354 /* ST */
#define MAX_MDIO_IO_LEN             14
/* on host pb44 we need to access by indexing with 2, on magpie it is 4 */
#define MDIO_REG_WIDTH              2   /* 2 bytes */

#define MDIO_REG_TO_OFFSET(__reg_number__)\
            (7 << 8)+(MDIO_REG_WIDTH*(__reg_number__))

typedef enum mdio_reg_set {
    mdio_lock_reg=0x0,
    mdio_reg_start=0x0, 
    mdio_reg_end=MDIO_REG_TO_OFFSET(7) 
} mdio_reg_set_t;

unsigned char
mdio_wait_for_lock(int unit)
{

        volatile int rddata;
        rddata = mdio_reg_read(unit, 
                    MDIO_REG_TO_OFFSET(mdio_lock_reg));
        
        while((rddata & 0x00ff) != MDIO_OWN_HST){
            rddata=mdio_reg_read(unit, 
                        MDIO_REG_TO_OFFSET(mdio_lock_reg));
            /* rddata1 = mdio_reg_read(1,  */
            /*             MDIO_REG_TO_OFFSET(mdio_lock_reg)); */
            // printk("received %x, %x, %d \n", rddata, rddata1, unit); 
        }
     //    printk("%s:%x\n", __func__, rddata); 
        return (rddata & 0xff00) >> 8;
}


/*
 * Extra flags can be used to convey the target the length of the 
 * bytes it is receiving. Host may not write all the 7 registers
 * The higher byte in lock register would tell, how many BYTES are
 * being exchanged with target at any time. 
 *  - '0' has no meaning,  so target may not read, instead try unlock immediate.
 *  - ODD length bytes, target would be reading from lower byte of last 
 *    register to read. 
 * 
 */
void 
mdio_release_lock(int unit, uint16_t extra_flags)
{

	volatile uint16_t val=MDIO_OWN_TGT | extra_flags;
	
    //printk("writiung value %x\n", val);
	mdio_reg_write(unit, 
        MDIO_REG_TO_OFFSET(mdio_lock_reg), extra_flags | MDIO_OWN_TGT);
	val = mdio_reg_read(unit,mdio_lock_reg);
}

/*
 * The current understanding is, target would be booting much faster than
 * host. So, target would own the lock register and places TU in 
 * mdio_reg_start+1, and never release lock until host is up. 
 * when host is up, reads register mdio_reg_reg_start, to see if TU is present
 * if TU is present, host writes HU in mdio_reg_start+4. 
 * When target sees HU in mdio_reg_start+4, releases lock
 */
int
mdio_start_handshake(int unit )
{
   /* wait until host sees Target UP, should see it immediate */
   printk("%s waiting for target to up unit %d\n", __func__, unit);
   (void)mdio_wait_for_lock(unit);
   return 0;
}
/*
 * - Total registers are available and 
 * - one is used for lock. 
 * - One block would be always 14 bytes OR 7 short words
 * - 
 */
int
mdio_write_block(int unit, unsigned char *bytes, int len)
{
    unsigned char i=0;
    uint16_t val=0;
    unsigned char next_write_reg = mdio_reg_start+1;

    if (len > MAX_MDIO_IO_LEN) return -1;
    (void)mdio_wait_for_lock(unit);
    for (i=0; i < len; i++) {
        if (!(i%2)) {
            val = bytes[i];
        } else {
            val = (val << 8) | bytes[i];
            /* write to the register at this location */
//            printk("%s-WR-%x-%4x\n", __func__, next_write_reg, val);
            mdio_reg_write(unit, 
                MDIO_REG_TO_OFFSET(next_write_reg), 
                val);
            next_write_reg+=1;
        }
    }
    if (len%2) {
        /* do one extra write for odd byte at end */
        mdio_reg_write(unit, 
            MDIO_REG_TO_OFFSET(next_write_reg), val);
    }
    adf_os_print(".");
    mdio_release_lock(unit, len<<8);
    return 0;
}

EXPORT_SYMBOL(mdio_start_handshake);
EXPORT_SYMBOL(mdio_write_block);
EXPORT_SYMBOL(mdio_wait_for_lock);
EXPORT_SYMBOL(mdio_release_lock);

unsigned int fw_compute_cksum(unsigned int *ptr, int len)
{
    unsigned int sum=0x0;
    int i=0;

    for (i=0; i<len; i++)
        sum += ~ptr[i];

    return sum;
}
#define MDIO_FWD_RESET 0x01
#define MDIO_FWD_GOOD  0x02
#define MDIO_FWD_START 0x04
a_status_t
mdio_block_send(int unit, void *img_ptr, unsigned int start_addr,
                unsigned int send_length, unsigned int exec_addr)
{
    mdio_bw_noexec_t  fw_bw_state;
    unsigned char *fwptr;

    fw_bw_state.start_address = start_addr ;
    fw_bw_state.length = (unsigned int)send_length; 
    fw_bw_state.exec_address = exec_addr;
    fw_bw_state.fwd_state = 0; 

    fw_bw_state.current_rd_ptr = 0;
    printk("mdio handshake is good \n");

    /* tell target, start address and length */  
    printk("%s exchanging block write parameters \n", __func__);
    mdio_write_block(unit, (unsigned char*)&fw_bw_state, 
         sizeof(fw_bw_state.start_address)+
         sizeof(fw_bw_state.length)+
         sizeof(fw_bw_state.exec_address));
    printk("\n%s - start_address 0x%08x length 0x%08x exec_address 0x%08x\n", 
                    __func__,fw_bw_state.start_address, 
                    fw_bw_state.length, fw_bw_state.exec_address);
    /* start firmware download */
    /* Note:
        firmware image is assumed to be multiples of 32 bits, which is size of integer */
    fw_bw_state.checksum = fw_compute_cksum((unsigned int*)img_ptr, fw_bw_state.length/sizeof(unsigned int));

    /* update the checksum to be calculated to target */
    mdio_write_block(unit, (unsigned char*)&fw_bw_state.checksum, sizeof(fw_bw_state.checksum) + 4);
    printk("Checksum for the file %x\n", fw_bw_state.checksum);

    fw_bw_state.current_rd_ptr=0;
    fw_bw_state.fwd_state = MDIO_FWD_RESET;

    while (fw_bw_state.fwd_state & MDIO_FWD_RESET) {
        fwptr=(unsigned char*)img_ptr;
                
        for (fw_bw_state.current_rd_ptr=0; 
            fw_bw_state.current_rd_ptr   < fw_bw_state.length;) {
                if ((fw_bw_state.length - fw_bw_state.current_rd_ptr ) > 14) {
                   mdio_write_block(unit, &fwptr[fw_bw_state.current_rd_ptr], 14);
                } else {
                   mdio_write_block(unit, &fwptr[fw_bw_state.current_rd_ptr],
                                      fw_bw_state.length-fw_bw_state.current_rd_ptr);
                   break;
                }
                fw_bw_state.current_rd_ptr += (2 * 7);
            }
        
		adf_os_print("done!\nSend  waiting for lock\n");
        /* after sending wait for lock and firware status */
        /* when last short block is written, lock is released by target without status
           so, take the lock and release it immediate for firmware to find the checksum
           when checksum is good, we get the correct status in next lock.
        */  
        mdio_wait_for_lock(unit); 
        mdio_release_lock(unit, 0);
        fw_bw_state.fwd_state = mdio_wait_for_lock(unit);
        if (fw_bw_state.fwd_state & MDIO_FWD_GOOD) {
            printk("MDIO FWD GOOD  - releasing the lock\n"); 
            mdio_release_lock(unit, MDIO_FWD_START << 8);
            break;
        } else {
            printk("MDIO FWD BAD - checksum error %x\n",fw_bw_state.fwd_state);
            fw_bw_state.fwd_state |= MDIO_FWD_RESET; /* in all other cases, continue loading again */
        }
        
    } 
    return A_STATUS_OK;
}

#endif 

int
fwd_bootfirmware_download(void)
{
  a_status_t status = A_STATUS_OK;
#ifdef ROM_MODE
  unsigned int tgt_stage1_fw_addr;
  void *fw_addr = NULL;
  a_uint32_t fw_size = 0;

#ifdef AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK
  fw_addr = (void*)waspRomInitFw;
  fw_size  = waspRomInitFwSize;
#else
  status = fwd_get_tgt_firmware("ath_tgt_fw1.fw");
  if(status != A_STATUS_OK)
      goto error;
  fw_addr = (void *)tgt_firmware->data;
  fw_size  = (a_uint32_t)tgt_firmware->size;
#endif

  if (mdio_boot_enable==1) {
      adf_os_print("FWD:loaded1 over MDIO MAC %d \n",mdio_mac);
      mdio_boot_init(mdio_mac);

      if (strncmp(mii_type,"rgmii", 5) == 0) {
          tgt_stage1_fw_addr = TGT_RGMII_STAGE1_FW_LOCATION;
      } else {
          tgt_stage1_fw_addr = TGT_STAGE1_FW_LOCATION;
      }

      adf_os_print("FWD:loaded1 \n");
      if (0 == mdio_start_handshake(mdio_mac)) {
        int start_time, end_time;
        start_time=jiffies;
        mdio_block_send(mdio_mac, fw_addr, tgt_stage1_fw_addr , 
                         fw_size, tgt_stage1_fw_addr);
        end_time=jiffies;
        printk("total transfer time in jiffies %d\n", adf_os_ticks_to_msecs(end_time - start_time));
      }    
      
      /* Pass the load the parameters */
      mdio_write_block(mdio_mac, (unsigned char*)&fwd_params, sizeof(struct hif_gmac_params));
      adf_os_print("\nHIF GMAC parametrs sent\n");
  }

#ifndef AH_OFFLOAD_TGT_FIRMWARE_STATIC_LINK
error:
    /* release target firmware memory */
    fwd_release_tgt_firmware();
#endif

#endif
  adf_os_print("\n");
  
  return status;
}

int fwd_module_init(void)
{
    adf_os_print("FWD:loaded\n");
#ifdef ROM_MODE
    /* Parse load time parameters  */
    fwd_parse_params(&fwd_params);
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




