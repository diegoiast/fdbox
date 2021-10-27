#include <stdio.h>
#include <stdlib.h>

#include "fdbox.h"
#include "help.h"
#include "lib/applet.h"

#if defined(__TURBOC__)
#include "lib/tc202/stdextra.h"
#endif

/*
 * This file is part of fdbox
 * For license - read license.txt
 */

extern struct applet commands[];

int command_help(int argc, char *argv[]) {
        struct applet *app = commands;
        printf("FDBox version %s (%d)\n", FDBOX_VERSION_STR, FDBOX_VERSION);
        printf("Available applets:\n");

        while (app != NULL && app->name != NULL) {
                printf("    %-15s%s\n", app->name, app->help_text());
                app++;
        }

        UNUSED(argv);
        UNUSED(argc);
        return EXIT_SUCCESS;
}

const char *help_help() { return "Displays about text (this help)"; }
