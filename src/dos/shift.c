#include <stdio.h>
#include <stdlib.h>

#include "dos/shift.h"
#include "dos/batch.h"
#include "fdbox.h"
#include "lib/nls.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_shift(int argc, char *argv[]) {
        return batch_shift();
}

const char *help_shift() { return _("Here should be a basic help for shift"); }
