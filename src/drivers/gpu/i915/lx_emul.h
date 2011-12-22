/*
 * \brief  Emulation of the Linux kernel API used by DRM
 * \author Norman Feske
 * \date   2010-07-23
 */

#ifndef _LX_EMUL_H_
#define _LX_EMUL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* DDE Kit includes */
#include <dde_kit/types.h>
#include <dde_kit/printf.h>
#include <dde_kit/panic.h>
#include <dde_kit/lock.h>

#define VERBOSE_LX_EMUL 0


/***************
 ** asm/bug.h **
 ***************/

#define WARN_ON(condition) ({ \
	int ret = !!(condition); \
	if (ret) dde_kit_debug("[%s] WARN_ON(" #condition ") ", __func__); \
	ret; })

#define WARN(condition, fmt, arg...) ({ \
	int ret = !!(condition); \
	if (ret) dde_kit_debug("[%s] *WARN* " fmt , __func__ , ##arg); \
	ret; })

#define BUG() do { \
	dde_kit_debug("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
	while (1); \
} while (0)

#define BUG_ON(condition) do { if (condition) BUG(); } while(0)


/*****************
 ** asm/param.h **
 *****************/

enum { HZ = 100UL };


/******************
 ** asm/atomic.h **
 ******************/

typedef int atomic_t;

void     atomic_set(atomic_t *p, atomic_t v);
atomic_t atomic_read(atomic_t *p);

void     atomic_inc(atomic_t *v);
void     atomic_dec(atomic_t *v);

void     atomic_add(int i, atomic_t *v);
void     atomic_sub(int i, atomic_t *v);


/*******************
 ** linux/types.h **
 *******************/

typedef dde_kit_int8_t   int8_t;
typedef dde_kit_uint8_t  uint8_t;
typedef dde_kit_int16_t  int16_t;
typedef dde_kit_uint16_t uint16_t;
typedef dde_kit_int32_t  int32_t;
typedef dde_kit_uint32_t uint32_t;
typedef dde_kit_size_t   size_t;
typedef dde_kit_int64_t  int64_t;
typedef dde_kit_uint64_t uint64_t;

/*
 * Types used in intel_bios.h
 */
typedef int8_t   s8;
typedef uint8_t  u8;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;

#ifndef __cplusplus
typedef int bool;
enum { true = 1, false = 0 };
#endif /* __cplusplus */

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif /* __cplusplus */
#endif /* NULL */

typedef unsigned       gfp_t;
typedef unsigned long  pgoff_t;
typedef long long      loff_t;
typedef long           ssize_t;
typedef unsigned long  uintptr_t;
typedef int            dev_t;
typedef size_t         resource_size_t;
typedef long           off_t;

/*
 * needed by i915_drv.h, 'struct drm_i915_private',
 * needed by drm_agpsupport.c
 */
typedef unsigned long dma_addr_t;

/* needed by 'virt_to_phys', which is needed by agp/generic.c */
typedef unsigned long phys_addr_t;


/****************
 ** asm/page.h **
 ****************/

/*
 * For now, hardcoded to x86_32
 */
enum {
	PAGE_SIZE = 4096,
	PAGE_SHIFT = 12,
	PAGE_OFFSET = 0,
	PAGE_MASK = ~(PAGE_SIZE - 1),
};

dma_addr_t page_to_phys(void *page);

/*
 * Needed by i915/i915_drv.h, 'struct drm_i915_gem_phys_object'
 */
struct page {

	/*
	 * members specific for Linux emulation, not present in Linux
	 */
	void *virt;
};

/* needed for agp/generic.c */
struct page *virt_to_page(void *addr);

typedef unsigned long pgprot_t;


/**********************
 ** asm/cacheflush.h **
 **********************/

int set_pages_uc(struct page *page, int numpages);
int set_pages_wb(struct page *page, int numpages);


/******************
 ** asm/system.h **
 ******************/

/*
 * Functions normally declared in arch/asm/cpufeature.h, included by
 * asm/system.h
 *
 * needed by intel-agp.c
 */
int __cpu_has_clflush();
#define cpu_has_clflush __cpu_has_clflush()

void clflush_cache_range(void *vaddr, unsigned int size);
int wbinvd_on_all_cpus(void);


/**********************
 ** linux/compiler.h **
 **********************/

#define unlikely

