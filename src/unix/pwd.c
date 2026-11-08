#include <stdio.h>
#include <stdlib.h>

#if defined(__GONWARE__) || defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#endif

#if defined(__TURBOC__)
#include <dir.h>
#endif

#if defined(__WATCOMC__)
#include <direct.h>
#endif

#ifdef __WIN32__
/* clang-format off */
#include <direct.h>
#include <windef.h>
#include <winbase.h>
/* clang-format on */
#endif

#include "fdbox.h"
#include "lib/nls.h"
#include "unix/pwd.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_pwd(int argc, char *argv[]) {
        char cwd[260];
#if defined(__WIN32__)
        if (GetCurrentDirectory(sizeof(cwd), cwd) == 0) {
                return EXIT_FAILURE;
        }
#else
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
                return EXIT_FAILURE;
        }
#endif
        printf("%s\n", cwd);
        UNUSED(argc);
        UNUSED(argv);
        return EXIT_SUCCESS;
}

const char *help_pwd() { return _("Print the current working directory"); }
