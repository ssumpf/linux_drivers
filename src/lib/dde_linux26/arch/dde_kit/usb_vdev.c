/*
 * \brief  DDE Linux 2.6 virtual USB device driver
 * \author Christian Helmuth
 * \author Dirk Vogt <dvogt@os.inf.tu-dresden.de>
 * \date   2009-04-20
 *
 * I would like to send big thanks to Dirk Vogt who implemented DDE USB - USB
 * core, host controller driver, and example gadgets in the TUD:OS group).
 *
 * This module is based on usb-skeleton.c by Greg Kroah-Hartman and inspired by
 * the L4Env implementation from the TUD:OS group in l4/pkg/usb/server/src/vudd.c.
 * Both files were released under the terms of the GNU General Public License
 * version 2.
 */

#include <linux/usb.h>

#include <dde_linux26/usb.h>

#include "local.h"

#define LOG(args...)                   \
	do {                               \
		if (0) fmt_check(args);        \
		printk(ESC_GREEN "usbvdev: "); \
		printk(args);                  \
		printk(ESC_END"\n");           \
	} while (0)
#define ERR(args...)                       \
	do {                                   \
		if (0) fmt_check(args);            \
		printk(ESC_RED "usbvdev: Error "); \
		printk(args);                      \
		printk(ESC_END"\n") ;              \
	} while (0)


/**
 * Structure to hold all of our device specific stuff
 */
struct vdev {
	struct usb_device    *dev;        /* the usb device for this device */
	struct usb_interface *interface;  /* the interface for this device */
	struct usb_device_id  device_id;  /* local copy of device ID */
};

/**
 * URB context
 */
struct urb_context {
	void *dev_handle;  /* device handle */
	void *urb_handle;  /* upper-layer URB handle (for call back) */
};


/**
 * Probe new device for responsibility
 *
 * We have registered ourself for all USB device IDs, so our probe function
 * will be called on connection of all devices. On return, we inform the server
 * framework via call back and tell USB core, that we care about the new
 * device. If a USB device driver registers, the server will inform us.
 */
static int vdev_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	/*
	 * We don't care about USB hubs, but they should not get here anyway.
	 */
	if ((id->match_flags == USB_DEVICE_ID_MATCH_DEV_CLASS && id->bDeviceClass == USB_CLASS_HUB)
	 || (id->match_flags == USB_DEVICE_ID_MATCH_INT_CLASS && id->bInterfaceClass == USB_CLASS_HUB))
		return -ENODEV;

	struct vdev *vdev = (struct vdev *) kmalloc(sizeof(*vdev), GFP_KERNEL);
	if (!vdev) return -ENOMEM;

	vdev->dev       = interface_to_usbdev(interface);
	vdev->interface = interface;
	vdev->device_id = *id;

	usb_set_intfdata(vdev->interface, vdev);
	usb_get_intf(vdev->interface);
	usb_get_dev(vdev->dev);

	/* inform upper layers */
	dde_linux26_usb_vdev_connect_cb(vdev);

	return 0;
}


static void vdev_disconnect(struct usb_interface *interface)
{
	struct vdev *vdev = (struct vdev *) usb_get_intfdata(interface);
	struct usb_device *dev = interface_to_usbdev(interface);

	/* inform upper layers */
	dde_linux26_usb_vdev_disconnect_cb(vdev);

	kfree(vdev);
	usb_set_intfdata(interface, NULL);
	usb_put_dev(dev);
	usb_put_intf(interface);
}


/**
 * Table of devices that work with this driver
 */
static struct usb_device_id vdev_id_table [] = {
	{ .driver_info = 1 }, /* match all IDs */
	{ }                   /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, vdev_id_table);


static struct usb_driver vdev_driver = {
	.name       = "usbvdev",
	.probe      = vdev_probe,
	.disconnect = vdev_disconnect,
	.id_table   = vdev_id_table,
};


static int __init vdev_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&vdev_driver);
	if (result)
		err("usb_register failed. Error number %d", result);

	return result;
}


static void __exit vdev_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&vdev_driver);
}

module_init(vdev_init);
module_exit(vdev_exit);

MODULE_AUTHOR ("Christian Helmuth, Dirk Vogt");
MODULE_DESCRIPTION("virtual USB device");
MODULE_LICENSE("GPL");


/*********************************
 ** DDE Linux 2.6 USB interface **
 *********************************/

static void urb_completion_handler(struct urb *urb)
{
#if 1
	struct urb_context *ctx = urb->context;

	LOG("URB %p completed", ctx->urb_handle);

	dde_linux26_usb_vdev_urb_complete_cb(ctx->dev_handle,
	                                     ctx->urb_handle);

	kfree(urb->context);
	usb_free_urb(urb);
#endif
}


