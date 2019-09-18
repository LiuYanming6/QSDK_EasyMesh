/*
* Copyright (c) 2011 Qualcomm Atheros, Inc..
* All Rights Reserved.
* Qualcomm Atheros Confidential and Proprietary.
*/

#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/dmapool.h>
#include <linux/platform_device.h>
#include <asm/unaligned.h>

#include <atheros.h>

#include "a_base_types.h"
#include "hif_api.h"
#include <hif_pcie.h>


/************************
 * Defines
 ***********************/

#define PCI_AHB_RESET_DMA               (1 << 29)
#define PCI_AHB_RESET_DMA_HST_RAW       (1 << 31)
/* Will be asserted on reset and cleared on s/w read */
#define PCI_AHB_RESET_DMA_HST           (1 << 19)
#define PCI_MISC2_RST_CFG_DONE          (1 <<  0)

/*****************************
 * Data definitions
 ****************************/
static pcie_softc_t *pciesc ;
static const char driver_name[] = "ath-pciep";

/* DMA interrupt mask */
static const unsigned int 
dma_intr_txdone = (  __DMA_INTR_TX0_DONE 
                   | __DMA_INTR_TX1_DONE ); 


static const unsigned int 
dma_intr_rxend =    ( __DMA_INTR_RX0_END | __DMA_INTR_RX1_END
                     | __DMA_INTR_RX2_END | __DMA_INTR_RX3_END);


static const unsigned int 
dma_intr_rxdone =   ( __DMA_INTR_RX0_DONE | __DMA_INTR_RX1_DONE 
                     | __DMA_INTR_RX2_DONE | __DMA_INTR_RX3_DONE);


static const unsigned int 
dma_intr_rx = (  __DMA_INTR_RX0_DONE | __DMA_INTR_RX1_DONE 
                | __DMA_INTR_RX2_DONE | __DMA_INTR_RX3_DONE
                | __DMA_INTR_RX0_END | __DMA_INTR_RX1_END
                | __DMA_INTR_RX2_END | __DMA_INTR_RX3_END );

static const unsigned int 
dma_intr_mask = (  __DMA_INTR_TX0_DONE | __DMA_INTR_TX1_DONE 
                 | __DMA_INTR_RX0_DONE | __DMA_INTR_RX1_DONE 
                 | __DMA_INTR_RX2_DONE | __DMA_INTR_RX3_DONE
                 | __DMA_INTR_RX0_END | __DMA_INTR_RX1_END
                 | __DMA_INTR_RX2_END | __DMA_INTR_RX3_END );


//static a_uint32_t dmabase ;
struct ath_pci_regs __iomem  *dmabase;

/* 
 * Pointers to individual dma queues.
*/
static pci_dma_softc_t *dmaq_rx0;
static pci_dma_softc_t *dmaq_rx1;
static pci_dma_softc_t *dmaq_rx2;
static pci_dma_softc_t *dmaq_rx3;
static pci_dma_softc_t *dmaq_tx0;
static pci_dma_softc_t *dmaq_tx1;

/* Set this when host-target reset sequence 
 * is complete and successful 
*/
static int init_ok = 0;

enum ath_pci_register{
        ATH_REG_AHB_RESET       = 0xb806001c,
        ATH_REG_MISC2_RESET     = 0xb80600bc,
        ATH_REG_RESET2          = 0xb80600c4,
        ATH_PCIEP_INTR_MASK     = 0xb80600c8,
        ATH_PCIEP_VD            = 0xb8230000
};


/** 
 * @brief Set this to nonzero
 *        to enable debug prints
 */
int hif_dbg_mask = 0 ;
module_param(hif_dbg_mask,int,0);
MODULE_PARM_DESC(hif_dbg_mask,"Set this to nonzero to enable debug prints");


/*
 * Function prototypes
 */

int __init 
ath_pci_init(void) ;

void __exit 
ath_pci_exit(void) ;



/*************************
 * Reset  
 *************************/

/*
 * Timer callback to periodically 
 * check for host reset
 *
 * XXX: find a cleaner approach for 
 * host->target reset indication.
 */