#define __user   /* needed by drm.h, 'struct drm_version' */
#define __iomem  /* needed by i915_drv.h, 'struct drm_i915_private' */


/********************
 ** linux/poison.h **
 ********************/

/*
 * In list.h, LIST_POISON1 and LIST_POISON2 are assigned to 'struct list_head
 * *' as well as 'struct hlist_node *' pointers. Consequently, C++ compiler
 * produces an error "cannot convert... in assignment". To compile 'list.h'
 * included by C++ source code, we have to define these macros to the only
 * value universally accepted for pointer assigments.h
 */

#ifdef __cplusplus
#define LIST_POISON1 0
#define LIST_POISON2 0
#else
#define LIST_POISON1 ((void *)0x00100100)
#define LIST_POISON2 ((void *)0x00200200)
#endif  /* __cplusplus */


/**********************
 ** linux/prefetch.h **
 **********************/

/* needed by linux/list.h */
#define prefetch(x) __builtin_prefetch(x)


/******************
 ** linux/list.h **
 ******************/

/*
 * Pull in the original linux/list.h header, but take care
 * of C++ keywords used within the file.
 */
#define new _new
#include <linux/list.h>
#undef new


/*******************************
 ** linux/errno.h and friends **
 *******************************/

/**
 * Error codes used by DRM
 *
 * Note that the codes do not correspond to those of the Linux kernel.
 */
enum {
	EINVAL      = 1,
	ENODEV      = 2,
	ENOMEM      = 3,
	EFAULT      = 4,
	EBADF       = 5,
	EAGAIN      = 6,
	ERESTARTSYS = 7,
	ENOSPC      = 8,
	EIO         = 9,
	EBUSY       = 10,
	EPERM       = 11,
	EINTR       = 12,
};


static inline bool IS_ERR(void *ptr) {
	return (unsigned long)(ptr) > (unsigned long)(-1000); }


static inline long PTR_ERR(const void *ptr) { return (long)ptr; }


/********************
 ** linux/kernel.h **
 ********************/

/*
 * Log tags
 */
#define KERN_DEBUG "DEBUG: "
#define KERN_ERR   "ERROR: "
#define KERN_INFO  "INFO: "

/*
 * Debug macros
 */
#if VERBOSE_LX_EMUL
#define printk  dde_kit_printf
#define vprintk dde_kit_vprintf
#define panic   dde_kit_panic
#else /* VERBOSE_LX_EMUL */
#define printk(...)
#define vprintk(...)
#define panic(...)
#endif /* VERBOSE_LX_EMUL */


/*
 * Bits and types
 */

#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))
#define lower_32_bits(n) ((u32)(n))

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/* needed by linux/list.h */
#define container_of(ptr, type, member) ({ \
	const typeof( ((type *)0)->member ) *__mptr = (ptr); \
	(type *)( (char *)__mptr - offsetof(type,member) );})

/* normally provided by linux/stddef.h, needed by linux/list.h */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/* copied from linux/kernel.h, needed for agp/generic.c */
#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1: __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2; })

/**
 * Return minimum of two given values
 *
 * XXX check how this function is used (argument types)
 */
static inline size_t min(size_t a, size_t b) {
	return a < b ? a : b; }

/*
 * XXX check if this function is called only with int arguments
 */
static inline int ALIGN(int x, int boundary) {
	return (x + boundary) & ~(boundary - 1); }


/************************
 ** linux/tracepoint.h **
 ************************/

/* needed by i915_trace.h */

#define TP_PROTO(args...) args
#define PARAMS(args...)   args
#define TP_ARGS(args...)  args

#define TRACE_EVENT(name, proto, args, struct, assign, print) \
	static inline void trace_##name() { }


/******************
 ** linux/log2.h **
 ******************/

/**
 * Round x to the next multiple of y
 *
 * Typically used for aligning an address x to a page boundary
 * with page size y.
 *
 * XXX check how this function is used (argument types)
 */
static inline size_t roundup(size_t x, size_t y) { return ((x + y - 1)/y)*y; }


/**********************************
 ** linux/bitops.h, asm/bitops.h **
 **********************************/

int __test_bit(int nr, const volatile long *addr);
void __set_bit(int nr, volatile long *addr);
void __clear_bit(int nr, volatile long *addr);
unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size);

#define set_bit(nr, addr)   __set_bit  (nr,       (volatile long *)(addr))
#define clear_bit(nr, addr) __clear_bit(nr,       (volatile long *)(addr))
#define test_bit(nr, addr)  __test_bit (nr, (const volatile long *)(addr))

