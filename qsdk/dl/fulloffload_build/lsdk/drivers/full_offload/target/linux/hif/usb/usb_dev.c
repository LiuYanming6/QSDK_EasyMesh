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

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/smp_lock.h>
#include <linux/skbuff.h>
#include <linux/device.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
#include <linux/usb_ch9.h>
#include <linux/usb_gadget.h>
#else
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#endif
#ifndef BUILD_PARTNER_PLATFORM
#include <../arch/mips/include/asm/mach-ar7240/ar7240.h>
#endif

#include "hif_usb.h"


#define USB_DEVICE_CLASS        0xFF
#define USB_DEVICE_SUBCLASS     0xFF
#define USB_DEVICE_PROTOCOL     0xFF
#define USB_ID_VENDOR           0x0CF3
#ifndef BUILD_PARTNER_PLATFORM
#ifdef CONFIG_MACH_AR934x
#define USB_ID_PRODUCT          0x9340
#else
#define USB_ID_PRODUCT          0x9330
#endif
#else
#define USB_ID_PRODUCT          0x9340
#endif /* BUILD_PARTNER_PLATFORM */
#define USB_BCD_DEVICE          0x0108
#define USB_LANG_ID             0x0409
#define USB_MAX_POWER           0xFA
#define USB_PHY_BIAS            0x3

#define USB_MANUFACTURER        0x10
#define USB_PRODUCT             0x20
#define USB_SERIAL_NUMBER       0x30

struct usb_qualifier_descriptor qualifier_des;
struct usb_device_descriptor device_des;

struct total_cfg_descriptor total_cfg_des;
struct total_cfg_descriptor other_total_cfg_des;

struct usb_string_descriptor string_descriptor0 = {
    0x04,			/* bLength(8) */
    0x03,			/* bUnicodeType(8) */
    {__constant_cpu_to_le16(USB_LANG_ID)},	/* wLANGID(16): Language ID:1033 */
};

uint8_t read_from_otp=0;

struct usb_string_descriptor_otp manufacturer_str_des;
struct usb_string_descriptor_otp product_str_des;
struct usb_string_descriptor_otp serial_number_str_des;

#define DT_OTHER_SPEED_CONFIGURATION    (0x07)
/* 5.8.3 Bulk Transfer Packet Size Constratints */
#define BULK_FS_MAX_PKT_SIZE		 (64)
#define BULK_HS_MAX_PKT_SIZE	    (256)
#define cUSB_DIR_HOST_OUT       0
#define cUSB_DIR_HOST_IN        1
#define USB_EP_ATTR_CTRL        0
#define USB_EP_ATTR_ISOCH       1
#define USB_EP_ATTR_BULK        2
#define USB_EP_ATTR_INTRPT      3

usb_config_t usb_setting;

uint8_t manufacturer_string_des_lang0409[] =	//ATHEROS
{
    0x10, 0x03, 0x41, 0x00, 0x54, 0x00, 0x48, 0x00,
    0x45, 0x00, 0x52, 0x00, 0x4f, 0x00, 0x53, 0x00
};

uint8_t product_string_des_lang0409[] =	//USB2.0 WLAN
{
    0x18, 0x03, 0x55, 0x00, 0x53, 0x00, 0x42, 0x00,
    0x32, 0x00, 0x2E, 0x00, 0x30, 0x00, 0x20, 0x00,
    0x57, 0x00, 0x4C, 0x00, 0x41, 0x00, 0x4E, 0x00
};

uint8_t serial_number_string_des_lang0409[] =	//12345
{
    0x0C, 0x03, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00,
    0x34, 0x00, 0x35, 0x00
};

