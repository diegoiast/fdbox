#include <stdio.h>
#include <stdlib.h>

#include "fdbox.h"
#include "unix/tail.h"
#include "lib/nls.h"

// This file is part of fdbox
// For license - read license.txt

int command_tail(int argc, char *argv[]) {
        fputs(_("tail - TODO: Unimplemented yet\n"), stdout);
        return EXIT_FAILURE;
}

const char *help_tail() { return _("Here should be a basic help for tail"); }
