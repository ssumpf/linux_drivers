/* Dummy functions for power management. */

#include "local.h"
#include <linux/device.h>

int device_pm_add(struct device * dev)
{
	WARN_UNIMPL;
	return 0;
}


void device_pm_remove(struct device * dev)
{
	WARN_UNIMPL;
}
