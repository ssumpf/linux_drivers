/*
 * \brief   DDE Linux 2.6 misc glue code
 * \author  Christian Helmuth
 * \date    2009-12-29
 */

#include <linux/time.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/jiffies.h>


void getnstimeofday(struct timespec *ts)
{
	jiffies_to_timespec(jiffies, ts);
}


/*
 * This is used by subsystems that don't want seekable
 * file descriptors
 */
int nonseekable_open(struct inode *inode, struct file *filp)
{
	filp->f_mode &= ~(FMODE_LSEEK | FMODE_PREAD | FMODE_PWRITE);
	return 0;
}


loff_t no_llseek(struct file *file, loff_t offset, int origin)
{
	return -ESPIPE;
}


void kill_fasync(struct fasync_struct **s, int a, int b)
{
}


int fasync_helper(int a, struct file *f, int b, struct fasync_struct **s)
{
	return 0;
}


struct file fastcall * fget(unsigned int fd)
{
	return 0;
}


void fastcall fput(struct file *s)
{
}
