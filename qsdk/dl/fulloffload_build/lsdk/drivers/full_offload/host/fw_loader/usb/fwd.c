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

#define WASP_TARGET 1
#define HORNET_TARGET 2
#define FIRMWARE_DOWNLOAD               0x30
#define FIRMWARE_DOWNLOAD_COMP          0x31

#define NUM_OF_WASP_FW_STAGE 2
#define NUM_OF_HORNET_FW_STAGE 2

a_uint32_t wasp_fw_target_addr[NUM_OF_WASP_FW_STAGE] = {0xbd004000, 0x80400000};


a_uint32_t hornet_fw_target_addr[NUM_OF_HORNET_FW_STAGE] = {0xbd006000, 
    0x80400000
};

extern const unsigned long waspRomInitFw[]; 
extern const unsigned long waspRomInitFwSize;

extern unsigned long vmlrd_lzma_entry;
extern const unsigned long vmlrd_lzma[];
extern const unsigned long vmlrd_lzmaSize;

/**
 * Prototypes
 */ 
a_status_t      fwd_send_next(fwd_softc_t *sc);

/********************************************************************/


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
    a_uint32_t len; 
    uint8_t* buf;
    a_int32_t status;
    a_uint32_t* src;
    a_uint32_t* dst;
    a_uint32_t  i, len32;


    len      =   fwd_chunk_len(sc);

    buf = kmalloc(len, GFP_KERNEL);

    if (!buf) {
        adf_os_print("FWD: packet allocation failed. \n");
        return A_STATUS_ENOMEM;
    }
    
    dst = (a_uint32_t*) buf;
    src = (a_uint32_t*) &sc->image[sc->offset];
    len32 = len/4;

    for(i=0; i<len32; i++)
    {
        dst[i] = adf_os_htonl(src[i]);
    }

    status = usb_download_image(sc->hif_handle,  FIRMWARE_DOWNLOAD, 
            sc->target_upload_addr[sc->current_stage]+ sc->offset, buf, len);

    kfree(buf);

    return status;
}


a_status_t fwd_firmware_download(hif_handle_t hif)
{
    fwd_softc_t    *sc;
    a_int32_t status = A_STATUS_OK, retval;
    a_uint32_t devid = usb_get_devid(hif);
    int i;

    sc = adf_os_mem_alloc(NULL, sizeof(fwd_softc_t));
    if (!sc) {
        adf_os_print("FWD: No memory for fwd context\n");
        return -1;
    }
    adf_os_mem_set(sc, 0, sizeof(fwd_softc_t));
    sc->hif_handle = hif;
    sc->target_type = devid;

    switch(devid)
    {
        case PRODUCT_AR9342:
            printk("Upload firmware for Wasp Target \n");
            sc->target_upload_addr = wasp_fw_target_addr;
            sc->target_exec_addr[0] = sc->target_upload_addr[0];
            sc->target_exec_addr[1] = sc->target_upload_addr[1];
            sc->total_stages = NUM_OF_WASP_FW_STAGE;
            sc->fw_images[0] = (unsigned char*)waspRomInitFw; 
            sc->fw_size[0] = waspRomInitFwSize;
            sc->fw_images[1] = (unsigned char*)vmlrd_lzma;
            sc->fw_size[1] = vmlrd_lzmaSize;
            break;

        case PRODUCT_AR9330:
            printk("Upload firmware for Hornet Target \n");
            sc->target_upload_addr = hornet_fw_target_addr;
            sc->target_exec_addr[0] = sc->target_upload_addr[0];
            sc->target_exec_addr[1] = sc->target_upload_addr[1];
            sc->total_stages = NUM_OF_HORNET_FW_STAGE;
            sc->fw_images[0] = (unsigned char*)waspRomInitFw; 
            sc->fw_size[0] = waspRomInitFwSize;
            sc->fw_images[1] = (unsigned char*)vmlrd_lzma;
            sc->fw_size[1] = vmlrd_lzmaSize;
            break;
        default:
            adf_os_print("fwd: unsupported devid: %x\n", devid);
            status = A_STATUS_ENOTSUPP;
            goto fail;
    }

    for(i =0 ; i<sc->total_stages; i++)
    {    
        int iter = 0;
        sc->offset  = 0;
        sc->current_stage    = i;
        printk("Firmware Upload Stage %d \n",i);
        sc->image = sc->fw_images[i];
        sc->size = sc->fw_size[i];
        printk("Stage %d  size =%d \n", i, sc->size);
        while(fwd_chunk_len(sc) != 0)
        {
            iter++;
            retval = fwd_send_next(sc);
            if(retval < 0)
            {
                printk("Error in uploading %d stage fw at offset %d \n",
                        sc->current_stage+1,sc->offset);
                status = retval;
                goto fail;
            }
            else
                sc->offset += retval;

        }
        printk("Successfully download %d stage firmware \n",sc->current_stage);
        retval = usb_download_image(sc->hif_handle,  
                        FIRMWARE_DOWNLOAD_COMP, 
                        sc->target_exec_addr[sc->current_stage], 
                        NULL, 0);
        printk("Send Download complete command for Stage %d \n",
                sc->current_stage);
    }

fail:
    adf_os_mem_free(sc);
    return status;
}

int fwd_bootfirmware_download(void)
{
    adf_os_print("Bootinit Completed...\n");
    return HIF_STATUS_OK;
}


    a_int32_t
fwd_module_init(void)
{
    adf_os_print("fwd_module_init : entry\n");
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



