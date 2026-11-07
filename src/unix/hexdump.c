#include <stdio.h>
#include <stdlib.h>

#include "fdbox.h"
#include "hexdump.h"
#include "lib/nls.h"

// This file is part of fdbox
// For license - read license.txt

int command_hexdump(int argc, char *argv[]) {
        fputs(_("hexdump - TODO: Unimplemented yet\n"), stdout);
        return EXIT_FAILURE;
}

const char *help_hexdump() { return _("Here should be a basic help for hexdump"); }
