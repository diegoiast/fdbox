#include <stdio.h>
#include <stdlib.h>

#include "dos/ver.h"
#include "fdbox.h"

/*
This file is part of fdbox
For license - read license.txt
*/

/* https://sourceforge.net/p/predef/wiki/Compilers/ */
#if defined(__GNUC__)
#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)
#       define COMPILER_NAME "gcc"
#       define COMPILER_VERSION GCC_VERSION
#elif defined(__TURBOC__)
#       define COMPILER_NAME "TurboC"
#       define COMPILER_VERSION __TURBOC__
#elif defined(__WATCOMC__)
#       define COMPILER_NAME "OpenWatcom"
#       define COMPILER_VERSION __WATCOMC__
#else
#       define COMPILER_NAME "???"
#       define COMPILER_VERSION 0
#endif

int command_ver(int argc, char *argv[]) {
        UNUSED(argc);
        UNUSED(argv);
        printf("%s %s (%s/%d)\n", "fdbox", FDBOX_VERSION_STR, COMPILER_NAME, COMPILER_VERSION);
        return EXIT_SUCCESS;
}

const char *help_ver() { return "Display the version of the shell"; }
