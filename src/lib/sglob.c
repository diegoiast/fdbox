/*
* This file is part of fdbox
*
* SerenityGlob is a multi platform port of the posix `glob()` api
* to MSDOS, Windows, SerenityOS and Posix systems.
*
* Copyright 2022 Diego Iastrubni <diegoiast@gmail.com>
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*   2. Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*
*   3. Neither the name of the copyright holder nor the names of its
*      contributors may be used to endorse or promote products derived
*      from this software without specific prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
*   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
* Implementation of a `glob()` for systems without glob.
* First implementation is aimed at @SerenityOS. However,
* porting this to Win32/Win64 or MSDOS should be trivial.
*
* Note that this code is not licensed under GPLv3 as the rest of
* the code fdbox. This code is licensed more permissive on purpose.
*/

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "lib/strextra.h"
#include "sglob.h"

/*
#define GLOB_DEFAULT_FLAGS (FNM_CASEFOLD | FNM_NOESCAPE | FNM_PERIOD)
 */
#define GLOB_DEFAULT_FLAGS (FNM_NOESCAPE | FNM_PERIOD)

struct file_entry {
        char *file_name;
        struct file_entry *next;
};

struct linked_file_list {
        int count;
        struct file_entry* head;
        struct file_entry* tail;
};

/* TODO - what if malloc failed? */
static void list_push_front(struct linked_file_list *list, const char *name)
{
        struct file_entry *next = malloc(sizeof(struct file_entry));
        next->file_name = strdup(name);
        next->next = NULL;
        if (list->head == NULL) {
                list->head = next;
        } else {
                next->next = list->head;
                list->head = next;
        }
        list->count ++;
}

static void list_free(struct linked_file_list *list, int also_content)
{
        struct file_entry *next;
        if (list == NULL) {
                return;
        }
        next = list->head;
        while (next != NULL) {
                struct file_entry *p = next;
                if (also_content) {
                        free(next->file_name);
                }
                next = next->next;
                free(p);
        }
}

static int glob_inside_dir(const char *dir_name, const char *pattern, struct linked_file_list* list);

static int entry_comparer(const void *pv1, const void *pv2) {
        const char *pe1 = pv1;
        const char *pe2 = pv2;
        return strcmp(pe1, pe2);
}

static int convert_list(struct linked_file_list files, glob_t *pglob) {
        struct file_entry *entry;
        int i;

        pglob->gl_pathv = malloc(sizeof(const char *) * files.count);
        if (pglob->gl_pathv == NULL) {
                return GLOB_NOSPACE;
        }
        entry = files.head;
        for (i = 0; i < files.count; i++) {
                pglob->gl_pathv[i] = entry->file_name;
                entry = entry->next;
        }
        pglob->gl_pathc = files.count;
        return 0;
}

int serenity_glob(const char *pattern, int flags, void *unused, glob_t *pglob) {
        char *dir = NULL;
        const char *glob_pattern;
        struct linked_file_list files;
        int rc;

        files.head = NULL;
        files.tail = NULL;
        files.count = 0;

        glob_pattern = file_base_name(pattern);
        dir = file_get_dir(pattern);

        if (!pattern /*|| flags != (flags & GLOB_FLAGS)*/ || unused || !pglob) {
                errno = EINVAL;
                return EINVAL;
        }

        rc = glob_inside_dir(dir, glob_pattern, &files);
        if (rc != 0) {
                list_free(&files, 1);
                free(dir);
                return rc;
        }

        /* warning in TC. pglob is passed by value. WTF? */
        if (convert_list(files, pglob) != 0) {
                free(dir);
                list_free(&files, 1);
                return GLOB_NOSPACE;
        }
        rc = files.count == 0 ? GLOB_NOMATCH : 0;
        list_free(&files, 0);
        free(dir);

        if ((flags & GLOB_NOSORT) == 0) {
                qsort(pglob->gl_pathv, pglob->gl_pathc, sizeof(char *), entry_comparer);
        }

        return rc;
}

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__serenity__)
#include <dirent.h>
#include <fnmatch.h>

