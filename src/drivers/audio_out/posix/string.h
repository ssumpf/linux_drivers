#ifndef _POSIX__STRING_H_
#define _POSIX__STRING_H_

#include <linux/string.h>

static inline char *strdup(const char *s)
{
	return kstrdup(s, GFP_KERNEL);
}

static inline int strcasecmp(const char *s1, const char *s2)
{
	size_t s1_len = strlen(s1);
	size_t s2_len = strlen(s2);

	return strnicmp(s1, s2, s1_len > s2_len ? s2_len : s1_len);
}

extern char *strerror(int err);

#endif /* _POSIX__STRING_H_ */
