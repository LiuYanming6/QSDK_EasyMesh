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

#include <adf_net.h>
#include <adf_os_util.h>
#include <adf_os_lock.h>
#include <adf_os_irq.h>
#include <adf_os_io.h>
#include <adf_os_pci.h>
#include <adf_os_types.h>
#include <adf_os_module.h>
#include <adf_os_mem.h>
#include <adf_os_timer.h>
#include <adf_os_time.h>
#include <adf_os_util.h>
#include <adf_os_defer.h>
#include <adf_os_lock.h>

#include<hif_api.h>
#include<hif_pcie.h>

/*
 * Data types
 */

/** 
 * @brief pcie HIF context definition
 *        for host
 */
typedef struct pcie_softc {
    hif_os_callback_t   oscb ;
    hif_callback_t      cb ;
    adf_os_device_t     osdev ;    /**< Device handle */
    adf_os_handle_t     os_hdl;    /**< Other handles*/
    pci_dma_softc_t rxdma[HIF_PCI_MAX_RXCHAINS];
    pci_dma_softc_t txdma[HIF_PCI_MAX_TXCHAINS];
    adf_os_spinlock_t   lock_irq;       /**< Intr. contention*/
    adf_os_bh_t         hifwork ;       /**< Deffered processing */
    a_uint32_t          status_intr;    /**< DMA intr status */
    a_uint32_t          tgt_reset_ok;   /**< Target device detect/reset ok */

    void *context;

} pcie_softc_t ;


/*
 * Data definitions
 */
//static a_uint32_t  hif_pci_intm   = 1; /* Default RX Mitigation turned on*/
static a_uint32_t  hif_pci_intm   = 0;

/**
  * RX Interrupt Mitigation mask
  */
static a_uint32_t     dma_intm_mask = (   __DMA_INTR_RX0_DONE |
                                          __DMA_INTR_RX0_END  |
                                          __DMA_INTR_TX0_DONE | 
                                          __DMA_INTR_TX1_DONE |
                                          __DMA_INTR_TX2_DONE |
                                          __DMA_INTR_TX3_DONE );

static a_uint32_t dma_intr_mask  = ( __DMA_INTR_RX0_DONE | 
                                     __DMA_INTR_RX1_DONE | 
                                     __DMA_INTR_TX0_DONE | 
                                     __DMA_INTR_TX1_DONE | 
                                     __DMA_INTR_TX2_DONE |
                                     __DMA_INTR_TX3_DONE );


/** 
 * @brief global pcie HIF context
 */
//static pcie_softc_t pciesc ;
static pcie_softc_t *pciesc ;


/* 
 * Constants 
 */
static const a_uint32_t     host_intr_mask =    __HOSTIF_INTR_DMA;

static const a_uint32_t     host_enb_mask  = (  __HOSTIF_INTR_TGT_DMA_RST |
                                                __HOSTIF_INTR_DMA );

static const a_uint32_t     def_ahb_mode   = (  __AHB_MODE_CUST_BURST       | 
                                                __AHB_MODE_PAGE_SIZE_4K     |
                                                __AHB_MODE_READ_PRE_FETCH   |
                                                __AHB_MODE_WRITE_BUFFER );



/*
 * Function Prototypes 
 */
static adf_drv_handle_t 
hif_pci_attach( adf_os_pci_data_t *pdata, adf_os_device_t );

/** 
 * @brief 
 * 
 * @param hdl
 */
static void hif_pci_detach(adf_drv_handle_t hdl);
static void hif_pci_suspend(adf_drv_handle_t hdl, adf_os_pm_t pm);
static void hif_pci_resume(adf_drv_handle_t hdl);

static inline a_uint32_t __pipe_to_txengine(a_uint32_t pipe);
//static a_uint32_t __rxeng_to_pipe(__dma_eng_t  eng_no) ;

static void __hif_pci_setup_rxdesc(pcie_softc_t *sc, a_uint32_t num_desc);
static void __hif_pci_cleanup_rxdesc(pcie_softc_t *sc, a_uint32_t num_desc);
static void __hif_pci_setup_txdesc(pcie_softc_t *sc, a_uint32_t num_desc);
static void __hif_pci_cleanup_txdesc(pcie_softc_t *sc, a_uint32_t num_desc);
static void __hif_pci_setup_rxeng(pcie_softc_t  *sc) ;
static void __hif_pci_cleanup_rxeng(pcie_softc_t  *sc) ;
static void __hif_pci_setup_txeng(pcie_softc_t  *sc) ;
static void __hif_pci_cleanup_txeng(pcie_softc_t  *sc) ;
void __hif_pci_reset(pcie_softc_t   *sc) ;
void __hif_pci_tgt_reset(pcie_softc_t   *sc);

