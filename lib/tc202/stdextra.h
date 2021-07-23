#ifndef STD_EXTRA
#define STD_EXTRA

#include <stddef.h>

#if defined(__TURBOC__) || defined(HI_TECH_C)
/* This came in C99, as we do not have it - emulate it lamely */
int snprintf(char *str, size_t size, const char *format, ...);
#define strcasecmp strcmpi
#endif

#if defined(__TURBOC__) || defined(HI_TECH_C) || defined(__WIN32__)
#define setenv setenv_impl
#endif

int setenv_impl(const char *name, const char *value, int overwrite);
void clear_env();

/* Thank you pacific C for being so lame */
#if defined(HI_TECH_C)
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

extern int	errno;			/* system error number */

/* OMG, this is so wrong...*/
#define strtol(a,b,c) atoi(a)

// OMG, I don't know why its missing
int putenv(char *string);

// This is starting to be lame
int strcasecmp(const char *s1, const char *s2);

#endif

#endif
