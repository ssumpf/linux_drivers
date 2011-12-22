/*
 * \brief   kmalloc() implementation
 * \author  Christian Helmuth
 * \date    2007-01-24
 *
 * In Linux 2.6 this resides in mm/slab.c.
 *
 * This implementation of kmalloc() stays with Linux's and uses kmem_caches for
 * some power of two bytes. For larger allocations ddedkit_large_malloc() is
 * used. This way, we optimize for speed and potentially waste memory
 * resources.
 */

/* Linux */
#include <linux/slab.h>
#include <linux/bootmem.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <asm/io.h>

/* DDE kit */
#include <dde_kit/memory.h>
#include <dde_kit/printf.h>

#include <dde_linux26/general.h>

/* This stuff is needed by some drivers, e.g. for ethtool.
 * XXX: This is a fake, implement it if you really need ethtool stuff.
 */
struct page* mem_map = NULL;
static bootmem_data_t contig_bootmem_data;
struct pglist_data contig_page_data = { .bdata = &contig_bootmem_data };

int remap_pfn_range(struct vm_area_struct *vma, unsigned long addr,
		    unsigned long pfn, unsigned long size, pgprot_t prot)
{
	return 0;
}
EXPORT_SYMBOL(remap_pfn_range);

/*******************
 ** Configuration **
 *******************/

#define DEBUG_MALLOC 0

/********************
 ** Implementation **
 ********************/

/*
 * These are the default caches for kmalloc. Custom caches can have other sizes.
 */
static struct cache_sizes malloc_sizes[] = {
#define CACHE(x) { .cs_size = (x) },
#include <linux/kmalloc_sizes.h>
	CACHE(ULONG_MAX)
#undef CACHE
};


/*
 * kmalloc() cache names
 */
static const char *malloc_names[] = {
#define CACHE(x) "size-" #x,
#include <linux/kmalloc_sizes.h>
	NULL
#undef CACHE
};


/**
 * Find kmalloc() cache for size
 */
static struct kmem_cache *find_cache(size_t size)
{
	struct cache_sizes *sizes;

	for (sizes = malloc_sizes; size > sizes->cs_size; ++sizes) ;

	return sizes->cs_cachep;
}


/**
 * Free previously allocated memory
 * @objp: pointer returned by kmalloc.
 *
 * If @objp is NULL, no operation is performed.
 *
 * Don't free memory not originally allocated by kmalloc()
 * or you will run into trouble.
 */
void kfree(const void *objp)
{
	if (!objp) return;

	/* find cache back-pointer */
	void **p = (void **)objp - 1;

	dde_kit_log(DEBUG_MALLOC, "objp=%p cache=%p (%d)",
	            p, *p, *p ? kmem_cache_size(*p) : 0);

	if (*p)
		/* free from cache */
		kmem_cache_free(*p, p);
	else
		/* no cache for this size - use dde_kit free */
		dde_kit_large_free(p);
}


/**
 * Allocate memory
 * @size: how many bytes of memory are required.
 * @flags: the type of memory to allocate.
 *
 * kmalloc is the normal method of allocating memory
 * in the kernel.
 */
void *__kmalloc(size_t size, gfp_t flags)
{
	/* add space for back-pointer */
	size += sizeof(void *);

	/* find appropriate cache */
	struct kmem_cache *cache = find_cache(size);

	void **p;
	if (cache)
		/* allocate from cache */
		p = kmem_cache_alloc(cache, flags);
	else
		/* no cache for this size - use dde_kit malloc */
		p = dde_kit_large_malloc(size);

	dde_kit_log(DEBUG_MALLOC, "size=%d, cache=%p (%d) => %p",
	           size, cache, cache ? kmem_cache_size(cache) : 0, p);

	/* return here in case of error */
	if (!p) return 0;

	/* zero page if demanded and no cache was used */
	if ((flags & __GFP_ZERO) && !cache)
		memset(p, 0, size);

	/* store back pointer */
	*p = cache;

	/* return pointer to actual chunk */
	return ++p;
}


void *dma_alloc_coherent(struct device *dev, size_t size, 
                         dma_addr_t *dma_handle, gfp_t flag)
{
	void *ret = (void *)__get_free_pages(flag, get_order(size));

	if (ret != NULL) {
		memset(ret, 0, size);
		*dma_handle = virt_to_bus(ret);
	}
	return ret;
}


void dma_free_coherent(struct device *dev, size_t size,
                       void *vaddr, dma_addr_t dma_handle)
{
	free_pages((unsigned long)vaddr, get_order(size));
}


/********************
 ** Initialization **
 ********************/

/**
 * kmalloc initialization
 */
void dde_linux26_kmalloc_init(void)
{
	struct cache_sizes  *sizes = malloc_sizes;
	const char         **names = malloc_names;

	/* init malloc sizes array */
	for (; sizes->cs_size != ULONG_MAX; ++sizes, ++names)
		sizes->cs_cachep = kmem_cache_create(*names, sizes->cs_size, 0, 0, 0, 0);
}
