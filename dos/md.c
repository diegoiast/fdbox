#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/md.h"
#include "fdbox.h"
#include "lib/args.h"
#include "lib/strextra.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#ifdef __MSDOS__
#include "lib/tc202/dos-glob.h"
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#include <sys/stat.h>
#endif

#ifdef _POSIX_C_SOURCE
#include <glob.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#endif

#ifdef __WIN32__
#include "lib/win32/dirent.h"
#include "lib/win32/win32-glob.h"
#include <stdbool.h>
#include <unistd.h>
#include <utime.h>
#endif

struct mkdir_config {
        bool recursive;
        struct command_config global;
};

static void mkdir_config_init(struct mkdir_config *config);
static bool mkdir_config_parse(int argc, char *argv[], struct mkdir_config *config);
static void mkdir_config_print(const struct mkdir_config *config);
static void mkdir_print_extended_help();
static bool mkdir_create_dir(char *dir_name, const struct mkdir_config *config);

int command_md(int argc, char *argv[]) {
        size_t i;
        struct mkdir_config config;

        mkdir_config_init(&config);
        if (!mkdir_config_parse(argc, argv, &config)) {
                printf("Failed parsing command line args\n");
                return EXIT_FAILURE;
        }
        /* mkdir_config_print(&config); */

        if (config.global.show_help) {
                mkdir_print_extended_help();
                return EXIT_SUCCESS;
        }
        for (i = 0; i < config.global.files.count; i++) {
                /* This is used only to move forward in the pointer, we will not modify the
                 * content of `dir_name`
                 */
                char *name = (char*) config.global.files.file[i];
                bool status = mkdir_create_dir(name, &config);
                if (!status) {
                        return EXIT_FAILURE;
                }
        }
        return EXIT_SUCCESS;
}

const char *help_md() { return "Create one or more directories"; }

static void mkdir_config_init(struct mkdir_config *config) {
        config->recursive = false;
        command_config_init(&config->global);
}

static bool mkdir_config_parse(int argc, char *argv[], struct mkdir_config *config) {
        int c;
        do {
                c = command_config_parse(argc, argv, &config->global);
                switch (tolower(c)) {
                case 'r':
                        config->recursive = true;
                        break;
                case ARG_PROCESSED:
                        break;
                case ARG_DONE:
                        break;
                default:
                        return false;
                }
        } while (c >= 0);

        return true;
}

static void mkdir_config_print(const struct mkdir_config *config) {
        size_t i;

        printf("recursive: %s\n", str_bool(config->recursive));
        command_config_print(&config->global);
}

static void mkdir_print_extended_help() {
        printf("%s\n", help_md());

        printf("   md [files] /r\n");
        printf("   /r Recursive creation (otherwise, creates only top level)\n");
}

static bool mkdir_create_dir(char *dir_name, const struct mkdir_config *config) {
        /* workflow - we first try the whole dir, if we fail
         * we will truncate the full path after each dir, and retry
         * trversing from left to right.
         * For example:
         * mkdir foo/bar/baz
         * fails ?
         * mkdir foo/
         * mkdir foo/bar
         * mkdir foo/bar/zaz
         *
         * The idea is that after each iteration, we truncate the original
         * path at the first "/" found.
         */
        int r;

        char *c = (char*) dir_name;
        char c2 = dir_name[0];
        bool failed_once = false;

        do {
#if defined(__MSDOS__) || defined(__WIN32__)
                r = mkdir(dir_name);
#else
                r = mkdir(dir_name, 0755);
#endif
                failed_once |= r != 0;
                *c = c2;
                c = strchr(c + 1, DIRECTORY_DELIMITER[0]);
                if (c == NULL) {
                        break;
                }
                c2 = *c;
                *c = 0;

                if (!config->recursive) {
                        failed_once = false;
                        break;
                }
        } while (r != 0);

        if (failed_once) {
#if defined(__MSDOS__) || defined(__WIN32__)
                r = mkdir(dir_name);
#else
                r = mkdir(dir_name, 0755);
#endif
        }

        if (config->global.verbose) {
                if (r == 0) {
                        printf("Created %s\n", dir_name);
                } else {
                        printf("Failed creating %s\n", dir_name);
                }
        }
        return r == 0;
}
