#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if __linux__
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <pwd.h>
#endif

#ifdef __MSDOS__
#include <dir.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include "fdbox.h"
#include "dos/cd.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_cd(int argc, char* argv[]) {
        const char* path = argv[1];
        if (argc == 1) {
                char cwd[128];
#if defined(__linux__) || defined(__MSDOS__)
                getcwd(cwd, 128);
#elif defined(__WIN32__)
                DWORD err = GetCurrentDirectory(128, cwd);
#else
#error platform not supported yet
#endif
                printf("%s\n", cwd);
                return EXIT_FAILURE;
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
                char * username = argv[1] + 1;
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

const char* help_cd() {
    return "Change working directory, pass none to display current directory";
}