void __hif_pci_dma_pull_reset(pcie_softc_t   *sc);
void __hif_pci_dma_put_reset(pcie_softc_t   *sc);
void __hif_pci_tgt_reset_done(pcie_softc_t *sc);


static a_status_t __hif_pcie_setup_intr(pcie_softc_t *sc);
adf_os_irq_resp_t hif_pcie_intr(adf_drv_handle_t hdl);

#ifdef ROM_MODE 
static void rombootfn(void *);
#endif

/*
 * Debugging stuff
 */

/** 
 * @brief Set this to nonzero
 *        to enable debug prints
 */
a_uint32_t hif_trace_mask =0 ;
adf_os_declare_param(hif_trace_mask, ADF_OS_PARAM_TYPE_INT32);


/*
 * Function definitions
 */

#ifdef ROM_MODE
static void rombootfn(void *cxt)
{
    pcie_softc_t *sc = (pcie_softc_t *)cxt;
    hif_os_callback_t *oscb = &sc->oscb;
    //a_uint32_t status ;

    if(sc->oscb.device_bootinit) {
            sc->oscb.device_bootinit();
    }

    oscb->device_detect((hif_handle_t)sc);

    /* All fw has been downloaded at this point. */

    __hif_pci_cleanup_txeng(sc);
    __hif_pci_cleanup_rxeng(sc);
    __hif_pci_cleanup_txdesc(sc, HIF_PCI_MAX_TX_DESC);
    __hif_pci_cleanup_rxdesc(sc, HIF_PCI_MAX_RX_DESC);

    printk("hif_pcie: wait 8 sec for targt fw boot...\n");
    adf_os_mdelay(8000);

    /* init tx rx descriptors */
    __hif_pci_setup_rxdesc(sc, HIF_PCI_MAX_RX_DESC);
    __hif_pci_setup_txdesc(sc, HIF_PCI_MAX_TX_DESC);

   __hif_pci_tgt_reset(sc);

    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_AHB_MODE, def_ahb_mode);

}

#endif //ROM_MODE

/** 
 * @brief Convert pipe number to tx dma engine queue.
 * 
 * @param pipe
 * 
 * @return 
 */
static inline 
a_uint32_t __pipe_to_txengine(a_uint32_t pipe)
{
    a_uint32_t eng = tx_eng_num(__DMA_ENGINE_TX0);

    switch(pipe)
    {
        case HIF_CONTROL_PIPE:
             eng = tx_eng_num(__DMA_ENGINE_TX0);
             break ;

        case HIF_COMMAND_PIPE:
             eng = tx_eng_num(__DMA_ENGINE_TX0);
             break ;

        case HIF_DATA_LP_PIPE:
             eng = tx_eng_num(__DMA_ENGINE_TX3);
             break ;

        case HIF_WSUPP_PIPE:
             eng = tx_eng_num(__DMA_ENGINE_TX3);
             break ;

        default:
             eng = tx_eng_num(__DMA_ENGINE_TX3);
             break ;
    }

    return eng;
}


/** 
 * @brief convert rx engine number to pipe id
 * 
 * @param eng_no
 * 
 * @return 
 */
#if 0
static a_uint32_t __rxeng_to_pipe(__dma_eng_t  eng_no)
{
    return  0 ;
}
#endif


/** 
 * @brief Cleanup Rx descriptors for all Rx chains.
 * 
 * @param 
 * 
 * @return 
 */
