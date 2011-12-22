#ifndef _POSIX__UNISTD_H_
#define _POSIX__UNISTD_H_

#include <asm/page.h>
#include <linux/types.h>
#include <linux/fs.h>

#define R_OK MAY_READ
#define W_OK MAY_WRITE
#define X_OK MAY_EXEC
#define F_OK 0

extern ssize_t read(int fd, void *buf, size_t count);
extern int close(int fd);
extern int access(const char *path, int mode);

enum { _SC_PAGE_SIZE };

extern long sysconf(int name);

#endif /* _POSIX__UNISTD_H_ */
