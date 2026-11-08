#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "fdbox.h"
#include "lib/args.h"
#include "lib/nls.h"
#include "unix/wc.h"

/*
This file is part of fdbox
For license - read license.txt
*/

struct counts {
        long lines;
        long words;
        long bytes;
};

static void wc_stream(FILE *file, struct counts *c) {
        int ch, in_word = 0;

        while ((ch = getc(file)) != EOF) {
                c->bytes++;
                if (ch == '\n') {
                        c->lines++;
                }
                if (isspace(ch)) {
                        in_word = 0;
                } else if (!in_word) {
                        in_word = 1;
                        c->words++;
                }
        }
}

static void wc_print(const struct counts *c, int l, int w, int b, const char *name) {
        if (l) {
                printf("%7ld", c->lines);
        }
        if (w) {
                printf("%7ld", c->words);
        }
        if (b) {
                printf("%7ld", c->bytes);
        }
        if (name != NULL) {
                printf(" %s", name);
        }
        printf("\n");
}

/* wc [-l] [-w] [-c] [file...] - with no flag, all three counts. */
int command_wc(int argc, char *argv[]) {
        int want_l = 0, want_w = 0, want_c = 0;
        const char *files[ARG_MAX_FILES];
        int nfiles = 0;
        int i, rc = EXIT_SUCCESS;
        struct counts total;

        for (i = 1; i < argc; i++) {
                char *arg = argv[i];
                if (arg[0] == '-' && arg[1] != '\0') {
                        char *p;
                        for (p = arg + 1; *p; p++) {
                                switch (*p) {
                                case 'l':
                                        want_l = 1;
                                        break;
                                case 'w':
                                        want_w = 1;
                                        break;
                                case 'c':
                                        want_c = 1;
                                        break;
                                default:
                                        break;
                                }
                        }
                        continue;
                }
                if (nfiles < ARG_MAX_FILES) {
                        files[nfiles++] = arg;
                }
        }
        if (!want_l && !want_w && !want_c) {
                want_l = want_w = want_c = 1;
        }

        total.lines = total.words = total.bytes = 0;
        if (nfiles == 0) {
                struct counts c;
                c.lines = c.words = c.bytes = 0;
                wc_stream(stdin, &c);
                wc_print(&c, want_l, want_w, want_c, NULL);
                return EXIT_SUCCESS;
        }
        for (i = 0; i < nfiles; i++) {
                struct counts c;
                FILE *file = fopen(files[i], "rb");
                if (file == NULL) {
                        fprintf(stderr, _("wc: cannot open %s\n"), files[i]);
                        rc = EXIT_FAILURE;
                        continue;
                }
                c.lines = c.words = c.bytes = 0;
                wc_stream(file, &c);
                fclose(file);
                wc_print(&c, want_l, want_w, want_c, files[i]);
                total.lines += c.lines;
                total.words += c.words;
                total.bytes += c.bytes;
        }
        if (nfiles > 1) {
                wc_print(&total, want_l, want_w, want_c, _("total"));
        }
        return rc;
}

const char *help_wc() { return _("Count lines, words and bytes (-l -w -c)"); }
