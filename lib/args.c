#include <stdlib.h>
#include <stdio.h>

#include "fdbox.h"
#include "args.h"

// This file is part of fdbox
// For license - read license.txt


int dos_parseargs(int *argc, char **argv[], const char* template, char **output) {
    
//     printf("Checking for argc=%d\n", *argc);
    char *c = ((*argv)[*argc]);
    const char *t = template;
    
    if (c == NULL) {
//         printf("No argument = -1\n");
        return ARG_DONE;
    }
//     DEBUG_LINE;
    while (*c  != '\0') {
//         DEBUG_LINE;
         // if this is not a command line argument, this is a filename
        if (*c != '/') {
            (*output) = c;
            (*argv)++;
//             printf("Found filename =%s\n", c);
            return ARG_STRING;
        }
        
        c++;
        while (*t != '\0') {
//             printf("t=%c, c=%c ?\n", *t, *c);
            if (*t == '[') {
                while (*t != '\0' && *t != ']') {
                    t++;
//                     printf(" - t=%c, c=%c ?\n", *t, *c);
                }
            }
            
            // not matching our template
            if (*t != *c) {
                t++;
                continue;
            }
            
            // is this a combo? does it need something to follow?
            t++;
            if (*t != '[' || *t == '\0') {
                (*argv)++;
                (*output) = NULL;
                
                if (*(c+1) != '\0') {
                    // there is something else - not good                    
//                     printf("Found simple argument %c->%c (BAD)\n", *c, *(c+1));
//                     printf("argv=%s\n", (*argv)[(*argc-1)]);
                    (*output) = (*argv)[(*argc-1)];
                    return ARG_EXTRA;
                }
//                 printf("Found simple argument %c->%c (OK)\n", *c, *(c+1));
                return *c;
            }
        }
        c++;
    }
//         printf("Arg not found\n");
    (*argc)++;
    (*output) = (*argv)[(*argc-1)];
    return ARG_NOT_EXISTING;
}
