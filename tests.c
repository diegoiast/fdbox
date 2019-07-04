
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lib/args.h"
#include "fdbox.h"

typedef bool (*function_test)();

void test(const char* message, function_test unittest);
bool test_args();

int main(int argc, char *argv[])
{
    test("args", test_args);
}

void test(const char* message, function_test unittest) {

    printf("Running test for %s: ", message);
    if (!unittest()) {
        printf(" - FAIL\n");
        exit(1);
    }
    printf("OK.\n");
}

bool test_args() {
    return true;
}
