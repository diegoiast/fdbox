#ifndef READLINE_H
#define READLINE_H

#ifdef __MSDOS__
#include "lib/tc202/stdbool.h"
#endif

#if defined(_POSIX_C_SOURCE) || defined(__APPLE__)
#include <stdbool.h>
#endif

#ifdef __WIN32__
#include <stdbool.h>
#endif

#include "lib/strextra.h"
#include <stddef.h>
#include <stdlib.h>

/* clang-format off */
#define KEY_ARROW_LEFT      0x7f4b
#define KEY_ARROW_RIGHT     0x7f4d
#define KEY_ARROW_UP        0x7f48
#define KEY_ARROW_DOWN      0x7f50
#define KEY_HOME            0x7f47
#define KEY_END             0x7f4f
#define KEY_PGDOWN          0x7f49
#define KEY_PGUP            0x7f51
#define KEY_BACKSPACE       '\b'
#define KEY_DEL             0x00ff
/* clang-format on */

/* moves the cursor to the left */
void move_cursor_back(size_t n);

/* read a single characrer, without enter */
int read_char();

/* old API - to be removed soon */
int read_string(char *line, size_t max_size);

/* clear screen */
void clear_screen();

struct readline_session {
        char *line;
        size_t max_size;
        size_t current_size;
        size_t current_history;
        size_t index;
        bool override;
        bool free_memory;
};

void readline_init();
void readline_deinit();

void readline_session_init(struct readline_session *session);
void readline_session_allocate(struct readline_session *session, size_t max_size);
void readline_session_deinit(struct readline_session *session);
int readline(struct readline_session *session);

size_t readline_delete_left(struct readline_session *session);
size_t readline_delete_right(struct readline_session *session);
size_t readline_replace(struct readline_session *session, size_t index, char c);
size_t readline_insert(struct readline_session *session, size_t index, char c);
size_t readline_set(struct readline_session *session, const char *new_text);
void readline_move_home(struct readline_session *session);
void readline_move_end(struct readline_session *session);
void readline_move_left(struct readline_session *session);
void readline_move_right(struct readline_session *session);

const char *readline_get_history(size_t i);

#endif
