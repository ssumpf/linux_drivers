/*
 * \brief  Implementation of the GPU driver interface for i915
 * \author Norman Feske
 * \date   2010-07-28
 */

/* Genode includes */
#include <base/env.h>
#include <util/string.h>
#include <base/printf.h>
#include <pci_device/client.h>
#include <util/avl_tree.h>
#include <base/sleep.h>

/* DRM includes */
#include <drm/drmP.h>
extern "C" {
#include <i915/i915_drv.h>
#include <i915_drm.h> /* for ioctl definitions, used for DMA init */
}

/* local includes */
#include "lx_emul_priv.h"
#include "driver.h"


/**********************************************
 ** Interface to Linux emulation environment **
 **********************************************/

struct Lx_pci_dev bridge_pdev;
struct Lx_pci_dev gpu_pdev;

/* probe.c */
extern "C" struct intel_device_info *probe_device_info(unsigned vendor,
                                                       unsigned device);

extern "C" int module_agp_intel_init(void);

extern struct pci_driver *get_last_registered_pci_driver();


/***************
 ** Utilities **
 ***************/

static inline void init_pci_dev_struct(struct Lx_pci_dev *pdev, Pci::Device_capability cap)
{
	Pci::Device_client device(cap);

	enum { PCI_CONFIG_IRQ = 0x3c, PCI_CONFIG_HEADER_TYPE = 0x8 };

	memset(pdev, 0, sizeof(*pdev));

	unsigned char bus = 0, slot = 0, fn = 0;
	device.bus_address(&bus, &slot, &fn);
	pdev->devfn = (slot << 3) | (fn & 0x7);

	pdev->irq = device.config_read(PCI_CONFIG_IRQ,
	                               Pci::Device::ACCESS_8BIT);

	pdev->hdr_type = device.config_read(PCI_CONFIG_HEADER_TYPE,
	                                    Pci::Device::ACCESS_8BIT);
	pdev->bus = 0;
	pdev->device = device.device_id();
	pdev->vendor = device.vendor_id();
	pdev->msi_enabled = 0;

	for (unsigned i = 0; i < DEVICE_COUNT_RESOURCE; i++) {
		Pci::Device::Resource rsc = device.resource(i);
		pdev->resource[i].start = rsc.base();
		pdev->resource[i].end   = rsc.base() + rsc.size() - 1;
	}

	pdev->cap = cap;
}


class Gem_object_handle : public Genode::Avl_node<Gem_object_handle>
{
	private:

		long _id;
		drm_gem_object *_obj;

	public:

		/**
		 * Constructor
		 */
		Gem_object_handle(long id, drm_gem_object *obj) :
			_id(id), _obj(obj) { }

		/**
		 * Accessor
		 */
		long id() { return _id; }

		/**
		 * Strict order criterion of the nodes in the tree
		 */
		bool higher(Gem_object_handle *n1) { return (n1->_id > _id); }

		/**
		 * Look up ID from tree
		 */
		Gem_object_handle *find_by_id(int id)
		{
			if (id == _id) return this;
			Gem_object_handle *n = child(id > _id);
			return n ? n->find_by_id(id) : 0;
		}

		drm_gem_object *gem_object() const { return _obj; }
};


/************************
 ** i915 driver client **
 ************************/

class Gpu_driver::Client : public drm_file
{
	private:

		/*
		 * Database of GEM buffer objects belonging to the client
		 */
		Genode::Avl_tree<Gem_object_handle> _gem_object_handles;
		long _id_cnt;

	public:

		Client() : _id_cnt(1)
		{
			/* initialize 'drm_file' structure */
			memset(static_cast<drm_file *>(this), 0, sizeof(drm_file));
		}

		Gem_object_handle *create_bo_handle(drm_gem_object *obj)
		{
			Gem_object_handle *handle = new (Genode::env()->heap())
			                                 Gem_object_handle(_id_cnt++, obj);
			_gem_object_handles.insert(handle);
			return handle;
		}

		Gem_object_handle *lookup_bo_handle(long id)
		{
			Gem_object_handle *first = _gem_object_handles.first();
			return first ? first->find_by_id(id) : 0;
		}
};


typedef Gpu_driver::Client Client;


/*****************
 ** i915 driver **
 *****************/

