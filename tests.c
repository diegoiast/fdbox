
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lib/args.h"
#include "lib/applet.h"
#include "fdbox.h"

typedef bool (*function_test)();

bool test(const char* message, function_test unittest);
bool test_args();
bool test_applets();

int main(int argc, char *argv[])
{
    bool ok = true;
    ok &= test("args", test_args);
    ok &= test("applets", test_applets);
//     test("args", test_args);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool test(const char* message, function_test unittest) {
    printf("Running test for %s:\n", message);
    if (!unittest()) {
        printf(" * %s -  FAIL\n", message);
        return false;
    }
    return true;
}

bool test_args() {
    return false;
}


//////////////////////////

bool verify_ptr_equals(void *arg1, void* arg2, const char * message) {
    printf(" * Checking %s: ", message);
    if (arg1 == arg2) {
        printf("OK\n");
        return true;
    }
    printf("FAIL (expecting %p, got %p)\n", arg1, arg2);
    return false;
}

/////////// applets
int applet1(int arc, char* argv[]) {
    return EXIT_SUCCESS;
}

int applet2(int arc, char* argv[]) {
    return EXIT_SUCCESS;
}

int applet3(int arc, char* argv[]) {
    return EXIT_SUCCESS;
}

bool test_applets() {
    struct applet commands[] = {
        { NULL,  &applet1, "applet1" },
        { NULL,  &applet2, "applet2" },
        { NULL,  NULL, NULL }
    };
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
    
