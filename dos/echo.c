#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/echo.h"
#include "fdbox.h"
#include "lib/args.h"
#include "lib/environ.h"

#if defined(__TURBOC__)
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#endif

/*
This file is part of fdbox
For license - read license.txt
*/

bool echo_is_on;

int command_echo(int argc, char *argv[]) {
        char line[1024 * 4];
        char expanded[1024 * 4];

        if (argc == 1) {
                printf("Echo is %s\n", echo_is_on ? "ON" : "OFF");
                return EXIT_SUCCESS;
        }

        if (argc == 2) {
                if (strcasecmp(argv[1], "on") == 0) {
                        echo_is_on = true;
                        return EXIT_SUCCESS;
                }
                if (strcasecmp(argv[1], "off") == 0) {
                        echo_is_on = false;
                        return EXIT_SUCCESS;
                }
        }

        if (!command_merge_args(argc - 1, argv + 1, line, sizeof(line))) {
                fprintf(stderr, "Warning - could not expand line\n");
        }

        expand_string(line, expanded, sizeof(expanded));
        puts(expanded);
        return EXIT_SUCCESS;
}

const char *help_echo() { return "Prints a string"; }
