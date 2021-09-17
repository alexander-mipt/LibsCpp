// gcc ../main.c -L<path_to_lib> -ltty -Wl,-rpath,<path_to_lib>

#include "stdio.h"
#include "tty/tty.h"
#include "unistd.h"

int main() {
    init();
    cls();
    frame f;
    init_frame(&f, xmax(), ymax());
    for (int i = 0 ; i < 1000; ++i) {
        //sleep(1);
        frame_set(&f, set_point(i,i), 'o');
        frame_draw(&f);
        frame_flush(&f);
    }
    // draw(0, 0, 'x');
    // draw(xmax(), ymax(), 'y');
    // setxy(0, 0, 'z');
    // setxy(xmax(), ymax(), 'f');
    fini_frame(&f);

    while(1);

    fini();
}
