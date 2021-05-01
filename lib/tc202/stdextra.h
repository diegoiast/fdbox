#ifndef STD_EXTRA
#define STD_EXTRA

#include <stddef.h>

/* This came in C99, as we do not have it - emulate it lamely */
int snprintf(char *str, size_t size, const char *format, ...);

#define strcasecmp strcmpi

#endif