endpt_config_t endpt_settings[USB_ENDPT_NUM] = {
    /*  address   direction       type      */
    {1, cUSB_DIR_HOST_OUT, USB_EP_ATTR_BULK,256,0}, /*Data Rx tgt tx hst*/
    {2, cUSB_DIR_HOST_IN, USB_EP_ATTR_BULK,256,0}, /*Data Tx tgt rx hst */
#if (ATH_SUPPORT_AOW == 1)
    {3, cUSB_DIR_HOST_OUT, USB_EP_ATTR_ISOCH, 1024,1}, /*AoW Tx hst*/
#endif
#ifdef ROM_ENUM_DISABLED
    {3, cUSB_DIR_HOST_IN, USB_EP_ATTR_BULK,256,0}, /*Evt Tx tgt rx hst */
    {4, cUSB_DIR_HOST_OUT, USB_EP_ATTR_BULK,256,0},/*Cmd Rx tgt tx hst */ 
#else
    {4, cUSB_DIR_HOST_OUT, USB_EP_ATTR_INTRPT,1024,1},/*Cmd Rx tgt tx hst*/
    {4, cUSB_DIR_HOST_IN, USB_EP_ATTR_INTRPT,1024,1}, /*Evt Tx tgt rx hst */
#endif
};

void 
init_device_des()
{
    device_des.bLength = USB_DT_DEVICE_SIZE;
    device_des.bDescriptorType = USB_DT_DEVICE;
    device_des.bcdUSB = __constant_cpu_to_le16(0x0200);
    device_des.bDeviceClass = 0xFF;
    device_des.bDeviceSubClass = 0xFF;
    device_des.bDeviceProtocol = 0xFF;
    device_des.bMaxPacketSize0 = 0x40;
    device_des.idVendor = __constant_cpu_to_le16(usb_setting.idVendor);
    device_des.idProduct = __constant_cpu_to_le16(usb_setting.idProduct);
    device_des.bcdDevice = __constant_cpu_to_le16(USB_BCD_DEVICE);
    device_des.iManufacturer = usb_setting.iManufacturer;
    device_des.iProduct = usb_setting.iProduct;
    device_des.iSerialNumber = usb_setting.iSerialNumber;
    device_des.bNumConfigurations = 0x1;
}

void 
init_qualifier_des()
{
    qualifier_des.bLength = sizeof(struct usb_qualifier_descriptor);
    qualifier_des.bDescriptorType = USB_DT_DEVICE_QUALIFIER;
    qualifier_des.bcdUSB = __constant_cpu_to_le16(0x0200);
    qualifier_des.bDeviceClass = 0xFF;
    qualifier_des.bDeviceSubClass = 0xFF;
    qualifier_des.bDeviceProtocol = 0xFF;
    qualifier_des.bMaxPacketSize0 = 0x40;
    qualifier_des.bNumConfigurations = 1;
    qualifier_des.bRESERVED = 0;
}

void 
init_configuration_des()
{
    total_cfg_des.cfg_des.bLength = USB_DT_CONFIG_SIZE;
    total_cfg_des.cfg_des.bDescriptorType = USB_DT_CONFIG;

    total_cfg_des.cfg_des.wTotalLength = __constant_cpu_to_le16(
                                          sizeof(struct total_cfg_descriptor));
    total_cfg_des.cfg_des.bNumInterfaces = 0x01;
    total_cfg_des.cfg_des.bConfigurationValue = 0x01;
    total_cfg_des.cfg_des.iConfiguration = 0x00;
    total_cfg_des.cfg_des.bmAttributes = 0x80;
    total_cfg_des.cfg_des.bMaxPower = usb_setting.bMaxPower;

    total_cfg_des.if_des.bLength = USB_DT_INTERFACE_SIZE;
    total_cfg_des.if_des.bDescriptorType = USB_DT_INTERFACE;
    total_cfg_des.if_des.bInterfaceNumber = 0;
    total_cfg_des.if_des.bAlternateSetting = 0;
    total_cfg_des.if_des.bNumEndpoints = USB_ENDPT_NUM;
    total_cfg_des.if_des.bInterfaceClass = 0xFF;
    total_cfg_des.if_des.bInterfaceSubClass = 0;
    total_cfg_des.if_des.bInterfaceProtocol = 0;
    total_cfg_des.if_des.iInterface = 0;
}