static void 
__hif_pci_cleanup_rxdesc(pcie_softc_t *sc, a_uint32_t num_desc)
{
    a_uint32_t i;

    for(i = 0 ; i < HIF_PCI_MAX_RXCHAINS ; i++)
    {
        pci_dma_deinit_rx(sc->osdev, &sc->rxdma[i], 
                          num_desc, MAX_NBUF_SIZE);
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
__hif_pci_setup_rxdesc(pcie_softc_t  *sc, a_uint32_t num_desc)
{
    a_uint32_t i ;

    for(i = 0 ; i < HIF_PCI_MAX_RXCHAINS ; i++)
    {
        sc->rxdma[i].head = 0 ;
        sc->rxdma[i].tail = 0 ;
        pci_dma_init_rx(sc->osdev, &sc->rxdma[i] , HIF_PCI_MAX_RX_DESC, 
                        MAX_NBUF_SIZE);
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
__hif_pci_cleanup_txdesc(pcie_softc_t *sc, a_uint32_t num_desc)
{
    a_uint32_t i;

    for(i = 0 ; i < HIF_PCI_MAX_TXCHAINS ; i++)
    {
        pci_dma_deinit_tx(sc->osdev, &sc->txdma[i], num_desc);
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
__hif_pci_setup_txdesc(pcie_softc_t  *sc, a_uint32_t num_desc)
{
    a_uint32_t i ;

    for(i = 0 ; i < HIF_PCI_MAX_TXCHAINS ; i++)
    {
        sc->txdma[i].head = 0;
        sc->txdma[i].tail = 0;
        pci_dma_init_tx(sc->osdev, &sc->txdma[i], HIF_PCI_MAX_TX_DESC);
    }

    return  ;
}



/** 
 * @brief Rx engine cleanup for all Rx chains.
 * 
 * @param sc
 */
static void
__hif_pci_cleanup_rxeng(pcie_softc_t  *sc)
{
     a_uint32_t  i;

    /* Stop rx dma engines */
    for (i = 0; i < HIF_PCI_MAX_RXCHAINS; i++) {
        adf_os_reg_write32(sc->osdev, reg_offset_rxdma(i,RX_DMA_START), 0);
    }
}



/** 
 * @brief Init rx chain registers
 * 
 * @param sc
 */
static void 
__hif_pci_setup_rxeng(pcie_softc_t  *sc) 
{
    a_uint32_t  i = 0, paddr;

    for (i = 0; i < HIF_PCI_MAX_RXCHAINS; i++) { 
        paddr = pci_dma_tail_addr(&sc->rxdma[i]);

        adf_os_reg_write32(sc->osdev, \
                reg_offset_rxdma(i,RX_DESC_START_ADDR) , paddr);

        adf_os_reg_write32(sc->osdev, \
                reg_offset_rxdma(i,RX_BURST_SIZE) , DMA_BURST_8W);
                                                  
#if defined (ADF_BIG_ENDIAN_MACHINE)
        adf_os_reg_write32(sc->osdev, reg_offset_rxdma(i,RX_DATA_SWAP) , 
                            DMA_DESC_SWAP_ON);
#endif
        adf_os_reg_write32(sc->osdev, reg_offset_rxdma(i,RX_DMA_START), 0x01);
    }

    return ;
}



/** 
 * @brief Tx engine cleanup for all Tx chains.
 * 
 * @param sc
 */
static void
__hif_pci_cleanup_txeng(pcie_softc_t  *sc)
{
    a_uint32_t  i ;
    
    /* Stop tx dma engines */
    for (i = 0; i < HIF_PCI_MAX_TXCHAINS; i++) {
        adf_os_reg_write32(sc->osdev, reg_offset_txdma(i,TX_DMA_START), 0);
    }
}


/** 
 * @brief Init tx chain registers
 * 
 * @param sc
 */
static void 
__hif_pci_setup_txeng(pcie_softc_t  *sc) 
{
    a_uint32_t  i , paddr;

    for (i = 0; i < HIF_PCI_MAX_TXCHAINS ; i++) {
        paddr = pci_dma_tail_addr(&sc->txdma[i]);

        adf_os_reg_write32(sc->osdev, \
                reg_offset_txdma(i,TX_DESC_START_ADDR) , paddr);

        adf_os_reg_write32(sc->osdev, \
                reg_offset_txdma(i,TX_BURST_SIZE) , DMA_BURST_16W);

        /**
          * Limit the TX interrupt to 16 packets or wait for 0xfff x 32
          * Cycles, which ever happens earlier
         */
//        if (i == 1)
//            adf_os_reg_write32(sc->osdev, reg_offset_txdma(i,TX_INTR_LIM) ,
//                    DMA_MAX_INTR_LIM);

#if defined (ADF_BIG_ENDIAN_MACHINE)
        adf_os_reg_write32(sc->osdev, reg_offset_txdma(i,TX_DATA_SWAP) , 
                               DMA_DESC_SWAP_ON);
#endif
    }

    return ;
}

  
/*
 * HIF api implementation
 */
 
/** 
 * @brief 
 * 
 * @param oscb
 * 
 * @return 
 */
hif_status_t
hif_init(hif_os_callback_t *oscb)
{
    pcie_softc_t *sc = pciesc ;

    if(!sc->tgt_reset_ok)
    {
        printk("hif_pcie: hif_init() called before target detect/reset. \n");
        return HIF_STATUS_ERROR;
    }

    memcpy(&sc->oscb, oscb, sizeof(hif_os_callback_t));

#ifdef ROM_MODE
    rombootfn((void *)sc);
#else
    oscb->device_detect((hif_handle_t)sc);
#endif
    return HIF_STATUS_OK;
}



/** 
 * @brief 
 * 
 * @param hif_handle
 * 
 * @return 
 */
hif_status_t
hif_cleanup(hif_handle_t  hif_handle)
{
    pcie_softc_t *sc = pciesc ;

    __hif_pci_cleanup_txeng(sc);
    __hif_pci_cleanup_rxeng(sc);

    /* Reset host DMA. Target should detect 
     * this reset and trigger full chip reset.
     */
    __hif_pci_dma_put_reset(sc);
    /* Dealy for target to detect reset */
    adf_os_mdelay(1000);
    __hif_pci_dma_pull_reset(sc);

    return HIF_STATUS_OK;
}

/** 
 * @brief 
 * 
 * @param hif_handle
 * 
 * @return 
 */
hif_status_t
hif_start(hif_handle_t hif_handle)
{
    return HIF_STATUS_OK;
}

/** 
 * @brief 
 * 
 * @param hif_handle
 * @param ctx
 */
void
hif_claim_device(hif_handle_t   hif_handle, void *ctx)
{
    pcie_softc_t *sc = pciesc ;

    sc->context = ctx ;

    return;
}

/** 
 * @brief 
 * 
 * @param hif_handle
 * @param cb
 */
void
hif_register(hif_handle_t hif_handle , hif_callback_t *cb)
{
    pcie_softc_t *sc = pciesc ;

    memcpy(&sc->cb, cb, sizeof(hif_callback_t));

    return ;
}


/** 
 * @brief 
 * 
 * @param hif_handle
 * @param pipeid
 * @param skb
 * 
 * @return 
 */
hif_status_t
hif_send(hif_handle_t hif_handle ,a_uint32_t pipeid , struct sk_buff * skb ) 
{
    a_uint32_t chain ;
    a_uint32_t head ,tail ;
    pcie_softc_t *sc ;
    zdma_swdesc_t *swdesc;
    pci_dma_softc_t * dma_q ;

    //sc = pciesc ;
    sc = (pcie_softc_t *)hif_handle;

    chain = __pipe_to_txengine(pipeid) ;

    dma_q = &sc->txdma[chain];

    tail = dma_q->tail;

    /* lock the queue; disable BH */
    adf_os_spin_lock_bh(&sc->lock_irq);

    head = dma_q->head;

    hif_trace("chain - %d; head - %d ; tail - %d ; "\
            "pkt_len - %d",chain,head, tail, skb->len);

    if(ring_tx_full(head, tail))
    {
        adf_os_spin_unlock_bh(&sc->lock_irq);
        return HIF_STATUS_ERROR ;
    }

    dma_q->head = ring_tx_incr(head);

    /* unlock the queue */
    adf_os_spin_unlock_bh(&sc->lock_irq);

    swdesc = &dma_q->sw_ring[head];

    adf_os_assert(swdesc->nbuf == ADF_NBUF_NULL);
    
    pci_dma_link_buf(sc->osdev, swdesc, skb);

    /* mark desc ready */
    pci_zdma_mark_rdy(swdesc, (ZM_FS_BIT | ZM_LS_BIT));

    /* write into register and start tx chain */
    adf_os_reg_write32(sc->osdev, reg_offset_txdma(chain, TX_DMA_START), 
                                   __DMA_REG_SET_TXCTRL);

    return HIF_STATUS_OK ;
}


/** 
 * @brief 
 * 
 * @param hif_handle
 * 
 * @return 
 */
a_uint32_t
hif_get_reserveheadroom(hif_handle_t hif_handle)
{
    return 0;
}

/** 
 * @brief 
 * 
 * @param type
 * 
 * @return 
 */
a_uint32_t
hif_get_ulpipe(hif_pipetype_t type)
{
    return (a_uint32_t)type ;
}

/** 
 * @brief 
 * 
 * @param type
 * 
 * @return 
 */
a_uint32_t
hif_get_dlpipe(hif_pipetype_t type)
{
    return 0;
}

/********************************
  Reset 
 ********************************/


/**
  * @brief Target reset over install the DMA interrupts and start
  *        the DMA Engines
  * 
  * @param sc
  */
void
__hif_pci_tgt_reset_done(pcie_softc_t *sc)
{
    adf_os_reg_write32(sc->osdev, __DMA_REG_IMR,  0x0);

    if (hif_pci_intm)
        adf_os_reg_write32(sc->osdev, __DMA_REG_IMR,  dma_intm_mask);
    else
        adf_os_reg_write32(sc->osdev, __DMA_REG_IMR,  dma_intr_mask);

    adf_os_print("DMR read= %#x\n",
    adf_os_reg_read32(sc->osdev, __DMA_REG_IMR));
    //adf_os_print("PCIe initialization done.\n");

    //adf_os_reg_write32(sc->osdev, __DMA_REG_PRIO, dma_prio_mask); 

    /* setup tx / rx engines */
    __hif_pci_setup_rxeng(sc) ;
    __hif_pci_setup_txeng(sc) ;

}



/**
   * @brief Put the DMA into Reset state
    */ 
void
__hif_pci_dma_put_reset(pcie_softc_t   *sc)
{
    a_uint32_t    r_data;

    hif_trace("dma_put_reset") ;

    r_data = adf_os_reg_read32(sc->osdev, __HOSTIF_REG_AHB_RESET);
    r_data |= __HOSTIF_RESET_HST_DMA;
    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_AHB_RESET, r_data);
    adf_os_reg_read32(sc->osdev, __HOSTIF_REG_AHB_RESET);

    adf_os_mdelay(1);
}

/** 
 * @brief Pull Magpie out of reset 
 * 
 * @param sc
 */
void
__hif_pci_pull_reset(pcie_softc_t *sc)
{
    a_uint32_t    r_data;

    hif_trace("pci pull reset") ;

    r_data = adf_os_reg_read32(sc->osdev, __HOSTIF_REG_AHB_RESET);
    r_data &= ~__HOSTIF_RESET_ALL;
    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_AHB_RESET, r_data);

    adf_os_mdelay(1);
}


/**
  * @brief Pull the DMA out of reset
  * 
  * @param sc
  */
void
__hif_pci_dma_pull_reset(pcie_softc_t   *sc)
{
    a_uint32_t    r_data;

    hif_trace("dma pull reset \n" ) ;

    r_data = adf_os_reg_read32(sc->osdev, __HOSTIF_REG_AHB_RESET);
    r_data &= ~__HOSTIF_RESET_HST_DMA;
    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_AHB_RESET, r_data);
    adf_os_reg_read32(sc->osdev, __HOSTIF_REG_AHB_RESET);
                            
    adf_os_mdelay(1);
}


/**
  * @brief Reset the Host DMA and wait until the Target has seen
  *        the Reset
  * 
  * @param sc
  */
void
__hif_pci_tgt_reset(pcie_softc_t   *sc)
{
    a_uint32_t   status = 0;

    __hif_pci_dma_put_reset(sc);
    
    adf_os_print("waiting for target dma to reset \n" ) ;
    for (;;) {
       status = adf_os_reg_read32(sc->osdev, __HOSTIF_REG_INTR_STATUS);

       /* Check for target dma reset. 
        * This bit may not be same for different targets. 
        */
       if ( !(status & __HOSTIF_RESET_TGT_DMA) )
                   break;
       
    }

    adf_os_print("Target dma reset detected... \n" ) ;

    __hif_pci_dma_pull_reset(sc);

    /**
      * clear the intr source
     */
    adf_os_reg_read32(sc->osdev, __HOSTIF_REG_AHB_RESET);

    /**
      * Handle target reset completion
     */
     __hif_pci_tgt_reset_done(sc);

    /* clear the interrupt */
    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_INTR_STATUS, status);
    adf_os_reg_read32(sc->osdev, __HOSTIF_REG_INTR_STATUS);

    return;
}



void
__hif_pci_reset(pcie_softc_t   *sc)
{
    /**
      *  Big Hammer reset
      * 
      *  NOTE: Hard reset (button) on the target resets the pci
     *  config space as well.
     */
     __hif_pci_pull_reset(sc);

    /**
      * Reset the Host side DMA
     */
    __hif_pci_dma_put_reset(sc);

    /**
     * Wait for the Target to reset itself
     */
    __hif_pci_tgt_reset(sc);

    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_AHB_MODE, def_ahb_mode);
}


/** 
 * PCI device handlers 
 */


/** 
 * @brief 
 * 
 * @param sc
 * 
 * @return 
 */
a_status_t
__hif_pcie_setup_intr(pcie_softc_t  *sc)
{
    a_status_t  error = A_STATUS_OK;

    /**
      * Clear all interrupts
     */
    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_INTR_ENB, 0x0);
    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_INTR_MSK, 0x0);

    error = adf_os_setup_intr(sc->osdev, hif_pcie_intr);
    if(error)
    {
        adf_os_print("Unable to register interrupt handler");
        return A_STATUS_EIO;
    }

    /**
      * Set all the Interrupts
     */
    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_INTR_ENB, host_enb_mask);
    adf_os_reg_write32(sc->osdev, __HOSTIF_REG_INTR_MSK, host_intr_mask);
    
    adf_os_print("IER read = 0x%x\n",
    adf_os_reg_read32(sc->osdev, __HOSTIF_REG_INTR_ENB));
    adf_os_print("IMR read = 0x%x\n",
    adf_os_reg_read32(sc->osdev, __HOSTIF_REG_INTR_MSK));
                                      
    return error;
}


