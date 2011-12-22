/*
 * \brief   Hardware-interrupt support
 * \author  Christian Helmuth
 * \date    2007-02-12
 *
 * XXX Consider support for IRQ_HANDLED and friends (linux/irqreturn.h)
 */

/* Linux */
#include <linux/interrupt.h>
#include <linux/string.h>     /* memset() */

/* DDE kit */
#include <dde_kit/interrupt.h>
#include <dde_kit/memory.h>

#include <dde_linux26/general.h>

/* local */
#include "local.h"

/* dummy */
irq_cpustat_t irq_stat[CONFIG_NR_CPUS];

/**
 * IRQ handling data
 */
static struct dde_linux26_irq
{
	unsigned                irq;     /* IRQ number */
	unsigned                count;   /* usage count */
	int                     shared;  /* shared IRQ */
	struct dde_kit_thread  *thread;  /* DDE kit interrupt thread */
	struct irqaction       *action;  /* Linux IRQ action */

	struct dde_linux26_irq *next;    /* next DDE IRQ */
} *used_irqs;


static void irq_thread_init(void *p)
{
	dde_linux26_process_add_worker("unused");
}


static void irq_handler(void *arg)
{
	struct dde_linux26_irq *irq = arg;
	struct irqaction *action;

#if 0
	DEBUG_MSG("irq 0x%x", irq->irq);
#endif
	/* interrupt occurred - call all handlers */
	for (action = irq->action; action; action = action->next)
		action->handler(action->irq, action->dev_id);

	/* upon return we check for pending soft irqs */
	if (local_softirq_pending())
		dde_linux26_raise_softirq();
}


/*****************************
 ** IRQ handler bookkeeping **
 *****************************/

/**
 * Claim IRQ
 *
 * \return usage counter or negative error code
 *
 * FIXME list locking
 * FIXME are there more races?
 */
static int claim_irq(struct irqaction *action)
{
	int shared = action->flags & SA_SHIRQ ? 1 : 0;
	struct dde_linux26_irq *irq;

	/* check if IRQ already used */
	for (irq = used_irqs; irq; irq = irq->next)
		if (irq->irq == action->irq) break;

	/* we have to setup IRQ handling */
	if (!irq) {
		/* allocate and initalize new descriptor */
		irq = dde_kit_simple_malloc(sizeof(*irq));
		if (!irq) return -ENOMEM;
		memset(irq, 0, sizeof(*irq));

		irq->irq    = action->irq;
		irq->shared = shared;
		irq->next   = used_irqs;
		used_irqs   = irq;

		/* attach to interrupt */
		int ret = dde_kit_interrupt_attach(irq->irq,
		                                   irq->shared,
		                                   irq_thread_init,
		                                   irq_handler,
		                                   (void *)irq);
		if (ret) {
			dde_kit_simple_free(irq);
			return -EBUSY;
		}
	}

	/* does desciptor allow our new handler? */
	if ((!irq->shared || !shared) && irq->action) return -EBUSY;

	/* add handler */
	irq->count++;
	action->next = irq->action;
	irq->action = action;

	return irq->count;
}


/**
 * Free previously claimed IRQ
 *
 * \return usage counter or negative error code
 */
static int release_irq(unsigned irq_num)
{
	WARN_UNIMPL;

	return -EPERM;
}


/***************
 ** Linux API **
 ***************/

/**
 * Request interrupt
 *
 * \param  irq       interrupt number
 * \param  handler   interrupt handler -> top half
 * \param  flags     interrupt handling flags (SA_SHIRQ, ...)
 * \param  dev_name  device name
 * \param  dev_id    cookie passed back to handler
 *
 * \return 0 on success; error code otherwise
 *
 * \todo FIXME consider locking!
 */
int request_irq(unsigned int irq, irq_handler_t handler,
                unsigned long flags, const char *dev_name, void *dev_id)
{
  if (!handler) return -EINVAL;

  /* facilitate Linux irqaction for this handler */
  struct irqaction *irq_action = dde_kit_simple_malloc(sizeof(*irq_action));
  if (!irq_action) return -ENOMEM;
  memset(irq_action, 0, sizeof(*irq_action));

  irq_action->handler = handler;
  irq_action->flags   = flags;
  irq_action->name    = dev_name;
  irq_action->dev_id  = dev_id;
  irq_action->irq     = irq;

  /* attach to IRQ */
  int err = claim_irq(irq_action);
  if (err < 0) return err;

  return 0;
}


/** Release Interrupt
 * \ingroup mod_irq
 *
 * \param  irq     interrupt number
 * \param  dev_id  cookie passed back to handler
 *
 */
void free_irq(unsigned int irq, void *dev_id)
{
	/* FIXME Release IRQ */
	/* FIXME Maybe, stop IRQ thread */

	WARN_UNIMPL;
}


void disable_irq(unsigned int irq)
{
	dde_kit_interrupt_disable(irq);
}


void disable_irq_nosync(unsigned int irq)
{
	/*
	 * Note:
	 * In contrast to the _nosync semantics, DDE kit's
	 * disable definitely waits until a currently executed
	 * IRQ handler terminates.
	 */
	dde_kit_interrupt_disable(irq);
}


void enable_irq(unsigned int irq)
{
	dde_kit_interrupt_enable(irq);
}
