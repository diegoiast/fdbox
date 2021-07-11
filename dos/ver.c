#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/ver.h"
#include "fdbox.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_ver(int argc, char *argv[]) {
        UNUSED(argc);
        UNUSED(argv);
        printf("%s %s\n",  "fdbox", FDBOX_VERSION_STR);
        return EXIT_SUCCESS;
}

const char *help_ver() {
        return "Display the vresion of the shell";
}
