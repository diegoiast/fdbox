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
        if (argc == 1) {
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

        default:
                /* ok its a file */
                if (config->files.count == ARG_MAX_FILES) {
                        config->files.overflow++;
                        return ARG_PROCESSED;
                }
                config->files.file[config->files.count] = arg;
                config->files.count++;
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

void command_config_print(const struct command_config *config) {
        size_t i;

        printf("VERBOSE = %d\n", config->verbose);
        printf("SHOW HELP = %d\n", config->show_help);
        printf("Glob %zu requested, %zu overflow\n", config->files.count + config->files.overflow,
               config->files.overflow);
        for (i = 0; i < config->files.count; i++) {
                printf(" -> %s\n", config->files.file[i]);
        }
}

/*
 We could also use this command and get empty arguments. We are not
 interested in that.
 token = strsep(&full_cmd, white_space);
 */
bool command_split_args(char *full_cmd, size_t *argc, const char *argv[], size_t max_argv) {
        char *token;
        const char *white_space = " \t";
        *argc = 0;
        token = strtok(full_cmd, white_space);
        while (token != NULL) {
                argv[*argc] = token;
                (*argc)++;
                if (*argc == max_argv) {
                        return false;
                }
                token = strtok(NULL, white_space);
        }
        return true;
}

bool command_merge_args(size_t argc, const char *argv[], char *line, size_t max_line_size) {
        size_t i = 0, l = 0;

        line[0] = 0;
        for (i = 0; i < argc; i++) {
                size_t k = strlen(argv[i]);
                if (l + k >= max_line_size) {
                        return false;
                }
                strncat(line, argv[i], max_line_size);
                strncat(line, " ", max_line_size);
                l += k;
        };
        return true;
}

/* new argument parsing API */
enum parsing_state { appending, scanning, escaping, equals };

void command_args_allocate(struct command_args *args, size_t count) {
        size_t i = args->argc;
        args->argv = realloc(args->argv, sizeof(const char *) * count);
        while (i < count) {
                args->argv[i] = NULL;
                i++;
        }
        args->max_args = count;
}

void command_args_free(struct command_args *args) {
        size_t i;
        for (i = 0; i < args->argc; i++) {
                free(args->argv[i]);
        }
        free(args->argv);
        args->argc = 0;
        args->argv = NULL;
        args->max_args = 0;
}

/* More advanced arguments split. Supports
 *  - quoting
 *  - non-srparated agrs (WIP)
 *  - does not nodify original string (allocates)
 */
int command_args_split(const char *line, struct command_args *args) {
        char word[256], *p_word = word;
        size_t wordlen = 0;
        const char *c = line;
        char bracket = 0;
        enum parsing_state state = scanning;

        memset(args, 0, sizeof(struct command_args));
        command_args_allocate(args, 8);
        word[0] = '\0';
        while (*c != 0) {
                bool copy_word = false;
                bool copy_char = false;
                bool new_word = false;

                if (isspace(*c)) {
                        /* do not copy empty words */
                        if (state == appending) {
                                if (bracket != 0) {
                                        copy_char = true;
                                } else {
                                        copy_word = true;
                                }
                                if (word[0] == '\0') {
                                        copy_word = true;
                                }
                        }
                } else if (*c == '\'' || *c == '"') {
                        if (state == scanning) {
                                state = appending;
                                bracket = *c;
                        } else if (state == appending) {
                                /* non-matching bracket is copied */
                                if (*c == bracket) {
                                        copy_char = false;
                                        bracket = 0;
                                } else {
                                        copy_char = true;
                                }
                        }
                } else if (*c == '=') {
                        if (state == appending) {
                                copy_word = true;
                                new_word = true;
                                state = equals;
                        } else {
                                copy_char = true;
                        }
                } else {
                        if (state == equals) {
                                new_word = true;
                                copy_word = true;
                                state = appending;
                        } else {
                                copy_char = true;
                                state = appending;
                        }
                }
                if (copy_char) {
                        if (wordlen < 256) {
                                *p_word = *c;
                                p_word++;
                                *p_word = '\0';
                                wordlen++;
                        }
                }
                if (copy_word) {
                        command_args_allocate(args, args->max_args + 5);
                        args->argv[args->argc] = strdup(word);
                        args->argc++;
                        word[0] = '\0';
                        p_word = word;
                        *p_word = '\0';
                        wordlen = 0;
                        bracket = 0;
                        if (state != equals) {
                                state = scanning;
                        }
                }
                if (new_word) {
                        if (wordlen < 256) {
                                *p_word = *c;
                                p_word++;
                                *p_word = '\0';
                                wordlen++;
                        }
                }
                c++;
        }

        command_args_allocate(args, args->max_args + 5);
        if (word[0] != '\0') {
                args->argv[args->argc] = strdup(word);
                args->argc++;
        }
        return EXIT_SUCCESS;
}
