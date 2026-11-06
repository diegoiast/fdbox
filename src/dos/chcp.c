#include <stdio.h>
#include <stdlib.h>

#include "dos/chcp.h"
#include "fdbox.h"

#if defined(__GONWARE__)
#include <gonware/syscall.h>
#endif

/*
This file is part of fdbox
For license - read license.txt

CHCP [number]: shows or changes the code page. On GonWare it is the OEM
code page of the 8.3 names on FAT volumes; GDOS loads UNI_<number>.TAB
from \GONWARE\NLS of the system volume (437 and 850 also from its
internal ramdisk).
*/

int command_chcp(int argc, char *argv[]) {
#if defined(__GONWARE__)
        long r;
        char *end;
        unsigned long n = 0;

        if (argc > 2) {
                fprintf(stderr, "CHCP [number]\n");
                return EXIT_FAILURE;
        }
        if (argc == 2) {
                n = strtoul(argv[1], &end, 10);
                if (*end || !n || n > 65535) {
                        fprintf(stderr, "Invalid code page - %s\n", argv[1]);
                        return EXIT_FAILURE;
                }
        }
        r = gdos_call2(GDOS_CODEPAGE, (long)n, 0);
        if (r == -GDOS_ENOENT) {
                fprintf(stderr, "Invalid code page - %s (no UNI_%03lu.TAB)\n", argv[1], n);
                return EXIT_FAILURE;
        }
        if (r < 0) {
                fprintf(stderr, "Invalid code page - %s (no single-byte DOS code page)\n", argv[1]);
                return EXIT_FAILURE;
        }
        printf("Active code page: %ld\n", r);
        return EXIT_SUCCESS;
#else
        (void)argc;
        (void)argv;
        fprintf(stderr, "CHCP: only on GonWare\n");
        return EXIT_FAILURE;
#endif
}

const char *help_chcp() { return "Shows or changes the code page"; }
