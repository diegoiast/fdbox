#include <stdio.h>
#include <stdlib.h>

#include "dos/keyb.h"
#include "fdbox.h"

#if defined(__GONWARE__)
#include <gonware/syscall.h>
#endif

/*
This file is part of fdbox
For license - read license.txt

KEYB [code]: shows or changes the keyboard layout by its DOS keyboard
code. GDOS starts with US (as DOS does) and has GR (German; DE is taken
as well); the installer writes the KEYB line into AUTOEXEC.BTM.
*/

int command_keyb(int argc, char *argv[]) {
#if defined(__GONWARE__)
        char cur[8];

        if (argc > 2) {
                fprintf(stderr, "KEYB [code]\n");
                return EXIT_FAILURE;
        }
        if (gdos_call2(GDOS_KEYB, argc == 2 ? (long)argv[1] : 0, (long)cur) < 0) {
                fprintf(stderr, "Invalid keyboard code specified - %s\n", argv[1]);
                return EXIT_FAILURE;
        }
        printf("Current keyboard code: %s\n", cur);
        return EXIT_SUCCESS;
#else
        (void)argc;
        (void)argv;
        fprintf(stderr, "KEYB: only on GonWare\n");
        return EXIT_FAILURE;
#endif
}

const char *help_keyb() { return "Shows or changes the keyboard layout"; }