/* copied from linux/bitops.h */
#define BITS_PER_BYTE		8
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

/* normally declared in asm-generic/bitops/ffs.h */
int ffs(int x);


/********************
 ** linux/string.h **
 ********************/

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int   strcmp(const char *s1, const char *s2);


/******************
 ** linux/init.h **
 ******************/

#define __init
#define __exit

/*
 * needed by intel-agp.c
 */
#define __devexit_p(x) x
#define __devinit
#define __devexit

/*
 * Module init/exit functions are defined as static functions. The entry point
 * to the module is then created via the 'module_init' and 'module_exit'
 * macros. Because we know at compile time, which module we want to all, we let
 * the macro create externally visible wrappers around the static init/exit
 * functions. The wrappers are called module_<init-function>.
 */
#define module_init(init_fn) int module_##init_fn(void) { return init_fn(); }
#define module_exit(exit_fn) void module_##exit_fn(void) { exit_fn(); }

/*
 * The same as above, but for module entrypoints declared via '__setup',
 * e.g., agp/backend.c
 *
 * The resulting module_<setup-function> takes a string as argument.
 */
#define __setup(x, init_fn) int module_##init_fn(char *arg) { return init_fn(arg); }


/********************
 ** linux/module.h **
 ********************/

#define EXPORT_SYMBOL(x)
#define EXPORT_SYMBOL_GPL(x)
#define MODULE_AUTHOR(x)
#define MODULE_DESCRIPTION(x)
#define MODULE_LICENSE(x)
#define MODULE_PARM_DESC(x, y)
#define MODULE_ALIAS_MISCDEV(x)  /* needed by agp/backend.c */

#define THIS_MODULE 0

#define MODULE_DEVICE_TABLE(type, name)

struct module;

/* needed by agp/backend.c */
int try_module_get(struct module *module);
void module_put(struct module *module);


/*************************
 ** linux/moduleparam.h **
 *************************/

#define module_param_named(name, value, type, perm)


/******************
 ** linux/slab.h **
 ******************/

void *kzalloc(size_t size, gfp_t flags);
void kfree(const void *);
void *kcalloc(size_t n, size_t size, gfp_t flags);
void *kmalloc(size_t size, gfp_t flags);


/**********************
 ** linux/spinlock.h **
 **********************/

/*
 * needed by drm_crtc.h
 */

typedef struct dde_kit_lock *spinlock_t;
#define DEFINE_SPINLOCK(name) spinlock_t name = 0;

void spin_lock(spinlock_t *lock);
void spin_unlock(spinlock_t *lock);
void spin_lock_init(spinlock_t *lock);
void spin_lock_irqsave(spinlock_t *lock, unsigned long flags);
void spin_lock_irqrestore(spinlock_t *lock, unsigned long flags);
void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);


/*******************
 ** linux/mutex.h **
 *******************/

struct mutex { struct dde_kit_lock *lock; };

void mutex_init(struct mutex *m);
void mutex_lock(struct mutex *m);
void mutex_unlock(struct mutex *m);
int  mutex_trylock(struct mutex *m);


/*******************
 ** linux/rwsem.h **
 *******************/

struct rw_semaphore { int dummy; };

void down_read(struct rw_semaphore *sem);
void up_read(struct rw_semaphore *sem);
void down_write(struct rw_semaphore *sem);
void up_write(struct rw_semaphore *sem);


/*********************
 ** linux/jiffies.h **
 *********************/

/*
 * XXX check how the jiffies variable is used
 */
extern unsigned long jiffies;

unsigned long msecs_to_jiffies(const unsigned int m);
long time_after_eq(long a, long b);


/*******************
 ** linux/timer.h **
 *******************/

struct timer_list { int dummy; };

void init_timer(struct timer_list *);
int mod_timer(struct timer_list *timer, unsigned long expires);
int del_timer(struct timer_list * timer);
int del_timer_sync(struct timer_list * timer);
void setup_timer(struct timer_list *timer,void (*function)(unsigned long),
                 unsigned long data);


/*******************
 ** linux/delay.h **
 *******************/

unsigned long msleep_interruptible(unsigned int msecs);


/***********************
 ** linux/workquque.h **
 ***********************/

struct work_struct { int dummy; };
struct workqueue_struct;
struct delayed_work { struct work_struct work; };

