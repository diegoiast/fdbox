#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#include "dos/batch.h"
#include "dos/call.h"
#include "dos/command.h"
#include "lib/args.h"
#include "fdbox.h"
#include "lib/nls.h"

/*
 This file is part of fdbox
 For license - read license.txt
*/

int command_call(int argc, char *argv[]) {
        char path[260], line[1024];

        if (argc < 2)
                return EXIT_SUCCESS;
        if (batch_find(argv[1], path, sizeof(path)))
                return batch_run(path, argc - 1, argv + 1);
        if (!command_merge_args(argc - 1, (const char **)(argv + 1), line, sizeof(line)))
                return EXIT_FAILURE;
        command_execute_line(line);
        return errno;
}

const char *help_call() { return _("Here should be a basic help for call"); }
