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
#ifdef FDBOX_NLS
#include <locale.h>
#endif
#include "lib/nls.h"

extern struct applet commands[];

#if defined(__GONWARE__)
#include <errno.h>
#include <gonware/syscall.h>
#include <strings.h>
#include <sys/stat.h>

#include "dos/batch.h"

/* the volumes GDOS saw not cleanly unmounted: OFSCK /AUTO on each, the
 * error level of the worst */
static int gonware_check(void) {
        struct gdos_volume vol;
        int worst = 0;

        for (char d = 'C'; d <= 'Z'; d++) {
                char line[32];
                if (gdos_call2(GDOS_VOLUME, d, &vol) || !(vol.flags & GDOS_VOL_DIRTY) ||
                    (vol.flags & GDOS_VOL_RDONLY))
                        continue;
                printf(_("%c: was not cleanly unmounted, checking it\n"), d);
                snprintf(line, sizeof(line), "OFSCK /AUTO %c:", d);
                errno = 0;
                command_execute_line(line);
                if (errno > worst)
                        worst = errno;
                if (!gdos_call2(GDOS_VOLUME, d, &vol) && (vol.flags & GDOS_VOL_DIRTY))
                        printf(_("%c: stays read-only: run OFSCK /F %c: or ORECOVER\n"), d, d);
        }
        return worst;
}

/* GCOM and COMMAND: "/C command", "FILE.BTM [args]", the prompt. GDOS
 * starts the first shell with /P: it runs \GONWARE\AUTOEXEC.BTM of the
 * system volume first (COMMAND.EXE frees the boot archive before, the
 * start from disk is through then). "GCOM /CHECK" checks the volumes that
 * were not cleanly unmounted and ends with 0 when they are clean, else it
 * stays at the prompt. */
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
        if (argc >= 2 && strcasecmp(argv[1], "/check") == 0) {
                int level;
                readline_init();
                level = gonware_check();
                readline_deinit();
                if (level < 2)
                        return 0; /* clean, or repaired */
                fputs(_("The check found errors it could not repair. EXIT goes on with the start.\n"), stdout);
                readline_init();
                atexit(readline_deinit);
                return command_command(1, argv);
        }
        if (argc >= 2 && argv[1][0] != '/' && batch_find(argv[1], path, sizeof(path))) {
                int level;
                readline_init();
                level = batch_run(path, argc - 1, argv + 1);
                readline_deinit();
                return level;
        }
        readline_init();
        atexit(readline_deinit);
        if (argc >= 2 && strcasecmp(argv[1], "/p") == 0) {
#if defined(GONWARE_COMMAND)
                gdos_call0(GDOS_BOOT_DONE);
#endif
                if (!gdos_call1(GDOS_INFO, &info) && info.system_drive) {
                        snprintf(path, sizeof(path), "%c:\\GONWARE\\AUTOEXEC.BTM", (char)info.system_drive);
                        if (!stat(path, &st)) {
                                char *args[] = {path, NULL};
                                batch_run(path, 1, args);
                        }
                }
        }
        return command_command(1, argv);
}
#endif

int main(int argc, char *argv[]) {
#ifdef FDBOX_NLS
#if !defined(__GONWARE__)
        /* GonWare's libintl takes the language from the environment and
         * finds the catalogues built into the program */
        setlocale(LC_ALL, "");
#ifdef FDBOX_LOCALEDIR
        bindtextdomain("fdbox", FDBOX_LOCALEDIR);
#endif
#endif
        textdomain("fdbox");
#endif
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
