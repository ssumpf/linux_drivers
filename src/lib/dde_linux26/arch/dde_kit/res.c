#include "local.h"

#include <linux/ioport.h>



/** Request an IO port region.
 *
 * \param start start port
 * \param n     number of ports
 * \param name  name of allocator (unused)
 *
 * \return NULL    error
 * \return !=NULL  success
 *
 * \bug Since no one in Linux uses this function's return value,
 *      we do not allocate and fill a resource struct.
 */
struct resource *request_region(resource_size_t start,
                                resource_size_t n,
                                const char *name)
{
	int err = dde_kit_request_io(start, n);

	if (err)
		return NULL;

	return (struct resource *)1;
}


/** List of memory regions that have been requested. This is used to
 *  perform ioremap() and iounmap()
 */
static LIST_HEAD(dde_linux26_mem_regions);

/** va->pa mapping used to store memory regions */
struct dde_linux26_mem_region {
	dde_kit_addr_t pa;
	dde_kit_addr_t va;
	unsigned int size;
	struct list_head list;
};

/** Request an IO memory region.
 *
 * \param start start address
 * \param n     size of memory area
 * \param name  name of allocator (unused)
 *
 * \return NULL	error
 * \return !=NULL success
 *
 * \bug Since no one in Linux uses this function's return value,
 *      we do not allocate and fill a resource struct.
 */
struct resource *request_mem_region(resource_size_t start,
                                    resource_size_t n,
                                    const char *name)
{
	dde_kit_addr_t va;
	struct dde_linux26_mem_region *mreg;

	int i = dde_kit_request_mem(start, n, 0, &va);

	if (i) {
		dde_kit_printf("request_mem_region() failed (start %lx, size %zx)",
		               (dde_kit_addr_t) start, (dde_kit_size_t) n);
		return NULL;
	}

	mreg = kmalloc(sizeof(struct dde_linux26_mem_region), GFP_KERNEL);
	dde_kit_assert(mreg);

	mreg->pa = start;
	mreg->va = va;
	mreg->size = n;
	list_add(&mreg->list, &dde_linux26_mem_regions);

//	dde_kit_pgtab_set_region_with_size((void *)va, start, n, 0);

	return (struct resource *)1;
}


/** Release IO port region.
  */
void release_region(resource_size_t start, resource_size_t n)
{
	/* FIXME: we need a list of "struct resource"s that have been
	 *        allocated by request_region() and then need to
	 *        free this stuff here! */
	dde_kit_release_io(start, n);
}


/** Release IO memory region.
 */
void release_mem_region(resource_size_t start, resource_size_t n)
{
	dde_kit_release_mem(start, n);
//	dde_kit_pgtab_clear_region((void *)(dde_kit_addr_t)start);
}


int check_region(resource_size_t start, resource_size_t n)
{
	WARN_UNIMPL;
	return -1;
}


int check_mem_region(resource_size_t start, resource_size_t n)
{
	WARN_UNIMPL;
	return -2;
}


/** Map physical I/O region into virtual address space.
 *
 * For our sake, this only returns the virtual address belonging to
 * the physical region, since we don't manage page tables ourselves.
 */
void __iomem * ioremap(unsigned long phys_addr, unsigned long size)
{
	struct list_head *pos, *head;
	head = &dde_linux26_mem_regions;

	list_for_each(pos, head) {
		struct dde_linux26_mem_region *mreg = list_entry(pos,
		                                                 struct dde_linux26_mem_region,
		                                                 list);
		if (mreg->pa >= phys_addr && mreg->pa + mreg->size > phys_addr)
			return (void *)(mreg->va + (phys_addr - mreg->pa));
	}

	return NULL;
}


void __iomem * ioremap_nocache(unsigned long offset, unsigned long size)
{
	return ioremap(offset, size);
}


void iounmap(volatile void __iomem *addr)
{
	WARN_UNIMPL;
}
