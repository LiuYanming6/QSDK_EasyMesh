/*
* Copyright (c) 2011 Qualcomm Atheros, Inc..
* All Rights Reserved.
* Qualcomm Atheros Confidential and Proprietary.
*/

#ifndef _HIF_PCI_H_
#define _HIF_PCI_H_

#include <linux/spinlock.h>
#include <linux/skbuff.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/device.h>

#include "a_base_types.h"
#include "hif_api.h"


#define hif_assert(expr)                                                \
            do {                                                        \
                  if(unlikely(!(expr))) {                               \
                    printk(KERN_ERR "Assertion failed! %s:%s %s:%d\n",  \
                    #expr, __FUNCTION__, __FILE__, __LINE__);           \
                    dump_stack();                                       \
                    panic("Take care of the assert first\n");           \
                  }                                                     \
                }while(0)


#define hif_trace(fmt, args...)                                             \
                    do {                                                    \
                            extern int hif_dbg_mask ;                       \
                            if(hif_dbg_mask){                               \
                                printk("hif_pcie: %s(): ",__FUNCTION__);    \
                                printk(fmt, ##args);                        \
                                printk("\n");                               \
                            }                                               \
                        }while(0)


/**
 * @brief Assert
 */
#define hif_assert(expr)  do {    \
    if(unlikely(!(expr))) {                                 \
        printk(KERN_ERR "Assertion failed! %s:%s %s:%d\n",   \
              #expr, __FUNCTION__, __FILE__, __LINE__);      \
        dump_stack();                                      \
        panic("Take care of the assert first\n");          \
    }     \
}while(0)



/* Wasp PCIe registers */
enum pci_regs{
    WASP_REG_AHB_RESET      = 0xb806001c,
    WASP_REG_MISC2_RESET    = 0xb80600bc,
    WASP_PCIEP_VD           = 0xb8230000,
};

#define DMA_BASE_OFF_HST            0x18127000

#define PCI_AHB_RESET_DMA           (1 << 29)
#define PCI_AHB_RESET_DMA_HST_RAW   (1 << 31)

/* Will be asserted on reset and cleared on s/w read */
#define PCI_AHB_RESET_DMA_HST       (1 << 19)
#define PCI_MISC2_RST_CFG_DONE      (1 <<  0)


#define ar7240_reg_rd(_phys)    (*(volatile a_uint32_t *)KSEG1ADDR(_phys))

#define ar7240_reg_wr_nf(_phys, _val) \
            ((*(volatile a_uint32_t *)KSEG1ADDR(_phys)) = (_val))

#define ar7240_reg_wr(_phys, _val) do { \
            ar7240_reg_wr_nf(_phys, _val);  \
            ar7240_reg_rd(_phys);           \
} while(0)



/* ******** End Pcie registers *********** */


#define MAX_TXDESC_SHIFT        8
#define MAX_RXDESC_SHIFT        8
#define HIF_PCI_MAX_TX_DESC     (1 << MAX_TXDESC_SHIFT)
#define HIF_PCI_MAX_RX_DESC     (1 << MAX_RXDESC_SHIFT)
#define HIF_PCI_MAX_RXCHAINS    4
#define HIF_PCI_MAX_TXCHAINS    2
#define MAX_NBUF_SIZE           2000 /**< XXX: check the max packet size */
#define PCI_NBUF_ALIGNMENT      32   
#define PCI_NBUF_BOUNDRY        4096   /**< nbuf will not cross this power of two boundary */



/* 
 * start address in physical memory and 
 * total len of dma registers (interrupt,
 * tx / rx chains). We need corresponding 
 * virtual address to access these.
 */
#define DMA_REGS_START          DMA_BASE_OFF_HST 
#define DMA_REGS_LEN            0xE00

/* 
 * amount of memory occupied by 
 * registers of one rx/tx dma chain 
 */
#define DMA_CHAIN_REG_LEN    0x100

#define ATH_VID                 0x168c /* Atheros Vendor ID */
#define ATH_WASP_PCI            0x0034 /* Wasp PCI Device ID  */


/*
 * Register definitions
 */

/* 
 * Offsets (from DMA_REGS_START) 
 * of various register blocks
 */
#define DMA_INTR_START          0x0
#define DMA_RX_0_START          0x800
#define DMA_RX_1_START          0x900
#define DMA_RX_2_START          0xA00
#define DMA_RX_3_START          0xB00
#define DMA_TX_0_START          0xC00
#define DMA_TX_1_START          0xD00


/*
 * Individual register names 
 * and their offsets within respective 
 * blocks (above).
 */

/* Interrupt registers */
#define INTR_STATUS             0x0
#define INTR_MASK               0x4

/* 
 * Offset of registers 
 * within a Rx DMA chain
*/
#define RX_DESC_START_ADDR      0x0
#define RX_DMA_START            0x4
#define RX_BURST_SIZE           0x8
#define RX_PKT_OFFSET           0xC
#define RX_CHECKSUM             0x10
#define RX_DBG                  0x14
#define RX_DBG_CUR_ADDR         0x18
#define RX_DATA_SWAP            0x1C


/* 
 * Offset of registers 
 * within a Tx DMA chain
*/
#define TX_DESC_START_ADDR      0x0
#define TX_DMA_START            0x4
#define TX_INTR_LIM             0x8
#define TX_BURST_SIZE           0xC
#define TX_DBG                  0x10
#define TX_DBG_CUR_ADDR         0x14
#define TX_DATA_SWAP            0x18


/* 
 * Use this macro and INTERRUPT_* above 
 * to get absolute offset for a
 * interrupt register.
 */
#define reg_offset_intr(reg)    ( DMA_INTR_START + reg )

/* 
 * Use this macro and RX_* above 
 * to get absolute offset for 
 * rx dma register of a particular chain (0,1,2,3).
 */
#define reg_offset_rxdma(chain,rx_reg)  \
                ( DMA_RX_0_START + (chain * DMA_CHAIN_REG_LEN) + rx_reg)
/* 
 * Use this macro and TX_* above 
 * to get absolute offset for 
 * tx dma register of a particular chain (0,1).
 */
#define reg_offset_txdma(chain,tx_reg)  \
                ( DMA_TX_0_START + (chain * DMA_CHAIN_REG_LEN) + tx_reg)




#define DMA_MAX_INTR_TIMO       0xFFF
#define DMA_MAX_INTR_CNT        0xF
#define DMA_MAX_INTR_LIM        ((DMA_MAX_INTR_TIMO << 4) | DMA_MAX_INTR_CNT)

/**
  * @brief DMA Burst sizes
  */
enum __dma_burst_size{
    DMA_BURST_4W   = 0x00,
    DMA_BURST_8W   = 0x01,
    DMA_BURST_16W  = 0x02
};


/**
  * @brief PCI Config Standard registers
 */
enum __pci_std_regs{
    __PCI_LATENCY_TIMER      = 0x0d,
    __PCI_CACHE_LINE_SIZE    = 0x0c,
    __PCI_MAGPIE_RETRY_COUNT = 0x41
};



/**
 * @brief DMA Engine
 */
typedef enum __dma_eng{
    __DMA_ENGINE_RX0 = 0,
    __DMA_ENGINE_RX1 = 1,
    __DMA_ENGINE_RX2 = 2,
    __DMA_ENGINE_RX3 = 3,
    __DMA_ENGINE_TX0 = 0,
    __DMA_ENGINE_TX1 = 1,
}__dma_eng_t;



/** 
 * @brief Interrupt status bits 
 */
typedef enum __dma_intr_bits{
    __DMA_INTR_TX1_END        = (1 << 25),/* */
    __DMA_INTR_TX0_END        = (1 << 24),/* */
    __DMA_INTR_TX1_DONE       = (1 << 17),/* */
    __DMA_INTR_TX0_DONE       = (1 << 16),/* */
    __DMA_INTR_RX3_END        = (1 << 11),/* */
    __DMA_INTR_RX2_END        = (1 << 10),/* */
    __DMA_INTR_RX1_END        = (1 << 9),/* */
    __DMA_INTR_RX0_END        = (1 << 8),/* */
    __DMA_INTR_RX3_DONE       = (1 << 3), /* */
    __DMA_INTR_RX2_DONE       = (1 << 2), /* */
    __DMA_INTR_RX1_DONE       = (1 << 1), /*RX1 received a packet*/
    __DMA_INTR_RX0_DONE       = (1 << 0), /*RX0 received a packet*/
}__dma_intr_bits_t;


/**
  * Register Values for DMA related operation
  */
typedef enum __dma_reg_val{
    __DMA_REG_SET_TXCTRL = 0x01,/*TX Start bit*/
    __DMA_REG_SET_RXCTRL = 0x01 /*RX Start bit*/
}__dma_reg_val_t;


/**
  * @brief PCI host interrupt bits
  */
typedef enum __hostif_intr_bits{ 
    __HOSTIF_INTR_TGT_DMA_RST   = (1 << 0),
    __HOSTIF_INTR_DMA           = (1 << 1)
}__hostif_intr_bits_t;


/**
  * @brief Enable or Disable Byte / Descriptor swapping
 */
enum __dma_byte_swap{
    DMA_BYTE_SWAP_OFF = 0x00,
    DMA_BYTE_SWAP_ON  = 0x01,
    DMA_DESC_SWAP_ON  = 0x02 /* Required for Big Endian Host*/
};


/** 
 * @brief Structure representing a h/w
 *        dma descriptor
 */
struct zsDmaDesc{
    /* Big Endian machine */
    a_uint16_t      ctrl;       // Descriptor control
    a_uint16_t      status;     // Descriptor status
    a_uint16_t      totalLen;   // Total length
    a_uint16_t      dataSize;   // Data size
    struct zsDmaDesc*        lastAddr;   // Last address of this chain
    a_uint32_t               dataAddr;   // Data buffer address
    struct zsDmaDesc*        nextAddr;   // Next TD address
    a_uint8_t                pad[12]; /* Pad for 32 byte Cache Alignment*/
};


/* Status bits definitions */
/* Own bits definitions */
#define ZM_OWN_BITS_MASK        0x3
#define ZM_OWN_BITS_SW          0x0
#define ZM_OWN_BITS_HW          0x1
#define ZM_OWN_BITS_SE          0x2
/* Control bits definitions */
/* First segament bit */
#define ZM_LS_BIT               0x100
/* Last segament bit */
#define ZM_FS_BIT               0x200


/** 
  * @brief Software dma descriptor for 
  *        each h/w dma descriptor.
  */
typedef struct zdma_swdesc {
    dma_addr_t          buf_addr ; /* bus address of a mapped buffer */
    a_uint32_t          buf_size;  /* length of a mapped buffer */
    struct sk_buff*      nbuf;
    dma_addr_t          hwaddr; /* dma address of h/w desc */
    struct zsDmaDesc    *descp; /* virt address of h/w desc */
}zdma_swdesc_t;


/** 
  * @brief Context for one Dma ring.
  */
typedef struct pci_dma_softc{
    zdma_swdesc_t       *sw_ring;
    struct zsDmaDesc    *hw_ring;
    /**
      * For ring mgmt
     */
    a_uint32_t          tail; /* dequeue*/
    a_uint32_t          head; /* enqueue*/
    spinlock_t          lock; /* lock for this ring */
    a_uint32_t          num_desc;
}pci_dma_softc_t;

#define PCI_DMA_MAPPING         1

#define ring_incr(_val, _lim)       ((_val) + 1)&((_lim) - 1)
#define ring_tx_incr(_val)          ring_incr((_val), HIF_PCI_MAX_TX_DESC)
#define ring_rx_incr(_val)          ring_incr((_val), HIF_PCI_MAX_RX_DESC)

//#define RING_MAX                    HIF_PCI_MAX_TX_DESC
#define RING_TX_MAX                 HIF_PCI_MAX_TX_DESC

//#define ring_full(head, tail)       (((head + 1) % RING_MAX) == tail ) 

#define ring_tx_full(head, tail)    (((head + 1) % RING_TX_MAX ) == tail )

#define ring_empty(head, tail)      (head == tail)

#define ring_free(_h, _t, _num)        \
            ((_h > _t) ? (_num - _h + _t) : (_t - _h))

#define ring_tx_free(_h, _t)        ring_free(_h, _t, HIF_PCI_MAX_TX_DESC)

#define hw_desc_own(hwdesc_p)       (((hwdesc_p)->status & ZM_OWN_BITS_MASK)== ZM_OWN_BITS_HW) 
#define sw_desc_own(hwdesc_p)       (((hwdesc_p)->status & ZM_OWN_BITS_MASK)== ZM_OWN_BITS_SW) 
#define hw_desc_len(hwdesc_p)       (hwdesc_p)->totalLen


struct ath_pci_ep_rx_regs {
    __u32   rx_base,
            rx_start,
            rx_burst_size,
            rx_pkt_offset,
            rx_checksum,
            rx_resrv[2],
            rx_swap,
            rx_resrv1[56];
};


struct ath_pci_ep_tx_regs {
    __u32   tx_base,
            tx_start,
            tx_intr_lim,
            tx_burst_size,
            tx_resrv[2],
            tx_swap,
            tx_resrv1[57];
};

struct ath_pci_regs {
        __u32 intr_status;
        __u32 intr_mask;
        __u32 resv[510];
        struct ath_pci_ep_rx_regs rx_regs[HIF_PCI_MAX_RXCHAINS];
        struct ath_pci_ep_tx_regs tx_regs[HIF_PCI_MAX_TXCHAINS];
};


/* pcie softc */
typedef struct pcie_softc {
    hif_os_callback_t            oscb;
    hif_callback_t               cb;
    pci_dma_softc_t              rxdma[HIF_PCI_MAX_RXCHAINS];
    pci_dma_softc_t              txdma[HIF_PCI_MAX_TXCHAINS];
    spinlock_t                   lock_irq; /**< Intr. contention*/
    void                         *context;

    struct device                *dev; /**< linux specific device pointer */
    struct platform_device       *pdev;
    struct ath_pci_regs __iomem  *reg_base ;
    struct dma_pool              *desc_dma_pool;

    unsigned int                 status_intr;
    struct tasklet_struct        hif_deferred_work ;
    struct timer_list            reset_timer ;

} pcie_softc_t ;



/* Read/write to txdma/rxdma/intr registers */
#define hif_wr_txdma(reg_base, chain, reg, val)                      \
               do {                                                  \
                   ath_reg_wr( (a_uint32_t)reg_base                  \
                                + reg_offset_txdma(chain,reg), val); \
                   ath_reg_rd( (a_uint32_t)reg_base                  \
                                + reg_offset_txdma(chain,reg));      \
               }while(0)

#define hif_wr_rxdma(reg_base, chain, reg, val)                      \
               do {                                                  \
                   ath_reg_wr( (a_uint32_t)reg_base                  \
                                + reg_offset_rxdma(chain,reg), val); \
                   ath_reg_rd( (a_uint32_t)reg_base                  \
                                + reg_offset_rxdma(chain,reg));      \
               }while(0)

#define hif_wr_intr(reg_base, reg, val)                        \
               do {                                            \
                   ath_reg_wr( (a_uint32_t)reg_base            \
                                + reg_offset_intr(reg), val);  \
                   ath_reg_rd( (a_uint32_t)reg_base            \
                                + reg_offset_intr(reg));       \
               }while(0)

#define hif_rd_intr(reg_base, reg)   \
               ath_reg_rd((a_uint32_t)reg_base + reg_offset_intr(reg))

#define hif_dma_mask_set(reg_base, mask)                   \
           ath_reg_rmw_set(                          \
               ((a_uint32_t)reg_base             \
               + reg_offset_intr(INTR_MASK)), mask)

#define hif_dma_mask_clear(reg_base, mask)                   \
           ath_reg_rmw_clear(                          \
               ((a_uint32_t)reg_base               \
               + reg_offset_intr(INTR_MASK)), mask)


/* Dma related function prototypes */
int
pci_dma_init(pcie_softc_t *sc);

int
pci_dma_deinit(pcie_softc_t *sc);


a_uint32_t
pci_dma_tail_addr(pci_dma_softc_t  *dma_q) ;


inline void
pci_dma_link_buf(struct device *osdev, zdma_swdesc_t *swdesc,
                         struct sk_buff *buf, enum dma_data_direction dir);

inline struct sk_buff *
pci_dma_unlink_buf(struct device *osdev, zdma_swdesc_t *swdesc, 
                                      enum dma_data_direction dir);

inline void
pci_zdma_mark_rdy(zdma_swdesc_t  *swdesc, a_uint16_t ctrl);


inline a_status_t
pci_dma_recv_refill(struct device *osdev, zdma_swdesc_t * swdesc,
                            a_uint32_t size);

#endif //_HIF_PCI_H_

