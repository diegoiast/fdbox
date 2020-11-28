#include <stdlib.h>
#include <stdio.h>

#include "lib/applet.h"
#include "fdbox.h"
#include "help.h"

/*
 * This file is part of fdbox
 * For license - read license.txt
 */

extern struct applet commands[];

int command_help(int arc, char* argv[]) {
struct applet *app = commands;
    printf("FDBox version %s (%d)\n", FDBOX_VERSION_STR, FDBOX_VERSION);
    printf("Available applets:\n");

    while (app != NULL && app->name != NULL) {
        printf("    %-20s%s\n", app->name, app->help_text() );
        app++;
    }
    return EXIT_FAILURE;
}

const char* help_help() {
    return "Displays about text (this help)";
}
