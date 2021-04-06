/* This file is part of fdbox
 For license - read license.txt
*/

/*
 * TODO
 * - dir /s - display accumulative byte count
 * - dir /a - filter types
 * - dir /p - pause - WIP
 * - display fanicer byte sizes
 * - fix display of subdirectories without any glob (dir /w ../)
 * - on msdos "dir c:*" (when in d:) displays garbage ("dir c:\*" works).
 */

/*
Places I look for inspiration:
https://github.com/tronkko/dirent/tree/master/examples
https://github.com/tronkko/dirent/blob/master/include/dirent.h
https://gist.github.com/saghul/8013376
https://github.com/tproffen/DiffuseCode/blob/master/lib_f90/win32-glob.c
*/

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dos/cmd_dir.h"
#include "fdbox.h"
#include "lib/args.h"
#include "lib/strextra.h"

#ifdef _POSIX_C_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <glob.h>
#include <malloc.h>
#include <stdbool.h>
#include <sys/stat.h>
#endif

#ifdef __WIN32__
#include "lib/win32/dirent.h"
#include "lib/win32/win32-glob.h"
#endif

#ifdef __MSDOS__
#include "lib/tc202/dos-glob.h"
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#include <sys/stat.h>
#endif

/***************************************************************************
 * Types used internally
 ***************************************************************************/

#define SORT_NAME 0x01
#define SORT_EXTENTION 0x02
#define SORT_DIRS 0x04
#define SORT_DATE 0x08
#define SORT_SIZE 0x10

#define MAX_DIR_FILES 128
#define MAX_DIR_ENTRY_FILES 1024

struct dir_config {
        bool show_help;
        bool pause;
        bool wide;
        int sort_order;
        bool subdirs;
        bool bare;
        bool lower_case;
        /* All other options are treated as files */
};

struct dir_files {
        char *files[MAX_DIR_FILES];
        size_t files_count;
};

struct file_entry {
        char *file_name;
        struct stat file_details;
};

/***************************************************************************
 * Forward declarations
 ***************************************************************************/
static void dir_config_init(struct dir_config *config, struct dir_files *files);
static void dir_config_print(struct dir_config *config);
static void dir_display_dir(struct dir_config *config, const char *dir_name,
                            struct dir_files *files2, int depth);
static bool dir_parse_config(int argc, char *argv[], struct dir_config *config,
                             struct dir_files *files);
static void dir_format_date_time(long ff_date, long ff_time, char *time, char *date);
static void dir_print_extended_help();

static int dir_file_order;
static int dir_file_comperator(const void *a, const void *b);

static bool found(const char *file_name, const struct file_entry files[], size_t file_count);

/* TODO - should this be moved to a library? */
static bool prefix(const char *pre, const char *str) { return strncmp(pre, str, strlen(pre)) == 0; }
static bool flag_test(int value, int flag);
static void flag_set(int *value, int flag, bool on);

/***************************************************************************
 * This is public API
 ***************************************************************************/
int command_dir(int argc, char *argv[]) {
        struct dir_config config;
        struct dir_files files;

        /* first read configuration from command line */
        dir_config_init(&config, &files);
        if (!dir_parse_config(argc, argv, &config, &files)) {
                printf("Failed parsing command line args\n");
                return EXIT_FAILURE;
        }
        /*        dir_config_print(&config); */

        if (config.show_help) {
                dir_print_extended_help();
                return EXIT_SUCCESS;
        }

        if (files.files_count == 0) {
#ifdef __MSDOS__
                files.files[0] = strdup("*.*");
#else
                files.files[0] = strdup("*");
#endif
                files.files_count = 1;
        }

        dir_display_dir(&config, ".", &files, 1);
        return EXIT_SUCCESS;
}

