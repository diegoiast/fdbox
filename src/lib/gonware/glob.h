/*
This file is part of fdbox
For license - read license.txt

glob() with DOS semantics for GonWare (GDOS): names match without regard
to case, "*.*" also matches names without an extension, "\" separates
directories (it is no escape character). Replaces <glob.h> of the C
library through the include path.
*/

#ifndef FDBOX_GONWARE_GLOB_H
#define FDBOX_GONWARE_GLOB_H

#include <stddef.h>

#define GLOB_ERR (1 << 0)
#define GLOB_MARK (1 << 1)
#define GLOB_NOSORT (1 << 2)
#define GLOB_DOOFFS (1 << 3)
#define GLOB_NOCHECK (1 << 4)
#define GLOB_APPEND (1 << 5)
#define GLOB_NOESCAPE (1 << 6)
#define GLOB_PERIOD (1 << 7)

#define GLOB_NOSPACE 1
#define GLOB_ABORTED 2
#define GLOB_NOMATCH 3

#define glob_t gdos_glob_t
#define glob gdos_glob
#define globfree gdos_globfree

typedef struct {
        size_t gl_pathc;
        char **gl_pathv;
        size_t gl_offs;
} glob_t;

int glob(const char *pattern, int flags, int (*errfunc)(const char *epath, int eerrno),
         glob_t *pglob);
void globfree(glob_t *pglob);

/* DOS wildcard match ('*', '?'), without regard to case */
int gdos_wildcard_match(const char *pattern, const char *name);

#endif
