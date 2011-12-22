#ifndef _POSIX__SYS__MMAN_H_
#define _POSIX__SYS__MMAN_H_

#include <asm/mman.h>

#define MAP_FAILED ((void *) -1)

extern void *mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);
extern int munmap(void *addr, size_t length);


#endif /* _POSIX__SYS__MMAN_H_ */