/* https://bitbucket.org/szx/glob/src/master/glob_posix.cpp */
int glob_inside_dir(const char *dir_name, const char *pattern, struct linked_file_list* list)
{
        struct dirent* entry;
        DIR *dir;

        if (dir_name == NULL) {
                dir_name = ".";
        }
        if (pattern == NULL) {
                pattern = "*";
        }
        dir = opendir(dir_name);
        if (dir == NULL) {
                return GLOB_ABORTED;
        }

        while ((entry = readdir(dir)) != 0) {
                int r = fnmatch(pattern, entry->d_name, GLOB_DEFAULT_FLAGS);
                if (r) {
                        list_push_front(list, entry->d_name);
#if 0
                        if (S_ISDIR(entry->d_type)) {
                                next = glob_inside_dir(entry->d_name, pattern, NULL, NULL)
                        }
#endif
                } else {
                        closedir(dir);
                        return r;
                }
        }

        return closedir(dir);
}
#elif defined(WIN32)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

int glob_inside_dir(const char *dir_name, const char *pattern, struct linked_file_list* list)
{
        if (dir_name == NULL) {
                dir_name = ".";
        }
        if (pattern == NULL) {
                pattern = "*";
        }

        WIN32_FIND_DATAA finddata;
        HANDLE hFindfile = FindFirstFileA(pattern, &finddata);
        bool failed = true;

        if  (hFindfile != INVALID_HANDLE_VALUE) {
                do {
                        list_push_front(list, finddata.cFileName);
#if 0
                        if (finddata is directory) {
                                next = glob_inside_dir(finddata.cFileName, pattern, NULL, NULL)
                        }
#endif
                } while (FindNextFileA(hFindfile, &finddata));
                failed = !FindClose(hFindfile);
        }

        if (failed) {
                return GLOB_ABORTED;
        }
        return 0;
}
#elif defined(__WATCOMC__) || defined(__TURBOC__)

#include <dos.h>

#if defined(__WATCOMC__)
#       include <stdlib.h>
#       include <stdbool.h>
#       define findfirst _dos_findfirst
#       define findnext _dos_findnext
#       define ffblk find_t
#       define FA_RDONLY _A_RDONLY
#       define FA_HIDDEN _A_HIDDEN
#       define FA_SYSTEM _A_SYSTEM
#       define FA_DIREC _A_SUBDIR
#       define GET_FILE_NAME(ff)(ff.name)
#elif defined (__TURBOC__)
#       include "lib/tc202/stdbool.h"
#       include <dir.h>
#       define GET_FILE_NAME(ff)(ff.ff_name)
#elif
        TODO: unsupported DOS compiler...
#endif

#define ATTRIBUTES FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_DIREC

int glob_inside_dir(const char *dir_name, const char *pattern, struct linked_file_list* list)
{
        int err;
        struct ffblk ff;

        if (dir_name == NULL) {
                dir_name = ".";
        }
        if (pattern == NULL) {
                pattern = "*.*";
        }

#if defined(__WATCOMC__)
        /* stupid watcom c, defines the function differently */
        err = _dos_findfirst(pattern, ATTRIBUTES, &ff);
#else
        err = findfirst(pattern, &ff, ATTRIBUTES);
#endif

        if (err == 0) {
                /* again stupid watcom c, file name member name is different  */
                const char *file_name = GET_FILE_NAME(ff);
                do {
                        list_push_front(list, file_name);
#if 0
                        if (S_ISDIR(entry->d_type)) {
                                next = glob_inside_dir(entry->d_name, pattern, NULL, NULL)
                        }
#endif
                        err = findnext(&ff);
                } while (err == 0);
        }

        /* TODO: how should we find an error? */
        return 0;
}
#endif

void serenity_globfree(glob_t *pglob)
{
        int i;
        if (pglob == NULL) {
                return;
        }
        for (i=0; i< pglob->gl_pathc; i++) {
                free(pglob->gl_pathv[i]);
        }
        free(pglob->gl_pathv);
        pglob->gl_pathv = NULL;
        pglob->gl_pathc = 0;
}
