#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/kernel.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>

struct net_device* demo_nic;

static int demo_nic_open(struct net_device *dev) {
    pr_info("mai_practice5: device opened\n");
    netif_start_queue(dev);
    return 0;
}

static int demo_nic_stop(struct net_device *dev) {
    pr_info("mai_practice5: device stopped\n");
    netif_stop_queue(dev);
    return 0;
}

static netdev_tx_t demo_nic_xmit(struct sk_buff *skb, struct net_device *dev) {
    pr_info("mai_practice5: hard_start_xmit called\n");

    // Печатаем MAC-адреса и длину пакета
    pr_info("src: %pM -> dst: %pM, len: %d\n",
            eth_hdr(skb)->h_source, eth_hdr(skb)->h_dest, skb->len);

    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;

    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static int demo_nic_init(struct net_device *dev) {
    pr_info("mai_practice5: device initialized\n");
    return 0;
}

static const struct net_device_ops demo_nic_netdev_ops = {
    .ndo_init = demo_nic_init,
    .ndo_open = demo_nic_open,
    .ndo_stop = demo_nic_stop,
    .ndo_start_xmit = demo_nic_xmit,
};

static void demo_nic_setup(struct net_device *dev) {
    ether_setup(dev);
    dev->netdev_ops = &demo_nic_netdev_ops;
    dev->flags |= IFF_NOARP;
    dev->features |= NETIF_F_HW_CSUM;
}

static int __init demo_nic_module_init(void) {
    int result;

    demo_nic = alloc_netdev(0, "maidev%d", NET_NAME_UNKNOWN, demo_nic_setup);
    if (!demo_nic)
        return -ENOMEM;

    if ((result = register_netdev(demo_nic))) {
        pr_err("mai_practice5: error %d registering device\n", result);
        free_netdev(demo_nic);
        return result;
    }

    pr_info("mai_practice5: module loaded\n");
    return 0;
}

static void __exit demo_nic_module_exit(void) {
    pr_info("mai_practice5: cleaning up module\n");
    unregister_netdev(demo_nic);
    free_netdev(demo_nic);
}

module_init(demo_nic_module_init);
module_exit(demo_nic_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student MAI");
MODULE_DESCRIPTION("MAI Practice 5 - network driver stub");
