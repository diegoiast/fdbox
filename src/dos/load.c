/*
This file is part of fdbox
For license - read license.txt

LOAD: loads a GonWare kernel module (GLM) as on NetWare; a name without a
path also comes from the boot archive as long as it is there.
*/

#include <stdio.h>
#include <stdlib.h>

#include "dos/load.h"
#include "fdbox.h"

#if defined(__GONWARE__)
#include <gonware/syscall.h>
#endif

int command_load(int argc, char *argv[]) {
        int i, rc = EXIT_SUCCESS;

        if (argc < 2) {
                printf("LOAD module [module ...]\n");
                return EXIT_FAILURE;
        }
#if defined(__GONWARE__)
        for (i = 1; i < argc; i++)
                if (gdos_call1(GDOS_LOAD, argv[i]) < 0) {
                        fprintf(stderr, "LOAD: %s was not loaded\n", argv[i]);
                        rc = EXIT_FAILURE;
                }
#else
        (void)i;
        fprintf(stderr, "LOAD: modules exist only on GonWare\n");
        rc = EXIT_FAILURE;
#endif
        return rc;
}

const char *help_load() { return "Loads a kernel module (GLM)"; }
