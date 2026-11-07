#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dos/set.h"
#include "lib/args.h"
#include "fdbox.h"
#include "lib/tc202/stdextra.h"
#include "lib/nls.h"

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
        const char *name = argv[1];
        const char *val = argv[3];

        if (argc == 1) {
                for (; *s; s++) {
                        printf("%s\n", *s);
                }
                return EXIT_SUCCESS;
        }

        {
                /* "SET NAME=value" as in DOS (also "SET NAME = value"): the
                 * name in capitals, an empty value removes the variable */
                char line[1024], *eq, *v;
                size_t i;
                if (!command_merge_args(argc - 1, (const char **)(argv + 1), line, sizeof(line)))
                        return EXIT_FAILURE;
                eq = strchr(line, '=');
                if (!eq) {
                        size_t n = strlen(line);
                        while (n && line[n - 1] == ' ')
                                line[--n] = 0;
                        for (; *s; s++)
                                if (strncasecmp(*s, line, n) == 0)
                                        printf("%s\n", *s);
                        return EXIT_SUCCESS;
                }
                v = eq + 1;
                while (eq > line && eq[-1] == ' ')
                        eq--;
                *eq = 0;
                while (*v == ' ')
                        v++;
                for (i = 0; line[i]; i++)
                        line[i] = (char)toupper((unsigned char)line[i]);
                (void)name;
                (void)val;
                if (!*v)
                        r = unsetenv(line);
                else
                        r = setenv(line, v, 1);
                return r ? EXIT_FAILURE : EXIT_SUCCESS;
        }
}

const char *help_set() { return _("Display or modify environment variable"); }