void reset_worker(unsigned long data)
{
        volatile unsigned int r_data;
        pcie_softc_t *sc = (pcie_softc_t *)data ;

        if(init_ok)
        {
                r_data = ath_reg_rd(ATH_REG_AHB_RESET);
                if (r_data & PCI_AHB_RESET_DMA_HST_RAW)
                {
                        /* Wait for host offload stack to finish cleanup. */
                        mdelay(5000);

                        printk("%s: Host DMA reset detected. "\
                                "Trigger full chip reset.\n",__FUNCTION__);
                        ath_reg_rmw_set( ATH_REG_AHB_RESET , (1<<24)) ;
                }
        }

        mod_timer( &sc->reset_timer, jiffies + msecs_to_jiffies(1000) );
}



/** 
 * @brief Handle pcie reset
 */
void
ath_pci_handle_reset(void)
{
        volatile unsigned int r_data;

        printk("Waiting for host reset..%d\n", __LINE__);
        /**
         * Poll until the Host has reset
         */
        for (;;) {
                r_data = ath_reg_rd(ATH_REG_AHB_RESET);

                if (r_data & PCI_AHB_RESET_DMA_HST_RAW)
                        break;
        }
        printk("received.\n");

        /**
         * Pull the AHB out of reset
         */
        ath_reg_rmw_clear(ATH_REG_AHB_RESET, PCI_AHB_RESET_DMA);
        udelay(10);

        /**
         * Put the AHB into reset
         */
        ath_reg_rmw_set(ATH_REG_AHB_RESET, PCI_AHB_RESET_DMA);
        udelay(10);

        /**
         * Pull the AHB out of reset
         */
        ath_reg_rmw_clear(ATH_REG_AHB_RESET, PCI_AHB_RESET_DMA);

        r_data = ath_reg_rd(ATH_PCIEP_VD);
        //printk("%s: ven-dev-id reg - 0x%x \n",__func__,r_data);
        if ((r_data & 0xffff0000) == 0xff1c0000) {
                //printk("%s: ************Program vend,dev ids****************\n",__func__);
                /*
                 * Program the vendor and device ids after reset.
                 * In the actual chip this may come from OTP
                 * The actual values will be finalised later
                 */
#if defined(CONFIG_MACH_AR934x)
                /* Wasp */
                ath_reg_wr(ATH_PCIEP_VD, 0x0034168c);
#elif defined(CONFIG_MACH_QCA955x)
                /* Scorpion */
                ath_reg_wr(ATH_PCIEP_VD, 0x0039168c);
#else
#error undefined platform
#endif
        }

        ath_reg_rmw_set(ATH_REG_MISC2_RESET, PCI_MISC2_RST_CFG_DONE);

        /* Set init successful status */
        init_ok = 1;
}



/*************************
 * Init & cleanup
 ************************/

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

        hif_wr_txdma(dmabase, i, TX_DESC_START_ADDR, paddr);
        hif_wr_txdma(dmabase, i, TX_BURST_SIZE, DMA_BURST_8W);

        /**
          * Limit the TX interrupt to 16 packets or wait for 0xfff x 32
          * Cycles, which ever happens earlier
         */
        hif_wr_txdma(dmabase, i, TX_INTR_LIM, DMA_MAX_INTR_LIM);

        hif_wr_txdma(dmabase, i, TX_DATA_SWAP, DMA_BYTE_SWAP_ON);

        switch(i)
        {
            case 0: 
                dmaq_tx0 = &sc->txdma[i];
                break;

            case 1: 
                dmaq_tx1 = &sc->txdma[i];
                break;

            default:
                printk("hif_pcie: Incorrect Tx chian number\n");
                hif_assert(0);
                break;
        }

        /* start tx engine */
        hif_wr_txdma(dmabase, i, TX_DMA_START, 0x1);
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
        hif_wr_txdma(dmabase, i, TX_DMA_START, 0);
    }

    dmaq_tx0 = dmaq_tx1 = 0;
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

        hif_wr_rxdma(dmabase, i, RX_DESC_START_ADDR, paddr);
        hif_wr_rxdma(dmabase, i, RX_BURST_SIZE, DMA_BURST_16W);
        hif_wr_rxdma(dmabase, i, RX_DATA_SWAP, DMA_BYTE_SWAP_ON);

        switch(i)
        {
            case 0: 
                dmaq_rx0 = &sc->rxdma[i];
                break;

            case 1: 
                dmaq_rx1 = &sc->rxdma[i];
                break;

            case 2: 
                dmaq_rx2 = &sc->rxdma[i];
                break;

            case 3: 
                dmaq_rx3 = &sc->rxdma[i];
                break;

            default:
                printk("hif_pcie: Incorrect Rx chian number\n");
                hif_assert(0);
                break;
        }

        /* start rx engine */
        hif_wr_rxdma(dmabase, i, RX_DMA_START, 0x01);
    }

    return ;
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
        hif_wr_rxdma(dmabase, i, RX_DMA_START, 0);
    }

    dmaq_rx0 = dmaq_rx1 = dmaq_rx2 = dmaq_rx3 = 0;
}




