#include "fdbox.h"
#include "lib/applet.h"
#include "lib/args.h"
#include "lib/environ.h"
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
bool test_prompts();

int main(int argc, char *argv[]) {
        bool ok = true;
        ok &= test("applets", test_applets);
        ok &= test("args", test_args);
        ok &= test("strings", test_strings);
        ok &= test("prompts", test_prompts);

        UNUSED(argc);
        UNUSED(argv);
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
bool test_args() {
#define MAX_ARGV 100
        bool ok = true;
        char c2[256], *c3;
        const char *argv[MAX_ARGV];
        size_t argc = 0;
        bool parsing_ok;

        // basic argument parsing - can we create argc/argv?
        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling) */
        memset(c2, 'x', 256);
        c3 = "";
        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.strcpy) */
        strcpy(c2, c3);
        parsing_ok = command_split_args(c2, &argc, argv, MAX_ARGV);
        ok |= parsing_ok && verify_int_equals(0, argc, "no arguments");

        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling) */
        memset(c2, 'x', 256);
        c3 = "asd asd";
        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.strcpy) */
        strcpy(c2, c3);
        parsing_ok = command_split_args(c2, &argc, argv, MAX_ARGV);
        ok |= parsing_ok && verify_int_equals(2, argc, "2 arguments");

        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling) */
        memset(c2, 'x', 256);
        c3 = "111 222 333 44             555 666";
        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.strcpy) */
        strcpy(c2, c3);
        parsing_ok = command_split_args(c2, &argc, argv, MAX_ARGV);
        ok |= parsing_ok && verify_int_equals(6, argc, "6 arguments, with spaces");

        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling) */
        memset(c2, 'x', 256);
        c3 = "dir /w";
        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.strcpy) */
        strcpy(c2, c3);
        parsing_ok = command_split_args(c2, &argc, argv, MAX_ARGV);
        ok |= parsing_ok && verify_int_equals(2, argc, "2 args - dir /w");

        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling) */
        memset(c2, 'x', 256);
        c3 = "dir /w /w";
        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.strcpy) */
        strcpy(c2, c3);
        parsing_ok = command_split_args(c2, &argc, argv, MAX_ARGV);
        ok |= parsing_ok && verify_int_equals(3, argc, "3 args - dir /w /w");

        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling) */
        memset(c2, 'x', 256);
        c3 = "dir /w /2";
        /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.strcpy) */
        strcpy(c2, c3);
        parsing_ok = command_split_args(c2, &argc, argv, MAX_ARGV);
        ok |= parsing_ok && verify_int_equals(3, argc, "3 args - dir /w /2");
        return ok;
}

/////////// applets

/* clang-format off */
int applet1(int argc, char *argv[]) { return EXIT_SUCCESS; UNUSED(argc); UNUSED(argv); }

int applet2(int argc, char *argv[]) { return EXIT_SUCCESS; UNUSED(argc); UNUSED(argv); }

int applet3(int argc, char *argv[]) { return EXIT_SUCCESS; UNUSED(argc); UNUSED(argv); }
/* clang-format on */

bool test_applets() {
        /* clang-format off */
        struct applet commands[] = {
                {NULL, &applet1, "applet1"},
                {NULL, &applet2, "applet2"},
                {NULL, NULL, NULL}
        };
        /* clang-format on */
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
        ok &= verify_string_equals(c, "null", "normal lower");
        c = str_to_lower(strcpy(str, "qwertyuiopQWERTYUIOP"));
        ok &= verify_string_equals(c, "qwertyuiopqwertyuiop", "normal lower");
        c = str_to_lower(strcpy(str, ""));
        ok &= verify_string_equals(c, strcpy(str, ""), "lower to empty string");
        c = str_to_lower(strcpy(str, "123456"));
        ok &= verify_string_equals(c, "123456", "numbers");

        return ok;
}

bool test_str_prefix() {
        const char *c;
        char str[100];
        bool ok = true;

        c = strcpy(str, "Lorem Ipsum");
        ok &= verify_int_equals(str_is_prefix(c, "L"), 1, "string starts with string (1)");
        ok &= verify_int_equals(str_is_prefix(c, "Lorem"), 1, "string starts with string (5)");
        ok &= verify_int_equals(str_is_prefix(c, "XXX"), 0, "string does not start with string");
        ok &= verify_int_equals(str_is_prefix(c, "Lorem Ipsum"), 1, "string starts with itself");
        ok &= verify_int_equals(str_is_prefix(c, ""), 1, "string starts with itself");

        return ok;
}

