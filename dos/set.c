#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/set.h"
#include "fdbox.h"
#include "lib/tc202/stdextra.h"

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
        int r;

        if (argc == 1) {
                for (; *s; s++) {
                        printf("%s\n", *s);
                }
                return EXIT_SUCCESS;
        }

        if (argc == 2) {
                printf("%s\n", getenv(argv[1]));
                return EXIT_SUCCESS;
        }

        const char *name = argv[1];
        const char *val = argv[3];
        r = setenv(name, val, 1);
        return r;
}

const char *help_set() { return "Display or modify environment variable"; }
