/*
 * This file is part of fdbox.
 *
 * SerenityGlob is a multi platform port of the posix `glob()` api
 * to MSDOS, Windows, SerenityOS and Posix systems.
 *
 * Copyright 2022 Diego Iastrubni <diegoiast@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the copyright holder nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Implementation of a `glob()` for systems without glob.
 * First implementation is aimed at @SerenityOS. However,
 * porting this to Win32/Win64 or MSDOS should be trivial.
 *
 * Note that this code is not licensed under GPLv3 as the rest of
 * the code fdbox. This code is licensed more permissive on purpose.
 */

#ifndef GLOB_H
#define GLOB_H 1

#define GLOB_FLAGS (GLOB_NOCHECK)

/* Bits set in the FLAGS argument to `glob' */

/* Posix */
#define GLOB_ERR (1 << 0)
#define GLOB_MARK (1 << 1)
#define GLOB_NOSORT (1 << 2)
#define GLOB_DOOFFS (1 << 3)
#define GLOB_NOCHECK (1 << 4)
#define GLOB_APPEND (1 << 5)
#define GLOB_NOESCAPE (1 << 6)
#define GLOB_PERIOD (1 << 7)

/* Posix2, GNU, BSD */
#define GLOB_MAGCHAR (1 << 8)
#define GLOB_ALTDIRFUNC (1 << 9)
#define GLOB_BRACE (1 << 10)
#define GLOB_NOMAGIC (1 << 11)
#define GLOB_TILDE (1 << 12)
#define GLOB_ONLYDIR (1 << 13)
#define GLOB_TILDE_CHECK (1 << 14)

/* Error returns from `glob' */
#define GLOB_NOSPACE 1
#define GLOB_ABORTED 2
#define GLOB_NOMATCH 3
#define GLOB_NOSYS 4

/* under Windows, this is already defined */
#ifndef MAX_PATH
#define MAX_PATH 256
#endif

typedef struct glob_t {
        unsigned gl_pathc;
        char **gl_pathv;
} glob_t;

int serenity_glob(const char *pattern, int flags, void *unused, glob_t *pglob);
void serenity_globfree(glob_t *pglob);

#endif