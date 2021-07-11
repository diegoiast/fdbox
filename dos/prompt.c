#include <stdio.h>
#include <stdlib.h>

#include "dos/prompt.h"
#include "fdbox.h"

#if defined(__MSDOS__) || defined(__WIN32__)
/* setenv is not supported under TC, implement this locally
 * using `putenv()`
 */
#include "lib/tc202/stdextra.h"
#endif

/*
This file is part of fdbox
For license - read license.txt
*/

int command_prompt(int argc, char *argv[]) {
        int r;
        const char *name = "PROMPT";
        const char *val;
        if (argc == 1) {
                val = "$P$G";
        } else {
                val = argv[1];
        }
        r = setenv(name, val, 1);
        return r;
}

const char *help_prompt() { return "Set the prompt"; }
