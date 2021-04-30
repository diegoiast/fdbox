#ifndef STD_EXTRA
#define STD_EXTRA

#include <stddef.h>

/* This came in C99, as we do not have it - emulate it lamely */
int snprintf(char *str, size_t size, const char *format, ...);

#define strcasecmp strcmpi

#if defined(__WIN32__)
/* This function is available on Linux, but now on Windows */
char *strsep(char **stringp, const char *delim) {
#endif

#endif
