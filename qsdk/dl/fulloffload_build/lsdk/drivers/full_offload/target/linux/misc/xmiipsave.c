/*
 * Copyright (c) 2013 Qualcomm Atheros, Inc. 
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


#include <linux/module.h>
#include <asm/byteorder.h>
#include <../arch/mips/include/asm/mach-ar7240/ar7240.h>


MODULE_LICENSE("Dual BSD/GPL");


void pciep_shutdown(void );
void usb_reset(void );
void i2s_reset(void );
void gmac0_reset(void );
void gmac1_reset(void );

int usb  = 0;
int pciep= 0;
int i2s   = 0;
int gmac0 =0;
int gmac1 =0;


module_param(usb,int,0);
module_param(pciep,int,0);
module_param(i2s,int,0);
module_param(gmac0,int,0);
module_param(gmac1,int,0);






int xmii_psaveinit(void)
{

    printk("%s \n", __FUNCTION__);

    if(pciep)
        pciep_shutdown();
    if(usb)
        usb_reset();
    if(i2s)
        i2s_reset();
    if(gmac0)
        gmac0_reset();
    if(gmac1)
        gmac1_reset();



    printk("%s done \n", __FUNCTION__);

    return 0;
}
void xmii_psaveexit(void){

    printk("%s \n", __FUNCTION__);

}
#define AUDIOPLL_PWD         0xb8116204
#define RST_RESET            0xb806001c 
#define RST_MISC2            0xb80600bc
#define pciep_phy_reg1       0xb8116d00
#define pciep_phy_reg3       0xb8116d08


#define BIT(_x)   ((unsigned int) 1 << (_x)) 


void pciep_shutdown(void )
{
    int mask;
    printk("%s \n",__FUNCTION__);

    /*mm 0xb80600bc bits 13 and 14 = 0;*/

    /*PCIE RC
      mm 0xb8116cc0 0x1061060e -- pcie_phy1 register for root complex -- PCIE_Testing.doc
      mm 0xb8116cc8 0x780c -- pcie_phy3 register for root complex -- PCIE_Testing.doc
      mm 0xb80f0008 bit 7 = 1;
      */

    /*PCIEEP
      mm 0xb8116d00 0x1061060e -- pcie_phy1 register for EP --WASPHostIF.doc
      mm 0xb8116d08 0x5c0c -- pcie_phy3 register for EP -- WaspHostIF.doc
      */


    printk("Initial values \n");
    printk(" RST_MISC2        %x \n",ar7240_reg_rd(RST_MISC2));
    printk(" pciep_phy_reg1   %x \n",ar7240_reg_rd(pciep_phy_reg1));
    printk(" pciep_phy_reg3   %x \n",ar7240_reg_rd(pciep_phy_reg3));

    ar7240_reg_rmw_clear(RST_MISC2,(BIT(14)));
    mask = (BIT(4) | BIT(5) | BIT(6) | BIT(13)  | BIT(14) | BIT(31));
    ar7240_reg_rmw_clear(pciep_phy_reg1,mask);

    ar7240_reg_rmw_set(pciep_phy_reg1, BIT(22));
    ar7240_reg_rmw_clear(pciep_phy_reg3, BIT(4));  

    printk(" Changed  values \n");
    printk(" RST_MISC2        %x \n",ar7240_reg_rd(RST_MISC2));
    printk(" pciep_phy_reg1   %x \n",ar7240_reg_rd(pciep_phy_reg1));
    printk(" pciep_phy_reg3   %x \n",ar7240_reg_rd(pciep_phy_reg3));


    printk("PCI endpoint Shutdown done\n");

}



void usb_reset(void )
{

    printk("%s \n", __FUNCTION__);
    printk("Initial  : AUDIOPLL_PWD %x \n\t  RST_RESET %x\n", ar7240_reg_rd(AUDIOPLL_PWD),ar7240_reg_rd(RST_RESET));
    ar7240_reg_rmw_clear(AUDIOPLL_PWD, BIT(16));

    ar7240_reg_rmw_set(RST_RESET, (BIT(4) |BIT(5) | BIT(10)));

    printk("after  : AUDIOPLL_PWD %x \n\t   RST_RESET %x\n", ar7240_reg_rd(AUDIOPLL_PWD),ar7240_reg_rd(RST_RESET));
}


void i2s_reset(void )
{

    printk("%s before RST_RESET %x \n", __FUNCTION__,ar7240_reg_rd(RST_RESET));
    ar7240_reg_rmw_set(RST_RESET, BIT(0));

    printk("%s After RST_RESET %x \n", __FUNCTION__,ar7240_reg_rd(RST_RESET));
}

void gmac0_reset(void )
{
    printk("GMAC0 External Phy reset \n");

    printk("%s before RST_RESET %x \n", __FUNCTION__,ar7240_reg_rd(RST_RESET));
    ar7240_reg_rmw_set(RST_RESET,(BIT(9)|BIT(22)));
    printk("%s after RST_RESET %x \n", __FUNCTION__,ar7240_reg_rd(RST_RESET));
}

void gmac1_reset(void )
{
    printk("GMAC1 +S27 internal reset \n");

    printk("%s before RST_RESET %x \n", __FUNCTION__,ar7240_reg_rd(RST_RESET));

    ar7240_reg_rmw_set(RST_RESET,(BIT(8)| BIT(13)|(BIT(23))));
    printk("%s after RST_RESET %x \n", __FUNCTION__,ar7240_reg_rd(RST_RESET));


}


module_init(xmii_psaveinit);
module_exit(xmii_psaveexit);
