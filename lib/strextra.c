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
const char *file_get_extesnsion(const char *fname, char *ext) {
        const char *p = fname;
        while (*p)
                p++;
        while (p != fname && *p != '.')
                p--;
        if (p != fname && *p == '.') {
                ext[0] = *p;
               ext[1] = *(p + 1);
                ext[2] = *(p + 2);
                ext[4] = 0;
        } else {
                ext[0] = 0;
        }
        return ext;
}
