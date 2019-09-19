#include "tnw_eth_port.h"

/************************************************************************************************************/
/* port status */
/************************************************************************************************************/
#include <linux/proc_fs.h>
#include "../ag71xx.h"
extern struct net_device *ag71xx_dev[5];
static void GetPortStatus(int port, int *enabled, int *link, int *rate)
{

    unsigned long int reg_val_0x00 = 0;
    unsigned long int reg_val_0x11 = 0;
    int bit15_14, bit10;
    struct ag71xx *ag = netdev_priv(ag71xx_dev[0]);

    reg_val_0x00 = mdiobus_read(ag->mii_bus, port, 0x00);
    *enabled =( (reg_val_0x00 >> 11) & 0x1 ) ? 0 : 1; // check bit 11, 0 is enabled => set enabled=1

    reg_val_0x11 = mdiobus_read(ag->mii_bus, port, 0x11);
    bit15_14 = (reg_val_0x11 >> 14) & 0x3; // get bit 15,14
    bit10 = (reg_val_0x11 >> 10) & 0x1; // get bit 10, for check link: 1/0, up/down. if down: speed is 0
    if (bit10) // check bit 0 for down/up
    {
        *link = 1; // LinkUp
        if (bit15_14 == 0) // bit 15,14: 00
            *rate = 10;
        else if (bit15_14 == 1) // bit 15,14: 01
            *rate = 100;
        else if (bit15_14 == 2) // bit 15,14: 10
            *rate = 1000;
        else
            *rate = 0;   
    }
    else
    {
        *link = 0; // LinkDown
        *rate = 0;
    }
}

static void SetPortSetting(int port, int enabled, int rate)
{
    unsigned long int reg_val_0x00 = 0;
    struct ag71xx *ag = NULL;
    struct phy_device *phydev = NULL;
    struct ethtool_cmd cmd = {0};

#if IS_ENABLED(CFG_WAN_AT_P4)
    if (port == 4)
#else
    if (port == 0)
#endif
        ag = netdev_priv(ag71xx_dev[0]);
    else
    {
        ag = netdev_priv(ag71xx_dev[1]);
        phydev = ag->phy_dev;
        if (phydev == NULL || phydev == 0 || phydev->addr == NULL || phydev->addr == 0)
            ag = netdev_priv(ag71xx_dev[0]); // TODO: port 0-3 maybe not work when you want to set port rate, but RIL-AC1200 only has port 4, cannot test !
    }

    phydev = ag->phy_dev;
    reg_val_0x00 = mdiobus_read(ag->mii_bus, port, 0x00);

    /* MII Control Register                           */
    /* bit 11 : power down,  1:power down, 0:power up */
    switch (enabled)
    {
        case 1:
            reg_val_0x00 &= ~(1 << 11); // set bit 11 to 0
            break;
        case 0:
            reg_val_0x00 |= (1 << 11); // set bit 11 to 1
            break;
        default:
            break;
    }
    switch (rate)
    {
        case 10: // 10 Mbps, fixed
            phydev->autoneg = AUTONEG_ENABLE;
            phydev->speed = SPEED_10;
            phydev->duplex = DUPLEX_FULL;
            phydev->advertising &= ~ADVERTISED_100baseT_Half;
            phydev->advertising &= ~ADVERTISED_100baseT_Full;
            phydev->advertising &= ~ADVERTISED_1000baseT_Half;
            phydev->advertising &= ~ADVERTISED_1000baseT_Full;
            phydev->advertising |= ADVERTISED_10baseT_Half;
            phydev->advertising |= ADVERTISED_10baseT_Full;
            break;
        case 100: // 100 Mbps, fixed
            phydev->autoneg = AUTONEG_ENABLE;
            phydev->speed = SPEED_100;
            phydev->duplex = DUPLEX_FULL;
            phydev->advertising &= ~ADVERTISED_10baseT_Half;
            phydev->advertising &= ~ADVERTISED_10baseT_Full;
            phydev->advertising &= ~ADVERTISED_1000baseT_Half;
            phydev->advertising &= ~ADVERTISED_1000baseT_Full;
            phydev->advertising |= ADVERTISED_100baseT_Half;
            phydev->advertising |= ADVERTISED_100baseT_Full;
	    break;
        case 1000: // 1000 Mbps, not fixed
            phydev->autoneg = AUTONEG_ENABLE;
            phydev->speed = SPEED_1000;
            phydev->duplex = DUPLEX_FULL;
            phydev->advertising &= ~ADVERTISED_10baseT_Half;
            phydev->advertising &= ~ADVERTISED_10baseT_Full;
            phydev->advertising &= ~ADVERTISED_100baseT_Half;
            phydev->advertising &= ~ADVERTISED_100baseT_Full;
            phydev->advertising |= ADVERTISED_1000baseT_Half;
            phydev->advertising |= ADVERTISED_1000baseT_Full;
	    break;
        case 0: // Auto
        default:
            phydev->autoneg = AUTONEG_ENABLE;
            phydev->speed = SPEED_1000;
            phydev->duplex = DUPLEX_FULL;
            phydev->advertising |= ADVERTISED_10baseT_Half;
            phydev->advertising |= ADVERTISED_10baseT_Full;
            phydev->advertising |= ADVERTISED_100baseT_Half;
            phydev->advertising |= ADVERTISED_100baseT_Full;
            phydev->advertising |= ADVERTISED_1000baseT_Half;
            phydev->advertising |= ADVERTISED_1000baseT_Full;
            rate = 0;
            break;
    }
    printk("Set Port %d ", port);
    (enabled) ? ( (rate == 0) ? printk("Enabled, Auto\n") : printk("Enabled, %d Mbps\n", rate) ) : ( printk("Disabled\n") );
    mdiobus_write(ag->mii_bus, port, 0x00, reg_val_0x00);
    if (enabled)
    {
        cmd.phy_address = port;
        cmd.supported = phydev->supported;
        cmd.eth_tp_mdix_ctrl = phydev->mdix;
        cmd.autoneg = phydev->autoneg;
        cmd.advertising = phydev->advertising & phydev->supported;
        cmd.duplex = phydev->duplex;
        phy_ethtool_sset(phydev, &cmd);
    }
}

