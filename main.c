#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib/args.h"
#include "lib/applet.h"
#include "fdbox.h"
#include "help.h"

extern struct applet commands[];

int main(int argc, char *argv[])
{
    struct applet *cmd = find_applet(argv[1], commands);
    if (cmd != NULL) {
        // shift argument list left, now applet name is argv[0]
        return cmd->handler(argc - 1, ++argv);
    }
    command_help(argc, argv);
    return EXIT_FAILURE;
}

