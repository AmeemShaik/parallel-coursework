/* 
 * COMP633 - Programming Assignment 1(a)
 * Zach Cross (zcross@cs.unc.edu)
 * N-bodies simulation (sequential)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define G 1.0
#ifndef N
 #define N 256
#endif

struct body
{
    double m;      // Mass
    double r_x;    // X component of position
    double r_y;    // Y component of position
    double v_x;    // X component of velocity
    double v_y;    // Y component of velocity
};

struct body b[N];

double dist(int i, int j) {
    return sqrt((b[j].r_y - b[i].r_y)*(b[j].r_y - b[i].r_y) +
        (b[j].r_x - b[i].r_x)*(b[j].r_x - b[i].r_x));
};

double fx(int i, int j) {
    double d = dist(i, j) * dist(i, j) * dist(i, j);
    return (G * b[i].m * b[j].m * (b[j].r_x - b[i].r_x))/d;
};

double fy(int i, int j) {
    double d = dist(i, j) * dist(i, j) * dist(i, j);
    return (G * b[i].m * b[j].m * (b[j].r_y - b[i].r_y))/d;
};

double fx(int i) {
    double result = 0;
    for(int j = 0; k <= N; j++) {
        if (j == i) {
            continue;
        }
        result += fx(i, j);
    }
};

double fx(int i) {
    double result = 0;
    for(int j = 0; k <= N; j++) {
        if (j == i) {
            continue;
        }
        result += fy(i, j);
    }
};

double ax(int i) {
    return fx(i)/b[i].m;
}

double ay(int i) {
    return fy(i)/b[i].m;
}

int main() {

    return EXIT_SUCCESS;
};