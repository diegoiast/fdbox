#ifndef GLOB_H
#define GLOB_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PATH 254
#define ENAMETOOLONG -1

/* Bits set in the FLAGS argument to `glob' */

/* Posix */
#define GLOB_ERR (1 << 0)
#define GLOB_MARK (1 << 1)
#define GLOB_NOSORT (1 << 2)
#define GLOB_DOOFFS (1 << 3)
#define GLOB_NOCHECK (1 << 4)
#define GLOB_APPEND (1 << 5)
#define GLOB_NOESCAPE (1 << 6)
#define GLOB_PERIOD (1 << 7)

/* Posix2, GNU, BSD */
#define GLOB_MAGCHAR (1 << 8)
#define GLOB_ALTDIRFUNC (1 << 9)
#define GLOB_BRACE (1 << 10)
#define GLOB_NOMAGIC (1 << 11)
#define GLOB_TILDE (1 << 12)
#define GLOB_ONLYDIR (1 << 13)
#define GLOB_TILDE_CHECK (1 << 14)

/* Error returns from `glob' */
#define GLOB_NOSPACE 1
#define GLOB_ABORTED 2
#define GLOB_NOMATCH 3
#define GLOB_NOSYS 4

typedef struct glob_t {
        unsigned gl_pathc;
        char **gl_pathv;
        int gl_offs;
} glob_t;

int glob(const char *pattern, int flags, int (*errfunc)(const char *epath, int eerrno),
         glob_t *pglob);
void globfree(glob_t *pglob);

/* these should not be here technically, they should be  in STAT.H */
#define __S_ISTYPE(mode, mask) (((mode)&S_IFMT) == (mask))
#define S_ISDIR(mode) __S_ISTYPE((mode), S_IFDIR)

#ifdef __cplusplus
}
#endif

#endif /* ifndef GLOB_H */
