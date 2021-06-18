#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/args.h"
#include "dos/type.h"
#include "fdbox.h"

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
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

        for (i=0; i< config.global.files.count; i++) {
                const char* file_name = config.global.files.file[i];
                FILE *file = fopen(file_name, "r");
                char line[1024];
                size_t line_number = 0;

                while (fgets(line, sizeof(line), file)) {
                        line_number ++;
                        if (config.show_lines) {
                                printf("%5zu  ", line_number);
                        }
                        printf("%s", line);
                }
                fclose(file);
        }

        return EXIT_FAILURE;
}

const char *help_type() { return "Prints the content of a file"; }

/* internal API, all functions bellow should be static */
static void type_config_init(struct type_config *config)
{
        config->show_lines = false;
        command_config_init(&config->global);
}

static bool type_config_parse(int argc, char *argv[], struct type_config *config)
{
        int c;
        do {
                c = command_config_parse(argc, argv, &config->global);
                switch (tolower(c)) {
                case 'l':
                        config->show_lines= true;
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

static void type_config_print(const struct type_config *config)
{
        printf("Show lines = %d", config->show_lines);
        command_config_print(&config->global);
}

static void type_print_extended_help()
{
        printf("%s\n", help_type());

        printf("   type [files] /l\n");
        printf("   /l print line numbers\n");
}