void INIT_WORK(struct work_struct *, void (*)(struct work_struct *));

void INIT_DELAYED_WORK(struct delayed_work *, void (*)(struct work_struct *));

struct workqueue_struct *create_singlethread_workqueue(const char *name);
void destroy_workqueue(struct workqueue_struct *wq);
int queue_delayed_work(struct workqueue_struct *wq,
                              struct delayed_work *work,
                              unsigned long delay);
int cancel_delayed_work_sync(struct delayed_work *work);
int queue_work(struct workqueue_struct *wq, struct work_struct *work);


/******************
 ** linux/wait.h **
 ******************/

typedef struct wait_queue_head { int dummy; } wait_queue_head_t;

struct wait_queue;
typedef struct wait_queue wait_queue_t;
typedef int (*wait_queue_func_t)(wait_queue_t *wait, unsigned mode, int flags, void *key);

struct wait_queue {
	void *priv;
	wait_queue_func_t func;
	struct list_head task_list;
};

#define __WAITQUEUE_INITIALIZER(name, tsk) { \
	.priv       = tsk,                       \
	.func       = default_wake_function,     \
	.task_list  = { NULL, NULL } }

#define DECLARE_WAITQUEUE(name, tsk) \
	wait_queue_t name = __WAITQUEUE_INITIALIZER(name, tsk)

#define DEFINE_WAIT(name) \
	wait_queue_t name; \
	dde_kit_debug("DEFINE_WAIT, not yet implemented");

struct lock_class_key;
void init_waitqueue_head(wait_queue_head_t *q);
void prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state);
void finish_wait(wait_queue_head_t *q, wait_queue_t *wait);

/*
 * called by 'i915_do_wait_request' to wait for the occurrence of
 * an IRQ.
 */
#define wait_event_interruptible(wq, condition) 0
//({ dde_kit_printf("wait_event_interruptible, not yet implemented\n"); 0; })

#define wait_event(wq, condition) \
	dde_kit_printf("wait_event, not yet implemented\n");

void add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait);
int default_wake_function(wait_queue_t *wait, unsigned mode, int flags, void *key);
void remove_wait_queue(wait_queue_head_t *q, wait_queue_t *wait);
void wake_up(wait_queue_head_t *wqh);


/******************
 ** linux/time.h **
 ******************/

/*
 * needed by i915_drv.h, 'struct drm_i915_error_state',
 * normally defined in 'linux/time.h'
 */
struct timeval { int dummy; };
void do_gettimeofday(struct timeval *tv);


/*******************
 ** linux/sched.h **
 *******************/

enum { TASK_RUNNING, TASK_INTERRUPTIBLE };

struct mm_struct;
struct task_struct {
	struct mm_struct *mm;
	char comm[16]; /* needed by agp/generic.c, only for debug output */
};

signed long schedule_timeout(signed long timeout);
void __set_current_state(int state);
int signal_pending(struct task_struct *p);
void schedule(void);

/* normally declared in linux/smp.h, included by sched.h */
int on_each_cpu(void (*func) (void *info), void *info, int wait);

/* normally defined in asm/current.h, included by sched.h */
extern struct task_struct *current;


/**********************
 ** linux/notifier.h **
 **********************/

struct notifier_block { int dummy; };


/**************************
 ** linux/i2c-algo-bit.h **
 **************************/

/* needed by intel_drv.h */
struct i2c_algo_bit_data { int dummy; };


/*****************
 ** linux/i2c.h **
 *****************/

struct i2c_adapter { int dummy; };


/*****************
 ** linux/idr.h **
 *****************/

/* needed by intel_drv.h */
struct idr { int dummy; };


/******************
 ** linux/kref.h **
 ******************/

struct kref { atomic_t refcount; };

void kref_init(struct kref *kref);
void kref_get(struct kref *kref);
int  kref_put(struct kref *kref, void (*release) (struct kref *kref));


/*********************
 ** linux/kobject.h **
 *********************/

struct kobject { int dummy; };

/* needed by pci/pci.h */
struct kobj_uevent_env;

enum kobject_action { KOBJ_CHANGE };

/* needed for i915_irq.h */
int kobject_uevent_env(struct kobject *kobj, enum kobject_action action,
                       char *envp[]);


/********************
 ** linux/device.h **
 ********************/

struct device { struct kobject kobj; };

