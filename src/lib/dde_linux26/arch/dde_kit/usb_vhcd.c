/*
 * \brief  DDE Linux 2.6 virtual USB host controller driver
 * \author Christian Helmuth
 * \author Dirk Vogt <dvogt@os.inf.tu-dresden.de>
 * \date   2009-04-20
 *
 * I would like to send big thanks to Dirk Vogt who implemented DDE USB - USB
 * core, host controller driver, and example gadgets in the TUD:OS group).
 *
 * The original implementation was in the L4Env from the TUD:OS group
 * (l4/pkg/usb/lib/libddeusb26/src/ddeusb_vhcd.c. This file was released under
 * the terms of the GNU General Public License version 2.
 */

#include <linux/platform_device.h>
#include <linux/usb.h>

#include <dde_linux26/usb.h>

#include "usb.h"
#include "hcd.h"

#include "local.h"

#define LOG(args...)                  \
	do {                              \
		if (0) fmt_check(args);       \
		printk(ESC_BLUE "usbvhcd: "); \
		printk(args);                 \
		printk(ESC_END"\n");          \
	} while (0)
#define ERR(args...)                       \
	do {                                   \
		if (0) fmt_check(args);            \
		printk(ESC_RED "usbvhcd: Error "); \
		printk(args);                      \
		printk(ESC_END"\n") ;              \
	} while (0)

static const char driver_name[] = "vhci-hcd";
static const char driver_desc[] = "virtual USB host controller";

static const char platform_device_name[] = "vhci";

/*******************************************
 ** Virtual host-controller configuration **
 *******************************************/

static __u8 vhcd_num_ports;

struct vhcd { };

/***********************
 ** Utility functions **
 ***********************/

/*
 * Convert between a usb_hcd pointer and the corresponding vhcd.
 */
static inline struct vhcd *hcd_to_vhcd(struct usb_hcd *hcd)
{
	return (struct vhcd *) (hcd->hcd_priv);
}


static inline struct usb_hcd *vhcd_to_hcd(struct vhcd *vhcd)
{
	return container_of((void *) vhcd, struct usb_hcd, hcd_priv);
}


/****************************
 ** Host-controller driver **
 ****************************/

static int vhcd_reset(struct usb_hcd *hcd)
{
	WARN_UNIMPL;
	return 0;
}


/*
 * XXX Power management is not supported yet.
 */
static int vhcd_suspend(struct usb_hcd *hcd, pm_message_t message)
{
	WARN_UNIMPL;
	return 0;
}

static int vhcd_resume(struct usb_hcd *hcd)
{
	WARN_UNIMPL;
	return 0;
}

static void vhcd_shutdown(struct usb_hcd *hcd)
{
	WARN_UNIMPL;
}

/*
 * XXX get frame number should return the current usr frame not many drivers
 * use it, so not supported yet
 */
static int vhcd_get_frame_number(struct usb_hcd *hcd)
{
	WARN_UNIMPL;
	return 0;
}


static void vhcd_endpoint_disable(struct usb_hcd *hcd, struct usb_host_endpoint *ep)
{
	WARN_UNIMPL;
}


static void vhcd_hub_irq_enable(struct usb_hcd *hcd)
{
//	WARN_UNIMPL;
}


static int vhcd_start_port_reset(struct usb_hcd *hcd, unsigned port_num)
{
	WARN_UNIMPL;
	return 0;
}


static int vhcd_bus_suspend(struct usb_hcd * hcd)
{
	WARN_UNIMPL;
	return 0;
}


static int vhcd_bus_resume(struct usb_hcd * hcd)
{
	WARN_UNIMPL;
	return 0;
}


/**
 * See USB 2.0 spec. Table 11-15 for details.
 */
