#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

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
#endif

#ifdef _POSIX_C_SOURCE
#include <stdbool.h>
#endif

#ifdef __WIN32__
#include <stdbool.h>
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
static int copy_single_file(const char *from, const char *to, struct copy_config *config);

int command_copy(int argc, char* argv[]) {
        struct copy_config config;
        copy_config_init(&config);
        copy_parse_config(argc, argv, &config);
        copy_print_config(&config);

        return copy_single_file(config.source_file, config.dest_file, &config);
}

const char* help_copy() {
    return "Here should be a basic help for copy";
}

static void copy_config_init(struct copy_config *config)
{
        /*config->verify = false;*/
        config->ask_overwrite = false;
        config->verbose = true;
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
                char c1, c2, c3;
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

static int copy_single_file(const char *from, const char *to, struct copy_config *config)
{
        FILE *source;
        FILE *dest;
        char buffer[4096];
        size_t total_read, total_written;

        source = fopen(from, "r");
        if (source == NULL) {
                fprintf(stderr, "Failed reading from %s\n", from);
                return errno;
        }
        dest = fopen(to, "wb");
        if (dest == NULL) {
                fprintf(stderr, "Failed openning  %s\n", to);
                return errno;
        }

        total_read = 0;
        total_written = 0;
        do {
                size_t actually_read, actually_written;

                actually_read = fread(buffer, 1, 4096, source);
                total_read += actually_read;
                if (actually_read == 0) {
                        break;
                }
                actually_written = fwrite(buffer, 1, actually_read, dest);
                total_written += actually_written;
                if (actually_written == 0) {
                        break;
                }
        } while (true);

        fclose(source);
        fclose(dest);
        if (total_read != total_written) {
                fprintf(stderr, "Failed writing to %s (wrote %ld/%ld bytes)\n", to, total_written, total_read);
                return errno;
        }
        return EXIT_SUCCESS;
}
