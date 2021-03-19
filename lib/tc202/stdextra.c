#include "lib/tc202/stdextra.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int snprintf(char *str, size_t size, const char *format, ...) {
        int i;
        va_list argp;
        va_start(argp, format);
        i = vsprintf(str, format, argp);
        va_end(argp);
        return i;
}
