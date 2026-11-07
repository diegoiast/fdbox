#include <stdio.h>
#include <stdlib.h>

#include "fdbox.h"
#include "unix/cal.h"
#include "lib/nls.h"

// This file is part of fdbox
// For license - read license.txt

int command_cal(int argc, char *argv[]) {
        fputs(_("cal - TODO: Unimplemented yet\n"), stdout);
        return EXIT_FAILURE;
}

const char *help_cal() { return _("Here should be a basic help for cal"); }
