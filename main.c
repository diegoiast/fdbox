#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fdbox.h"

#include "dos/beep.h"
#include "dos/call.h"
#include "dos/cd.h"
#include "dos/cls.h"
#include "dos/copy.h"
#include "dos/date.h"
#include "dos/dir.h"
#include "dos/echo.h"
#include "dos/exit.h"
#include "dos/for.h"
#include "dos/goto.h"
#include "dos/if.h"
#include "dos/mv.h"
#include "dos/path.h"
#include "dos/prompt.h"
#include "dos/pwd.h"
#include "dos/reboot.h"
#include "dos/rem.h"
#include "dos/set.h"
#include "dos/shift.h"
#include "dos/time.h"
#include "dos/type.h"
#include "unix/cal.h"
#include "unix/head.h"
#include "unix/hexdump.h"
#include "unix/tail.h"
#include "./help.h"

struct applet commands[] = {
    { &help_beep,  &command_beep, "beep" },
    { &help_call,  &command_call, "call" },
    { &help_cd,  &command_cd, "cd" },
    { &help_cls,  &command_cls, "cls" },
    { &help_copy,  &command_copy, "copy" },
    { &help_date,  &command_date, "date" },
    { &help_dir,  &command_dir, "dir" },
    { &help_echo,  &command_echo, "echo" },
    { &help_exit,  &command_exit, "exit" },
    { &help_for,  &command_for, "for" },
    { &help_goto,  &command_goto, "goto" },
    { &help_if,  &command_if, "if" },
    { &help_mv,  &command_mv, "mv" },
    { &help_path,  &command_path, "path" },
    { &help_prompt,  &command_prompt, "prompt" },
    { &help_pwd,  &command_pwd, "pwd" },
    { &help_reboot,  &command_reboot, "reboot" },
    { &help_rem,  &command_rem, "rem" },
    { &help_set,  &command_set, "set" },
    { &help_shift,  &command_shift, "shift" },
    { &help_time,  &command_time, "time" },
    { &help_type,  &command_type, "type" },
    { &help_cal,  &command_cal, "cal" },
    { &help_head,  &command_head, "head" },
    { &help_hexdump,  &command_hexdump, "hexdump" },
    { &help_tail,  &command_tail, "tail" },
    { &help_help,  &command_help, "help" },
   { NULL, NULL}
};

struct applet* find_applet(const char* applet_name);

int main(int argc, char *argv[])
{    
    struct applet *cmd = find_applet(argv[1]);
    if (cmd != NULL) {
        // shift argument list left, now applet name is argv[0]
        return cmd->handler(argc - 1, ++argv);
    }
    command_help(argc, argv);
    return EXIT_FAILURE;
}

struct applet* find_applet(const char* applet_name) {
   struct applet *app = commands;
   if (applet_name == NULL) {
       return NULL;
   }   
   while (app != NULL && app->name != NULL) {
      if (strcmp(app->name, applet_name) == 0) {
         return app;
      }
      app ++;
   }
   return NULL;
}
