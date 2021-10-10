#include <stdio.h>
#include <stdlib.h>

#include "dos/cls.h"
#include "lib/readline.h"
#include "fdbox.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_cls(int argc, char *argv[]) {
        clear_screen();
        UNUSED(argc);
        UNUSED(argv);
        return EXIT_SUCCESS;
}

const char *help_cls() { return "Clear screen"; }
