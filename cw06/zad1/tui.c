#define _XOPEN_SOURCE 500
#include "tui.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

static WINDOW *win_main, *win_chat, *win_input, *box1, *box2;

void draw_windows() {
  clear();

  int lines, cols;
  getmaxyx(stdscr, lines, cols);

  win_chat = subwin(win_main, lines - 5, cols - 4, 2, 2);
  scrollok(win_chat, TRUE);

  win_input = subwin(win_main, 1, cols - 8, lines - 2, 4);
  scrollok(win_input, TRUE);

  box1 = subwin(win_main, lines - 3, cols, 0, 0);
  box2 = subwin(win_main, 3, cols, lines - 3, 0);

  wattron(box1, A_DIM);
  box(box1, 0, 0);
  wattroff(box1, A_DIM);

  wattron(box2, A_DIM);
  box(box2, 0, 0);
  wattroff(box2, A_DIM);

  wattron(box1, COLOR_PAIR(PAIR_TITLE) | A_BOLD);
  mvwaddstr(box1, 0, 2, "CHAT");
  wattroff(box1, COLOR_PAIR(PAIR_TITLE) | A_BOLD);

  wattron(box2, COLOR_PAIR(PAIR_TITLE) | A_BOLD);
  mvwaddstr(box2, 0, 2, "INPUT");
  wattroff(box2, COLOR_PAIR(PAIR_TITLE) | A_BOLD);

  wattron(box2, COLOR_PAIR(PAIR_PROMPT) | A_BOLD);
  mvwaddstr(box2, 1, 2, "> ");
  wattroff(box2, COLOR_PAIR(PAIR_PROMPT) | A_BOLD);

  wrefresh(box1);
  wrefresh(box2);
}

static int last_length = 0;
static char* last_buffer = 0;

static int current_length = 0;

int input(char* buffer, int max_size) {
  int ch;

  switch (ch = getch()) {
    case 8:
    case 127:
    case KEY_LEFT: {
      if (current_length > 0) {
        buffer[--current_length] = '\0';
      }
      break;
    }
    case KEY_UP: {
      current_length = last_length;
      sprintf(buffer, "%s", last_buffer);
      break;
    }
    case KEY_RESIZE: {
      draw_windows();
      break;
    }
    case ERR: {
      break;
    }
    case '\n': {
      buffer[current_length] = '\0';
      int tmp = current_length;
      current_length = 0;

      wclear(win_input);
      wrefresh(win_input);

      last_buffer = strdup(buffer);
      last_length = tmp;

      return tmp;
      break;
    }
    default: {
      if (current_length < max_size - 1) {
        buffer[current_length++] = ch;
      }
    }
  }

  if (ch != ERR) {
    wclear(win_input);
    wmove(win_input, 0, 0);
    wattron(win_input, COLOR_PAIR(PAIR_DEFAULT) | A_BOLD);

    for (int i = 0; i < current_length; ++i) {
      waddch(win_input, buffer[i]);
    }

    wattroff(win_input, COLOR_PAIR(PAIR_DEFAULT) | A_BOLD);
    wrefresh(win_input);
  }

  return 0;
}

void terminal_start() {
  if ((win_main = initscr()) == NULL) {
    terminal_stop();
    err("unable to initialize ncurses window");
  }

  noecho();
  halfdelay(1);
  keypad(win_main, TRUE);

  if (start_color() == ERR || !has_colors()) {
    terminal_stop();
    err("unable to use colors");
  }

  use_default_colors();
  init_pair(PAIR_DEFAULT, -1, -1);
  init_pair(PAIR_TITLE, COLOR_CYAN, -1);
  init_pair(PAIR_PROMPT, COLOR_YELLOW, -1);
  init_pair(PAIR_INFO, COLOR_WHITE, -1);
  init_pair(PAIR_MESSAGE, COLOR_WHITE, -1);
  init_pair(PAIR_ERROR, COLOR_RED, -1);
  init_pair(PAIR_SUCCESS, COLOR_GREEN, -1);
}

void terminal_stop() {
  delwin(win_input);
  delwin(win_chat);
  delwin(box1);
  delwin(box2);
  delwin(win_main);
  endwin();
}

void tui_refresh() {
  wrefresh(win_chat);
  wcursyncup(win_input);
  wrefresh(win_input);
}

void add_message(int attr, const char* format, ...) {
  va_list args;
  va_start(args, format);
  wattron(win_chat, attr);
  vwprintw(win_chat, format, args);
  wattroff(win_chat, attr);
  va_end(args);
}
