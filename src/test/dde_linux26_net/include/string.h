/*
 * \brief  Declarations of string functions and common definitions used by uIP
 * \author Christian Helmuth
 * \date   2009-02-21
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <inttypes.h>

#ifndef NULL
#define NULL (void *)0
#endif

void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);

#endif /* _STRING_H_ */
