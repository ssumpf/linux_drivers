/*
 * \brief   DDE Linux 2.6 virtual memory helpers
 * \author  Christian Helmuth
 * \date    2009-12-29
 */

/* Linux */
#include <linux/vmalloc.h>
#include <asm/pgtable.h>

/* DDE kit */
#include <dde_kit/memory.h>
#include <dde_kit/lock.h>

static struct vm_struct *__get_vm_area_node(unsigned long size, unsigned long flags,
					    unsigned long start, unsigned long end,
					    int node, gfp_t gfp_mask)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n", __FUNCTION__);
	return NULL;
}

struct vm_struct *__get_vm_area(unsigned long size, unsigned long flags,
				unsigned long start, unsigned long end)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n", __FUNCTION__);
	return NULL;
//	return __get_vm_area_node(size, flags, start, end, -1, GFP_KERNEL);
}

/**
 *	get_vm_area  -  reserve a contingous kernel virtual area
 *	@size:		size of the area
 *	@flags:		%VM_IOREMAP for I/O mappings or VM_ALLOC
 *
 *	Search an area of @size in the kernel virtual mapping area,
 *	and reserved it for out purposes.  Returns the area descriptor
 *	on success or %NULL on failure.
 */
struct vm_struct *get_vm_area(unsigned long size, unsigned long flags)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n", __FUNCTION__);
	return NULL;
	//return __get_vm_area(size, flags, VMALLOC_START, VMALLOC_END);
}

struct vm_struct *get_vm_area_node(unsigned long size, unsigned long flags,
				   int node, gfp_t gfp_mask)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n", __FUNCTION__);
	return NULL;
//	return __get_vm_area_node(size, flags, VMALLOC_START, VMALLOC_END, node,
//				  gfp_mask);
}

/* Caller must hold vmlist_lock */
static struct vm_struct *__find_vm_area(void *addr)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n", __FUNCTION__);
//	struct vm_struct *tmp;
//
//	for (tmp = vmlist; tmp != NULL; tmp = tmp->next) {
//		 if (tmp->addr == addr)
//			break;
//	}
//	return tmp;
	return NULL;
}

/* Caller must hold vmlist_lock */
static struct vm_struct *__remove_vm_area(void *addr)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n", __FUNCTION__);
	return NULL;

}

/**
 *	remove_vm_area  -  find and remove a contingous kernel virtual area
 *	@addr:		base address
 *
 *	Search for the kernel VM area starting at @addr, and remove it.
 *	This function returns the found VM area, but using it is NOT safe
 *	on SMP machines, except for its size or flags.
 */
struct vm_struct *remove_vm_area(void *addr)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n", __FUNCTION__);
			//TODO AWE
	return NULL;
}

void __vunmap(void *addr, int deallocate_pages)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n", __FUNCTION__);
	return;
}


/**
 *	vunmap  -  release virtual mapping obtained by vmap()
 *	@addr:		memory base address
 *
 *	Free the virtually contiguous memory area starting at @addr,
 *	which was created from the page array passed to vmap().
 *
 *	Must not be called in interrupt context.
 */
void vunmap(void *addr)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n", __FUNCTION__);
}

/**
 *	vmap  -  map an array of pages into virtually contiguous space
 *	@pages:		array of page pointers
 *	@count:		number of pages to map
 *	@flags:		vm_area->flags
 *	@prot:		page protection for the mapping
 *
 *	Maps @count pages from @pages into contiguous kernel virtual
 *	space.
 */
void *vmap(struct page **pages, unsigned int count,
		unsigned long flags, pgprot_t prot)
{
	printk(KERN_WARNING "vmalloc.c::%s UNIMPL\n%p %p %p %p %p %p %p\n", __FUNCTION__,
	       __builtin_return_address(0),
	       __builtin_return_address(1),
	       __builtin_return_address(2),
	       __builtin_return_address(3),
	       __builtin_return_address(4),
	       __builtin_return_address(5),
	       __builtin_return_address(6));
	return NULL;
}

