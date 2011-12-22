/*
 * \brief   Kmem_cache implementation
 * \author  Christian Helmuth
 * \date    2007-01-22
 *
 * In Linux 2.6 this resides in mm/slab.c.
 *
 * I'll disregard the following function currently...
 *
 * extern struct kmem_cache *kmem_find_general_cachep(size_t size, gfp_t gfpflags);
 * extern void *kmem_cache_zalloc(struct kmem_cache *, gfp_t);
 */

/* Linux */
#include <linux/slab.h>

/* DDE kit */
#include <dde_kit/memory.h>
#include <dde_kit/lock.h>
#include <dde_kit/printf.h>


/*******************
 ** Configuration **
 *******************/

#define DEBUG_SLAB 0

#ifdef DEBUG_SLAB
# define DEBUG_SLAB_ALLOC 0
#else
# define DEBUG_SLAB_ALLOC 0
#endif

/*
 * Kmem cache structure
 */
struct kmem_cache
{
	const char          *name;               /* cache name */
	unsigned             size;               /* object size */

	struct dde_kit_slab *dde_kit_slab_cache; /* backing DDE kit cache */
	struct dde_kit_lock *cache_lock;         /* synchronize access to cache */
	void (*ctor)(void*, struct kmem_cache *, unsigned long); /* object constructor */
	void (*dtor)(void*, struct kmem_cache *, unsigned long); /* object destructor */
};


/**
 * Return size of objects in cache
 */
unsigned int kmem_cache_size(struct kmem_cache *cache)
{
	return cache->size;
}


/**
 * Return name of cache
 */
const char *kmem_cache_name(struct kmem_cache *cache)
{
	return cache->name;
}


/**
 * kmem_cache_shrink - Shrink a cache.
 * @cachep: The cache to shrink.
 *
 * Releases as many slabs as possible for a cache.
 * To help debugging, a zero exit status indicates all slabs were released.
 */
int kmem_cache_shrink(struct kmem_cache *cache)
{
	/* noop */
	return 1;
}


/**
 * kmem_cache_free - Deallocate an object
 * @cachep: The cache the allocation was from.
 * @objp: The previously allocated object.
 *
 * Free an object which was previously allocated from this
 * cache.
 */
void kmem_cache_free(struct kmem_cache *cache, void *objp)
{
	dde_kit_log(DEBUG_SLAB_ALLOC, "\"%s\" (%p)", cache->name, objp);

	if (cache->dtor)
		cache->dtor(objp, cache, 0);

	dde_kit_lock_lock(cache->cache_lock);
	dde_kit_slab_free(cache->dde_kit_slab_cache, objp);
	dde_kit_lock_unlock(cache->cache_lock);
}


/**
 * kmem_cache_alloc - Allocate an object
 * @cachep: The cache to allocate from.
 * @flags: See kmalloc().
 *
 * Allocate an object from this cache.  The flags are only relevant
 * if the cache has no available objects.
 */
void *kmem_cache_alloc(struct kmem_cache *cache, gfp_t flags)
{
	void *ret;

	dde_kit_log(DEBUG_SLAB_ALLOC, "\"%s\" flags=%x", cache->name, flags);

	dde_kit_lock_lock(cache->cache_lock);
	ret = dde_kit_slab_alloc(cache->dde_kit_slab_cache);
	dde_kit_lock_unlock(cache->cache_lock);

	/* return here in case of error */
	if (!ret) return 0;

	/* zero page if demanded */
	if (flags & __GFP_ZERO)
		memset(ret, 0, cache->size);

	if (cache->ctor)
		cache->ctor(ret, cache, SLAB_CTOR_CONSTRUCTOR);

	return ret;
}


/**
 * kmem_cache_destroy - delete a cache
 * @cachep: the cache to destroy
 *
 * Remove a struct kmem_cache object from the slab cache.
 * Returns 0 on success.
 *
 * It is expected this function will be called by a module when it is
 * unloaded.  This will remove the cache completely, and avoid a duplicate
 * cache being allocated each time a module is loaded and unloaded, if the
 * module doesn't have persistent in-kernel storage across loads and unloads.
 *
 * The cache must be empty before calling this function.
 *
 * The caller must guarantee that noone will allocate memory from the cache
 * during the kmem_cache_destroy().
 */
void kmem_cache_destroy(struct kmem_cache *cache)
{
	dde_kit_log(DEBUG_SLAB, "\"%s\"", cache->name);

	dde_kit_slab_destroy(cache->dde_kit_slab_cache);
	dde_kit_simple_free(cache);
}


/**
 * kmem_cache_create - Create a cache.
 * @name: A string which is used in /proc/slabinfo to identify this cache.
 * @size: The size of objects to be created in this cache.
 * @align: The required alignment for the objects.
 * @flags: SLAB flags
 * @ctor: A constructor for the objects.
 * @dtor: A destructor for the objects.
 *
 * Returns a ptr to the cache on success, NULL on failure.
 * Cannot be called within a int, but can be interrupted.
 * The @ctor is run when new pages are allocated by the cache
 * and the @dtor is run before the pages are handed back.
 *
 * @name must be valid until the cache is destroyed. This implies that
 * the module calling this has to destroy the cache before getting unloaded.
 *
 * The flags are
 *
 * %SLAB_POISON - Poison the slab with a known test pattern (a5a5a5a5)
 * to catch references to uninitialised memory.
 *
 * %SLAB_RED_ZONE - Insert `Red' zones around the allocated memory to check
 * for buffer overruns.
 *
 * %SLAB_HWCACHE_ALIGN - Align the objects in this cache to a hardware
 * cacheline.  This can be beneficial if you're counting cycles as closely
 * as davem.
 */
struct kmem_cache * kmem_cache_create(const char *name, size_t size, size_t align,
                                      unsigned long flags,
                                      void (*ctor)(void *, struct kmem_cache *, unsigned long),
                                      void (*dtor)(void *, struct kmem_cache *, unsigned long))
{
	dde_kit_log(DEBUG_SLAB, "\"%s\" obj_size=%d", name, size);

	struct kmem_cache *cache;

	if (!name) {
		printk("kmem_cache name reqeuired\n");
		return 0;
	}

	cache = dde_kit_simple_malloc(sizeof(*cache));
	if (!cache) {
		printk("No memory for slab cache\n");
		return 0;
	}

	/* Initialize a physically contiguous cache for kmem */
	if (!(cache->dde_kit_slab_cache = dde_kit_slab_init(size))) {
		printk("DDE kit slab init failed\n");
		dde_kit_simple_free(cache);
		return 0;
	}

	cache->name = name;
	cache->size = size;
	cache->ctor = ctor;
	cache->dtor = dtor;

	dde_kit_lock_init(&cache->cache_lock);

	return cache;
}
