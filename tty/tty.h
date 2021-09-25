#pragma once

#include <stdio.h>
#include "sys/types.h"
#define EMPTY ' '

typedef struct termios term_t;
typedef struct winsize win_t;
typedef unsigned short pixl_t;
typedef unsigned long ulong_t;

typedef struct frame {
    char* buf;
    pixl_t width;
    pixl_t height;
    ulong_t size;
} frame;

typedef struct cell {
    pixl_t x;
    pixl_t y;
} cell;

static term_t* tty = NULL;
static win_t* win = NULL;

int time_passed(long seconds, long miliseconds);

void init();
void fini();
void init_tty();
void fini_tty();
void init_win();
void fini_win();

static void on_winch(int signo);
static void on_term(int signo);

void init_frame(frame* frm, const pixl_t width, const pixl_t height);
void fini_frame(frame* frm);
int frame_set(frame* frm, cell point, char val);
int frame_unset(frame* frm, cell point);
char frame_get(frame* frm, cell point);
void frame_flush(frame* frm);
void frame_draw(frame* frm);
ulong_t xy_to_offset(frame* frm, pixl_t x, pixl_t y);

cell set_point(frame* frm, long x, long y);



void cls();
void setxy(int x, int y, char c);
void setcolor(int c);
void draw(int x, int y, char c);
void hline(int x, int y, int len);
void vline(int x, int y, int len);
pixl_t x_end();
pixl_t y_end();