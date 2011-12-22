/*
 * \brief  DDE Linux 2.6 processes
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 */

#include <dde_linux26/general.h>

#include <asm/atomic.h>

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/thread_info.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/vmalloc.h>

#include "local.h"

/*****************************************************************************
 ** Current() implementation                                                **
 *****************************************************************************/
struct thread_info *current_thread_info(void)
{
	dde_linux26_thread_data *cur = (dde_linux26_thread_data *)dde_kit_thread_get_my_data();
	return &LX_THREAD(cur);
}

struct task_struct *get_current(void)
{
	return current_thread_info()->task;
}

/*****************************************************************************
 ** PID-related stuff                                                       **
 **                                                                         **
 ** Linux manages lists of PIDs that are handed out to processes so that at **
 ** a later point it is able to determine which task_struct belongs to a    **
 ** certain PID. We implement this with a single list holding the mappings  **
 ** for all our threads.                                                    **
 *****************************************************************************/

static LIST_HEAD(_pid_task_list);
static struct dde_kit_lock *_pid_task_list_lock;

/** PID to task_struct mapping */
struct pid2task
{
    struct list_head list;    /**< list data */
    pid_t pid;                /**< PID */
    struct task_struct *ts;   /**< task struct */
};

/** Attach PID to a certain task struct. */
int fastcall attach_pid(struct task_struct *task, enum pid_type type
                        __attribute__((unused)), int nr)
{
	/* Initialize a new pid2task mapping */
	struct pid2task *pt = kmalloc(sizeof(struct pid2task), GFP_KERNEL);
	pt->pid = nr;
	pt->ts = task;

	/* add to list */
	dde_kit_lock_lock(_pid_task_list_lock);
	list_add(&pt->list, &_pid_task_list);
	dde_kit_lock_unlock(_pid_task_list_lock);
	
	return 0;
}

/** Detach PID from a task struct. */
void fastcall detach_pid(struct task_struct *task, enum pid_type type
                                          __attribute__((unused)))
{
	struct list_head *p, *n, *h;

	h = &_pid_task_list;
	
	dde_kit_lock_lock(_pid_task_list_lock);
	/* search for mapping with given task struct and free it if necessary */
	list_for_each_safe(p, n, h) {
		struct pid2task *pt = list_entry(p, struct pid2task, list);
		if (pt->ts == task) {
			list_del(p);
			kfree(pt);
			break;
		}
	}
	dde_kit_lock_unlock(_pid_task_list_lock);
}

struct task_struct *find_task_by_pid_type(int type, int nr)
{
	struct list_head *h, *p;
	h = &_pid_task_list;

	dde_kit_lock_lock(_pid_task_list_lock);
	list_for_each(p, h) {
		struct pid2task *pt = list_entry(p, struct pid2task, list);
		if (pt->pid == nr) {
			dde_kit_lock_unlock(_pid_task_list_lock);
			return pt->ts;
		}
	}
	dde_kit_lock_unlock(_pid_task_list_lock);

	return NULL;
}

/*****************************************************************************
 ** kernel_thread() implementation                                          **
 *****************************************************************************/
/* Struct containing thread data for a newly created kthread. */
struct __kthread_data
{
	int (*fn)(void *);
	void *arg;
	char name[20];
};

/** Counter for running kthreads. It is used to create unique names
 *  for kthreads.
 */
static atomic_t kthread_count = ATOMIC_INIT(0);

/** Entry point for new kernel threads. Make this thread a DDE Linux 2.6
 *  worker and then execute the real thread fn.
 */
static void __kthread_helper(void *arg)
{
	struct __kthread_data k = *((struct __kthread_data *)arg);
	vfree(arg);

	dde_linux26_process_add_worker("unused");
	
	do_exit(k.fn(k.arg));
}

/** Our implementation of Linux' kernel_thread() function. Setup a new
 * thread running our __kthread_helper() function.
 */
