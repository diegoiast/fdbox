#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "lib/args.h"
#include "fdbox.h"
#include "dir.h"


// This file is part of fdbox
// For license - read license.txt

/*
Displays a list of files and subdirectories in a directory.                     
                                                                                
DIR [drive:][path][filename] [/P] [/W] [/A[[:]attribs]] [/O[[:]sortord]]        
    [/S] [/B] [/L] [/C[H]]                                                      
                                                                                
  [drive:][path][filename]   Specifies drive, directory, and/or files to list.  
  /P      Pauses after each screenful of information.                           
  /W      Uses wide list format.                                                
  /A      Displays files with specified attributes.                             
  attribs   D  Directories   R  Read-only files         H  Hidden files         
            S  System files  A  Files ready to archive  -  Prefix meaning "not" 
  /O      List by files in sorted order.                                        
  sortord   N  By name (alphabetic)       S  By size (smallest first)           
            E  By extension (alphabetic)  D  By date & time (earliest first)    
            G  Group directories first    -  Prefix to reverse order            
            C  By compression ratio (smallest first)                            
  /S      Displays files in specified directory and all subdirectories.         
  /B      Uses bare format (no heading information or summary).                 
  /L      Uses lowercase.                                                       
  /C[H]   Displays file compression ratio; /CH uses host allocation unit size.  
                                                                                
Switches may be preset in the DIRCMD environment variable.  Override            
preset switches by prefixing any switch with - (hyphen)--for example, /-W. 
 */

struct dir_config
{
    bool pause;                  // p
    bool wide;                   // w
//    int attr;                    // a D[dirs], H[hidden], S[system], R[readonly], A[Archive] -[not]
//    int sort_order;              // o N[name], E[extention] G[dirs first] D[date] S[size] U[unsorted] - reverse
    bool subdirs;                // s
    bool bare;                   // b
    bool lower_case;             // l
    bool display_compress;       // c[h]
    char files[128];
};

static void dir_config_init(struct dir_config *config);
static void dir_config_print(struct dir_config *config);
static bool dir_parse_config(int argc, char* argv[], struct dir_config *config);

const char* pb(bool b)
{
        return b ? "true" : "false";
}

void print_config(const struct dir_config *config)
{
        printf("\tpause=%s\n", pb(config->pause));
        printf("\twide_format=%s\n", pb(config->wide));
//        printf("\tattr=%d\n", config->attr);
//        printf("\tsort_order=%d\n", config->sort_order);
        printf("\tsubdirs=%s\n", pb(config->subdirs));
        printf("\tlower_case=%s\n", pb(config->lower_case));
        printf("\tdisplay_compress=%d\n", config->display_compress);
        printf("%s\n", config->files);
}

int command_dir(int argc, char* argv[]) {
        struct dir_config config;
        bool parsed = dir_parse_config(argc, argv, &config);
        if (parsed) {
                print_config(&config);
        } else {
                printf("Failed parsing command line args\n");
        }
        return EXIT_FAILURE;
}

const char* help_dir() {
    return "Here should be a basic help for dir";
}

static bool dir_parse_config(int argc, char* argv[], struct dir_config *config) {
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
                                    break;
                            case 'e':
                            case 'E':
                                    break;
                            case 'g':
                            case 'G':
                                    break;
                            case 'd':
                            case 'D':
                                    break;
                            case 's':
                            case 'S':
                                    break;
                            case 'u':
                            case 'U':
                                    break;
                            default:
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
                    default:
                            printf("invalid command line switch at index %d/1\n", i);
                    }
                    break;
            default:
                    strncpy(config->files, argv[i], 128);
                    break;
            }
    }

}
static void dir_config_init(struct dir_config *config) {
    config->pause = false;
    config->wide = false;
    config->bare = false;
//    config->compress_ratio = 0; // 1 = yes, 2 = host allocation unit size
    config->lower_case = false;
    config->subdirs = false;
    config->files[0] = 0;
}

static void dir_config_print(struct dir_config *config) {
    printf("pause=%d\n", config->pause);
    printf("wide=%d\n", config->wide);
    printf("base=%d\n", config->bare);
//    printf("compress_ratio=%d\n", config->compress_ratio);
    printf("lowercase=%d\n", config->lower_case);
    printf("subdirs=%d\n", config->subdirs);
    printf("files=%s\n", config->files);
}
