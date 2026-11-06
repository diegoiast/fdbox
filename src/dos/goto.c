#include <stdio.h>
#include <stdlib.h>

#include "dos/goto.h"
#include "dos/batch.h"
#include "fdbox.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_goto(int argc, char *argv[]) {
        if (argc < 2) {
                fprintf(stderr, "GOTO needs a label\n");
                return EXIT_FAILURE;
        }
        return batch_goto(argv[1]);
}

const char *help_goto() { return "Here should be a basic help for goto"; }
