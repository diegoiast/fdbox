#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "dos/type.h"
#include "lib/args.h"

#define LINE_PRINT "%5zd  "

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#undef LINE_PRINT
#define LINE_PRINT "%5d  "
#endif

#ifdef _POSIX_C_SOURCE
#include <stdbool.h>
#endif

#ifdef __WIN32__
#include <stdbool.h>
#endif

/*
This file is part of fdbox
For license - read license.txt
*/

struct type_config {
        bool show_lines;
        struct command_config global;
};

static void type_config_init(struct type_config *config);
static bool type_config_parse(int argc, char *argv[], struct type_config *config);
static void type_config_print(const struct type_config *config);
static void type_print_extended_help();

int command_type(int argc, char *argv[]) {
        struct type_config config;
        size_t i;

        type_config_init(&config);
        type_config_parse(argc, argv, &config);
        /* type_config_print(&config); */

        if (config.global.show_help) {
                type_print_extended_help();
                return EXIT_SUCCESS;
        }
        if (config.global.files.count == 0) {
                type_print_extended_help();
                return EXIT_SUCCESS;
        }

        for (i = 0; i < config.global.files.count; i++) {
                const char *file_name = config.global.files.file[i];
                char line[1024];
                size_t line_number = 0;
                FILE *file = fopen(file_name, "r");

                if (file == NULL) {
                        return EXIT_FAILURE;
                }

                while (fgets(line, sizeof(line), file)) {
                        line_number++;
                        if (config.show_lines) {
                                printf(LINE_PRINT, line_number);
                        }
                        printf("%s", line);
                }
                fclose(file);
        }

        return EXIT_SUCCESS;
}

const char *help_type() { return "Prints the content of a file"; }

/* internal API, all functions bellow should be static */
static void type_config_init(struct type_config *config) {
        config->show_lines = false;
        command_config_init(&config->global);
}

static bool type_config_parse(int argc, char *argv[], struct type_config *config) {
        int c1, c2;
        do {
                c1 = command_config_parse(argc, argv, &config->global);
                c2 = tolower(c1);
                switch (c2) {
                case 'l':
                        config->show_lines = true;
                        break;
                case ARG_PROCESSED:
                        break;
                case ARG_DONE:
                        break;
                default:
                        return false;
                }
        } while (c1 >= 0);
        return true;
}

static void type_config_print(const struct type_config *config) {
        printf("Show lines = %d", config->show_lines);
        command_config_print(&config->global);
}

static void type_print_extended_help() {
        printf("%s\n", help_type());

        printf("   type [files] /l\n");
        printf("   /l print line numbers\n");
}
