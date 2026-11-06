#ifndef __for_h__
#define __for_h__

/*
This file is part of fdbox
For license - read license.txt
*/

int command_for(int arc, char *argv[]);
/* the whole line "FOR %V IN (set) DO command": the error level */
int for_line(const char *line);
const char *help_for();

#endif //__for_h__
