#ifndef _POSIX__FCNTL_H_
#define _POSIX__FCNTL_H_

#include <asm/fcntl.h>

#define O_ASYNC FASYNC

extern int open(const char *file, int flags);
extern int fcntl(int fd, int cmd, ...);

#endif /* _POSIX__FCNTL_H_ */
