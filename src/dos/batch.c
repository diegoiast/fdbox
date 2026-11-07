/*
This file is part of fdbox
For license - read license.txt
*/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

#include "dos/batch.h"
#include "dos/command.h"
#include "dos/prompt.h"
#include "lib/environ.h"
#include "fdbox.h"
#include "lib/nls.h"

extern bool echo_is_on;

struct batch *batch_current;

static bool has_extension(const char *name, const char *ext) {
        size_t n = strlen(name), e = strlen(ext);
        return n > e && strcasecmp(name + n - e, ext) == 0;
}

static bool exists(const char *path) {
        struct stat st;
        return stat(path, &st) == 0 && !S_ISDIR(st.st_mode);
}

static bool find_in(const char *dir, const char *name, char *path, size_t size) {
        static const char *const ext[] = {".BTM", ".BAT"};
        const char *sep = "";
        size_t i;

        if (dir && *dir && !strchr("\\/:", dir[strlen(dir) - 1]))
                sep = DIRECTORY_DELIMITER;
        if (has_extension(name, ".btm") || has_extension(name, ".bat")) {
                snprintf(path, size, "%s%s%s", dir ? dir : "", sep, name);
                return exists(path);
        }
        for (i = 0; i < sizeof(ext) / sizeof(ext[0]); i++) {
                snprintf(path, size, "%s%s%s%s", dir ? dir : "", sep, name, ext[i]);
                if (exists(path))
                        return true;
        }
        return false;
}

bool batch_find(const char *name, char *path, size_t size) {
        const char *p;

        if (find_in(NULL, name, path, size))
                return true;
        if (strpbrk(name, "\\/:"))
                return false;
        for (p = getenv("PATH"); p && *p;) {
                char dir[260];
                size_t n = strcspn(p, ";");
                if (n && n < sizeof(dir)) {
                        memcpy(dir, p, n);
                        dir[n] = 0;
                        if (find_in(dir, name, path, size))
                                return true;
                }
                p += n + (p[n] == ';');
        }
        return false;
}

static char **read_lines(const char *path, size_t *count) {
        FILE *f = fopen(path, "r");
        char buf[1024], **lines = NULL;
        size_t n = 0;

        if (!f)
                return NULL;
        while (fgets(buf, sizeof(buf), f)) {
                char **l = realloc(lines, (n + 1) * sizeof(char *));
                size_t len = strcspn(buf, "\r\n");
                if (!l)
                        break;
                lines = l;
                buf[len] = 0;
                lines[n++] = strdup(buf);
        }
        fclose(f);
        *count = n;
        return lines ? lines : calloc(1, sizeof(char *));
}

/* %0-%9 from the batch's arguments, %NAME% from the environment, %% */
static void expand(const struct batch *b, const char *src, char *dst, size_t size) {
        size_t o = 0;

        while (*src && o + 1 < size) {
                const char *add = NULL;
                char name[100];

                if (*src != '%') {
                        dst[o++] = *src++;
                        continue;
                }
                if (src[1] == '%') {
                        dst[o++] = '%';
                        src += 2;
                        continue;
                }
                if (isdigit((unsigned char)src[1])) {
                        int i = b->shift + (src[1] - '0');
                        add = i < b->argc ? b->args[i] : "";
                        src += 2;
                } else {
                        const char *end = strchr(src + 1, '%');
                        size_t n = end ? (size_t)(end - src - 1) : 0;
                        if (!end || !n || n >= sizeof(name)) {
                                dst[o++] = *src++;
                                continue;
                        }
                        memcpy(name, src + 1, n);
                        name[n] = 0;
                        add = getenv(name);
                        if (!add) {
                                size_t i;
                                for (i = 0; i < n; i++)
                                        name[i] = (char)toupper((unsigned char)name[i]);
                                add = getenv(name);
                        }
                        if (!add)
                                add = "";
                        src = end + 1;
                }
                while (*add && o + 1 < size)
                        dst[o++] = *add++;
        }
        dst[o] = 0;
}

int batch_run(const char *path, int argc, char *argv[]) {
        struct batch b;
        char line[1024];
        int level = 0, i;

        memset(&b, 0, sizeof(b));
        b.lines = read_lines(path, &b.count);
        if (!b.lines) {
                fprintf(stderr, _("%s: cannot read the batch file\n"), path);
                return 1;
        }
        b.args[0] = strdup(path);
        for (i = 1; i < argc && i < BATCH_MAX_ARGS; i++)
                b.args[i] = strdup(argv[i]);
        b.argc = i;
        b.parent = batch_current;
        batch_current = &b;

        while (!b.done && b.pc < b.count) {
                const char *raw = b.lines[b.pc++];
                char *l;
                bool quiet = false;

                while (*raw == ' ' || *raw == '\t')
                        raw++;
                if (*raw == ':' || *raw == 0)
                        continue; /* a label, or nothing */
                if (*raw == '@') {
                        quiet = true;
                        raw++;
                }
                expand(&b, raw, line, sizeof(line));
                l = line;
                while (*l == ' ' || *l == '\t')
                        l++;
                if (!*l)
                        continue;
                if (echo_is_on && !quiet) {
                        char prompt[256];
                        const char *t = getenv("PROMPT");
                        get_prompt(t ? t : "$P$G", prompt, sizeof(prompt));
                        printf("%s%s\n", prompt, l);
                }
                errno = level;
                if (command_execute_line(l) != EXIT_SUCCESS)
                        b.done = true; /* EXIT ends the batch file */
                level = errno;
        }

        batch_current = b.parent;
        for (i = 0; i < b.argc; i++)
                free(b.args[i]);
        for (size_t k = 0; k < b.count; k++)
                free(b.lines[k]);
        free(b.lines);
        errno = level;
        return level;
}

int batch_goto(const char *label) {
        struct batch *b = batch_current;
        size_t i, n;

        if (!b) {
                fputs(_("GOTO works only in batch files\n"), stderr);
                return EXIT_FAILURE;
        }
        while (*label == ':')
                label++;
        if (strcasecmp(label, "eof") == 0) {
                b->pc = b->count;
                return EXIT_SUCCESS;
        }
        n = strcspn(label, " \t");
        for (i = 0; i < b->count; i++) {
                const char *l = b->lines[i];
                while (*l == ' ' || *l == '\t')
                        l++;
                if (*l == ':' && strncasecmp(l + 1, label, n) == 0 &&
                    (l[1 + n] == 0 || isspace((unsigned char)l[1 + n]))) {
                        b->pc = i + 1;
                        return EXIT_SUCCESS;
                }
        }
        fprintf(stderr, _("Label not found: %.*s\n"), (int)n, label);
        b->done = true;
        return EXIT_FAILURE;
}

int batch_shift(void) {
        if (!batch_current)
                return EXIT_FAILURE;
        if (batch_current->shift < batch_current->argc)
                batch_current->shift++;
        return EXIT_SUCCESS;
}
