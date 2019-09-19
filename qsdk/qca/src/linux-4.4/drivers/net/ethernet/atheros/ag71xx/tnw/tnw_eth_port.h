#ifndef __TNW_ETH_PORT_H__
#define __TNW_ETH_PORT_H__

#include <linux/if_vlan.h>

//#include <cfg_product.h>
#define CFG_WAN_AT_P4 1
#define CFG_ETHERNET_PORT_NUM 5
#define CFG_NETWORK_WAN1_IF_NAME ""
#define CFG_NETWORK_WAN1_IF_PORT 0
#define CFG_NETWORK_LAN1_IF_NAME "eth0"
#define CFG_NETWORK_LAN1_IF_PORT 4

//#include <cfg_product_variables.h>
#define NODE_EXCHANGE_ETH_PORT                  "eth_port"

int tnw_eth_port_proc_create(void);
int tnw_eth_port_proc_remove(void);
int tnw_eth_port_change_check(char *int_face, int up);

#endif /* __TNW_ETH_PORT_H__ */
