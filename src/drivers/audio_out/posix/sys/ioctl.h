#ifndef _POSIX__SYS__IOCTL_H_
#define _POSIX__SYS__IOCTL_H_

#include <asm/ioctl.h>

extern int ioctl(int fd, unsigned long int request, ...);

#endif /* _POSIX__SYS__IOCTL_H_ */
