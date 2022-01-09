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

char *str_del_char(char *s, size_t index) {
        memmove(s + index, s + index + 1, strlen(s) - index);
        return s;
}

char *str_ins_char(char *s, size_t max_length, char c, size_t index) {
        size_t l = strlen(s);
        size_t d;
        if (index >= max_length) {
                return s;
        }
        if (index == max_length - 1) {
                s[index] = c;
                return s;
        }

        d = l - index + 1;
        memmove(s + index + 1, s + index, d);
        s[index] = c;
        return s;
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
const char *file_get_extension(const char *fname) {
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

void str_list_init(struct str_list *list, size_t length) {
        size_t i;
        list->length = length;
        list->next = 0;
        list->items = malloc(sizeof(char *) * length);
        for (i = 0; i < length; i++) {
                list->items[i] = NULL;
        }
}

void str_list_free(struct str_list *list) {
        size_t index = (list->next + list->length - 1) % list->length;
        while (list->items[index] != NULL) {
                free(list->items[index]);
                list->items[index] = NULL;
                index = (index + list->length - 1) % list->length;
        }
        free(list->items);
        list->items = 0;
        list->length = 0;
        list->next = 0;
}

void str_list_push(struct str_list *list, const char *s) {
        if (list->items[list->next] != NULL) {
                free(list->items[list->next]);
        }
        list->items[list->next] = strdup(s);
        list->next = (list->next + 1) % list->length;
}

char *str_list_pop(struct str_list *list) {
        size_t next = (list->next + list->length - 1) % list->length;
        char *c = list->items[next];
        if (c == NULL) {
                return c;
        }
        list->items[next] = NULL;
        list->next = next;
        return c;
}

const char *str_list_peek(struct str_list *list) {
        size_t next = (list->next + list->length - 1) % list->length;
        const char *c = list->items[next];
        return c;
}

const char *str_list_get(struct str_list *list, size_t n) {
        size_t index;
        if (n >= list->length) {
                return NULL;
        }
        index = (list->next + list->length - n - 1) % list->length;
        return list->items[index];
}
