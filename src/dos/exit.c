#include <stdio.h>
#include <stdlib.h>

#include "dos/exit.h"
#include "fdbox.h"
#include "lib/nls.h"

/*
This file is part of fdbox
For license - read license.txt
*/

/* The shell's main loop intercepts "exit" to break out of it (see command.c).
 * This handler covers the applet form ("fdbox exit [code]") and, as in DOS,
 * returns an optional exit status. */
int command_exit(int argc, char *argv[]) {
        if (argc > 1) {
                return atoi(argv[1]);
        }
        return EXIT_SUCCESS;
}

const char *help_exit() { return _("Leave the shell, with an optional status code"); }