void
__hif_pci_xmit_done(pcie_softc_t   *sc, __dma_eng_t  eng)
{
    hif_callback_t      *cb ; 
    adf_nbuf_t          buf;
    adf_os_device_t     osdev = sc->osdev;
    pci_dma_softc_t   * dma_q ;
    struct zsDmaDesc  * hwdesc;
    zdma_swdesc_t     * swdesc;
    a_uint32_t          tail, head, txeng;
    hif_status_t status ;

    /* XXX: Use same array for tx/rx engines */ 
    txeng = eng - __DMA_ENGINE_TX0 ; 

    //adf_os_spin_lock_bh(&sc->lock_irq);

    dma_q = &sc->txdma[txeng];
    head = dma_q->head;
    tail = dma_q->tail;
    cb = &sc->cb ;

    do {
        
        hif_trace("eng - %d; head - %d ; tail - %d",txeng,head,tail);
    
        /* get sw desc ptr, XXX: try with swdesc++ */ 
        swdesc = &dma_q->sw_ring[tail];
        hwdesc = swdesc->descp;
 
        /* if ring is empty, break */
        if (ring_empty(head, tail) || hw_desc_own(hwdesc))
        {
            hif_trace("breaking...");
            break;
        }

        /* increment tail */
        tail = ring_tx_incr(tail);

         /* unlink buffer */
        buf = pci_dma_unlink_buf(osdev, swdesc);
        adf_os_assert(buf);

        /* indicate tx completion to upper layer */
        status = cb->send_buf_done(cb->context , buf);

    }while(1) ;

    dma_q->tail = tail ;

    //adf_os_spin_unlock_bh(&sc->lock_irq);

    return ;
}


