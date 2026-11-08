#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdbox.h"
#include "lib/args.h"
#include "lib/nls.h"
#include "unix/tail.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#define DEFAULT_LINES 10
#define TAIL_LINE 1024

/* Print the last `count` lines of an open stream. The lines are kept in a
 * ring buffer of `count` slots, so only that many need to be held at once. */
static int tail_stream(FILE *file, long count) {
        char *ring;
        long total = 0, i, first;
        char line[TAIL_LINE];

        if (count <= 0) {
                return EXIT_SUCCESS;
        }
        ring = malloc((size_t)count * TAIL_LINE);
        if (ring == NULL) {
                return EXIT_FAILURE;
        }
        while (fgets(line, TAIL_LINE, file) != NULL) {
                strcpy(ring + (total % count) * TAIL_LINE, line);
                total++;
        }
        first = total > count ? total - count : 0;
        for (i = first; i < total; i++) {
                fputs(ring + (i % count) * TAIL_LINE, stdout);
        }
        free(ring);
        return EXIT_SUCCESS;
}

int command_tail(int argc, char *argv[]) {
        long count = DEFAULT_LINES;
        const char *files[ARG_MAX_FILES];
        int nfiles = 0;
        int i, rc = EXIT_SUCCESS;

        for (i = 1; i < argc; i++) {
                char *arg = argv[i];
                if (arg[0] == '-' && arg[1] != '\0') {
                        if (arg[1] == 'n' && arg[2] == '\0' && i + 1 < argc) {
                                count = atol(argv[++i]);
                        } else if (arg[1] == 'n') {
                                count = atol(arg + 2);
                        } else if (isdigit((unsigned char)arg[1])) {
                                count = atol(arg + 1);
                        }
                        continue;
                }
                if (nfiles < ARG_MAX_FILES) {
                        files[nfiles++] = arg;
                }
        }
        if (count < 0) {
                count = 0;
        }
        if (nfiles == 0) {
                return tail_stream(stdin, count);
        }
        for (i = 0; i < nfiles; i++) {
                FILE *file = fopen(files[i], "r");
                if (file == NULL) {
                        fprintf(stderr, _("tail: cannot open %s\n"), files[i]);
                        rc = EXIT_FAILURE;
                        continue;
                }
                if (nfiles > 1) {
                        printf("%s==> %s <==\n", i > 0 ? "\n" : "", files[i]);
                }
                if (tail_stream(file, count) != EXIT_SUCCESS) {
                        rc = EXIT_FAILURE;
                }
                fclose(file);
        }
        return rc;
}

const char *help_tail() { return _("Print the last lines of a file (default 10, -n COUNT)"); }
