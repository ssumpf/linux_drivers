#ifndef _POSIX__STDIO_H_
#define _POSIX__STDIO_H_

#include <stdlib.h>
#include <stdint.h>

typedef int FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define fopen(a, b)         (NULL)
#define fprintf(f, args...) printk(args)
#define fputs(s, f)         printk("%s", s)
#define putc(c, f)          printk("%c", c)

#endif /* _POSIX__STDIO_H_ */