void 
init_other_speed_cfg_des()
{
    int i;
    uint8_t *src_ptr, *dst_ptr;

    src_ptr = (uint8_t *) & (total_cfg_des);
    dst_ptr = (uint8_t *) & (other_total_cfg_des);

    for (i = 0; i < sizeof(struct total_cfg_descriptor); i++) {
        *dst_ptr = *src_ptr;
        dst_ptr++;
        src_ptr++;
    }

    other_total_cfg_des.cfg_des.bDescriptorType = DT_OTHER_SPEED_CONFIGURATION;
    for (i = 0; i < USB_ENDPT_NUM; i++) {

        other_total_cfg_des.endpt_des[i].wMaxPacketSize=__constant_cpu_to_le16(
                BULK_HS_MAX_PKT_SIZE);
    }
}

struct usb_endpoint_descriptor* 
get_endpt_des(int pipe_address, int pipe_direction)
{
    int index;
    struct usb_endpoint_descriptor *result = NULL;
    for(index = 0; index < USB_ENDPT_NUM; index++){
        if((endpt_settings[index].address == pipe_address)
                && (endpt_settings[index].direction == pipe_direction))
        {
            result = &total_cfg_des.endpt_des[index];
            break;
        }
    }
    return result;

}

void 
init_endpt_des()
{
    int i = 0;
    for (; i < USB_ENDPT_NUM; i++) {
        total_cfg_des.endpt_des[i].bLength = sizeof(
                                             struct usb_endpoint_descriptor);
        total_cfg_des.endpt_des[i].bDescriptorType = USB_DT_ENDPOINT;
        total_cfg_des.endpt_des[i].bEndpointAddress = (
                                               endpt_settings[i].address) |
                                               (endpt_settings[i].direction == 
                                               cUSB_DIR_HOST_OUT ? 0 : 0x80);
        total_cfg_des.endpt_des[i].bmAttributes = endpt_settings[i].type;
        total_cfg_des.endpt_des[i].wMaxPacketSize = __constant_cpu_to_le16(
                                                 endpt_settings[i].maxPktSize);
        total_cfg_des.endpt_des[i].bInterval = endpt_settings[i].interval;

    }
}


int 
get_configuration(const struct usb_ctrlrequest *ctrl, uint8_t *buf)
{
    int result = 0;

    //Work around for bug ExtraViewID 65785,set Endpt DES/Other Speed DES again
    //Becuase at this time the Speed query should be OK,no more speed detected 
    init_endpt_des();
    init_other_speed_cfg_des();

    result = sizeof(struct total_cfg_descriptor);
    total_cfg_des.cfg_des.wTotalLength = cpu_to_le16(result);
    memcpy(buf, &total_cfg_des, sizeof(struct total_cfg_descriptor));

    return result;
}

int 
get_string_descriptor(const struct usb_ctrlrequest *ctrl, uint8_t *buf, 
                      int read_from_otp)
{
    int result = -EINVAL;
    uint16_t value = le16_to_cpu(ctrl->wValue);
    if ((value & 0xFF) == 0) {
        result = sizeof(struct usb_string_descriptor);
        memcpy(buf, &string_descriptor0, sizeof(struct usb_string_descriptor));
    } 
    else if ((value & 0xFF) == usb_setting.iManufacturer) {
        if (read_from_otp == 0) {
            result = sizeof(manufacturer_string_des_lang0409);
            memcpy(buf, manufacturer_string_des_lang0409, 
                   sizeof(manufacturer_string_des_lang0409));			
        } else {
            result = manufacturer_str_des.bLength;
            memcpy(buf, &manufacturer_str_des, result);
        }
    }
    else if ((value & 0xFF) == usb_setting.iProduct) {
        if (read_from_otp == 0) {
            result = sizeof(product_string_des_lang0409);
            memcpy(buf, product_string_des_lang0409, 
                   sizeof(product_string_des_lang0409));

        } else{
            result = product_str_des.bLength;
            memcpy(buf, &product_str_des, result);

        }
    } 
    else if ((value & 0xFF) == usb_setting.iSerialNumber) {
        if (read_from_otp == 0) {
            result = sizeof (serial_number_string_des_lang0409);
            memcpy(buf, serial_number_string_des_lang0409, result);

        } 
        else {
            result = serial_number_str_des.bLength;
            memcpy(buf, &serial_number_str_des, result);

        }
    } 
    else{
        printk("->Could not deal!\n");
    }
    return result;
}

