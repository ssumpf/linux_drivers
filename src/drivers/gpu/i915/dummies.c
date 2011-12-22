/*
 * \brief  Dummy implementations functions used by DRM
 * \author Norman Feske
 * \date   2010-07-23
 */

#include <drmP.h>

#define DUMMIES_VERBOSE 0
#if DUMMIES_VERBOSE
#define TRACE dde_kit_printf("\033[32m%s\033[0m called, not implemented\n", __PRETTY_FUNCTION__)
#else
#define TRACE
#endif


/****************************
 ** linux/mm.h, asm/page.h **
 ****************************/

void unmap_mapping_range(struct address_space *mapping,
                         loff_t const holebegin, loff_t const holelen,
                         int even_cows) { TRACE; }
int vm_insert_pfn(struct vm_area_struct *vma, unsigned long addr,
                  unsigned long pfn) { TRACE; return 0; } 
int set_page_dirty(struct page *page) { TRACE; return 0; }
void kunmap_atomic(void *vaddr, enum km_type idx) { TRACE; }
void *kmap(struct page *page) { TRACE; return 0; }
void kunmap(struct page *page) { TRACE; }
int get_user_pages(void *tsk, struct mm_struct *mm,
                   unsigned long start, int nr_pages, int write, int force,
                   struct page **pages, struct vm_area_struct **vmas) { TRACE; return 0; }
void SetPageDirty(struct page *page) { TRACE; }
void SetPageReserved(struct page *page) { TRACE; }
void ClearPageReserved(struct page *page) { TRACE; }
void page_cache_release(struct page *page) { TRACE; }
struct page *virt_to_page(void *addr) { TRACE; return 0; }
int set_pages_uc(struct page *page, int numpages) { TRACE; return 0; }
int set_pages_wb(struct page *page, int numpages) { TRACE; return 0; }
void *page_address(struct page *page) { TRACE; return 0; }

/* refcount management - no need to implement it for now */
void get_page(struct page *page) { TRACE; }
void put_page(struct page *page) { TRACE; }


/******************
 ** linux/kref.h **
 ******************/

void kref_init(struct kref *kref) { TRACE; }
void kref_get(struct kref *kref) { TRACE; }
int kref_put(struct kref *kref, void (*release) (struct kref *kref)) { TRACE; return 0; }


/******************
 ** linux/swap.h **
 ******************/

void mark_page_accessed(struct page *p) { TRACE; }


/**********************
 ** linux/spinlock.h **
 **********************/

void spin_lock_irqrestore(spinlock_t *lock, unsigned long flags) { TRACE; }


/*******************
 ** linux/mutex.h **
 *******************/

int mutex_trylock(struct mutex *m) { TRACE; return 0; }
void down_read(struct rw_semaphore *sem) { TRACE; }
void up_read(struct rw_semaphore *sem) { TRACE; }
void down_write(struct rw_semaphore *sem) { TRACE; }
void up_write(struct rw_semaphore *sem) { TRACE; }


/*************************************
 ** Memory allocation, linux/slab.h **
 *************************************/

void *kcalloc(size_t n, size_t size, gfp_t flags) { TRACE; return 0; }



/********************
 ** linux/ioport.h **
 ********************/

int request_resource(struct resource *root, struct resource *n) { TRACE; return 0; }
int release_resource(struct resource *r) { TRACE; return 0; }


/**********************
 ** PCI, linux/pci.h **
 **********************/

int pci_bus_alloc_resource(struct pci_bus *bus,
                           struct resource *res, resource_size_t size,
                           resource_size_t align, resource_size_t min,
                           unsigned int type_mask,
                           resource_size_t (*alignf)(void *,
                                                     const struct resource *,
                                                     resource_size_t,
                                                     resource_size_t),
                           void *alignf_data) { TRACE; return 0; }
resource_size_t pcibios_align_resource(void *p, const struct resource *r,
                                       resource_size_t s1,
                                       resource_size_t s2) { TRACE; return 0; }
void *pci_get_drvdata(struct pci_dev *pdev) { TRACE; return 0; }
int pci_set_power_state(struct pci_dev *dev, pci_power_t state) { TRACE; return 0; };
int pci_enable_msi(struct pci_dev *dev) { TRACE; return -1; }
int pci_disable_msi(struct pci_dev *dev) { TRACE; return -1; }
void pci_dev_put(struct pci_dev *dev) { TRACE; } struct workqueue_struct;
int pci_set_dma_mask(struct pci_dev *dev, u64 mask) { TRACE; return 0; }
int pci_enable_device_mem(struct pci_dev *dev) { TRACE; return 0; }
int pci_assign_resource(struct pci_dev *dev, int i) { TRACE; return 0; }
int pci_set_consistent_dma_mask(struct pci_dev *dev, u64 mask) { TRACE; return 0; }
void pci_set_drvdata(struct pci_dev *pdev, void *data) { TRACE; }
void pci_unregister_driver(struct pci_driver *driver) { TRACE; }
struct pci_dev *pci_get_class(unsigned int class, struct pci_dev *from) { TRACE; return 0; }
int pci_enable_device(struct pci_dev *dev) { TRACE; return 0; }


