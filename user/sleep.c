// Paige Ogden's sleep function for users

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int ticks;

    if (argc != 2) {
        fprintf(2, "usage: sleep <ticks>\n");
        exit(1);
    }

    for (int i = 0; argv[1][i] != '\0'; i++) {
        if (argv[1][i] < '0' || argv[1][i] > '9') {
            fprintf(2, "usage: sleep <tickets>\n");
            exit(1);
        }
    } 

    ticks = atoi(argv[1]);

    if (pause(ticks) < 0) {
        fprintf(2, "usage: sleep <ticket>\n");
        exit(1);
    }

    exit(0);
}