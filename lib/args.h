#ifndef __args_h__
#define __args_h__

/*
This file is part of fdbox
For license - read license.txt
*/

#include <stdlib.h>

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined (__WATCOMC__)
#include <stdbool.h>
#endif

#ifdef __WIN32__
#include <stdbool.h>
#endif

#if defined(__TURBOC__)
#include "lib/tc202/stdbool.h"
#endif

#define ARG_DONE -1
#define ARG_PROCESSED 0

#define ARG_MAX_FILES 128

void hexDump(char *desc, void *addr, int len);
void print_agrs(int argc, char *argv[]);
/* int dos_parseargs(int *argc, char **argv[], const char* template, char **output); */

struct command_glob {
        const char *file[ARG_MAX_FILES];
        size_t count;
        size_t overflow;
};

struct command_config {
        bool show_help;
        bool verbose;
        struct command_glob files;

        struct {
                int current_argument;
        } state;
};

void command_config_init(struct command_config *config);
int command_config_parse(int argc, char *argv[], struct command_config *config);
const char *command_config_next(int argc, char *argv[], struct command_config *config);
void command_config_print(const struct command_config *config);

bool command_split_args(char *full_cmd, size_t *argc, const char *argv[], size_t max_argv);
bool command_merge_args(size_t argc, const char *argv[], char *line, size_t max_line_size);

struct command_args {
        /* these two are the ones passwed to `main` like functions */
        size_t argc;
        char **argv;

        /* in practice - some argv, will be null. The real length of argv is not
         * argc, but max_args. The rest is uninitialized
         */
        size_t max_args;
};

void command_args_allocate(struct command_args *args, size_t count);
void command_args_free(struct command_args *args);
int command_args_split(const char *line, struct command_args *args);

#endif //__args_h__
