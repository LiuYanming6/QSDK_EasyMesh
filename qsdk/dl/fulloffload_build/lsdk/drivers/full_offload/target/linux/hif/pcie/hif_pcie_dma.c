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


#include <linux/module.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/dmapool.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/vmalloc.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/unaligned.h>


#include<hif_pcie.h>

#define DESC_DMA_POOL	"pciep-desc-dma"

#define size_sw     (sizeof(struct zdma_swdesc) * num_desc)
#define size_hw     (sizeof(struct zsDmaDesc) * num_desc)


static void
pci_dma_init_ring(zdma_swdesc_t  *swdesc , a_uint32_t  num_desc)
{
    a_uint32_t  i = 0, j = 0;

    for (i = 0; i < num_desc; i++) {
        j = (i + 1) % num_desc;
        swdesc[i].descp->nextAddr = (struct zsDmaDesc *)swdesc[j].hwaddr;
    }
}

/** 
 * @brief 
 * 
 * @param osdev
 * @param sc
 * @param num_desc
 */
void
pci_dma_dealloc_swdesc(struct device *osdev, pci_dma_softc_t *sc, 
                            a_uint32_t  num_desc, struct dma_pool *pool)
{
    a_uint32_t i;
    zdma_swdesc_t      *swdesc  = NULL;
     
    swdesc = sc->sw_ring ;
    for (i=0 ; i < num_desc; i++) {
        dma_pool_free(pool, swdesc[i].descp, swdesc[i].hwaddr);
    }

    vfree(sc->sw_ring);
    sc->sw_ring = NULL;
}

/** 
 * @brief Allocate and initialize s/w and 
 *        h/w descriptors for one chain
 * 
 * @param osdev
 * @param sc
 * @param num_desc
 */
static void
pci_dma_alloc_swdesc(pci_dma_softc_t *sc, a_uint32_t  num_desc, struct dma_pool *pool)
{
    a_uint32_t          i       = 0;
    zdma_swdesc_t      *swdesc  = NULL;
    //struct zsDmaDesc   *hwdesc  = NULL; 
    //struct zsDmaDesc   *paddr  = NULL; 

    sc->sw_ring = (zdma_swdesc_t *)vmalloc( size_sw);
    hif_assert(sc->sw_ring);
    memset((void *)sc->sw_ring, 0, size_sw) ;

    swdesc = sc->sw_ring;

   /* init s/w desc */
    for (i = 0; i < num_desc; i++) {
	swdesc[i].descp = dma_pool_alloc(pool, GFP_KERNEL | GFP_DMA, &swdesc[i].hwaddr);
        hif_assert(swdesc[i].descp);
        memset((void *)swdesc[i].descp, 0, sizeof(struct zsDmaDesc));
    }

    sc->num_desc = num_desc;

    pci_dma_init_ring(swdesc, num_desc);
}



a_uint32_t
pci_dma_tail_addr(pci_dma_softc_t  *dma_q) 
{
    a_uint32_t tail = dma_q->tail;
    zdma_swdesc_t  *swdesc = &dma_q->sw_ring[tail];

    return (swdesc->hwaddr);
}



/** 
 * @brief Remove skb references in s/w desc
 * 
 * @param osdev
 * @param swdesc
 * 
 * @return 
 */
inline struct sk_buff *
pci_dma_unlink_buf(struct device *osdev, zdma_swdesc_t *swdesc, enum dma_data_direction dir)
{
    struct sk_buff *skb;
    
    hif_assert((dir == DMA_TO_DEVICE) || (dir == DMA_FROM_DEVICE));

    skb = swdesc->nbuf ;
    dma_unmap_single(osdev, swdesc->buf_addr, swdesc->buf_size, dir);

    swdesc->buf_addr = 0;
    swdesc->buf_size = 0;
    swdesc->nbuf  = NULL ;
    
    return skb ;
}



/** 
 * @brief Mark the H/W descriptor ready
 * 
 * @param swdesc
 * @param ctrl
 */
inline void
pci_zdma_mark_rdy(zdma_swdesc_t  *swdesc, a_uint16_t ctrl)
{
    struct zsDmaDesc  *hwdesc = swdesc->descp;

    hwdesc->dataAddr = (a_uint32_t)swdesc->buf_addr;
    hwdesc->dataSize = swdesc->buf_size;
    hwdesc->lastAddr = (struct zsDmaDesc  *)swdesc->hwaddr;
    hwdesc->ctrl     = ctrl;
    /* Should be the last operation */
    hwdesc->status   = ZM_OWN_BITS_HW;
}



