#include <stdio.h>
#include <stdlib.h>

#ifdef __MSDOS__
#include <conio.h>
#endif

#include "dos/cls.h"
#include "fdbox.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_cls(int argc, char *argv[]) {
#ifdef __linux__
        printf("\e[1;1H\e[2J");
#endif

#ifdef WIN32
        printf("\e[1;1H\e[2J");
#endif

#ifdef __MSDOS__
        clrscr();
#endif
        UNUSED(argc);
        UNUSED(argv);
        return EXIT_FAILURE;
}

const char *help_cls() { return "Clear screen"; }