/************************
 * PCI Driver Interface 
 ***********************/

/** 
 * @brief Tx END
 * 
 * @param 
 * @param 
 * 
 * @return 
 */
/*
void
__hif_pci_xmit_end(pcie_softc_t   *sc, __dma_eng_t  eng)
{
    return ;
}
*/



/** 
 * @brief Tx DONE/COMPLETE
 * 
 * @param 
 * @param 
 * 
 * @return 
 */
static void
__hif_pci_xmit_done(pcie_softc_t   *sc, pci_dma_softc_t *dma_q, a_uint32_t quota)
{
    hif_status_t (*tx_done)(void *ctx, struct sk_buff *netbuf);
    void              * ctx ;
    struct sk_buff    * buf;
    struct device     * osdev = sc->dev;
    struct zsDmaDesc  * hwdesc;
    zdma_swdesc_t     * swdesc;
    a_uint32_t          tail, head;

    head = dma_q->head;
    tail = dma_q->tail;
    tx_done = sc->cb.send_buf_done;
    ctx = sc->cb.context;

    for(; quota; quota--) {
        /* get sw desc ptr, XXX: try with swdesc++ */
        swdesc = &dma_q->sw_ring[tail];
        hwdesc = swdesc->descp;

        /* if ring is empty, break */
        if ( unlikely(ring_empty(head, tail) || hw_desc_own(hwdesc)) )
            break;

        /* increment tail */
        tail = ring_tx_incr(tail);

         /* unlink buffer */
        buf = pci_dma_unlink_buf(osdev, swdesc, DMA_TO_DEVICE);
        hif_assert(buf);

        /* indicate tx completion to upper layer */
        tx_done(ctx , buf);

    }

    dma_q->tail = tail ;

    return ;
}



/** 
 * @brief Convert Rx engine number to corresponding pipe
 * 
 * @param 
 * 
 * @return 
 */
/*
static a_uint32_t 
__rxeng_to_pipe(a_uint32_t eng)
{
    a_uint32_t pipe = 0 ;

    switch(eng)
    {
        case __DMA_ENGINE_RX0:
             pipe = HIF_CONTROL_PIPE;
             break ;

        case __DMA_ENGINE_RX1: 
             pipe = HIF_COMMAND_PIPE;
             break ;

        case __DMA_ENGINE_RX2: 
             pipe = HIF_DATA_LP_PIPE;
             break ;

        case __DMA_ENGINE_RX3: 
             pipe = HIF_WSUPP_PIPE;
             break ;

        default:
             pipe =  HIF_DATA_LP_PIPE;
             break ;
    }
    
    return pipe;
}
*/




/** 
 * @brief Rx DONE/COMPLETE
 * 
 * @param 
 * @param 
 * 
 * @return
 * 
 * XXX: Investigate need to using quotas. 
 */
