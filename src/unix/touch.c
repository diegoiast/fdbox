#include <stdio.h>
#include <stdlib.h>

#include "fdbox.h"
#include "lib/nls.h"
#include "unix/touch.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__DJGPP__) || defined(__TURBOC__) || \
    defined(__WATCOMC__)
#include <utime.h>
#define HAVE_UTIME 1
#endif

#ifdef __WIN32__
#include <sys/utime.h>
#define utime _utime
#define HAVE_UTIME 1
#endif

/* touch [file...] - update a file's time to now, creating it if missing. */
static int touch_one(const char *path) {
        FILE *file;
#ifdef HAVE_UTIME
        if (utime(path, NULL) == 0) {
                return 0;
        }
#endif
        file = fopen(path, "ab");
        if (file == NULL) {
                return -1;
        }
        fclose(file);
#ifdef HAVE_UTIME
        utime(path, NULL);
#endif
        return 0;
}

int command_touch(int argc, char *argv[]) {
        int i, rc = EXIT_SUCCESS;

        if (argc < 2) {
                fprintf(stderr, _("touch: a file name is needed\n"));
                return EXIT_FAILURE;
        }
        for (i = 1; i < argc; i++) {
                if (touch_one(argv[i]) != 0) {
                        fprintf(stderr, _("touch: cannot touch %s\n"), argv[i]);
                        rc = EXIT_FAILURE;
                }
        }
        return rc;
}

const char *help_touch() { return _("Create a file or update its time to now"); }
