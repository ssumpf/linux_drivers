/*
 * \brief   DDE Linux 2.6 audio
 * \author  Christian Helmuth
 * \date    2009-12-16
 */

/* Linux */
#include <linux/fs.h>
#include <linux/major.h>

/* ALSA */
#include <sound/driver.h>
#include <sound/core.h>
#include <sound/asound.h>
#include <sound/minors.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>

/* POSIX emulation */
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

/* Genode */
#include <dde_linux26/audio.h>


/**
 * ALSA-major file operations
 */
static const struct file_operations *alsa_fops;

/**
 * ALSA device structure
 */
struct _sound_device {
	int          fd;
	char         name[32];
	struct inode inode;
	struct file  file;
};

/**
 * ALSA devices (control, pcm, seq, timer) found during scan
 */
enum { MAX_DEVICES = 32 };
static struct _sound_device alsa_devices[MAX_DEVICES];


static struct _sound_device *get_dev_by_name(const char *name)
{
	int i;
	for (i = 0; i < MAX_DEVICES; i++) {
		if (!strcmp(alsa_devices[i].name, name))
			return &alsa_devices[i];
	}

	return 0;
}


static struct _sound_device *get_dev_by_fd(int fd)
{
	int i;
	for (i = 0; i < MAX_DEVICES; i++) {
		if (alsa_devices[i].fd == fd)
			return &alsa_devices[i];
	}

	return 0;
}


char *dde_linux26_get_hwbuf_by_fd(int fd)
{
	struct file *file = &(get_dev_by_fd(fd))->file;
	struct snd_pcm_file *pcm_file = file->private_data;
	struct snd_pcm_substream *stream = pcm_file->substream;

	return stream->runtime->dma_area;
}


/*******************
 ** Linux support **
 *******************/

int register_chrdev(unsigned int major, const char *name, const struct file_operations *fops)
{
	switch (major) {

	case SOUND_MAJOR:
		/* OSS file ops */
//		printk("SOUND_MAJOR registered fops %p\n", fops);
		/* oss_fops = fops; */
		break;

	case CONFIG_SND_MAJOR:
		/* ALSA file ops */
//		printk("SND_MAJOR registered fops %p\n", fops);
		alsa_fops = fops;
		break;

	default:
		printk("unknown chrdev registered fops %p major %d name %s\n", fops, major, name);
	}

	return 0;
}


int unregister_chrdev(unsigned int major, const char *name)
{
	return 0;
}


/*********************
 ** POSIX emulation **
 *********************/

/*
 * errno.h
 *
 * XXX errno definitely will not work for multiple threads, but we do not care
 * here.
 */
int errno;

/*
 * stdio.h
 */

FILE *stdin  = (FILE *) 0;
FILE *stdout = (FILE *) 1;
FILE *stderr = (FILE *) 2;

/*
 * fcntl.h
 */

int open(const char *file, int flags)
{
	struct _sound_device *dev = get_dev_by_name(file);
	if (!dev) {
		errno = ENOENT;
		return -1;
	}

	if ((flags & O_ACCMODE) != dev->file.f_flags) {
		errno = EACCES;
		return -1;
	}

//	if (flags & ~O_ACCMODE)
//		printk("open: ignoring flags %o\n", flags & ~O_ACCMODE);

	return dev->fd;
}

int fcntl(int fd, int cmd, ...)
{
	struct _sound_device *dev = get_dev_by_fd(fd);
	if (!dev) {
		errno = EBADF;
		return -1;
	}

	int ret = 0;
	va_list va;
	va_start(va, cmd);
	switch (cmd) {
	case F_SETFL:
		{
			int flags = va_arg(va, int);
//			if ((flags &= ~O_ACCMODE))
//				printk("fcntl: ignoring setting of flags %o\n", flags);
			break;
		}
	case F_GETFL:
		{
			ret = dev->file.f_flags;
			break;
		}
	default:
		printk("fcntl: unknown cmd %d\n", cmd);
		errno = EBADF;
		ret = -1;
	}
	va_end(va);

	return ret;
}