int kernel_thread(int (*fn)(void *), void *arg, unsigned long flags)
{
	struct dde_kit_thread *t;
	struct __kthread_data *kt = vmalloc(sizeof(struct __kthread_data));

	int threadnum = atomic_inc_return(&kthread_count);
	kt->fn        = fn;
	kt->arg       = arg;

	snprintf(kt->name, sizeof(kt->name), ".kthread%x", threadnum);
	t = dde_kit_thread_create(__kthread_helper, (void *)kt, kt->name);
	dde_kit_assert(t);

	return dde_kit_thread_get_id(t);
}

/** Our implementation of exit(). For DDE purposes this only relates
 * to kernel threads.
 */
fastcall void do_exit(long code)
{
	struct dde_kit_thread *t = DDE_KIT_THREAD(lxtask_to_ddethread(current));
//	printk("Thread %s exits with code %x\n", dde_kit_thread_get_name(t), code);

	/* do some cleanup */
	detach_pid(current, 0);
	
	/* goodbye, cruel world... */
	dde_kit_thread_exit();

	while (1) ;
}

/*****************************************************************************
 ** Misc functions                                                          **
 *****************************************************************************/

void dump_stack(void)
{
}


/**************************************
 ** DDE kit gluecode, init functions **
 **************************************/

/**
 * Initialize a DDE Linux 2.6 thread
 *
 * - Allocate thread data, as well as a Linux task struct, 
 * - Fill in default values for thread_info, and task,
 * - Adapt task struct's thread_info backreference
 * - Initialize the DDE sleep lock
 */
static inline dde_linux26_thread_data *init_dde_linux26_thread(void)
{
	dde_linux26_thread_data *t = vmalloc(sizeof(dde_linux26_thread_data));
	dde_kit_assert(t);
	
	memcpy(&LX_THREAD(t), &init_thread, sizeof(struct thread_info));

	LX_TASK(t) = vmalloc(sizeof(struct task_struct));
	dde_kit_assert(LX_TASK(t));

	memcpy(LX_TASK(t), &init_task, sizeof(struct task_struct));

	/* nice: Linux backreferences a task`s thread_info from the
	*        task struct (which in turn can be found using the
	*        thread_info...) */
	LX_TASK(t)->thread_info = &LX_THREAD(t);

	/* initialize this thread's sleep lock */
	SLEEP_LOCK(t) = dde_kit_sem_init(0);

	return t;
}

/* Process setup for worker threads */
void dde_linux26_process_add_worker(const char *name)
{
	dde_linux26_thread_data *cur = init_dde_linux26_thread();

	/*
	 * If this function is called for a kernel_thread, the thread already has
	 * been set up and we just need to store the DDE kit thread handle.
	 * However, this function may also be called directly to turn other threads
	 * into a DDE thread. Then, we need to initialize here.
	 */
	cur->_dde_kit_thread = dde_kit_thread_myself();
	if (cur->_dde_kit_thread == NULL) {
		/* external threads can be adopted */
		cur->_dde_kit_thread = dde_kit_thread_adopt_myself(name);
		if (cur->_dde_kit_thread == NULL)
			dde_kit_panic("Panic: thread was not adopted by DDE kit");
	}

	dde_kit_thread_set_my_data(cur);

	attach_pid(LX_TASK(cur), 0,
               dde_kit_thread_get_id(cur->_dde_kit_thread));

	/*
	 * Linux' default is to have this set to 1 initially and let the
	 * scheduler set this to 0 later on.
	 */
	current_thread_info()->preempt_count = 0;
}

int dde_linux26_process_from_dde_kit(struct dde_kit_thread *t)
{
	dde_linux26_thread_data *cur = init_dde_linux26_thread();
	cur->_dde_kit_thread = t;
	dde_kit_thread_set_data(t, cur);
	attach_pid(LX_TASK(cur), 0, dde_kit_thread_get_id(t));
	return 0;
}

/** Function to initialize the first DDE process.
 */
int dde_linux26_process_init(void)
{
	/* init slab caches */
	dde_kit_lock_init(&_pid_task_list_lock);

	return 0;
}

