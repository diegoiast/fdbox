#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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
    char pause;
    char wide;
    char bare;
    char compress_ratio; //  0 = no // 1 = yes, 2 = host allocation unit size
    char lowercase;
    char subdirs;
    const char* files;
};
void dir_config_init(struct dir_config *config);
void dir_config_print(struct dir_config *config);
void print_unimplemented(const char* arg);

int command_dir(int argc, char* argv[]) {
    char *c;
    int r;
    struct dir_config config;

    dir_config_init(&config);
    // dir_config_print(&config);

    argc = 1; // ???
    while ((r = dos_parseargs(&argc, &argv, "pwa[drhsa-]o[nsedgc-]sblc[h]", &c)) >= 0)  {
//         printf(" dos_parseargs = %c (%d)\n", r < 255? r: '?', r);
        switch (r) {
        case 'a':
            while (c != NULL) {
                char reverse = false;
                if (*c == '-') {
                    reverse = true;
                    c++;
                }
                switch (*c) {
                case 'd':
                    /* code */
                    break;
                case 'r':
                    /* code */
                    break;
                case 'h':
                    /* code */
                    break;
                case 's':
                    /* code */
                    break;
                case 'a':
                    /* code */
                    break;                
                default:
                    break;
                }
                c++;
            }
            break;
        case 'b':
            config.bare = true;
            break;
        case 'c':
            if (c != NULL && *c == 'h') {
                config.compress_ratio = 2;
            } else {
                config.compress_ratio = 1;
            }
            break;
        case 'l':
            config.lowercase = true;
            break;
        case 'o':
            while (c != NULL) {
                char reverse = false;
                if (*c == '-') {
                    reverse = true;
                    c++;
                }
                switch (*c) {
                case 'n':
                    /* code */
                    break;
                case 's':
                    /* code */
                    break;
                case 'e':
                    /* code */
                    break;
                case 'd':
                    /* code */
                    break;
                case 'g':
                    /* code */
                    break;                
                case 'c':
                    /* code */
                    break;                
                default:
                    break;
                }
                c++;
            }
            break;
        case 's':
            config.subdirs = true;
            break;
        case 'p':
            config.pause = true;
            break;
        case 'w':
            config.wide = true;
            break;
    
        case ARG_STRING:
            config.files = c;
            break;
        case ARG_EXTRA:
            printf("Unknown argument suffix - %s\n",  c);
            help_dir();
            return EXIT_FAILURE;
        case ARG_NOT_EXISTING:
//         default:
            printf("Unknown argument %s\n", c);
            help_dir();
            return EXIT_FAILURE;
        }
    }
    dir_config_print(&config);
    return EXIT_FAILURE;
}

void print_unimplemented(const char* arg) {
    printf("Argument %s is still not implemented", arg);
}

void dir_config_init(struct dir_config *config) {
    config->pause = false;
    config->wide = false;
    config->bare = false;
    config->compress_ratio = 0; // 1 = yes, 2 = host allocation unit size
    config->lowercase = false;
    config->subdirs = false;
    config->files = NULL;
}

void dir_config_print(struct dir_config *config) {
    printf("pause=%d\n", config->pause);
    printf("wide=%d\n", config->wide);
    printf("base=%d\n", config->bare);
    printf("compress_ratio=%d\n", config->compress_ratio);
    printf("lowercase=%d\n", config->lowercase);
    printf("subdirs=%d\n", config->subdirs);
    printf("files=%s\n", config->files);
}

const char* help_dir() {
    return "Here should be a basic help for dir";
}

