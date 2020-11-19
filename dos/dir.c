// This file is part of fdbox
// For license - read license.txt

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>

#include "lib/args.h"
#include "fdbox.h"
#include "dir.h"

// https://github.com/tronkko/dirent/tree/master/examples
// https://github.com/tronkko/dirent/blob/master/include/dirent.h
// https://gist.github.com/saghul/8013376
// https://github.com/tproffen/DiffuseCode/blob/master/lib_f90/win32-glob.c

#ifdef _POSIX_C_SOURCE
#include <dirent.h>
#include <glob.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif

#ifdef __WIN32__
#include "lib/win32/dirent.h"
#endif

// this is to shut up QtCreator
#ifndef bool
#define bool int
#define false 0
#define true !false
#endif

///////////////////////////////////////////////////////////////////////////////
// Types used internally

#define SORT_NAME      0b00000001
#define SORT_EXTENTION 0b00000010
#define SORT_DIRS      0b00000100
#define SORT_DATE      0b00001000
#define SORT_SIZE      0b00010000

struct dir_config {
    bool show_help;              // ?
    bool pause;                  // p
    bool wide;                   // w
//    int attr;                    // a D[dirs], H[hidden], S[system], R[readonly], A[Archive] -[not]
    int sort_order;              //
    bool subdirs;                // s
    bool bare;                   // b
    bool lower_case;             // l
//    bool display_compress;       // c[h]
// All other options are treated as files
};

struct dir_files {
    char* files[128];
    size_t files_count;
};

struct file_details {
        char* file_name;
        struct stat file_details;
};

///////////////////////////////////////////////////////////////////////////////
// Forward declarations
static void dir_config_init(struct dir_config *config, struct dir_files *files);
static void dir_config_print(struct dir_config *config);
static void dir_display_dir(struct dir_config *config, const char* dir_name, struct dir_files *files2, int depth);
static bool dir_parse_config(int argc, char* argv[], struct dir_config *config, struct dir_files *files);
static void dir_print_extended_help();


static int dir_file_order;
static int dir_file_comperator(const void *a, const void *b);

static bool found(const char* file_name, const struct file_details files[], size_t file_count);
static const char* pb(bool b);
static char* string_to_lower(char* s);

static bool flag_test(int value, int flag);
static bool flag_set(int *value, int flag, bool on);

///////////////////////////////////////////////////////////////////////////////
// This is public API
int command_dir(int argc, char* argv[]) {
        struct dir_config config;
        struct dir_files files;

        // first read configuration from command line
        dir_config_init(&config, &files);
        if (!dir_parse_config(argc, argv, &config, &files)) {
                printf("Failed parsing command line args\n");
                return EXIT_FAILURE;
        }
//        dir_config_print(&config);

        if (config.show_help) {
                dir_print_extended_help();
                return EXIT_SUCCESS;
        }

        if (files.files_count == 0) {
                files.files[0] = "*";
                files.files_count = 1;
        }

        dir_display_dir(&config, ".", &files, 1);
}