Pci::Device_capability I915_gpu_driver::_find_bridge()
{
	Pci::Device_capability cap = _pci.first_device();

	/*
	 * Iterate through the available PCI devices and return device found
	 * at bus 0, slot 0, and function 0.
	 */
	while (cap.valid()) {

		unsigned char bus = 0, slot = 0, fn = 0;
		Pci::Device_client(cap).bus_address(&bus, &slot, &fn);
		if (bus == 0 && slot == 0 && fn == 0)
			break;

		Pci::Device_capability next_cap = _pci.next_device(cap);
		_pci.release_device(cap);

		cap = next_cap;
	}
	return cap;
}


Pci::Device_capability I915_gpu_driver::_find_gpu()
{
	Pci::Device_capability cap = _pci.first_device();

	/*
	 * Iterate through the available PCI devices and return first supported
	 * device.
	 */
	while (cap.valid()) {

		Pci::Device_client device(cap);

		/*
		 * If the pointer returned from 'probe_device_info' is non-null, we
		 * found a supported device.
		 */
		if (probe_device_info(device.vendor_id(), device.device_id()))
			break;

		Pci::Device_capability next_cap = _pci.next_device(cap);
		_pci.release_device(cap);

		cap = next_cap;
	}
	return cap;
}


Gpu_driver::Client *I915_gpu_driver::create_client()
{
	return new (Genode::env()->heap()) Gpu_driver::Client();
}


extern struct drm_ioctl_desc i915_ioctls[];


int I915_gpu_driver::ioctl(I915_gpu_driver::Client *client, int request, void *arg)
{
	if (request < 0 || request >= i915_max_ioctl) {
		PERR("invalid request, request=%x", request);
		return -1;
	}

	return i915_ioctls[request].func(_dev, arg, client);
}


void *I915_gpu_driver::map_buffer_object(Client *client, long handle)
{
	drm_gem_object *obj = drm_gem_object_lookup(_dev, client, handle);

	if (!obj) {
		PERR("invalid buffer-object handle");
		return 0;
	}

	enum { GFP_MASK = 0 /* not looked at */ };
	int res = i915_gem_object_get_pages(obj, GFP_MASK);
	if (res) {
		PERR("i915_gem_object_get_pages failed");
		return 0;
	}

	struct drm_i915_gem_object *obj_priv = to_intel_bo(obj);
	return obj_priv->pages[0] ? obj_priv->pages[0]->virt : 0;
}


void I915_gpu_driver::unmap_buffer_object(Client *client, long handle)
{
	PWRN("not yet implemented");
}


uint16_t I915_gpu_driver::device_id()
{
	return gpu_pdev.device;
}


