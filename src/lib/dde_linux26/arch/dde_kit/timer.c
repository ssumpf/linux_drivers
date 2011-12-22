/*
 * \brief  DDE Linux 2.6 timers
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 */

#include <linux/timer.h>
#include <linux/fs.h>

#include "local.h"


DECLARE_INITVAR(dde_linux26_timer);

/* Definitions from linux/kernel/timer.c */

/*
 * per-CPU timer vector definitions:
 */
#define TVN_BITS (CONFIG_BASE_SMALL ? 4 : 6)
#define TVR_BITS (CONFIG_BASE_SMALL ? 6 : 8)
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)

typedef struct tvec_s {
	struct list_head vec[TVN_SIZE];
} tvec_t;

typedef struct tvec_root_s {
	struct list_head vec[TVR_SIZE];
} tvec_root_t;

struct tvec_t_base_s {
	spinlock_t lock;
	struct timer_list *running_timer;
	unsigned long timer_jiffies;
	tvec_root_t tv1;
	tvec_t tv2;
	tvec_t tv3;
	tvec_t tv4;
	tvec_t tv5;
} ____cacheline_aligned_in_smp;

typedef struct tvec_t_base_s tvec_base_t;

tvec_base_t boot_tvec_bases __attribute__((unused));

static DEFINE_PER_CPU(tvec_base_t *, tvec_bases) __attribute__((unused)) = &boot_tvec_bases;

void fastcall init_timer(struct timer_list *timer) { }

void add_timer(struct timer_list *timer)
{
	CHECK_INITVAR(dde_linux26_timer);
	/*
	 * The DDE kit provides jiffies and HZ. Therefore, we just need to hand
	 * over the timeout.
	 */
	timer->dde_kit_timer = dde_kit_timer_add((void *)timer->function,
	                                         (void *)timer->data,
	                                         timer->expires);
}


void add_timer_on(struct timer_list *timer, int cpu)
{
	add_timer(timer);
}


int del_timer(struct timer_list * timer)
{
	CHECK_INITVAR(dde_linux26_timer);

	int pending = 0;

	if (timer->dde_kit_timer) {
		pending = dde_kit_timer_pending(timer->dde_kit_timer) ? 1 : 0;
		dde_kit_timer_del(timer->dde_kit_timer);
		timer->dde_kit_timer = 0;
	}

	/* return 1 if timer was pending, 0 otherwise */
	return pending;
}

int del_timer_sync(struct timer_list *timer)
{
	return del_timer(timer);
}


int __mod_timer(struct timer_list *timer, unsigned long expires)
{
	/*
	 * XXX: Naive implementation. If we really need to be fast with
	 *      this function, we can implement a faster version inside
	 *      the DDE kit. Bjoern just does not think that this is the
	 *      case.
	 */
	int r;
	
	CHECK_INITVAR(dde_linux26_timer);
	r = del_timer(timer);

	timer->expires = expires;
	add_timer(timer);

	return (r > 0);
}


int mod_timer(struct timer_list *timer, unsigned long expires)
{
	return __mod_timer(timer, expires);
}


int timer_pending(const struct timer_list *timer)
{
	CHECK_INITVAR(dde_linux26_timer);
	/* There must be a valid DDE kit timer in the timer field
	 * *AND* it must be pending in the DDE kit.
	 */
	return ((timer->dde_kit_timer != 0) 
		    && dde_kit_timer_pending(timer->dde_kit_timer));
}


/**
 * msleep - sleep safely even with waitqueue interruptions
 * @msecs: Time in milliseconds to sleep for
 */
void msleep(unsigned int msecs)
{
	dde_kit_thread_msleep(msecs);
}

void msleep_interruptible(unsigned int msecs)
{
	CHECK_INITVAR(dde_linux26_timer);
	current->state = TASK_INTERRUPTIBLE;
	schedule_timeout(msecs);
}

void __const_udelay(unsigned long xloops)
{
	WARN_UNIMPL;
}


void __udelay(unsigned long usecs)
{
	dde_kit_thread_usleep(usecs);
}


void __ndelay(unsigned long nsecs)
{
	dde_kit_thread_nsleep(nsecs);
}


static void _init_timers(void *p)
{
	dde_linux26_process_add_worker("unused");
}


void dde_linux26_timer_init(void)
{
	dde_kit_timer_init(_init_timers, 0);

	INITIALIZE_INITVAR(dde_linux26_timer);
}


__attribute__((weak)) void do_gettimeofday (struct timeval *tv)
{
	tv->tv_sec  = jiffies / HZ;
	tv->tv_usec = (jiffies % HZ) * 1000000 / HZ;
}

struct timespec current_fs_time(struct super_block *sb)
{
	struct timespec now = {0,0};
	WARN_UNIMPL;
	return now;
}


unsigned long round_jiffies(unsigned long j)
{
	return j;
}
