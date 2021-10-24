/*
This file is part of fdbox
For license - read license.txt
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/command.h"
#include "dos/if.h"
#include "fdbox.h"
#include "lib/environ.h"
#include "lib/strextra.h"

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__)
#include <glob.h>
#include <stdbool.h>
#include <sys/stat.h>
#endif

#ifdef __WIN32__
#include "lib/win32/dirent.h"
#include "lib/win32/win32-glob.h"
#include <stdbool.h>
#endif

#if defined(__TURBOC__)
#include "lib/tc202/dos-glob.h"
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#include <sys/stat.h>
#endif

#if defined(__WATCOMC__)
#include "lib/tc202/dos-glob.h"
#include <strings.h>
#include <sys/stat.h>
#endif

static bool if_file_exists(const char *path);
static char *find_if_command(int argc, char *argv[]);
static char *find_else_command(int argc, char *argv[]);

int command_if(int argc, char *argv[]) {
        bool reverse_token = false;
        bool evaluated_value;
        char *command_to_execute;
        int arg_index = 1;
        int rc = EXIT_FAILURE;
        int save_errno = errno;

        if (strcasecmp(argv[arg_index], "not") == 0) {
                reverse_token = true;
                arg_index++;
        }

        if (strcasecmp(argv[arg_index], "exist") == 0) {
                arg_index++;
                evaluated_value = if_file_exists(argv[arg_index]);
        } else if (strcasecmp(argv[arg_index], "ERRORLEVEL") == 0) {
                arg_index++;
                if (argv[arg_index] != NULL && arg_index < argc) {
                        int error_test = 0;
                        error_test = strtol(argv[arg_index], NULL, 10);
                        evaluated_value = error_test == save_errno;
                } else {
                        /* we could not even parse the error level, this is bad */
                        evaluated_value = false;
                        reverse_token = false;
                }

        } else {
                /* this is a comperation, command extensions are not suported yet */
                /* see
                 * https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/if
                 */
                /* HACK:
                 * for now we support a simple scenario:
                 * `if %PATH% == "c:\" echo ok else echo fail`
                 * Note how we need a separator between  the "==".
                 * We need to fix `command_split_args()` to be more compatible with MSDOS
                 * arguments separation.
                 */

                if (argc - arg_index > 3 && strcmp(argv[arg_index + 1], "==") == 0) {
                        char *arg1 = argv[arg_index];
                        char *arg2 = argv[arg_index + 2];
                        char exp1[128];
                        char exp2[128];

                        expand_string(arg1, exp1, 128);
                        expand_string(arg2, exp2, 128);

                        arg_index += 2;

                        evaluated_value = strncmp(exp1, exp2, 128) == 0;
                } else {
                        /* this is not a valid assignment, let it fail randomly */
                        evaluated_value = false;
                        reverse_token = false;
                }
        }

        evaluated_value = reverse_token ? !evaluated_value : evaluated_value;

        if (evaluated_value) {
                command_to_execute = find_if_command(argc - arg_index, argv + arg_index);
        } else {
                command_to_execute = find_else_command(argc - arg_index, argv + arg_index);
        }
        if (command_to_execute != NULL) {
                rc = command_execute_line(command_to_execute);
                free(command_to_execute);
        }
        return rc;
}

const char *help_if() { return "Conditionally execute a command"; }

/* this implementation also returns true for directories, is this OK? */
static bool if_file_exists(const char *path) {
        struct stat path_stat;
        int rc = stat(path, &path_stat);
        return rc == 0;
}

/* This function concatinates all arguments for the
 * positive resolution of "if".
 * The input is the first token to be added
 *
 * if [condition] [ok_statements] else [fail_statments]
 *
 * it will return [ok_statements]
 */
static char *find_if_command(int argc, char *argv[]) {
        char *c = strdup("");
        int l = 0;

        while (argc > 1) {
                if (argv[1] != NULL) {
                        if (strcasecmp(argv[1], "else") == 0) {
                                break;
                        }
                        c = realloc(c, l + strlen(argv[1]) + 2);
                        strcat(c, argv[1]);
                        strcat(c, " ");
                        l = strlen(c);
                }
                argv++;
                argc--;
        }

        return c;
}

/* This function concatinates all arguments for the
 * positive resolution of "if".
 * The input is the first token to be added
 *
 * if [condition] [ok_statements] else [fail_statments]
 *
 * it will return [fail_statments]
 */
static char *find_else_command(int argc, char *argv[]) {
        char *c;
        int l = 0;

        while (argc != 0) {
                if (argv[1] != NULL) {
                        if (strcasecmp(argv[1], "else") == 0) {
                                break;
                        }
                }
                argv++;
                argc--;
        }
        /* now else */
        if (argc == 0) {
                return NULL;
        }

        /* are we at the "else" ?*/
        /* TODO - should we skip nulls? */
        if (strcasecmp(argv[1], "else") != 0) {
                return NULL;
        }
        argv++;
        argc--;

        c = strdup("");
        while (argc > 1) {
                if (argv[1] != NULL) {
                        c = realloc(c, l + strlen(argv[1]) + 2);
                        strcat(c, argv[1]);
                        strcat(c, " ");
                        l = strlen(c);
                }
                argv++;
                argc--;
        }

        return c;
}