static void dir_display_dir(struct dir_config *config, const char *dir_name,
                            struct dir_files *files2, int depth) {
        /* then find all available files */
        struct file_entry files[MAX_DIR_ENTRY_FILES];
        size_t file_count = 0, requested_count = 0;
        size_t i, j;

        long long int total_bytes = 0;
        long long int free_bytes = 0;
        int total_files = 0;
        int total_dirs = 0;
        int lines = 0;
        int cols = 0;

        for (i = 0; i < files2->files_count; i++) {
                glob_t globbuf = {0};
                glob(files2->files[i], GLOB_DOOFFS, NULL, &globbuf);
                for (j = 0; j != globbuf.gl_pathc; j++) {
                        const char *file_name = globbuf.gl_pathv[j];
                        requested_count++;
                        if (requested_count > MAX_DIR_ENTRY_FILES) {
                                continue;
                        }
                        if (!found(file_name, files, file_count)) {
                                files[file_count].file_name = strdup(globbuf.gl_pathv[j]);
                                stat(files[file_count].file_name, &files[file_count].file_details);
                                file_count++;
                        }
                }
                globfree(&globbuf);
        }

        /* and sort if needed */
        if (config->sort_order != 0) {
                dir_file_order = config->sort_order;
                qsort(files, file_count, sizeof(struct file_entry), dir_file_comperator);
        }

        if (requested_count != file_count) {
                fprintf(stderr, "Warning: %d/%d files will not be showed\n",
                        (int)requested_count - MAX_DIR_ENTRY_FILES, (int)requested_count);
        }
        if (!config->bare) {
                printf("\nDirectory of %s\n", dir_name);
        }
        for (i = 0; i < file_count; i++) {
                char display[200];
                char *fname;

                total_bytes += files[i].file_details.st_size;
                fname = files[i].file_name;

                if (strcmp(dir_name, ".") != 0) {
                        if (prefix(dir_name, fname)) {
                                fname += strlen(dir_name) + 1;
                        }
                }
                if (S_ISDIR(files[i].file_details.st_mode)) {
                        total_dirs++;
                        if (config->wide) {
                                snprintf(display, 200, "[%s]", fname);
                        } else {
                                snprintf(display, 200, "%s%s", fname, DIRECTORY_DELIMITER);
                        }
                } else {
                        total_files++;
                        snprintf(display, 200, "%s", fname);
                }

                if (config->lower_case) {
                        str_to_lower(display);
                }

                if (config->wide) {
                        printf("%-18s ", display);
                        cols++;
                        if (cols == 4) {
                                cols = 0;
                                lines++;
                                putchar('\n');
                        }
                } else {
                        char *ttime = ctime(&files[i].file_details.st_mtime);
                        if (S_ISDIR(files[i].file_details.st_mode)) {
                                printf("%-40s %10s %s", display, "<DIR>", ttime);
                        } else {
                                printf("%-40s %10ld %s", display, files[i].file_details.st_size,
                                       ttime);
                        }
                        lines++;
                }

                /* TODO read terminal height in runtime */
                /* TODO convert to press any key */
                /* https://stackoverflow.com/questions/18801483/press-any-key-to-continue-function-in-c/18801616
                 */
                if (config->pause && lines == 24) {
                        printf("Press enter to continue...\n");
                        getchar();
                }

                if (config->subdirs && !S_ISDIR(files[i].file_details.st_mode)) {
                        /* file name is no longer needed */
                        free(files[i].file_name);
                }
        }
        if (cols != 0) {
                putchar('\n');
        }
        if (!config->bare) {
                printf("%10d files(s)\t %lld bytes\n", total_files, total_bytes);
                printf("%10d dirs(s) \t %lld free\n", total_dirs, free_bytes);
        }
        for (i = 0; i < file_count; i++) {
                if (config->subdirs) {
                        struct dir_files f;
                        char *s;

                        if (!S_ISDIR(files[i].file_details.st_mode)) {
                                continue;
                        }
                        if (files[i].file_name == 0 || files[i].file_name[0] == '\0') {
                                continue;
                        }
                        s = (char *)malloc(strlen(files[i].file_name) + 5);
                        strcpy(s, files[i].file_name);
                        strcat(s, "/*.*");
                        f.files[0] = s;
                        f.files_count = 1;
                        dir_display_dir(config, files[i].file_name, &f, depth + 1);
                        free(s);
                }
                free(files[i].file_name);
        }
}

const char *help_dir() { return "Displays a list of files and subdirectories in a directory"; }

/****************************************************************************
 * internal functions
 ***************************************************************************/
static void dir_config_init(struct dir_config *config, struct dir_files *files) {
        config->pause = false;
        config->wide = false;
        config->bare = false;
        config->sort_order = 0;
        config->lower_case = false;
        config->subdirs = false;
        config->show_help = false;
        files->files_count = 0;
}

