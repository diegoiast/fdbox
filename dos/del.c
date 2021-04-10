#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dos/del.h"
#include "lib/strextra.h"
#include "fdbox.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#ifdef __MSDOS__
#include "lib/tc202/dos-glob.h"
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#include <sys/stat.h>
#endif

#ifdef _POSIX_C_SOURCE
#include <glob.h>
#include <stdbool.h>
#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>
#endif

#ifdef __WIN32__
#include "lib/win32/dirent.h"
#include "lib/win32/win32-glob.h"
#include <stdbool.h>
#include <unistd.h>
#include <utime.h>
#endif

struct del_config {
        bool show_help;
        bool prompt;
        bool verbose;
        bool recursive;
        bool force;
        const char *file_glob[128];
        size_t file_glob_count;
};

/* homage to a classic
 * https://thedailywtf.com/articles/what_is_truth_0x3f_
 */
typedef enum deletion_result {
        yes, no, cancel
} deletion_result;

static void del_config_init(struct del_config *config);
static bool del_config_parse(int argc, char *argv[], struct del_config *config);
static bool del_config_print(const struct del_config *config);
static void del_print_extended_help();
static deletion_result del_single_file(struct del_config *config, const char* file_name);
static deletion_result del_single_dir(struct del_config *config, const char* file_name, int *deleted_file_count, int *found_file_count);

int command_del(int argc, char *argv[])
{
        int i;
        int total_deleted_files = 0;
        int total_files = 0;
        struct del_config config;

        del_config_init(&config);
        if (!del_config_parse(argc, argv, &config)) {
                printf("Failed parsing command line args\n");
                return EXIT_FAILURE;
        }
/*      del_config_print(&config); */

        if (config.show_help) {
                del_print_extended_help();
                return EXIT_SUCCESS;
        }
        for (i = 0; i < config.file_glob_count; i++ ) {
                glob_t globbuf = {0};
                deletion_result result;
                int j;

                glob(config.file_glob[i], GLOB_DOOFFS, NULL, &globbuf);
                for (j = 0; j != globbuf.gl_pathc; j++) {
                        const char *file_name = globbuf.gl_pathv[j];
                        struct stat st;
                        int deleted_files = 0;
                        int tested_files = 0;

                        stat(file_name, &st);
                        if (S_ISDIR(st.st_mode)) {
                                result = del_single_dir(&config, file_name, &deleted_files, &tested_files);
                        } else {
                                result = del_single_file(&config, file_name);
                                deleted_files = 1;
                                tested_files = 1;
                        }
                        if (result == yes) {
                                total_deleted_files += deleted_files;
                        } else if (result == cancel) {
                                break;
                        }
                        total_files += tested_files;
                }
                globfree(&globbuf);

                if (result == cancel) {
                        break;
                }
        }

        if (config.verbose) {
                if (total_files == total_deleted_files) {
                        printf("Deleted %d files\n", total_files);
                } else {
                        printf("Deleted %d/%d files\n", total_deleted_files, total_files);
                }
        }
        return EXIT_FAILURE;
}

const char *help_del()
{
        return "Delete one or more files";
}

void del_config_init(struct del_config *config)
{
        config->show_help = false;
        config->prompt = false;
        config->verbose = false;
        config->recursive = false;
        config->file_glob_count = 0;
        memset(config->file_glob, 0, sizeof(config->file_glob));
}

bool del_config_parse(int argc, char *argv[], struct del_config *config)
{
        size_t i;
        for (i = 1; i < (size_t)argc; i++) {
                char c1, c2;
                c1 = tolower(argv[i][0]);
                switch (c1) {
                case ARGUMENT_DELIMIER:
                        c2 = tolower(argv[i][1]);
                        switch (c2) {
                        case 'p':
                                config->prompt = true;
                                break;
                        case 'f':
                                config->force = true;
                                break;
                        case 'v':
                                config->verbose = true;
                                break;
                        case 'r':
                                config->recursive = true;
                                break;
                        case '?':
                        case 'h':
                                config->show_help = true;
                                break;
                        default:
                                return false;
                        }
                        break;

                default:
                        /* ok its a file */
                        config->file_glob[config->file_glob_count] = argv[i];
                        config->file_glob_count ++;
                }
        }
        return true;
}

bool del_config_print(const struct del_config *config)
{
        int i;
        printf("prompt for deletion: %s\n", str_bool(config->prompt));
        printf("verbose: %s\n", str_bool(config->verbose));
        printf("recursive: %s\n", str_bool(config->recursive));
        printf("show help %s\n", str_bool(config->show_help));
        for (i = 0; i < config->file_glob_count; i++ ) {
                printf(" -> %s\n", config->file_glob[i]);
        }
}

void del_print_extended_help()
{
        printf("%s\n", help_del());

        printf("   del [files] /p /v /r /f\n");
        printf("   /p Prompt for deletion of each file\n");
        printf("   /v Verbose delete\n");
        printf("   /r Recursive delete (delete subdirs)\n");
        printf("   /r Force deletion of subdirs, or RO files\n");
}

deletion_result del_single_file(struct del_config *config, const char* file_name)
{
        bool delete_file = true;
        int r;

        if (config->prompt) {
                int c;
                printf("%s :Delete this file?  (Yes/No/All/Cancel)", file_base_name(file_name));
                c = fgetc(stdin);
                switch (tolower(c)) {
                case 'y':
                        break;
                case 'n':
                        delete_file = false;
                        break;
                case 'a':
                        config->prompt = false;
                        break;
                case 'c':
                default:
                        return cancel;
                        break;
                }

        }

        if (!delete_file) {
             return no;
        }
        if (config->verbose) {
                printf(" <x> %s\n", file_name);
        }
        r = remove(file_name);
        return r == 0 ? yes : no;
}

static deletion_result del_single_dir(struct del_config *config, const char* file_name, int *deleted_file_count, int *found_file_count)
{
        glob_t globbuf = {0};
        deletion_result result;
        char fname[256];
        int j;

        *deleted_file_count = 0;
        if (!config->force) {
                return no;
        }

        fname[0] = 0;
        strcat(fname, file_name);
        strcat(fname, DIRECTORY_DELIMITER);
        strcat(fname, ALL_FILES_GLOB);

        glob(fname, GLOB_DOOFFS, NULL, &globbuf);

        for (j = 0; j != globbuf.gl_pathc; j++) {
                (*found_file_count) ++;
                result = del_single_file(config, globbuf.gl_pathv[j]);
                if (result == yes) {
                        (*deleted_file_count) ++;
                } else if (result == cancel) {
                        break;
                }
        }
        globfree(&globbuf);
        return result;
}
