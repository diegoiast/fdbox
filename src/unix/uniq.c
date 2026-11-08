#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fdbox.h"
#include "lib/nls.h"
#include "unix/uniq.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#define UNIQ_LINE 4096

/* Print a run of equal lines once, honouring the flags. */
static void uniq_emit(const char *line, long run, int count, int only_dup, int only_uni) {
        if (only_dup && run < 2) {
                return;
        }
        if (only_uni && run > 1) {
                return;
        }
        if (count) {
                printf("%4ld ", run);
        }
        fputs(line, stdout);
}

/* uniq [-c] [-d] [-u] [file] - collapse adjacent equal lines. */
int command_uniq(int argc, char *argv[]) {
        int count = 0, only_dup = 0, only_uni = 0;
        const char *name = NULL;
        FILE *file = stdin;
        char prev[UNIQ_LINE], cur[UNIQ_LINE];
        int have_prev = 0;
        long run = 0;
        int i;

        for (i = 1; i < argc; i++) {
                char *arg = argv[i];
                if (arg[0] == '-' && arg[1] != '\0') {
                        if (strchr(arg, 'c') != NULL) {
                                count = 1;
                        }
                        if (strchr(arg, 'd') != NULL) {
                                only_dup = 1;
                        }
                        if (strchr(arg, 'u') != NULL) {
                                only_uni = 1;
                        }
                        continue;
                }
                name = arg;
        }
        if (name != NULL) {
                file = fopen(name, "r");
                if (file == NULL) {
                        fprintf(stderr, _("uniq: cannot open %s\n"), name);
                        return EXIT_FAILURE;
                }
        }

        while (fgets(cur, sizeof(cur), file) != NULL) {
                if (have_prev && strcmp(cur, prev) == 0) {
                        run++;
                        continue;
                }
                if (have_prev) {
                        uniq_emit(prev, run, count, only_dup, only_uni);
                }
                strcpy(prev, cur);
                have_prev = 1;
                run = 1;
        }
        if (have_prev) {
                uniq_emit(prev, run, count, only_dup, only_uni);
        }
        if (file != stdin) {
                fclose(file);
        }
        return EXIT_SUCCESS;
}

const char *help_uniq() { return _("Collapse adjacent equal lines (-c -d -u)"); }
