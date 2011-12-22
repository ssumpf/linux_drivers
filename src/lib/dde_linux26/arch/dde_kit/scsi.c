/*
 * \brief   DDE Linux 2.6 SCSI emulation
 * \author  Christian Helmuth
 * \date    2009-10-29
 *
 * XXX NOTES XXX
 *
 * struct scsi_host_template
 *
 * struct scsi_host
 *
 *   host_lock used by scsi_unlock, scsi_lock
 *   max_id    used by usb_stor_report_device_reset
 *
 * struct scsi_cmnd
 *
 * functions
 *
 *   scsi_add_host
 *   scsi_host_alloc
 *   scsi_host_get
 *   scsi_host_put
 *   scsi_remove_host
 *   scsi_report_bus_reset
 *   scsi_report_device_reset
 *   scsi_scan_host
 */

#include <scsi/scsi.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <linux/blkdev.h>

#include <dde_linux26/block.h>
#include "local.h"


/***************
 ** SCSI host **
 ***************/

struct Scsi_Host *scsi_host_alloc(struct scsi_host_template *t, int priv_size)
{
	DEBUG_MSG("t=%p, priv_size=%d", t, priv_size);

	static int free = 1;

	/* XXX we not some extra space for hostdata[] */
	static char buffer[4096] __attribute__((aligned(4096)));
	static struct Scsi_Host *host = (struct Scsi_Host *)buffer;

	/* FIXME we support only one host for now */
	if (!free) return 0;
	free = 0;

	host->host_lock = &host->default_lock;
	spin_lock_init(host->host_lock);

	host->host_no = 13;
	host->max_id = 8;
	host->hostt = t;

//	rval = scsi_setup_command_freelist(shost);
//	if (rval)
//		goto fail_kfree;

//	shost->ehandler = kthread_run(scsi_error_handler, shost,
//			"scsi_eh_%d", shost->host_no);
//	if (IS_ERR(shost->ehandler)) {
//		rval = PTR_ERR(shost->ehandler);
//		goto fail_destroy_freelist;
//	}

	return host;
}


int scsi_add_host(struct Scsi_Host *host, struct device *dev)
{
	DEBUG_MSG("host=%p, dev=%p", host, dev);

	return 0;
}


void scsi_remove_host(struct Scsi_Host *host)
{
	DEBUG_MSG("host=%p", host);
}


struct Scsi_Host *scsi_host_get(struct Scsi_Host *host)
{
	WARN_UNIMPL;

	return 0;
}


void scsi_host_put(struct Scsi_Host *host)
{
	WARN_UNIMPL;
}

static void inquiry_done(struct scsi_cmnd *cmd)
{
	DEBUG_MSG("Vendor id: %s Product id: %s",
	          &(((char *)cmd->request_buffer)[8]),
	          &(((char *)cmd->request_buffer)[16]));
	complete(cmd->back);
}

static void scsi_done(struct scsi_cmnd *cmd)
{
	complete(cmd->back);
}

void scsi_scan_host(struct Scsi_Host *host)
{
	struct scsi_cmnd   *cmnd;
	struct scsi_device *sdev;
	struct scsi_target *target;
	struct completion compl;
	void *result;

	init_completion(&compl);

	cmnd = (struct scsi_cmnd *)kmalloc(sizeof(struct scsi_cmnd), GFP_KERNEL);
	sdev = (struct scsi_device *)kmalloc(sizeof(struct scsi_device), GFP_KERNEL);
	target = (struct scsi_target *)kmalloc(sizeof(struct scsi_target), GFP_KERNEL);

	/* init device */
	sdev->sdev_target = target;
	sdev->host = host;
	sdev->id  = 0;
	sdev->lun = 0;
	host->hostt->slave_alloc(sdev);
	host->hostt->slave_configure(sdev);

	/* inquiry (36 bytes for usb) */
	result = kmalloc(sdev->inquiry_len, GFP_KERNEL);
	memset(cmnd->cmnd, 0, MAX_COMMAND_SIZE);
	cmnd->cmnd[0] = INQUIRY;
	cmnd->cmnd[4] = sdev->inquiry_len;
	cmnd->device = sdev;
	cmnd->cmd_len = 6;
	cmnd->sc_data_direction = DMA_FROM_DEVICE;
	cmnd->request_buffer = result;
	cmnd->request_bufflen = sdev->inquiry_len;
	cmnd->back = &compl;

	host->hostt->queuecommand(cmnd, inquiry_done);
	wait_for_completion(&compl);

	/* if PQ and PDT are zero we have a direct access block device conntected */
	if (!((char*)result)[0] && !dde_linux26_block_dev_add(sdev))
	{}
	else {
		kfree(sdev);
		kfree(target);
	}

	kfree(cmnd);
	kfree(result);
}


/*************************
 ** SCSI event handling **
 *************************/

void scsi_report_bus_reset(struct Scsi_Host *host, int channel)
{
	WARN_UNIMPL;
}


void scsi_report_device_reset(struct Scsi_Host *host, int channel, int target)
{
	WARN_UNIMPL;
}


/*************************************
 ** A little bit block-device layer **
 *************************************/
/*
 *   blk_queue_dma_alignment
 *   blk_queue_max_sectors
 */
void blk_queue_max_sectors(struct request_queue *q, unsigned int max_sectors)
{
	WARN_UNIMPL;
}


void blk_queue_dma_alignment(struct request_queue *q, int mask)
{
	WARN_UNIMPL;
}
