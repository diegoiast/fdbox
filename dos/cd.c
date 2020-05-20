#include <stdlib.h>
#include <stdio.h>

#if __linux__
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include "fdbox.h"
#include "cd.h"

// This file is part of fdbox
// For license - read license.txt

int command_cd(int argc, char* argv[]) {
    if (argc == 1)
    {
        char cwd[128];
#ifdef __linux__
           getcwd(cwd, 128);
#endif

#ifdef WIN32
        DWORD err = GetCurrentDirectory(128, cwd);
#endif

        printf("%s\n", cwd); 
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}

const char* help_cd() {
    return "Change working directory, pass none to display current directory";
}
