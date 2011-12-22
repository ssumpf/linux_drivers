/*
 * \brief  DDE Linux 2.6 locals
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 */

#ifndef _ARCH__DDE_KIT__LOCAL_H_
#define _ARCH__DDE_KIT__LOCAL_H_

#include <linux/sched.h>

#include <dde_kit/assert.h>
#include <dde_kit/initcall.h>
#include <dde_kit/interrupt.h>
#include <dde_kit/lock.h>
#include <dde_kit/memory.h>
#include <dde_kit/panic.h>
#include <dde_kit/pci.h>
#include <dde_kit/pgtab.h>
#include <dde_kit/printf.h>
#include <dde_kit/resources.h>
#include <dde_kit/semaphore.h>
#include <dde_kit/thread.h>
#include <dde_kit/types.h>
#include <dde_kit/timer.h>

#include <dde_linux26/general.h>

#define DDE_DEBUG     1

typedef struct dde_linux26_thread_data
{
	/* NOTE: _threadinfo needs to be first in this struct! */
	struct thread_info     _thread_info;
	struct dde_kit_thread *_dde_kit_thread;
	struct dde_kit_sem    *_sleep_lock;
} dde_linux26_thread_data;

#define LX_THREAD(thread_data)     ((thread_data)->_thread_info)
#define LX_TASK(thread_data)       ((thread_data)->_thread_info.task)
#define DDE_KIT_THREAD(thread_data) ((thread_data)->_dde_kit_thread)
#define SLEEP_LOCK(thread_data)    ((thread_data)->_sleep_lock)

#define ESC_RED   "\033[31m"
#define ESC_GREEN "\033[32m"
#define ESC_BLUE  "\033[34m"
#define ESC_CYAN  "\033[36m"
#define ESC_END   "\033[0m"

extern void fmt_check(const char *format, ...) __attribute__((format(printf, 1, 2)));

#if DDE_DEBUG
#define DEBUG_MSG(args...)                            \
	do {                                              \
		if (0) fmt_check(args);                       \
		printk("%s: " ESC_CYAN, __PRETTY_FUNCTION__); \
		printk(args);                                 \
		printk(ESC_END"\n");                          \
	} while (0)

#define DECLARE_INITVAR(name) \
	static struct { \
		int _initialized; \
		char *name; \
	} init_##name = {0, #name,}

#define INITIALIZE_INITVAR(name) init_##name._initialized = 1

#define CHECK_INITVAR(name) \
	if (init_##name._initialized == 0) { \
		printk("dde_linux26: " ESC_RED "Using uninitialized subsystem: "#name ESC_END "\n"); \
		BUG(); \
	}

#else /* !DDE_DEBUG */

#define DEBUG_MSG(...)             do {} while(0)
#define DECLARE_INITVAR(name)
#define CHECK_INITVAR(name)        do {} while(0)
#define INITIALIZE_INITVAR(name)   do {} while(0)

#endif

/* since _thread_info always comes first in the thread_data struct,
 * we can derive the dde_linux26_thread_data from a task struct by simply
 * dereferencing its thread_info pointer
 */
static inline dde_linux26_thread_data *lxtask_to_ddethread(struct task_struct *t)
{
	return (dde_linux26_thread_data *)(t->thread_info);
}

extern struct thread_info init_thread;
extern struct task_struct init_task;


extern void dde_linux26_raise_softirq(void);


/***********************
 ** DDE Linux 2.6 NET **
 ***********************/

#include <linux/skbuff.h>

/**
 * Run callback function
 *
 * \param   s  socket buffer containing packet
 */
extern int dde_linux26_do_rx_callback(struct sk_buff *s);


/******************************
 ** DDE Linux 2.6 subsystems **
 ******************************/

/**
 * Initialize process subsystem
 *
 * XXX The calling thread does not become a DDE kit thread!
 */
extern int dde_linux26_process_init(void);

/**
 * Initialize SoftIRQ subsystem
 */
extern void dde_linux26_softirq_init(void);

/**
 * Initialize timer subsystem
 */
extern void dde_linux26_timer_init(void);

/**
 * Initialize memory subsystem
 */
extern void dde_linux26_kmalloc_init(void);

/**
 * Initialize printk
 */
extern void dde_linux26_printk_init(void);

#endif /* _ARCH__DDE_KIT__LOCAL_H_ */
