/*
 * \brief   Linux-kernel printf() variants
 * \author  Christian Helmuth
 * \date    2009-01-28
 */

#include <linux/kernel.h>

#include <dde_kit/printf.h>
#include <dde_kit/lock.h>

/**
 * Mutex for printk message buffer
 */
static struct dde_kit_lock *printk_lock;


asmlinkage int printk(const char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);

	return r;
}


asmlinkage int vprintk(const char *fmt, va_list args)
{
	int len;
	static char buf[1024];

	dde_kit_lock_lock(printk_lock);

	len = vscnprintf(buf, sizeof(buf), fmt, args);
	dde_kit_print(buf);

	dde_kit_lock_unlock(printk_lock);

	return len;
}


void dde_linux26_printk_init(void)
{
	dde_kit_lock_init(&printk_lock);
}