static int eth_port_read_proc(struct seq_file *seq, void *v)
{
	int port;
    for (port = 0; port < CFG_ETHERNET_PORT_NUM; port++)
    {
        int enabled = -1;
        int link = -1;
        int rate = -1;
        GetPortStatus(port, &enabled, &link, &rate);
        if (link != -1 && rate != -1)
        {
            if (enabled == 1)
                (link)?( seq_printf(seq, "Port %d: LinkUp, %d Mbps\n", port, rate) ):( seq_printf(seq, "Port %d: LinkDown\n", port) );
            else if (enabled == 0)
                seq_printf(seq, "Port %d: Disabled\n", port);
            else
            {}
        }
    }
	return 0;
}

static int eth_port_write_proc(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{
    char buf_in[16] = {0};
    int port = -1;
    char str1[32] = {0};
    char str2[32] = {0};

    int enabled = -1;
    int link = -1;
    int rate = -1;

    if ( count > sizeof(buf_in) )
        count = sizeof(buf_in);

    if ( copy_from_user(buf_in, buffer, count) )
        return -EFAULT;

    /* get para */
    sscanf(buf_in, "%d %s %s", &port, str1, str2);
    if (port < 0 || port >= CFG_ETHERNET_PORT_NUM)
    {
        printk("port number invalid !\n");
        goto END;
    }

    /* set led control mode */
#if 0 // not support
    if ( !strcmp(str1, "led") )
    {
        if ( !strcmp(str2, "hw") || !strcmp(str2, "sw") || !strcmp(str2, "switch")) // hardware or switch
            SetPortLedSetting(port, 0);
        else if ( !strcmp(str2, "gpio") )
            SetPortLedSetting(port, 1);
        goto END;
    }
#endif

    if ( !strcmp(str1, "1") || !strcmp(str1, "0") )
    {
        enabled = simple_strtol(str1, NULL, 10);
        rate = simple_strtol(str2, NULL, 10);
        if (enabled == 0 || enabled == 1)
        {
            /* Write Setting */
            SetPortSetting(port, enabled, rate);
            goto END;
        }
    }

    /* Read Status for only input port number */
    GetPortStatus(port, &enabled, &link, &rate);
    if (link != -1 && rate != -1)
    {
        if (enabled == 1)
        {
            (link)?( printk("Port %d: LinkUp, %d Mbps\n", port, rate) ):( printk("Port %d: LinkDown\n", port) );
        }
        else if (enabled == 0)
        {
            printk("Port %d: Disabled\n", port);
        }
        else
        {}
    }

END:
    return count;
}

static ssize_t eth_port_open(struct inode *inode, struct file *file)
{
	return single_open(file, eth_port_read_proc, NULL);
}

static const struct file_operations tnw_eth_port_status_fops = {
	.owner 		= THIS_MODULE,
	.open       = eth_port_open,
	.read	 	= seq_read,
	.llseek	 	= seq_lseek,
	.write 		= eth_port_write_proc,
	.release 	= single_release
};

//extern struct proc_dir_entry *cli_exchange_proc_dir;
static struct proc_dir_entry *tnw_eth_port_status_proc;

int tnw_eth_port_proc_create(void)
{
#if 0
    if (cli_exchange_proc_dir == NULL)
    {
        printk("ETH_PORT, no cli_exchange directory.\n");
        return -1;
    }
#endif

    tnw_eth_port_status_proc = proc_create(NODE_EXCHANGE_ETH_PORT, 0644, NULL /*cli_exchange_proc_dir*/, &tnw_eth_port_status_fops);
    if (tnw_eth_port_status_proc == NULL)
        return -1;

    return 0;
}

int tnw_eth_port_proc_remove(void)
{
    if (tnw_eth_port_status_proc)
    {
        remove_proc_entry(NODE_EXCHANGE_ETH_PORT, NULL /*cli_exchange_proc_dir*/);
        return 0;
    }
    else
        return -1;
}

/************************************************************************************************************/
/* port change event */
/************************************************************************************************************/

#define ETH_SKB_SIZE	2048

struct eth_event {
	int port;
	char *action;
	struct sk_buff *skb;
	struct work_struct	work;
};

extern u64 uevent_next_seqnum(void);

static int eth_event_add_var(struct eth_event *event, int argv, const char *format, ...)
{
	static char buf[128];
	char *s;
	va_list args;
	int len;

	if (argv)
		return 0;

	va_start(args, format);
	len = vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);

	if (len >= sizeof(buf)) 
    {
		WARN_ON(1);
		return -ENOMEM;
	}

	s = skb_put(event->skb, len + 1);
	strcpy(s, buf);

	return 0;
}

