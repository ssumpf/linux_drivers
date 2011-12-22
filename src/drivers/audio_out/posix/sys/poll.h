#ifndef _POSIX__SYS__POLL_H_
#define _POSIX__SYS__POLL_H_

#include <asm/poll.h>

extern int poll(struct pollfd *fds, unsigned long nfds, int timeout);

#endif /* _POSIX__SYS__POLL_H_ */