void
__hif_pci_recv_pkt(pcie_softc_t   *sc, __dma_eng_t  eng)
{
    adf_nbuf_t         buf;

    adf_os_device_t     osdev = sc->osdev;
    pci_dma_softc_t   * dma_q = &sc->rxdma[eng];
    struct zsDmaDesc  * hwdesc;
    zdma_swdesc_t     * swdesc;
    a_uint32_t          head, 
                        tail, 
                        pkt_len;

    //a_uint32_t pipeid;

    /* Receiving pipeid is not used by upper layers of offload stack. */
    //pipeid = __rxeng_to_pipe(eng) ;

    head = dma_q->head;
    tail = dma_q->tail;

    do {
        hif_trace("eng - %d; head - %d ", eng, head);

        swdesc = &dma_q->sw_ring[head];
        hwdesc = swdesc->descp;

        if (hw_desc_own(hwdesc))
             break;

        pkt_len = hw_desc_len(hwdesc);
        head    = ring_rx_incr(head);
        buf     = pci_dma_unlink_buf(osdev, swdesc);

        adf_os_assert(buf);

        pci_dma_recv_refill(osdev, swdesc, MAX_NBUF_SIZE);
        tail    = ring_rx_incr(tail);

        adf_nbuf_put_tail(buf, pkt_len);  
        
        /* Receiving pipeid is not used by upper layers of offload stack. */
        //sc->cb.recv_buf(sc->cb.context, buf, pipeid);
        sc->cb.recv_buf(sc->cb.context, buf, 0);

    } while(1);

    dma_q->head = head;
    dma_q->tail = tail;

    return ;
}

