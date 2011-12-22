#ifndef _ASMi386_PARAM_H
#define _ASMi386_PARAM_H

#ifdef DDE_LINUX
/*
 * Including <dde_kit/timer.h> would be best here, but DDE_KIT_HZ is not a
 * macro but an enum. So, we rely on DDE_KIT_HZ == 100 here!
 */
#undef CONFIG_HZ
#define CONFIG_HZ 100
#endif

#ifdef __KERNEL__
# define HZ		CONFIG_HZ	/* Internal kernel timer frequency */
# define USER_HZ	100		/* .. some user interfaces are in "ticks" */
# define CLOCKS_PER_SEC		(USER_HZ)	/* like times() */
#endif

#ifndef HZ
#define HZ 100
#endif

#define EXEC_PAGESIZE	4096

#ifndef NOGROUP
#define NOGROUP		(-1)
#endif

#define MAXHOSTNAMELEN	64	/* max length of hostname */

#endif
