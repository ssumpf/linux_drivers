/*
 * \brief  DDE Linux 2.6 generals
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 */

#ifndef _DDE_LINUX26__GENERAL_H_
#define _DDE_LINUX26__GENERAL_H_

#include <dde_kit/thread.h>

#define WARN_UNIMPL         printk("unimplemented: %s\n", __FUNCTION__)

/**
 * Add calling thread to DDE Linux 2.6
 *
 * This needs to be called for every thread that will issue calls
 * to Linux functions.
 *
 * \param name  is set as thread name if thread was created externally,
 *              otherwise unused
 */
void dde_linux26_process_add_worker(const char *name);

/**
 * Add existing DDE kit thread to DDE Linux 2.6
 *
 * \param thread  existing DDE kit thread handle
 *
 * Existing DDE kit threads (e.g., timers) that will end up running Linux code
 * (e.g. timer functions) must explicitly added to DDE Linux 2.6 process
 * subsystem.
 */
int dde_linux26_process_from_dde_kit(struct dde_kit_thread *thread);

/**
 * Perform vital initcalls to initialize DDE Linux 2.6
 *
 * This includes initialization of the DDE kit and adoption of the calling
 * thread as DDE main thread. Note, only DDE threads may execute Linux code.
 */
void dde_linux26_init(void);

#endif /* _DDE_LINUX26__GENERAL_H_ */
