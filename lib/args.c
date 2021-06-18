#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "fdbox.h"
#include "lib/args.h"

/*
This file is part of fdbox
For license - read license.txt
*/

/* https://stackoverflow.com/a/7776146 */
void hexDump(char *desc, void *addr, int len) {
        int i;
        unsigned char buff[17];
        unsigned char *pc = (unsigned char *)addr;

        /* Output description if given. */
        if (desc != NULL) {
                printf("%s: %p\n", desc, addr);
        }
        if (len == 0) {
                printf("  ZERO LENGTH\n");
                return;
        }
        if (len < 0) {
                printf("  NEGATIVE LENGTH: %i\n", len);
                return;
        }

        /* Process every byte in the data. */
        for (i = 0; i < len; i++) {
                /* Multiple of 16 means new line (with line offset). */

                if ((i % 16) == 0) {
                        /* Just don't print ASCII for the zeroth line. */
                        if (i != 0)
                                printf("  %s\n", buff);

                        /* Output the offset. */
                        printf("  %04x ", i);
                }

                /* Now the hex code for the specific character. */
                printf(" %02x", pc[i]);

                /* And store a printable ASCII character for later */
                if ((pc[i] < 0x20) || (pc[i] > 0x7e))
                        buff[i % 16] = '.';
                else
                        buff[i % 16] = pc[i];
                buff[(i % 16) + 1] = '\0';
        }

        /* Pad out last line if not exactly 16 characters. */
        while ((i % 16) != 0) {
                printf("   ");
                i++;
        }

        /* And print the final ASCII bit. */
        printf("  %s\n", buff);
}

void print_agrs(int argc, char *argv[]) {
        int i;
        if (argc == 0) {
                printf("No args\n");
                return;
        }
        if (argv == NULL) {
                printf("argv = NULL\n");
                return;
        }
        printf("argv =%p\n", argv);
        for (i = 0; i < argc; i++) {
                printf("%d: '%s', ", i, argv[i]);
        }
}


void command_config_init(struct command_config *config) {
        config->verbose = false;
        config->show_help = false;
        config->files.count = 0;
        config->files.overflow = 0;
        memset(config->files.file, 0, sizeof(config->files));
        config->state.current_argument = 1;
}

int command_config_parse(int argc, char *argv[], struct command_config *config) {
        char c;
        char *arg;

        if (config->state.current_argument >= argc) {
                return ARG_DONE;
        }

        arg = argv[config->state.current_argument];
        config->state.current_argument++;

        switch (arg[0]) {
        case ARGUMENT_DELIMIER:
                c = arg[1];
                switch (tolower(c)) {
                case '?':
                case 'h':
                        config->show_help = true;
                        return ARG_PROCESSED;
                case 'v':
                        config->verbose = true;
                        return ARG_PROCESSED;
                default:
                        return c;
                }
                break;

        default:
                /* ok its a file */
                if (config->files.count == ARG_MAX_FILES) {
                        config->files.overflow ++;
                        return ARG_PROCESSED;
                }
                config->files.file[config->files.count] = arg;
                config->files.count ++;
                return ARG_PROCESSED;
        }
}

const char *command_config_next(int argc, char *argv[], struct command_config *config) {
        char *arg;

        if (config->state.current_argument >= argc) {
                return NULL;
        }
        arg = argv[config->state.current_argument];
        config->state.current_argument++;
        return arg;
}

void command_config_print(const struct command_config *config)
{
        size_t i;

        printf("VERBOSE = %d\n", config->verbose);
        printf("SHOW HELP = %d\n", config->show_help);
        printf("Glob %zu requested, %zu overflow\n", config->files.count+config->files.overflow, config->files.overflow);
        for (i = 0; i < config->files.count; i++) {
                printf(" -> %s\n", config->files.file[i]);
        }
}
