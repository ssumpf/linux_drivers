/*
 * \brief  DDE Linux 2.6 Memory helpers
 * \author Björn Döbel
 * \author Christian Helmuth
 * \date   2008-11-11
 */

#ifndef _DDE_LINUX26__MEM_H_
#define _DDE_LINUX26__MEM_H_

#include <linux/mm.h>

void dde_linux26_page_cache_add(struct page *p);
void dde_linux26_page_cache_remove(struct page *p);
struct page* dde_linux26_page_lookup(unsigned long va);

#endif /* _DDE_LINUX26__MEM_H_ */
