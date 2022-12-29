#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
// https://docs.microsoft.com/en-us/windows/desktop/api/utilapiset/nf-utilapiset-beep
#include <utilapiset.h>
#endif

#include "dos/beep.h"
#include "fdbox.h"
#include "lib/args.h"

/*
This file is part of fdbox
For license - read license.txt
*/

int command_beep(int argc, char *argv[]) {
        struct command_config config;
        int frequency = -1;
        int length = -1;
        int c1, c2;
        const char *argument;

        command_config_init(&config);
        do {
                c1 = command_config_parse(argc, argv, &config);
                c2 = tolower(c1);
                switch (c2) {
                case 'f':
                        argument = command_config_next(argc, argv, &config);
                        if (argument == NULL) {
                                printf("Missing argument /f - frequency\n");
                                return EXIT_FAILURE;
                        }
                        frequency = strtol(argument, NULL, 0);
                        if (errno != 0) {
                                printf("Invalid time argument: %s\n", argument);
                                return EXIT_FAILURE;
                        }
                        break;
                case 'l':
                        argument = command_config_next(argc, argv, &config);
                        if (argument == NULL) {
                                printf("Missing argument /l - length\n");
                                return EXIT_FAILURE;
                        }
                        length = strtol(argument, NULL, 0);
                        if (errno != 0) {
                                printf("Invalid time argument: %s\n", argument);
                                return EXIT_FAILURE;
                        }
                        break;
                case ARG_PROCESSED:
                        break;
                case ARG_DONE:
                        break;
                default:
                        printf("No supported argument /%c\n", c2);
                        return EXIT_FAILURE;
                }
        } while (c1 >= 0);

        if (config.files.count != 0) {
                printf("Invalid argument: %s\n", config.files.file[0]);
                return EXIT_FAILURE;
        }

        if (config.show_help) {
                printf("%s\n", help_beep());
                printf("\n");
                printf("    beep /f [frequency] /t [time/milliseconds]\n");
                printf("\n  \f requecny in hertz");
                printf("\n  \t length of the beep in milliseconds");
                return EXIT_SUCCESS;
        }
#if defined(__unix__) || defined(__APPLE__) || defined(__DJGPP__)
        printf("\a");
        UNUSED(frequency);
        UNUSED(length);
#elif defined(WIN32)
        Beep(frequency, length);
#elif defined(__MSDOS__)
        sound(frequency);
        delay(length);
        nosound();
#endif
        return EXIT_SUCCESS;
}

const char *help_beep() {
#if defined(__unix__) || defined(__APPLE__)
        return "Sounds a unix bell: '\\a' (all arguments are ignored for compatibility with DOS)";
#elif defined(WIN32) || defined(__MSDOS__) || defined(__WATCOMC__)
        return "Makes sound for a specified length in milliseconds";
#endif
}
