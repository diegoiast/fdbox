#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fdbox.h"
#include "dos/copy.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#include "lib/tc202/dos-glob.h"

#define DIRECTORY_DELIMITER "\\"
#endif

#ifdef _POSIX_C_SOURCE
#include <stdbool.h>
#define DIRECTORY_DELIMITER "/"
#endif

#ifdef __WIN32__
#include "lib/win32/dirent.h"
#include "lib/win32/win32-glob.h"
#include <stdbool.h>
#define DIRECTORY_DELIMITER "/"
#include <unistd.h>
#endif

struct copy_config {
//        bool verify;
        bool ask_overwrite;
        bool verbose;
        bool copy_attributes;
        const char* source_file;
        const char* dest_file;
};

static void copy_config_init(struct copy_config *config);
static bool copy_parse_config(int argc, char* argv[], struct copy_config *config);
static bool copy_print_config(struct copy_config *config);
static bool copy_is_dir(const char *path);
static char *copy_append_path(const char *directory, const char *file_name);
static const char *copy_base_name(const char *file_name);
static int copy_single_file(const char *from, const char *to, struct copy_config *config);

int command_copy(int argc, char *argv[])
{
    struct copy_config config;
    const char *file_name;
    bool free_memory = false;
    int return_val;

    copy_config_init(&config);
    copy_parse_config(argc, argv, &config);
    /* copy_print_config(&config); */

    file_name = config.dest_file;
    if (copy_is_dir(file_name)) {
        const char *base_name = copy_base_name(config.source_file);
        file_name = copy_append_path(file_name, base_name);
        free_memory = true;
    }

    return_val = copy_single_file(config.source_file, file_name, &config);
    if (free_memory) {
        free((char *) file_name);
    }
    return return_val;
}

const char* help_copy() {
    return "Here should be a basic help for copy";
}

static void copy_config_init(struct copy_config *config)
{
        /*config->verify = false;*/
        config->ask_overwrite = false;
        config->verbose = false;
        config->copy_attributes = true;
        config->source_file = NULL;
        config->dest_file = NULL;
}

/*
 * MSDOS
 * /a ascii
 * /b binary
 * /v verify
 * /y - no confirmation, always overrites
 * /-y prompts to confirm every overwrite
 *
 * FreeCOM
 *
 */

static bool copy_parse_config(int argc, char* argv[], struct copy_config *config)
{
        size_t i;

        for (i=1; i < (size_t)argc; i++) {
                char c1, c2;
                c1 = argv[i][0];
                switch (c1) {
                case '/':
                        c2 = argv[i][1];
                        switch (c2) {
                        case 'a':
                                config->copy_attributes = true;
                                break;
                        case 'y':
                                config->ask_overwrite = false;
                                break;
                        case 'v':
                                config->verbose = true;
                                break;
                        }
                        break;
                default:
                        // ok its a file
                        if (config->source_file == NULL) {
                                config->source_file = strdup(argv[i]);
                        } else {
                                config->dest_file = strdup(argv[i]);
                        }
                }
        }
}

static bool copy_print_config(struct copy_config *config)
{
        printf("ask_overwrite=%d\n", config->ask_overwrite);
        printf("verbose=%d\n", config->verbose);
        printf("copy_attributes=%d\n", config->copy_attributes);
        printf("source_file=%s\n", config->source_file);
        printf("dest_file=%s\n", config->dest_file);
}

static bool copy_is_dir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

static char *copy_append_path(const char *directory, const char *file_name)
{
    int l1 = strlen(directory);
    int l2 = strlen(file_name);
    char *full_file_name;

    /* does the directory end with / or  \\ ? */
    if (directory[l1 - 1] == '/' || directory[l1 - 1] == '\\') {
        full_file_name = malloc(l1 + l2);
        strcpy(full_file_name, directory);
        strcat(full_file_name, file_name);
    } else {
        /* it does not, we need to add it manually */
        full_file_name = malloc(l1 + l2 + 1);
        strcpy(full_file_name, directory);
        strcat(full_file_name, DIRECTORY_DELIMITER);
        strcat(full_file_name, file_name);
    }

    return full_file_name;
}

static const char *copy_base_name(const char *file_name)
{
    int i = strlen(file_name);
    const char *c = file_name + i - 1;
    while (c != file_name && *c != '/' && *c != '\\') {
        c--;
    }
    return file_name;
}

static int copy_single_file(const char *from, const char *to, struct copy_config *config)
{
        FILE *source;
        FILE *dest;
        char buffer[4096];
        size_t total_read, total_written, total_size;

        source = fopen(from, "rb");
        if (source == NULL) {
                fprintf(stderr, "Failed reading from %s\n", from);
                return errno;
        }
        dest = fopen(to, "wb");
        if (dest == NULL) {
            fprintf(stderr, "Failed openning %s for writing\n", to);
            return errno;
        }

        fseek(source, 0L, SEEK_END);
        total_size = ftell(source);
        fseek(source, 0L, SEEK_SET);
/* TODO should I use:
 *         rewind(source);
 */

        total_read = 0;
        total_written = 0;
        int last_displayed = 101;
        do {
                size_t actually_read, actually_written;
                int current;

                actually_read = fread(buffer, 1, sizeof(buffer), source);
                total_read += actually_read;
                if (actually_read == 0) {
                        break;
                }
                actually_written = fwrite(buffer, 1, actually_read, dest);
                total_written += actually_written;
                if (actually_written == 0) {
                        break;
                }

                current = ((total_written * 100) / total_size);
                if (config->verbose && current != last_displayed) {
                        printf("\r%02d%% - %s\r", current, to);
                        fflush(stdout);
                        last_displayed = current;
                }
        } while (true);
        /* TODO - clear all line */
        if (config->verbose) {
                printf("\r%02d%% - %s -> %s\n", (int) ((total_written * 100) / total_size), from, to);
        }

        fclose(source);
        fclose(dest);
        if (total_read != total_written) {
                fprintf(stderr, "Failed writing to %s (wrote %ld/%ld bytes)\n", to, total_written, total_read);
                return errno;
        }
        return EXIT_SUCCESS;
}
