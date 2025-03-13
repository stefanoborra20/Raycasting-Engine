#include <stdlib.h>
#include "main.h"

int main(int argc, char *argv[]) {
    
    if (!raycaster_init(argc, argv)) exit(EXIT_FAILURE);

    if (!raycaster_start()) exit(EXIT_FAILURE);

    raycaster_quit();
    exit(EXIT_SUCCESS);
}
