#include "lib/strextra.h"
#include <ctype.h>
#include <string.h>

const char *str_bool(bool b) { return b ? "true" : "false"; }

char *str_to_lower(char *s) {
        char *s1 = s;
        while (*s) {
                *s = tolower(*s);
                s++;
        }
        return s1;
}

bool str_is_prefix(const char *str, const char *pre) { return strncmp(pre, str, strlen(pre)) == 0; }

bool str_ends_with(const char *str, char c) {
        char *cc = strrchr(str, c);
        if (cc == NULL) {
                return false;
        }
        if (*(cc + 1) != '\0') {
                return false;
        }
        return true;
}

const char *file_base_name(const char *file_name) {
        int i = strlen(file_name);
        const char *c = file_name + i - 1;
        while (c != file_name && *c != '/' && *c != '\\') {
                c--;
        }

        if (*c == '/' || *c == '\\') {
                c++;
        }
        return c;
}

/* lets assume extensions are 3 letters only for now */
const char *file_get_extesnsion(const char *fname) {
        const char *p = fname;
        while (*p) {
                p++;
        }
        while (p != fname && *p != '.') {
                p--;
        }
        if (*p == '.') {
                p++;
        }
        return p;
}

#if defined(__WIN32__)
/* happily borrowed from https://stackoverflow.com/a/8514474 */
char *strsep(char **stringp, const char *delim) {
        char *start = *stringp;
        char *p;

        p = (start != NULL) ? strpbrk(start, delim) : NULL;

        if (p == NULL) {
                *stringp = NULL;
        } else {
                *p = '\0';
                *stringp = p + 1;
        }

        return start;
}
#endif
