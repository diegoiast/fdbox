#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dos/date.h"
#include "fdbox.h"

/*
This file is part of fdbox
For license - read license.txt
*/

#ifdef _POSIX_C_SOURCE
#include <stdbool.h>
#include <sys/time.h>
#endif

#ifdef __WIN32__
#include <stdbool.h>
#include <sys/time.h>
#endif

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#include <dos.h>
#endif

struct date_config {
        bool show_help;
        bool interactive;
        char *new_date;
};

static void date_config_init(struct date_config *config);
static bool date_config_parse(int argc, char *argv[], struct date_config *config);
static void date_config_print(struct date_config *config);
static int date_set_new_date(char *new_date);
static void date_print_date();
static void date_print_extended_help();

static const char *pb(bool b);

int command_date(int argc, char *argv[]) {
        struct date_config config;
        date_config_init((&config));
        if (!date_config_parse(argc, argv, &config)) {
                date_print_extended_help();
                return EXIT_FAILURE;
        }
        /* date_config_print(&config); */

        if (config.show_help) {
                date_print_extended_help();
                return EXIT_SUCCESS;
        }
        if (config.new_date != NULL) {
                return date_set_new_date(config.new_date);
        }
        date_print_date();
        if (config.interactive) {
                char new_date[128];
                printf("Enter new date (mm-dd-[yy]yy): ");
                fgets(new_date, 128, stdin);
                return date_set_new_date(new_date);
        }
        return EXIT_SUCCESS;
}

const char *help_date() { return "Here should be a basic help for date"; }

static void date_config_init(struct date_config *config) {
        config->show_help = false;
        config->interactive = true;
        config->new_date = NULL;
}

static bool date_config_parse(int argc, char *argv[], struct date_config *config) {
        size_t i;

        for (i = 1; i < (size_t)argc; i++) {
                char c1, c2;
                c1 = argv[i][0];
                switch (c1) {
                case ARGUMENT_DELIMIER:
                        c2 = argv[i][1];
                        switch (c2) {
                        case 'd':
                        case 'D':
                                config->interactive = false;
                                break;
                        case 'h':
                        case 'H':
                        case '?':
                                config->show_help = true;
                                break;
                        default:
                                return false;
                        }
                        break;
                default:
                        config->new_date = argv[i];
                        config->interactive = false;
                }
        }
        return true;
}

static void date_config_print(struct date_config *config) {
        printf("\tShow help = %s\n", pb(config->show_help));
        printf("\tInteractive = %s\n", pb(config->interactive));
        printf("\tnew date = %s\n", config->new_date ? config->new_date : "NULL");
}

static int date_set_new_date(char *new_date) {
        int year = -1, month = -1, day = -1;
        char *token;

        token = strtok(new_date, "-");
        month = token != NULL ? atoi(token) : -1;
        token = strtok(NULL, "-");
        day = token != NULL ? atoi(token) : -1;
        token = strtok(NULL, "-");
        year = token != NULL ? atoi(token) : -1;

        if (year <= 0 || month <= 0 || day <= 0) {
                return EXIT_FAILURE;
        }
        /*        printf("TODO: set the date to %02d-%02d-%4d\n", day, month, year); */

#if defined(_POSIX_C_SOURCE) || defined(__WIN32__)
        {
                time_t new_time;
                struct tm tm;
                struct timeval tv;
                new_time = time(NULL);
                tm = *localtime(&new_time);
                tm.tm_year = year - 1900;
                tm.tm_mon = month - 1;
                tm.tm_mday = day;
                new_time = mktime(&tm);
                tv.tv_sec = new_time;
                tv.tv_usec = 0;
                return settimeofday(&tv, NULL);
        }
#endif

#if defined(__MSDOS__)
        {
                struct date the_date;
                the_date.da_year = year;
                the_date.da_mon = month;
                the_date.da_day = day;
                setdate(&the_date);
        }
#endif
        return EXIT_SUCCESS;
}

static void date_print_date() {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        printf("Current date is %02d-%02d-%4d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

static void date_print_extended_help() {
        printf("%s\n", help_date());
        printf("        date /d [new date]\n");
        printf("        /d displays date and exits\n\n");
        printf("If you pass a date to this command, it will just set it\n");
        printf("and not be interactive. If you don't - the command will\n");
        printf("prompt you for a new date\n");
}

static const char *pb(bool b) { return b ? "true" : "false"; }
