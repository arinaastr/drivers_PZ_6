#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/pci.h>

#define DRIVER_NAME "mai_practice6_real"
#define VENDOR_ID 0x8086
#define DEVICE_ID 0x100e

static struct net_device *netdev;
static struct pci_dev *pdev;

static netdev_tx_t netdev_xmit(struct sk_buff *skb, struct net_device *dev)
{
    int i;
    
    printk(KERN_INFO DRIVER_NAME ": === REAL PACKET RECEIVED ===\n");
    printk(KERN_INFO DRIVER_NAME ": Interface: enp0s3\n");
    printk(KERN_INFO DRIVER_NAME ": Driver: e1000\n");
    printk(KERN_INFO DRIVER_NAME ": Vendor:Device: 8086:100e\n");
    printk(KERN_INFO DRIVER_NAME ": sk_buff address: %p\n", (void*)skb);
    printk(KERN_INFO DRIVER_NAME ": Packet length: %u bytes\n", skb->len);
    
    printk(KERN_INFO DRIVER_NAME ": Data (hex): ");
    for (i = 0; i < skb->len && i < 64; i++) {
        printk(KERN_CONT "%02x ", skb->data[i]);
    }
    printk(KERN_CONT "\n");
    
    printk(KERN_INFO DRIVER_NAME ": Data (ascii): ");
    for (i = 0; i < skb->len && i < 64; i++) {
        char c = skb->data[i];
        printk(KERN_CONT "%c", (c >= 32 && c < 127) ? c : '.');
    }
    printk(KERN_CONT "\n");
    
    printk(KERN_INFO DRIVER_NAME ": === END OF PACKET ===\n");
    
    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static int netdev_open(struct net_device *dev)
{
    printk(KERN_INFO DRIVER_NAME ": Real device enp0s3 opened\n");
    netif_start_queue(dev);
    return 0;
}

static int netdev_stop(struct net_device *dev)
{
    printk(KERN_INFO DRIVER_NAME ": Real device enp0s3 closed\n");
    netif_stop_queue(dev);
    return 0;
}

static const struct net_device_ops netdev_ops = {
    .ndo_open = netdev_open,
    .ndo_stop = netdev_stop,
    .ndo_start_xmit = netdev_xmit,
};

static int pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    int err;
    
    printk(KERN_INFO DRIVER_NAME ": Found Intel 82540EM network card\n");
    printk(KERN_INFO DRIVER_NAME ": Vendor: 0x%04x, Device: 0x%04x\n", 
           pdev->vendor, pdev->device);
    
    // Создаем сетевой интерфейс
    netdev = alloc_etherdev(0);
    if (!netdev) {
        printk(KERN_ERR DRIVER_NAME ": Failed to allocate net device\n");
        return -ENOMEM;
    }
    
    SET_NETDEV_DEV(netdev, &pdev->dev);
    netdev->netdev_ops = &netdev_ops;
    
    // Устанавливаем имя интерфейса как enp0s3
    strcpy(netdev->name, "enp0s3");
    
    err = register_netdev(netdev);
    if (err) {
        printk(KERN_ERR DRIVER_NAME ": Failed to register net device: %d\n", err);
        free_netdev(netdev);
        return err;
    }
    
    pci_set_drvdata(pdev, netdev);
    
    printk(KERN_INFO DRIVER_NAME ": Real network driver registered for enp0s3\n");
    printk(KERN_INFO DRIVER_NAME ": Using e1000 driver for Intel 82540EM\n");
    
    return 0;
}

static void pci_remove(struct pci_dev *pdev)
{
    struct net_device *dev = pci_get_drvdata(pdev);
    
    printk(KERN_INFO DRIVER_NAME ": Unloading real network driver\n");
    unregister_netdev(dev);
    free_netdev(dev);
}

static struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(VENDOR_ID, DEVICE_ID) },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static struct pci_driver pci_driver = {
    .name = DRIVER_NAME,
    .id_table = pci_ids,
    .probe = pci_probe,
    .remove = pci_remove,
};

static int __init real_driver_init(void)
{
    printk(KERN_INFO DRIVER_NAME ": Initializing real network driver\n");
    printk(KERN_INFO DRIVER_NAME ": Targeting Intel 82540EM (8086:100e)\n");
    return pci_register_driver(&pci_driver);
}

static void __exit real_driver_exit(void)
{
    printk(KERN_INFO DRIVER_NAME ": Unloading real network driver\n");
    pci_unregister_driver(&pci_driver);
}

module_init(real_driver_init);
module_exit(real_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Real Network Driver for Practice 6 - Intel 82540EM");
MODULE_VERSION("1.0");
