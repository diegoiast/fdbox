#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdbox.h"
#include "lib/args.h"
#include "lib/nls.h"
#include "unix/head.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#define DEFAULT_LINES 10

/* Print the first `count` lines of an open stream. A line longer than the
 * buffer still counts as one line (it is counted when its newline arrives). */
static void head_stream(FILE *file, long count) {
        char line[1024];
        long printed = 0;

        while (printed < count && fgets(line, sizeof(line), file) != NULL) {
                fputs(line, stdout);
                if (strchr(line, '\n') != NULL) {
                        printed++;
                }
        }
}

int command_head(int argc, char *argv[]) {
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
                head_stream(stdin, count);
                return EXIT_SUCCESS;
        }
        for (i = 0; i < nfiles; i++) {
                FILE *file = fopen(files[i], "r");
                if (file == NULL) {
                        fprintf(stderr, _("head: cannot open %s\n"), files[i]);
                        rc = EXIT_FAILURE;
                        continue;
                }
                if (nfiles > 1) {
                        printf("%s==> %s <==\n", i > 0 ? "\n" : "", files[i]);
                }
                head_stream(file, count);
                fclose(file);
        }
        return rc;
}

const char *help_head() { return _("Print the first lines of a file (default 10, -n COUNT)"); }
