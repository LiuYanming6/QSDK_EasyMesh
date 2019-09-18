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

#define ADF_DMA_SUPPORT
#include<adf_os_dma.h>

#include<hif_pcie.h>

static void
pci_dma_init_ring(zdma_swdesc_t  *swdesc , a_uint32_t  num_desc)
{
    a_uint32_t  i = 0, j = 0;

    for (i = 0; i < num_desc; i++) {
        j = (i + 1) % num_desc;
        swdesc[i].descp->nextAddr = (struct zsDmaDesc *)swdesc[j].hwaddr;
    }
}

#define size_sw     (sizeof(struct zdma_swdesc) * num_desc)
#define size_hw     (sizeof(struct zsDmaDesc) * num_desc)

/** 
 * @brief 
 * 
 * @param osdev
 * @param sc
 * @param num_desc
 */
void
pci_dma_dealloc_swdesc(adf_os_device_t osdev, pci_dma_softc_t *sc, 
                            a_uint32_t  num_desc)
{
    adf_os_dmamem_free(osdev, size_hw, PCI_DMA_MAPPING,
                              sc->hw_ring, sc->dmap);

    adf_os_mem_free(sc->sw_ring);
}

/** 
 * @brief Allocate and initialize s/w and 
 *        h/w descriptors for one chain
 * 
 * @param osdev
 * @param sc
 * @param num_desc
 */
void
pci_dma_alloc_swdesc(adf_os_device_t osdev, pci_dma_softc_t *sc,
                             a_uint32_t  num_desc)
{
    a_uint32_t          i       = 0;
    zdma_swdesc_t      *swdesc  = NULL;
    struct zsDmaDesc   *hwdesc  = NULL, 
                       *paddr   = NULL;

    sc->sw_ring = adf_os_mem_alloc(osdev, size_sw);
    adf_os_assert(sc->sw_ring);
    adf_os_mem_set((void *)sc->sw_ring, 0, size_sw) ;

    sc->hw_ring = adf_os_dmamem_alloc(osdev, size_hw, 
                                PCI_DMA_MAPPING, &sc->dmap);
    adf_os_assert(sc->hw_ring);
    adf_os_mem_set((void *)sc->hw_ring, 0, size_hw) ;

    swdesc = sc->sw_ring;
    hwdesc = sc->hw_ring;
    paddr  = (struct zsDmaDesc *) adf_os_dmamem_map2addr(sc->dmap) ;

    /* init s/w desc */
    for (i = 0; i < num_desc; i++) {
        swdesc[i].descp = &hwdesc[i];
        swdesc[i].hwaddr = (adf_os_dma_addr_t) paddr;
        paddr = ((struct zsDmaDesc *)paddr + 1);
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
 * @brief Add skb into s/w desc
 * 
 * @param osdev
 * @param swdesc
 * @param buf
 */
void
pci_dma_link_buf(adf_os_device_t osdev, zdma_swdesc_t *swdesc, 
                         adf_nbuf_t  buf)
{
    adf_os_dmamap_info_t  sg = {0};

    adf_nbuf_map(osdev, swdesc->nbuf_map, buf, ADF_OS_DMA_TO_DEVICE);

    /**
      * XXX: for TX gather we need to use multiple swdesc
     */
    adf_nbuf_dmamap_info(swdesc->nbuf_map, &sg);
    adf_os_assert(sg.nsegs == 1);

    swdesc->nbuf       = buf;
    swdesc->buf_addr   = (a_uint8_t *)sg.dma_segs[0].paddr;
    swdesc->buf_size   = sg.dma_segs[0].len;
}


/** 
 * @brief Remove skb references in s/w desc
 * 
 * @param osdev
 * @param swdesc
 * 
 * @return 
 */
adf_nbuf_t
pci_dma_unlink_buf(adf_os_device_t osdev, zdma_swdesc_t *swdesc)
{
    adf_nbuf_t  buf = swdesc->nbuf;

    adf_nbuf_unmap(osdev, swdesc->nbuf_map, ADF_OS_DMA_TO_DEVICE);

    swdesc->buf_addr = NULL;
    swdesc->buf_size = 0;
    swdesc->nbuf     = ADF_NBUF_NULL;

    return buf;
}


/** 
 * @brief Mark the H/W descriptor ready
 * 
 * @param swdesc
 * @param ctrl
 */
void
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
 * @brief 
 * 
 * @param osdev
 * @param swdesc
 * @param size
 * 
 * @return 
 */
a_status_t
pci_dma_recv_refill(adf_os_device_t   osdev, zdma_swdesc_t     * swdesc,
                            a_uint32_t   size)
{
    adf_nbuf_t          buf;

    buf = adf_nbuf_alloc(NULL, size, 0, PCI_NBUF_ALIGNMENT);
    if (!buf)
        adf_os_assert(0);

    pci_dma_link_buf(osdev, swdesc, buf);

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
pci_dma_recv_init(adf_os_device_t osdev, pci_dma_softc_t  *dma_q,
                          adf_nbuf_t  buf, a_uint32_t  pos)
{
    zdma_swdesc_t  *swdesc;

    swdesc = &dma_q->sw_ring[pos];

    pci_dma_link_buf(osdev, swdesc, buf);

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
void pci_dma_deinit_tx(adf_os_device_t osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc)
{

    zdma_swdesc_t  *swdesc;
    a_uint32_t    i = 0;

    swdesc = dma_q->sw_ring;

    for (i = 0; i < num_desc; i++)
    {
        adf_nbuf_dmamap_destroy(osdev, swdesc[i].nbuf_map);
        swdesc[i].nbuf = NULL ;
    }
    
    pci_dma_dealloc_swdesc(osdev, dma_q, num_desc) ;
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
pci_dma_deinit_rx(adf_os_device_t osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc, adf_os_size_t buf_size)
{
    a_uint32_t  i;
    zdma_swdesc_t  *swdesc;
    //adf_nbuf_t  buf;

    swdesc = dma_q->sw_ring;

    for (i = 0; i < num_desc ; i++) {
        adf_nbuf_free(swdesc[i].nbuf);
        adf_nbuf_dmamap_destroy( osdev, swdesc[i].nbuf_map);
    }

    pci_dma_dealloc_swdesc(osdev, dma_q, num_desc);
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
pci_dma_init_rx(adf_os_device_t osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc, adf_os_size_t buf_size)
{
    a_uint32_t  i;
    zdma_swdesc_t  *swdesc;
    adf_nbuf_t  buf;

    pci_dma_alloc_swdesc(osdev, dma_q, num_desc);

    swdesc = dma_q->sw_ring;

    for (i = 0; i < num_desc ; i++) {
        adf_nbuf_dmamap_create(osdev, &swdesc[i].nbuf_map);

        buf = adf_nbuf_alloc(NULL, buf_size, 0, PCI_NBUF_ALIGNMENT);
        adf_os_assert(buf);
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
pci_dma_init_tx(adf_os_device_t osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc)
{
    zdma_swdesc_t  *swdesc;
    a_uint32_t    i = 0;

    pci_dma_alloc_swdesc(osdev, dma_q, num_desc);

    swdesc = dma_q->sw_ring;

    for (i = 0; i < num_desc; i++)
    {
      adf_nbuf_dmamap_create(osdev, &swdesc[i].nbuf_map);
      swdesc[i].nbuf = NULL ;
    }
}

