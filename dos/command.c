/*
This file is part of fdbox
For license - read license.txt
*/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/applet.h"
#include "lib/args.h"
#include "lib/environ.h"
#include "lib/strextra.h"
#include "lib/readline.h"

#include "dos/prompt.h"

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#endif

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__)
#include <stdbool.h>
#include <unistd.h>
#endif

#ifdef __WIN32__
#include <io.h>
#include <conio.h>
#include <stdbool.h>
#include <windows.h>
#endif

struct command_shell_config {
        struct command_config global;
};

static void command_shell_config_init(struct command_shell_config *config);
static bool command_shell_config_parse(int argc, char *argv[], struct command_shell_config *config);
static void command_shell_config_print(const struct command_shell_config *config);
static void command_shell_print_extended_help();

size_t read_line(char line[], int max_size);

/* TODO - I am unsure if this is the best way to tell the main loop
 * we should exit. For now it works
 */

int command_execute_line(const char *line) {
        struct command_args args;
        struct applet *cmd;
        int code;
        extern struct applet commands[];

        /* this function will not modify the args, so its marked `const
         * but some commands (date/time) will modify the args instead of making copies
         * this is OK for now */

        if (command_args_split(line, &args)) {
                fprintf(stderr, "Command line parsing failed\n");
                return EXIT_SUCCESS;
        }

        if (args.argc == 0) {
                command_args_free(&args);
                return EXIT_SUCCESS;
        }

        /* Special handling for exit, as it should break the main loop */
        if (strcasecmp(args.argv[0], "exit") == 0) {
                command_args_free(&args);
                return EXIT_FAILURE;
        }

        cmd = find_applet(CASE_INSENSITVE, args.argv[0], commands);
        if (cmd != NULL) {
                code = cmd->handler(args.argc, args.argv);
                errno = code;
                if (code != EXIT_SUCCESS) {
                        fprintf(stderr, "Command failed (%d)\n", code);
                }
        } else {
                fprintf(stderr, "Command not found - [%s][%s]\n", args.argv[0], line);
                errno = ENOENT;
        }

        command_args_free(&args);
        return EXIT_SUCCESS;
}

int command_command(int argc, char *argv[]) {
        char line[1024], *pos;
        int code;
        struct command_shell_config config;

        command_shell_config_init(&config);
        command_shell_config_parse(argc, argv, &config);
        /* command_shell_config_print(&config); */

        if (config.global.show_help) {
                command_shell_print_extended_help();
                return EXIT_SUCCESS;
        }

        do {
                char prompt[256];
                const char *t;

                t = getenv("PROMPT");
                if (t == NULL) {
                        command_prompt(1, NULL);
                        t = getenv("PROMPT");
                }
                get_prompt(t, prompt, 256);
                printf("%s", prompt);
                read_line(line, 1024);

                if ((pos = strchr(line, '\n')) != NULL) {
                        *pos = '\0';
                }
                code = command_execute_line(line);
                if (code != EXIT_SUCCESS) {
                        break;
                }
        } while (true);
        return EXIT_SUCCESS;
}

const char *help_command() { return "Runs an MS-DOS interactive shell"; }

/* internal API, all functions bellow should be static */
static void command_shell_config_init(struct command_shell_config *config) {
        command_config_init(&config->global);
}

static bool command_shell_config_parse(int argc, char *argv[],
                                       struct command_shell_config *config) {
        int c1, c2;
        do {
                c1 = command_config_parse(argc, argv, &config->global);
                c2 = tolower(c1);
                switch (c2) {
                case ARG_PROCESSED:
                case ARG_DONE:
                        break;
                default:
                        return false;
                }
        } while (c1 >= 0);
        return true;
}

static void command_shell_config_print(const struct command_shell_config *config) {
        command_config_print(&config->global);
}

static void command_shell_print_extended_help() {
        printf("%s\n", help_command());

        printf("   command {shell command} /l\n");
        printf("   Runs an interactive shell \n");
        printf("   TODO: properly implement the command.com swithces \n");
}


/* read line */
bool is_interactive()
{
        return isatty(fileno(stdin));
}

int read_line_simple(char line[], int max_size) {
        line[0] = 0;
        fgets(line, max_size, stdin);
        return strlen(line);
}


size_t read_line(char line[], int max_size) {
        int l;
        if (!is_interactive()) {
                return read_line_simple(line, max_size);
        }
        l = read_string(line, max_size);
        if (l < 0) {
                l = read_line_simple(line, max_size);
        }
        return l;
}
