/*
This file is part of fdbox
For license - read license.txt
*/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/applet.h"
#include "lib/args.h"
#include "lib/environ.h"
#include "lib/strextra.h"

#include "dos/prompt.h"

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#include "lib/tc202/stdextra.h"
#endif

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__)
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>            
#endif

#ifdef __WIN32__
#include <io.h>
#include <conio.h>
#include <stdbool.h>
#include <windows.h>
#endif

struct command_shell_config {
        struct command_config global;
};

static void command_shell_config_init(struct command_shell_config *config);
static bool command_shell_config_parse(int argc, char *argv[], struct command_shell_config *config);
static void command_shell_config_print(const struct command_shell_config *config);
static void command_shell_print_extended_help();

size_t read_line(char line[], int max_size);

/* TODO - I am unsure if this is the best way to tell the main loop
 * we should exit. For now it works
 */

int command_execute_line(const char *line) {
        struct command_args args;
        struct applet *cmd;
        int code;
        extern struct applet commands[];

        /* this function will not modify the args, so its marked `const
         * but some commands (date/time) will modify the args instead of making copies
         * this is OK for now */

        if (command_args_split(line, &args)) {
                fprintf(stderr, "Command line parsing failed\n");
                return EXIT_SUCCESS;
        }

        if (args.argc == 0) {
                command_args_free(&args);
                return EXIT_SUCCESS;
        }

        /* Special handling for exit, as it should break the main loop */
        if (strcasecmp(args.argv[0], "exit") == 0) {
                command_args_free(&args);
                return EXIT_FAILURE;
        }

        cmd = find_applet(CASE_INSENSITVE, args.argv[0], commands);
        if (cmd != NULL) {
                code = cmd->handler(args.argc, args.argv);
                errno = code;
                if (code != EXIT_SUCCESS) {
                        fprintf(stderr, "Command failed (%d)\n", code);
                }
        } else {
                fprintf(stderr, "Command not found - [%s][%s]\n", args.argv[0], line);
                errno = ENOENT;
        }

        command_args_free(&args);
        return EXIT_SUCCESS;
}

int command_command(int argc, char *argv[]) {
        char line[1024], *pos;
        int code;
        struct command_shell_config config;

        command_shell_config_init(&config);
        command_shell_config_parse(argc, argv, &config);
        /* command_shell_config_print(&config); */

        if (config.global.show_help) {
                command_shell_print_extended_help();
                return EXIT_SUCCESS;
        }

        do {
                char prompt[256];
                const char *t;

                t = getenv("PROMPT");
                if (t == NULL) {
                        command_prompt(1, NULL);
                        t = getenv("PROMPT");
                }
                get_prompt(t, prompt, 256);
                printf("%s", prompt);
                read_line(line, 1024);

                if ((pos = strchr(line, '\n')) != NULL) {
                        *pos = '\0';
                }
                code = command_execute_line(line);
                if (code != EXIT_SUCCESS) {
                        break;
                }
        } while (true);
        return EXIT_SUCCESS;
}

const char *help_command() { return "Runs an MS-DOS interactive shell"; }

/* internal API, all functions bellow should be static */
static void command_shell_config_init(struct command_shell_config *config) {
        command_config_init(&config->global);
}

static bool command_shell_config_parse(int argc, char *argv[],
                                       struct command_shell_config *config) {
        int c1, c2;
        do {
                c1 = command_config_parse(argc, argv, &config->global);
                c2 = tolower(c1);
                switch (c2) {
                case ARG_PROCESSED:
                case ARG_DONE:
                        break;
                default:
                        return false;
                }
        } while (c1 >= 0);
        return true;
}

static void command_shell_config_print(const struct command_shell_config *config) {
        command_config_print(&config->global);
}

static void command_shell_print_extended_help() {
        printf("%s\n", help_command());

        printf("   command {shell command} /l\n");
        printf("   Runs an interactive shell \n");
        printf("   TODO: properly implement the command.com swithces \n");
}


