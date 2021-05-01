/*
This file is part of fdbox
For license - read license.txt
*/

#ifndef __STR_EXTRA__
#define __STR_EXTRA__

#include <stdlib.h>

#ifdef _POSIX_C_SOURCE
#include <stdbool.h>
#endif

#ifdef __WIN32__
#include <stdbool.h>
#endif

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#endif

/* returns a string value, Java Bool.toString() */
const char *str_bool(bool b);

/* similar to tolower(), but works on a whole string. inline */
char *str_to_lower(char *s);

/* java - string.startsWith() */
bool str_is_prefix(const char *str, const char *pre);

bool str_ends_with(const char *str, char c);

/* fnsplit, more or less, returing only the filename+ext, without path */
const char *file_base_name(const char *file_name);

/* returns the last tip of the file name */
const char *file_get_extesnsion(const char *fname);

#if defined(__WIN32__)
/* This function is available on Linux, but now on Windows */
char *strsep(char **stringp, const char *delim);
#endif

#endif /* __STR_EXTRA__ */
