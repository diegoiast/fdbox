#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dos/command.h"
#include "dos/for.h"
#include "fdbox.h"

#if defined(__TURBOC__)
#include "lib/tc202/dos-glob.h"
#elif defined(__WIN32__)
#include "lib/win32/win32-glob.h"
#else
#include <glob.h>
#endif

/*
This file is part of fdbox
For license - read license.txt

FOR %V IN (set) DO command

The set holds names and wildcards (separated by spaces, commas or
semicolons); a wildcard stands for the matching files. The command runs
once per name with %V replaced by it (in a batch file %%V). The error
level is the last command's.
*/

static const char *skip_space(const char *s) {
        while (isspace((unsigned char)*s))
                s++;
        return s;
}

/* the next word of s into word; the rest */
static const char *next_word(const char *s, char *word, size_t size) {
        size_t n = 0;

        s = skip_space(s);
        while (*s && !isspace((unsigned char)*s) && *s != '(') {
                if (n + 1 < size)
                        word[n++] = *s;
                s++;
        }
        word[n] = 0;
        return s;
}

/* the command with each var replaced by item, then run */
static int run_one(const char *var, const char *command, const char *item) {
        char line[1024];
        size_t o = 0, vlen = strlen(var), ilen = strlen(item);
        const char *c = command;

        while (*c && o + 1 < sizeof(line)) {
                if (strncasecmp(c, var, vlen) == 0) {
                        if (o + ilen + 1 >= sizeof(line))
                                break;
                        memcpy(line + o, item, ilen);
                        o += ilen;
                        c += vlen;
                } else
                        line[o++] = *c++;
        }
        line[o] = 0;
        return command_execute_line(line);
}

int for_line(const char *line) {
        char word[64], var[64], set[1024], item[260];
        const char *s, *close, *command;
        int level = 0;
        size_t n;

        s = next_word(line, word, sizeof(word)); /* FOR */
        s = next_word(s, var, sizeof(var));
        if (var[0] != '%' || !var[1]) {
                fprintf(stderr, "Syntax error: FOR %%V IN (set) DO command\n");
                return 1;
        }
        s = next_word(s, word, sizeof(word));
        s = skip_space(s);
        if (strcasecmp(word, "in") != 0 || *s != '(' || !(close = strchr(s, ')'))) {
                fprintf(stderr, "Syntax error: FOR %%V IN (set) DO command\n");
                return 1;
        }
        n = (size_t)(close - s - 1);
        if (n >= sizeof(set))
                n = sizeof(set) - 1;
        memcpy(set, s + 1, n);
        set[n] = 0;
        command = next_word(close + 1, word, sizeof(word));
        command = skip_space(command);
        if (strcasecmp(word, "do") != 0 || !*command) {
                fprintf(stderr, "Syntax error: FOR %%V IN (set) DO command\n");
                return 1;
        }

        for (s = set; *(s = s + strspn(s, " \t,;"));) {
                n = strcspn(s, " \t,;");
                if (n >= sizeof(item))
                        n = sizeof(item) - 1;
                memcpy(item, s, n);
                item[n] = 0;
                s += n;
                if (strpbrk(item, "*?")) {
                        glob_t g;
                        size_t i;
                        memset(&g, 0, sizeof(g));
                        if (glob(item, 0, NULL, &g) == 0)
                                for (i = 0; i < g.gl_pathc; i++) {
                                        if (run_one(var, command, g.gl_pathv[i]) != EXIT_SUCCESS)
                                                break;
                                        level = errno;
                                }
                        globfree(&g);
                } else {
                        if (run_one(var, command, item) != EXIT_SUCCESS)
                                break;
                        level = errno;
                }
        }
        return level;
}

/* FOR as an applet (the shell gives it the whole line, see command.c) */
int command_for(int argc, char *argv[]) {
        char line[1024];
        size_t o = 0;
        int i;

        line[0] = 0;
        for (i = 0; i < argc; i++)
                o += (size_t)snprintf(line + o, o < sizeof(line) ? sizeof(line) - o : 0, "%s%s",
                                      i ? " " : "", argv[i]);
        return for_line(line);
}

const char *help_for() { return "Runs a command for each file of a set"; }
