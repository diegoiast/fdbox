#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "fdbox.h"
#include "lib/args.h"

/*
This file is part of fdbox
For license - read license.txt
*/

/* https://stackoverflow.com/a/7776146 */
void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    /* Output description if given. */
    if (desc != NULL) {
        printf ("%s: %p\n", desc, addr);
    }
    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    /* Process every byte in the data. */
    for (i = 0; i < len; i++) {
        /* Multiple of 16 means new line (with line offset). */

        if ((i % 16) == 0) {
            /* Just don't print ASCII for the zeroth line. */
            if (i != 0)
                printf ("  %s\n", buff);

            /* Output the offset. */
            printf ("  %04x ", i);
        }

        /* Now the hex code for the specific character. */
        printf (" %02x", pc[i]);

        /* And store a printable ASCII character for later */
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    /* Pad out last line if not exactly 16 characters. */
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    /* And print the final ASCII bit. */
    printf ("  %s\n", buff);
}


void print_agrs(int argc, char *argv[]) {
    int i;
    if (argc == 0) {
        printf("No args\n");
        return;
    }
    if (argv ==NULL) {
        printf("argv = NULL\n");
        return;
    }
    printf("argv =%p\n", argv);
    for (i=0; i<argc; i++) {
        printf("%d: '%s', ", i, argv[i]);
    }
}

int dos_parseargs(int *argc, char **argv[], const char* template, char **output) {
    printf("Checking for argc=%d\n", *argc);
    print_agrs(*argc, *argv);
    char *c = ((*argv)[*argc]);
    const char *t = template;
    
    if (c == NULL) {
/*         printf("No argument = -1\n"); */
        return ARG_DONE;
    }
    while (*c  != '\0') {
        DEBUG_LINE;
         /* if this is not a command line argument, this is a filename */
        if (*c != '/') {
            (*output) = c;
            (*argv)++;
            printf("Found filename =%s\n", c);
            return ARG_STRING;
        }
        DEBUG_LINE;
        
        c++;
        while (*t != '\0') {
            printf("t=%c, c=%c ?\n", *t, *c);
            if (*t == '[') {
                while (*t != '\0' && *t != ']') {
                    t++;
/*                     printf(" - t=%c, c=%c ?\n", *t, *c); */
                }
            }
            
            // not matching our template
            if (*t != *c) {
                t++;
                continue;
            }
            
            /* is this a combo? does it need something to follow? */
            t++;
            if (*t != '[' || *t == '\0') {
                (*argv)++;
                (*output) = NULL;
                
                if (*(c+1) != '\0') {
                    /* there is something else - not good */
                    printf("Found simple argument %c->%c (BAD)\n", *c, *(c+1));
                    (*output) = (*argv)[(*argc-1)];
                    return ARG_EXTRA;
                }
                printf("Found simple argument %c->%c (OK)\n", *c, *(c+1));
                return *c;
            }
        }
        c++;
    }
        printf("Arg not found\n");
    (*argc)++;
    (*output) = (*argv)[(*argc-1)];
    return ARG_NOT_EXISTING;
}