#define dev_info(dev, format, arg...)   dde_kit_printf ("dev_info: "  format , ## arg)
#define dev_warn(dev, format, arg...)   dde_kit_debug ("dev_warn: "  format , ## arg)
#define dev_err(dev, format, arg...)    dde_kit_debug ("dev_error: " format , ## arg)
#define dev_emerg(dev, format, arg...)  dde_kit_debug ("dev_emerg: " format , ## arg)

#define dev_printk(level, dev, format, arg...) \
	dde_kit_printf("dev: " format , ## arg)


/*************************
 ** linux/dma-mapping.h **
 *************************/

static inline u64 DMA_BIT_MASK(unsigned n) {
	return (n == 64) ? ~0ULL : (1ULL << n) - 1; }


/****************
 ** linux/pm.h **
 ****************/

typedef struct pm_message { int event; } pm_message_t;

enum { PM_EVENT_SUSPEND = 0x0002 }; /* used by i915_dma.c */


/*********************
 ** linux/uaccess.h **
 *********************/

enum { VERIFY_READ = 0 };

bool access_ok(int access, void *addr, size_t size);

size_t copy_from_user(void *to, void *from, size_t len);
size_t copy_to_user(void *dst, void *src, size_t len);

#define __copy_from_user                  copy_from_user
#define __copy_to_user                    copy_to_user
#define __copy_from_user_inatomic         copy_from_user
#define __copy_to_user_inatomic           copy_to_user
#define __copy_from_user_inatomic_nocache copy_from_user


/****************
 ** asm/mtrr.h **
 ****************/

/*
 * needed by i915_dma.c
 */

static inline int mtrr_del(int reg, unsigned long base, unsigned long size) {
	return -ENODEV; }

static inline int mtrr_add(unsigned long offset, unsigned long size,
                           unsigned int flags, char inc) {
    return 0; }

enum { MTRR_TYPE_WRCOMB = 1 };


/********************
 ** linux/dcache.h **
 ********************/

struct dentry { struct inode *d_inode; };

extern int sysctl_vfs_cache_pressure;


/******************
 ** linux/path.h **
 ******************/

struct path { struct dentry *dentry; };


/****************
 ** linux/fs.h **
 ****************/

/*
 * In i915_gem.c, in particular function 'i915_gem_object_truncate' relies on
 * certain Linux fs infrastructure.  So we have to declare the involved structs
 * here.
 */

struct inode;
struct inode_operations { void (*truncate) (struct inode *); };

struct inode {
	const struct inode_operations *i_op;
	struct address_space *i_mapping;
};

struct file { struct path f_path; };

enum {
	PROT_READ  = 0x1,
	PROT_WRITE = 0x2
};

enum { MAP_SHARED = 0x1 };


/*********************
 ** linux/highmem.h **
 *********************/

/**
 * Kernel memory types - only those types needed by DRM
 */
enum km_type { KM_USER0, KM_USER1, KM_TYPE_NR, KM_IRQ0 };

void *kmap_atomic(struct page *page, enum km_type idx);
void kunmap_atomic(void *vaddr, enum km_type idx);

void *kmap(struct page *page);
void kunmap(struct page *page);


/*****************
 ** linux/gfp.h **
 *****************/

enum {
	__GFP_ZERO    = 0x8000u, /* needed by intel-agp.c */
	__GFP_NORETRY = 0x1000u,
	__GFP_NOWARN  = 0x200u,
	__GFP_COLD    = 0x100u,
	GFP_ATOMIC    = 0x20u,
	GFP_DMA32     = 0x04u,   /* needed by intel-agp.c */
	GFP_KERNEL    = 0x0u,
};

/* needed by intel-agp.c */
struct page *alloc_pages(gfp_t gfp_mask, unsigned int order);

static inline struct page * alloc_page(gfp_t gfp_mask) {
	return alloc_pages(gfp_mask, 0); }

void free_pages(struct page *page, unsigned int order);

static inline void __free_page(struct page *p) {
	free_pages(p, 0); }

#define __free_pages free_pages


/*********************
 ** linux/proc_fs.h **
 *********************/

struct proc_dir_entry;


/************************
 ** linux/page-flags.h **
 ************************/

void SetPageDirty(struct page *page);

/* needed by agp/generic.c */
void SetPageReserved(struct page *page);
void ClearPageReserved(struct page *page);


/**********************
 ** linux/mm_types.h **
 **********************/

struct vm_area_struct {
	unsigned long  vm_start;
	void          *vm_private_data;
};

struct mm_struct { struct rw_semaphore mmap_sem; };


/****************
 ** linux/mm.h **
 ****************/

extern unsigned long totalram_pages;

enum {
	VM_FAULT_OOM    = 0x001,
	VM_FAULT_SIGBUS = 0x002,
	VM_FAULT_NOPAGE = 0x100,
};

enum { FAULT_FLAG_WRITE = 0x1 };

enum { DEFAULT_SEEKS = 2 };

#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)

struct vm_fault {
	void *virtual_address;
	unsigned int flags;
};

struct file;
struct address_space;

unsigned long do_mmap(struct file *file, unsigned long addr,
                      unsigned long len, unsigned long prot,
                      unsigned long flag, unsigned long offset);

void unmap_mapping_range(struct address_space *mapping,
                         loff_t const holebegin, loff_t const holelen,
                         int even_cows);

int vm_insert_pfn(struct vm_area_struct *vma, unsigned long addr,
                  unsigned long pfn);

int set_page_dirty(struct page *page);

void get_page(struct page *page);
void put_page(struct page *page);

int get_user_pages(void *tsk, struct mm_struct *mm,
                   unsigned long start, int nr_pages, int write, int force,
                   struct page **pages, struct vm_area_struct **vmas);

void *page_address(struct page *page);

struct shrinker {
	int (*shrink)(int nr_to_scan, gfp_t gfp_mask);
	int seeks;
};

void register_shrinker(struct shrinker *);
void unregister_shrinker(struct shrinker *);


/*********************
 ** linux/pagemap.h **
 *********************/

/*
 * needed by intel-agp.c
 */

gfp_t mapping_gfp_mask(struct address_space * mapping);
struct page *read_cache_page_gfp(struct address_space *mapping,
                                 pgoff_t index, gfp_t gfp_mask);
void page_cache_release(struct page *page);


/*********************
 ** linux/vmalloc.h **
 *********************/

enum { VM_IOREMAP = 0x1 };

/* needed by agp/generic.c */
void *vmalloc(unsigned long size);
void vfree(const void *addr);

/* needed by drm_memory.c */
void vunmap(const void *addr);
void *vmap(struct page **pages, unsigned int count, unsigned long flags, pgprot_t prot);


/******************
 ** linux/swap.h **
 ******************/

void mark_page_accessed(struct page *);


/************************
 ** linux/io-mapping.h **
 ************************/

/*
 * needed by 'drm/i915/i915_drv.h'
 */

struct io_mapping;

void *io_mapping_map_atomic_wc(struct io_mapping *mapping, unsigned long offset);
void io_mapping_unmap_atomic(void *vaddr);

struct io_mapping *io_mapping_create_wc(resource_size_t base, unsigned long size);
void io_mapping_free(struct io_mapping *mapping);


/****************
 ** linux/io.h **
 ****************/

#define writel(value, addr) (*(volatile uint32_t *)(addr) = (value))

static inline void writeq(long long value, long long *addr) {
	dde_kit_debug("writeq(value=%x, addr=%p) called, not implemented",
	              (int)value, addr);
}

#define readb(addr) ({ \
	dde_kit_debug("readb(addr=%p) called, not implemented", \
	              (void *)addr); \
	0; })