void
__hif_pci_recv_start(pcie_softc_t   *sc, __dma_eng_t  eng)
{
    hif_trace(" ");
    
    adf_os_reg_write32(sc->osdev, reg_offset_rxdma(eng, RX_DMA_START) ,
                                       __DMA_REG_SET_RXCTRL);

    return ;
}

/** 
 * @brief Handle dma intrrupt in BH context
 * 
 * @param sc
 */
void __hif_pcie_dma_intr(void *cxt)
{
    a_uint32_t  status = 0;
    pcie_softc_t *sc ;

    sc = (pcie_softc_t *)cxt ;

    status = sc->status_intr ;
    status &= dma_intr_mask ;

    do {

#if defined (MAGPIE_AR7100)
        ar7100_flush_pci();
#elif defined (MAGPIE_AR7240)
        ar7240_flush_pcie();
#endif

        /**
         * TX done reap
         */ 
        if(status & __DMA_INTR_TX0_DONE)
            __hif_pci_xmit_done(sc, __DMA_ENGINE_TX0);

        if(status & __DMA_INTR_TX1_DONE)
            __hif_pci_xmit_done(sc, __DMA_ENGINE_TX1);

        if(status & __DMA_INTR_TX2_DONE)
            __hif_pci_xmit_done(sc, __DMA_ENGINE_TX2);
 
        if(status & __DMA_INTR_TX3_DONE)
            __hif_pci_xmit_done(sc, __DMA_ENGINE_TX3); 
       
        /** 
         * RX Reap and enable 
         */ 
        if(status & __DMA_INTR_RX0_DONE)
            __hif_pci_recv_pkt(sc, __DMA_ENGINE_RX0);
 
        if(status & __DMA_INTR_RX0_END)
            __hif_pci_recv_start(sc, __DMA_ENGINE_RX0);
 
        if(status & __DMA_INTR_RX1_DONE)
            __hif_pci_recv_pkt(sc, __DMA_ENGINE_RX1);

        if(status & __DMA_INTR_RX1_END)
            __hif_pci_recv_start(sc, __DMA_ENGINE_RX1);                         

        status  = adf_os_reg_read32(sc->osdev, reg_offset_intr(INTERRUPT_REG));

        status &= dma_intr_mask;

    }while(status);

    /* endable dma intr */
    adf_os_reg_write32(sc->osdev, __DMA_REG_IMR,  dma_intr_mask);
    adf_os_reg_read32(sc->osdev, __DMA_REG_IMR) ;


}



