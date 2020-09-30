#ifndef __GOL_H__
#define __GOL_H__

#include <stdio.h>

typedef struct field_s {
  size_t width;
  size_t height;
  char **curr_state;
  char **next_state;
} field_t, *field;

field new_field(size_t rows, size_t cols);
void free_field(field f);

#endif /* __GOL_H__ */
