/*
This file is part of fdbox
For license - read license.txt
*/

#include <string.h>
#include <stdlib.h>

#ifdef __MSDOS__
#include <dir.h>
#endif

#ifdef _POSIX_C_SOURCE
#include <unistd.h>
#endif

#ifdef __WIN32__
#include <unistd.h>
#endif

#include "lib/strextra.h"
#include "lib/environ.h"

char *get_prompt(const char *prompt, char prompt_string[], size_t prompt_str_len) {
        int i = 0, j = 0;
        char *p;

        while (prompt[i]) {
                switch (prompt[i]) {
                case '$':
                        i++;
                        switch (prompt[i]) {
                        case 'b':
                                prompt_string[j] = '|';
                                j++;
                                i++;
                                break;
                        case 'c':
                                prompt_string[j] = ')';
                                j++;
                                i++;
                                break;
                        case 'g':
                        case 'G':
                                prompt_string[j] = '>';
                                j++;
                                i++;
                                break;
                        case 'p':
                        case 'P':
                                p = (char *)malloc(128);
                                getcwd(p, 128);
/* On Windows and Unix, there are mixed case file systems
 * We will show eaxctly whats given to us
 * On MSDOS (well, also FreeDOS...) the OS will give us upper case
 * dir names always, so we have the option to play
 */
#ifndef __MSDOS__
                                if (prompt[i] == 'p') {
                                        str_to_lower( p );
                                }
#endif
                                prompt_string[j] = 0;
                                strncat(prompt_string, p, prompt_str_len);
                                j += strlen(p);
                                i++;
                                free(p);
                                break;
                        default: /*j++;*/
                                i++;
                                break;
                        }
                        break;
                default:
                        prompt_string[j] = prompt[i];
                        i++;
                        j++;
                        break;
                }
        }

        prompt_string[j] = 0;
        return prompt_string;
}
