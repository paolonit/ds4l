#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
 
static int controller_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "Controller (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    return 0;
}
 
static void controller_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Controller unplugged\n");
}
 
static struct usb_device_id controller_table[] =
{
    { USB_DEVICE(0x054c, 0x05c4) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, controller_table);
 
static struct usb_driver controller_driver =
{
    .name = "controller_driver",
    .id_table = controller_table,
    .probe = controller_probe,
    .disconnect = controller_disconnect,
};
 
static int __init controller_init(void)
{
    return usb_register(&controller_driver);
}
 
static void __exit controller_exit(void)
{
    usb_deregister(&controller_driver);
}
 
module_init(controller_init);
module_exit(controller_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tom Paoloni");
MODULE_DESCRIPTION("DS4 USB Driver Module");