/*
 * string.h
 */

char *strerror(int err)
{
	return "error strings not supported";
}

/*
 * unistd.h
 */

ssize_t read(int fd, void *buf, size_t count)
{
	errno = EBADF;
	return -1;
}

int close(int fd)
{
	errno = EBADF;
	return -1;
}

int access(const char *file, int mode)
{
	if (!get_dev_by_name(file)) {
		errno = ENOENT;
		return -1;
	}

	return 0;
}

long sysconf(int name)
{
	switch (name) {
	case _SC_PAGE_SIZE:
		return PAGE_SIZE;

	default:
		errno = EINVAL;
		return -1;
	}
}

/*
 * sys/ioctl.h
 */

int ioctl(int fd, unsigned long int request, ...)
{
	struct _sound_device *dev = get_dev_by_fd(fd);
	if (!dev) {
		errno = EBADF;
		return -1;
	}

	va_list va;
	va_start(va, request);
	unsigned long arg = va_arg(va, unsigned long);
	int ret = dev->file.f_op->unlocked_ioctl(&dev->file, request, arg);
	va_end(va);

	if (ret) {
		errno = -ret;
		return -1;
	}

	return 0;
}

/*
 * sys/poll.h
 */

int poll(struct pollfd *fds, unsigned long nfds, int timeout)
{
	errno = EBADF;
	return -1;
}

/*
 * sys/mman.h
 */

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	errno = EBADF;
	return MAP_FAILED;
}

int munmap(void *addr, size_t length)
{
	errno = EINVAL;
	return -1;
}


/************************
 ** Internal audio API **
 ***********************/

int dde_linux26_audio_init_devices()
{
	int ret, card, dev_num = 0, pcms = 0;
	struct _sound_device *dev;

	/* scan for cards (resp. control devices) */
	for (card = 0; card < SNDRV_CARDS && dev_num < MAX_DEVICES; card++) {
		dev = &alsa_devices[dev_num];
		memset(dev, 0, sizeof(*dev));
		dev->file.f_flags = O_RDWR;
		dev->file.f_mode  = FMODE_READ | FMODE_WRITE;
		dev->inode.i_rdev = MKDEV(CONFIG_SND_MAJOR,
		                          SNDRV_MINOR(card, SNDRV_MINOR_CONTROL));

		/* open will set new file operations */
		int ret = alsa_fops->open(&dev->inode, &dev->file);
		if (ret < 0) continue;

		snprintf(dev->name, sizeof(dev->name), "/dev/snd/controlC%d", card);
		dev->fd = dev_num + 3;
		printk("%s dev %p (%d) f_op %p\n", dev->name, dev, dev->fd, dev->file.f_op);
		dev_num++;

		/* scan for playback pcms of this card */
		int pcm;
		for (pcm = SNDRV_MINOR_PCM_PLAYBACK;
		     pcm < SNDRV_MINOR_PCM_PLAYBACK + SNDRV_MINOR_PCMS && dev_num < MAX_DEVICES;
		     pcm++) {
			dev = &alsa_devices[dev_num];
			memset(dev, 0, sizeof(*dev));
			dev->file.f_flags = O_RDWR;
			dev->file.f_mode  = FMODE_WRITE;
			dev->inode.i_rdev = MKDEV(CONFIG_SND_MAJOR,
			                          SNDRV_MINOR(card, pcm));

			/* open will set new file operations */
			int ret = alsa_fops->open(&dev->inode, &dev->file);
			if (ret < 0) continue;

			snprintf(dev->name, sizeof(dev->name), "/dev/snd/pcmC%dD%dp",
			         card, pcm - SNDRV_MINOR_PCM_PLAYBACK);
			dev->fd = dev_num + 3;
			printk("\t%s dev %p (%d) f_op %p\n", dev->name, dev, dev->fd, dev->file.f_op);
			dev_num++; pcms++;
		}
	}

	return pcms;
}
