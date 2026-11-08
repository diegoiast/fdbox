#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/path.h"
#include "fdbox.h"
#include "lib/args.h"
#include "lib/nls.h"

#if defined(__MSDOS__) || defined(__WIN32__)
/* setenv/unsetenv are not in Turbo C; the shim provides them over putenv() */
#include "lib/tc202/stdextra.h"
#endif

/*
This file is part of fdbox
For license - read license.txt
*/

/* The DOS PATH command: no argument prints the search path, "PATH ;" clears
 * it, otherwise the arguments become the new path. It is the PATH environment
 * variable, so SET and PATH see the same value. */
int command_path(int argc, char *argv[]) {
        const char *current;
        char line[1024];

        if (argc == 1) {
                current = getenv("PATH");
                printf("PATH=%s\n", current != NULL ? current : "");
                return EXIT_SUCCESS;
        }
        if (argc == 2 && strcmp(argv[1], ";") == 0) {
                return unsetenv("PATH") == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        if (!command_merge_args(argc - 1, (const char **)(argv + 1), line, sizeof(line))) {
                return EXIT_FAILURE;
        }
        return setenv("PATH", line, 1) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

const char *help_path() { return _("Display or set the command search path (PATH)"); }
