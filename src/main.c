#include "dos/command.h"
#include "dos/ver.h"
#include "fdbox.h"
#include "help.h"
#include "lib/applet.h"
#include "lib/args.h"
#include "lib/readline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct applet commands[];

#if defined(__GONWARE__)
#include <errno.h>
#include <gonware/syscall.h>
#include <strings.h>
#include <sys/stat.h>

#include "dos/batch.h"

/* GCOM: "GCOM /C command", "GCOM FILE.BTM [args]", or the prompt after
 * \GONWARE\AUTOEXEC.BTM of the system volume */
static int gonware_main(int argc, char *argv[]) {
        char path[260];
        struct stat st;
        struct gdos_info info;

        if (argc >= 3 && strcasecmp(argv[1], "/c") == 0) {
                char line[1024];
                if (!command_merge_args(argc - 2, (const char **)(argv + 2), line, sizeof(line)))
                        return EXIT_FAILURE;
                readline_init();
                errno = 0;
                command_execute_line(line);
                readline_deinit();
                return errno;
        }
        if (argc >= 2 && batch_find(argv[1], path, sizeof(path))) {
                int level;
                readline_init();
                level = batch_run(path, argc - 1, argv + 1);
                readline_deinit();
                return level;
        }
        readline_init();
        atexit(readline_deinit);
        if (!gdos_call1(GDOS_INFO, &info) && info.system_drive) {
                snprintf(path, sizeof(path), "%c:\\GONWARE\\AUTOEXEC.BTM", (char)info.system_drive);
                if (!stat(path, &st)) {
                        char *args[] = {path, NULL};
                        batch_run(path, 1, args);
                }
        }
        return command_command(argc, argv);
}
#endif

int main(int argc, char *argv[]) {
#if defined(__GONWARE__)
        if (argc < 2 || argv[1][0] == '/' || !find_applet(CASE_INSENSITVE, argv[1], commands))
                return gonware_main(argc, argv);
#endif
        struct applet *cmd = find_applet(CASE_INSENSITVE, argv[1], commands);
        struct command_config config;

        if (cmd != NULL) {
                /* shift argument list left, now applet name is argv[0] */
                return cmd->handler(argc - 1, ++argv);
        }

        command_config_init(&config);
        command_config_parse(argc, argv, &config);

        if (config.show_help) {
                command_help(argc - 1, ++argv);
                return EXIT_SUCCESS;
        }

        /* OK - this is not verbose, but version */
        if (config.verbose) {
                return command_ver(argc - 1, ++argv);
        }

        readline_init();
        atexit(readline_deinit);
        return command_command(argc - 1, ++argv);
}
