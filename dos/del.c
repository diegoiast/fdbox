#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/del.h"
#include "fdbox.h"
#include "lib/args.h"
#include "lib/strextra.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#ifdef __MSDOS__
#include "lib/tc202/dos-glob.h"
#include "lib/tc202/stdextra.h"
#include <sys/stat.h>
#endif

#ifdef _POSIX_C_SOURCE
#include <glob.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#endif

#ifdef __WIN32__
#include "lib/win32/dirent.h"
#include "lib/win32/win32-glob.h"
#include <unistd.h>
#include <utime.h>
#endif

struct del_config {
        bool prompt;
        bool recursive;
        bool force;
        struct command_config global;
};

/* homage to a classic
 * https://thedailywtf.com/articles/what_is_truth_0x3f_
 */
typedef enum deletion_result { yes, no, cancel } deletion_result;

static void del_config_init(struct del_config *config);
static bool del_config_parse(int argc, char *argv[], struct del_config *config);
static void del_config_print(const struct del_config *config);
static void del_print_extended_help();
static deletion_result del_single_file(struct del_config *config, const char *file_name);
static deletion_result del_dir(struct del_config *config, const char *file_name,
                               int *deleted_file_count, int *found_file_count,
                               int *deleted_dirs_count, int *found_dirs_count);

int command_del(int argc, char *argv[]) {
        size_t i;
        int total_deleted_files = 0;
        int total_files = 0;
        int total_deleted_dirs = 0;
        int total_dirs = 0;
        struct del_config config;

        del_config_init(&config);
        if (!del_config_parse(argc, argv, &config)) {
                printf("Failed parsing command line args\n");
                return EXIT_FAILURE;
        }
        /* del_config_print(&config); */

        if (config.global.show_help) {
                del_print_extended_help();
                return EXIT_SUCCESS;
        }
        for (i = 0; i < config.global.files.count; i++) {
                deletion_result result;
                int tested_files = 0;
                int tested_dirs = 0;
                int deleted_files = 0;
                int deleted_dirs = 0;

                result = del_dir(&config, config.global.files.file[i], &deleted_files,
                                 &tested_files, &deleted_dirs, &tested_dirs);
                total_deleted_dirs += deleted_dirs;
                total_deleted_files += deleted_files;
                total_dirs += tested_dirs;
                total_files += tested_files;

                if (result == cancel) {
                        break;
                }
        }

        if (config.global.verbose) {
                if (total_files != 0) {
                        if (total_files == total_deleted_files) {
                                printf("Deleted %d files\n", total_files);
                        } else {
                                printf("Deleted %d/%d files\n", total_deleted_files, total_files);
                        }
                }
                if (total_dirs != 0) {
                        if (total_dirs == total_deleted_dirs) {
                                printf("Deleted %d directories\n", total_dirs);
                        } else {
                                printf("Deleted %d/%d directories\n", total_deleted_dirs,
                                       total_dirs);
                        }
                }
        }
        return EXIT_FAILURE;
}

const char *help_del() { return "Delete one or more files"; }

void del_config_init(struct del_config *config) {
        config->prompt = false;
        config->recursive = false;
        command_config_init(&config->global);
}

bool del_config_parse(int argc, char *argv[], struct del_config *config) {
        int c1, c2;
        do {
                c1 = command_config_parse(argc, argv, &config->global);
                c2 = tolower(c1);
                switch (c2) {
                case 'p':
                        config->prompt = true;
                        break;
                case 'f':
                        config->force = true;
                        break;
                case 'r':
                        config->recursive = true;
                        break;
                case ARG_PROCESSED:
                        break;
                case ARG_DONE:
                        break;
                default:
                        return false;
                }
        } while (c2 >= 0);
        return true;
}

static void del_config_print(const struct del_config *config) {
        int i;
        printf("prompt for deletion: %s\n", str_bool(config->prompt));
        printf("recursive: %s\n", str_bool(config->recursive));
        printf("verbose: %s\n", str_bool(config->global.verbose));
        printf("show help %s\n", str_bool(config->global.show_help));
        command_config_print(&config->global);
}

static void del_print_extended_help() {
        printf("%s\n", help_del());

        printf("   del [files] /p /v /r /f\n");
        printf("   /p Prompt for deletion of each file\n");
        printf("   /v Verbose delete\n");
        printf("   /r Recursive delete (delete subdirs)\n");
        printf("   /f Force deletion of subdirs, or RO files\n");
}

static deletion_result del_single_file(struct del_config *config, const char *file_name) {
        bool delete_file = true;
        int r;

        if (config->prompt) {
                int c;
                printf("%s :Delete this file? (Yes/No/All/Cancel) ", file_base_name(file_name));
                c = fgetc(stdin);
                fgetc(stdin);
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
        if (config->global.verbose && !config->prompt) {
                printf(" <f> %s\n", file_name);
        }
        r = remove(file_name);
        return r == 0 ? yes : no;
}

static deletion_result del_dir(struct del_config *config, const char *file_name,
                               int *deleted_file_count, int *found_file_count,
                               int *deleted_dirs_count, int *found_dirs_count) {
        glob_t globbuf = {0};
        deletion_result result = yes;
        bool delete_dir_on_exit = false;
        char fname[256];
        size_t j;
        struct stat st;

        fname[0] = 0;
        strcat(fname, file_name);
        stat(file_name, &st);
        if (S_ISDIR(st.st_mode)) {
                if (!str_ends_with(file_name, '/') && !str_ends_with(file_name, '\\')) {
                        strcat(fname, DIRECTORY_DELIMITER);
                }
                strcat(fname, ALL_FILES_GLOB);
                delete_dir_on_exit = true;
                (*found_dirs_count)++;
        }
        glob(fname, GLOB_DOOFFS, NULL, &globbuf);
        for (j = 0; j != globbuf.gl_pathc; j++) {
                const char *name = globbuf.gl_pathv[j];
                stat(name, &st);
                if (S_ISDIR(st.st_mode)) {
                        result = del_dir(config, name, deleted_file_count, found_file_count,
                                         deleted_dirs_count, found_dirs_count);
                } else {
                        (*found_file_count)++;
                        result = del_single_file(config, globbuf.gl_pathv[j]);
                        if (result == yes) {
                                (*deleted_file_count)++;
                        }
                }

                if (result == cancel) {
                        break;
                }
        }
        globfree(&globbuf);
        if (result == yes && delete_dir_on_exit) {
                /* this is actualy a directory */
                remove(file_name);
                (*deleted_dirs_count)++;
                if (config->global.verbose) {
                        printf(" <d> %s\n", file_name);
                }
        }
        return result;
}
