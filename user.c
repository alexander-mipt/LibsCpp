// gcc ../main.c -L<path_to_lib> -ltty -Wl,-rpath,<path_to_lib>

#include "stdio.h"
#include "unistd.h"
#include "string.h"

// my lib
#include "tty/tty.h"

#define LIFE 'G'
#define DIED 'X'
#define BORN 'B'

int check_neighbours(frame* frm, long x, long y) {
    int hx[8] = {-1, 0, 1, 1,  1,  0, -1, -1};
    int hy[8] = { 1, 1, 1, 0, -1, -1, -1,  0};
    int neighbours = 0;

    for (int i = 0; i < 8; ++i) {
        char someone = frame_get(frm, set_point(frm, x + hx[i], y + hy[i]));
        if (someone == LIFE || someone == DIED) {
            neighbours++;
        }
    }
    cell p = set_point(frm, x, y);
    char me = frame_get(frm, p);
    if (me == EMPTY && neighbours == 3) {
        frame_set(frm, p, BORN);
    } else if (me == LIFE && (neighbours > 3 || neighbours < 2)) {
        frame_set(frm, p, DIED);
    }

    return 0;
}

int clear_troops(frame* frm) {
    char* meta_state = memchr(frm->buf, DIED, frm->size);
    
    size_t offset = 0;
    while (meta_state != NULL) {
        *meta_state = EMPTY;
        offset = frm->buf - meta_state;
        meta_state = memchr(meta_state, DIED, frm->size - offset);
    }

    offset = 0;
    meta_state = memchr(frm->buf, BORN, frm->size);
    while (meta_state != NULL) {
        *meta_state = LIFE;
        offset = frm->buf - meta_state;
        meta_state = memchr(meta_state, BORN, frm->size - offset);
    }

    return 0;
}

void h_cycle(frame* frm, pixl_t x, pixl_t y) {
    frame_set(frm, set_point(frm, x - 1, y), LIFE);
    frame_set(frm, set_point(frm, x, y), LIFE);
    frame_set(frm, set_point(frm, x + 1, y), LIFE);
}

void v_cycle(frame* frm, pixl_t x, pixl_t y) {
    frame_set(frm, set_point(frm, x, y - 1), LIFE);
    frame_set(frm, set_point(frm, x, y), LIFE);
    frame_set(frm, set_point(frm, x, y + 1), LIFE);
}

void planner(frame* frm, pixl_t x, pixl_t y) {
    frame_set(frm, set_point(frm, x - 1, y), LIFE);
    frame_set(frm, set_point(frm, x + 1, y), LIFE);
    frame_set(frm, set_point(frm, x, y + 1), LIFE);
    frame_set(frm, set_point(frm, x + 1, y + 1), LIFE);
    frame_set(frm, set_point(frm, x + 1, y - 1), LIFE);
}

int main() {
    // init gamefield
    init();
    cls();
    frame f;
    init_frame(&f, x_end(), y_end());
    long i = 0;

    // init figures
    h_cycle(&f, 4, 4);
    v_cycle(&f, x_end() - 4, y_end() - 1);
    planner(&f, 15, 15);
    
    // draw
    frame_draw(&f);

    // game cycles
    while (1) {
        //getchar();
        if (time_passed(0, 50)) {
            for (long x = 0; x < f.width; ++x) {
                for (long y = 0; y < f.height; ++y) {
                    check_neighbours(&f, x, y);
                }
            }
            clear_troops(&f);

            cls();
            frame_draw(&f);
            i++;
            // frame_flush(&f);
        }
    }
    
    fini_frame(&f);
    fini();
}