static int vhcd_hub_control(struct usb_hcd *hcd,
                            u16 request, u16 wValue, u16 wIndex,
                            char *buf, u16 wLength) 
{
	if (!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags))
		return -ETIMEDOUT;

	printk("Control.. ");

	if (wIndex > vhcd_num_ports) {
		printk("wIndex too large (0x%x)", wIndex);
		return -ENODEV;
	}

	/* port 0 is special and real ports start at index 1 */
	__u8 port_num = ((__u8 ) (wIndex & 0x00ff));

	switch (request) {
	case GetHubDescriptor:
		{
			struct usb_hub_descriptor *desc = (struct usb_hub_descriptor *)buf;
			memset(desc, 0, sizeof(*desc));
			desc->bDescLength         = sizeof(*desc);
			desc->bDescriptorType     = 0x29;  /* USB 2.0 11.23.2.1 */
			desc->bNbrPorts           = vhcd_num_ports;
			desc->wHubCharacteristics = (__u16) (__constant_cpu_to_le16 (0x0001));
			memset(desc->DeviceRemovable, ~0, sizeof(desc->DeviceRemovable));
			memset(desc->PortPwrCtrlMask, ~0, sizeof(desc->PortPwrCtrlMask));
		}
		break;

	case GetHubStatus:
		printk("GetHubStatus %x %x %x", wValue, wIndex, wLength);
		memset(buf, 0, wLength);
		break;

	case SetPortFeature:
		printk("SetPortFeature %x %x %x", wValue, wIndex, wLength);
//		TODO
//		switch (wValue) {
//		case USB_PORT_FEAT_SUSPEND:
//			LOG("unimplemented hub control request: SetPortFeature USB_PORT_FEAT_SUSPEND");
//			break;
//		case USB_PORT_FEAT_RESET:
//			/* if it's already running, disconnect first */
//			if (data->port_status[port] & USB_PORT_STAT_ENABLE) {
//				data->port_status[port] &= ~(USB_PORT_STAT_ENABLE |
//				                             USB_PORT_STAT_LOW_SPEED |
//				                             USB_PORT_STAT_HIGH_SPEED);
//			}
//			/* fall through */
//		default:
//			data->port_status[port] |= (1 << wValue);
//			break;
//		}
		break;

	case ClearPortFeature:
		printk("ClearPortFeature %x %x %x", wValue, wIndex, wLength);

		switch (wValue) {
		case USB_PORT_FEAT_C_CONNECTION:
		case USB_PORT_FEAT_C_RESET:
			dde_linux26_usb_vhcd_clear_feature_cb(port_num, wValue);
			break;
		default:
			printk("unhandled feature %d", wValue);
		}
		break;

	case GetPortStatus:
		printk("GetPortStatus %x %x %x", wValue, wIndex, wLength);
		if (wIndex == 0) {
			LOG("Port 0?");
			return -EPIPE;
		}

		__u32 port_status = dde_linux26_usb_vhcd_port_status_cb(port_num);
		printk(" => %04x", port_status);

//		if ((port_status & (1 << USB_PORT_FEAT_RESET)) != 0
//		   /* && time_after (jiffies, dum->re_timeout) */) {
//			port_status |= (1 << USB_PORT_FEAT_C_RESET);
//			port_status &= ~(1 << USB_PORT_FEAT_RESET);
//
//			if (data->gadget[port].id) {
//				LOG(" enabled port %d ddeusb device id %p)\n", port, data->gadget[port].id);
//				port_status |= USB_PORT_STAT_ENABLE;
//			}
//			LOG(" enable rhport %d (status)\n", port);
//			port_status |= USB_PORT_STAT_ENABLE;
//
//		}

		*((__u32 *)buf) = port_status;
		break;

	case ClearHubFeature:
		printk("ClearHubFeature %x %x %x", wValue, wIndex, wLength);
		break;
	case SetHubFeature:
		printk("SetHubFeature %x %x %x", wValue, wIndex, wLength);
		break;
	default:
		printk("unknown request %04x", request);
	}

	printk("\n");

	return 0;
}


/*
 * Hubd is polling this funtion. It returns ports with status changes (events).
 */
