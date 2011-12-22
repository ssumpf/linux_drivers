#include <linux/cpumask.h>

#include "local.h"

/** Available CPUs (NUM_CPUS bits) */
cpumask_t cpu_possible_map = CPU_MASK_ALL;
/** Online CPUs */
cpumask_t cpu_online_map = CPU_MASK_CPU0;
/** Present CPUs */
cpumask_t cpu_present_map = CPU_MASK_CPU0;

/** Send reschedule to another CPU.
 *
 * We don't need this for now.
 */
void smp_send_reschedule(int cpu)
{
}
