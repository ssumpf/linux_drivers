#ifndef _SCSI_SCSI_DEVICE_H
#define _SCSI_SCSI_DEVICE_H

#include <linux/device.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <asm/atomic.h>

/*
struct request_queue;
struct scsi_cmnd;
struct scsi_lun;
struct scsi_sense_hdr;

struct scsi_mode_data {
	__u32	length;
	__u16	block_descriptor_length;
	__u8	medium_type;
	__u8	device_specific;
	__u8	header_length;
	__u8	longlba:1;
};*/

/*
 * sdev state: If you alter this, you also need to alter scsi_sysfs.c
 * (for the ascii descriptions) and the state model enforcer:
 * scsi_lib:scsi_device_set_state().
 */

//enum scsi_device_state {
//	SDEV_CREATED = 1,	/* device created but not added to sysfs
//				 * Only internal commands allowed (for inq) */
//	SDEV_RUNNING,		/* device properly configured
//				 * All commands allowed */
//	SDEV_CANCEL,		/* beginning to delete device
//				 * Only error handler commands allowed */
//	SDEV_DEL,		/* device deleted 
//				 * no commands allowed */
//	SDEV_QUIESCE,		/* Device quiescent.  No block commands
//				 * will be accepted, only specials (which
//				 * originate in the mid-layer) */
//	SDEV_OFFLINE,		/* Device offlined (by error handling or
//				 * user request */
//	SDEV_BLOCK,		/* Device blocked by scsi lld.  No scsi 
//				 * commands from user or midlayer should be issued
//				 * to the scsi lld. */
//};

struct scsi_device {
	struct Scsi_Host *host;
	struct request_queue *request_queue;
	unsigned int id, lun, channel;

	char type;
	char scsi_level;
	unsigned char inquiry_len;		/* valid bytes in 'inquiry' */
	struct scsi_target *sdev_target;	/* used only for single_lun */

	unsigned lockable:1;			/* Able to prevent media removal */
	unsigned use_10_for_ms:1; 		/* first try 10-byte mode sense/select */
	unsigned skip_ms_page_8:1;		/* do not use MODE SENSE page 0x08 */
	unsigned skip_ms_page_3f:1;		/* do not use MODE SENSE page 0x3f */
	unsigned use_192_bytes_for_3f:1;	/* ask for 192 bytes from page 0x3f */
	unsigned fix_capacity:1;		/* READ_CAPACITY is too high by 1 */
	unsigned retry_hwerror:1;		/* Retry HARDWARE_ERROR */

	struct device		sdev_gendev;
} __attribute__((aligned(sizeof(unsigned long))));

#define	to_scsi_device(d)	\
	container_of(d, struct scsi_device, sdev_gendev)
#define sdev_printk(prefix, sdev, fmt, a...)    \
	dev_printk(prefix, &(sdev)->sdev_gendev, fmt, ##a)

/* enum scsi_target_state {
	STARGET_RUNNING = 1,
	STARGET_DEL,
};*/

/*
 * scsi_target: representation of a scsi target, for now, this is only
 * used for single_lun devices. If no one has active IO to the target,
 * starget_sdev_user is NULL, else it points to the active sdev.
 */
struct scsi_target {
	unsigned int		pdt_1f_for_no_lun;	/* PDT = 0x1f */
						/* means no lun present */

	char			scsi_level;
//	unsigned long		starget_data[0]; /* for the transport */
//	/* starget_data must be the last element!!!! */
} __attribute__((aligned(sizeof(unsigned long))));

#endif /* _SCSI_SCSI_DEVICE_H */
