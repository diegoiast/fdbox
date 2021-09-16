
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_POSIX_C_SOURCE) || defined(__DJGPP__) || defined(__APPLE__)
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#endif

#if defined (__TURBOC__)
#include <dir.h>
#endif

#if defined(__WATCOMC__)
#include <direct.h> 
#endif

#ifdef WIN32
// Windows needs this specific include order, and clang-format will
// try to re-order by ABC. Lets not.
// clang-format off
#include <direct.h>
#include <windef.h>
#include <winbase.h>
// clang-format on
#endif

#include "dos/cd.h"
#include "fdbox.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_cd(int argc, char *argv[]) {
        const char *path = argv[1];
        if (argc == 1) {
                char cwd[128];
#if defined(__linux__) || defined(__MSDOS__) || defined(__APPLE__) || defined(__WATCOMC__)
                getcwd(cwd, 128);
#elif defined(__WIN32__)
                DWORD err = GetCurrentDirectory(128, cwd);
#else
#error platform not supported yet
#endif
                printf("%s\n", cwd);
                return EXIT_SUCCESS;
        }
        if (argc != 2) {
                printf("cd can only have a single directory\n");
                return EXIT_FAILURE;
        }

#if defined(__linux__)
        // On linux - lets also support ~ for "cd ~" and "cd ~user"
        // I might do shell glob expansion as bash does... I see this will happen
        // a lot
        struct passwd *p = NULL;
        if (argv[1][0] == '~') {
                char *username = argv[1] + 1;
                if (*username == '\0') {
                        path = getenv("HOME");
                        if (path == NULL || path[0] == '\0') {
                                p = getpwuid(getuid());
                        }
                } else {
                        p = getpwnam(username);
                }
                if (path == NULL && p == NULL) {
                        return EXIT_FAILURE;
                }
                if (p != NULL)
                        path = p->pw_dir;
        }
#endif
        return chdir(path);
}

const char *help_cd() { return "Change working directory, pass none to display current directory"; }
