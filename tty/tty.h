#pragma once

#include <stdio.h>
#include "sys/types.h"

typedef struct termios term_t;
typedef struct winsize win_t;

typedef struct frame {
    char* buf;
    ushort width;
    ushort height;
    unsigned size;
} frame;

typedef struct cell {
    unsigned short x;
    unsigned short y;
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

void init_frame(frame* frm, const ushort width, const ushort height);
void fini_frame(frame* frm);
int frame_set(frame* frm, cell point, char val);
int frame_unset(frame* frm, cell point);
char frame_get(frame* frm, cell point);
void frame_flush(frame* frm);
void frame_draw(frame* frm);
cell set_point(ushort x, ushort y);



void cls();
void setxy(int x, int y, char c);
void setcolor(int c);
void draw(int x, int y, char c);
void hline(int x, int y, int len);
void vline(int x, int y, int len);
unsigned short xmax();
unsigned short ymax();