static int vhcd_hub_status_data(struct usb_hcd *hcd, char *buf)
{
	/* calles too erly */
	if (!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags))
		return -ETIMEDOUT;

	__u32 changed_ports = dde_linux26_usb_vhcd_ports_changed_cb();

	if (changed_ports) {
		*((__u32 *)buf) = changed_ports;
		printk("===>  %x (%d)\n", *((__u32 *)buf), sizeof(changed_ports));
		return sizeof(changed_ports);
	}

	return 0;
}


static int vhcd_urb_enqueue(struct usb_hcd *hcd,
                            struct usb_host_endpoint *ep,
                            struct urb *urb,
                            gfp_t mem_flags)
{
	int ret = 0;
	unsigned int transfer_flags = 0 ;

	struct usb_device * udev = urb->dev;

	/* FIXME Check for non existent device */

	if (!HC_IS_RUNNING(hcd->state)) {
		LOG("HC is not running\n");
		return -ENODEV;
	}

	/* we have to trap some control messages, i.e. USB_REQ_SET_ADDRESS... */
	/* TODO we don't have to do it here, but in the server */

	if (usb_pipedevice(urb->pipe) == 0) {
		__u8 type = usb_pipetype(urb->pipe);
		struct usb_ctrlrequest *ctrlreq = (struct usb_ctrlrequest *) urb->setup_packet;

		if (type != PIPE_CONTROL || !ctrlreq ) {
			LOG("invalid request to devnum 0\n");
			ret = -EINVAL;
			goto no_need_xmit;
		}

		switch (ctrlreq->bRequest) {

		case USB_REQ_SET_ADDRESS:
			LOG("SetAddress Request (%d) to port %d\n",
			          ctrlreq->wValue, urb->dev->portnum);

			spin_lock (&urb->lock);
			if (urb->status == -EINPROGRESS) {
				/* This request is successfully completed. */
				/* If not -EINPROGRESS, possibly unlinked. */
				urb->status = 0;
			}
			spin_unlock (&urb->lock);

			goto no_need_xmit;

		case USB_REQ_GET_DESCRIPTOR:
			if (ctrlreq->wValue == (USB_DT_DEVICE << 8))
				LOG("Get_Descriptor to device 0 (get max pipe size)\n");
			goto out;

		default:
			/* NOT REACHED */
			LOG("invalid request to devnum 0 bRequest %u, wValue %u\n",
			          ctrlreq->bRequest, ctrlreq->wValue);
			ret = -EINVAL;
			goto no_need_xmit;
		}
	}

out:
	if (urb->status != -EINPROGRESS) {
		LOG("URB already unlinked!, status %d\n", urb->status);
		return urb->status;
	}

	if (usb_pipeisoc(urb->pipe)) {
		LOG("ISO URBs not supported");
		ret = -EINVAL;
		goto no_need_xmit;
	}

	urb->hcpriv = (void *) hcd_to_vhcd(hcd);
	LOG("hcpriv %p", urb->hcpriv);

	transfer_flags = urb->transfer_flags;
	usb_get_urb(urb);

#if 0
	d_urb->type              = usb_pipetype(urb->pipe);
	d_urb->dev_id            = data->gadget[urb->dev->portnum-1].id;
	d_urb->endpoint          = usb_pipeendpoint(urb->pipe);
	d_urb->direction         = 0 || usb_pipein(urb->pipe);
	d_urb->interval          = urb->interval;
	d_urb->transfer_flags    = urb->transfer_flags;
	d_urb->number_of_packets = urb->number_of_packets;
	d_urb->priv              = priv;
	d_urb->size              = urb->transfer_buffer_length; 
	d_urb->data			     = urb->transfer_buffer;
	d_urb->phys_addr	     = d_urb->data?virt_to_phys(d_urb->data):0;


	if (urb->setup_packet) {
		memcpy(d_urb->setup_packet, urb->setup_packet, 8);
	}

	/* XXX ISO ? */
//	if (urb->number_of_packets)
//		memcpy(d_urb->iso_desc, urb->iso_frame_desc, urb->number_of_packets*sizeof(struct usb_iso_packet_descriptor));

	ret = libddeusb_submit_d_urb(d_urb);
#else
	unsigned port_num = urb->dev->portnum;

	switch (usb_pipetype(urb->pipe)) {

	case PIPE_CONTROL:
		{
			struct usb_ctrlrequest *req = (struct usb_ctrlrequest *)
			                               urb->setup_packet;
			dde_linux26_usb_vhcd_submit_control_urb_cb(port_num,
			                                           usb_pipeendpoint(urb->pipe),
			                                           usb_pipein(urb->pipe),
			                                           urb, /* handle */
			                                           sizeof(*req), req);
		}
		break;

	case PIPE_INTERRUPT:
		printk(" int\n");
//	dde_linux26_usb_vhcd_submit_urb(urb->transfer_buffer,
//	                                urb->transfer_buffer_length);
		return -EINVAL;
		break;

	/* unsupported transfer types */
	case PIPE_BULK:
		printk(" bulk\n");
		return -EINVAL;
	case PIPE_ISOCHRONOUS:
		printk(" isoc\n");
		return -EINVAL;
	}
#endif

//	if (ret) {
//		LOG("URB SUBMIT FAILED (%d).",ret);
//		/* s.t. went wrong. */	
//		spin_lock_irqsave(&data->lock, flags);  
//		data->rcv_buf[i]=NULL;
//		spin_unlock_irqrestore(&data->lock, flags);
//		down(&data->rcv_buf_free);
//		kmem_cache_free(priv_cache, urb->hcpriv);
//		usb_put_urb(urb);
//		urb->status = ret;
//		urb->hcpriv = NULL;
//		libddeusb_free_d_urb(d_urb);
//		return ret;
//	}

	LOG("URB %p submitted", urb);

	return 0;

no_need_xmit:
	usb_hcd_giveback_urb(hcd, urb);
	return 0;
}


