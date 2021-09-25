#include "tty.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#include <time.h>

int time_passed(long seconds, long miliseconds) {
    static char reset = 1;
    static long sec, nsec;
    struct timespec t0;

    clock_gettime(CLOCK_MONOTONIC, &t0);

    if (reset) {
        sec = t0.tv_sec;
        nsec = t0.tv_nsec;
        reset = 0;
        return 1;
    }
    
    if (t0.tv_sec - sec > seconds || (seconds == 0 && t0.tv_sec - sec == 0 && t0.tv_nsec - nsec > miliseconds * 1000000)) {
        reset = 1;
    }

    return 0;
}



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
    //cls();
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

pixl_t x_end() {
    assert(win != NULL);
    return win->ws_col;
}

pixl_t y_end() {
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

ulong_t xy_to_offset(frame* frm, pixl_t x, pixl_t y) {
    assert(x < frm->width);
    assert(y < frm->height);

    return y * frm->width + y + x;
}

void init_frame(frame* frm, const pixl_t width, const pixl_t height) {
    assert(frm != NULL);
    ulong_t size = (width + 1) * height;
    frm->buf = (char*) calloc(size, sizeof(char));
    assert(frm->buf != NULL);

    frm->height = height;
    frm->width = width;
    frm->size = size;

    for (unsigned r = 0; r < size; ++r)
            frm->buf[r] = EMPTY;

    for (long r = 1; r < height; ++r) {
        frm->buf[r * width + r - 1] = '\n';
    }
    frm->buf[frm->size - 1] = '\0';
}

void fini_frame(frame* frm) {
    assert(frm != NULL);
    free(frm->buf);
}

int frame_set(frame* frm, cell point, char val) {
    assert(frm != NULL && frm->buf != NULL);
    frm->buf[xy_to_offset(frm, point.x, point.y)] = val;
    return 0;
}

int frame_unset(frame* frm, cell point) {
    assert(frm != NULL);
    return frame_set(frm, point, EMPTY);
}

char frame_get(frame* frm, cell point) {
    assert(frm != NULL);
    return frm->buf[xy_to_offset(frm, point.x, point.y)];
}

void frame_flush(frame* frm) {
    assert(frm != NULL && frm->buf != NULL);
    for (pixl_t r = 0; r < frm->width; ++r) {
        for (pixl_t c = 0; c < frm->height; ++c) {
            frm->buf[xy_to_offset(frm, r, c)] = EMPTY;
        }
    }
}

void frame_draw(frame* frm) {
    assert(frm != NULL && frm->buf != NULL);
    write(STDOUT_FILENO, frm->buf, frm->size);
}

cell set_point(frame* frm, long x, long y) {
    assert(frm != NULL);
    cell p = {0,0};

    if (x < 0) {
        p.x = -x % frm->width;
        if (p.x != 0)
            p.x = frm->width - p.x;
    }
    else
        p.x = x % frm->width;


    if (y < 0) {
        p.y = -y % frm->height;
        if (p.y != 0)
            p.y = frm->height - p.y;
    }
    else
        p.y = y % frm->height;

    
    return p;
}

