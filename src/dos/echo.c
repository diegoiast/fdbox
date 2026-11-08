#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/echo.h"
#include "fdbox.h"
#include "lib/args.h"
#include "lib/environ.h"
#include "lib/nls.h"

#if defined(__TURBOC__)
#include "lib/tc202/stdextra.h"
#endif

#if defined(__WATCOMC__) || defined(__GONWARE__) || defined(_POSIX_C_SOURCE) || defined(__APPLE__)
#include <strings.h>
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
                printf(_("Echo is %s\n"), echo_is_on ? "ON" : "OFF");
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

        if (!command_merge_args(argc - 1, (const char **)(argv + 1), line, sizeof(line))) {
                fputs(_("Warning - could not expand line\n"), stderr);
        }

        expand_string(line, expanded, sizeof(expanded));
        puts(expanded);
        return EXIT_SUCCESS;
}

int echo_line(const char *text) {
        char expanded[1024 * 4];
        const char *end = text + strlen(text);

        while (end > text && (end[-1] == ' ' || end[-1] == '\t'))
                end--;
        if (end == text) {
                printf(_("Echo is %s\n"), echo_is_on ? "ON" : "OFF");
                return EXIT_SUCCESS;
        }
        if (end - text == 2 && strncasecmp(text, "on", 2) == 0) {
                echo_is_on = true;
                return EXIT_SUCCESS;
        }
        if (end - text == 3 && strncasecmp(text, "off", 3) == 0) {
                echo_is_on = false;
                return EXIT_SUCCESS;
        }
        expand_string(text, expanded, sizeof(expanded));
        puts(expanded);
        return EXIT_SUCCESS;
}

const char *help_echo() { return _("Prints a string"); }
