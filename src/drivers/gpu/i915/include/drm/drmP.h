/*
 * \brief  Platform interface of DRM code
 * \author Norman Feske
 * \date   2010-07-14
 */

#ifndef _DRMP_H_
#define _DRMP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* DDE Kit includes */
#include <dde_kit/panic.h>
#include <dde_kit/pci.h>

/* local includes */
#include <lx_emul.h>

/*
 * Unfortunately, DRM headers use certain C++ keywords as variable names.
 * To enable the inclusion of 'drmP.h' from C++ source codes, we have to
 * rename these identifiers.
 */
#ifdef __cplusplus
#define new     _new
#define virtual _virtual
#define private _private
#endif /* __cplusplus */

#include <drm/drm.h>
#include <drm/drm_hashtab.h>
#include <drm/drm_mm.h>
#include <drm/drm_crtc.h>

#ifdef __cplusplus
#undef virtual
#undef new
#undef private
#endif /* __cplusplus */


/*******************
 ** DRM constants **
 *******************/

enum {
	DRM_MINOR_CONTROL    = 2,
	DRM_MINOR_RENDER     = 3,
	DRM_MAGIC_HASH_ORDER = 4,
};

enum {
	DRM_AUTH          =  0x1,
	DRM_MASTER        =  0x2,
	DRM_ROOT_ONLY     =  0x4,
	DRM_CONTROL_ALLOW =  0x8,
	DRM_UNLOCKED      = 0x10,
};

enum {
	DRIVER_USE_AGP     = 0x1,
	DRIVER_REQUIRE_AGP = 0x2,
	DRIVER_HAVE_IRQ    = 0x40,
	DRIVER_IRQ_SHARED  = 0x80,
	DRIVER_GEM         = 0x1000,
	DRIVER_MODESET     = 0x2000,
};

enum { DRM_HZ = HZ };


/****************
 ** DRM macros **
 ****************/

/*
 * Type and function mappings
 */
#define DRM_IRQ_ARGS        void *arg
#define DRM_ARRAY_SIZE      ARRAY_SIZE
#define DRM_WAKEUP          wake_up
#define DRM_INIT_WAITQUEUE  init_waitqueue_head
#define DRM_AGP_KERN        struct agp_kern_info
#define DRM_AGP_MEM         struct agp_memory
#define DRM_COPY_TO_USER    copy_to_user

/*
 * Debug macros
 */
#define DRM_VERBOSE 0

