#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/md.h"
#include "fdbox.h"
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
        bool show_help;
        bool verbose;
        bool recursive;
        char *dir_names[128];
        size_t dir_names_count;
};

static void mkdir_config_init(struct mkdir_config *config);
static bool mkdir_config_parse(int argc, char *argv[], struct mkdir_config *config);
static bool mkdir_config_print(const struct mkdir_config *config);
static void mkdir_print_extended_help();
static bool mkdir_create_dir(char *dir_name, const struct mkdir_config *config);

int command_md(int argc, char *argv[]) {
        int i;
        struct mkdir_config config;

        mkdir_config_init(&config);
        if (!mkdir_config_parse(argc, argv, &config)) {
                printf("Failed parsing command line args\n");
                return EXIT_FAILURE;
        }
        /* mkdir_config_print(&config); */

        if (config.show_help) {
                mkdir_print_extended_help();
                return EXIT_SUCCESS;
        }
        for (i = 0; i < config.dir_names_count; i++) {
                char *name = config.dir_names[i];
                bool status = mkdir_create_dir(name, &config);
                if (!status) {
                        return EXIT_FAILURE;
                }
        }
        return EXIT_SUCCESS;
}

const char *help_md() { return "Create one or more directories"; }

void mkdir_config_init(struct mkdir_config *config) {
        config->show_help = false;
        config->verbose = false;
        config->recursive = false;
        config->dir_names_count = 0;
        memset(config->dir_names, 0, sizeof(config->dir_names));
}

bool mkdir_config_parse(int argc, char *argv[], struct mkdir_config *config) {
        size_t i;
        for (i = 1; i < (size_t)argc; i++) {
                char c1, c2;
                c1 = tolower(argv[i][0]);
                switch (c1) {
                case ARGUMENT_DELIMIER:
                        c2 = tolower(argv[i][1]);
                        switch (c2) {
                        case 'v':
                                config->verbose = true;
                                break;
                        case 'r':
                                config->recursive = true;
                                break;
                        case '?':
                        case 'h':
                                config->show_help = true;
                                break;
                        default:
                                return false;
                        }
                        break;

                default:
                        /* ok its a dir */
                        config->dir_names[config->dir_names_count] = argv[i];
                        config->dir_names_count++;
                }
        }
        return true;
}

bool mkdir_config_print(const struct mkdir_config *config) {
        int i;
        printf("verbose: %s\n", str_bool(config->verbose));
        printf("recursive: %s\n", str_bool(config->recursive));
        printf("show help %s\n", str_bool(config->show_help));
        for (i = 0; i < config->dir_names_count; i++) {
                printf(" -> %s\n", config->dir_names[i]);
        }
}

void mkdir_print_extended_help() {
        printf("%s\n", help_md());

        printf("   md [files] /p /v /r /f\n");
        printf("   /v Verbose delete\n");
        printf("   /r Recursive delete (delete subdirs)\n");
        printf("   /f Force deletion of subdirs, or RO files\n");
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
        char *c = dir_name;
        char c2 = dir_name[0];
        bool failed_once = false;

        do {
#if defined(__MSDOS__)
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
        } while (r = !0);

        if (failed_once) {
#if defined(__MSDOS__)
                r = mkdir(dir_name);
#else
                r = mkdir(dir_name, 0755);
#endif
        }
        return r == 0;
}
