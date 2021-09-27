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

int main(int argc, char *argv[]) {
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
        
        setup_terminal();
        atexit(restore_terminal);
        return command_command(argc - 1, ++argv);
}
