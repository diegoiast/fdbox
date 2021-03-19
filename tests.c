
#include "fdbox.h"
#include "lib/applet.h"
#include "lib/args.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef bool (*function_test)();

bool test(const char *message, function_test unittest);
bool test_args();
bool test_applets();

int main(int argc, char *argv[]) {
        bool ok = true;
        ok &= test("applets", test_applets);
        ok &= test("args", test_args);
        //     test("args", test_args);
        return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool test(const char *message, function_test unittest) {
        printf("Running test for %s:\n", message);
        if (!unittest()) {
                printf(" * %s -  FAIL\n", message);
                return false;
        }
        return true;
}

//////////////////////////
// assert like functions.
// TODO : if we convert this to a macro, we will have line numbers

bool verify_ptr_equals(void *arg1, void *arg2, const char *message) {
        printf(" * Checking %s: ", message);
        if (arg1 == arg2) {
                printf("OK\n");
                return true;
        }
        printf("FAIL (expecting %p, got %p)\n", arg1, arg2);
        return false;
}

bool verify_int_equals(int arg1, int arg2, const char *message) {
        printf(" * Checking %s: ", message);
        if (arg1 == arg2) {
                printf("OK\n");
                return true;
        }
        printf("FAIL (expecting %d, got %d)\n", arg1, arg2);
        return false;
}

///////////////////////////////////////////
// Argumnents

int split_strings(char *c2, char *argv[][100]) {
        int argc = 0;
        char *token;
        while ((token = strsep(&c2, " "))) {
                if (*token == '\0') {
                        continue;
                }
                (*argv)[argc] = token;
                argc++;
        }
        return argc;
}

bool test_args() {
        bool ok = true;
        char c2[256], *c3;
        char *argv[100];
        int argc = -1, r;

        // basic argument parsing - can we create argc/argv?
        argc = split_strings(strcpy(c2, ""), &argv);
        ok |= verify_int_equals(0, argc, "no arguments");

        memset(c2, 'x', 256);
        argc = split_strings(strcpy(c2, "asd asd"), &argv);
        ok |= verify_int_equals(2, argc, "2 arguments");

        memset(c2, 'x', 256);
        argc = split_strings(strcpy(c2, "111 222 333 44             555 666"), &argv);
        ok |= verify_int_equals(6, argc, "6 arguments");

        memset(c2, 'x', 256);
        argc = split_strings(strcpy(c2, "dir /w"), &argv);
        ok |= verify_int_equals(2, argc, "2 arguments");

        memset(c2, 'x', 256);
        argc = split_strings(strcpy(c2, "dir /w /w"), &argv);
        ok |= verify_int_equals(3, argc, "3 arguments");

        memset(c2, 'x', 256);
        argc = split_strings(strcpy(c2, "dir /w /e"), &argv);
        ok |= verify_int_equals(3, argc, "3 arguments");

        // let the fun begin
        argc = split_strings(strcpy(c2, "dir /w"), &argv);
        //     print_agrs(argc, argv);
        hexDump("argc - 1", c2, 50);

        //     DEBUG_LINE;
        //     print_agrs(argc, argv);
        // TODO - I am unhappy about this typocast
        while ((r = dos_parseargs(&argc, (char ***)&argv, "w", &c3)) >= 0) {
                if (r != 'w') {
                        ok = false;
                        printf(" * Checking %c: FAIL\n", 'w');
                }
        }
        if (ok) {
                printf(" * Checking single arg: OK\n");
        }

        argc = split_strings(strcpy(c2, "dir /w /e"), &argv);
        print_agrs(argc, argv);
        hexDump("argc - 2", c2, 50);
        return ok;
        //     print_agrs(argc, argv);
        while ((r = dos_parseargs(&argc, (char ***)&argv, "we", &c3)) >= 0) {
                if (r != 'w' || r != 'e') {
                        ok = false;
                        printf(" * Checking 2 args: FAIL\n");
                }
        }
        if (ok) {
                printf(" * Checking double arg: OK\n");
        }

        return ok;
}

/////////// applets
int applet1(int arc, char *argv[]) { return EXIT_SUCCESS; }

int applet2(int arc, char *argv[]) { return EXIT_SUCCESS; }

int applet3(int arc, char *argv[]) { return EXIT_SUCCESS; }

bool test_applets() {
        struct applet commands[] = {
            {NULL, &applet1, "applet1"}, {NULL, &applet2, "applet2"}, {NULL, NULL, NULL}};
        bool ok = true;
        struct applet *c;

        c = find_applet("applet1", commands);
        ok &= verify_ptr_equals(c->handler, applet1, "applet 1 available");
        c = find_applet("applet2", commands);
        ok &= verify_ptr_equals(c->handler, applet2, "applet 2 available");
        c = find_applet("applet3", commands);
        ok &= verify_ptr_equals(c, NULL, "applet 3 un-available");
        return ok;
}
