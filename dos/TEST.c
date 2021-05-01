#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/TEST.h"
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

struct TEST_config {
        bool show_help;
        const char *file_glob[128];
        size_t file_glob_count;
};

static void TEST_config_init(struct TEST_config *config);
static bool TEST_config_parse(int argc, char *argv[], struct TEST_config *config);
static bool TEST_config_print(const struct del_config *config);
static void TEST_print_extended_help();

int command_TEST(int argc, char *argv[]) {
        int i;
        struct TEST_config config;

        TEST_config_init(&config);
        if (!TEST_config_parse(argc, argv, &config)) {
                printf("Failed parsing command line args\n");
                return EXIT_FAILURE;
        }
        TEST_config_print(&config);

        if (config.show_help) {
                TEST_print_extended_help();
                return EXIT_SUCCESS;
        }
        for (i = 0; i < config.file_glob_count; i++) {
        }

        return EXIT_FAILURE;
}

const char *help_TEST() { return "TODO: WRITE DOCUMENTATION"; }

void TEST_config_init(struct TEST_config *config) {
        config->show_help = false;
        config->file_glob_count = 0;
        memset(config->file_glob, 0, sizeof(config->file_glob));
}

bool TEST_config_parse(int argc, char *argv[], struct TEST_config *config) {
        size_t i;
        for (i = 1; i < (size_t)argc; i++) {
                char c1, c2;
                c1 = tolower(argv[i][0]);
                switch (c1) {
                case ARGUMENT_DELIMIER:
                        c2 = tolower(argv[i][1]);
                        switch (c2) {
                        case '?':
                        case 'h':
                                config->show_help = true;
                                break;
                        default:
                                return false;
                        }
                        break;

                default:
                        /* ok its a file */
                        config->file_glob[config->file_glob_count] = argv[i];
                        config->file_glob_count++;
                }
        }
        return true;
}

bool TEST_config_print(const struct del_config *config) {
        int i;
        for (i = 0; i < config->file_glob_count; i++) {
                printf(" -> %s\n", config->file_glob[i]);
        }
}

void TEST_print_extended_help() {
        printf("%s\n", help_del());

        printf("   TODO [files] ....\n");
}