/** 
 * @brief Add skb into s/w desc
 * 
 * @param osdev
 * @param swdesc
 * @param buf
 */
inline void
pci_dma_link_buf(struct device *osdev, zdma_swdesc_t *swdesc, 
                         struct sk_buff *buf, enum dma_data_direction dir)
{
    size_t len ;
    
    hif_assert((dir == DMA_TO_DEVICE) || (dir == DMA_FROM_DEVICE));
    
    len              = (buf->len ? buf->len : skb_tailroom(buf));

    swdesc->buf_addr = dma_map_single(osdev, buf->data, len, dir);
    swdesc->buf_size = len ;
    swdesc->nbuf     = buf;
}


/** 
 * @brief 
 * 
 * @param osdev
 * @param swdesc
 * @param size
 * 
 * @return 
 */
inline a_status_t
pci_dma_recv_refill(struct device *osdev, zdma_swdesc_t * swdesc,
                            a_uint32_t size)
{
    struct sk_buff *buf;

    buf = dev_alloc_skb(size);
    if (!buf)
        hif_assert(0);

    pci_dma_link_buf(osdev, swdesc, buf, DMA_FROM_DEVICE);

    pci_zdma_mark_rdy(swdesc, (ZM_FS_BIT | ZM_LS_BIT));

    return A_STATUS_OK;
}



/** 
 * @brief 
 * 
 * @param osdev
 * @param dma_q
 * @param buf
 * @param pos
 * 
 * @return 
 */
a_status_t
pci_dma_recv_init(struct device *osdev, pci_dma_softc_t  *dma_q,
                          struct sk_buff *buf, a_uint32_t  pos)
{
    zdma_swdesc_t  *swdesc;

    swdesc = &dma_q->sw_ring[pos];

    pci_dma_link_buf(osdev, swdesc, buf, DMA_FROM_DEVICE);

    pci_zdma_mark_rdy(swdesc, (ZM_FS_BIT | ZM_LS_BIT));

    return A_STATUS_OK;
}


/** 
 * @brief Deinit one TX dam ring
 * 
 * @param osdev
 * @param dma_q
 * @param num_desc
 * @param buf_size
 */
void pci_dma_deinit_tx(struct device *osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc,struct dma_pool *pool)
{
    zdma_swdesc_t  *swdesc;

    swdesc = dma_q->sw_ring;

    pci_dma_dealloc_swdesc(osdev, dma_q, num_desc, pool) ;
}

/** 
 * @brief Deinit one RX dam ring
 * 
 * @param osdev
 * @param dma_q
 * @param num_desc
 * @param buf_size
 */
void
pci_dma_deinit_rx(struct device *osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc, a_uint32_t buf_size, struct dma_pool *pool)
{
    a_uint32_t  i;
    zdma_swdesc_t  *swdesc;
    struct sk_buff *skb;

    swdesc = dma_q->sw_ring;

    for (i = 0; i < num_desc ; i++) {
        if(swdesc[i].nbuf)
        {
            skb = pci_dma_unlink_buf(osdev, &swdesc[i], DMA_FROM_DEVICE) ;
            dev_kfree_skb_any(skb);
        }
    }

    pci_dma_dealloc_swdesc(osdev, dma_q, num_desc, pool);
}

/** 
 * @brief 
 * 
 * @param osdev
 * @param dma_q
 * @param num_desc
 * @param buf_size
 */
void
pci_dma_init_rx(struct device * osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc, a_uint32_t buf_size, struct dma_pool *pool)
{
    a_uint32_t  i;
    a_uint32_t  offset;
    zdma_swdesc_t  *swdesc;
    struct sk_buff  *buf;

    pci_dma_alloc_swdesc(dma_q, num_desc, pool);

    swdesc = dma_q->sw_ring;

    for (i = 0; i < num_desc ; i++) {
        //buf = alloc_skb(buf_size + PCI_NBUF_ALIGNMENT - 1, GFP_KERNEL | GFP_DMA);
        buf = dev_alloc_skb(buf_size + PCI_NBUF_ALIGNMENT - 1);
        hif_assert(buf);
        offset = ((unsigned long) buf->data) % PCI_NBUF_ALIGNMENT;
        if(offset)
            skb_reserve(buf, PCI_NBUF_ALIGNMENT - offset);

        pci_dma_recv_init(osdev, dma_q, buf, i);
    }
}

