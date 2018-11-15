#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/hid.h>
#include <linux/input.h>
#include <linux/sysfs.h>

struct controller_data {
	struct hid_device *hdev;
	struct input_dev *input_dev;

	char *name;
};
static struct attribute *controller_attrs[] = {
	&dev_attr_name.attr,
	&dev_attr_hex.attr,
	NULL,
};

static struct attribute_group controller_attr_group = {
	.attrs = controller_attrs,
};

static int controller_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int error;
	struct controller_data *data;
	struct usb_interface *intf;
	struct usb_device *usbdev;
	struct hid_report *report;
	
	data = kzalloc(sizeof(struct controller_data), GFP_KERNEL);
	data->hdev = hdev;
	hid_set_drvdata(hdev,data);


	printk(KERN_INFO "Controller (%04X:%04X) plugged\n", id->vendor, id->product);

	intf = to_usb_interface(hdev->dev.parent);
	usbdev = interface_to_usbdev(intf);

	hid_parse(hdev);
	hid_hw_start(hdev, HID_CONNECT_DEFAULT | HID_CONNECT_HIDINPUT_FORCE);
	hdev->ll_driver->open(hdev);

	data->input_dev = input_allocate_device();

	input_set_drvdata(data->input_dev, hdev);

	input_register_device(data->input_dev);

	sysfs_create_group(&(hdev->dev.kobj), &controller_attr_group);
	
	return 0;
}

 
static void controller_remove(struct hid_device *hdev)
{
	struct controller_data *data;
	hdev->ll_driver->close(hdev);
	hid_hw_stop(hdev);
	sysfs_remove_group(&(hdev->dev.kobj), &controller_attr_group);

	data = hid_get_drvdata(hdev);

	input_unregister_device(data->input_dev);

	kfree(data->name);
	kfree(data);

	printk(KERN_INFO "Controller unplugged\n");
}

static int controller_raw_event(struct hid_device *hdev, struct hid_report *report, u8 *raw_data, int size) {
	unsigned char strH[16];
	int i,j;
	memset(strH, 0, sizeof(strH));
	for(i=0,j=0; i<strlen(raw_data);i++,j+=3)
	{
		sprintf((char*)strH+j, "%02X ", raw_data[i]);
	}
	strcpy(raw_hex, strH);
	if(raw_data[5] != 8) {
		printk(KERN_INFO "%s", strH);
	}
	return 0;
}
 
static struct hid_device_id controller_table[] =
{
	{ HID_USB_DEVICE(0x054c, 0x05c4) },
	{} /* Terminating entry */
};
MODULE_DEVICE_TABLE (hid, controller_table);
 
static struct hid_driver controller_driver =
{
	.name = "controller_driver",
	.id_table = controller_table,
	.probe = controller_probe,
	.remove = controller_remove,
	.raw_event = controller_raw_event,
};
 
static int __init controller_init(void)
{
	return hid_register_driver(&controller_driver);
}
 
static void __exit controller_exit(void)
{
	hid_unregister_driver(&controller_driver);
}
 
module_init(controller_init);
module_exit(controller_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tom Paoloni");
MODULE_DESCRIPTION("DS4 USB Driver Module");
