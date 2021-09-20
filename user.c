// gcc ../main.c -L<path_to_lib> -ltty -Wl,-rpath,<path_to_lib>

#include "stdio.h"
#include "tty/tty.h"
#include "unistd.h"

int main() {
    init();
    cls();
    frame f;
    init_frame(&f, xmax(), ymax());
    long i = -1;
    while (1) {
        if (time_passed(0, 10)) {
            i++;
            cls();
            frame_set(&f, set_point(i,i % ymax()), 'o');
            frame_draw(&f);
            // frame_flush(&f);
        }
    }
    
    fini_frame(&f);
    fini();
}
