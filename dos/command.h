#ifndef __command_h__
#define __command_h__

/*
This file is part of fdbox
For license - read license.txt
*/

int command_command(int arc, char *argv[]);
const char *help_command();

/* EXIT_FAILURE means exit was triggered */
int command_execute_line(char *line);

#endif //__command_h__