static void
__hif_pci_recv_pkt(pcie_softc_t   *sc, pci_dma_softc_t *dma_q)
{
    hif_status_t (*recv_buf)(void *ctx, 
                      struct sk_buff *netbuf, a_uint8_t pipeid);
    void              * ctx;
    struct sk_buff    * buf;
    struct device     * osdev = sc->dev;
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
    recv_buf = sc->cb.recv_buf;
    ctx = sc->cb.context;

    do {
        hif_trace("head - %d ", head);

        swdesc = &dma_q->sw_ring[head];
        hwdesc = swdesc->descp;

        if (hw_desc_own(hwdesc))
             break;

        pkt_len = hw_desc_len(hwdesc);
        head    = ring_rx_incr(head);
        buf     = pci_dma_unlink_buf(osdev, swdesc, DMA_FROM_DEVICE);

        hif_assert(buf);

        hif_assert(skb_tailroom(buf) >= pkt_len);

        skb_put(buf,pkt_len);

        /* Receiving pipeid is not used by upper layers of offload stack. */
        //sc->cb.recv_buf(sc->cb.context, buf, pipeid);
        recv_buf(ctx, buf, 0);

    } while(1);

    while(!ring_empty(head,tail)) {
        swdesc = &dma_q->sw_ring[tail];
        pci_dma_recv_refill(osdev, swdesc, MAX_NBUF_SIZE);
        tail    = ring_rx_incr(tail);
    }


    dma_q->head = head;
    dma_q->tail = tail;

    return ;
}




/** 
 * @brief Rx END
 * 
 * @param 
 * @param 
 * 
 * @return 
 */
static inline void
__hif_pci_recv_start(pcie_softc_t   *sc, __dma_eng_t  eng)
{
    hif_trace(" ");

    /* start rx engine */
    hif_wr_rxdma(dmabase, eng, RX_DMA_START, 0x01);

    return ;
}




/** 
 * @brief Deferred processing of interrupts
 * 
 * @param data - struct ath_pci_dev * 
 */
void hif_deferred_worker(unsigned long data)
{
    pcie_softc_t *sc = (pcie_softc_t *)data ;
    a_uint32_t  status = 0;
    a_uint32_t  mask   = 0;

    /* Disable all dma interrupts */
    mask = hif_rd_intr(dmabase, INTR_MASK);
    hif_wr_intr(dmabase, INTR_MASK, 0);

    status = sc->status_intr;
    sc->status_intr = 0;

    /* XXX: Fixme */
    /* For the first pass assume all intr are set.
    */
    status |= dma_intr_mask;
   
    do {

#if defined (MAGPIE_AR7100)
        ar7100_flush_pci();
#elif defined (MAGPIE_AR7240)
        ar7240_flush_pcie();
#endif

        /**
         * TX Reap
         */
        if(mask & dma_intr_txdone) {
            if(status & __DMA_INTR_TX0_DONE)
                __hif_pci_xmit_done(sc, dmaq_tx0, HIF_PCI_MAX_TX_DESC);

            if(status & __DMA_INTR_TX1_DONE)
                __hif_pci_xmit_done(sc, dmaq_tx1, HIF_PCI_MAX_TX_DESC);
        }

/*
        if(status & __DMA_INTR_TX0_END)
            __hif_pci_xmit_end(sc, __DMA_ENGINE_TX0);
        if(status & __DMA_INTR_TX1_END)
            __hif_pci_xmit_end(sc, __DMA_ENGINE_TX1);
*/


        /** 
         * RX Reap and enable 
         */
        if(status & __DMA_INTR_RX0_DONE)
            __hif_pci_recv_pkt(sc, dmaq_rx0);

        if(status & __DMA_INTR_RX0_END)
            __hif_pci_recv_start(sc, __DMA_ENGINE_RX0);

        if(status & __DMA_INTR_RX1_DONE)
            __hif_pci_recv_pkt(sc, dmaq_rx1);

        if(status & __DMA_INTR_RX1_END)
            __hif_pci_recv_start(sc, __DMA_ENGINE_RX1);

        if(status & __DMA_INTR_RX2_DONE)
            __hif_pci_recv_pkt(sc, dmaq_rx2);

        if(status & __DMA_INTR_RX2_END)
            __hif_pci_recv_start(sc, __DMA_ENGINE_RX2);

        if(status & __DMA_INTR_RX3_DONE)
            __hif_pci_recv_pkt(sc, dmaq_rx3);

        if(status & __DMA_INTR_RX3_END)
            __hif_pci_recv_start(sc, __DMA_ENGINE_RX3);

        /* read recent interrupt status */
        status = hif_rd_intr(dmabase, INTR_STATUS);
        status &= dma_intr_mask;

    }while(status);

    /* enable all intrrupts */
    hif_wr_intr(dmabase, INTR_MASK, dma_intr_mask);

    return ;
}