#define readw(addr) (*(volatile uint16_t *)(addr))
#define readl(addr) (*(volatile uint32_t *)(addr))
#define readq(addr) ({ \
	dde_kit_debug("readq(addr=%p) called, not implemented", \
	              (void *)addr); \
	0; })

void *ioremap(resource_size_t offset, unsigned long size);
void  iounmap(volatile void *addr);
void *ioremap_wc(resource_size_t phys_addr, unsigned long size);

#define ioremap_nocache ioremap_wc

phys_addr_t virt_to_phys(volatile void *address);


/********************
 ** linux/ioport.h **
 ********************/

/*
 * needed for intel-agp.c 'struct resource' is a member of 'struct _intel_private'
 */

struct resource
{
	resource_size_t start; /* needed for i915_dma.c */
	resource_size_t end;   /* needed for intel-agp.c */
	const char     *name;  /* needed for intel-agp.c */
	unsigned long  flags;  /* needed for intel-agp.c */
};

enum { IORESOURCE_MEM = 0x00000200 };

/*
 * needed for drm_crtc_helper.h, included by i915_dma.c
 */
int release_resource(struct resource *r);

/*
 * needed for intel-agp.c
 */
int request_resource(struct resource *root, struct resource *n);
extern struct resource iomem_resource;

/*********************
 ** linux/pci_ids.h **
 *********************/

