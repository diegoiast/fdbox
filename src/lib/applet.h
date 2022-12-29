#ifndef FDBOX_APPLETS
#define FDBOX_APPLETS

typedef int (*function_handler)(int, char *[]);
typedef const char *(*function_help)();

/* The only problematic platform is turbo C, all others are modern */
#if defined(__TURBOC__)
#include "lib/tc202/stdbool.h"
#else
#include <stdbool.h>
#endif

#define CASE_INSENSITIVE false
#define CASE_SENSITIVE true

struct applet {
        function_help help_text;
        function_handler handler;
        const char *name;
};

struct applet *find_applet(bool sensitive, const char *applet_name, struct applet commands[]);

#endif // FDBOX_APPLETS
