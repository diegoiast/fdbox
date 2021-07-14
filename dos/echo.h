#ifndef __echo_h__
#define __echo_h__

/*
This file is part of fdbox
For license - read license.txt
*/

#include <stdlib.h>

#ifdef _POSIX_C_SOURCE
#include <stdbool.h>
#endif

#ifdef __WIN32__
#include <stdbool.h>
#endif

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#endif

int command_echo(int arc, char *argv[]);
const char *help_echo();

extern bool echo_is_on;

#endif //__echo_h__