/* pulling in the original linux/pci_ids.h */
#include <linux/pci_ids.h>


/*****************
 ** linux/pci.h **
 *****************/

/*
 * Definitions normally found in pci_regs.h
 */
enum { PCI_BASE_ADDRESS_MEM_MASK = ~0x0fUL };
enum { PCI_CAP_ID_AGP            = 0x02 };
enum { PCI_AGP_STATUS            = 4 /* status register */ };
enum { PCI_AGP_COMMAND           = 8 /* control register */ };

enum { PCI_ANY_ID = ~0 };
enum { DEVICE_COUNT_RESOURCE = 12 };
enum { PCIBIOS_MIN_MEM = 0UL };

/*
 * PCI types
 */
struct pci_bus;
struct pci_dev {
	unsigned int devfn;
	unsigned int irq;
	struct resource resource[DEVICE_COUNT_RESOURCE];
	struct pci_bus *bus; /* needed for i915_dma.c */
	unsigned short vendor;  /* needed for intel-agp.c */
	unsigned short device;
	u8 hdr_type;
	bool msi_enabled;
	struct device dev; /* needed for intel-agp.c */
};

/*
 * Deal with C++ keyword used as member name of 'pci_device_id'
 */
#ifdef __cplusplus
#define class device_class
#endif /* __cplusplus */

struct pci_device_id {
	u32 vendor, device, subvendor, subdevice, class, class_mask;
	unsigned long driver_data;
};

#ifdef __cplusplus
#undef class
#endif /* __cplusplus */

/*
 * Interface functions provided by the intel agp driver. The driver
 * announces the initialized 'pci_driver' structure to the kernel by
 * calling 'pci_register_driver'.
 */
struct pci_driver {
	char                        *name;
	const struct pci_device_id  *id_table;
	int                        (*probe)  (struct pci_dev *dev,
	                                      const struct pci_device_id *id);
	void                       (*remove) (struct pci_dev *dev);
};

typedef enum { PCI_D0 = 0 } pci_power_t;

static inline uint32_t PCI_DEVFN(unsigned slot, unsigned func) {
	return ((slot & 0x1f) << 3) | (func & 0x07); }

static inline unsigned PCI_FUNC(unsigned devfn) {
	return devfn & 0x7; }

int pci_bus_read_config_byte(struct pci_bus *bus, unsigned int devfn, int where, u8 *val);
int pci_bus_read_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 *val);
int pci_bus_read_config_dword(struct pci_bus *bus, unsigned int devfn, int where, u32 *val);
int pci_bus_write_config_byte(struct pci_bus *bus, unsigned int devfn, int where, u8 val);
int pci_bus_write_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 val);
int pci_bus_write_config_dword(struct pci_bus *bus, unsigned int devfn, int where, u32 val);

static inline
int pci_read_config_byte(struct pci_dev *dev, int where, u8 *val) {
	return pci_bus_read_config_byte(dev->bus, dev->devfn, where, val); }

static inline
int pci_read_config_word(struct pci_dev *dev, int where, u16 *val) {
	return pci_bus_read_config_word(dev->bus, dev->devfn, where, val); }

static inline
int pci_read_config_dword(struct pci_dev *dev, int where, u32 *val) {
	return pci_bus_read_config_dword(dev->bus, dev->devfn, where, val); }

static inline
int pci_write_config_byte(struct pci_dev *dev, int where, u8 val) {
	return pci_bus_write_config_byte(dev->bus, dev->devfn, where, val); }

static inline
int pci_write_config_word(struct pci_dev *dev, int where, u16 val) {
	return pci_bus_write_config_word(dev->bus, dev->devfn, where, val); }

static inline
int pci_write_config_dword(struct pci_dev *dev, int where, u32 val) {
	return pci_bus_write_config_dword(dev->bus, dev->devfn, where, val); }

