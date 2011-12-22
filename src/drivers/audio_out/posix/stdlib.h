#ifndef _POSIX__STDLIB_H_
#define _POSIX__STDLIB_H_

#include <linux/stddef.h>
#include <linux/slab.h>

static inline void *calloc(size_t n, size_t size)
{
	size_t *ptr = kzalloc(n * size + sizeof(size_t), GFP_KERNEL);
	*ptr = size;
	return ++ptr;
}

static inline void free(void *p)
{
	if (!p) return;

	size_t *ptr = (size_t *)p - 1;
	kfree(ptr);
}

static inline void *malloc(size_t size)
{
	size_t *ptr = kmalloc(size + sizeof(size_t), GFP_KERNEL);
	*ptr = size;
	return ++ptr;
}

static inline void *realloc(void *ptr, size_t new_size)
{
	if (ptr == 0)
		return malloc(new_size);

	size_t old_size = *((size_t *)ptr--);
	if (new_size <= old_size)
		return ptr;

	void *nptr = malloc(new_size);
	if (!nptr)
		return 0;
	memcpy(nptr, ptr, old_size);
	free(ptr);

	return nptr;
}

static inline void *alloca(size_t size)
{
	return __builtin_alloca(size);
}

static inline void qsort(void *base, size_t nmemb, size_t size,
                         int(*compar)(const void *, const void *))
{ }

#endif /* _POSIX__STDLIB_H_ */