static void dir_display_dir(struct dir_config *config, const char* dir_name, struct dir_files *files2, int depth)
{
        // then find all available files
        struct file_details files[1000];
        int file_count = 0;

        for (size_t i=0; i<files2->files_count; i++ ) {
                glob_t globbuf = {0};
                glob(files2->files[i], GLOB_DOOFFS, NULL, &globbuf);
                for (size_t j = 0; j != globbuf.gl_pathc; j++) {
                        const char* file_name = globbuf.gl_pathv[j];
                        if (!found(file_name, files, file_count)) {
                                files[file_count].file_name = strdup(globbuf.gl_pathv[j]);
                                file_count ++;
                        }
                }
                globfree(&globbuf);
        }

        // then get info about each file
        for (int i=0; i< file_count; i++) {
                stat(files[i].file_name, &files[i].file_details);
        }
        // and sort if needed
        if (config->sort_order != 0) {
                dir_file_order = config->sort_order;
                qsort(files, file_count, sizeof(struct file_details), dir_file_comperator);
        }

        long long int total_bytes = 0;
        long long int free_bytes = 0;
        int total_files = 0;
        int total_dirs = 0;
        int lines = 0;
        int cols = 0;
        if (!config->bare) {
                printf("\nDirectory of %s (depth=%d)\n", dir_name, depth);
        }
        for (int i=0; i< file_count; i++) {
                char display[200];
                total_bytes += files[i].file_details.st_size;
                if (S_ISDIR(files[i].file_details.st_mode)) {
                        total_dirs ++;
                        if (config->wide) {
                                snprintf(display, 200, "[%s]", files[i].file_name);
                        }
                        else {
                                snprintf(display, 200, "%s/", files[i].file_name);
                        }
                } else {
                        total_files ++;
                        snprintf(display, 200, "%s", files[i].file_name);
                }

                if (config->lower_case) {
                        string_to_lower(display);
                }

                if (config->wide) {
                        printf("%-18s ", display);
                        cols ++;
                        if (cols == 4) {
                                cols = 0;
                                lines++;
                                putchar('\n');
                        }
                } else {
                        if (S_ISDIR(files[i].file_details.st_mode)) {
                                printf("%-40s %20s %s %s\n", display, "<DIR>", "01-12-2020", "13:45");
                        } else {
                                printf("%-40s %20ld %s %s\n", display, files[i].file_details.st_size, "01-12-2020", "13:45");
                        }
                        lines ++;
                }

                if (config->subdirs && !S_ISDIR(files[i].file_details.st_mode)) {
                        // file name is no longer needed
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

        if (config->subdirs) {
                for (int i=0; i< file_count; i++) {
                        if (!S_ISDIR(files[i].file_details.st_mode)) {
                                continue;
                        }
                        if (files[i].file_name == 0 || files[i].file_name[0] == '\0') {
                                continue;
                        }
                        struct dir_files f;
                        char *s = malloc(strlen(files[i].file_name) + 5);
                        strcpy(s, files[i].file_name);
                        strcat(s, "/*.*");
                        f.files[0] = s;
                        f.files_count = 1;
                        dir_display_dir(config, files[i].file_name, &f, depth +1 );
                        free(s);
                        free(files[i].file_name);
                }
        }
}

const char* help_dir() {
        return "Displays a list of files and subdirectories in a directory";
}

///////////////////////////////////////////////////////////////////////////////
// internal functions
static void dir_config_init(struct dir_config *config, struct dir_files *files) {
        config->pause = false;
        config->wide = false;
        config->bare = false;
        config->sort_order = 0;
        //    config->compress_ratio = 0; // 1 = yes, 2 = host allocation unit size
        config->lower_case = false;
        config->subdirs = false;
        config->show_help = false;
        files->files_count = 0;
}

static void dir_config_print(struct dir_config *config) {
        printf("\tpause=%s\n", pb(config->pause));
        printf("\twide_format=%s\n", pb(config->wide));
//        printf("\tattr=%d\n", config->attr);
        printf("\tsubdirs=%s\n", pb(config->subdirs));
        printf("\tlower_case=%s\n", pb(config->lower_case));
//        printf("\tdisplay_compress=%d\n", config->display_compress);

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

//        for (size_t i=0; i<config->files_count; i++ ) {
//                printf("%s ", config->files[i]);
//        }
        printf("\n");
}

static bool dir_parse_config(int argc, char* argv[], struct dir_config *config, struct dir_files *files) {
        for (int i=1; i < argc; i++) {
            char c1, c2;

            c1 = argv[i][0];
            switch (c1) {
            case '/':
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
                                    flag_set( &config->sort_order, SORT_NAME, true);
                                    break;
                            case 'e':
                            case 'E':
                                    flag_set( &config->sort_order, SORT_EXTENTION, true);
                                    break;
                            case 'g':
                            case 'G':
                                    flag_set( &config->sort_order, SORT_DIRS, true);
                                    break;
                            case 'd':
                            case 'D':
                                    flag_set( &config->sort_order, SORT_DATE, true);
                                    break;
                            case 's':
                            case 'S':
                                    flag_set( &config->sort_order, SORT_SIZE, true);
                                    break;
                            default:
                                    printf("invalid sort argument %d/\n", i);
                                    return false;
                                    break;
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
                            printf("invalid command line switch at index %d/1\n", i);
                            return false;
                    }
                    break;
            default:
                    files->files[files->files_count] = argv[i];
                    files->files_count ++;
                    break;
            }
        }
        return true;
}

static void dir_print_extended_help()
{
        printf("%s\n", help_dir());
        printf("   dir [files] /p /w /s /b /l /O [files]\n");

        printf("   /p pause after each screen\n");
        printf("   /w use wide format, no details\n");
        printf("   /s display contents of sub directories as well\n" );
        printf("   /b bare format - print no headers of suffix\n" );
        printf("   /l lower case - all names are dislpayed in lower case\n" );
        printf("   /a[] filter files by attributes\n");
        printf("       /ad - directoroies    /ar - readonly   /ah - hidden\n");
        printf("       /as - system files\n");
        printf("   /o[esdng] sort files\n" );
        printf("       /oe - extension       /os - by name    /od - date\n" );
        printf("       /on - by size         /og - directories first \n" );
        printf("Examples:\n");
        printf("> dir /w /os *.exe\n");
        printf("> dir /p /b *.txt *.com\n");
}

static int dir_file_comperator(const void *a, const void *b)
{
        const struct file_details *file1 = (const struct file_details*) (a);
        const struct file_details *file2 = (const struct file_details*) (b);

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
                        order --;
                } else if (!S_ISDIR(file1->file_details.st_mode) && S_ISDIR(file2->file_details.st_mode)) {
                        order ++;
                } else if (!S_ISDIR(file1->file_details.st_mode) && !S_ISDIR(file2->file_details.st_mode)) {
                        if (file1->file_details.st_size > file2->file_details.st_size) {
                                order --;
                        } else if (file1->file_details.st_size < file2->file_details.st_size) {
                                order ++;
                        }
                }
        }
        if (flag_test(dir_file_order, SORT_DATE)) {
                if (file1->file_details.st_mtim.tv_sec > file2->file_details.st_mtim.tv_sec) {
                        order ++;
                }
                if (file1->file_details.st_mtim.tv_sec < file2->file_details.st_mtim.tv_sec) {
                        order --;
                }
        }
        if (flag_test(dir_file_order, SORT_NAME)) {
                int v;
                // TODO - config should be global, and we should use it here
//                v = strcmp(file1->file_name, file2->file_name);
                v = strcasecmp(file1->file_name, file2->file_name);
                if (v > 0) {
                        order += 2;
                }
                if (v < 0) {
                        order -= 2;
                }
        }
        // TODO - sort by ext
        return order;
}


static bool found(const char* file_name, const struct file_details files[], size_t file_count) {
        for (size_t i=0; i<file_count; i++) {
                if (strcmp(file_name, files[i].file_name ) == 0) {
                        return true;
                }
        }
        return false;
}

static const char* pb(bool b)
{
        return b ? "true" : "false";
}

static char* string_to_lower(char* s)
{
        while (*s) {
                *s = tolower(*s);
                s++;
        }
}

static bool flag_test(int value, int flag)
{
        return (value & flag) != 0;
}
static bool flag_set(int *value, int flag, bool on)
{
        if (on) {
                *value |= flag;
        } else {
                *value ^= !flag;
        }
}
