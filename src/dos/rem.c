#include <stdio.h>
#include <stdlib.h>

#include "dos/rem.h"
#include "fdbox.h"
#include "lib/nls.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_rem(int argc, char *argv[]) {
        UNUSED(argc);
        UNUSED(argv);
        return EXIT_SUCCESS;
}

const char *help_rem() { return _("Used in batch files for comments"); }