static void dir_config_print(struct dir_config *config) {
        printf("\tpause=%s\n", str_bool(config->pause));
        printf("\twide_format=%s\n", str_bool(config->wide));
        printf("\tsubdirs=%s\n", str_bool(config->subdirs));
        printf("\tlower_case=%s\n", str_bool(config->lower_case));

        printf("\tsort=");
        if (flag_test(config->sort_order, SORT_NAME)) {
                printf("name ");
        }
        if (flag_test(config->sort_order, SORT_EXTENTION)) {
                printf("extension ");
        }
        if (flag_test(config->sort_order, SORT_DIRS)) {
                printf("directories ");
        }
        if (flag_test(config->sort_order, SORT_DATE)) {
                printf("date ");
        }
        if (flag_test(config->sort_order, SORT_SIZE)) {
                printf("size ");
        }
        if (config->sort_order == 0) {
                printf("UNSORTED");
        }
        putchar('\n');

        /*
        for (size_t i=0; i<config->files_count; i++ ) {
               printf("%s ", config->files[i]);
        }
        */
        printf("\n");
}

static bool dir_parse_config(int argc, char *argv[], struct dir_config *config,
                             struct dir_files *files) {
        size_t i, files_requested = 0;

        for (i = 1; i < (size_t)argc; i++) {
                char c1, c2;

                c1 = argv[i][0];
                switch (c1) {
                case ARGUMENT_DELIMIER:
                        c2 = argv[i][1];
                        switch (c2) {
                        case 'p':
                        case 'P':
                                config->pause = true;
                                if (argv[i][2] != 0) {
                                        return false;
                                }
                                break;
                        case 'w':
                        case 'W':
                                config->wide = true;
                                if (argv[i][2] != 0) {
                                        return false;
                                }
                                break;
                        case 'a':
                                switch (argv[i][2]) {
                                case 'd':
                                case 'D':
                                        break;
                                case 'h':
                                case 'H':
                                        break;
                                case 's':
                                case 'S':
                                        break;
                                case 'r':
                                case 'R':
                                        break;
                                case 'a':
                                case 'A':
                                        break;
                                default:
                                        break;
                                }
                                break;
                        case 'o':
                                switch (argv[i][2]) {
                                case 'n':
                                case 'N':
                                        flag_set(&config->sort_order, SORT_NAME, true);
                                        break;
                                case 'e':
                                case 'E':
                                        flag_set(&config->sort_order, SORT_EXTENTION, true);
                                        break;
                                case 'g':
                                case 'G':
                                        flag_set(&config->sort_order, SORT_DIRS, true);
                                        break;
                                case 'd':
                                case 'D':
                                        flag_set(&config->sort_order, SORT_DATE, true);
                                        break;
                                case 's':
                                case 'S':
                                        flag_set(&config->sort_order, SORT_SIZE, true);
                                        break;
                                default:
                                        printf("invalid sort argument %d/\n", (int)i);
                                        return false;
                                }
                                break;
                        case 's':
                                config->subdirs = true;
                                if (argv[i][2] != 0) {
                                        return false;
                                }
                                break;
                        case 'b':
                                config->bare = true;
                                if (argv[i][2] != 0) {
                                        return false;
                                }
                                break;
                        case 'l':
                                config->lower_case = true;
                                if (argv[i][2] != 0) {
                                        return false;
                                }
                                break;
                        case '?':
                                config->show_help = true;
                                break;
                        default:
                                fprintf(stderr, "invalid command line switch at index %ld\n", i);
                                return false;
                        }
                        break;
                default:
                        files_requested++;
                        if (files->files_count < MAX_DIR_FILES) {
                                files->files[files->files_count] = argv[i];
                                files->files_count++;
                        }
                        break;
                }
        }
        if (files_requested != files->files_count) {
                fprintf(stderr, "Warning: %d/%d files not be displayed\n",
                        (int)files_requested - MAX_DIR_FILES, (int)files_requested);
        }
        return true;
}