#ifndef BUILD_PARTNER_PLATFORM
#ifdef CONFIG_MACH_AR934x
#define WASP_OTP_BASE           AR7240_APB_BASE+0x00130000
#define otp_reg_read(x)         ar7240_reg_rd( (WASP_OTP_BASE + x) )
#else
#define HORNET_OTP_BASE         AR7240_APB_BASE+0x00100000
#define otp_reg_read(x) 	ar7240_reg_rd( (HORNET_OTP_BASE + x) )

#endif

static void mem_shift(uint32_t *dst, uint32_t shift, uint32_t len)
{
    uint8_t *pDst = (uint8_t *)dst;
    uint8_t *pSrc = (uint8_t *)(dst+shift);
    uint32_t i=0;

    for(i=0; i<len; i++)
        *pDst = *pSrc;
}

static uint32_t otp_read(uint32_t addr)
{
    uint32_t     rdata;
    uint32_t     i=0;

    rdata = otp_reg_read(OTP_MEM_ADDRESS + addr);

    do {
        if(i++>65536) goto ERR_DONE;

        rdata = otp_reg_read(OTP_STATUS0_ADDRESS);
    } while(OTP_STATUS0_EFUSE_ACCESS_BUSY_GET(rdata) ||
            !OTP_STATUS0_EFUSE_READ_DATA_VALID_GET(rdata));

    return (otp_reg_read(OTP_STATUS1_ADDRESS));
ERR_DONE:
    return 0xdeadbeef;
}

static uint32_t otp_copy(uint32_t *dst, uint32_t *src, long len)
{
    long size = len; // round to alignment
    unsigned long shift = 0;
    long i=0;
    unsigned long s;
    uint32_t rdata;

    shift = (uint32_t)src % 4;

    /* make sure the offset is aligned */
    s = (shift==0)?(uint32_t)src:(((uint32_t)src) & 0xfc);

    /* read data from otp in word size */
    for(i=0; i<size; i+=4) {
        rdata = otp_read(s+i);
        if (rdata == 0xdeadbeef)
            return 0xdeadbeef;
        else
            dst[i/4] = rdata;
    }

    /* move data to align 4 bytes */
    if( shift!=0 )
        mem_shift(dst, shift, len);

    return HIF_STATUS_OK;
}

static uint32_t otp_compute_cksum(uint32_t *ptr, int len)
{
    unsigned int sum=0x0;
    int i=0;

    for (i=0; i<(len/4); i++)
        sum = sum ^ ptr[i];

    return sum;
}

static uint8_t *otp_read_usb(void)
{
    uint8_t *pData = NULL;
    uint8_t off_tbl[4] = {0};
    int i=0;

    struct otp_usb_hdr img_hdr;

    if( 0xdeadbeef == otp_copy((uint32_t *)off_tbl, (uint32_t *)OTP_USB_TBL_OFFSET, 4) )
        goto ERR_DONE;

    /* Check if the offset table exist */
    for(i=1; i>-1; i--) {
        if(off_tbl[i]!=0xff && off_tbl[i]!=0x0) {
            /* Inspect the information header */
            printk("offset[%d]: %d\n", i, off_tbl[i]*2);

            if( 0xdeadbeef == otp_copy((uint32_t *)&img_hdr, (uint32_t *)(OTP_BASE+(off_tbl[i]*2)), sizeof(struct otp_usb_hdr)))
                goto ERR_DONE;

            if( img_hdr.len!=0 && img_hdr.len!=0xff ) {
                unsigned long sum = 0;

                pData = (uint8_t *)OTP_SRAM_BASE;
                //Copy Data include header
                if( 0xdeadbeef == otp_copy((uint32_t *)pData, (uint32_t *)(OTP_BASE+(off_tbl[i]*2)), img_hdr.len) ){
                    pData = NULL;
                    goto ERR_DONE;
                }

                //Calculate Checksum including header
                sum = otp_compute_cksum((uint32_t *)pData, img_hdr.len);
                if(sum) {
                    pData = NULL;
                    printk("checksum fail: %08x\n", (unsigned int)sum);
                }
                else
                {
                    //Return pointer to USB info starting address (not include header)
                    pData+=4;
                    break;
                }
            }
        }
    }

ERR_DONE:
    return pData;	
}
#endif /* BUILD_PARTNER_PLATFORM */

