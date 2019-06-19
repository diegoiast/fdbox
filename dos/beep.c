#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32
// https://docs.microsoft.com/en-us/windows/desktop/api/utilapiset/nf-utilapiset-beep
#include <utilapiset.h>
#endif

#include "fdbox.h"
#include "beep.h"

// This file is part of fdbox
// For license - read license.txt

void debug_args(int argc, char* argv[]);

const char* next(int argc, char* argv[], int current);

int command_beep(int argc, char* argv[]) {
    const char *v;
    int frequency = 0;
    int lenght = 0;

    // debug_args(argc, argv);
    for (int i=1; i<argc; i++) {
        // I have no idea why this could happen.
        if (argv[i] == NULL) {
            break;
        }
        int l = strlen(argv[i]);
        if (argv[i][0] == '/') {
            if (l != 2) {
                printf("Missing argument /? \n");
                return EXIT_FAILURE;
            }
            char parameter = argv[i][1];
            switch (parameter) {
            case 't':
                v = next(argc, argv, i);
                if (v == NULL) {
                    printf("Missing argument /t - time\n");
                    return EXIT_FAILURE;
                }
                lenght = strtol(v, NULL, 0);
                if (errno != 0) {
                    printf("Invalid time argument: %s\n", v);
                    return EXIT_FAILURE;
                }
                break;
            case 'f':
                v = next(argc, argv, i);
                if (v == NULL) {
                    printf("Missing argument /f - frequency\n");
                    return EXIT_FAILURE;
                }
                if (errno != 0) {
                    printf("Invalid frequency argument: %s\n", v);
                    return EXIT_FAILURE;
                }
                frequency = strtol(v, NULL, 0);
                break;
            default:
                printf("No supported argument /%c\n", parameter);
                return EXIT_FAILURE;
            }
        }
    }

// printf("f=%d, t=%d\n", frequency, lenght);
#if defined(UNIX) || defined(__APPLE__)
    printf ("\a");
    UNUSED(frequency);
    UNUSED(lenght);
#endif
#ifdef WIN32
    Beep(frequency, lenght);
#endif
#ifdef DOS
    sound(frequency);
    delay(length);
    nosound();
#endif
    return EXIT_SUCCESS;
}

const char* help_beep() {
#if defined(UNIX) || defined(__APPLE__)
    return "Sounds a unix bell: '\\a' (all arguments are ignored for compatibility with DOS)";
#endif
#ifdef WIN32
    return "Makes sound for a specified length in milliseconds";
#endif
#ifdef DOS
    return "Makes sound for a specified length in milliseconds";
#endif
}

void debug_args(int argc, char* argv[]) {
    printf("argc = %d\n", argc);
    for(int i=0; i<argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}

const char* next(int argc, char* argv[], int current) {
    if (current >= argc) {
        return NULL;
    }
    return argv[current+1];
}
