#include <stddef.h>

#define KEY_ARROW_LEFT      0x7f4b
#define KEY_ARROW_RIGHT     0x7f4d
#define KEY_ARROW_UP        0x7f48
#define KEY_ARROW_DOWN      0x7f50
#define KEY_HOME            0x7f47
#define KEY_END             0x7f4f
#define KEY_PGDOWN          0x7f49
#define KEY_PGUP            0x7f51

/* moves the cursor to the left */
void move_cursor_back(size_t n);

/* read a single characrer, without enter */
int read_char();
int read_string(char *line, size_t max_size);

void setup_terminal();
void restore_terminal();