bool test_str_char_suffix() {
        bool ok = true;

        ok &= verify_int_equals(str_ends_with("123", '3'), 1, "String ends (digit)");
        ok &= verify_int_equals(str_ends_with("3", '3'), 1, "String ends (digit)");
        ok &= verify_int_equals(str_ends_with("333", '3'), 1, "String ends (digit)");
        ok &= verify_int_equals(str_ends_with("aaa", 'a'), 1, "String ends (alpha)");
        ok &= verify_int_equals(str_ends_with("cba", 'a'), 1, "String ends (alpha)");
        ok &= verify_int_equals(str_ends_with("a", 'a'), 1, "String ends (alpha)");
        ok &= verify_int_equals(str_ends_with("A", 'a'), 0, "String not ends (alpha)");
        ok &= verify_int_equals(str_ends_with("Aa-", 'a'), 0, "String not ends (alpha)");
        ok &= verify_int_equals(str_ends_with("", 'a'), 0, "String not ends (alpha)");
        return ok;
}

bool test_file_basename() {
        char str[100];
        const char *c;
        bool ok = true;

        strcpy(str, "file.txt");
        c = file_base_name(str);
        ok &= verify_string_equals(c, "file.txt", "full file name");

        strcpy(str, "c:\\windows\\drivers\\file.txt");
        c = file_base_name(str);
        ok &= verify_string_equals(c, "file.txt", "windows path");

        strcpy(str, "\\windows\\drivers\\file.txt");
        c = file_base_name(str);
        ok &= verify_string_equals(c, "file.txt", "windows path (no drive)");

        strcpy(str, "windows\\drivers\\file.txt");
        c = file_base_name(str);
        ok &= verify_string_equals(c, "file.txt", "windows path (relative)");

        strcpy(str, "..\\..\\windows\\file.txt");
        c = file_base_name(str);
        ok &= verify_string_equals(c, "file.txt", "windows path (relative, parent)");

        strcpy(str, "/tmp/file.txt");
        c = file_base_name(str);
        ok &= verify_string_equals(c, "file.txt", "unix path");

        strcpy(str, "tmp/file.txt");
        c = file_base_name(str);
        ok &= verify_string_equals(c, "file.txt", "unix path (relative)");

        strcpy(str, "../../../tmp/file.txt");
        c = file_base_name(str);
        ok &= verify_string_equals(c, "file.txt", "unix path (relative, parent)");

        strcpy(str, "c:/test/messedup/..\\windows.exe.manifest");
        c = file_base_name(str);
        ok &= verify_string_equals(c, "windows.exe.manifest", "mixed path (unix/windows, parent)");

        return ok;
}

bool test_file_extensions() {
        char str[100];
        const char *c;
        bool ok = true;

        strcpy(str, "file.txt");
        c = file_get_extesnsion(str);
        ok &= verify_string_equals(c, "txt", "getting normal 8.3 extention");

        strcpy(str, "/var/lib/file.txt");
        c = file_get_extesnsion(str);
        ok &= verify_string_equals(c, "txt", "getting normal 8.3 extention + path");

        strcpy(str, "/var/lib/file.text");
        c = file_get_extesnsion(str);
        ok &= verify_string_equals(c, "text", "getting normal long extention + path");

        strcpy(str, "/var/lib/file.txt.text.file.blabla");
        c = file_get_extesnsion(str);
        ok &= verify_string_equals(c, "blabla", "getting very long extention + path");

        return ok;
}

bool test_strings() {
        bool ok = true;
        ok &= test_string_lower();
        ok &= test_str_prefix();
        ok &= test_str_char_suffix();
        ok &= test_file_basename();
        ok &= test_file_extensions();
        return ok;
}

bool test_prompt_letter(char l, char k) {
        bool ok = true;
        char p1[16], p2[16];
        char message[100];
        int i;

        snprintf(p1, 16, "$%c", l);
        get_prompt(p1, p2, 256);
        i = strlen(p2);
        snprintf(message, 100, "prompt size for $%c", l);
        ok &= verify_int_equals(i, 1, message);
        snprintf(message, 100, "char $%c = %d", l, k);
        ok &= verify_int_equals(p2[0], k, message);
        return ok;
}

bool test_prompts() {
        bool ok = true;
        char prompt[100], message[200];

        ok &= test_prompt_letter('$', '$');
        ok &= test_prompt_letter('_', '\n');
        ok &= test_prompt_letter('a', '&');
        ok &= test_prompt_letter('b', '|');
        ok &= test_prompt_letter('c', '(');
        ok &= test_prompt_letter('e', 27);
        ok &= test_prompt_letter('g', '>');
        ok &= test_prompt_letter('h', 8);
        ok &= test_prompt_letter('l', '<');
        ok &= test_prompt_letter('q', '=');
        ok &= test_prompt_letter('s', ' ');

        /* TODO */
        /* date */
        /* time */
        /* drive */

        get_prompt("$V", prompt, 100);
        ok &= verify_int_equals(strlen(prompt), strlen(FDBOX_VERSION_STR),
                                "testing prompt version length");
        snprintf(message, 200, "testing for prompt version, get %s, expected %s", prompt,
                 FDBOX_VERSION_STR);
        ok &= verify_string_equals(prompt, FDBOX_VERSION_STR, message);

        return ok;
}
