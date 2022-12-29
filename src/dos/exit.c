#include <stdio.h>
#include <stdlib.h>

#include "dos/exit.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_exit(int argc, char *argv[]) {
        printf("exit - TODO: Unimplemented yet\n");
        return EXIT_FAILURE;
}

const char *help_exit() { return "Exits the interactive shell"; }