static int vhcd_urb_dequeue(struct usb_hcd *hcd, struct urb *urb)
{
	WARN_UNIMPL;
	return 0;
}

static int vhcd_start(struct usb_hcd *hcd)
{
	printk("Start..\n");

	hcd->self.uses_dma = 0;
	hcd->state = HC_STATE_RUNNING;

	return 0;
}


static struct hc_driver vhcd_driver = {
	.description      = driver_name,
	.product_desc     = driver_desc,
	
	.hcd_priv_size    = sizeof(struct vhcd),
	.flags            = HCD_USB2,

	.start            = vhcd_start,
	.urb_enqueue      = vhcd_urb_enqueue,
	.urb_dequeue      = vhcd_urb_dequeue,
	.hub_status_data  = vhcd_hub_status_data,
	.hub_control      = vhcd_hub_control,

	/* not implemented */
	.reset            = vhcd_reset,
	.suspend          = vhcd_suspend,
	.resume           = vhcd_resume,
	.shutdown         = vhcd_shutdown,
	.get_frame_number = vhcd_get_frame_number,
	.endpoint_disable = vhcd_endpoint_disable,
	.hub_irq_enable   = vhcd_hub_irq_enable,
	.start_port_reset = vhcd_start_port_reset,
	.bus_suspend      = vhcd_bus_suspend,
	.bus_resume       = vhcd_bus_resume,
};

/*********************
 ** Platform driver **
 *********************/

static int vhcd_platform_driver_remove(struct platform_device *dev)
{
	WARN_UNIMPL;
	return 0;
}


static int vhcd_platform_driver_suspend(struct platform_device *dev,
                                        pm_message_t state)
{
	WARN_UNIMPL;
	return 0;
}


static int vhcd_platform_driver_resume(struct platform_device *dev)
{
	WARN_UNIMPL;
	return 0;
}


