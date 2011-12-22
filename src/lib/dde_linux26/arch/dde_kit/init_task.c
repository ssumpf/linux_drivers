/*
 * \brief  DDE Linux 2.6 task init
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 */

#include <asm/desc.h>
#include <asm/pgtable.h>
#include <asm/uaccess.h>

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/kernel.h>
#include <linux/mqueue.h>
#include <linux/module.h>
#include <linux/personality.h>

#include "local.h"

/* init task */
struct task_struct init_task;

/* From kernel/pid.c */
#define BITS_PER_PAGE           (PAGE_SIZE*8)
#define BITS_PER_PAGE_MASK      (BITS_PER_PAGE-1)

/* From init/main.c */
enum system_states system_state;
EXPORT_SYMBOL(system_state);

struct fs_struct        init_fs      = INIT_FS;
struct files_struct     init_files   = INIT_FILES;
struct signal_struct    init_signals = INIT_SIGNALS(init_signals);
struct sighand_struct   init_sighand = INIT_SIGHAND(init_sighand);
struct mm_struct        init_mm      = INIT_MM(init_mm);
pgd_t swapper_pg_dir[1024];
union thread_union init_thread_union = { INIT_THREAD_INFO(init_task) };
struct group_info init_groups = {.usage = ATOMIC_INIT(2)};
struct nsproxy init_nsproxy = INIT_NSPROXY(init_nsproxy);

struct user_struct root_user = {
	.__count    = ATOMIC_INIT(1),
	.processes  = ATOMIC_INIT(1),
	.files      = ATOMIC_INIT(0),
	.sigpending = ATOMIC_INIT(0),
	.mq_bytes   = 0,
	.locked_shm = 0,
};

struct ipc_namespace init_ipc_ns = {
	.kref = {
		.refcount = ATOMIC_INIT(2),
	},
};

struct uts_namespace init_uts_ns = {
	.kref = {
		.refcount   = ATOMIC_INIT(2),
	},
	.name = {
		.sysname    = "DDE",
		.nodename   = "",
		.release    = "2.6",
		.version    = "19",
		.machine    = "",
		.domainname = "",
	},
};

struct exec_domain default_exec_domain = {
	.name       = "Linux",      /* name */
	.handler    = NULL,         /* no signaling! */
	.pers_low   = 0,            /* PER_LINUX personality. */
	.pers_high  = 0,            /* PER_LINUX personality. */
	.signal_map = 0,            /* Identity map signals. */
	.signal_invmap  = 0,        /*  - both ways. */
};

/* copy of the initial task struct */
struct task_struct 	init_task 	= INIT_TASK(init_task);
/* copy of the initial thread info (which contains init_task) */
struct thread_info  init_thread = INIT_THREAD_INFO(init_task);

long do_no_restart_syscall(struct restart_block *param)
{
	return -EINTR;
}
