/*
 * \brief  DDE Linux 2.6 signals
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 *
 * This is just a dummy signal implementation to satisfy contrib code
 * dependencies.
 */

#include "local.h"


int sigprocmask(int how, sigset_t *set, sigset_t *oldset)
{
	return 0;
}


void flush_signals(struct task_struct *t)
{
}


int do_sigaction(int sig, struct k_sigaction *act, struct k_sigaction *oact)
{
	return 0;
}