void dde_linux26_usb_vdev_submit_control_urb(void *dev_handle,
                                             int endpoint,
                                             int direction_input,
                                             void *urb_handle,
                                             dde_kit_size_t data_size,
                                             void *data)
{
	int ret;
	struct urb *urb;
	struct usb_device *dev = ((struct vdev *)dev_handle)->dev;

	unsigned int pipe = 0;
	int target_endp;
	int target_dir;
	struct usb_ctrlrequest * req = NULL;

//	mutex_lock(&client->lock);
//	ddev= client->dev[d_urb->dev_id];
//	mutex_unlock(&client->lock);

	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) return;

	struct urb_context *context;
	context = (struct urb_context *)kmalloc(sizeof(*context), GFP_KERNEL);
	if (!context) {
		usb_free_urb(urb);
		return;
	}
	context->dev_handle = dev_handle;
	context->urb_handle = urb_handle;

	/*
	 * set up the common URB data
	 */
	urb->dev                     = dev;
	urb->transfer_flags          = 0;
	urb->transfer_buffer         = NULL;
	urb->setup_packet            = NULL;
	urb->transfer_buffer_length  = 0;
	urb->interval                = 0;
	urb->context                 = context;
	urb->complete                = urb_completion_handler;
	urb->start_frame             = 0;
	urb->number_of_packets       = 0;
	urb->actual_length           = 0;

	/* there are three control messages we have to be aware of:*
	 * - set interface (because we have to activate the new    *
	 *   altsetting also on our side)                          *
	 * - clear halt                                            *
	 * -                                                       *
	 * this two calls are blocking so we have to put them      *
	 * into a workqueue.                                       */

	/*
	 * look what kind of ctl-msg it is
	 */
	req = (struct usb_ctrlrequest *) data;

	/*
	 * is it a set interface command? 
	 */
	if ((req->bRequest == USB_REQ_SET_INTERFACE) &&
	 (req->bRequestType == USB_RECIP_INTERFACE)) {
		/*
		 * if so, we have to schedule the setinterface work of this device 
		 */
//		__u16 alternate;
//		__u16 interface;
//		ddev->alternate = le16_to_cpu(req->wValue);
//		ddev->interface = le16_to_cpu(req->wIndex);
//		ddev->urb_id=context->urb_id;
//		INIT_WORK(&ddev->work, ddeusb_set_interface_work);
//		schedule_work(&ddev->work);
//		ret=0;
//		goto DONE;
		printk("%s:%d", __func__, __LINE__);
		return;
	}

	/*
	 * is it a clear halt cmd? 
	 */
	if ((req->bRequest == USB_REQ_CLEAR_FEATURE) &&
	    (req->bRequestType == USB_RECIP_ENDPOINT) &&
	    (req->wValue == USB_ENDPOINT_HALT)) {
//		target_endp = le16_to_cpu(req->wIndex) & 0x000f;
//		target_dir = le16_to_cpu(req->wIndex) & 0x0080;
//		if (target_dir)
//			ddev->pipe=usb_rcvctrlpipe (urb->dev, target_endp);
//		else
//			ddev->pipe=usb_sndctrlpipe (urb->dev, target_endp);
//
//		ddev->urb_id=context->urb_id;
//		INIT_WORK(&ddev->work,ddeusb_clear_halt_work);
//		schedule_work(&ddev->work);
//		ret=0;
//		goto DONE;
		printk("%s:%d", __func__, __LINE__);
		return;
	}

	/*
	 * We have to send the URB to the device...
	 */
	if (direction_input)
		urb->pipe = usb_rcvctrlpipe(dev, endpoint);
	else
		urb->pipe = usb_sndctrlpipe(dev, endpoint);

	printk("devpath: %s", dev->devpath);
	printk("%p\n", dev);
	printk("bus: %p\n", dev->bus);
	printk("controller: %p\n", dev->bus->controller);
	printk("%p\n", dev->bus->controller->dma_mask);

	urb->setup_packet    = usb_buffer_alloc(dev, 8, GFP_KERNEL, &urb->setup_dma);
	urb->transfer_flags |= URB_NO_SETUP_DMA_MAP;

	memcpy(urb->setup_packet, data, 8);

	/*
	 * finally submit the URB
	 */

	ret = usb_submit_urb(urb, GFP_KERNEL);
	if (ret) {
		LOG("error submiting urb: %d\n", ret);
		return;
		/* XXX free allocations? */
	}

	LOG("URB %p submitted", urb_handle);
}
