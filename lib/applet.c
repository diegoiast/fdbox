#include <stddef.h>
#include <string.h>

#include "lib/applet.h"

#if defined(__TURBOC__)
#include "lib/tc202/stdextra.h"
#endif

struct applet *find_applet(bool sensitive, const char *applet_name, struct applet commands[]) {
        struct applet *app = commands;
        if (applet_name == NULL) {
                return NULL;
        }
        while (app != NULL && app->name != NULL) {
                if (sensitive) {
                        if (strcmp(app->name, applet_name) == 0) {
                                return app;
                        }
                } else {
                        if (strcasecmp(app->name, applet_name) == 0) {
                                return app;
                        }
                }
                app++;
        }
        return NULL;
}