/** 
 * @brief PCIE EP ISR
 * 
 * @param 
 */
irqreturn_t
hif_isr(int irq, void *dev)
{
    pcie_softc_t *sc = (pcie_softc_t *)dev;
    a_uint32_t status = 0;
    a_uint32_t mask = 0 ;

    hif_assert(sc);

#if !defined(CONFIG_MACH_QCA955x)
    /* Flush the entry */
    ath_flush_pcie();
#endif

    /* Save dma intr mask and disable intr */
    mask = hif_rd_intr(dmabase, INTR_MASK);
    hif_wr_intr(dmabase, INTR_MASK, 0);

    /* read dma intr status bits */
    status = hif_rd_intr(dmabase, INTR_STATUS);
    status &= dma_intr_mask ;

    hif_assert(status);

    /*
     * Tx Reap. Disabled Tx intr indicate 
     * chains are being updated by Tx path.
    */
    if(mask & dma_intr_txdone) {
        if(status & __DMA_INTR_TX0_DONE)
            __hif_pci_xmit_done(sc, dmaq_tx0, HIF_PCI_MAX_TX_DESC);

        if(status & __DMA_INTR_TX1_DONE)
            __hif_pci_xmit_done(sc, dmaq_tx1, HIF_PCI_MAX_TX_DESC);

        sc->status_intr &= ~dma_intr_txdone;
    } else {
        sc->status_intr |= dma_intr_txdone;
    }
        
    /* 
     * Don't schedule the BH unless Rx interrupt has occured.
    */
    if(status & dma_intr_rx) {
        status &= ~dma_intr_txdone;
        sc->status_intr |= status;
        mask &= ~(sc->status_intr);
        tasklet_schedule( &sc->hif_deferred_work );
    }

    hif_wr_intr(dmabase, INTR_MASK, mask);

    return IRQ_HANDLED;
}



#if 0
static void 
ath_pci_dev_release(struct device *dev)
{
    return ;
}
#endif

static noinline int
ath_pci_dev_init(pcie_softc_t *sc)
{
#if !defined(CONFIG_MACH_QCA955x)
        device_initialize(&sc->pdev->dev);
#endif

        //pci->dev->release = ath_pci_dev_release;
        //pci->dev->parent = dev;

#if !defined(CONFIG_MACH_QCA955x)
        device_add(&sc->pdev->dev);
#endif

    ath_pci_handle_reset();

    tasklet_init( &pciesc->hif_deferred_work ,
                    hif_deferred_worker , (unsigned long)pciesc );

    /* initialize host reset detect timer */
    init_timer( &pciesc->reset_timer);
    pciesc->reset_timer.expires  = jiffies + msecs_to_jiffies(1000);
    pciesc->reset_timer.function = reset_worker;
    pciesc->reset_timer.data = (unsigned long)pciesc;
    add_timer( &pciesc->reset_timer);
    /* setup tx / rx engines */
    __hif_pci_setup_txeng(sc) ;
    __hif_pci_setup_rxeng(sc) ;

    hif_wr_intr(dmabase, INTR_MASK, dma_intr_mask);

    return 0;
}



