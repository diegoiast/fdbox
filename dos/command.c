/*
This file is part of fdbox
For license - read license.txt
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/args.h"
#include "lib/applet.h"
#include "lib/environ.h"
#include "lib/strextra.h"

#include "dos/type.h"
#include "fdbox.h"

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
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

struct command_shell_config {
        struct command_config global;
};

static void command_shell_config_init(struct command_shell_config *config);
static bool command_shell_config_parse(int argc, char *argv[], struct command_shell_config *config);
static void command_shell_config_print(const struct command_shell_config *config);
static void command_shell_print_extended_help();

int command_command(int argc, char *argv[]) {
        char line[1024], *pos;
        int code;
        size_t c_argc;
        char *c_argv[256];
        bool parsed_ok;
        struct command_shell_config config;
        struct applet *cmd;
        extern struct applet commands[];

        command_shell_config_init(&config);
        command_shell_config_parse(argc, argv, &config);
        /* command_shell_config_print(&config); */

        if (config.global.show_help) {
                command_shell_print_extended_help();
                return EXIT_SUCCESS;
        }

        do {
                /* TODO - print correct prompt */
                char prompt[256];
                get_prompt("$P$G", prompt, 256);
                printf("%s ", prompt);
                fgets(line, 1024, stdin);

                if ((pos = strchr(line, '\n')) != NULL) {
                    *pos = '\0';
                }
                /* this function will not modify the args, so its marked `const
                 * but some commands (date/time) will modify the args instead of making copies
                 * this is OK for now */
                parsed_ok = command_split_args(line, &c_argc, (const char**) c_argv, 256);
                if (!parsed_ok) {
                        fprintf(stderr, "Command line parsing failed\n");
                        continue;
                }

                if (c_argc == 0) {
                        continue;
                }

                /* Special handling for exit, as it should break the main loop */
                if (strcasecmp(c_argv[0], "exit") == 0) {
                         break;
                }

                /* otherwise - we know the drill, we did it once in main.c */
                cmd = find_applet(c_argv[0], commands);
                if (cmd != NULL) {
                        code = cmd->handler(c_argc, c_argv);
                        if (code != EXIT_SUCCESS) {
                                fprintf(stderr, "Command failed (%d)\n", code);
                        }
                } else {
                        fprintf(stderr, "Command not found\n");
                }
        } while (true);
        return EXIT_SUCCESS;
}

const char *help_command() { return "Runs an MS-DOS interactive shell"; }

/* internal API, all functions bellow should be static */
static void command_shell_config_init(struct command_shell_config *config)
{
        command_config_init(&config->global);
}

static bool command_shell_config_parse(int argc, char *argv[], struct command_shell_config *config)
{
        int c;
        do {
                c = command_config_parse(argc, argv, &config->global);
                switch (tolower(c)) {
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

static void command_shell_config_print(const struct command_shell_config *config)
{
        command_config_print(&config->global);
}

static void command_shell_print_extended_help()
{
        printf("%s\n", help_type());

        printf("   type [files] /l\n");
        printf("   Runs an interactive shell \n");
}
