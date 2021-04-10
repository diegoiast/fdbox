/* auto generated file */
/* not modify manually*/

/* general includes */
#include "fdbox.h"
#include "lib/applet.h"
#include <stddef.h>

/* applets includes */
#include "dos/beep.h"
#include "dos/call.h"
#include "dos/cd.h"
#include "dos/cls.h"
#include "dos/cmd_dir.h"
#include "dos/copy.h"
#include "dos/datetime.h"
#include "dos/del.h"
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
#include "dos/type.h"
#if 0
#include "unix/cal.h"
#include "unix/head.h"
#include "unix/hexdump.h"
#include "unix/tail.h"
#endif
#include "./help.h"

struct applet commands[] = {{help_beep, command_beep, "beep"},
                            {help_call, command_call, "call"},
                            {help_cd, command_cd, "cd"},
                            {help_cls, command_cls, "cls"},
                            {help_copy, command_copy, "copy"},
                            {help_date, command_date, "date"},
                            {help_del, command_del, "del"},
                            {help_dir, command_dir, "dir"},
                            {help_echo, command_echo, "echo"},
                            {help_exit, command_exit, "exit"},
                            {help_for, command_for, "for"},
                            {help_goto, command_goto, "goto"},
                            {help_if, command_if, "if"},
                            {help_mv, command_mv, "mv"},
                            {help_path, command_path, "path"},
                            {help_prompt, command_prompt, "prompt"},
                            {help_pwd, command_pwd, "pwd"},
                            {help_reboot, command_reboot, "reboot"},
                            {help_rem, command_rem, "rem"},
                            {help_set, command_set, "set"},
                            {help_shift, command_shift, "shift"},
                            {help_time, command_time, "time"},
#if 0
    { help_type,  command_type, "type" },
    { help_cal,  command_cal, "cal" },
    { help_head,  command_head, "head" },
    { help_hexdump,  command_hexdump, "hexdump" },
    { help_tail,  command_tail, "tail" },
#endif
                            {help_help, command_help, "help"},
                            {NULL, NULL, NULL}};
