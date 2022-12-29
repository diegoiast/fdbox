#include "lib/tc202/stdextra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__TURBOC__)
#include "lib/tc202/stdbool.h"
#elif defined(__linux__) || defined(__WIN32__)
#include <stdbool.h>
#endif

#if defined(__TURBOC__)
int snprintf(char *str, size_t size, const char *format, ...) {
        int i;
        va_list argp;
        UNUSED(size);
        va_start(argp, format);
        i = vsprintf(str, format, argp);
        va_end(argp);
        return i;
}
#endif

bool save_env_locally(const char *name, char *value);

#define SAVED_ENV_LENGTH 20
char *saved_env[SAVED_ENV_LENGTH] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

int setenv_impl(const char *name, const char *value, int overwrite) {
        char *c;
        char *var;
        int length;

        c = getenv(name);

        if (!overwrite && c != NULL) {
                return 0;
        }

        length = strlen(name) + strlen(value) + 2;
        var = (char *)malloc(length);
        snprintf(var, length, "%s=%s", name, value);
        if (!save_env_locally(name, var)) {
                free(var);
                return EXIT_FAILURE;
        }
        return putenv(var);
}

void clear_env() {
        int i;
        for (i = 0; i < SAVED_ENV_LENGTH; i++) {
                free(saved_env[i]);
                saved_env[i] = NULL;
        }
}

bool save_env_locally(const char *name, char *value) {
        int i;
        int l;
        int first_available_slot = -1;
        int found_slot = -1;

        l = strlen(name);
        for (i = 0; i < SAVED_ENV_LENGTH; i++) {
                char *pp;
                if (first_available_slot == -1 && saved_env[i] == NULL) {
                        first_available_slot = i;
                }
                if (saved_env[i] == NULL) {
                        continue;
                }
                pp = strstr(saved_env[i], name);
                if (pp == NULL) {
                        continue;
                }
                if (saved_env[i][l] == '=') {
                        found_slot = i;
                        break;
                }
        }

        if (found_slot >= 0) {
                free(saved_env[found_slot]);
                first_available_slot = found_slot;
        }

        if (first_available_slot < 0) {
                return false;
        }

        saved_env[first_available_slot] = value;
        return true;
}