/*********
 ** VGA **
 *********/

int vga_client_register(struct pci_dev *pdev, void *cookie,
                        void (*irq_set_state)(void *cookie, bool state),
                        unsigned int (*set_vga_decode)(void *cookie, bool state)) { TRACE; return 0; };

int vga_switcheroo_register_client(struct pci_dev *dev,
                                   void (*set_gpu_state)(struct pci_dev *dev,
                                   enum vga_switcheroo_state),
                                   bool (*can_switch)(struct pci_dev *dev)) { TRACE; return 0; };

void vga_switcheroo_unregister_client(struct pci_dev *dev) { TRACE; }
int vga_switcheroo_process_delayed_switch(void) { TRACE; return 0; }


/*************************************
 ** I/O Mapping, linux/io-mapping.h **
 *************************************/

struct io_mapping;
//void  io_mapping_unmap_atomic(void *vaddr) { TRACE; }
void  io_mapping_unmap_atomic(void *vaddr) { }
void  iounmap(volatile void *addr) { TRACE; }
void  io_mapping_free(struct io_mapping *mapping) { TRACE; }

phys_addr_t virt_to_phys(volatile void *address) { TRACE; return 0; }


/***************
 ** Workqueue **
 ***************/

/*
 * Used by i915_irq.c, function 'i915_driver_irq_preinstall' to install
 * the handlers 'i915_hotplug_work_func' and 'i915_error_work_func'.
 * Both handlers are there to interact with the Linux userland (uevent
 * and sysfs). - not needed here.
 */
void INIT_WORK(struct work_struct *ws, void (*cb)(struct work_struct *)) { TRACE; }
void destroy_workqueue(struct workqueue_struct *wq) { TRACE; } struct io_mapping;
//int queue_delayed_work(struct workqueue_struct *wq,
//                              struct delayed_work *work,
//                              unsigned long delay) { TRACE; return 0; }
int queue_delayed_work(struct workqueue_struct *wq,
                              struct delayed_work *work,
                              unsigned long delay) { return 0; }
int queue_work(struct workqueue_struct *wq, struct work_struct *work) { TRACE; return 0; }


/***************
 ** Waitqueue **
 ***************/

void wake_up(wait_queue_head_t *wqh) { TRACE; }
void add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait) { TRACE; }
int  default_wake_function(wait_queue_t *wait, unsigned mode, int flags, void *key) { TRACE; return 0; }
void remove_wait_queue(wait_queue_head_t *q, wait_queue_t *wait) { TRACE; }
void init_waitqueue_head(wait_queue_head_t *q) { TRACE; }
void finish_wait(wait_queue_head_t *q, wait_queue_t *wait) { TRACE; }
void prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state) { TRACE; }


/***********
 ** Timer **
 ***********/

void setup_timer(struct timer_list *timer,void (*function)(unsigned long), unsigned long data) { TRACE; }
//int mod_timer(struct timer_list *timer, unsigned long expires) { TRACE; return 0; }
int mod_timer(struct timer_list *timer, unsigned long expires) { return 0; }
int del_timer(struct timer_list * timer) { TRACE; return 0; }
int del_timer_sync(struct timer_list * timer) { TRACE; return 0; }


/****************
 ** Intel i915 **
 ****************/

void *i915_powersave(void) { TRACE; return 0; }
void *i915_suspend(void) { TRACE; return 0; }
void *i915_resume(void) { TRACE; return 0; }
void *i965_reset(void) { TRACE; return 0; }

void *intel_modeset_init(void) { TRACE; return 0; }
void *intel_init_bios(void) { TRACE; return 0; }
void *intel_modeset_vga_set_state(void) { TRACE; return 0; }
void *intel_cleanup_overlay(void) { TRACE; return 0; }
void *intel_modeset_cleanup(void) { TRACE; return 0; }
void *intel_overlay_put_image(void) { TRACE; return 0; }
void *intel_get_pipe_from_crtc_id(void) { TRACE; return 0; }
void *intel_overlay_attrs(void) { TRACE; return 0; }
//void *intel_mark_busy(void) { TRACE; return 0; }
void *intel_mark_busy(void) { return 0; }
void intel_prepare_page_flip(struct drm_device *dev, int plane) { TRACE; }
void intel_finish_page_flip(struct drm_device *dev, int pipe) { TRACE; }

void *i8xx_disable_fbc(void) { TRACE; return 0; }
void *g4x_disable_fbc(void) { TRACE; return 0; }


/********************
 ** Bit operations **
 ********************/

int __test_bit(int nr, const volatile long *addr) { TRACE; return 0; }
void __clear_bit(int nr, volatile long *addr) { TRACE; }


/*********************
 ** linux/vmalloc.h **
 *********************/

void vfree(const void *addr) { TRACE; }
void vunmap(const void *addr) { TRACE; }
void *vmap(struct page **pages, unsigned int count, unsigned long flags, pgprot_t prot) { TRACE; return 0; }


/**********
 ** Misc **
 **********/

/*
 * Global variables
 */
