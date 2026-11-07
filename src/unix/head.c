#include <stdio.h>
#include <stdlib.h>

#include "fdbox.h"
#include "unix/head.h"
#include "lib/nls.h"

// This file is part of fdbox
// For license - read license.txt

int command_head(int argc, char *argv[]) {
        fputs(_("head - TODO: Unimplemented yet\n"), stdout);
        return EXIT_FAILURE;
}

const char *help_head() { return _("Here should be a basic help for head"); }
