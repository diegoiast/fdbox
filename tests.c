
#include "fdbox.h"
#include "lib/applet.h"
#include "lib/args.h"
#include "lib/strextra.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef bool (*function_test)();

bool test(const char *message, function_test unittest);
bool test_args();
bool test_applets();
bool test_strings();

int main(int argc, char *argv[]) {
        bool ok = true;
        ok &= test("applets", test_applets);
//        ok &= test("args", test_args);
        ok &= test("strings", test_strings);
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

bool verify_string_equals(const char *arg1, const char *arg2, const char *message) {
        printf(" * Checking %s: ", message);
        if (strcmp(arg1, arg2) == 0) {
                printf("OK\n");
                return true;
        }
        printf("FAIL (expecting %s, got %s)\n", arg1, arg2);
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

// UNused yes
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


bool test_string_lower() {
        const char *c;
        char str[100];
        bool ok = true;

        c = str_to_lower(strcpy(str, "NULL"));
        ok &= verify_string_equals(c, "null", "Testing normal lower");
        c = str_to_lower(strcpy(str,"qwertyuiopQWERTYUIOP"));
        ok &= verify_string_equals(c, "qwertyuiopqwertyuiop", "Testing normal lower");
        c = str_to_lower(strcpy(str, ""));
        ok &= verify_string_equals(c, strcpy(str, ""), "Testing lower to empty string");
        c = str_to_lower(strcpy(str, "123456"));
        ok &= verify_string_equals(c, "123456", "Testing numbers");

        return ok;
}

bool test_str_prefix() {
        const char *c;
        char str[100];
        bool ok = true;

        c = strcpy(str, "Lorem Ipsum");
        ok &= verify_int_equals(str_is_prefix(c, "L"), 1, "Testing string starts with string (1)");
        ok &= verify_int_equals(str_is_prefix(c, "Lorem"), 1, "Testing string starts with string (5)");
        ok &= verify_int_equals(str_is_prefix(c, "XXX"), 0, "Testing string does not start with string");
        ok &= verify_int_equals(str_is_prefix(c, "Lorem Ipsum"), 1, "Testing string starts with itself");
        ok &= verify_int_equals(str_is_prefix(c, ""), 1, "Testing string starts with itself");

        return ok;
}

bool test_strings() {
        bool ok = true;
        ok &= test_string_lower();
        ok &= test_str_prefix();
        return ok;
}