static int vhcd_platform_driver_probe(struct platform_device *dev)
{
	/* check if it is our device... */
	if (strcmp(dev->name, platform_device_name)) {
		LOG("%s: this device \"%s\" is not my business...",
		          driver_name, dev->name);
		return -ENODEV;
	}

	/* create the hcd */
	struct usb_hcd *hcd = usb_create_hcd(&vhcd_driver, &dev->dev, dev->dev.bus_id);
	if (!hcd) {
		LOG("create hcd failed\n");
		return -ENOMEM;
	}

	/* make it known to the world! */
	return usb_add_hcd(hcd, 0, 0);
}

static struct platform_driver vhcd_platform_driver = {
	.probe   = vhcd_platform_driver_probe,
	.remove  = vhcd_platform_driver_remove,
	.suspend = vhcd_platform_driver_suspend,
	.resume  = vhcd_platform_driver_resume,
	.driver  = {
		.name = platform_device_name,
		.owner = THIS_MODULE,
	},
};


/*********************
 ** Platform device **
 *********************/

void vhcd_platform_device_release(struct device *dev)
{
	WARN_UNIMPL;
}


static struct platform_device vhcd_platform_device = {
	.name = platform_device_name,
	.id = -1,
	.dev = {
		.release = vhcd_platform_device_release,
		.dma_mask = 0,
	},
};



/**
 * Initialization
 */
static int vhcd_init(void)
{
	if (usb_disabled())
		return -ENODEV;

	/* remember number of ports - port 0 is special */
	vhcd_num_ports = dde_linux26_usb_vhcd_ports_cb() - 1;

	int ret;

	ret = platform_device_register(&vhcd_platform_device);
	if (ret < 0)
		return ret;

	ret = platform_driver_register(&vhcd_platform_driver);
	if (ret < 0) {
		platform_device_unregister(&vhcd_platform_device);
		return ret;
	}

//	/* creating cache for privs */
//	priv_cache  = kmem_cache_create("privcache", sizeof(struct ddeusb_vhcd_urbpriv),0,0,0,0);

	/* XXX I think, we do not need it - khubd should do the stuff */
//	/* we'll start the notification thread here but:
//	 * we have to be shure that this is called before any
//	 * other usb(gadget) driver, because this would lead into
//	 * deadlock */
//	kthread_run(server_thread,(void *)NULL, "vhcd_srvth");

	return 0;
}
subsys_initcall(vhcd_init);

MODULE_AUTHOR("Christian Helmuth, Dirk Vogt");
MODULE_DESCRIPTION(driver_desc);
MODULE_LICENSE("GPL");


/*********************************
 ** DDE Linux 2.6 USB interface **
 *********************************/

void dde_linux26_usb_vhcd_urb_complete(void *urb_handle,
                                       dde_kit_size_t data_size, void *data)
{
	LOG("URB %p completed", urb_handle);

	struct urb *urb = (struct urb *)urb_handle;

	LOG("urb @ %p, urb->setup_packet @ %p data_size %zd data %p hc_priv %p",
	    urb, urb->setup_packet, data_size, data, urb->hcpriv);

	/* FIXME the URB may have been unlinked */
	if (!urb) return;

	/* FIXME update URB */
//	urb->status         = irq_d_urb->status;
//	urb->actual_length  = irq_d_urb->actual_length;
//	urb->start_frame    = irq_d_urb->start_frame;
//	urb->interval       = irq_d_urb->interval;
//	urb->error_count    = irq_d_urb->error_count;
//	urb->transfer_flags = irq_d_urb->transfer_flags;

	if (urb->setup_packet)
		memcpy(urb->setup_packet, data, 8);

	/* XXX no ISOC */
//	if (urb->number_of_packets)
//		memcpy(urb->iso_frame_desc, irq_d_urb->iso_desc,
//		       urb->number_of_packets * sizeof(struct usb_iso_packet_descriptor));

	struct usb_hcd *hcd = vhcd_to_hcd(urb->hcpriv);
	urb->hcpriv = 0;
	LOG("hcd %p", hcd);

	usb_hcd_giveback_urb(hcd, urb);

	usb_put_urb(urb);
}
