#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fdbox.h"
#include "lib/nls.h"
#include "unix/cal.h"

/*
This file is part of fdbox
For license - read license.txt
*/

static int is_leap(int year) { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); }

static int days_in_month(int month, int year) {
        static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month == 2 && is_leap(year)) {
                return 29;
        }
        return days[month - 1];
}

/* Day of week of a date, 0 = Sunday (Sakamoto's method). */
static int day_of_week(int year, int month, int day) {
        static const int shift[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
        if (month < 3) {
                year -= 1;
        }
        return (year + year / 4 - year / 100 + year / 400 + shift[month - 1] + day) % 7;
}

/* cal [month [year]] - the current month by default. */
int command_cal(int argc, char *argv[]) {
        static const char *const names[] = {N_("January"), N_("February"), N_("March"),
                                            N_("April"),   N_("May"),      N_("June"),
                                            N_("July"),    N_("August"),   N_("September"),
                                            N_("October"), N_("November"), N_("December")};
        int month, year, first, count, column, day;
        time_t now;
        struct tm *local;

        now = time(NULL);
        local = localtime(&now);
        month = local->tm_mon + 1;
        year = local->tm_year + 1900;

        if (argc >= 2) {
                month = atoi(argv[1]);
        }
        if (argc >= 3) {
                year = atoi(argv[2]);
        }
        if (month < 1 || month > 12 || year < 1) {
                fprintf(stderr, _("cal: invalid month or year\n"));
                return EXIT_FAILURE;
        }

        first = day_of_week(year, month, 1);
        count = days_in_month(month, year);

        printf("    %s %d\n", _(names[month - 1]), year);
        fputs(_("Su Mo Tu We Th Fr Sa\n"), stdout);
        for (column = 0; column < first; column++) {
                fputs("   ", stdout);
        }
        for (day = 1; day <= count; day++) {
                printf("%2d ", day);
                column++;
                if (column % 7 == 0) {
                        fputs("\n", stdout);
                }
        }
        if (column % 7 != 0) {
                fputs("\n", stdout);
        }
        return EXIT_SUCCESS;
}

const char *help_cal() { return _("Print a month calendar (cal [month [year]])"); }
