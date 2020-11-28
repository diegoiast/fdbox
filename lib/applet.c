#include <string.h>
#include <stddef.h>
#include "lib/applet.h"

struct applet* find_applet(const char* applet_name, struct applet commands[]) {
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
