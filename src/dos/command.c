

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
#include "dos/echo.h"
#include "dos/for.h"
#include "dos/prompt.h"
#include "fdbox.h"

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

static int execute_command(const char *line) {
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

        /* ECHO prints the text as written: "ECHO a=b" keeps "a=b" */
        if (strcasecmp(args.argv[0], "echo") == 0) {
                const char *t = line;
                while (isspace((unsigned char)*t))
                        t++;
                t += 4;
                if (*t == ' ' || *t == '\t')
                        t++;
                command_args_free(&args);
                errno = echo_line(t);
                return EXIT_SUCCESS;
        }

        /* FOR takes its command as written, before words are split */
        if (strcasecmp(args.argv[0], "for") == 0) {
                command_args_free(&args);
                errno = for_line(line);
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

#if defined(__GONWARE__) || defined(_POSIX_C_SOURCE) || defined(__APPLE__)
#include <fcntl.h>
#include <unistd.h>

#define PIPE_STAGES 8

/* "<", ">" and ">>" of one command */
struct redirect {
        char in[260], out[260];
        bool append;
};

/* takes the redirections outside quotes out of cmd; false: a name is missing */
static bool redirect_parse(char *cmd, struct redirect *r) {
        char *s = cmd, *o = cmd, quote = 0;

        memset(r, 0, sizeof(*r));
        while (*s) {
                char *dst;
                size_t n = 0;

                if (quote || *s == '"') {
                        quote = quote ? (*s == quote ? 0 : quote) : *s;
                        *o++ = *s++;
                        continue;
                }
                if (*s != '<' && *s != '>') {
                        *o++ = *s++;
                        continue;
                }
                dst = *s == '<' ? r->in : r->out;
                if (*s == '>')
                        r->append = s[1] == '>';
                s += *s == '>' && s[1] == '>' ? 2 : 1;
                while (isspace((unsigned char)*s))
                        s++;
                if (*s == '"') {
                        for (s++; *s && *s != '"'; s++)
                                if (n < 259)
                                        dst[n++] = *s;
                        s += *s == '"';
                } else {
                        for (; *s && !isspace((unsigned char)*s) && !strchr("<>|", *s); s++)
                                if (n < 259)
                                        dst[n++] = *s;
                }
                dst[n] = 0;
                if (!n)
                        return false;
        }
        /* "ECHO A > X" writes "A", not "A " */
        while (o > cmd && isspace((unsigned char)o[-1]) && !quote)
                o--;
        *o = 0;
        return true;
}

/* moves fd to target, keeping the old one in *saved */
static bool redirect_fd(const char *path, int flags, int target, int *saved) {
        int fd = open(path, flags, 0666);

        if (fd < 0) {
                fprintf(stderr, "%s: cannot open\n", path);
                return false;
        }
        *saved = dup(target);
        dup2(fd, target);
        close(fd);
        return true;
}

static void redirect_undo(int saved[2]) {
        int i;

        fflush(stdout);
        for (i = 0; i < 2; i++)
                if (saved[i] >= 0) {
                        dup2(saved[i], i);
                        close(saved[i]);
                }
}

/* one command with its redirections; in and out (pipe files) unless the
 * command names its own */
static int execute_redirected(char *cmd, const char *in, const char *out) {
        struct redirect r;
        int saved[2] = {-1, -1}, code, level;

        if (!redirect_parse(cmd, &r)) {
                fprintf(stderr, "Syntax error: a redirection without a file name\n");
                errno = 1;
                return EXIT_SUCCESS;
        }
        if (!*r.in && in)
                snprintf(r.in, sizeof(r.in), "%s", in);
        if (!*r.out && out) {
                snprintf(r.out, sizeof(r.out), "%s", out);
                r.append = false;
        }
        fflush(stdout);
        if ((*r.in && !redirect_fd(r.in, O_RDONLY, 0, &saved[0])) ||
            (*r.out && !redirect_fd(r.out, O_WRONLY | O_CREAT | (r.append ? O_APPEND : O_TRUNC), 1,
                                    &saved[1]))) {
                redirect_undo(saved);
                errno = 1;
                return EXIT_SUCCESS;
        }
        code = execute_command(cmd);
        level = errno;
        redirect_undo(saved);
        errno = level;
        return code;
}

/* a pipe is a file, as in DOS: in %TEMP%, %TMP% or the current directory */
static void pipe_name(unsigned n, char *path, size_t size) {
        const char *dir = getenv("TEMP");
        const char *sep = DIRECTORY_DELIMITER;

        if (!dir || !*dir)
                dir = getenv("TMP");
        if (!dir || !*dir)
                dir = "", sep = "";
        else if (strchr("\\/:", dir[strlen(dir) - 1]))
                sep = "";
        snprintf(path, size, "%s%sPIPE%u.$$$", dir, sep, n);
}

/* "a | b | c": each command's output is the next one's input */
int command_execute_line(const char *line) {
        char buf[1024], *stage[PIPE_STAGES], pipes[2][260], quote = 0, *s;
        unsigned n = 1, i;
        int code = EXIT_SUCCESS;

        snprintf(buf, sizeof(buf), "%s", line);
        stage[0] = buf;
        for (s = buf; *s; s++) {
                if (quote || *s == '"')
                        quote = quote ? (*s == quote ? 0 : quote) : *s;
                else if (*s == '|') {
                        if (n == PIPE_STAGES) {
                                fprintf(stderr, "Too many pipes\n");
                                errno = 1;
                                return EXIT_SUCCESS;
                        }
                        *s = 0;
                        stage[n++] = s + 1;
                }
        }
        for (i = 0; i < n; i++) {
                const char *in = i ? pipes[(i - 1) & 1] : NULL;
                char *out = i + 1 < n ? pipes[i & 1] : NULL;

                if (out)
                        pipe_name(i, out, sizeof(pipes[0]));
                code = execute_redirected(stage[i], in, out);
                if (in)
                        remove(in);
                if (code != EXIT_SUCCESS) {
                        if (out)
                                remove(out);
                        break;
                }
        }
        return code;
}
#else
int command_execute_line(const char *line) { return execute_command(line); }
#endif

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
                fflush(stdout); /* the prompt has no line end */
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
