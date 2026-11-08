#ifndef __batch_h__
#define __batch_h__

/*
This file is part of fdbox
For license - read license.txt

Batch files (.BAT, and .BTM as in 4DOS): lines with %0-%9, %VAR% and %%,
labels, "@" and ECHO OFF, GOTO, SHIFT and CALL.
*/

#include <stdbool.h>
#include <stddef.h>

#define BATCH_MAX_ARGS 64

struct batch {
        char **lines;
        size_t count, pc;
        char *args[BATCH_MAX_ARGS];
        int argc, shift;
        bool done;
        struct batch *parent;
};

/* the batch file running, NULL at the prompt */
extern struct batch *batch_current;

/* runs it to its end; returns the error level of its last command */
int batch_run(const char *path, int argc, char *argv[]);

/* NAME, NAME.BTM or NAME.BAT in the current directory or in PATH */
bool batch_find(const char *name, char *path, size_t size);

int batch_goto(const char *label);
int batch_shift(void);

#endif /* __batch_h__ */
