// gcc ../main.c -L<path_to_lib> -ltty -Wl,-rpath,<path_to_lib>

#include "stdio.h"
#include "tty/tty.h"
#include "unistd.h"

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
    cell p = set_point(frm, x,y);
    char me = frame_get(frm, p);
    if (me == EMPTY && neighbours == 3) {
        frame_set(frm, p, BORN);
    } else if (me == LIFE && (neighbours > 3 || neighbours < 2)) {
        frame_set(frm, p, DIED);
    }

    return 0;
}

int clear_troops(frame* frm, pixl_t x, pixl_t y) {
    cell p = set_point(frm, x, y);
    char place = frame_get(frm, p);
    if (place == DIED) {
        frame_set(frm, p, EMPTY);
    }
    else if (place == BORN) {
        frame_set(frm, p, LIFE);
    }
    return 0;
}

void cycle(frame* frm, pixl_t x, pixl_t y) {
    frame_set(frm, set_point(frm, x - 1, y), LIFE);
    frame_set(frm, set_point(frm, x, y), LIFE);
    frame_set(frm, set_point(frm, x + 1, y), LIFE);
}

void planner(frame* frm, pixl_t x, pixl_t y) {
    frame_set(frm, set_point(frm, x - 1, y), LIFE);
    frame_set(frm, set_point(frm, x + 1, y), LIFE);
    frame_set(frm, set_point(frm, x, y + 1), LIFE);
    frame_set(frm, set_point(frm, x + 1, y + 1), LIFE);
    frame_set(frm, set_point(frm, x + 1, y - 1), LIFE);
}

int main() {
    init();
    cls();
    frame f;
    init_frame(&f, x_end(), y_end());
    long i = 0;
    // cycle
    cycle(&f, 4, 4);
    planner(&f, 15, 15);
    cls();
    frame_draw(&f);
    while (1) {
        if (time_passed(0, 500)) {
            frame_set(&f, set_point(&f, 0, i+2), 'f');
            //frame_set(&f, set_point(&f, 0, i+2-1), '*');
            for (long x = 0; x < f.width; ++x) {
                for (long y = 0; y < f.height; ++y) {
                    check_neighbours(&f, x, y);
                }
            }
            for (pixl_t x = 0; x < f.width; ++x) {
                for (pixl_t y = 0; y < f.height; ++y) {
                    clear_troops(&f, x, y);
                }
            }
            //frame_set(&f, set_point(&f, 5, 5), 'D');
            //printf("%c\n\n\n", frame_get(&f, set_point(&f, 5, 5)));
            cls();
            frame_draw(&f);
            i++;
            // frame_flush(&f);
        }
    }
   while(1);
    
    fini_frame(&f);
    fini();
}
