#ifndef TUI_H
#define TUI_H

#include <ncurses.h>

#define PAIR_DEFAULT 1
#define PAIR_TITLE 2
#define PAIR_PROMPT 3
#define PAIR_INFO 4
#define PAIR_MESSAGE 5
#define PAIR_ERROR 6
#define PAIR_SUCCESS 7

#define INFO(format, ...) \
  add_message(COLOR_PAIR(PAIR_INFO) | A_DIM, format, ##__VA_ARGS__);

#define SENT(format, ...) \
  add_message(COLOR_PAIR(PAIR_MESSAGE) | A_BOLD, format, ##__VA_ARGS__);

#define RECV(format, ...) \
  add_message(COLOR_PAIR(PAIR_MESSAGE), format, ##__VA_ARGS__);

#define ERROR(format, ...) \
  add_message(COLOR_PAIR(PAIR_ERROR), format, ##__VA_ARGS__);

#define SUCCESS(format, ...) \
  add_message(COLOR_PAIR(PAIR_SUCCESS) | A_BOLD, format, ##__VA_ARGS__);

void draw_windows();
int input(char* buffer, int max_size);

void terminal_start();
void terminal_stop();

void tui_refresh();

void add_message(int attr, const char* format, ...);

#endif
