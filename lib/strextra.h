/*
This file is part of fdbox
For license - read license.txt
*/

#ifndef __STR_EXTRA__
#define __STR_EXTRA__

#include <stdlib.h>

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__WATCOMC__)
#include <stdbool.h>
#endif

#ifdef __WIN32__
#include <stdbool.h>
#endif

#if defined(__TURBOC__)
#include "lib/tc202/stdbool.h"
#endif

/* returns a string value, Java - Bool.toString() */
const char *str_bool(bool b);

/* similar to tolower(), but works on a whole string. modifies input */
char *str_to_lower(char *s);

/* deletes a single char within a string, pushing everything after leftwise */
char *str_del_char(char *s, size_t index);

/* inserts a single char within a string, pushing everything after rightwise*/
char *str_ins_char(char *s, size_t max_length, char c, size_t index);

/* java - string.startsWith() */
bool str_is_prefix(const char *str, const char *pre);

/* java - string.endsWith() */
bool str_ends_with(const char *str, char c);

/* fnsplit, more or less, returing only the filename+ext, without path */
const char *file_base_name(const char *file_name);

/* returns the last tip of the file name */
const char *file_get_extesnsion(const char *fname);

#if defined(__WIN32__)
/* This function is available on Linux, but now on Windows */
char *strsep(char **stringp, const char *delim);
#endif

struct str_list {
        size_t length;
        size_t next;
        char **items;
};

void str_list_init(struct str_list *list, size_t length);
void str_list_free(struct str_list *list);
void str_list_push(struct str_list *list, const char *s);
char *str_list_pop(struct str_list *list);
const char *str_list_peek(struct str_list *list);
const char *str_list_get(struct str_list *list, size_t n);

#endif /* __STR_EXTRA__ */