unsigned long jiffies;
int sysctl_vfs_cache_pressure;
struct resource iomem_resource;


int ffs(int x) { TRACE; return 0; }
long time_after_eq(long a, long b) { TRACE; return 0; }
unsigned long msecs_to_jiffies(const unsigned int m) { TRACE; return 0; }

int cancel_delayed_work_sync(struct delayed_work *work) { TRACE; return 0; }
int signal_pending(struct task_struct *p) { TRACE; return 0; }
void schedule(void) { TRACE; }

int kobject_uevent_env(struct kobject *kobj, enum kobject_action action,
                       char *envp[]) { TRACE; return 0; }

unsigned long local_irq_save(unsigned long flags) { TRACE; return 0; }
unsigned long local_irq_restore(unsigned long flags) { TRACE; return 0; }

void do_gettimeofday(struct timeval *tv) { TRACE; }
signed long schedule_timeout(signed long timeout) { TRACE; return 0; }
void __set_current_state(int state) { TRACE; }
unsigned long msleep_interruptible(unsigned int msecs) { TRACE; return 0; }

void register_shrinker(struct shrinker *s) { TRACE; }
void unregister_shrinker(struct shrinker *s) { TRACE; }

/* cpufeature.h */
int __cpu_has_clflush() { TRACE; return 0; }
void clflush_cache_range(void *vaddr, unsigned int size) { TRACE; }
int wbinvd_on_all_cpus(void) { TRACE; return 0; }

/* asm/agp.h (from arch/include) */
char *alloc_gatt_pages(unsigned size_log2) { TRACE; return 0; }
void free_gatt_pages(void *table, unsigned order) { TRACE; }
void flush_agp_cache(void) { TRACE; }

/* missing piece of the AGP driver */
int agp_frontend_initialize(void) { TRACE; return 0; }
void agp_frontend_cleanup(void) { TRACE; }
struct agp_bridge_data;
int agp_3_5_enable(struct agp_bridge_data *bridge) { TRACE; return 0; }

void free_pages(struct page *page, unsigned int order) { TRACE; }
int on_each_cpu(void (*func) (void *info), void *info, int wait) { TRACE; return 0; }
void module_put(struct module *module) { TRACE; }


/*******************
 ** DRM functions **
 *******************/

/* drm_gem.c - managing object handles ... use custom implementation */
//void drm_gem_object_handle_unreference_unlocked(struct drm_gem_object *obj) { TRACE; }
void drm_gem_object_handle_unreference_unlocked(struct drm_gem_object *obj) { }
//void drm_gem_object_reference(struct drm_gem_object *obj) { TRACE; }
void drm_gem_object_reference(struct drm_gem_object *obj) { }
//void drm_gem_object_unreference(struct drm_gem_object *obj) { TRACE; }
void drm_gem_object_unreference(struct drm_gem_object *obj) { }
//void drm_gem_object_unreference_unlocked(struct drm_gem_object *obj) { TRACE; }
void drm_gem_object_unreference_unlocked(struct drm_gem_object *obj) { }

/* drm_mm.c */
void drm_mm_put_block(struct drm_mm_node *cur) { TRACE; }
void drm_mm_takedown(struct drm_mm *mm) { TRACE; }

/* pci */
void drm_pci_free(struct drm_device *dev, drm_dma_handle_t *dmah) { TRACE; }

/* irq */
int drm_irq_uninstall(struct drm_device *dev) { TRACE; return 0; }

int drm_vblank_init(struct drm_device *dev, int num_crtcs) { TRACE; return 0; }

/* drm_context.c */
struct drm_local_map *drm_getsarea(struct drm_device *dev) { TRACE; return 0; }

void *drm_helper_initial_config(void) { TRACE; return 0; }
void *drm_fb_helper_restore(void) { TRACE; return 0; }

void drm_sysfs_hotplug_event(struct drm_device *dev) { TRACE; }
void drm_handle_vblank(struct drm_device *dev, int crtc) { TRACE; }

int drm_ctxbitmap_init(struct drm_device *dev) { TRACE; return 0; }
void drm_ctxbitmap_cleanup(struct drm_device *dev) { TRACE; }
int drm_device_is_agp(struct drm_device *dev) { TRACE; return 0; }
int drm_proc_init(struct drm_minor *minor, int minor_id,
                  struct proc_dir_entry *root) { TRACE; return 0; }
int drm_proc_cleanup(struct drm_minor *minor, struct proc_dir_entry *root) { TRACE; return 0; }
void drm_sysfs_device_remove(struct drm_minor *minor) { TRACE; }
int drm_rmmap(struct drm_device *dev, struct drm_local_map *map) { TRACE; return 0; }
void drm_vblank_cleanup(struct drm_device *dev) { TRACE; }

/* drm_hashtab.c */
int drm_ht_insert_item(struct drm_open_hash *ht, struct drm_hash_item *item) {
	TRACE;
	dde_kit_printf("item->key = %ld\n", item->key);
	return 0;
}
int drm_ht_remove_item(struct drm_open_hash *ht, struct drm_hash_item *item) { TRACE; return 0; }

