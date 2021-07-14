/*
This file is part of fdbox
For license - read license.txt
*/

#ifndef FDBOX_ENVIRON
#define FDBOX_ENVIRON

#include <stddef.h>

char *get_prompt(const char *prompt, char prompt_string[], size_t prompt_str_len);

void expand_string(const char *src, char *dest, size_t dest_length);
#endif // FDBOX_ENVIRON