static int eth_hotplug_fill_event(struct eth_event *event)
{
	int ret;

	ret = eth_event_add_var(event, 0, "HOME=%s", "/");
	if (ret)
		return ret;

	ret = eth_event_add_var(event, 0, "PATH=%s",
					"/sbin:/bin:/usr/sbin:/usr/bin");
	if (ret)
		return ret;

	ret = eth_event_add_var(event, 0, "SUBSYSTEM=%s", "ethernet");
	if (ret)
		return ret;

	ret = eth_event_add_var(event, 0, "ACTION=%s", event->action);
	if (ret)
		return ret;

	ret = eth_event_add_var(event, 0, "PORT=%d", event->port);
	if (ret)
		return ret;

	ret = eth_event_add_var(event, 0, "SEQNUM=%llu", uevent_next_seqnum());

	return ret;
}

static void eth_hotplug_work(struct work_struct *work)
{
	struct eth_event *event = container_of(work, struct eth_event, work);
	int ret = 0;

	event->skb = alloc_skb(ETH_SKB_SIZE, GFP_KERNEL);
	if (!event->skb)
		goto out_free_event;

	ret = eth_event_add_var(event, 0, "%s@", event->action);
	if (ret)
		goto out_free_skb;

	ret = eth_hotplug_fill_event(event);
	if (ret)
		goto out_free_skb;

	NETLINK_CB(event->skb).dst_group = 1;
	broadcast_uevent(event->skb, 0, 1, GFP_KERNEL);

 out_free_skb:
	if (ret) {
		kfree_skb(event->skb);
	}
 out_free_event:
	kfree(event);
}

int tnw_eth_port_change_check(char *int_face, int up)
{
    struct eth_event *event;
    event = kzalloc(sizeof(*event), GFP_KERNEL);
	if (!event)
		return -ENOMEM;

    if ( !strcmp(int_face, CFG_NETWORK_WAN1_IF_NAME) )
        event->port = CFG_NETWORK_WAN1_IF_PORT;
    else if ( !strcmp(int_face, CFG_NETWORK_LAN1_IF_NAME) )
        event->port = CFG_NETWORK_LAN1_IF_PORT;
    else {}
    
    event->action = up ? "up" : "down";

	INIT_WORK(&event->work, (void *)(void *)eth_hotplug_work);
	schedule_work(&event->work);

    return 0;
}

