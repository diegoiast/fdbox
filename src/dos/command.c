

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
#include "lib/readline.h"

#include "dos/prompt.h"

#if defined(__TURBOC__)
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#include <io.h>
#include <process.h>
#endif

#if defined(__WATCOMC__)
#include <io.h>
#include <process.h>
#include <strings.h>
#endif

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__)
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#ifdef __WIN32__
#include <conio.h>
#include <io.h>
#include <process.h>
#include <stdbool.h>
#include <windows.h>
#endif

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__)

#define P_WAIT 0    /* child runs separately, parent waits until exit */
#define P_NOWAIT 1  /* both concurrent -- not implemented */
#define P_OVERLAY 2 /* child replaces parent, parent no longer exists */

/* TODO properly support values documented here:
   https://www.qnx.com/developers/docs/6.5.0SP1.update/com.qnx.doc.neutrino_lib_ref/s/spawnvp.html
 */
int spawnvp(int mode, char *path, char *argv[]) {
        pid_t child_pid;
        child_pid = fork();
        if (child_pid != 0) {
                int status = 0;
                int rc = waitpid(child_pid, &status, 0);
                if (rc < 0) {
                    return rc;
                }
                /* https://man7.org/linux/man-pages/man2/wait.2.html */
                int a_status = WEXITSTATUS(status);
                int b_staus = WIFEXITED(status);
                if (WIFEXITED(status)) {
                    return WEXITSTATUS(status);
                }
                /* TODO - use better errors */
                return -1;
        } else {
                execvp(path, argv);
                /* fprintf(stderr, "an error occurred in execvp\n"); */
                abort();
        }
        return 0;
}
#endif

struct command_shell_config {
        struct command_config global;
};

static void command_shell_config_init(struct command_shell_config *config);
static bool command_shell_config_parse(int argc, char *argv[], struct command_shell_config *config);
static void command_shell_config_print(const struct command_shell_config *config);
static void command_shell_print_extended_help();
static char *find_batch_in_path(const char *batch_file_name);

int read_line(char line[], int max_size);

/* TODO - I am unsure if this is the best way to tell the main loop
 * we should exit. For now it works
 */

int command_execute_line(const char *line) {
        struct command_args args;
        struct applet *cmd;
        const char *command_name;
        int code;
        bool is_silent = false; /* TODO use global state from echo! */
        extern struct applet commands[];
        bool is_interactive = isatty(fileno(stdin));

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

        command_name = args.argv[0];
        if (*command_name == '@') {
                is_silent = !is_silent;
                command_name = command_name + 1;
        }
        /* ok, this fails, since we modify the original line, this will get fixed soon */
        if (!is_silent && !is_interactive) {
                puts(line);
        }

        /* Special handling for exit, as it should break the main loop */
        if (strcasecmp(command_name, "exit") == 0) {
                command_args_free(&args);
                return EXIT_FAILURE;
        }

        cmd = find_applet(CASE_INSENSITVE, command_name, commands);
        if (cmd != NULL) {
                code = cmd->handler(args.argc, args.argv);
                errno = code;
                if (code != EXIT_SUCCESS) {
                        fprintf(stderr, "Command failed (%d)\n", code);
                }
        } else {
                /* is it a batch file ? */
                char *batch_file_path = find_batch_in_path(command_name);
                bool ok = false;

                if (batch_file_path != NULL) {
                        if (freopen(batch_file_path, "r", stdin)) {
                                ok = true;
                        }
                } else {
                        int err;
                        readline_deinit();
                        err = spawnvp(P_WAIT, args.argv[0], args.argv);
                        readline_init();
                        if (err != 0) {
                                /* fprintf(stderr, "Command not found - %d \n", err); */
                        } else {
                            ok = true;
                        }
                }

                if (!ok) {
                        fprintf(stderr, "Command not found\n");
                        errno = ENOENT;
                }
                free(batch_file_path);
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
                int l;
                bool is_interactive = isatty(fileno(stdin));
                if (is_interactive) {
                        char prompt[256];
                        const char *t;
                        command_prompt(1, NULL);
                        t = getenv("PROMPT");
                        if (t == NULL) {
                                command_prompt(1, NULL);
                                t = getenv("PROMPT");
                        }
                        get_prompt(t, prompt, 256);
                        printf("%s", prompt);
                }
                l = read_line(line, 1024);
                if (l < 0) {
                        return EXIT_FAILURE;
                }

                if (feof(stdin)) {
                        printf("EOF, exit!");
                        break;
                }
                if ((pos = strchr(line, '\r')) != NULL) {
                        *pos = '\0';
                }
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

static char *find_batch_in_path(const char *batch_file_name) {
        char *f;
        if (file_exists(batch_file_name)) {
                return strdup(batch_file_name);
        }
        f = malloc(strlen(batch_file_name) + 4);
        strcpy(f, batch_file_name);
        strcat(f, ".bat");
        if (file_exists(f)) {
                return f;
        }
        free(f);
        return NULL;
}

/* read line */
bool is_interactive() { return isatty(fileno(stdin)); }

int read_line_simple(char line[], int max_size) {
        line[0] = 0;
        fgets(line, max_size, stdin);
        return strlen(line);
}

int read_line(char line[], int max_size) {
        int l;
        if (!is_interactive()) {
                return read_line_simple(line, max_size);
        }
        l = read_string(line, max_size);
        return l;
}
