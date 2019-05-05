#include "error.h"
#include "message.h"
#include "types.h"
#include "utils.h"
#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

WINDOW *win_main, *win_chat, *win_input, *box1, *box2;

static int public_queue, private_queue;

void draw_windows()
{
    clear();

    int lines, cols;
    getmaxyx(stdscr, lines, cols);

    win_chat = subwin(win_main, lines * 0.8 - 2, cols - 2, 1, 1);
    win_input = subwin(win_main, lines * 0.2 - 2, cols - 2, lines * 0.8 + 2, 1);

    box1 = subwin(win_main, (lines * 0.8), cols, 0, 0);
    box2 = subwin(win_main, (lines * 0.2), cols, (lines * 0.8) + 1, 0);
    box(box1, 0, 0);
    box(box2, 0, 0);
    wrefresh(box1);
    wrefresh(box2);
}

int input(char* buffer, size_t max_size)
{
    int i = 0;
    int ch;
    wmove(win_input, 0, 0);
    wrefresh(win_input);

    while ((ch = getch()) != '\n') {
        if (ch == 8 || ch == 127 || ch == KEY_LEFT) {
            if (i > 0) {
                wprintw(win_input, "\b \b");
                buffer[--i] = '\0';
                wrefresh(win_input);
            }
        } else if (ch == KEY_RESIZE) {
            continue;
        } else if (ch != ERR) {
            if (i < max_size - 1) {
                buffer[i++] = ch;
                waddch(win_input, ch);
                wrefresh(win_input);
            }
        }
    }

    buffer[i] = '\0';
    wclear(win_input);
    wrefresh(win_input);

    return i;
}

void terminal_stop()
{
    delwin(win_input);
    delwin(win_chat);
    delwin(box1);
    delwin(box2);
    delwin(win_main);
    endwin();
}

void cleanup()
{
    terminal_stop();
    msgctl(private_queue, IPC_RMID, NULL);
}

void terminal_start()
{
    if ((win_main = initscr()) == NULL) {
        cleanup();
        err("unable to initialize ncurses window");
    }

    noecho();
    cbreak();
    keypad(win_main, TRUE);

    if (start_color() == ERR || !has_colors()) {
        cleanup();
        err("unable to use colors");
    }

    use_default_colors();
    init_pair(1, -1, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_RED, -1);
}

void handle_sigint(int sig)
{
    exit(0);
}

int main(int argc, char* argv[])
{
    if ((public_queue = msgget(get_public_key(), 0)) == -1) {
        perr("unable to open queue");
    }

    atexit(cleanup);
    signal(SIGINT, handle_sigint);

    key_t private_key = get_private_key();
    if ((private_queue = msgget(private_key, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        perr("unable to create queue");
    }

    terminal_start();
    draw_windows();

    message_t message;
    message.type = TYPE_INIT;
    sprintf(message.buffer, "%u", private_key);
    if ((msgsnd(public_queue, &message, MESSAGE_SIZE, 0) == -1)) {
        cleanup();
        err("unable to register");
    }

    while (1) {
        wrefresh(win_chat);
        wcursyncup(win_input);
        wrefresh(win_input);

        char buffer[MESSAGE_BUFFER_SIZE];
        int len = input(buffer, sizeof(buffer));

        message.type = 1;
        sprintf(message.buffer, "%s", buffer);

        if ((msgsnd(public_queue, &message, MESSAGE_SIZE, 0) == -1)) {
            cleanup();
            perr("unable to send message");
        }

        wprintw(win_chat, "message %d: %s\n", len, buffer);
    }

    cleanup();
    return 0;
}
