#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/tc202/stdextra.h"
#include "dos/set.h"
#include "fdbox.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#if defined(__MSDOS__) || defined(__WIN32__)
/* setenv is not supported under TC, implement this locally
 * using `putenv()`
 */
#include "lib/tc202/stdextra.h"
#endif

int command_set(int argc, char *argv[]) {
        extern char **environ;
        char **s = environ;
        char *c;
        int r;

        if (argc == 1) {
                for (; *s; s++) {
                        printf("%s\n", *s);
                }
                return EXIT_SUCCESS;
        }

        c = strchr(argv[1], '=');
        if (c == 0) {
                printf("%s\n", getenv(argv[1]));
        } else {
                const char *name = argv[1];
                const char *val = c + 1;
                *c = 0;
                r = setenv(name, val, 1);
                return r;
        }
        return EXIT_SUCCESS;
}

const char *help_set() { return "Here should be a basic help for set"; }