/* read line */
bool is_interactive()
{
        return isatty(fileno(stdin));
}

int read_line_simple(char line[], int max_size) {
        line[0] = 0;
        fgets(line, max_size, stdin);
        return strlen(line);
}

#define KEY_ARROW_LEFT      0xff4b
#define KEY_ARROW_RIGHT     0xff4d
#define KEY_ARROW_UP        0xff48
#define KEY_ARROW_DOWN      0xff50
#define KEY_HOME            0xff47
#define KEY_END             0xff4f
#define KEY_PGDOWN          0xff49
#define KEY_PGUP            0xff51

#if defined(__WIN32__)
int get_char_win32() {
        DWORD cc;
        HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

        // console not found
        if (h == NULL) {
                return -1;
        }

        for (;;) {
                INPUT_RECORD irec;
                ReadConsoleInput(h, &irec, 1, &cc);

                if (irec.EventType == KEY_EVENT && irec.Event.KeyEvent.bKeyDown) {
                        if (irec.Event.KeyEvent.uChar.AsciiChar != 0) {
                                return irec.Event.KeyEvent.uChar.AsciiChar;
                        }
                        else {
                                switch (irec.Event.KeyEvent.wVirtualScanCode) {
                                        case KEY_ARROW_LEFT & 0x00ff:
                                                return KEY_ARROW_LEFT;
                                        case KEY_ARROW_RIGHT & 0x00ff:
                                                return KEY_ARROW_RIGHT;
                                        case KEY_ARROW_UP & 0x00ff:
                                                return KEY_ARROW_UP;
                                        case KEY_ARROW_DOWN & 0x00ff:
                                                return KEY_ARROW_DOWN;
                                        case KEY_HOME & 0x00ff:
                                                return KEY_HOME;
                                        case KEY_END & 0x00ff:
                                                return KEY_END;
                                        case KEY_PGDOWN & 0x00ff:
                                                return KEY_PGDOWN;
                                        case KEY_PGUP & 0x00ff:
                                                return KEY_PGUP;
                                }
                        }
/*
                //&& ! ((KEY_EVENT_RECORD&)irec.Event).wRepeatCount )
                        printf("*** Pressed %d,%c  unicode=%x, scancode=%x, keycode=%x, flags=%x\n",
                               irec.Event.KeyEvent.uChar.AsciiChar,
                               irec.Event.KeyEvent.uChar.AsciiChar,
                               irec.Event.KeyEvent.uChar.UnicodeChar,
                               irec.Event.KeyEvent.wVirtualScanCode,
                                irec.Event.KeyEvent.wVirtualKeyCode,
                                irec.Event.KeyEvent.dwControlKeyState
                        );
                        return irec.Event.KeyEvent.uChar.AsciiChar;
                        */
                }
        }
        return -1;
}
#define get_char_impl get_char_win32

#elif defined(_POSIX_C_SOURCE) || defined(__APPLE__)
int get_char_posix() {
        int i = getchar();        
        switch (i) {
        case '\033':
                i = getchar();
                if (i == '[') {
                        i = getchar();
                        switch (i) {
                        case 'A':
                                return KEY_ARROW_UP;
                        case 'B':
                                return KEY_ARROW_DOWN;
                        case 'C':
                                return KEY_ARROW_RIGHT;
                        case 'D':
                                return KEY_ARROW_LEFT;
                        case 'H':
                                return KEY_HOME;
                        case 'F':
                                return KEY_END;
                        case '6':
                                return KEY_PGDOWN;
                        case '5':
                                return KEY_PGUP;
                        default:
                                printf("read escape code: %d\n", i);
                        }
                }
                return 0;
        case '\177':
                return '\b';
        default:
                return i;
        }
}
#define get_char_impl get_char_posix

#elif defined(__TURBOC__)

