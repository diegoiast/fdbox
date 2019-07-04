#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib/args.h"
#include "fdbox.h"
#include "help.h"

extern struct applet commands[];

struct applet* find_applet(const char* applet_name);

int main(int argc, char *argv[])
{
    struct applet *cmd = find_applet(argv[1]);
    if (cmd != NULL) {
        // shift argument list left, now applet name is argv[0]
        return cmd->handler(argc - 1, ++argv);
    }
    command_help(argc, argv);
    return EXIT_FAILURE;
}

struct applet* find_applet(const char* applet_name) {
    struct applet *app = commands;
    if (applet_name == NULL) {
        return NULL;
    }
    while (app != NULL && app->name != NULL) {
        if (strcmp(app->name, applet_name) == 0) {
            return app;
        }
        app ++;
    }
    return NULL;
}