static int
ath_pci_dev_probe(struct platform_device *pdev)
{
        struct ath_pci_regs __iomem *reg_base;
        int retval = 0;

#if CONFIG_MACH_QCA955x
        /* Set RC/EP in EP mode */
        ath_reg_rmw_set(ATH_REG_RESET2, RST_RESET2_EP_MODE_MASK);
#endif

        pciesc = (pcie_softc_t *)vmalloc(sizeof(pcie_softc_t));
        if (pciesc == NULL) {
                printk("Unable to allocate pci device\n");
                return -ENOMEM;
        }
        memset(pciesc, 0, sizeof(pcie_softc_t));
        pciesc->pdev = pdev;
        pciesc->dev = &pdev->dev;
        pciesc->dev->parent = pciesc->dev;
        spin_lock_init(&pciesc->lock_irq);

        if (pci_dma_init(pciesc) != 0) {
                retval = -ENOMEM;
                goto err1;
        }
        /* Allocate and map resources */
        if (!request_mem_region(pdev->resource[0].start,
                                pdev->resource[0].end - pdev->resource[0].start + 1,
                                driver_name)) {
                printk("ath_pci_dev_probe: controller already in use\n");
                retval = -EBUSY;
                goto err1;
        }

        reg_base = ioremap(pdev->resource[0].start,
                        pdev->resource[0].end - pdev->resource[0].start + 1);
        if (!reg_base) {
                printk("ath_pci_dev_probe: error mapping memory\n");
                retval = -EFAULT;
                goto err2;
        }

        pciesc->reg_base = reg_base;
        //dmabase = (a_uint32_t)reg_base;
        dmabase = reg_base;

        /* Mask dma interrupts initially */
        hif_wr_intr(dmabase, INTR_MASK, 0);

        /* Interrupt Request */
        if ((retval = request_irq(pdev->resource[1].start, hif_isr,
#ifdef CONFIG_MACH_QCA955x
                                IRQF_DISABLED,
#else
                                IRQF_SHARED,
#endif
                                driver_name, (void*)pciesc)) != 0) {
                printk("request interrupt %x failed\n", pdev->resource[1].start);
                retval = -EBUSY;
                goto err3;
        }

        if (ath_pci_dev_init(pciesc) == 0) {

                printk("%s: ath_pci_dev_init() returns SUCCESS \n",__FUNCTION__);
                return 0;
        }
        else printk("%s: ath_pci_dev_init() returns ERROR \n",__FUNCTION__);

        free_irq(pdev->resource[1].start, (void*)pciesc);
err3:
        printk(" ");
        iounmap(reg_base);
        dmabase = NULL;

err2:
        printk(" ");
        release_mem_region(pdev->resource[0].start,
                                pdev->resource[0].end - pdev->resource[0].start + 1);

err1:
        vfree(pciesc);
        pciesc = NULL;

        return retval;
}



static int 
ath_pci_dev_remove(struct platform_device *pdev)
{
        hif_wr_intr(dmabase, INTR_MASK, 0);

        __hif_pci_cleanup_txeng(pciesc);
        __hif_pci_cleanup_rxeng(pciesc);

        tasklet_kill(&pciesc->hif_deferred_work);
        del_timer_sync(&pciesc->reset_timer);

        free_irq(pdev->resource[1].start, (void *)pciesc);
        iounmap(pciesc->reg_base);
        dmabase = NULL;
        release_mem_region(pdev->resource[0].start,
                        pdev->resource[0].end - pdev->resource[0].start + 1);

        pci_dma_deinit(pciesc);

        //device_unregister(dev->dev);

        vfree(pciesc);
        pciesc = NULL ;

        return 0;
}


static struct platform_driver ath_pci_ep_drv = {
        .probe  = ath_pci_dev_probe,
        .remove = ath_pci_dev_remove,
        .driver = {
                .name = (char *)driver_name,
                .owner = THIS_MODULE,
        },
};

int __init ath_pci_init(void)
{
    return platform_driver_register(&ath_pci_ep_drv);
}


void __exit ath_pci_exit(void)
{
    platform_driver_unregister(&ath_pci_ep_drv);
}



/*************************
 * HIF api implementation
 *************************/ 

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

    memcpy(&sc->oscb, oscb, sizeof(hif_os_callback_t));

    oscb->device_detect((hif_handle_t)sc);

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
hif_claim_device(hif_handle_t hif_handle, void *ctx)
{
    pcie_softc_t *sc = (pcie_softc_t *)hif_handle ;

    sc->context = ctx ;

    return ;
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
    pcie_softc_t *sc = (hif_handle_t)hif_handle ;
    
    memcpy(&sc->cb, cb, sizeof(hif_callback_t));

    return ;
}