#define swap16(x) \
        ((((x) & 0xff00) >> 8) | \
         (((x) & 0x00ff) << 8) )

static int make_str_des(struct usb_string_descriptor_otp *str_des, uint32_t size,
                 uint32_t descriptorType, uint8_t *data)
{
    int i = 0;

    if (size > MAX_STR_SIZE / 2) {
        return HIF_STATUS_ERROR;
    }

    str_des->bLength = size * 2 + 2;	//size*2->unicode, then plus 1 bytes for size, 1 byte for descriptor type
    str_des->bDescriptorType = descriptorType;

    for (i = 0; i < size; i++) {
        str_des->wData[i] = (__le16) swap16(*data);
        data++;
    }

    return HIF_STATUS_OK;
}

static int read_otp_data(uint8_t *otp_ptr)
{
    uint32_t size;
    uint32_t index = 0;
    int ret = HIF_STATUS_ERROR;

    usb_setting.idVendor = (uint16_t) (otp_ptr[0] << 8) + ((uint16_t) otp_ptr[1]);
    usb_setting.idProduct = (uint16_t) (otp_ptr[2] << 8) + ((uint16_t) otp_ptr[3]);
    string_descriptor0.wData[0] = swap16((uint16_t) (otp_ptr[4] << 8) + ((uint16_t) otp_ptr[5]));
    usb_setting.bMaxPower = otp_ptr[6];

    index = 7;
    usb_setting.iManufacturer = otp_ptr[index];	//7
    size = otp_ptr[++index];	//8 size
    ret = make_str_des(&manufacturer_str_des, size, 0x3, &otp_ptr[index + 1]);
    if (ret == HIF_STATUS_ERROR)
        return HIF_STATUS_ERROR;

    index += size;
    usb_setting.iProduct = otp_ptr[++index];
    size = otp_ptr[++index];
    ret = make_str_des(&product_str_des, size, 0x3, &otp_ptr[index + 1]);
    if (ret == HIF_STATUS_ERROR)
        return HIF_STATUS_ERROR;

    index += size;
    if (otp_ptr[++index] != 0)	//Serial Number string may be Empty, so check this value
    {
        usb_setting.iSerialNumber = otp_ptr[index];
        size = otp_ptr[++index];
        ret = make_str_des(&serial_number_str_des, size, 0x3, &otp_ptr[index + 1]);
        if (ret == HIF_STATUS_ERROR)
            return HIF_STATUS_ERROR;
    } else {
        usb_setting.iSerialNumber = 0;
    }

    return HIF_STATUS_OK;
}

static void USB_set_default_value(void)
{
    usb_setting.idVendor = USB_ID_VENDOR;
    usb_setting.idProduct = USB_ID_PRODUCT;
    usb_setting.iManufacturer = USB_MANUFACTURER;
    usb_setting.iProduct = USB_PRODUCT;
    usb_setting.iSerialNumber = USB_SERIAL_NUMBER;
    usb_setting.bMaxPower = USB_MAX_POWER;
    usb_setting.bUSBPhyBias = USB_PHY_BIAS;
}
void 
hif_read_usb_config()
{
    uint8_t *custom_usb_info = NULL;
    int ret = HIF_STATUS_ERROR;

#ifndef BUILD_PARTNER_PLATFORM
    custom_usb_info = otp_read_usb();
#endif

    if (custom_usb_info) {
        ret = read_otp_data((uint8_t *) custom_usb_info);
        if (ret == HIF_STATUS_ERROR) {
            read_from_otp = 0;
            USB_set_default_value();
        } else {
            read_from_otp = 1;
        }
    } else {
        read_from_otp = 0;
        USB_set_default_value();
    }
	
    printk("VID=0x%x,PID=0x%x\n", usb_setting.idVendor,
            usb_setting.idProduct);
    printk("iManufacturer=0x%x,iProduct=0x%x,iSerialNumber=0x%x\n",
            usb_setting.iManufacturer, usb_setting.iProduct, usb_setting.iSerialNumber);
    printk("bMaxPower=0x%x\n", usb_setting.bMaxPower);
}
