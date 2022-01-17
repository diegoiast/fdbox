#ifndef STD_EXTRA
#define STD_EXTRA

#include <stddef.h>
#include "fdbox.h"

#if defined(__TURBOC__)
/* This came in C99, as we do not have it - emulate it lamely */
int snprintf(char *str, size_t size, const char *format, ...);
#define strcasecmp strcmpi
#endif

#if defined(__TURBOC__) || defined(__WIN32__)
#define setenv setenv_impl
#endif

int setenv_impl(const char *name, const char *value, int overwrite);
void clear_env();

#endif
