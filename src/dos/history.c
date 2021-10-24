#include "dos/history.h"
#include "fdbox.h"
#include "lib/readline.h"
#include <stdio.h>
#include <stdlib.h>

int command_history(int argc, char *argv[]) {
        int n = 0;
        const char *h;
        UNUSED(argc);
        UNUSED(argv);

        h = readline_get_history(n);
        while (h != NULL) {
                printf("%d %s\n", n + 1, h);
                n++;
                h = readline_get_history(n);
        }
        return EXIT_SUCCESS;
}

const char *help_history() { return "Displays all readline history available"; }
