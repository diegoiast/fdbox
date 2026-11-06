

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
#include "lib/strextra.h"

#include "dos/batch.h"
#include "dos/prompt.h"

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
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

#if defined(__GONWARE__)
#include <gonware/syscall.h>
#include <strings.h>

#define P_WAIT 0

extern char **environ;

/* GDOS starts a program and waits for its end (GDOS_SPAWN); there is no
 * fork. A name without a directory is looked for in the current directory
 * and then in PATH, a name without an extension also as NAME.EXE. */
static int spawn_try(const char *dir, const char *name, char *argv[], int *status) {
        char path[260];
        int ext = strrchr(name, '.') > strrchr(name, '\\');
        long r;

        if (dir && *dir)
                snprintf(path, sizeof(path), "%s%s%s%s", dir,
                         strchr("\\/:", dir[strlen(dir) - 1]) ? "" : "\\", name, ext ? "" : ".EXE");
        else
                snprintf(path, sizeof(path), "%s%s", name, ext ? "" : ".EXE");
        r = gdos_call3(GDOS_SPAWN, path, argv, environ);
        if (r == -GDOS_ENOENT || r == -GDOS_ENOTDIR)
                return 0;
        *status = r < 0 ? 1 : (int)r;
        if (r < 0)
                fprintf(stderr, "%s: cannot start (error %ld)\n", path, -r);
        return 1;
}

int spawnvp(int mode, char *name, char *argv[]) {
        int status = 0;
        const char *p;
        (void)mode;
        if (spawn_try(NULL, name, argv, &status))
                return status;
        if (strpbrk(name, "\\/:"))
                return -1;
        p = getenv("PATH");
        while (p && *p) {
                char dir[260];
                size_t n = strcspn(p, ";");
                if (n && n < sizeof(dir)) {
                        memcpy(dir, p, n);
                        dir[n] = 0;
                        if (spawn_try(dir, name, argv, &status))
                                return status;
                }
                p += n + (p[n] == ';');
        }
        return -1;
}

#elif defined(_POSIX_C_SOURCE) || defined(__APPLE__)

#define P_WAIT 0    /* child runs separately, parent waits until exit */
#define P_NOWAIT 1  /* both concurrent -- not implemented */
#define P_OVERLAY 2 /* child replaces parent, parent no longer exists */

int spawnvp(int mode, char *path, char *argv[]) {
        pid_t child_pid;
        child_pid = fork();
        if (child_pid != 0) {
                int status;
                waitpid(child_pid, &status, 0);
                return status;
        } else {
                execvp(path, argv);
                fprintf(stderr, "an error occurred in execvp\n");
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

int read_line(char line[], int max_size);

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

#if defined(__GONWARE__)
        /* "D:" changes the current drive */
        if (args.argc == 1 && isalpha((unsigned char)args.argv[0][0]) && args.argv[0][1] == ':' &&
            !args.argv[0][2]) {
                if (chdir(args.argv[0]))
                        fprintf(stderr, "Invalid drive specification\n");
                command_args_free(&args);
                return EXIT_SUCCESS;
        }
#endif

        /* Special handling for exit, as it should break the main loop */
        if (strcasecmp(args.argv[0], "exit") == 0) {
                command_args_free(&args);
                return EXIT_FAILURE;
        }

        cmd = find_applet(CASE_INSENSITVE, args.argv[0], commands);
        if (cmd != NULL) {
                code = cmd->handler(args.argc, args.argv);
                errno = code;
#if !defined(__GONWARE__)
                /* on GonWare the commands report their errors themselves */
                if (code != EXIT_SUCCESS) {
                        fprintf(stderr, "Command failed (%d)\n", code);
                }
#endif
        } else {
                int err;
                char batch[260];
                readline_deinit();
                err = spawnvp(P_WAIT, args.argv[0], args.argv);
                readline_init();
#if defined(__GONWARE__)
                /* spawnvp: -1 if there is no such program, else its exit status */
                if (err == -1 && batch_find(args.argv[0], batch, sizeof(batch)))
                        err = batch_run(batch, args.argc, args.argv);
                else if (err == -1) {
                        fprintf(stderr, "Bad command or file name - %s\n", args.argv[0]);
                        err = 1;
                }
                errno = err; /* the error level */
#else
                (void)batch;
                if (err != 0) {
                        fprintf(stderr, "Command not found - %d \n", err);
                }
#endif
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
                int l;

                t = getenv("PROMPT");
                if (t == NULL) {
                        command_prompt(1, NULL);
                        t = getenv("PROMPT");
                }
                get_prompt(t, prompt, 256);
                printf("%s", prompt);
                l = read_line(line, 1024);
                if (l < 0) {
                        return EXIT_FAILURE;
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