struct pci_dev *pci_get_bus_and_slot(unsigned int bus, unsigned int devfn);
int pci_bus_alloc_resource(struct pci_bus *bus,
                           struct resource *res, resource_size_t size,
                           resource_size_t align, resource_size_t min,
                           unsigned int type_mask,
                           resource_size_t (*alignf)(void *,
                                                     const struct resource *,
                                                     resource_size_t,
                                                     resource_size_t),
                           void *alignf_data);
resource_size_t pcibios_align_resource(void *, const struct resource *,
                                       resource_size_t,
                                       resource_size_t);
size_t pci_resource_len(struct pci_dev *dev, unsigned bar);
size_t pci_resource_start(struct pci_dev *dev, unsigned bar);
void *pci_get_drvdata(struct pci_dev *pdev);
int pci_set_power_state(struct pci_dev *dev, pci_power_t state);
void pci_dev_put(struct pci_dev *dev);
int pci_enable_msi(struct pci_dev *dev);
int pci_disable_msi(struct pci_dev *dev);
struct pci_dev *pci_get_device(unsigned int vendor, unsigned int device,
                               struct pci_dev *from);
int pci_set_dma_mask(struct pci_dev *dev, u64 mask);
int pci_find_capability(struct pci_dev *dev, int cap);
int pci_enable_device(struct pci_dev *dev);
int pci_assign_resource(struct pci_dev *dev, int i);
int pci_set_consistent_dma_mask(struct pci_dev *dev, u64 mask);
void pci_set_drvdata(struct pci_dev *pdev, void *data);
struct pci_dev *pci_get_class(unsigned int device_class, struct pci_dev *from);
int pci_register_driver(struct pci_driver *driver);
void pci_unregister_driver(struct pci_driver *driver);

/* taken from linux/pci.h, needed by agp/generic.c */
#define for_each_pci_dev(d) while ((d = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, d)) != NULL)


/***************
 ** asm/agp.h **
 ***************/

char *alloc_gatt_pages(unsigned order);
void free_gatt_pages(void *table, unsigned order);
void flush_agp_cache(void);

static inline int map_page_into_agp(struct page *p) {
	return set_pages_uc(p, 1); }

static inline int unmap_page_from_agp(struct page *p) {
	return set_pages_wb(p, 1); }


/*************************
 ** linux/agp_backend.h **
 *************************/

#include <linux/agp_backend.h>


/***********************
 ** linux/irqreturn.h **
 ***********************/

typedef int irqreturn_t;
enum irqreturn { IRQ_NONE, IRQ_HANDLED, };


/****************
 ** linux/fb.h **
 ****************/

struct fb_var_screeninfo;
struct fb_cmap;

/* needed for drm_crtc_helper.h, included by i915_dma.c */
struct fb_info;


/********************
 ** linux/vgaarb.h **
 ********************/

/*
 * needed for compiling i915_dma.c
 */

enum {
	VGA_RSRC_LEGACY_IO  = 0x01,
	VGA_RSRC_LEGACY_MEM = 0x02,
	VGA_RSRC_NORMAL_IO  = 0x04,
	VGA_RSRC_NORMAL_MEM = 0x08,
};

int vga_client_register(struct pci_dev *pdev, void *cookie,
                        void (*irq_set_state)(void *cookie, bool state),
                        unsigned int (*set_vga_decode)(void *cookie, bool state));


/****************************
 ** linux/vga_switcheroo.h **
 ****************************/

/*
 * needed for compiling i915_dma.c
 */

enum vga_switcheroo_state { VGA_SWITCHEROO_OFF, VGA_SWITCHEROO_ON };

int vga_switcheroo_register_client(struct pci_dev *dev,
                                   void (*set_gpu_state)(struct pci_dev *dev,
                                   enum vga_switcheroo_state),
                                   bool (*can_switch)(struct pci_dev *dev));
void vga_switcheroo_unregister_client(struct pci_dev *dev);
int vga_switcheroo_process_delayed_switch(void);


/**********************
 ** linux/irqflags.h **
 **********************/

unsigned long local_irq_save(unsigned long flags);
unsigned long local_irq_restore(unsigned long flags);


/**********************
 ** asm/pgtable_64.h **
 **********************/

/*
 * The definition of PAGE_AGP is used only by drm_memory.c, function
 * 'agp_remap'. We define it such that we see its first actual use.
 */
static inline unsigned long __PAGE_AGP_used() {
	dde_kit_debug("PAGE_AGP used\n"); return 0; }

#define PAGE_AGP __PAGE_AGP_used()

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LX_EMUL_H_ */