/** 
 * @brief 
 * 
 * @param osdev
 * @param dma_q
 * @param num_desc
 */
void    
pci_dma_init_tx(struct device *osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc, struct dma_pool *pool)
{
    zdma_swdesc_t  *swdesc;
    a_uint32_t    i = 0;

    pci_dma_alloc_swdesc(dma_q, num_desc, pool);

    swdesc = dma_q->sw_ring;
    for (i = 0; i < num_desc; i++)
    {
      swdesc[i].nbuf = NULL ;
    }
}



/** 
 * @brief Cleanup Rx descriptors for all Rx chains.
 * 
 * @param 
 * 
 * @return 
 */
static void
__hif_pci_cleanup_rxdesc(pcie_softc_t *sc, a_uint32_t num_desc, struct dma_pool *pool)
{
    a_uint32_t i;

    for(i = 0 ; i < HIF_PCI_MAX_RXCHAINS ; i++)
    {
        pci_dma_deinit_rx(sc->dev, &sc->rxdma[i],
                          num_desc, MAX_NBUF_SIZE, pool);
        sc->rxdma[i].head = 0 ;
        sc->rxdma[i].tail = 0 ;
    }

}


/** 
 * @brief Allocate rx descriptors
 * 
 * @param sc
 * @param num_desc
 */
static void
__hif_pci_setup_rxdesc(pcie_softc_t  *sc, a_uint32_t num_desc, struct dma_pool *pool)
{
    a_uint32_t i ;

    for(i = 0 ; i < HIF_PCI_MAX_RXCHAINS ; i++)
    {
        sc->rxdma[i].head = 0 ;
        sc->rxdma[i].tail = 0 ;
        pci_dma_init_rx(sc->dev, &sc->rxdma[i] , num_desc,
                        MAX_NBUF_SIZE, pool);
    }

    return  ;
}


/** 
 * @brief Cleanup Tx descriptors for all Tx chains.
 * 
 * @param 
 * 
 * @return 
 */
static void
__hif_pci_cleanup_txdesc(pcie_softc_t *sc, a_uint32_t num_desc, struct dma_pool *pool)
{
    a_uint32_t i;

    for(i = 0 ; i < HIF_PCI_MAX_TXCHAINS ; i++)
    {
        pci_dma_deinit_tx(sc->dev, &sc->txdma[i], num_desc, pool);
        sc->txdma[i].head = 0;
        sc->txdma[i].tail = 0;
    }
}


/** 
 * @brief Allocate rx descriptors
 * 
 * @param sc
 * @param num_desc
 */
static void
__hif_pci_setup_txdesc(pcie_softc_t  *sc, a_uint32_t num_desc, struct dma_pool *pool)
{
    a_uint32_t i ;

    for(i = 0 ; i < HIF_PCI_MAX_TXCHAINS ; i++)
    {
        sc->txdma[i].head = 0;
        sc->txdma[i].tail = 0;
        pci_dma_init_tx(sc->dev, &sc->txdma[i], num_desc, pool);
    }

    return  ;
}



int
pci_dma_init(pcie_softc_t *sc)
{
    sc->desc_dma_pool = dma_pool_create(DESC_DMA_POOL, sc->dev, \
                        sizeof(struct zsDmaDesc), PCI_NBUF_ALIGNMENT, 0);

    if(!sc->desc_dma_pool)
        return -1;

    __hif_pci_setup_txdesc(sc, HIF_PCI_MAX_TX_DESC, sc->desc_dma_pool );
    __hif_pci_setup_rxdesc(sc, HIF_PCI_MAX_RX_DESC, sc->desc_dma_pool );

    return 0;
}


int
pci_dma_deinit(pcie_softc_t *sc)
{
    __hif_pci_cleanup_txdesc(sc, HIF_PCI_MAX_TX_DESC , sc->desc_dma_pool);
    __hif_pci_cleanup_rxdesc(sc, HIF_PCI_MAX_RX_DESC , sc->desc_dma_pool);

    dma_pool_destroy(sc->desc_dma_pool);

    return 0;

}



