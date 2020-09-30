#include <stdlib.h>
#include <unistd.h>
#include <windows.h>

#include "game_of_life.h"

#define HEIGHT 50
#define WIDTH 101

#define DEAD '.'
#define ALIVE 'x'

#define SLEEP_TIME 17 // ms

// ? - diamond
// 0 - glider
// 1 - glider gun
#define DRAW 1

void adjust_console() {
  HWND console = GetConsoleWindow();
  MoveWindow(console, 50, 50, 1440, 900, TRUE);
}

field new_field(size_t height, size_t width) {
  field f = (field)malloc(sizeof(struct field_s));
  f->width = width;
  f->height = height;
  f->curr_state = malloc(sizeof(char*) * height);
  f->next_state = malloc(sizeof(char*) * height);
  for (size_t y = 0; y < height; y++) {
    f->curr_state[y] = malloc(sizeof(char*) * width);
    f->next_state[y] = malloc(sizeof(char*) * width);
  }

  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      f->curr_state[y][x] = DEAD;
      f->next_state[y][x] = DEAD;
    }
    f->curr_state[y][width - 1] = '\0';
    f->next_state[y][width - 1] = '\0';
  }
  return f;
}

void free_field(field f) {
  for (size_t y = 0; y < f->height; y++) {
    free(f->curr_state[y]);
    free(f->next_state[y]);
  }
  free(f);
}

void draw_diamond(field f) {
  size_t y = 20;
  size_t x = 50;

  for (size_t i = 0; i < 12; i++) {
    if (i >= 4 && i < 8) {
      f->curr_state[y - 4][x + i] = ALIVE;
      f->curr_state[y + 4][x + i] = ALIVE;
    }
    if (i >= 2 && i < 10) {
      f->curr_state[y - 2][x + i] = ALIVE;
      f->curr_state[y + 2][x + i] = ALIVE;
    }
    f->curr_state[y][x + i] = ALIVE;
  }
}

void draw_glider(field f) {
  size_t y = 5;
  size_t x = 5;
  f->curr_state[y][x] = ALIVE;
  f->curr_state[y + 1][x + 1] = ALIVE;
  f->curr_state[y + 2][x + 1] = ALIVE;
  f->curr_state[y][x + 2] = ALIVE;
  f->curr_state[y + 1][x + 2] = ALIVE;
}

void draw_glider_gun(field f) {
  size_t y = 20;
  size_t x = 10;
  f->curr_state[y][x] = ALIVE;
  f->curr_state[y][x + 1] = ALIVE;
  f->curr_state[y + 1][x] = ALIVE;
  f->curr_state[y + 1][x + 1] = ALIVE;

  f->curr_state[y - 2][x + 13] = ALIVE;
  f->curr_state[y - 2][x + 12] = ALIVE;
  f->curr_state[y - 1][x + 11] = ALIVE;

  f->curr_state[y][x + 10] = ALIVE;
  f->curr_state[y + 1][x + 10] = ALIVE;
  f->curr_state[y + 2][x + 10] = ALIVE;
  
  f->curr_state[y + 3][x + 11] = ALIVE;
  f->curr_state[y + 4][x + 12] = ALIVE;
  f->curr_state[y + 4][x + 13] = ALIVE;
  
  f->curr_state[y - 1][x + 15] = ALIVE;
  f->curr_state[y + 1][x + 14] = ALIVE;
  f->curr_state[y + 3][x + 15] = ALIVE;

  f->curr_state[y][x + 16] = ALIVE;
  f->curr_state[y + 1][x + 16] = ALIVE;
  f->curr_state[y + 2][x + 16] = ALIVE;

  f->curr_state[y + 1][x + 17] = ALIVE;

  f->curr_state[y - 2][x + 20] = ALIVE;
  f->curr_state[y - 1][x + 20] = ALIVE;
  f->curr_state[y][x + 20] = ALIVE;
  f->curr_state[y - 2][x + 21] = ALIVE;
  f->curr_state[y - 1][x + 21] = ALIVE;
  f->curr_state[y][x + 21] = ALIVE;
  
  f->curr_state[y - 3][x + 22] = ALIVE;
  f->curr_state[y + 1][x + 22] = ALIVE;

  f->curr_state[y - 4][x + 24] = ALIVE;
  f->curr_state[y - 3][x + 24] = ALIVE;
  
  f->curr_state[y + 1][x + 24] = ALIVE;
  f->curr_state[y + 2][x + 24] = ALIVE;

  f->curr_state[y - 2][x + 35] = ALIVE;
  f->curr_state[y - 2][x + 34] = ALIVE;
  f->curr_state[y - 1][x + 34] = ALIVE;
  f->curr_state[y - 1][x + 35] = ALIVE;
}

size_t count_neighbours(field f, size_t i, size_t j) {
  size_t count = 0;
  size_t max_col = f->width - 1;
  for (size_t y = i - 1; y <= (i + 1); y++) {
    for (size_t x = j - 1; x <= (j + 1); x++) {
      if (x == j && y == i) continue;
      if ((x < max_col) && (y < f->height) && (x >= 0) && (y >= 0) ) {
        if (f->curr_state[y][x] == ALIVE) count++;
      }
    }
  }
  return count;
}

void apply_default_rules(field f) {
  size_t max_col = f->width - 1;
  for (size_t y = 0; y < f->height; y++) {
    for (size_t x = 0; x < max_col; x++) {
      size_t nghbr_cnt = count_neighbours(f, y, x);

      if (f->curr_state[y][x] == ALIVE) {
        if (nghbr_cnt == 2 || nghbr_cnt == 3) {
          f->next_state[y][x] = ALIVE;
        } else {
          f->next_state[y][x] = DEAD;  
        }
      } else {
        if (nghbr_cnt == 3) {
          f->next_state[y][x] = ALIVE;
        } else {
          f->next_state[y][x] = DEAD;
        }
      }
    }
  }
  for (size_t y = 0; y < f->height; y++) {
    for (size_t x = 0; x < max_col; x++) {
      f->curr_state[y][x] = f->next_state[y][x];
    }
  }
}

int main(int argc, char **argv) {
  adjust_console();

  field f = new_field(HEIGHT, WIDTH);

  switch (DRAW) {
    case 0: draw_glider(f); break;
    case 1: draw_glider_gun(f); break;
    default: draw_diamond(f); break;
  }

  while (1) {
    for (size_t y = 0; y < f->height; y++) {
      printf("%s\n", f->curr_state[y]);
    }
    puts("");
    Sleep(SLEEP_TIME);
    system("cls");
    apply_default_rules(f);
  }

  free_field(f);
  return 0;
}