static void dir_print_extended_help() {
        printf("%s\n", help_dir());
        printf("   dir [files] /p /w /s /b /l /O [files]\n");

        printf("   /p pause after each screen\n");
        printf("   /w use wide format, no details\n");
        printf("   /s display contents of sub directories as well\n");
        printf("   /b bare format - print no headers of suffix\n");
        printf("   /l lower case - all names are dislpayed in lower case\n");
        printf("   /a[] filter files by attributes\n");
        printf("       /ad - directoroies    /ar - readonly   /ah - hidden\n");
        printf("       /as - system files\n");
        printf("   /o[esdng] sort files\n");
        printf("       /oe - extension       /os - by name    /od - date\n");
        printf("       /on - by size         /og - directories first \n");
        printf("Examples:\n");
        printf("> dir /w /os *.exe\n");
        printf("> dir /p /b *.txt *.com\n");
}

static void dir_format_date_time(long ff_fdate, long ff_ftime, char *time, char *date) {
        int year, month, day;
        int hour, minute;
        bool longyear = true;

        year = (ff_fdate >> 9) + 80;
        if (longyear)
                year += 1900;
        else
                year %= 100;
        day = ff_fdate & 0x001f;
        month = (ff_fdate >> 5) & 0x000f;
        hour = ff_ftime >> 5 >> 6;
        minute = (ff_ftime >> 5) & 0x003f;

        snprintf(date, 10, "%d/%d/%d", day, month, year);
        snprintf(time, 10, "%02d:%0d", hour, minute);
}

static int dir_file_comperator(const void *a, const void *b) {
        const struct file_entry *file1 = (const struct file_entry *)(a);
        const struct file_entry *file2 = (const struct file_entry *)(b);

        int order = 0;
        if (flag_test(dir_file_order, SORT_DIRS)) {
                if (S_ISDIR(file1->file_details.st_mode) && !S_ISDIR(file2->file_details.st_mode)) {
                        order -= 10;
                }
                if (!S_ISDIR(file1->file_details.st_mode) && S_ISDIR(file2->file_details.st_mode)) {
                        order += 10;
                }
        }
        if (flag_test(dir_file_order, SORT_SIZE)) {
                if (S_ISDIR(file1->file_details.st_mode) && !S_ISDIR(file2->file_details.st_mode)) {
                        order--;
                } else if (!S_ISDIR(file1->file_details.st_mode) &&
                           S_ISDIR(file2->file_details.st_mode)) {
                        order++;
                } else if (!S_ISDIR(file1->file_details.st_mode) &&
                           !S_ISDIR(file2->file_details.st_mode)) {
                        if (file1->file_details.st_size > file2->file_details.st_size) {
                                order--;
                        } else if (file1->file_details.st_size < file2->file_details.st_size) {
                                order++;
                        }
                }
        }
        if (flag_test(dir_file_order, SORT_DATE)) {
                if (file1->file_details.st_mtime > file2->file_details.st_mtime) {
                        order++;
                }
                if (file1->file_details.st_mtime < file2->file_details.st_mtime) {
                        order--;
                }
        }
        if (flag_test(dir_file_order, SORT_NAME)) {
                int v;
                /* TODO - config should be global, and we should use it here */
                /* v = strcmp(file1->file_name, file2->file_name); */
                v = strcasecmp(file1->file_name, file2->file_name);
                if (v > 0) {
                        order += 2;
                }
                if (v < 0) {
                        order -= 2;
                }
        }
        if (flag_test(dir_file_order, SORT_EXTENTION)) {
                const char *ext1, *ext2;
                int v;
                ext1 = file_get_extesnsion(file1->file_name);
                ext2 = file_get_extesnsion(file2->file_name);
                printf("testing %s:%s\n", ext1, ext2);
                v = strcasecmp(ext1, ext2);
                if (v > 0) {
                        order += 2;
                }
                if (v < 0) {
                        order -= 2;
                }
        }
        return order;
}

static bool found(const char *file_name, const struct file_entry files[], size_t file_count) {
        size_t i;
        for (i = 0; i < file_count; i++) {
                if (strcmp(file_name, files[i].file_name) == 0) {
                        return true;
                }
        }
        return false;
}

static bool flag_test(int value, int flag) { return (value & flag) != 0; }

static void flag_set(int *value, int flag, bool on) {
        if (on) {
                *value |= flag;
        } else {
                *value ^= !flag;
        }
}
