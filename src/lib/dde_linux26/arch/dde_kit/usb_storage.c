/*
 * \brief  DDE Linux 2.6 USB storage
 * \author Sebastian Sumpf <sebastian.sumpf@genode-labs.com>
 * \date   2009-11-17
 */

#include <dde_linux26/block.h>

#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>

#include "local.h"

enum {
	MAX_USB_INDEX = 4
};

struct usb_stor
{
	unsigned int block_size;
	unsigned long block_count;
	struct scsi_device *sdev;
};

static struct usb_stor usb_devices[MAX_USB_INDEX];
static int init = 0;

dde_linux26_block_plugin_cb current_plugin_callback = NULL;

static void dde_linux26_plugin_rx_callback(int usb_index)
{
	if (current_plugin_callback)
		current_plugin_callback(usb_index);
}

dde_linux26_block_plugin_cb dde_linux26_block_register_plugin_callback(dde_linux26_block_plugin_cb cb)
{
	dde_linux26_block_plugin_cb old = current_plugin_callback;

	current_plugin_callback = cb;

	return old;
}

static void scsi_done(struct scsi_cmnd *cmnd)
{
	complete(cmnd->back);
}

static void capacity(int usb_index)
{
	struct scsi_device *sdev;
	struct scsi_cmnd *cmnd;
	void *result;
	struct completion compl;

	sdev = usb_devices[usb_index].sdev;
	cmnd = (struct scsi_cmnd *) kmalloc(sizeof(struct scsi_cmnd), GFP_KERNEL);
	result = kmalloc(8, GFP_KERNEL);

	memset(cmnd->cmnd, 0, MAX_COMMAND_SIZE);
	cmnd->cmnd[0] = READ_CAPACITY;
	cmnd->cmd_len = 10;
	cmnd->request_buffer = result;
	cmnd->request_bufflen = 8;
	cmnd->device = sdev;
	cmnd->sc_data_direction = DMA_FROM_DEVICE;

	init_completion(&compl);
	cmnd->back = &compl;

	sdev->host->hostt->queuecommand(cmnd, scsi_done);
	wait_for_completion(&compl);

	usb_devices[usb_index].block_count = be32_to_cpu(*(__be32*)result);
	usb_devices[usb_index].block_size = be32_to_cpu(*(__be32*)(result + 4));

	/* if device returns the highest block number */
	if (!sdev->fix_capacity)
		usb_devices[usb_index].block_count++;

	DEBUG_MSG("block size: %u (0x%x) block count: %lu (0x%lx)", usb_devices[usb_index].block_size,
	                                                            usb_devices[usb_index].block_size,
	                                                            usb_devices[usb_index].block_count,
	                                                            usb_devices[usb_index].block_count);

	kfree(cmnd);
	kfree(result);
}

int dde_linux26_block_dev_add(struct scsi_device *sdev)
{
	int i;

	if (!init) {
		memset(usb_devices, 0, MAX_USB_INDEX * sizeof(struct usb_stor));
		init = 1;
	}

	for (i = 0; i < MAX_USB_INDEX; i++) {
		if (!usb_devices[i].sdev) {
			usb_devices[i].sdev = sdev;
			break;
		}
	}

	if (i == MAX_USB_INDEX) {
		DEBUG_MSG("Failed to add USB device %p", sdev);
		return -EBLK_NOMEM;
	}

	capacity(i);
	dde_linux26_plugin_rx_callback(i);
	return 0;
}

int dde_linux26_block_count(int usb_index)
{
	if (!dde_linux26_block_present(usb_index))
		return -EBLK_NODEV;

	return usb_devices[usb_index].block_count;
}

int dde_linux26_block_size(usb_index)
{
	if (!dde_linux26_block_present(usb_index))
		return -EBLK_NODEV;

	return usb_devices[usb_index].block_size;
}

static int block_io(int usb_index, unsigned long block_nr, void *buffer, char opcode,
             enum dma_data_direction sc_data_direction)
{
	struct scsi_cmnd *cmnd;
	struct completion compl;
	__be32 be_block_nr;

	if (!dde_linux26_block_present(usb_index))
		return -EBLK_NODEV;

	if (block_nr > usb_devices[usb_index].block_count)
		return -EBLK_FAULT;

	cmnd = (struct scsi_cmnd *) kmalloc(sizeof(struct scsi_cmnd), GFP_KERNEL);
	memset(cmnd->cmnd, 0, MAX_COMMAND_SIZE);

	cmnd->cmnd[0] = opcode;
	cmnd->request_bufflen = usb_devices[usb_index].block_size;
	cmnd->request_buffer = buffer;
	cmnd->cmd_len = 10;
	cmnd->device = usb_devices[usb_index].sdev;
	be_block_nr = cpu_to_be32(block_nr);
	memcpy(&cmnd->cmnd[2], &be_block_nr, 4);
	cmnd->cmnd[8] = 1;
	cmnd->sc_data_direction = sc_data_direction;

	init_completion(&compl);
	cmnd->back = &compl;

	if (usb_devices[usb_index].sdev->host->hostt->queuecommand(cmnd, scsi_done)) {
		kfree(cmnd);
		return -EBLK_BUSY;
	}
	wait_for_completion(&compl);
	kfree(cmnd);

	return 0;
}


int dde_linux26_block_read(int usb_index, unsigned long block_nr, void *buffer)
{
	return block_io(usb_index, block_nr, buffer, READ_10, DMA_FROM_DEVICE);
}


int dde_linux26_block_write(int usb_index, unsigned long block_nr, void *buffer)
{
	return block_io(usb_index, block_nr, buffer, WRITE_10, DMA_TO_DEVICE);
}

int dde_linux26_block_present(int usb_index)
{
	if (usb_index < 0 && usb_index >= MAX_USB_INDEX)
		return 0;

	return (usb_devices[usb_index].sdev != NULL);
}

void * dde_linux26_block_malloc(unsigned long size)
{
	return kmalloc(size, GFP_KERNEL);
}

void dde_linux26_block_free(void *ptr)
{
	kfree(ptr);
}
