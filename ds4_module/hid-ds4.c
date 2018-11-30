#include "ds4-id.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/hid.h>
#include <linux/input.h>
#include <linux/sysfs.h>
#include <asm/uaccess.h>	/* raw_copy_to_user() */

#define DS4_NAME "Sony DualShock 4"

/*=========================*/
/*======== Globals ========*/
/*=========================*/

struct ds4_data {
	struct hid_device *hdev;
	struct input_dev *input_dev;

	char *name;
};

static struct attribute *ds4_attrs[] = {
	NULL,
};

static struct attribute_group ds4_attr_group = {
	.attrs = ds4_attrs,
};

u8 *raw_input = NULL;
static ssize_t raw_input_size = sizeof(raw_input) * 2;

static int device_file_major_number = 0;

static const char device_name[] = "DS4";

/*============================================*/
/*======== Raw Input Character Driver ========*/
/*============================================*/

/* Activated when the character driver is read. 
   Copies raw controller input to userspace. */
static ssize_t device_file_read(struct file *file_ptr
				, char __user *user_buffer
				, size_t count
				, loff_t *position)
{
	if( *position >= raw_input_size )
		return 0;

	if( *position + count > raw_input_size )
		count = raw_input_size - *position;
	
	if( raw_copy_to_user(user_buffer, raw_input + *position, count) != 0 ) {
		printk(KERN_WARNING "DS4: raw_copy_to_user() failed.");
		return -EFAULT;	
	}
	
	*position += count;
	return count;
}

/* Simple character driver file operations */
static struct file_operations simple_driver_fops = 
{
	.owner	= THIS_MODULE,
	.read	 = device_file_read,
};

/* Registers the character driver */
int register_device(void)
{
	int result = 0;
	printk( KERN_NOTICE "DS4: register_device() is called." );

	result = register_chrdev( 0, device_name, &simple_driver_fops );
	if( result < 0 )
	{
		printk( KERN_WARNING "DS4:  can\'t register character device with errorcode = %i", result );
		return result;
	}

	device_file_major_number = result;
	printk( KERN_NOTICE "DS4: registered character device with major number = %i and minor numbers 0...255"
		, device_file_major_number );

	return 0;
}

/* Unregisters the character driver */
void unregister_device(void)
{
	printk( KERN_NOTICE "DS4: unregister_device() is called" );
	if(device_file_major_number != 0)
	{
		unregister_chrdev(device_file_major_number, device_name);
	}
}

/*===================================*/
/*======== HID Kernel Module ========*/
/*===================================*/

/* Activates and initializes the controller when it is plugged in. */
static int ds4_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int error;
	struct ds4_data *data;
	struct usb_interface *intf;
	struct usb_device *usbdev;
	
	intf = to_usb_interface(hdev->dev.parent);
	usbdev = interface_to_usbdev(intf);

	data = kzalloc(sizeof(struct ds4_data), GFP_KERNEL);
	if (data == NULL) {
		dev_err(&hdev->dev, "Cannot allocate space for DS4 device attributes\n");
		error = -ENOMEM;
		goto err_no_cleanup;
	}

	data->hdev = hdev;
	hid_set_drvdata(hdev,data);


	printk(KERN_NOTICE "DS4: Controller has been plugged in\n");

	
	error = hid_parse(hdev);
	if(error) {
		dev_err(&hdev->dev, DS4_NAME " device parse failed\n");
		error = -EINVAL;
		goto err_cleanup_data;
	}

	error = hid_hw_start(hdev, HID_CONNECT_DEFAULT | HID_CONNECT_HIDINPUT_FORCE);
	if(error) {
		dev_err(&hdev->dev, DS4_NAME " hardware start failed\n");
	}

	error = hdev->ll_driver->open(hdev);
	if(error) {
		dev_err(&hdev->dev, DS4_NAME " failed to open input interrupt pipe for key and joystick events\n");
		error = -EINVAL;
		goto err_cleanup_data;
	}

	data->input_dev = input_allocate_device();
	if (data->input_dev == NULL) {
		dev_err(&hdev->dev, DS4_NAME " error initializing the input device");
		error = -ENOMEM;
		goto err_cleanup_data;
	}

	input_set_drvdata(data->input_dev, hdev);

	error = input_register_device(data->input_dev);
	if (error) {
		dev_err(&hdev->dev, DS4_NAME " error registering the input device");
		error = -EINVAL;
		goto err_cleanup_input_dev;
	}

	sysfs_create_group(&(hdev->dev.kobj), &ds4_attr_group);

	dbg_hid("DS4 activated and initialized\n");	

	return 0;

err_cleanup_input_dev:
	input_free_device(data->input_dev);

err_cleanup_data:
	kfree(data);

err_no_cleanup:
	hid_set_drvdata(hdev, NULL);
	return error;

}

/* Removes the controller when it is unplugged and frees data */
static void ds4_remove(struct hid_device *hdev)
{
	struct ds4_data *data;
	hdev->ll_driver->close(hdev);
	hid_hw_stop(hdev);
	sysfs_remove_group(&(hdev->dev.kobj), &ds4_attr_group);

	data = hid_get_drvdata(hdev);

	input_unregister_device(data->input_dev);

	kfree(data->name);
	kfree(data);

	printk(KERN_INFO "ds4 unplugged\n");
}

/* Activated when the controller receives a raw input event */
static int ds4_raw_event(struct hid_device *hdev, struct hid_report *report, u8 *raw_data, int size) {
	raw_input = raw_data;
	return 0;
}

/* HID Table. Used to choose the correct USB device */
static struct hid_device_id ds4_table[] =
{
	{ HID_USB_DEVICE(USB_VENDOR_ID_SONY, USB_DEVICE_ID_SONY_DS4) },
	{} /* Terminating entry */
};
MODULE_DEVICE_TABLE (hid, ds4_table);

/* Basic HID driver set up */
static struct hid_driver ds4_driver =
{
	.name = "hid_ds4",
	.id_table = ds4_table,
	.probe = ds4_probe,
	.remove = ds4_remove,
	.raw_event = ds4_raw_event,
};

/* Module initilize */
static int __init ds4_init(void)
{
	register_device();
	return hid_register_driver(&ds4_driver);
}

/* Module exit */
static void __exit ds4_exit(void)
{
	unregister_device();
	hid_unregister_driver(&ds4_driver);
}
 
module_init(ds4_init);
module_exit(ds4_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tom Paoloni");
MODULE_DESCRIPTION("Sony DualShock 4 HID Driver");

