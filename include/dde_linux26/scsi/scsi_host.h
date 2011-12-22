#ifndef _SCSI_SCSI_HOST_H
#define _SCSI_SCSI_HOST_H

#include <linux/device.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>

//struct request_queue;
//struct block_device;
//struct completion;
//struct module;
struct scsi_cmnd;
struct scsi_device;
struct scsi_target;
struct Scsi_Host;
//struct scsi_host_cmd_pool;
//struct scsi_transport_template;
//struct blk_queue_tags;


/*
 * The various choices mean:
 * NONE: Self evident.	Host adapter is not capable of scatter-gather.
 * ALL:	 Means that the host adapter module can do scatter-gather,
 *	 and that there is no limit to the size of the table to which
 *	 we scatter/gather data.
 * Anything else:  Indicates the maximum number of chains that can be
 *	 used in one scatter-gather request.
 */
#define SG_NONE 0
#define SG_ALL 0xff



struct scsi_host_template {
	struct module *module;
	const char *name;

	const char *(* info)(struct Scsi_Host *);
	int (* queuecommand)(struct scsi_cmnd *,
			     void (*done)(struct scsi_cmnd *));
	int (* eh_abort_handler)(struct scsi_cmnd *);
	int (* eh_device_reset_handler)(struct scsi_cmnd *);
	int (* eh_bus_reset_handler)(struct scsi_cmnd *);
//	int (* eh_host_reset_handler)(struct scsi_cmnd *);
	int (* slave_alloc)(struct scsi_device *);
	int (* slave_configure)(struct scsi_device *);
	int (*proc_info)(struct Scsi_Host *, char *, char **, off_t, int, int);
	char *proc_name;
	int can_queue;
	int this_id;
	unsigned short sg_tablesize;
	unsigned short max_sectors;
#define SCSI_DEFAULT_MAX_SECTORS	1024
	short cmd_per_lun;

	unsigned use_clustering:1;
	unsigned emulated:1;
	unsigned skip_settle_delay:1;

	struct device_attribute **sdev_attrs;
};

struct Scsi_Host {
	spinlock_t *host_lock;
	spinlock_t  default_lock;

	struct scsi_host_template *hostt;
	unsigned int max_id;

	unsigned short host_no;  /* Used for IOCTL_GET_IDLUN, /proc/scsi et al. */
	/*
	 * We should ensure that this is aligned, both for better performance
	 * and also because some compilers (m68k) don't automatically force
	 * alignment to a long boundary.
	 */
	unsigned long hostdata[0]  /* Used for storage of host specific stuff */
		__attribute__ ((aligned (sizeof(unsigned long))));
};

extern struct Scsi_Host *scsi_host_alloc(struct scsi_host_template *, int);
extern int __must_check scsi_add_host(struct Scsi_Host *, struct device *);
extern void scsi_scan_host(struct Scsi_Host *);
//extern void scsi_rescan_device(struct device *);
extern void scsi_remove_host(struct Scsi_Host *);
extern struct Scsi_Host *scsi_host_get(struct Scsi_Host *);
extern void scsi_host_put(struct Scsi_Host *t);
//extern struct Scsi_Host *scsi_host_lookup(unsigned short);
//extern const char *scsi_host_state_name(enum scsi_host_state);

//extern u64 scsi_calculate_bounce_limit(struct Scsi_Host *);

//static inline struct device *scsi_get_device(struct Scsi_Host *shost)
//{
//        return shost->shost_gendev.parent;
//}

/**
 * scsi_host_scan_allowed - Is scanning of this host allowed
 * @shost:	Pointer to Scsi_Host.
 **/
//static inline int scsi_host_scan_allowed(struct Scsi_Host *shost)
//{
//	return shost->shost_state == SHOST_RUNNING;
//}

//extern void scsi_unblock_requests(struct Scsi_Host *);
//extern void scsi_block_requests(struct Scsi_Host *);

//struct class_container;
//
//extern struct request_queue *__scsi_alloc_queue(struct Scsi_Host *shost,
//						void (*) (struct request_queue *));
/*
 * These two functions are used to allocate and free a pseudo device
 * which will connect to the host adapter itself rather than any
 * physical device.  You must deallocate when you are done with the
 * thing.  This physical pseudo-device isn't real and won't be available
 * from any high-level drivers.
 */
//extern void scsi_free_host_dev(struct scsi_device *);
//extern struct scsi_device *scsi_get_host_dev(struct Scsi_Host *);

/* legacy interfaces */
//extern struct Scsi_Host *scsi_register(struct scsi_host_template *, int);
//extern void scsi_unregister(struct Scsi_Host *);
//extern int scsi_host_set_state(struct Scsi_Host *, enum scsi_host_state);

#endif /* _SCSI_SCSI_HOST_H */
