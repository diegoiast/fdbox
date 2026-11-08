#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdbox.h"
#include "lib/nls.h"
#include "unix/basename.h"

/*
This file is part of fdbox
For license - read license.txt
*/

static int is_sep(char c) { return c == '/' || c == '\\'; }

/* basename path [suffix] - the file name part of a path, both / and \ count
 * as separators; an optional suffix is removed when it matches. */
int command_basename(int argc, char *argv[]) {
        char path[260];
        char *base, *p;
        size_t n;

        if (argc < 2) {
                fprintf(stderr, _("basename: a path is needed\n"));
                return EXIT_FAILURE;
        }
        strncpy(path, argv[1], sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';

        n = strlen(path);
        while (n > 1 && is_sep(path[n - 1])) {
                path[--n] = '\0';
        }
        base = path;
        for (p = path; *p; p++) {
                if (is_sep(*p)) {
                        base = p + 1;
                }
        }
        if (argc >= 3) {
                size_t bl = strlen(base), sl = strlen(argv[2]);
                if (sl > 0 && sl < bl && strcmp(base + bl - sl, argv[2]) == 0) {
                        base[bl - sl] = '\0';
                }
        }
        printf("%s\n", base);
        return EXIT_SUCCESS;
}

const char *help_basename() {
        return _("Strip the directory (and an optional suffix) from a path");
}
