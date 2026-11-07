/*
This file is part of fdbox
For license - read license.txt

LOAD module [arguments]: loads a GonWare kernel module (GLM) as on
NetWare; a name without a path also comes from the boot archive as long
as it is there, or from \GONWARE. A GLM with a glm_main runs in the
foreground until it returns (LOAD SERVER is the same as SERVER). A GLM
starts by its name alone too; drivers (.HBA, .DEV) only with LOAD, the
check that one means it.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/load.h"
#include "fdbox.h"

#if defined(__GONWARE__)
#include <gonware/syscall.h>
#endif

int command_load(int argc, char *argv[]) {
#if defined(__GONWARE__)
        static char args[256];
        long r;
        int i;

        if (argc < 2) {
                printf("LOAD module [arguments]\n");
                return EXIT_FAILURE;
        }
        args[0] = 0;
        for (i = 2; i < argc && strlen(args) + strlen(argv[i]) + 2 <= sizeof(args); i++) {
                if (i > 2)
                        strcat(args, " ");
                strcat(args, argv[i]);
        }
        r = gdos_call2(GDOS_LOAD, argv[1], args);
        if (r == -GDOS_ENOENT) {
                fprintf(stderr, "LOAD: %s not found\n", argv[1]);
                return EXIT_FAILURE;
        }
        if (r < 0) {
                fprintf(stderr, "LOAD: %s was not loaded\n", argv[1]);
                return EXIT_FAILURE;
        }
        return (int)r;
#else
        (void)argc;
        (void)argv;
        fprintf(stderr, "LOAD: modules exist only on GonWare\n");
        return EXIT_FAILURE;
#endif
}

const char *help_load() { return "Loads a kernel module (GLM)"; }
