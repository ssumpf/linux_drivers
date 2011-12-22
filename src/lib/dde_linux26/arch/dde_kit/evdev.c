/*
 * \brief  DDE Linux 2.6 input event device
 * \author Christian Helmuth
 * \author Dirk Vogt <dvogt@os.inf.tu-dresden.de>
 * \date   2009-04-20
 *
 * The original implementation was in the L4Env from the TUD:OS group
 * (l4/pkg/input/lib/src/l4evdev.c). This file was released under the terms of
 * the GNU General Public License version 2.
 */

#include <linux/input.h>

#include <dde_linux26/input.h>

#define EVDEV_DEVICES 16

//#define DEBUG_EVDEV 1

/**
 * Input event device structure
 */
struct evdev {
	int    exists;
	int    isopen;
	int    devn;
	char   name[16];
	struct input_handle handle;
};

/**
 * Input event device database
 */
static struct evdev evdev_devices[EVDEV_DEVICES];
#define DEVS evdev_devices

/**
 * Function called on each input event
 */
static dde_linux26_input_event_cb callback;


static void evdev_event_cb(struct input_handle *handle, unsigned int type,
                           unsigned int code, int value)
{
#if DEBUG_EVDEV
	static unsigned long count = 0;
#endif

	/* filter sound events */
	if (test_bit(EV_SND, handle->dev->evbit)) return;

	/* filter input_repeat_key() */
	if ((type == EV_KEY) && (value == 2)) return;

	/* filter EV_SYN */
	if (type == EV_SYN) return;

	/* generate arguments and call back */
	enum dde_linux26_input_event arg_type;
	unsigned arg_keycode = KEY_UNKNOWN;
	int arg_ax = 0, arg_ay = 0, arg_rx = 0, arg_ry = 0;

	switch (type) {

	case EV_KEY:
		arg_keycode = code;
		switch (value) {

		case 0:
			arg_type = EVENT_TYPE_RELEASE;
			break;

		case 1:
			arg_type = EVENT_TYPE_PRESS;
			break;

		default:
			printk("Unknown key event value %d - not handled\n", value);
			return;
		}
		break;

	case EV_ABS:
		switch (code) {

		case ABS_X:
			arg_type = EVENT_TYPE_MOTION;
			arg_ax = value;
			break;

		case ABS_Y:
			arg_type = EVENT_TYPE_MOTION;
			arg_ay = value;
			break;

		case ABS_WHEEL:
			/*
			 * XXX I do not know, how to handle this correctly. At least, this
			 * scheme works on Qemu.
			 */
			arg_type = EVENT_TYPE_WHEEL;
			arg_ry = value;
			break;

		default:
			printk("Unknown absolute event code %d - not handled\n", code);
			return;
		}
		break;

	case EV_REL:
		switch (code) {

		case REL_X:
			arg_type = EVENT_TYPE_MOTION;
			arg_rx = value;
			break;

		case REL_Y:
			arg_type = EVENT_TYPE_MOTION;
			arg_ry = value;
			break;

		case REL_HWHEEL:
			arg_type = EVENT_TYPE_WHEEL;
			arg_rx = value;
			break;

		case REL_WHEEL:
			arg_type = EVENT_TYPE_WHEEL;
			arg_ry = value;
			break;

		default:
			printk("Unknown relative event code %d - not handled\n", code);
			return;
		}
		break;

	default:
		printk("Unknown event type %d - not handled\n", type);
		return;
	}

	if (callback)
		callback(arg_type, arg_keycode, arg_ax, arg_ay, arg_rx, arg_ry);

#if DEBUG_EVDEV
	printk("event[%ld]. dev: %s, type: %d, code: %d, value: %d\n",
	       count++, handle->dev->name, type, code, value);
#endif
}


/*
 * XXX had connect/disconnect to be locked?
 */


/**
 * Callback from input subsystem if new device is connected
 */
static struct input_handle * evdev_connect(struct input_handler *handler,
                                           struct input_dev *dev, const struct input_device_id *id)
{
	struct evdev *evdev;
	int devn;

	for (devn = 0; (devn < EVDEV_DEVICES) && (DEVS[devn].exists); devn++);
	if (devn == EVDEV_DEVICES) {
		printk("Error: no more free evdev devices\n");
		return NULL;
	}

	evdev = &DEVS[devn];

	memset(evdev, 0, sizeof (struct evdev));

	sprintf(evdev->name, "event%d", devn);
	evdev->exists         = 1;
	evdev->devn           = devn;
	evdev->handle.dev     = dev;
	evdev->handle.handler = handler;
	evdev->handle.private = evdev;

	input_open_device(&evdev->handle);

	printk("connect \"%s\"\n", dev->name);

	return &evdev->handle;
}


/**
 * Callback from input subsystem if device is disconnected
 */
static void evdev_disconnect(struct input_handle *handle)
{
	struct evdev *evdev = handle->private;

	evdev->exists = 0;

	if (evdev->isopen)
		input_close_device(handle);
	else /* XXX what about pending events? */
		memset(&DEVS[evdev->devn], 0, sizeof(struct evdev));
}


static struct input_device_id evdev_ids[] = {
	{ .driver_info = 1 },  /* Matches all devices */
	{ },                   /* Terminating zero entry */
};


static struct input_handler evdev_handler = {
	.event =      evdev_event_cb,
	.connect =    evdev_connect,
	.disconnect = evdev_disconnect,
	.fops =       NULL,               /* not used */
	.minor =      0,                  /* not used */
	.name =       "evdev",
	.id_table =   evdev_ids
};


/****************************
 ** Input event device API **
 ****************************/

/**
 * Initialization
 */
int dde_linux26_input_init(dde_linux26_input_event_cb cb)
{
	if (!cb) return -EINVAL;

	callback = cb;
	input_register_handler(&evdev_handler);

	return 0;
}


/**
 * Deinitialization
 */
void dde_linux26_input_exit(void)
{
	input_unregister_handler(&evdev_handler);
}


/****************
 ** Linux glue **
 ****************/

int register_chrdev(unsigned int i, const char *s,
                    const struct file_operations *fops)
{
	return 0;
}
