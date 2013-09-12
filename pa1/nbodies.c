/* 
 * COMP633 - Programming Assignment 1(a)
 * Zach Cross (zcross@cs.unc.edu)
 * N-bodies simulation (sequential)
 */

#include <stdio.h>
#include <stdlib.h>

struct body
{
    double m;      // Mass
    double r_x;    // X component of position
    double r_y;    // Y component of position
    double v_x;    // X component of velocity
    double v_y;    // Y component of velocity
};

int N = 256;

double f(i, j) {

};

int main(int argc, char **argv) {

    if (argc > 2) {
        printf("Error: expected zero or one arguments.");
        return(EXIT_FAILURE);
    } else if (argc == 2 && atoi(argv[1])) {
        N = atoi(argv[1]);
    }

    printf("Simulating %d bodies.\n", N);

    return EXIT_SUCCESS;
};