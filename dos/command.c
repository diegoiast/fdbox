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

#include "dos/prompt.h"
#include "fdbox.h"

#if defined(__TURBOC__)
#include <io.h>
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#endif

#ifdef _POSIX_C_SOURCE
#include <stdbool.h>
#include <unistd.h>
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
static char* find_batch_in_path(const char* batch_file_name);

/* TODO - I am unsure if this is the best way to tell the main loop
 * we should exit. For now it works
 */

int command_execute_line_new(const char *line) {
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
                fprintf(stderr, "Command not found\n");
                errno = ENOENT;
        }

        command_args_free(&args);
        return EXIT_SUCCESS;
}

int command_execute_line_old(char *line) {
        size_t c_argc;
        char *c_argv[256];
        const char* command_name;
        bool parsed_ok;
        bool is_silent = false; /* TODO use global state from echo! */
        struct applet *cmd;
        int code;
        extern struct applet commands[];
        bool is_interactive = isatty(fileno(stdin));

        /* this function will not modify the args, so its marked `const
         * but some commands (date/time) will modify the args instead of making copies
         * this is OK for now */
        parsed_ok = command_split_args(line, &c_argc, (const char **)c_argv, 256);
        if (!parsed_ok) {
                fprintf(stderr, "Command line parsing failed\n");
                return EXIT_SUCCESS;
        }

        if (c_argc == 0) {
                return EXIT_SUCCESS;
        }

        command_name = c_argv[0];
        if (*command_name == '@') {
                is_silent = !is_silent;
                command_name = command_name+1;
        }

        /* Special handling for exit, as it should break the main loop */

        if (strcasecmp(command_name, "exit") == 0) {
                if (is_interactive) {
                        printf("interactive shell, not exit, just redirecting stdin back\n");
                        if ((freopen("", "r", stdin) == NULL)) {
                            fprintf(stderr, "Error reopening stdin\n");
                            abort();
                        }
                }
                else {
                        printf("exit shell!\n");
                        return EXIT_SUCCESS;
                }
        }

        cmd = find_applet(CASE_INSENSITVE, c_argv[0], commands);
        /* ok, this fails, since we modify the original line, this will get fixed soon */
        if (!is_silent && !is_interactive) {
                puts(line);
        }
       if (cmd != NULL) {
                code = cmd->handler(c_argc, c_argv);
                errno = code;
                if (code != EXIT_SUCCESS) {
                        fprintf(stderr, "Command failed (%d)\n", code);
                }
        } else {
                /* is it a batch file ? */
                char* batch_file_path = find_batch_in_path(command_name);
                bool ok = false;

                if (batch_file_path != NULL) {
                        if (freopen(batch_file_path, "r", stdin)) {
                                ok = true;
                        }
                } else {
                        /* execvp */
                }

                if (!ok) {
                        fprintf(stderr, "Command not found\n");
                        errno = ENOENT;
                }
                free(batch_file_path);
        }

        return EXIT_SUCCESS;
}

int command_execute_line(char *line) { return command_execute_line_new(line); }

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
                bool is_interactive = isatty(fileno(stdin));
                if (is_interactive) {
                        char prompt[256];
                        const char *t;
                        t = getenv("PROMPT");
                        if (t == NULL) {
                                command_prompt(1, NULL);
                                t = getenv("PROMPT");
                        }
                        get_prompt(t, prompt, 256);
                        printf("%s", prompt);
                }

                if (feof(stdin)) {
                        printf("EOF, exit!");
                        break;
                }
                fgets(line, 1024, stdin);
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
                        break;
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

static char* find_batch_in_path(const char* batch_file_name)
{
        if (file_exists(batch_file_name)) {
                return strdup(batch_file_name);
        }
        return NULL;
}
