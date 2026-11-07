#include <stdio.h>
#include <stdlib.h>

#include "dos/exit.h"
#include "fdbox.h"
#include "lib/nls.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_exit(int argc, char *argv[]) {
        fputs(_("exit - TODO: Unimplemented yet\n"), stdout);
        return EXIT_FAILURE;
}

const char *help_exit() { return _("Exits the interactive shell"); }
