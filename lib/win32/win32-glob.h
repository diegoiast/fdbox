/* libSoX minimal glob for MS-Windows: (c) 2009 SoX contributors
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef GLOB_H
#define GLOB_H 1

#define GLOB_FLAGS (GLOB_NOCHECK)

/* Bits set in the FLAGS argument to `glob' */

/* Posix */
#define	GLOB_ERR        (1 << 0)
#define	GLOB_MARK       (1 << 1)
#define	GLOB_NOSORT     (1 << 2)
#define	GLOB_DOOFFS     (1 << 3)
#define	GLOB_NOCHECK    (1 << 4)
#define	GLOB_APPEND     (1 << 5)
#define	GLOB_NOESCAPE   (1 << 6)
#define	GLOB_PERIOD     (1 << 7)

/* Posix2, GNU, BSD */
#define GLOB_MAGCHAR    (1 << 8)
#define GLOB_ALTDIRFUNC (1 << 9)
#define GLOB_BRACE      (1 << 10)
#define GLOB_NOMAGIC    (1 << 11)
#define GLOB_TILDE      (1 << 12)
#define GLOB_ONLYDIR    (1 << 13)
#define GLOB_TILDE_CHECK (1 << 14)

/* Error returns from `glob' */
#define	GLOB_NOSPACE    1
#define	GLOB_ABORTED    2
#define	GLOB_NOMATCH    3
#define GLOB_NOSYS      4

typedef struct glob_t
{
    unsigned gl_pathc;
    char **gl_pathv;
} glob_t;

#ifdef __cplusplus
extern "C" {
#endif

int
glob(
    const char *pattern,
    int flags,
    void *unused,
    glob_t *pglob);

void
globfree(
    glob_t* pglob);

#ifdef __cplusplus
}
#endif

#endif /* ifndef GLOB_H */