int get_char_dos()
{
        int c = getch();
        
        /* extended ASCII FTW */
        if (c == 0) {
                c = getch();
                switch (c) {
                case KEY_ARROW_LEFT & 0x00ff:
                        return KEY_ARROW_LEFT;
                case KEY_ARROW_RIGHT & 0x00ff:
                        return KEY_ARROW_RIGHT;
                case KEY_ARROW_UP & 0x00ff:
                        return KEY_ARROW_UP;
                case KEY_ARROW_DOWN & 0x00ff:
                        return KEY_ARROW_DOWN;
                case KEY_HOME & 0x00ff:
                        return KEY_HOME;
                case KEY_END & 0x00ff:
                        return KEY_END;
                case KEY_PGDOWN & 0x00ff:
                        return KEY_PGDOWN;
                case KEY_PGUP & 0x00ff:
                        return KEY_PGUP;
                default: 
                        return 0;
                }
        }
        return c;
}

#define get_char_impl get_char_dos
#else
#error Undefined platform - we cannot read a line on a terminal
TODO - it seems that this platform is not supported yet - you need to define a function
"get_char_my_platform()" that reads a char without enter beeing pressed. Then

define get_char_impl get_char_my_platform

#endif


void move_cursor_back(size_t n) {
        int i;
        for (i=0; i < n; i++) {
                putchar('\b');
        }        
}

int read_line_console(char *line, size_t max_size) {
        size_t index = 0;
        size_t size = 0;
        
        line[0] = 0;

        while (index < max_size) {
                int c = get_char_impl();

                switch (c) {
                case '\r':
                case '\n':
                        putchar('\n');
                        line[size] = 0;
                        return size;
                case KEY_HOME:
                        move_cursor_back(index);
                        index = 0;
                        fflush(stdout);
                        break;
                case KEY_END:
                        while (line[index] != '\0') {
                                putchar(line[index]);
                                index++;
                        }
                        fflush(stdout);
                        break;
                case KEY_ARROW_LEFT:
                        if (index > 0) {
                                index --;
                                putchar('\b');
                        }
                        break;
                case KEY_ARROW_RIGHT: {
                        char next = line[index];
                        if (index < max_size  && next != '\0') {
                                putchar(line[index]);
                                index ++;
                        }
                        break;
                }
                case '\b': {
                        size_t line_length, i;
                        str_del_char(line, index-1);
                        move_cursor_back(index);
                        printf("%s ",line);                        
                        line_length = strlen(line);
                        move_cursor_back(line_length+1);
                        index --;
                        if (index > line_length) {
                                index = line_length;
                        }
                        for (i = 0; i < index; i++) {
                                putchar(line[i]);
                        }
                        fflush(stdout);
                        break;
                }
                default:
                        /* Don't pass arrows, we nuked non-ascii... which is another problem */
                        if (c <= 0xff) {
                                line[index] = (char)c;
                                putchar(c);
                                index++;
                                if (index > size) {
                                        size = index;
                                }
                                line[size] = 0;
                        }
                }
        }
        return size;
}

size_t read_line(char line[], int max_size) {
        int l;
        if (!is_interactive()) {
                return read_line_simple(line, max_size);
        }
        l = read_line_console(line, max_size);
        if (l < 0) {
                l = read_line_simple(line, max_size);
        }
        return l;
}

/* https://stackoverflow.com/a/1798833 */
void setup_terminal()
{
#ifdef _POSIX_C_SOURCE        
        static struct termios oldt, newt;
        tcgetattr( STDIN_FILENO, &oldt);
        newt = oldt;        
        newt.c_lflag &= ~(ICANON | ECHO);          
        tcsetattr( STDIN_FILENO, TCSANOW, &newt);
#endif
}

void restore_terminal()
{
#ifdef _POSIX_C_SOURCE        
        static struct termios oldt, newt;
        tcgetattr( STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag |= (ICANON | ECHO);          
        tcsetattr( STDIN_FILENO, TCSANOW, &newt);
#endif        
}
