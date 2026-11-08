#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdbox.h"
#include "lib/nls.h"
#include "unix/dirname.h"

/*
This file is part of fdbox
For license - read license.txt
*/

static int is_sep(char c) { return c == '/' || c == '\\'; }

/* dirname path - everything up to the last separator, "." when there is
 * none; both / and \ count as separators. */
int command_dirname(int argc, char *argv[]) {
        char path[260];
        char *last, *p;
        size_t n;

        if (argc < 2) {
                fprintf(stderr, _("dirname: a path is needed\n"));
                return EXIT_FAILURE;
        }
        strncpy(path, argv[1], sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';

        n = strlen(path);
        while (n > 1 && is_sep(path[n - 1])) {
                path[--n] = '\0';
        }
        last = NULL;
        for (p = path; *p; p++) {
                if (is_sep(*p)) {
                        last = p;
                }
        }
        if (last == NULL) {
                strcpy(path, ".");
        } else {
                while (last > path && is_sep(last[-1])) {
                        last--;
                }
                if (last == path) {
                        path[1] = '\0'; /* a separator at the root stays */
                } else {
                        *last = '\0';
                }
        }
        printf("%s\n", path);
        return EXIT_SUCCESS;
}

const char *help_dirname() { return _("Strip the last component from a path"); }
