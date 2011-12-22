/******************************************************************************
 * Bjoern Doebel <doebel@tudos.org>                                           *
 *                                                                            *
 * (c) 2005 - 2007 Technische Universitaet Dresden                            *
 * This file is part of DROPS, which is distributed under the terms of the    *
 * GNU General Public License 2. Please see the COPYING file for details.     *
 ******************************************************************************/

/*
 * \brief    vmalloc implementation
 * \author   Bjoern Doebel
 * \date     2007-07-30
 */

/* Linux */
#include <linux/vmalloc.h>

/* DDE kit */
#include <dde_kit/memory.h>
#include <dde_kit/lock.h>

void *vmalloc(unsigned long size)
{
	return dde_kit_simple_malloc(size);
}

void vfree(void *addr)
{
	dde_kit_simple_free(addr);
}
