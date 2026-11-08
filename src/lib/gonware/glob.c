/*
This file is part of fdbox
For license - read license.txt

glob() with DOS semantics for GonWare, see glob.h.
*/

#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

#include "glob.h"

static int match(const char *p, const char *n) {
        for (; *p; p++, n++) {
                if (*p == '*') {
                        while (p[1] == '*')
                                p++;
                        if (!p[1])
                                return 1;
                        for (; *n; n++)
                                if (match(p + 1, n))
                                        return 1;
                        return match(p + 1, n);
                }
                if (!*n)
                        return 0;
                if (*p != '?' && tolower((unsigned char)*p) != tolower((unsigned char)*n))
                        return 0;
        }
        return !*n;
}

int gdos_wildcard_match(const char *pattern, const char *name) {
        size_t l = strlen(pattern);
        if (match(pattern, name))
                return 1;
        /* "NAME.*" and "*.*" also stand for names without an extension */
        if (l >= 2 && !strcmp(pattern + l - 2, ".*") && !strchr(name, '.')) {
                char base[260];
                if (l - 2 >= sizeof(base))
                        return 0;
                memcpy(base, pattern, l - 2);
                base[l - 2] = 0;
                return match(base, name);
        }
        return 0;
}

static int add(glob_t *g, size_t *cap, const char *dir, size_t dirlen, const char *name) {
        size_t n = strlen(name);
        char *s = malloc(dirlen + n + 1);
        if (!s)
                return GLOB_NOSPACE;
        memcpy(s, dir, dirlen);
        memcpy(s + dirlen, name, n + 1);
        if (g->gl_offs + g->gl_pathc + 2 > *cap) {
                size_t c = *cap ? *cap * 2 : 16;
                char **v = realloc(g->gl_pathv, c * sizeof(char *));
                if (!v) {
                        free(s);
                        return GLOB_NOSPACE;
                }
                g->gl_pathv = v;
                *cap = c;
        }
        g->gl_pathv[g->gl_offs + g->gl_pathc++] = s;
        g->gl_pathv[g->gl_offs + g->gl_pathc] = NULL;
        return 0;
}

static int compare(const void *a, const void *b) {
        return strcasecmp(*(char *const *)a, *(char *const *)b);
}

int glob(const char *pattern, int flags, int (*errfunc)(const char *epath, int eerrno),
         glob_t *g) {
        size_t cap = 0, dirlen = 0, first;
        const char *name = pattern;
        char dir[260];
        struct stat st;
        DIR *d;
        int r = 0;

        (void)errfunc;
        if (!(flags & GLOB_APPEND)) {
                if (!(flags & GLOB_DOOFFS))
                        g->gl_offs = 0;
                g->gl_pathc = 0;
                g->gl_pathv = calloc(g->gl_offs + 1, sizeof(char *));
                if (!g->gl_pathv)
                        return GLOB_NOSPACE;
                cap = g->gl_offs + 1;
        } else {
                cap = g->gl_offs + g->gl_pathc + 1;
        }
        first = g->gl_pathc;

        for (const char *p = pattern; *p; p++)
                if (*p == '\\' || *p == '/' || *p == ':')
                        name = p + 1;
        dirlen = (size_t)(name - pattern);
        if (dirlen >= sizeof(dir))
                return GLOB_NOSPACE;
        memcpy(dir, pattern, dirlen);
        dir[dirlen] = 0;

        if (!strpbrk(name, "*?")) {
                /* no wildcard: the name itself, if it is there */
                if (!stat(pattern, &st) || (flags & GLOB_NOCHECK))
                        return add(g, &cap, "", 0, pattern);
                return GLOB_NOMATCH;
        }

        d = opendir(dirlen ? dir : ".");
        if (d) {
                struct dirent *e;
                while (!r && (e = readdir(d)))
                        if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..") &&
                            gdos_wildcard_match(name, e->d_name))
                                r = add(g, &cap, dir, dirlen, e->d_name);
                closedir(d);
        } else if (flags & GLOB_ERR) {
                return GLOB_ABORTED;
        }
        if (r)
                return r;
        if (g->gl_pathc == first) {
                if (flags & GLOB_NOCHECK)
                        return add(g, &cap, "", 0, pattern);
                return GLOB_NOMATCH;
        }
        if (!(flags & GLOB_NOSORT))
                qsort(g->gl_pathv + g->gl_offs + first, g->gl_pathc - first, sizeof(char *),
                      compare);
        return 0;
}

void globfree(glob_t *g) {
        if (!g->gl_pathv)
                return;
        for (size_t i = 0; i < g->gl_pathc; i++)
                free(g->gl_pathv[g->gl_offs + i]);
        free(g->gl_pathv);
        g->gl_pathv = NULL;
        g->gl_pathc = 0;
}