I915_gpu_driver::I915_gpu_driver() : _bridge_cap(_find_bridge()), _gpu_cap(_find_gpu())
{
	/*
	 * This function performs initializations normally done in 'drm_stub.c'.
	 */

	/* create and pre-initialize 'pci_dev' structure for bridge */
	init_pci_dev_struct(&bridge_pdev, _bridge_cap);

	/* create and pre-initialize 'pci_dev' structure for gpu */
	init_pci_dev_struct(&gpu_pdev, _gpu_cap);

	/*
	 * Initialize AGP subsystem
	 */
	int res = module_agp_intel_init();
	struct pci_driver *agp_driver = get_last_registered_pci_driver();
	PDBG("module_agp_intel_init returned %d, driver at %p", res, agp_driver);

	/* the second argument is not used by 'agp_intel_probe' */
	res = agp_driver->probe(&bridge_pdev, 0);

	/*
	 * Obtain Intel-specific device info flags
	 */
	struct intel_device_info *device_info = 0;
	device_info = probe_device_info(gpu_pdev.vendor, gpu_pdev.device);

	if (!device_info) {
		PERR("could not find a supported device");
		return;
	}

	/* create and pre-initialize 'drm_driver' structure */
	struct drm_driver *drv = new (Genode::env()->heap()) drm_driver();
	memset(drv, 0, sizeof(*drv));
	drv->driver_features = DRIVER_USE_AGP | DRIVER_REQUIRE_AGP |
	                       DRIVER_HAVE_IRQ | DRIVER_IRQ_SHARED | DRIVER_GEM |
	                       DRIVER_MODESET;

	/* create and pre-initialize 'drm_minor' structure */
	struct drm_minor *minor = new (Genode::env()->heap()) drm_minor();
	memset(minor, 0, sizeof(*minor));
	minor->master = 0; /* XXX master */

#if 0 /* currently not needed */
	/*
	 * 'mm_private' contains a client-local address space used for
	 * mapping GTT ranges to the client ('i915_gem_create_mmap_offset').
	 */
	struct drm_gem_mm *mm_private = new (Genode::env()->heap()) drm_gem_mm();
	memset(mm_private, 0, sizeof(*mm_private));
	enum {
		DRM_FILE_PAGE_OFFSET_START = 0x50000000 / PAGE_SIZE,
		DRM_FILE_PAGE_OFFSET_SIZE  = 0x10000000 / PAGE_SIZE
	};
	if (drm_mm_init(&mm_private->offset_manager, DRM_FILE_PAGE_OFFSET_START,
	                                             DRM_FILE_PAGE_OFFSET_SIZE)) {
	    PERR("initialization of mm_private failed");
		return;
	}
#endif

	/* create and pre-initialize 'drm_device' structure */
	_dev = new (Genode::env()->heap()) drm_device();
	memset(_dev, 0, sizeof(*_dev));
	mutex_init(&_dev->struct_mutex);
	_dev->pci_device                = gpu_pdev.device;
	_dev->pdev                      = &gpu_pdev;
	_dev->driver                    = drv;
	_dev->dev_private               = 0;
	_dev->mm_private                = 0;  /* would be needed for mmap_gtt_ioctl */
	_dev->types[0]                  = _DRM_STAT_LOCK;
	_dev->types[1]                  = _DRM_STAT_OPENS;
	_dev->types[2]                  = _DRM_STAT_CLOSES;
	_dev->types[3]                  = _DRM_STAT_IOCTLS;
	_dev->types[4]                  = _DRM_STAT_LOCKS;
	_dev->types[5]                  = _DRM_STAT_UNLOCKS;
	_dev->counters                  = 5;
	_dev->mode_config.fb_base       = 0; /* written by i915_dma.c, but not used */
	_dev->mode_config.num_connector = 0; /* needed for i915_irq */
	_dev->open_count                = 0;
	_dev->vblank_disable_allowed    = 0; /* filled out by i915_dma.c */
	_dev->max_vblank_count          = 0; /* filled out by i915_dma.c  */
	_dev->primary                   = minor; /* XXX */
	_dev->object_count              = 0;
	_dev->object_memory             = 0;
	_dev->pin_count                 = 0;
	_dev->pin_memory                = 0;
	_dev->gtt_count                 = 0;
	_dev->gtt_memory                = 0;
	_dev->gtt_total                 = 0;
	_dev->invalidate_domains        = 0;
	_dev->flush_domains             = 0;

	PDBG("call drm_agp_init");
	_dev->agp = drm_agp_init(_dev);
	if (!_dev->agp) {
		PERR("drm_agp_init failed\n");
		return;
	}

	if (drm_ctxbitmap_init(_dev) != 0) {
		PERR("drm_ctxbitmap_init failed\n");
		return;
	}

	res = i915_driver_load(_dev, (unsigned long)device_info);
	if (res) {
		PERR("i915_driver_load failed, ret=%d", res);
		return;
	}

	i915_disable_vblank(_dev, 0);
	i915_disable_vblank(_dev, 1);
};


Gpu_driver *gpu_driver()
{
	static I915_gpu_driver i915_driver_inst;
	return &i915_driver_inst;
}


/***************************
 ** GEM handle management **
 ***************************/

extern "C"
int drm_gem_handle_create(struct drm_file *filp,
                          struct drm_gem_object *obj,
                          u32 *handlep)
{
	Client *client = static_cast<Client *>(filp);

	if (!client) {
		PERR("invalid client context");
		return -1;
	}

	Gem_object_handle *handle = client->create_bo_handle(obj);
	if (!handle) {
		PERR("object handle allocation failed");
		return -2;
	}

	*handlep = handle->id();
	PDBG("allocated GEM buffer object ID %ld, size=%zd", handle->id(), obj->size);
	return 0;
}


extern "C"
struct drm_gem_object *drm_gem_object_lookup(struct drm_device *dev,
                                             struct drm_file *filp,
                                             u32 id)
{
	Client *client = static_cast<Client *>(filp);

	if (!client) {
		PERR("invalid client context, filp=%p, id=%d", filp, id);
		return 0;
	}

	Gem_object_handle *handle = client->lookup_bo_handle(id);
	if (!handle) {
		PERR("object handle lookup for id %ld failed", (long)id);
		return 0;
	}

	return handle->gem_object();
}
