#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdbox.h"
#include "lib/args.h"
#include "lib/nls.h"
#include "unix/sort.h"

/*
This file is part of fdbox
For license - read license.txt
*/

static int cmp_forward(const void *a, const void *b) {
        return strcmp(*(const char *const *)a, *(const char *const *)b);
}

static int cmp_reverse(const void *a, const void *b) {
        return strcmp(*(const char *const *)b, *(const char *const *)a);
}

/* Append the lines of a stream to a growable array of copies. Returns 0 on
 * success, -1 when out of memory. A line longer than the buffer is split. */
static int read_lines(FILE *file, char ***lines, size_t *count, size_t *cap) {
        char buf[4096];

        while (fgets(buf, sizeof(buf), file) != NULL) {
                char *copy;
                if (*count == *cap) {
                        size_t next = *cap ? *cap * 2 : 64;
                        char **grown = realloc(*lines, next * sizeof(char *));
                        if (grown == NULL) {
                                return -1;
                        }
                        *lines = grown;
                        *cap = next;
                }
                copy = malloc(strlen(buf) + 1);
                if (copy == NULL) {
                        return -1;
                }
                strcpy(copy, buf);
                (*lines)[(*count)++] = copy;
        }
        return 0;
}

/* sort [-r] [file...] - sort lines, from all files together, -r reverses. */
int command_sort(int argc, char *argv[]) {
        int reverse = 0;
        const char *files[ARG_MAX_FILES];
        int nfiles = 0;
        int i, rc = EXIT_SUCCESS;
        char **lines = NULL;
        size_t count = 0, cap = 0, j;

        for (i = 1; i < argc; i++) {
                char *arg = argv[i];
                if (arg[0] == '-' && arg[1] != '\0') {
                        if (strchr(arg, 'r') != NULL) {
                                reverse = 1;
                        }
                        continue;
                }
                if (nfiles < ARG_MAX_FILES) {
                        files[nfiles++] = arg;
                }
        }

        if (nfiles == 0) {
                if (read_lines(stdin, &lines, &count, &cap) != 0) {
                        rc = EXIT_FAILURE;
                }
        } else {
                for (i = 0; i < nfiles; i++) {
                        FILE *file = fopen(files[i], "r");
                        if (file == NULL) {
                                fprintf(stderr, _("sort: cannot open %s\n"), files[i]);
                                rc = EXIT_FAILURE;
                                continue;
                        }
                        if (read_lines(file, &lines, &count, &cap) != 0) {
                                rc = EXIT_FAILURE;
                        }
                        fclose(file);
                }
        }

        if (count > 0) {
                qsort(lines, count, sizeof(char *), reverse ? cmp_reverse : cmp_forward);
                for (j = 0; j < count; j++) {
                        fputs(lines[j], stdout);
                        /* a split last line may lack its newline */
                        if (strchr(lines[j], '\n') == NULL) {
                                putchar('\n');
                        }
                }
        }
        for (j = 0; j < count; j++) {
                free(lines[j]);
        }
        free(lines);
        return rc;
}

const char *help_sort() { return _("Sort lines of text (-r reverses)"); }
