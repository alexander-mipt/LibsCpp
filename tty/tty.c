#include "tty.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>



void init() {
    init_win();
    init_tty();
    assert(tty != NULL && win != NULL);
}

void fini() {
    assert(tty != NULL && win != NULL);
    fini_win();
    fini_tty();
}

void init_tty() {
    tty = (term_t*) calloc(1, sizeof(term_t));
    assert(tty != NULL);

    tcgetattr(0, tty);

    tty->c_lflag &= ~ICANON;
    tty->c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, tty);

    signal(SIGINT, on_term);
    signal(SIGTERM, on_term);
    signal(SIGWINCH, on_winch);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void fini_tty() {
    assert(tty != NULL);
    tty->c_lflag |= ICANON;
    tty->c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, tty);
    free(tty);
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGWINCH, SIG_DFL);
    setvbuf(stdout, NULL, _IOLBF, 0);
    cls();
}

void init_win() {
    win = (win_t*) calloc(1, sizeof(win_t));
    assert(win != NULL);
    ioctl(STDIN_FILENO, TIOCGWINSZ, win);
}

void fini_win() {
    assert(win != NULL);
    free(win);
}

static void on_winch(int signo) {
    assert(signo == SIGWINCH);
    cls();
}

static void on_term(int signo) {
    assert(signo == SIGTERM || signo == SIGINT);
    fini_win();
    fini_tty();
    exit(0);
}

void cls() {
  printf("\e[H\e[J\e[m");
}

void setxy(int x, int y, char c) {
    printf("\e[%d;%dH%c", win->ws_row - y, x, c);
}

unsigned short xmax() {
    assert(win != NULL);
    return win->ws_col;
}

unsigned short ymax() {
    assert(win != NULL);
    return win->ws_row;
}

void setcolor(int c) {
    printf("\e[%dm", c ? c + 30 : 0);
}

void draw(int x, int y, char c) {
    printf("\e[%d;%dH%c", y, x, c);
}

void hline(int x, int y, int len) {
    for (int i = x; i < len; i++) {
        draw(i, y, 'S');
    }
}

void vline(int x, int y, int len) {
    for (int i = y; i < len; i++) {
        draw(x, i, 'S');
    }
}

void init_frame(frame* frm, const ushort width, const ushort height) {
    assert(frm != NULL);
    frm->buf = (char**) calloc(height, sizeof(char*));
    assert(frm->buf != NULL);
    for (ushort r = 0; r < height; ++r) {
        frm->buf[r] = (char*) calloc(width + 1, sizeof(char));
        assert(frm->buf[r] != NULL);
        if (r < height - 1)
            frm->buf[r][width] = '\n';
    }

    frm->height = height;
    frm->width = width;

    for (ushort r = 0; r < height; ++r)
        for (ushort c = 0; c < width; ++c) {
            frm->buf[r][c] = ' ';
        }
}

void fini_frame(frame* frm) {
    assert(frm != NULL);
    for (int r = 0; r < frm->height; ++r) {
        free(frm->buf[r]);
    }
    free(frm->buf);
}

int frame_set(frame* frm, cell point, char val) {
    assert(frm != NULL && frm->buf != NULL);
    point.x %= frm->width;
    point.y %= frm->height;
    frm->buf[point.y][point.x] = val;

    return 0;
}

int frame_unset(frame* frm, cell point) {
    return frame_set(frm, point, '\0');
}

char frame_get(frame* frm, cell point) {
    assert(frm != NULL);
    
    return frm->buf[point.y][point.x];
}

void frame_flush(frame* frm) {
    assert(frm != NULL && frm->buf != NULL);
    for (ushort r = 0; r < frm->height; ++r) {
        for (ushort c = 0; c < frm->width; ++c)
            frm->buf[r][c] = ' ';
    }
}

void frame_draw(frame* frm) {
    assert(frm != NULL && frm->buf != NULL);
    /*for (ushort r = 0; r < frm->height; ++r) {
        for (ushort c = 0; c <= frm->width; ++c) {
            fprintf(stderr, "%c", frm->buf[r][c]);
        }
    }*/
    for (ushort r = 0; r < frm->height; ++r)
        write(STDOUT_FILENO, frm->buf[r], (frm->width + 1));
}

cell set_point(ushort x, ushort y) {
    cell p;
    p.x = x;
    p.y = y;
    return p;
}