/** 
 * @brief Interrupt Handler
 * 
 * @param hdl
 * 
 * @return 
 */
adf_os_irq_resp_t hif_pcie_intr(adf_drv_handle_t hdl)
{
    pcie_softc_t *sc = pciesc ;
    a_uint32_t status ;

#if defined (PCI_IRQ_SHARED)
    status = adf_os_reg_read32(sc->osdev, __HOSTIF_REG_INTR_STATUS);

    if ( adf_os_unlikely((status & host_intr_mask) == 0)){
           adf_os_print("others\n");
           return ADF_OS_IRQ_NONE;
    }
#else
    /**
      * For dedicated PCI links
      */
      status = __HOSTIF_INTR_DMA;
#endif

    if (status & __HOSTIF_INTR_DMA) {
        
        /* read dma intr status bits */
        sc->status_intr  = adf_os_reg_read32(sc->osdev, 
                                reg_offset_intr(INTERRUPT_REG));
        adf_os_reg_write32(sc->osdev, 
                __HOSTIF_REG_INTR_CLR, __HOSTIF_INTR_DMA);

        /* disable dma intr */
        adf_os_reg_write32(sc->osdev, __DMA_REG_IMR,  0);
        adf_os_reg_read32(sc->osdev, __DMA_REG_IMR) ;

        /* schedule work */
        adf_os_sched_bh(sc->os_hdl, &sc->hifwork) ;

    }

    return ADF_OS_IRQ_HANDLED;
}




/** 
 * @brief 
 * 
 * @param data
 * @param osdev
 * 
 * @return 
 */
static adf_drv_handle_t 
hif_pci_attach(adf_os_pci_data_t *data, adf_os_device_t osdev)
{
    pcie_softc_t  *sc = NULL  ;

    hif_trace(" ");

    printk("%s: \n",__func__);
    sc = pciesc ;

    sc->osdev = osdev ;
    sc->os_hdl = adf_os_pcidev_to_os(sc->osdev);

    /* init tx rx descriptors */
    __hif_pci_setup_rxdesc(sc, HIF_PCI_MAX_RX_DESC);
    __hif_pci_setup_txdesc(sc, HIF_PCI_MAX_TX_DESC);
 
    /* Setup intr */
    __hif_pcie_setup_intr(sc) ;
   
    /* reset target */
    __hif_pci_reset(sc) ;

   /* init locks, timers */
     adf_os_spinlock_init(&sc->lock_irq);

    /* Latency timer = 128 cycles*/
    adf_os_pci_config_write8(sc->osdev, __PCI_LATENCY_TIMER, 0x80);
    adf_os_pci_config_write8(sc->osdev, __PCI_CACHE_LINE_SIZE, 0x08);
    adf_os_pci_config_write8(sc->osdev, __PCI_MAGPIE_RETRY_COUNT, 0x0);

    adf_os_create_bh(sc->os_hdl, &sc->hifwork,
                               __hif_pcie_dma_intr ,( void *)sc) ;
    sc->tgt_reset_ok = 1;

    return (adf_drv_handle_t) sc ;
}

