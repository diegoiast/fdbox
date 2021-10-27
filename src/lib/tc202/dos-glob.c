#include <dos.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/tc202/dos-glob.h"

#if defined(__TURBOC__)
#include <dir.h>

#elif defined(__WATCOMC__)

#include <stdlib.h>

#define findfirst _dos_findfirst
#define findnext _dos_findnext

#define FA_RDONLY _A_RDONLY
#define FA_HIDDEN _A_HIDDEN
#define FA_SYSTEM _A_SYSTEM
#define FA_DIREC _A_SUBDIR
#endif

#define ATTRIBUTES FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_DIREC

struct file_entry {
        char name[MAX_PATH];
        struct file_entry *next;
};

static int insert(const char *path, const char *name, struct file_entry **phead) {
        int len;
        struct file_entry *cur = (struct file_entry *)malloc(sizeof(struct file_entry));
        if (!cur) {
                return ENOMEM;
        }

        /* todo: we need snprintf */
        len = sprintf(cur->name, "%s%s", path, name);
        cur->name[MAX_PATH - 1] = 0;
        cur->next = *phead;
        *phead = cur;

        return len < 0 || len >= MAX_PATH ? ENAMETOOLONG : 0;
}

static int entry_comparer(const void *pv1, const void *pv2) {
        const struct file_entry *const *pe1 = pv1;
        const struct file_entry *const *pe2 = pv2;
        return stricmp((*pe1)->name, (*pe2)->name);
}

int glob(const char *pattern, int flags, int (*errfunc)(const char *epath, int eerrno),
         glob_t *pglob) {
        char path[MAX_PATH];
        struct file_entry *head = NULL;
        int err = 0, err2;
        size_t len;
        unsigned entries = 0;

#if defined(__WATCOMC__)
        struct find_t ff;
#else
        struct ffblk ff;
#endif
        /*
            if (!pattern || flags != (flags & GLOB_FLAGS) || unused || !pglob)
            {
                errno = EINVAL;
                return EINVAL;
            }
        */
        path[MAX_PATH - 1] = 0;
        strncpy(path, pattern, MAX_PATH);
        if (path[MAX_PATH - 1] != 0) {
                errno = ENAMETOOLONG;
                return ENAMETOOLONG;
        }

        len = strlen(path);
        while (len > 0 && path[len - 1] != '/' && path[len - 1] != '\\')
                len--;
        path[len] = 0;

#if defined(__WATCOMC__)
        err = _dos_findfirst(pattern, ATTRIBUTES, &ff);
#else
        err = findfirst(pattern, &ff, ATTRIBUTES);
#endif
        if (err != 0) {
                if (flags & GLOB_NOCHECK) {
                        err = insert("", pattern, &head);
                        entries++;
                }
        } else {
                do {
#if defined(__WATCOMC__)
                        const char *file_name = ff.name;
#else
                        const char *file_name = ff.ff_name;
#endif

                        err2 = insert(path, file_name, &head);
                        entries++;
                        err = findnext(&ff);
                } while (!err && !err2);
                err = 0;
        }

        if (err == 0) {
                pglob->gl_pathv = malloc((entries + 1) * sizeof(char *));
                if (pglob->gl_pathv) {
                        pglob->gl_pathc = entries;
                        pglob->gl_pathv[entries] = NULL;
                        for (; head; head = head->next, entries--)
                                pglob->gl_pathv[entries - 1] = (char *)head;
                        qsort(pglob->gl_pathv, pglob->gl_pathc, sizeof(char *), entry_comparer);
                } else {
                        pglob->gl_pathc = 0;
                        err = ENOMEM;
                }
        } else if (pglob) {
                pglob->gl_pathc = 0;
                pglob->gl_pathv = NULL;
        }

        if (err) {
                struct file_entry *cur;
                while (head) {
                        cur = head;
                        head = head->next;
                        free(cur);
                }
                errno = err;
        }

        return err;
}

void globfree(glob_t *pglob) {
        if (pglob) {
                char **cur;
                for (cur = pglob->gl_pathv; *cur; cur++) {
                        free(*cur);
                }

                pglob->gl_pathc = 0;
                pglob->gl_pathv = NULL;
        }
}