/** 
 * @brief Get a tx engine number for a pipe
 * 
 * @param pipe
 * 
 * @return Tx engine number
 */
static inline a_uint32_t 
__pipe_to_txengine(a_uint32_t pipe)
{
    a_uint32_t eng ;

    eng = __DMA_ENGINE_TX0;

    switch(pipe)
    {
        case HIF_CONTROL_PIPE:
             eng = __DMA_ENGINE_TX0;
             break ;

        case HIF_COMMAND_PIPE:
             eng = __DMA_ENGINE_TX0;
             break ;

        case HIF_DATA_LP_PIPE:
             eng = __DMA_ENGINE_TX1;
             break ;

        case HIF_WSUPP_PIPE:
             eng = __DMA_ENGINE_TX1;
             break ;

        default:
             eng = __DMA_ENGINE_TX1;
             break ;
    }
    return eng;
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

    pcie_softc_t *sc;
    a_uint32_t head ,tail ;
    zdma_swdesc_t *swdesc;
    pci_dma_softc_t * dma_q ;
    a_uint32_t chain;
    //a_uint32_t mask;

    sc = (pcie_softc_t *)hif_handle ;

    chain = __pipe_to_txengine(pipeid);
    dma_q = &sc->txdma[chain];

    tail = dma_q->tail;

    /* Disable Tx interrupts */
    hif_dma_mask_clear(dmabase, dma_intr_txdone);

    /* lock the queue; */
    spin_lock(&sc->lock_irq);

    head = dma_q->head;

    hif_trace("chain - %d; head - %d ; tail - %d ; "\
            "pkt_len - %d",chain,head, tail, skb->len);

    if(ring_tx_full(head, tail))
    {
        hif_wr_intr(dmabase, INTR_MASK, dma_intr_mask); 
        spin_unlock(&sc->lock_irq);
        return HIF_STATUS_ERROR ;
    }

    dma_q->head = ring_tx_incr(head);

    /* unlock the queue */ 
    spin_unlock(&sc->lock_irq);

    swdesc = &dma_q->sw_ring[head];

    hif_assert(swdesc->nbuf == NULL);

    pci_dma_link_buf(sc->dev, swdesc, skb, DMA_TO_DEVICE);

    /* mark desc ready */
    pci_zdma_mark_rdy(swdesc, (ZM_FS_BIT | ZM_LS_BIT));

    /* Enable Tx interrupts only after tx descriptor is marked ready. */
    hif_dma_mask_set(dmabase, dma_intr_txdone);

    /* start tx engine */
    hif_wr_txdma(dmabase, chain, TX_DMA_START, 0x1);

    return HIF_STATUS_OK;
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
    return (a_uint32_t)type;
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


/** 
 * @brief Module Init
 * 
 * @return 
 */
int hif_pcie_module_init(void )
{
    int ret = 0;

    ret = ath_pci_init() ;
    
    if(ret == 0)
        printk("hif_pcie loaded.\n");

    return ret;    
}


/** 
 * @brief Module Exit
 */
void hif_pcie_module_exit(void )
{
    ath_pci_exit() ;

    printk("hif_pcie unloaded.\n");

    return; 
}

module_init(hif_pcie_module_init);
module_exit(hif_pcie_module_exit);

EXPORT_SYMBOL(hif_init);
EXPORT_SYMBOL(hif_cleanup);
EXPORT_SYMBOL(hif_start);
EXPORT_SYMBOL(hif_claim_device);
EXPORT_SYMBOL(hif_send);
EXPORT_SYMBOL(hif_register);
EXPORT_SYMBOL(hif_get_reserveheadroom);
EXPORT_SYMBOL(hif_get_ulpipe);
EXPORT_SYMBOL(hif_get_dlpipe);


MODULE_AUTHOR("Atheros Communications, Inc.");
MODULE_DESCRIPTION("Atheros Device Module");
MODULE_LICENSE("Dual BSD/GPL");