#if DRM_VERBOSE
#define DRM_INFO(fmt, arg...) \
	dde_kit_printf("[" DRM_NAME ":%s] *INFO* "   fmt , __func__ , ##arg)

#define DRM_ERROR(fmt, arg...) \
	dde_kit_debug("[" DRM_NAME ":%s] *ERROR* "  fmt , __func__ , ##arg)

#define DRM_DEBUG(fmt, arg...) \
	dde_kit_printf("[" DRM_NAME ":%s] *DEBUG* "  fmt , __func__ , ##arg)

#define DRM_DEBUG_DRIVER(fmt, arg...) \
	dde_kit_printf("[" DRM_NAME ":%s] *DRIVER* " fmt , __func__ , ##arg)
#else
#define DRM_INFO(fmt, arg...)
#define DRM_ERROR(fmt, arg...)
#define DRM_DEBUG(fmt, arg...)
#define DRM_DEBUG_DRIVER(fmt, arg...)
#endif

/***************
 ** DRM types **
 ***************/

/*
 * Forward type declarations
 */
struct drm_device;
struct drm_mm_node;
struct drm_master;
struct drm_file;
struct drm_crtc;

/**
 * Ioctl handler function
 */
typedef int drm_ioctl_t(struct drm_device *dev, void *data,
                        struct drm_file *file_priv);

/**
 * Ioctl representation
 */
struct drm_ioctl_desc {
	unsigned int cmd;
	int          flags;
	drm_ioctl_t *func;
};

#define DRM_IOCTL_DEF(ioctl, _func, _flags) \
	[ioctl & 0xff] = {.cmd = ioctl, .func = _func, .flags = _flags}

struct drm_driver {
	u32 (*get_vblank_counter) (struct drm_device *dev, int crtc);
	u32 driver_features;
};

/* needed by drm_agpsupport.c */
struct drm_agp_mem {
	unsigned long handle;
	DRM_AGP_MEM *memory;
	unsigned long bound;
	int pages;
	struct list_head head;
};

struct drm_agp_head {
	DRM_AGP_KERN agp_info;
	unsigned long base;

	/*
	 * Members used by drm_agpsupport.c
	 */
	int acquired;
	struct agp_bridge_data *bridge;
	int enabled;
	unsigned long mode;
	struct list_head memory;
	int cant_use_aperture;
	unsigned long page_mask;
};

struct drm_device {
	int pci_device;
	struct pci_dev *pdev;
	struct mutex struct_mutex;
	struct drm_driver *driver;
	void *dev_private;
	struct drm_gem_mm *mm_private;
	enum drm_stat_type types[15];
	unsigned long counters;
	struct address_space *dev_mapping;
	struct drm_agp_head *agp;
	int irq_enabled; /* needed for i915_dma.c */
	spinlock_t count_lock;
	struct drm_mode_config mode_config;
	int open_count;
	int vblank_disable_allowed;
	u32 max_vblank_count;
	struct drm_minor *primary; /* needed by i915_dma.c */
	atomic_t object_count;
	atomic_t object_memory;
	atomic_t pin_count;
	atomic_t pin_memory;
	atomic_t gtt_count;
	atomic_t gtt_memory;
	uint32_t gtt_total;
	uint32_t invalidate_domains;
	uint32_t flush_domains;
};

struct drm_map_list {
	struct drm_hash_item hash;
	struct drm_local_map *map;
	struct drm_mm_node *file_offset_node;
};

struct drm_gem_object {

	/** Related drm device */
	struct drm_device *dev;

	/** File representing the shmem storage */
	struct file *filp;

	/**
	 * Size of the object, in bytes.  Immutable over the object's
	 * lifetime.
	 */
	size_t size;

	/**
	 * Global name for this object, starts at 1. 0 means unnamed.
	 * Access is covered by the object_name_lock in the related drm_device
	 */
	int name;

	/* Mapping info for this object */
	struct drm_map_list map_list;

	/**
	 * Memory domains. These monitor which caches contain read/write data
	 * related to the object. When transitioning from one set of domains
	 * to another, the driver is called to ensure that caches are suitably
	 * flushed and invalidated
	 */
	uint32_t read_domains;
	uint32_t write_domain;

	/**
	 * While validating an exec operation, the
	 * new read/write domain values are computed here.
	 * They will be transferred to the above values
	 * at the point that any cache flushing occurs
	 */
	uint32_t pending_read_domains;
	uint32_t pending_write_domain;

	void *driver_private;
};

typedef struct drm_dma_handle {
	void *vaddr;
	size_t size;
	dma_addr_t busaddr;  /* needed for i915_dma.c */
} drm_dma_handle_t;

typedef struct drm_local_map {
	size_t offset;             /* Requested physical address (0 for SAREA)*/
	unsigned long size;        /* Requested physical size (bytes) */
	enum drm_map_type type;    /* Type of memory to map */
	enum drm_map_flags flags;  /* Flags */
	void *handle;              /* User-space: "Handle" to pass to mmap() */
	                           /* Kernel-space: kernel-virtual address */
	int mtrr;                  /* MTRR slot used */
} drm_local_map_t;

struct drm_gem_mm {
	struct drm_mm offset_manager;
	struct drm_open_hash offset_hash;
};

struct drm_lock_data {
	struct drm_hw_lock *hw_lock; /* for i915_dma.c */
	struct drm_file *file_priv; /* for i915_dma.c */
};

struct drm_master {
	struct drm_lock_data lock; /* needed for i915_dma.c */
	void *driver_priv; /* needed for i915_dma.c */
};

struct drm_file {
	void *driver_priv;
	struct drm_minor *minor;  /* needed for drm_agpsupport.c */
	struct drm_master *master;  /* needed for i915_dma.c */
};

/*
 * needed for drm_agpsupport.c
 */
struct drm_minor {
	struct device kdev;  /* needed by i915_irq.c */
	struct drm_device *dev;
	struct drm_master *master; /* needed for i915_dma.c */
};

/*
 * needed for drm_crtc_helper.h, included by i915_dma.c
 */
struct drm_encoder { void *helper_private; };
struct drm_mode_set { };
struct drm_crtc { void *helper_private;};

/*
 * needed for intel_drv.h
 */
struct drm_framebuffer { };


/********************
 ** DRM prototypes **
 ********************/

#include <drm/drm_proto.h>


/******************
 ** Misc helpers **
 ******************/

/* normally found in drm_os_linux.h */
#define DRM_WAIT_ON( ret, queue, timeout, condition )		\
do {								\
	DECLARE_WAITQUEUE(entry, current);			\
	unsigned long end = jiffies + (timeout);		\
	add_wait_queue(&(queue), &entry);			\
								\
	for (;;) {						\
		__set_current_state(TASK_INTERRUPTIBLE);	\
		if (condition)					\
			break;					\
		if (time_after_eq(jiffies, end)) {		\
			ret = -EBUSY;				\
			break;					\
		}						\
		schedule_timeout((HZ/100 > 1) ? HZ/100 : 1);	\
		if (signal_pending(current)) {			\
			ret = -EINTR;				\
			break;					\
		}						\
	}							\
	__set_current_state(TASK_RUNNING);			\
	remove_wait_queue(&(queue), &entry);			\
} while (0)

/* normally found in Linux drmP.h */
#define LOCK_TEST_WITH_RETURN( dev, _file_priv )				\
do {										\
	if (!_DRM_LOCK_IS_HELD(_file_priv->master->lock.hw_lock->lock) ||	\
	    _file_priv->master->lock.file_priv != _file_priv)	{		\
		DRM_ERROR( "%s called without lock held, held  %d owner %p %p\n",\
			   __func__, _DRM_LOCK_IS_HELD(_file_priv->master->lock.hw_lock->lock),\
			   _file_priv->master->lock.file_priv, _file_priv);	\
		return -EINVAL;							\
	}									\
} while (0)

static inline int drm_core_check_feature(struct drm_device *dev, int feature) {
	return ((dev->driver->driver_features & feature) ? 1 : 0); }

static inline int drm_core_has_AGP(struct drm_device *dev) {
	return drm_core_check_feature(dev, DRIVER_USE_AGP); }

/*
 * Functions normally provided by drm_bufs.c
 */
static inline resource_size_t
drm_get_resource_start(struct drm_device *dev, unsigned int rsc) {
	return pci_resource_start(dev->pdev, rsc); }

static inline resource_size_t
drm_get_resource_len(struct drm_device *dev, unsigned int rsc) {
	return pci_resource_len(dev->pdev, rsc); }

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _DRMP_H_ */
