/*
This file is part of fdbox
For license - read license.txt
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fdbox.h"

#ifdef __MSDOS__
#include <dir.h>
#endif

#if defined(_POSIX_C_SOURCE) || defined(__WIN32__) || defined(__DJGPP__)
#include <unistd.h>
#endif

#ifdef __TURBOC__
#include "lib/tc202/stdextra.h"
#endif

#include "lib/environ.h"
#include "lib/strextra.h"

/*
 * https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/prompt
 * https://www.computerhope.com/prompthl.htm
 * http://wiki.freedos.org/wiki/index.php/Prompt
 */

char *get_prompt(const char *prompt, char prompt_string[], size_t prompt_str_len) {
        while (*prompt) {
                switch (*prompt) {
                case '$':
                        prompt++;
                        switch (tolower(*prompt)) {
                        case '$':
                                *prompt_string = '$';
                                break;
                        case '_':
                                *prompt_string = '\n';
                                break;
                        case 'a':
                                *prompt_string = '&';
                                break;
                        case 'b':
                                *prompt_string = '|';
                                break;
                        case 'c':
                                *prompt_string = '(';
                                break;
                        case 'd': {
                                time_t t = time(NULL);
                                struct tm tm = *localtime(&t);
                                char date_str[25];
                                int k;

                                k = snprintf(date_str, 25, "%02d-%02d-%4d", tm.tm_mday,
                                             tm.tm_mon + 1, tm.tm_year + 1900);
                                *prompt_string = 0;
                                strncat(prompt_string, date_str, prompt_str_len);
                                prompt_string += k - 1;
                                break;
                        }
                        case 'e':
                                *prompt_string = 27;
                                break;
                        case 'f':
                                *prompt_string = ')';
                                break;
                        case 'g':
                                *prompt_string = '>';
                                break;
                        case 'h':
                                /* back space - delete one char to the left */
                                *prompt_string = 8;
                                break;
                        case 'l':
                                *prompt_string = '<';
                                break;
                        case 'm':
                                /* remote drive - to be implemented under windows only */
                                *prompt_string = ' ';
                                prompt--;
                                break;
                        case 'n':
                                /* current drive - on unix its ignored */
                                /* GetCurrentDirectory() on win32
                                 * On DOS?
                                 */
                                *prompt_string = 'C';
                                prompt--;
                                break;
                        case 'p': {
                                char *cwd = (char *)malloc(128);
                                getcwd(cwd, 128);
/* On Windows and Unix, there are mixed case file systems
 * We will show eaxctly whats given to us
 * On MSDOS (well, also FreeDOS...) the OS will give us upper case
 * dir names always, so we have the option to play
 */
#ifndef __MSDOS__
                                if (*prompt == 'p') {
                                        str_to_lower(cwd);
                                }
#endif
                                *prompt_string = 0;
                                strncat(prompt_string, cwd, prompt_str_len);
                                prompt_string += strlen(cwd) - 1;
                                free(cwd);
                                break;
                        }
                        case 'q':
                                *prompt_string = '=';
                                break;
                        case 's':
                                *prompt_string = ' ';
                                break;
                        case 't': {
                                time_t t = time(NULL);
                                struct tm tm = *localtime(&t);
                                char time_str[25];
                                int k;
                                k = snprintf(time_str, 25, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min,
                                             tm.tm_sec);
                                *prompt_string = 0;
                                strncat(prompt_string, time_str, prompt_str_len);
                                prompt_string += k - 1;
                                break;
                        case 'v':
                                *prompt_string = 0;
                                strncat(prompt_string, FDBOX_VERSION_STR, prompt_str_len);
                                prompt_string += strlen(FDBOX_VERSION_STR) - 1;
                                break;
                        }
                        default:
                                break;
                        }
                        break;
                default:
                        *prompt_string = *prompt;
                        break;
                }
                prompt++;
                prompt_string++;
        }

        *prompt_string = 0;
        return prompt_string;
}

void expand_string(const char *src, char *dest, size_t dest_length) {
        char var_name[100], *var_p = NULL;
        char *var_value;
        size_t var_len;
        char *dest_p = dest;
        size_t dest_written = 0;

        *dest_p = 0;
        while (*src != 0 && dest_written < dest_length) {
                switch (*src) {
                case '%':
                        if (var_p == NULL) {
                                /* we start a new variable */
                                var_p = var_name;
                                *var_p = 0;
                                var_len = 0;
                        } else {
                                /* we end a variable */
                                *var_p = 0;
                                if (var_name[0] != '\0') {
                                        var_value = getenv(var_name);
                                        if (var_value != NULL) {
                                                var_len = strlen(var_value);
                                                /* we stop processing if we don't have more space */
                                                if (dest_written + var_len >= dest_length) {
                                                        return;
                                                }
                                                strcat(dest_p, var_value);
                                                dest_p += var_len;
                                                dest_written += var_len;
                                        }
                                }
                                var_p = 0;
                        }
                        break;
                default:
                        if (var_p == NULL) {
                                *dest_p = *src;
                                dest_p++;
                                dest_written += 1;
                                *dest_p = 0;
                        } else {
                                *var_p = *src;
                                var_p++;
                        }
                }
                src++;
        }
        *dest_p = 0;
}
