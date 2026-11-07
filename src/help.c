#include <stdio.h>
#include <stdlib.h>

#include "fdbox.h"
#include "help.h"
#include "lib/applet.h"
#include "lib/nls.h"

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
        printf(_("FDBox version %s (%d)\n"), FDBOX_VERSION_STR, FDBOX_VERSION);
        fputs(_("Available applets:\n"), stdout);

        while (app != NULL && app->name != NULL) {
                printf("    %-15s%s\n", app->name, app->help_text());
                app++;
        }

        UNUSED(argv);
        UNUSED(argc);
        return EXIT_SUCCESS;
}

const char *help_help() { return _("Displays about text (this help)"); }
