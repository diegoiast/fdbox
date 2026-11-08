#include <stdio.h>
#include <stdlib.h>

#include "fdbox.h"
#include "lib/args.h"
#include "lib/nls.h"
#include "unix/hexdump.h"

/*
This file is part of fdbox
For license - read license.txt
*/

/* A canonical hex dump of an open stream: an 8-digit offset, 16 bytes in hex
 * split into two groups of eight, then the printable ASCII between bars. */
static void hexdump_stream(FILE *file) {
        unsigned char buf[16];
        unsigned long offset = 0;
        size_t got;

        while ((got = fread(buf, 1, sizeof(buf), file)) > 0) {
                size_t i;
                printf("%08lx  ", offset);
                for (i = 0; i < 16; i++) {
                        if (i < got) {
                                printf("%02x ", buf[i]);
                        } else {
                                printf("   ");
                        }
                        if (i == 7) {
                                printf(" ");
                        }
                }
                printf(" |");
                for (i = 0; i < got; i++) {
                        int c = buf[i];
                        putchar((c >= 0x20 && c < 0x7f) ? c : '.');
                }
                printf("|\n");
                offset += got;
        }
        printf("%08lx\n", offset);
}

int command_hexdump(int argc, char *argv[]) {
        const char *files[ARG_MAX_FILES];
        int nfiles = 0;
        int i, rc = EXIT_SUCCESS;

        for (i = 1; i < argc; i++) {
                if (argv[i][0] != '-' && nfiles < ARG_MAX_FILES) {
                        files[nfiles++] = argv[i];
                }
        }
        if (nfiles == 0) {
                hexdump_stream(stdin);
                return EXIT_SUCCESS;
        }
        for (i = 0; i < nfiles; i++) {
                FILE *file = fopen(files[i], "rb");
                if (file == NULL) {
                        fprintf(stderr, _("hexdump: cannot open %s\n"), files[i]);
                        rc = EXIT_FAILURE;
                        continue;
                }
                hexdump_stream(file);
                fclose(file);
        }
        return rc;
}

const char *help_hexdump() { return _("Dump the contents of a file as hex and ASCII"); }
