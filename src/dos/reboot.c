#include <stdio.h>
#include <stdlib.h>

#include "dos/reboot.h"
#include "fdbox.h"
#include "lib/nls.h"

#if defined(__GONWARE__)
#include <gonware/syscall.h>
#endif

/*
This file is part of fdbox
For license - read license.txt

REBOOT and POWEROFF: on GonWare the kernel writes every volume back and
marks it clean before it restarts or switches off the machine.
*/

static int power(const char *name, int how) {
#if defined(__GONWARE__)
        fflush(stdout);
        gdos_call1(GDOS_POWER, how);
        fprintf(stderr, how == GDOS_POWER_REBOOT ? _("%s: the machine did not restart\n")
                                                 : _("%s: the machine did not switch off\n"),
                name);
#else
        (void)how;
        fprintf(stderr, _("%s: only on GonWare\n"), name);
#endif
        return EXIT_FAILURE;
}

int command_reboot(int argc, char *argv[]) {
        (void)argc;
        (void)argv;
#if defined(__GONWARE__)
        return power("REBOOT", GDOS_POWER_REBOOT);
#else
        return power("REBOOT", 1);
#endif
}

int command_poweroff(int argc, char *argv[]) {
        (void)argc;
        (void)argv;
#if defined(__GONWARE__)
        return power("POWEROFF", GDOS_POWER_OFF);
#else
        return power("POWEROFF", 0);
#endif
}

const char *help_reboot() { return _("Writes the volumes back and restarts the machine"); }

const char *help_poweroff() { return _("Writes the volumes back and switches the machine off"); }
