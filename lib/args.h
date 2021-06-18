#ifndef __args_h__
#define __args_h__

/*
This file is part of fdbox
For license - read license.txt
*/

#include <stdlib.h>

#ifdef _POSIX_C_SOURCE
#include <stdbool.h>
#endif

#ifdef __WIN32__
#include <stdbool.h>
#endif

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#endif

#define ARG_DONE -1
#define ARG_PROCESSED 0

#define ARG_NOT_EXISTING 1000
#define ARG_EXTRA 1001
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

#endif //__args_h__
