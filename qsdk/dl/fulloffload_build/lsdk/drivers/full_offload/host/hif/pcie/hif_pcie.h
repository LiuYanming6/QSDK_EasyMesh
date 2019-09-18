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

#ifndef _HIF_PCI_H_
#define _HIF_PCI_H_

#include<adf_net.h>

/*XXX: remove the extra tabs  */
#define hif_trace(fmt, args...)                                             \
                    do {                                                    \
                            extern a_uint32_t hif_trace_mask ;              \
                            if(hif_trace_mask){                               \
                                printk("hif_pcie: %s(): ",__FUNCTION__);    \
                                printk(fmt, ##args);                        \
                                printk("\n");                               \
                            }                                               \
                        }while(0)




#define MAX_NBUF_SIZE           2000 /**< XXX: check the max packet size */
#define PCI_NBUF_ALIGNMENT      4   
/**
  * @brief PCI Standard registers
  */
enum __pci_std_regs{
    __PCI_LATENCY_TIMER      = 0x0d,
    __PCI_CACHE_LINE_SIZE    = 0x0c,
    __PCI_MAGPIE_RETRY_COUNT = 0x41
};


#define MAX_TXDESC_SHIFT        8
#define MAX_RXDESC_SHIFT        8
#define HIF_PCI_MAX_TX_DESC     (1 << MAX_TXDESC_SHIFT)
#define HIF_PCI_MAX_RX_DESC     (1 << MAX_RXDESC_SHIFT)
#define HIF_PCI_MAX_RXCHAINS    2
#define HIF_PCI_MAX_TXCHAINS    4


#define ATH_VID                 0x168c /* Vendor ID*/
#define ATH_WASP_PCI            0x0034 /* Wasp PCI Device ID  */
#define ATH_MERLIN_PCI          0x002a /* Merlin Device ID*/
#define ATH_MAGPIE_PCI          0xff1d /* Magpie PCI Device ID  */
#define ATH_MAGPIE_PCIE         0xff1c /* Magpie PCIE Device ID */
#define ATH_OWL_PCI             0x0024 /* Owl Device ID */
#define ATH_MERLIN_PCI          0x002a /* Merlin Device ID*/
#define ATH_WASP_PCI_OTHER      0x0031 /* Wasp Device ID */
#define ATH_SCORPION_PCI        0x0039 /* Scorpion Device ID*/


/*
 * Register definitions
 * Verify register offsets for multiple chips
 */

/* 
 * Target PCIe BAR 0 
 * register spaces start offsets 
 */
#define DMA_INTR_START          0x0
#define DMA_RX_0_START          0x800
#define DMA_RX_1_START          0x900
#define DMA_TX_0_START          0xC00
#define DMA_TX_1_START          0xD00
#define DMA_TX_2_START          0xE00
#define DMA_TX_3_START          0xF00
#define EEPROM_START            0x2000
#define HOST_IF_START           0x4000
#define PCIE_CNTRL_START        0x5000

/*XXX: remove the duplicate code  */
/*
 * Individual PCIe BAR 0 register names 
 * and their offsets within respective 
 * address spaces (above).
 */

/* Interrupt registers */
#define INTERRUPT_REG           0x0
#define INTERRUPT_MASK          0x4
#define INTERRUPT_PRIORITY      0x8

/* 
 * HOST DMA Rx Chain 
 * Register offsets 
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
    * HOST DMA Tx Chain 
     * Register offsets 
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
 * rx dma register of a particular chain (0,1).
 */
#define reg_offset_rxdma(chain,rx_reg)  \
                ( DMA_RX_0_START + (chain*0x100) + rx_reg)
/* 
 * Use this macro and TX_* above 
 * to get absolute offset for 
 * tx dma register of a particular chain (0,1,2,3).
 */
#define reg_offset_txdma(chain,tx_reg)  \
                ( DMA_TX_0_START + (chain*0x100) + tx_reg)



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
  * @brief Enable or Disable Byte / Descriptor swapping
  */
enum __dma_byte_swap{
    DMA_BYTE_SWAP_OFF = 0x00,
    DMA_BYTE_SWAP_ON  = 0x01,
    DMA_DESC_SWAP_ON  = 0x02 /* Required for Big Endian Host*/
};


typedef enum __hostif_reg{
    __HOSTIF_REG_AHB_RESET     = 0x4000,/*AHB Reset reg*/
    __HOSTIF_REG_AHB_MODE      = 0x4024,/*AHB Mode reg*/
    __HOSTIF_REG_INTR_CLR      = 0x4028,/*Interrupt Status Reg,to clr */ 
    __HOSTIF_REG_INTR_STATUS   = 0x4028,/*Interrupt Status Reg */
    __HOSTIF_REG_INTR_ENB      = 0x402c,/*Interrupt Enable Reg */
    __HOSTIF_REG_INTR_MSK      = 0x4034,/*Interrupt Mask Reg */
}__hostif_reg_t;


/**
  * @brief AHB Mode register values
  */
enum __pci_ahb_mode{
    __AHB_MODE_WRITE_EXACT     = (0x00 << 0),
    __AHB_MODE_WRITE_BUFFER    = (0x01 << 0),
    __AHB_MODE_READ_EXACT      = (0x00 << 1),
    __AHB_MODE_READ_CACHE_LINE = (0x01 << 1),
    __AHB_MODE_READ_PRE_FETCH  = (0x02 << 1),
    __AHB_MODE_PAGE_SIZE_4K    = 0x18,
    __AHB_MODE_CUST_BURST      = 0x40
};



/**
  *@brief DMA registers
  */
typedef enum __dma_reg_off {
    /**
      * Common for all Engines
     */
    __DMA_REG_ISR      = 0x00,/* Interrupt Status Register */
    __DMA_REG_IMR      = 0x04,/* Interrupt Mask Register */
    __DMA_REG_PRIO     = 0x08,/* DMA Engine Priority Register*/
    /**
      * Transmit
      */
    __DMA_REG_TXDESC   = 0x00,/* TX DP */
    __DMA_REG_TXSTART  = 0x04,/* TX start */
    __DMA_REG_INTRLIM  = 0x08,/* TX Interrupt limit */
    __DMA_REG_TXBURST  = 0x0c,/* TX Burst Size */
    __DMA_REG_TXSWAP   = 0x18,/* TX swap */
    /**
     * Receive
     */
    __DMA_REG_RXDESC   = 0x00,/* RX DP */
    __DMA_REG_RXSTART  = 0x04,/* RX Start */
    __DMA_REG_RXBURST  = 0x08,/* RX Burst Size */
    __DMA_REG_RXPKTOFF = 0x0c,/* RX Packet Offset */
    __DMA_REG_RXSWAP   = 0x1c /* RX Desc Swap */
} __dma_reg_off_t;



/**
 * @brief PCI host interrupt bits
 */
typedef enum __hostif_intr_bits{
    __HOSTIF_INTR_TGT_DMA_RST   = (1 << 0),
    __HOSTIF_INTR_DMA           = (1 << 1)
}__hostif_intr_bits_t;


/**
 * @brief Interrupt status bits
 */
typedef enum __dma_intr_bits{
    __DMA_INTR_TX3_END   = (1 << 27),/*TX3 reached the end or Under run*/
    __DMA_INTR_TX2_END   = (1 << 26),/*TX2 reached the end or Under run*/
    __DMA_INTR_TX1_END   = (1 << 25),/*TX1 reached the end or Under run*/
    __DMA_INTR_TX0_END   = (1 << 24),/*TX0 reached the end or Under run*/
    __DMA_INTR_TX3_DONE  = (1 << 19),/*TX3 has transmitted a packet*/
    __DMA_INTR_TX2_DONE  = (1 << 18),/*TX2 has transmitted a packet*/
    __DMA_INTR_TX1_DONE  = (1 << 17),/*TX1 has transmitted a packet*/
    __DMA_INTR_TX0_DONE  = (1 << 16),/*TX0 has transmitted a packet*/
    __DMA_INTR_RX1_END   = (1 << 9), /*RX1 reached the end or Under run*/
    __DMA_INTR_RX0_END   = (1 << 8), /*RX0 reached the end or Under run*/
    __DMA_INTR_RX1_DONE  = (1 << 1), /*RX1 received a packet*/
    __DMA_INTR_RX0_DONE  = 1,        /*RX0 received a packet*/
}__dma_intr_bits_t;


/**
 * @brief DMA Engine
 * Note: H/W priority order ( n+1 > n > n-1 > n-2)
 */
typedef enum __dma_eng{
    __DMA_ENGINE_RX0 = 0,
    __DMA_ENGINE_RX1 = 1,
    __DMA_ENGINE_TX0 = 2,
    __DMA_ENGINE_TX1 = 3,
    __DMA_ENGINE_TX2 = 4,
    __DMA_ENGINE_TX3 = 5,
    __DMA_ENGINE_MAX
}__dma_eng_t;


#define tx_eng_num(x)                (x - __DMA_ENGINE_TX0)

/**
  * Register Values for DMA related operation
  */ 
typedef enum __dma_reg_val{
    __DMA_REG_SET_TXCTRL = 0x01,/*TX Start bit*/
    __DMA_REG_SET_RXCTRL = 0x01 /*RX Start bit*/
}__dma_reg_val_t;    


/**
  * @brief AHB reset register
  */
typedef enum __hostif_ahb_reset {
    __HOSTIF_RESET_ALL      = (1 << 16),
    __HOSTIF_RESET_HST_DMA  = (1 << 24),
    __HOSTIF_RESET_TGT_DMA  = (1 << 25)
} __hostif_ahb_reset;




/** 
 * @brief Structure representing a h/w
 *        dma descriptor
 */
struct zsDmaDesc{
#if defined (ADF_BIG_ENDIAN_MACHINE)
    a_uint16_t      ctrl;       // Descriptor control
    a_uint16_t      status;     // Descriptor status
    a_uint16_t      totalLen;   // Total length
    a_uint16_t      dataSize;   // Data size
#elif defined (ADF_LITTLE_ENDIAN_MACHINE)
    a_uint16_t      status;     // Descriptor status
    a_uint16_t      ctrl;       // Descriptor control
    a_uint16_t      dataSize;   // Data size
    a_uint16_t      totalLen;   // Total length
#else
#error "Endianess unknown, Fix me"
#endif
    struct zsDmaDesc*        lastAddr;   // Last address of this chain
    a_uint32_t               dataAddr;   // Data buffer address
    struct zsDmaDesc*        nextAddr;   // Next TD address
    a_uint8_t                pad[12];   /* Pad for 32 byte Cache Alignment*/
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
    a_uint8_t               *buf_addr;
    a_uint32_t               buf_size;
    adf_nbuf_t               nbuf;
    adf_os_dma_map_t         nbuf_map;
    adf_os_dma_addr_t        hwaddr;
    struct zsDmaDesc        *descp;
}zdma_swdesc_t;


/** 
  * @brief Context for one Dma ring.
  */
typedef struct pci_dma_softc{
    adf_os_dma_map_t     dmap;
    zdma_swdesc_t       *sw_ring;
    struct zsDmaDesc    *hw_ring;
    /**
      * For ring mgmt
     */
    a_uint32_t           tail;/* dequeue*/
    a_uint32_t           head;/* enqueue*/
    a_uint32_t           num_desc;
}pci_dma_softc_t;

#define PCI_DMA_MAPPING         1

#define ring_incr(_val, _lim)       ((_val) + 1)&((_lim) - 1)
#define ring_tx_incr(_val)          ring_incr((_val), HIF_PCI_MAX_TX_DESC)
#define ring_rx_incr(_val)          ring_incr((_val), HIF_PCI_MAX_RX_DESC)

#define RING_MAX                    HIF_PCI_MAX_TX_DESC
#define RING_TX_MAX                 HIF_PCI_MAX_TX_DESC

#define ring_full(head, tail)       (((head + 1) % RING_MAX ) == tail ) 
#define ring_tx_full(head, tail)    (((head + 1) % RING_TX_MAX ) == tail ) 

#define ring_empty(head, tail)      (head == tail)

#define ring_free(_h, _t, _num)        \
            ((_h > _t) ? (_num - _h + _t) : (_t - _h))

#define ring_tx_free(_h, _t)        ring_free(_h, _t, HIF_PCI_MAX_TX_DESC)

#define hw_desc_own(hwdesc_p)       ((hwdesc_p)->status == ZM_OWN_BITS_HW) 
#define hw_desc_len(hwdesc_p)       (hwdesc_p)->totalLen

/* Dma related function prototypes */
void
pci_dma_alloc_swdesc(adf_os_device_t osdev, pci_dma_softc_t *sc,
                                     a_uint32_t  num_desc) ;
a_uint32_t
pci_dma_tail_addr(pci_dma_softc_t  *dma_q);

adf_nbuf_t
pci_dma_unlink_buf(adf_os_device_t osdev, zdma_swdesc_t *swdesc) ;

void
pci_dma_link_buf(adf_os_device_t osdev, zdma_swdesc_t *swdesc,
                                 adf_nbuf_t  buf) ;

void
pci_zdma_mark_rdy(zdma_swdesc_t  *swdesc, a_uint16_t ctrl) ;

void
pci_dma_init_tx(adf_os_device_t osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc) ;

void pci_dma_deinit_tx(adf_os_device_t osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc);
void
pci_dma_init_rx(adf_os_device_t osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc, adf_os_size_t buf_size) ;

void
pci_dma_deinit_rx(adf_os_device_t osdev, pci_dma_softc_t *dma_q,
                        a_uint32_t num_desc, adf_os_size_t buf_size) ;
a_status_t
pci_dma_recv_refill(adf_os_device_t   osdev, zdma_swdesc_t     * swdesc,
                            a_uint32_t   size) ;




#endif //_HIF_PCI_H_

