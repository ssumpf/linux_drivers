#ifndef _I386_CURRENT_H
#define _I386_CURRENT_H

#include <asm/pda.h>
#include <linux/compiler.h>

struct task_struct;

#ifndef DDE_LINUX
static __always_inline struct task_struct *get_current(void)
{
	return read_pda(pcurrent);
}
#else
struct task_struct *get_current(void);
#endif

 
#define current get_current()

#endif /* !(_I386_CURRENT_H) */