/** 
 * @brief 
 * 
 * @param hdl
 */
static void       
hif_pci_detach(adf_drv_handle_t hdl)
{
    pcie_softc_t  *sc = (pcie_softc_t *)hdl  ;

    /* cleanup tx/rx engines */
    __hif_pci_cleanup_txeng(sc);
    __hif_pci_cleanup_rxeng(sc);
 
    /* de-init tx/rx desc */ 
    __hif_pci_cleanup_txdesc(sc, HIF_PCI_MAX_TX_DESC);
    __hif_pci_cleanup_rxdesc(sc, HIF_PCI_MAX_RX_DESC);

    return ;
}

/** 
 * @brief 
 * 
 * @param hdl
 * @param pm
 */
static void       
hif_pci_suspend(adf_drv_handle_t hdl, adf_os_pm_t pm)
{
    return;
}


/** 
 * @brief 
 * 
 * @param hdl
 */
static void
hif_pci_resume(adf_drv_handle_t hdl)
{
    return;
}


/*
 * PCI device id table
 */
static adf_os_pci_devid_t  pci_devids[] = {
      { ATH_VID, ATH_WASP_PCI, ADF_OS_PCI_ANY_ID, ADF_OS_PCI_ANY_ID},
      { ATH_VID, ATH_MAGPIE_PCI, ADF_OS_PCI_ANY_ID, ADF_OS_PCI_ANY_ID},
      { ATH_VID, ATH_OWL_PCI, ADF_OS_PCI_ANY_ID, ADF_OS_PCI_ANY_ID},
      { ATH_VID, ATH_MAGPIE_PCIE, ADF_OS_PCI_ANY_ID, ADF_OS_PCI_ANY_ID},
      { ATH_VID, ATH_MERLIN_PCI, ADF_OS_PCI_ANY_ID, ADF_OS_PCI_ANY_ID},
      { ATH_VID, ATH_SCORPION_PCI, ADF_OS_PCI_ANY_ID, ADF_OS_PCI_ANY_ID},
      { ATH_VID, ATH_WASP_PCI_OTHER, ADF_OS_PCI_ANY_ID, ADF_OS_PCI_ANY_ID},
    {0},
};

/*
 * PCI driver info
 */
adf_os_pci_drvinfo_t drv_info = adf_os_pci_set_drv(hif_pci,&pci_devids[0], \
                                hif_pci_attach, hif_pci_detach, \
                                hif_pci_suspend, hif_pci_resume);



/** 
 * @brief Module init
 * 
 * @return 
 */
a_status_t 
pcie_module_init(void)
{
    a_status_t status ;

    pciesc = (pcie_softc_t *)adf_os_mem_alloc( NULL, sizeof(pcie_softc_t)) ;
    if(!pciesc)
        return A_STATUS_FAILED ;

    adf_os_mem_set((void *)pciesc, 0, sizeof(pcie_softc_t)) ;

    if( adf_os_pci_drv_reg(&drv_info) == A_STATUS_OK)
    { 
        status = A_STATUS_OK;
        adf_os_print("hif_pcie module loaded.\n");
    }
    else
        status = A_STATUS_FAILED ;

    return status ;
}


/** 
 * @brief Module exit
 */
void pcie_module_exit(void)
{
    adf_os_free_intr(pciesc->osdev);
    adf_os_pci_drv_unreg("hif_pci");
    adf_os_mem_free(pciesc) ;
    adf_os_print("hif_pcie module unloaded.\n");
}

adf_os_virt_module_init(pcie_module_init);
adf_os_virt_module_exit(pcie_module_exit);

/*
 * HIF APIs
 */
adf_os_export_symbol(hif_init);
adf_os_export_symbol(hif_register);
adf_os_export_symbol(hif_cleanup);
adf_os_export_symbol(hif_start);
adf_os_export_symbol(hif_claim_device);
adf_os_export_symbol(hif_send);
adf_os_export_symbol(hif_get_reserveheadroom);
adf_os_export_symbol(hif_get_ulpipe);
adf_os_export_symbol(hif_get_dlpipe);


MODULE_AUTHOR("Atheros Communications, Inc.");
MODULE_DESCRIPTION("Atheros Device Module");
MODULE_LICENSE("Dual BSD/GPL");


