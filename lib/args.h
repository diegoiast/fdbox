#ifndef __args_h__
#define __args_h__

// This file is part of fdbox
// For license - read license.txt

#define ARG_DONE         -1
#define ARG_STRING       0
#define ARG_NOT_EXISTING 1000
#define ARG_EXTRA        1001

int dos_parseargs(int *argc, char **argv[], const char* template, char **output);

#endif //__args_h__
